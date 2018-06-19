#ifndef DPSCONNECTIVITYUTIL_H_
#define DPSCONNECTIVITYUTIL_H_

#include "CommonUtil.hpp"

/***************************************************************************
 *                    dpsConnectivity test class 
 ***************************************************************************
 */
class DpsConnectivityTest
{
public:

/*
 *----------------------------------------------------------------------*
 *         test parameters container                                    *
 *----------------------------------------------------------------------*
 */
  struct DpsConnectivityTestParam
  {
    /*original input parameters*/
    STRING dpsPins;

    /*new generated parameter for convenience*/
    TM::DCTEST_MODE testMode;           /*expanded and validated pins stored in Vector*/
    STRING_VECTOR expandedDpsPins;      /*current test mode*/
    STRING testsuiteName;               /*current testsuite name*/

    /*initialize stuff to some defaults.*/ 
    void init()
    {
      dpsPins = "";
      testMode = TM::GPF;
      expandedDpsPins.clear();
    } 

    /*default constructor for intializing parameters*/
    DpsConnectivityTestParam()
    {
      init();
    }             
  };

  /*
   *----------------------------------------------------------------------*
   *         test limit container                                    *
   *----------------------------------------------------------------------*
   */
  struct DpsConnectivityTestLimit
  {
    LIMIT limit;
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
    DpsConnectivityTestLimit()
    {
      init();
    }
  };

  /*
  *----------------------------------------------------------------------*
  *         test results container                                       *
  *----------------------------------------------------------------------*
  */
  struct DpsConnectivityTestResult
  {
    /*result container per Site
    *for current value, the unit is: A
    */
    MeasurementResultContainer dpsResult;

    /*initialize all stuffs to some defaults.*/
    void init()
    {
      dpsResult.init();
    }

    /*default constructor*/
    DpsConnectivityTestResult()
    {
      init();
    }
  };

/*
 *----------------------------------------------------------------------*
 *         public interfaces of DpsConnectivity test                          *
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
                                DpsConnectivityTestParam& param)
  {   
    /*Init param*/  
    param.init();
    param.dpsPins = dpsPins;
    param.expandedDpsPins = PinUtility.getDpsPinNamesFromPinList(dpsPins,TRUE);
    /*get the test mode and test suite name*/
    param.testMode = CommonUtil::getMode();
    GET_TESTSUITE_NAME(param.testsuiteName);   
  }

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
  static void processLimit(const string& testname,DpsConnectivityTestLimit& testLimit)
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
  static void doMeasurement(const DpsConnectivityTestParam& param,  
                            DpsConnectivityTestResult& result)
  {

    ON_FIRST_INVOCATION_BEGIN();
      /*Init result*/  
      result.init();
      DISCONNECT();
      executeAndGetResult(param.expandedDpsPins, param.testMode, result.dpsResult);
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
  static void judgeAndDatalog(const DpsConnectivityTestParam& param, 
                              const DpsConnectivityTestResult& result,
                              DpsConnectivityTestLimit& testLimit, 
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
        ARRAY_D measuredValueArray(param.expandedDpsPins.size());
        for ( STRING_VECTOR::size_type j = 0; j < param.expandedDpsPins.size(); ++j )
        {
          bool isOnePass = result.dpsResult.getPinPassFail( param.expandedDpsPins[j],nSiteNumber );
          measuredValueArray[j] = isOnePass?MPR_LOG_PF_TEST_PASS:MPR_LOG_PF_TEST_FAIL;
          isPass = isPass && isOnePass;
        } 
        //Special MPR for PVAL/PPF test      
        if (testLimit.isLimitTableUsed)
        {     
          V93kLimits::TMLimits::LimitInfo& limitInfo = V93kLimits::tmLimits.getLimit(
            param.testsuiteName,testLimit.testname); 
          TESTSET().cont(TM::CONTINUE).testnumber(limitInfo.TestNumber)
                 .judgeAndLog_ParametricTest(param.expandedDpsPins,
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
                                           param.expandedDpsPins,
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
      throw Error("DpsConnectivityTest::judgeAndDatalog",
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
  static void reportToUI(const DpsConnectivityTestParam& param, 
                         const DpsConnectivityTestResult& result, 
                         const STRING& output)
  {
    CommonUtil::printTestSuiteName(output, "DPS connectivity '", "'");
    
    STRING allPins ;
    STRING_VECTOR::const_iterator it;
    Boolean isPass = TRUE;

    INT nSiteNumber = CURRENT_SITE_NUMBER();

    switch ( param.testMode )
    {
    case TM::PVAL:  /* same as PPF */
    case TM::PPF:
      for ( it = param.expandedDpsPins.begin(); it != param.expandedDpsPins.end(); ++it )
      {
        isPass = result.dpsResult.getPinPassFail( *it,nSiteNumber );
        CommonUtil::datalogToWindow( output, *it, isPass );
      }
      break;

    case TM::GPF:
      isPass = result.dpsResult.getGlobalPassFail(nSiteNumber);
      CommonUtil::datalogToWindow(output,param.testsuiteName, isPass);
      break;

    default:
      throw Error("DpsConnectivityTest:: reportToUI",
                  "Unknown Test Mode!");
      break;
    }/*end switch*/
  }
  
private:
  DpsConnectivityTest() {}//private constructor to prevent instantiation.
  
  static void executeAndGetResult(
                 const STRING_VECTOR&    dpsPins,
                 const TM::DCTEST_MODE&  mode,
                 MeasurementResultContainer& dpsResult);
                 
  static void retrieveResult(
                 const STRING&           fwAnswer,
                 const TM::DCTEST_MODE&  mode,
                 const STRING_VECTOR&    dpsPins,
                 MeasurementResultContainer& dpsResult);
                 
  static void retrieveResultForPPF(
                 const STRING&           fwAnswer,
                 const STRING_VECTOR&    dpsPins,
                 MeasurementResultContainer& dpsResult);
};

/*
 *----------------------------------------------------------------------*
 * Routine: DpsConnectivityTest::executeAndGetResult
 *
 * Purpose: send firmwarm command and check
 *       DPS connectivity 
 *      (Force and Sense Connectivity Check)
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note: 
 *   1. assume firmware command works fine.
 *   2. must be called inside ON_FIRST_INVOCATION_BEGIN/END block,
 *      because FOR_EACH_SITE_BEGIN/END block is used below.
 *----------------------------------------------------------------------*
 */
inline void DpsConnectivityTest::executeAndGetResult(
                                   const STRING_VECTOR&    dpsPins,
                                   const TM::DCTEST_MODE&  mode,
                                   MeasurementResultContainer& dpsResult)
{

  STRING fwQuery = "";
  STRING fwAnswer = "";
  STRING dpsPinsString = PinUtility.createPinListFromPinNames(dpsPins);
  CHAR stringOfSiteNumber[5] = "\0"; 

  switch ( mode )
  {
  case TM::PPF:
  case TM::PVAL: /* consider PVAL as PPF for 'PDMM?' */
    FOR_EACH_SITE_BEGIN();
      fwQuery += "NOOP?,,,\n";/*distinguish every site for firmware paser.*/
      snprintf(stringOfSiteNumber,5,"%4d", CURRENT_SITE_NUMBER());
      /* set focus site */
      fwQuery += "PQFC ";
      fwQuery += stringOfSiteNumber ;
      fwQuery += "\n";
      /* create fw command for checking dps status */
      fwQuery += "PDMM? FO_SE, (" + dpsPinsString + ")\n"; 
    FOR_EACH_SITE_END();
    break;
  case TM::GPF:
    fwQuery += "PDMM? FO_SE, (" + dpsPinsString + ")\nPRLT? ALL\n";
    break;
  default:
    throw Error("DpsConnectivityTest::executeAndGetResult",
                "Unknown execute mode.");
    break; 
  }

  /* send query command and get answer */
  FW_TASK( fwQuery, fwAnswer );

  /* get and record results */
  retrieveResult(fwAnswer,mode,dpsPins,dpsResult);
}

/*
 *----------------------------------------------------------------------*
 * Routine: DpsConnectivityTest::retrieveResult
 *
 * Purpose: retrieve and record the result of Dps Connectivity. 
 *               
 *----------------------------------------------------------------------*
 * Description:  
 *
 * Note: 
 *   assume firmware command works fine.
 *       
 *----------------------------------------------------------------------*
 */
inline void DpsConnectivityTest::retrieveResult(
                                   const STRING&           fwAnswer,
                                   const TM::DCTEST_MODE&  mode,
                                   const STRING_VECTOR&    dpsPins,
                                   MeasurementResultContainer& dpsResult)
{
  switch ( mode )
  {
  case TM::PPF:
  case TM::PVAL: 
    retrieveResultForPPF(fwAnswer,dpsPins,dpsResult);
    break;
  case TM::GPF:
    CommonUtil::parseAndRecordPRLTResult(fwAnswer,dpsResult);
    break;
  default:
    throw Error("DpsConnectivityTest::retrieveResult",
                "Unknown execute mode.");
    break; 
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: DpsConnectivityTest::retrieveResultForPPF
 *
 * Purpose: retrieve and record the result of Dps Connectivity for PPF
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
inline void DpsConnectivityTest::retrieveResultForPPF(
                                   const STRING&           fwAnswer,
                                   const STRING_VECTOR&    dpsPins,
                                   MeasurementResultContainer& dpsResult)
{
/*
 ***********************************
 * To analyze the query result:
 * examples of the answer string :
 * PDMM FO_SE,,POS,P,(Vcc)
 * PDMM FO_SE,,NEG,P,(Vcc)
 * PDMM FO_SE,,POS,F,(Vee)
 * PDMM FO_SE,,NEG,P,(Vee)
 ***********************************
 */

  STRING::size_type i = 0;
  STRING::size_type position = 0;

  /* retrieve and record results for each site */
  FOR_EACH_SITE_BEGIN();
    INT nSiteNumber = CURRENT_SITE_NUMBER();
    STRING resToken = "";
    Boolean isPass = TRUE;       /* define for per pin result */
    Boolean bGlobalResult = TRUE; /* define for global result */
  
    position = fwAnswer.find("NOOP",position);
      
    for ( STRING_VECTOR::const_iterator it = dpsPins.begin(); 
        it != dpsPins.end(); ++it )
    {
      /* get POS&NEG results for each pin */
      i = position;
      isPass = TRUE; 
      for ( INT j = 0;  j < 2;  ++j )
      {
        i = fwAnswer.find( *it, i+5 );
        if ( i == string::npos )
        {  /*exceptional case: no response for this dps pin*/
          throw Error( "DpsConnectivityTest::retrieveResultForPPF",
                       "Unexpected result from FW answer." );
        }
        resToken = fwAnswer.substr( i-3,1 );
        /*
         * To get Pass or Fail:
         * apply 'relational and' to positive and negative results
         * for the total result of the specified pin.
         */
        isPass = isPass && 
                 ( ((resToken == "P") || (resToken == "p"))?TRUE:FALSE );
      }        
      /*update GPF result*/
      if ( (!isPass) )
      {
        bGlobalResult = FALSE; 
      }
      dpsResult.setPinPassFail( *it,isPass,nSiteNumber);
    }/*end of for-loop*/
  
    position = i;
    /*set global result for the site*/
    dpsResult.setGlobalPassFail(bGlobalResult,nSiteNumber);
  FOR_EACH_SITE_END();
}

#endif /*DPSCONNECTIVITYTEST_H_*/
