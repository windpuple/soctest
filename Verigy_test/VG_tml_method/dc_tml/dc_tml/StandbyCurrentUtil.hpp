#ifndef STANDBYCURRENTUTIL_H_
#define STANDBYCURRENTUTIL_H_

#include "CommonUtil.hpp"
#include "DpsUtil.hpp"

/***************************************************************************
 *                    standbyCurrent test class 
 ***************************************************************************
 */  
class StandbyCurrentTest
{
public:

/*
 *----------------------------------------------------------------------*
 *         test parameters container                                    *
 *----------------------------------------------------------------------*
 */ 
  struct StandbyCurrentTestParam
  {
    /* original input parameters*/
    STRING dpsPins;
    INT samples;
    double settlingTime_ms;
    STRING termination;

    /* new generated parameter for convenience*/
    
    TM::DCTEST_MODE testMode;       /*Per pin value, Per pin pass/fail or global pass/fail*/
    STRING testsuiteName;           /*current testsuite name*/
    STRING_VECTOR expandedDpsPins;  /*expanded and validated pins stored in Vector*/

    /*initialize stuff to some defaults.*/
    void init()
    {
      dpsPins = "";
      samples = 0;
      settlingTime_ms = 0.0;
      
      expandedDpsPins.clear();
      testMode = TM::GPF;
    }

    /*default constructor for intializing parameters*/
    StandbyCurrentTestParam()
    {
      init();
    }
  };
  
  /*
   *----------------------------------------------------------------------*
   *         test limit container                                         *
   *----------------------------------------------------------------------*
   */ 
  struct StandbyCurrentTestLimit
  {
    LIMIT limit;                    /*The LIMIT object of this test.*/
    string testname;
    bool isLimitTableUsed; 
    bool use_uAAsDefaultUnit; 
    /*initialize limit to default.*/
    void init()
    {
      limit = TM::Fail;  
      testname.clear();
      isLimitTableUsed = true;
      use_uAAsDefaultUnit = false;
    }
    
    /*default constructor */
    StandbyCurrentTestLimit()
    {
      init();
    }
  };


/*
 *----------------------------------------------------------------------*
 *         test results container                                       *
 *----------------------------------------------------------------------*
 */
  struct StandbyCurrentTestResult
  {

    /*result container 
     *for value, the unit is: A
     */
    MeasurementResultContainer dpsResult;

    /*initialize all stuffs to some defaults.*/
    void init()
    {
      dpsResult.init();
    }

    /*default constructor*/
    StandbyCurrentTestResult()
    {
      init();
    }
  };


/*
 *----------------------------------------------------------------------*
 *         public interfaces of standby current test                    *
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
                                const DOUBLE settlingTime_ms,
                                const STRING& termination,
                                StandbyCurrentTestParam& param)
  {
    /*initial param*/
    param.init();
    /*validate pinlist and expand it*/
    param.dpsPins = CommonUtil::trim(dpsPins);
    param.expandedDpsPins = PinUtility.getDpsPinNamesFromPinList(dpsPins,TRUE);

    param.samples = samples;
    param.settlingTime_ms = settlingTime_ms;
    param.termination = CommonUtil::trim(termination);  
     
    /*get test suite name and test mode*/
    GET_TESTSUITE_NAME(param.testsuiteName);
    param.testMode = CommonUtil::getMode();
  }
  
  static void processLimit(const string& testname, const bool testnameHasDefaultUnit,
    StandbyCurrentTestLimit& testLimit)
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
    if (testnameHasDefaultUnit && testLimit.limit.unit().empty())
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
  static void doMeasurement(const StandbyCurrentTestParam& param, 
                            const StandbyCurrentTestLimit& testLimit,
                            StandbyCurrentTestResult& result)
  {

    DPS_TASK dpsTask;
    STRING strTermPins;

    ON_FIRST_INVOCATION_BEGIN();
    
      /*CONNECT: firstly try to connect*/
      CONNECT();
  
      if ( param.termination == TURN_OFF )
      {
        strTermPins = CommonUtil::getTerminatedPins();
        CommonUtil::disconnectPins(strTermPins);
      }
  
      DpsUtil::currentMeasurement(param.dpsPins,
                                  testLimit.limit,
                                  param.testMode,
                                  dpsTask,
                                  param.samples,
                                  param.settlingTime_ms);
  
      if ( param.termination == TURN_OFF )
      {
        CommonUtil::connectPins(strTermPins);
      }

    ON_FIRST_INVOCATION_END() ;


    /*EXECUTE & RESULT HANDLING*/ 
    /*init GPF for this site*/
    result.dpsResult.setGlobalPassFail(true); 
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
 *   judge results of 'result' with pass limit from 'testLimit'
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
  static void judgeAndDatalog(const StandbyCurrentTestParam& param, 
                              const StandbyCurrentTestLimit& testLimit, 
                              const StandbyCurrentTestResult& result, 
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
    double factor = 1;
    if (testLimit.use_uAAsDefaultUnit)
    {
      factor = 1e6;
    }
    switch ( param.testMode )
    {
    case TM::PVAL: /*pass/fail value measurement*/
      {
        for (j = 0; j < param.expandedDpsPins.size(); ++j)
        {
          double dVal = result.dpsResult.getPinsValue(param.expandedDpsPins[j]); // [A]
          measuredValueArray[j] = factor * dVal;
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

    case TM::PPF: /*pass/fail per pin*/

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

    case TM::GPF: /*global pass fail*/
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
      throw Error("StandbyCurrentTest::judgeAndDatalog",
                  "Unknown Test Mode");
    }/*end -switch*/
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
 *       b) pass range from pass limit of testLimit,
 *       c) pass or fail
 * 
 *   INPUT:  param              - test parameters
 *           testLimit          - test limit container
 *           result             - result container
 *           output             - "None" or "ReportUI" 
 *   OUTPUT: 
 *   RETURN:  
 * Note:
 *----------------------------------------------------------------------*
 */ 
  static void reportToUI(const StandbyCurrentTestParam& param, 
                         const StandbyCurrentTestLimit& testLimit, 
                         const StandbyCurrentTestResult& result, 
                         const STRING& output)
  {
    CommonUtil::printTestSuiteName(output, "standby current '", "'");

    Boolean isPass;
    STRING_VECTOR::const_iterator it;

    switch ( param.testMode )
    {
    case TM::PVAL: /*pass/fail value measurement*/
      {
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
      }
      break;

    case TM::PPF: /*pass/fail per pin*/

      for ( it=param.expandedDpsPins.begin(); it!=param.expandedDpsPins.end(); ++it )
      {
        isPass = result.dpsResult.getPinPassFail((*it));
        CommonUtil::datalogToWindow(output, *it, isPass);
      }
      break;

    case TM::GPF: /*global pass fail*/
      isPass = result.dpsResult.getGlobalPassFail();

      CommonUtil::datalogToWindow(output,param.testsuiteName, isPass);
      break;

    default:
      throw Error("StandbyCurrentTest::reportToUI",
                  "Unknown Test Mode");
    }/*end -switch*/
  }

private:
  StandbyCurrentTest() {}; //private constructor to prevent instantiation.
};
#endif /*STANDBYCURRENT_H_*/
