#ifndef DPSSTATUSUTIL_H_
#define DPSSTATUSUTIL_H_

#include "CommonUtil.hpp"
#include "DpsUtil.hpp"

/***************************************************************************
 *                    dpsStatus test class 
 ***************************************************************************
 */
class DpsStatusTest
{
public:

/*
 *----------------------------------------------------------------------*
 *         test parameters container                                    *
 *----------------------------------------------------------------------*
 */
  struct DpsStatusTestParam
  {
    /*original input parameters*/
    STRING dpsPins;
    STRING constantCurrent;
    STRING unregulated;
    STRING overVoltage;
    STRING overPowerTemp; 

    /*new generated parameter for convenience*/
    STRING_VECTOR expandedPins;       /*expanded and validated pins stored in Vector*/
    TM::DCTEST_MODE testMode;         /*current test mode*/
    STRING testsuiteName;             /*current testsuite name*/

    /*initialize stuff to some defaults.*/ 
    void init()
    {
      dpsPins = "";
      expandedPins.clear();
      testMode = TM::GPF;
    } 

    /*default constructor for intializing parameters*/
    DpsStatusTestParam()
    {
      init();
    }     
  };

  /*
   *----------------------------------------------------------------------*
   *         test limit container                                    *
   *----------------------------------------------------------------------*
   */
  struct DpsStatusTestLimit
  {
    LIMIT limit;                      /*LIMIT object for this test*/
    string testname;
    bool isLimitTableUsed; 
    /*init limit*/
    void init()
    {
      limit = TM::Fail;
      testname.clear();
      isLimitTableUsed = true;
    }
    
    /*default constructor */
    DpsStatusTestLimit()
    {
      init();
    }
  };

  /*
   *----------------------------------------------------------------------*
   *         test results container                                       *
   *----------------------------------------------------------------------*
   */
  struct DpsStatusTestResult
  {
    /*result container per Site
    *for voltage value, the unit is: A
    */
    MeasurementResultContainer dpsResult;

    /*initialize all stuffs to some defaults.*/
    void init()
    {
      dpsResult.init();
    }

    /*default constructor*/
    DpsStatusTestResult()
    {
      init();
    }
  };

/*
 *----------------------------------------------------------------------*
 *         public interfaces of DpsStatus dc test                          *
 *----------------------------------------------------------------------*
 */

/*
 *----------------------------------------------------------------------*
 * Routine: processParameter
 *
 * Purpose: parse input parameters and setup internal parameters
 *
 *----------------------------------------------------------------------*
 * Description:
 *   
 * Note:
 *----------------------------------------------------------------------*
 */
  static void processParameters(const STRING& dpsPins,
                                const STRING& constantCurrent,
                                const STRING& unregulated,
                                const STRING& overVoltage,
                                const STRING& overPowerTemp,
                                DpsStatusTestParam& param)
  {
    /*Init param*/
    param.init();
    param.dpsPins = CommonUtil::trim(dpsPins);
    param.expandedPins = PinUtility.getDpsPinNamesFromPinList(dpsPins,TRUE);
    param.constantCurrent = CommonUtil::trim(constantCurrent);
    param.unregulated = CommonUtil::trim(unregulated);
    param.overVoltage = CommonUtil::trim(overVoltage);
    param.overPowerTemp = CommonUtil::trim(overPowerTemp); 
    /*get the test mode and test suite name*/
    param.testMode = CommonUtil::getMode();
    GET_TESTSUITE_NAME(param.testsuiteName);
  };

/*
 *----------------------------------------------------------------------*
 * Routine: processLimit
 *
 * Purpose: process limit
 *
 *----------------------------------------------------------------------*
 * Description:
 *   
 * Note:
 *----------------------------------------------------------------------*
 */
  static void processLimit(const string& testname,DpsStatusTestLimit& testLimit)
  {   
    /* init testLimit*/
    testLimit.init();
    testLimit.testname = CommonUtil::trim(testname);

    //if limit CSV file is load, get limit from it;otherwise get limit from testflow
    string testsuiteName;
    GET_TESTSUITE_NAME(testsuiteName);
    TesttableLimitHelper ttlHelper(testsuiteName);
    if (ttlHelper.isLimitCsvFileLoad())
    {
      ttlHelper.getLimit(testLimit.testname, testLimit.limit);
    }
      
    testLimit.isLimitTableUsed = ttlHelper.isAllInTable();

    if (!testLimit.isLimitTableUsed)
    {
      testLimit.limit = GET_LIMIT_OBJECT(testLimit.testname);
    }
  }

/*
 *----------------------------------------------------------------------*
 * Routine: doMeasurement
 *
 * Purpose: execute measurement by DPS and store results
 *
 *----------------------------------------------------------------------*
 * Description:
 *   INPUT:  param       - test parameters
 *           result      - result container
 *
 *   OUTPUT: 
 *   RETURN: 
 *----------------------------------------------------------------------*
 */
  static void doMeasurement(const DpsStatusTestParam& param, 
                            DpsStatusTestResult& result)
  {
    ON_FIRST_INVOCATION_BEGIN();
      /*Init result*/
      result.init();
      CONNECT();
      executeAndGetResult(param.constantCurrent,
                          param.unregulated,
                          param.overVoltage,
                          param.overPowerTemp,
                          param.expandedPins,
                          param.testMode,
                          result.dpsResult);
    ON_FIRST_INVOCATION_END();



  }

/*
 *----------------------------------------------------------------------*
 * Routine: judgeAndDatalog
 *
 * Purpose: judge and put result into event datalog stream.
 *
 *----------------------------------------------------------------------*
 * Description:
 *   judge results of 'result' with pass limits from 'param'
 * 
 *   INPUT:  param       - test parameters
 *           result      - result container
 *           _results[4] - retrieved by built-in tf_result() in testflow.
 *   OUTPUT: 
 *   RETURN: 
 * Note:
 *----------------------------------------------------------------------*
 */
  static void judgeAndDatalog(const DpsStatusTestParam& param, 
                              const DpsStatusTestResult& result,
                              DpsStatusTestLimit& testLimit,
                              double _results[4])
  {
    Boolean isPass = TRUE;

    INT nSiteNumber = CURRENT_SITE_NUMBER();
    /*init _results*/
    _results[0]=_results[2]= DBL_MAX; 
    _results[1]=_results[3]= -DBL_MAX;

    switch ( param.testMode )
    {
    case TM::PVAL:  /* same as PPF */
    case TM::PPF:
      {
        ARRAY_D measuredValueArray(param.expandedPins.size());
        for ( STRING_VECTOR::size_type j = 0; j < param.expandedPins.size(); ++j )
        {
          bool isOnePass = result.dpsResult.getPinPassFail( param.expandedPins[j],nSiteNumber );
          measuredValueArray[j] = isOnePass?MPR_LOG_PF_TEST_PASS:MPR_LOG_PF_TEST_FAIL;
          isPass = isPass && isOnePass;
        } 
        //Special MPR for PVAL/PPF test      
        if (testLimit.isLimitTableUsed)
        {     
          V93kLimits::TMLimits::LimitInfo& limitInfo = V93kLimits::tmLimits.getLimit(
            param.testsuiteName,testLimit.testname); 
          TESTSET().cont(TM::CONTINUE).testnumber(limitInfo.TestNumber)
                 .judgeAndLog_ParametricTest(param.expandedPins,
                                           testLimit.testname,
                                           isPass?TM::Pass : TM::Fail,
                                           measuredValueArray);
          if (!isPass && !limitInfo.BinsNumString.empty())
          {
            SET_MULTIBIN(limitInfo.BinsNumString,limitInfo.BinhNum);
          }
        }
        else
        {
           TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                           param.expandedPins,
                                           testLimit.testname,
                                           isPass?TM::Pass : TM::Fail,
                                           measuredValueArray);
 
        }
      }
      break;

    case TM::GPF:
      isPass = result.dpsResult.getGlobalPassFail(nSiteNumber);
      if (testLimit.isLimitTableUsed)
      {
        V93kLimits::TMLimits::LimitInfo& limitInfo = V93kLimits::tmLimits.getLimit(
          param.testsuiteName,testLimit.testname);
        TESTSET().cont(TM::CONTINUE).testnumber(limitInfo.TestNumber)
               .judgeAndLog_ParametricTest(param.dpsPins,
                                           testLimit.testname,
                                           isPass?TM::Pass : TM::Fail,
                                           0);
        if (!isPass && !limitInfo.BinsNumString.empty())
        {
          SET_MULTIBIN(limitInfo.BinsNumString,limitInfo.BinhNum);
        }
      }
      else
      {
         TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                           param.dpsPins,
                                           testLimit.testname,
                                           isPass?TM::Pass : TM::Fail,
                                           0);

      }
      break;

    default:
      throw Error("DpsStatusTest::judgeAndDatalog",
                  "Unknown Test Mode!");
      break;
    }/*end switch*/
  }

/*
 *----------------------------------------------------------------------*
 * Routine: reportToUI
 *
 * Purpose: output result to UIWindow 
 *
 *----------------------------------------------------------------------*
 * Description:
 *   display: 
 *       a) results from result,
 *       b) pass range from pass limits of param,
 *       c) pass or fail
 * 
 *   INPUT:  param              - test parameters
 *           output             - "None" or "ReportUI" 
 *           result             - result container
 *   OUTPUT: 
 *   RETURN:  
 * Note:
 *----------------------------------------------------------------------*
 */ 
  static void reportToUI(const DpsStatusTestParam& param, 
                         const DpsStatusTestResult& result, 
                         const STRING& output)
  {
    CommonUtil::printTestSuiteName(output, "DPS status '", "'");

    STRING allPins ;
    STRING_VECTOR::const_iterator it;
    Boolean isPass = TRUE;

    INT nSiteNumber = CURRENT_SITE_NUMBER();

    switch ( param.testMode )
    {
    case TM::PVAL:  /* same as PPF */
    case TM::PPF:
      for ( it = param.expandedPins.begin(); it != param.expandedPins.end(); ++it )
      {
        isPass = result.dpsResult.getPinPassFail(*it, nSiteNumber );
        CommonUtil::datalogToWindow( output, *it, isPass );
      }
      break;

    case TM::GPF:
      allPins = PinUtility.createPinListFromPinNames(param.expandedPins);
      isPass = result.dpsResult.getGlobalPassFail(nSiteNumber);
      CommonUtil::datalogToWindow(output,param.testsuiteName, isPass);
      break;

    default:
      throw Error("DpsStatusTest::reportToUI",
                  "Unknown Test Mode!");
      break;
    }/*end switch*/
  }
  
private:
  DpsStatusTest() {} //private constructor to prevent instantiation.
  
  static void executeAndGetResult(
                 const STRING&           constantCurrent,
                 const STRING&           unregulated,
                 const STRING&           overVoltage,
                 const STRING&           overPowerTemp,
                 const STRING_VECTOR&    dpsPins,
                 const TM::DCTEST_MODE&  mode,
                 MeasurementResultContainer& dpsResult);
                            
  static void retrieveResult(
                 const STRING&           fwAnswer,
                 const TM::DCTEST_MODE&  mode,
                 const INT               sizeOfDpsPins,
                 MeasurementResultContainer& dpsResult);
                                       
  static void retrieveResultForPPF(
                 const STRING&           fwAnswer,
                 const INT               sizeOfDpsPins,
                 MeasurementResultContainer& dpsResult);
};
/*
 *----------------------------------------------------------------------*
 * Routine: DpsStatusTest::executeAndGetResult
 *
 * Purpose: send firmware command to check the 
 *               DPS pins status.
 *----------------------------------------------------------------------*
 * Description:  
 *
 * Note: 
 *   1. assume firmware command works fine.
 *   2. must be called inside ON_FIRST_INVOCATION_BEGIN/END block,
 *      because FOR_EACH_SITE_BEGIN/END block is used below.     
 *----------------------------------------------------------------------*
 */
inline void DpsStatusTest::executeAndGetResult(
                                   const STRING&           constantCurrent,
                                   const STRING&           unregulated,
                                   const STRING&           overVoltage,
                                   const STRING&           overPowerTemp,
                                   const STRING_VECTOR&    dpsPins,
                                   const TM::DCTEST_MODE&  mode,
                                   MeasurementResultContainer& dpsResult)
{


  /*check the state option*/
  INT commandCodeOfCheckDpsStatus = 0;

  if ( constantCurrent == TURN_ON )
  {
    commandCodeOfCheckDpsStatus |= DpsUtil::CONSTANT_CURRENT;
  }

  if ( unregulated == TURN_ON )
  {
    commandCodeOfCheckDpsStatus |= DpsUtil::UNREGULATED;
  }

  if ( overVoltage == TURN_ON )
  {
    commandCodeOfCheckDpsStatus |= DpsUtil::OVER_VOLTAGE;
  }

  if ( overPowerTemp == TURN_ON )
  {
    commandCodeOfCheckDpsStatus |= DpsUtil::OVER_POWER_TEMP;
  }

  CHAR stringOfCommandCode[3] = "\0";
  snprintf(stringOfCommandCode,3,"%2d",commandCodeOfCheckDpsStatus);

  STRING fwQuery = "";
  STRING fwAnswer = "";
  CHAR stringOfSiteNumber[5] = "\0";  
  STRING dpsPinsString = PinUtility.createPinListFromPinNames(dpsPins);
  switch ( mode )
  {
  case TM::PPF:
  case TM::PVAL: /* consider PVAL as PPF for 'PSAS?' */
    FOR_EACH_SITE_BEGIN();
      snprintf(stringOfSiteNumber,5,"%4d", CURRENT_SITE_NUMBER());
      /* set focus site */
      fwQuery += "PQFC ";
      fwQuery += stringOfSiteNumber ;
      fwQuery += "\n";
      /* create fw command for checking dps status */
      fwQuery += "PSAS? ";      
      fwQuery += stringOfCommandCode;
      fwQuery += ",(" + dpsPinsString + ")\n";
    FOR_EACH_SITE_END();
    break;
  case TM::GPF:
    fwQuery += "PSAS? ";      
    fwQuery += stringOfCommandCode;
    fwQuery += ",(" + dpsPinsString + ")\nPRLT? ALL\n";
    break;
  default:
    throw Error("DpsStatusTest::executeAndGetResult",
                "Unknown execute mode.");
    break; 
  }

  /* send query command and get answer */
  FW_TASK( fwQuery, fwAnswer );

  /* get and record results */
  retrieveResult(fwAnswer,mode,dpsPins.size(),dpsResult);
}

/*
 *----------------------------------------------------------------------*
 * Routine: DpsStatusTest::retrieveResult
 *
 * Purpose: retrieve and record the result of Dps Status. 
 *               
 *----------------------------------------------------------------------*
 * Description:  
 *
 * Note: 
 *   assume firmware command works fine.
 *       
 *----------------------------------------------------------------------*
 */
inline void DpsStatusTest::retrieveResult(
                                   const STRING&           fwAnswer,
                                   const TM::DCTEST_MODE&  mode,
                                   const INT               sizeOfDpsPins,
                                   MeasurementResultContainer& dpsResult)
{
  switch ( mode )
  {
  case TM::PPF:
  case TM::PVAL: 
    retrieveResultForPPF(fwAnswer,sizeOfDpsPins,dpsResult);
    break;
  case TM::GPF:
    CommonUtil::parseAndRecordPRLTResult(fwAnswer,dpsResult);
    break;
  default:
    throw Error("DpsStatusTest::retrieveResult",
                "Unknown execute mode.");
    break; 
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: DpsStatusTest::retrieveResultForPPF
 *
 * Purpose: retrieve and record the result of Dps Status for PPF mode.
 *               
 *----------------------------------------------------------------------*
 * Description:  
 *
 * Note: 
 *   1. assume firmware command works fine.
 *   2. must be called inside ON_FIRST_INVOCATION_BEGIN/END block,
 *      because FOR_EACH_SITE_BEGIN/END block is used below.     
 *----------------------------------------------------------------------*
 */
inline void DpsStatusTest::retrieveResultForPPF(
                                   const STRING&           fwAnswer,
                                   const INT               sizeOfDpsPins,
                                   MeasurementResultContainer& dpsResult)
{
/*
 *************************************
 * To analyze the query result.
 * examples of the answer string :
 * (1) PSAS F,(Vcc,Vee,Vdd)\n
 * (2) PSAS P,(Vcc)\nPSAS? F,(Vee)\n
 *************************************
 */

  STRING::size_type i = 0;
  STRING::size_type j = 0;
  i = fwAnswer.find( "PSAS",i );

  /* retrieve and record results for each site */
  FOR_EACH_SITE_BEGIN();    
    STRING resToken = "";
    INT siteNumber = CURRENT_SITE_NUMBER();
    Boolean isPass = TRUE;       /* define for per pin result */
    Boolean bGlobalResult = TRUE; /* define for global result */
  
    /*use it to stop parsing result for each site */
    INT numberOfPins = sizeOfDpsPins;
  
    while ( (i != string::npos) && (numberOfPins > 0) )
    {
      /*retrieve the result of checking states*/
      i = fwAnswer.find( ',',i+5 );
      resToken = fwAnswer[i-1];
  
      /*get pass/fail */
      isPass = ( (resToken == "P") || (resToken == "p") )?TRUE:FALSE;
  
      /*update GPF result*/
      if ( (!isPass) )
      {
        bGlobalResult = FALSE; 
      }
  
      i = fwAnswer.find( '(',i+1 );
      if (string::npos == i)
      {
        break;
      }
      ++i; /*point to the starting letter following the '(' */
      j = fwAnswer.find( ')',i+1 );
      if (string::npos == j)
      {
        break;
      }
  
      /*retrieve the pin name*/
      resToken = fwAnswer.substr( i,j-i );
  
      /*Parse this substring and record the results*/
      STRING_VECTOR arrayOfPinNames;
      arrayOfPinNames.clear();
      CommonUtil::splitStr( resToken,',',arrayOfPinNames );
      for ( STRING_VECTOR::const_iterator it = arrayOfPinNames.begin();
          it != arrayOfPinNames.end(); ++it )
      {
        dpsResult.setPinPassFail( *it,isPass,siteNumber );
        --numberOfPins;
      }
      /*try to get the next line starting with 'PSAS' */
      i = fwAnswer.find( "PSAS",j+1 );
    }
    /* set global result for the site */
    dpsResult.setGlobalPassFail( bGlobalResult,siteNumber );

  FOR_EACH_SITE_END();

}
#endif /*DPSSTATUS_H_*/
