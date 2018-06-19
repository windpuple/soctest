#ifndef OPERATINGCURRENTUTIL_H_
#define OPERATINGCURRENTUTIL_H_

#include "CommonUtil.hpp"
#include "FunctionalUtil.hpp"
#include "DpsUtil.hpp"

/***************************************************************************
 *                    operatingCurrent test class 
 ***************************************************************************
 */  
class OperatingCurrentTest
{
public:

/*
 *----------------------------------------------------------------------*
 *         test parameters container                                    *
 *----------------------------------------------------------------------*
 */ 
  struct OperatingCurrentTestParam
  {
    /*original input parameters*/
    STRING dpsPins;
    INT samples;
    DOUBLE delayTime;
    STRING termination;

    /*new generated parameter for convenience*/
    
    TM::DCTEST_MODE testMode;               /*current test mode*/ 
    STRING_VECTOR expandedDpsPins;          /*expanded and validated Dps pins stored in Vector*/
    STRING testsuiteName;                    /*current testsuite name*/

    /*initialize stuff to some defaults.*/  
    void init()
    {
      dpsPins = "";
      samples = 4; /*recommended default value*/
      delayTime = 0.0;
     
      testMode = TM::GPF;
      expandedDpsPins.clear();
    }

    /*default constructor for intializing parameters*/
    OperatingCurrentTestParam()
    {
      init();
    }
  };
  
  /*
   *----------------------------------------------------------------------*
   *         test limit container                                         *
   *----------------------------------------------------------------------*
   */ 
  struct OperatingCurrentTestLimit
  {
    LIMIT limit;                            /*LIMIT object for this test*/
    string testname;
    bool isLimitTableUsed; 
    bool use_uAAsDefaultUnit;

    /*initialize limit to default.*/ 
    void init()
    {
      limit = TM::Fail;
      testname.clear();
      isLimitTableUsed = true;
      use_uAAsDefaultUnit = true;
    }
    
    /*default constructor */
    OperatingCurrentTestLimit()
    {
      init();
    }    
  };
  

  /*
 *----------------------------------------------------------------------*
 *         test results container                                       *
 *----------------------------------------------------------------------*
 */
  struct OperatingCurrentTestResult
  {
    /*result container per site:
    *for value, the unit is: A
    */
    MeasurementResultContainer dpsResult;

    /*initialize all stuffs to some defaults.*/
    void init()
    {
      dpsResult.init();
    }

    /*default constructor*/
    OperatingCurrentTestResult()
    {
      init();
    }
  };

/*
 *----------------------------------------------------------------------*
 *         public interfaces of OperatingCurrent  test                          *
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
                                const INT samples,
                                const DOUBLE delayTime, 
                                const STRING& termination,
                                OperatingCurrentTestParam& param)
  {
    param.dpsPins = dpsPins;
    param.samples = samples;
    param.delayTime = delayTime;
    param.termination = CommonUtil::trim(termination);
    param.expandedDpsPins = PinUtility.getDpsPinNamesFromPinList(dpsPins,TRUE);
    GET_TESTSUITE_NAME(param.testsuiteName);
    param.testMode = CommonUtil::getMode();
  }
  
  /*
   *----------------------------------------------------------------------*
   * Routine: processLimit
   *
   * Purpose: process Limit
   *
   *----------------------------------------------------------------------*
   * Description:
   *   
   * Note:
   *----------------------------------------------------------------------*
   */
  static void processLimit(const string& testname, 
    const bool testnameHasUnit, OperatingCurrentTestLimit& testLimit)
  {    
    /* init testLimit*/
    testLimit.init();
    testLimit.testname = CommonUtil::trim(testname);

    //if limit CSV file is load, get limit from it;otherwise get limit from testflow.
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
    //set default unit as "uA" 
    if (testnameHasUnit && testLimit.limit.unit().empty())
    {
      testLimit.use_uAAsDefaultUnit = true;
      testLimit.limit.unit("uA");
    }
    else
    {
      testLimit.use_uAAsDefaultUnit = false;
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
 *           testLimit   - test limit container
 *           result      - result container
 *
 *   OUTPUT: 
 *   RETURN: 
 *----------------------------------------------------------------------*
 */      
  static void doMeasurement(const OperatingCurrentTestParam& param, 
                            const OperatingCurrentTestLimit& testLimit, 
                            OperatingCurrentTestResult & result)
  {
    DPS_TASK dpsTask;
    STRING strTermPins;

    ON_FIRST_INVOCATION_BEGIN();
       
      CONNECT();
  
      if ( param.termination == TURN_OFF )
      {
        strTermPins = CommonUtil::getTerminatedPins();
        CommonUtil::disconnectPins(strTermPins);
      }
  
      FunctionalUtil::runSequencer(TM::ENDLESS_PATTERN_LOOP);
  
      DpsUtil::currentMeasurement(param.dpsPins,
                                  testLimit.limit,
                                  param.testMode,
                                  dpsTask,
                                  param.samples,
                                  param.delayTime);
  
      if ( !FunctionalUtil::isSequencerRunning() )
      {
        throw Error("OperatingCurrentTest::doMeasurement()",
                    "The Sequencer is stopped when measuring!");
      }
  
      FunctionalUtil::abortSequencer(); 
  
      if ( param.termination == TURN_OFF )
      {
        CommonUtil::connectPins(strTermPins);
      }

    ON_FIRST_INVOCATION_END();

    DpsUtil::currentGetResult(param.expandedDpsPins,
                              param.testMode,
                              dpsTask,
                              result.dpsResult);
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
  *           testLimit   - test limit container
  *           result      - result container
  *           _results[4] - retrieved by built-in tf_result() in testflow.
  *   OUTPUT: 
  *   RETURN: 
  * Note:
  *----------------------------------------------------------------------*
  */ 
  static void judgeAndDatalog(const OperatingCurrentTestParam& param, 
                              const OperatingCurrentTestLimit& testLimit, 
                              const OperatingCurrentTestResult& result,
                              double _results[4])
  {

    Boolean isPass = true;
    STRING_VECTOR::size_type j = 0;
    /*init _results*/
    _results[0]=_results[2]= DBL_MAX; 
    _results[1]=_results[3]= -DBL_MAX;

    pair<DOUBLE,DOUBLE> minmax;
    minmax.first  =  DBL_MAX;   //min val
    minmax.second = -DBL_MAX;   //max val
    ARRAY_D measuredValueArray(param.expandedDpsPins.size());

    switch ( param.testMode )
    {
    case TM::PVAL:
      {
        double factor_ToDefaultUnit = 1;
        if (testLimit.use_uAAsDefaultUnit)
        {
          factor_ToDefaultUnit = 1e6;
        }

        for (j = 0; j < param.expandedDpsPins.size(); ++j)
        {
          double dVal = result.dpsResult.getPinsValue(param.expandedDpsPins[j]);
          measuredValueArray[j] = factor_ToDefaultUnit * dVal;

          minmax.first = MIN(minmax.first,dVal);  //min value
          minmax.second = MAX(minmax.second,dVal);//max value
        }
        //MPR record
        if (testLimit.isLimitTableUsed)
        {
          TestSet.cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                           param.expandedDpsPins,
                                           testLimit.testname,
                                           V93kLimits::tmLimits,
                                           measuredValueArray);
        }
        else
        {
          TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                           param.expandedDpsPins,
                                           testLimit.testname,
                                           measuredValueArray);
 
        } 
        _results[0] = minmax.first; // min current [A]
        _results[1] = minmax.second;// max current [A]
      }
      break;

    case TM::PPF:

      for (j = 0; j < param.expandedDpsPins.size(); ++j)
      {
        bool isOnePass = result.dpsResult.getPinPassFail(param.expandedDpsPins[j]);
        measuredValueArray[j] = isOnePass?MPR_LOG_PF_TEST_PASS:MPR_LOG_PF_TEST_FAIL;
        isPass = isPass && isOnePass;
      }
      //Special MPR for PPF test      
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
      break;

    case TM::GPF:
      isPass = result.dpsResult.getGlobalPassFail();
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
      throw Error("OperatingCurrentTest::judgeAndDatalog",
                  "Unknown Test Mode");
    }
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
  *           testLimit          - test limit container
  *           result             - result container
  *   OUTPUT: 
  *   RETURN:  
  * Note:
  *----------------------------------------------------------------------*
  */ 
  static void reportToUI(const OperatingCurrentTestParam& param, 
                         const OperatingCurrentTestLimit& testLimit, 
                         const OperatingCurrentTestResult& result, 
                         const STRING& output)
  {
    CommonUtil::printTestSuiteName(output, "operating current '", "'");

    Boolean isPass;
    STRING_VECTOR::const_iterator it;

    switch ( param.testMode )
    {
    case TM::PVAL:
        for ( it = param.expandedDpsPins.begin(); it != param.expandedDpsPins.end(); ++it )
        {
          double dVal = result.dpsResult.getPinsValue((*it));
          CommonUtil::datalogToWindow(output,
                                      *it, 
                                      "A",
                                      dVal, 
                                      testLimit.use_uAAsDefaultUnit?"uA":"",
                                      testLimit.limit);
        }
      break;

    case TM::PPF:

      for ( it=param.expandedDpsPins.begin(); it!=param.expandedDpsPins.end(); ++it )
      {
        isPass = result.dpsResult.getPinPassFail((*it));
        CommonUtil::datalogToWindow(output, *it, isPass);
      }
      break;

    case TM::GPF:
      isPass = result.dpsResult.getGlobalPassFail();
      CommonUtil::datalogToWindow(output,param.testsuiteName, isPass);
      break;

    default:
      throw Error("OperatingCurrentTest::CurrentReportUI",
                  "Unknown Test Mode");
    }/*end -switch*/
  }
private:
  OperatingCurrentTest() {} //private constructor to prevent instantiation.
};

#endif /*OPERATINGCURRENTTEST_H_*/

