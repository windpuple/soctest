#ifndef HIGHZUTIL_H_
#define HIGHZUTIL_H_

#include "CommonUtil.hpp"
#include "FunctionalUtil.hpp"
#include "PmuUtil.hpp"

/***************************************************************************
 *                    highZ test class 
 ***************************************************************************
 */  
class HighZTest
{
public:

/*
 *----------------------------------------------------------------------*
 *         test parameters container                                    *
 *----------------------------------------------------------------------*
 */ 
  struct HighZTestParam
  {
    /*original input parameters*/
    STRING pinlist;
    DOUBLE forceVoltage;
    DOUBLE settlingTime;
    STRING relaySwitchMode;

    /*new generated parameter for convenience*/
    STRING testsuiteName;         /*current testsuite name*/
    STRING_VECTOR expandedPins;   /*expanded and validated pins stored in Vector*/   
    TM::DCTEST_MODE testMode;     /*current test mode*/
    STRING strPinList;            /*STRING of O and IO pins for pmuCurrentMeasurement */

    /*initialize stuff to some defaults.*/ 
    void init()
    {
      pinlist = "";
      settlingTime = 0.0;
      strPinList = "";
      forceVoltage = 0;
      relaySwitchMode = "Unknown";
      expandedPins.clear();
      testMode = TM::GPF;
    } 

    /*default constructor for intializing parameters*/
    HighZTestParam()
    {
      init();
    }
  };
  
  /*
   *----------------------------------------------------------------------*
   *         test limit(s) container                                    *
   *----------------------------------------------------------------------*
   */ 
  struct HighZTestLimit
  {
     LIMIT limit;                  /*LIMIT object for this test*/
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
     HighZTestLimit()
     {
      init();
     }
    
  };


  /*
 *----------------------------------------------------------------------*
 *         test results container                                       *
 *----------------------------------------------------------------------*
 */
  struct HighZTestResult
  {
    /*result container :
    *for value, the unit is: A
    */
    MeasurementResultContainer measureResult;
    Boolean funcResult;     /*true: PASS, false: FAIL*/ 

    /*initialize all stuffs to some defaults.*/
    void init()
    {
      measureResult.init();
      funcResult = true;
    }

    /*default constructor*/
    HighZTestResult()
    {
      init( );
    }
  };


/*
 *----------------------------------------------------------------------*
 *         public interfaces of highZ test                          *
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
  static void processParameters(const STRING& pinlist, 
                                const DOUBLE forceVoltage,
                                const DOUBLE settlingTime, 
                                const STRING& relaySwitchMode,
                                HighZTestParam& param)
  {   
    /*initial param*/
    param.init();
    /* 
     * To expand O and IO pins
     * since PMU_VFIM can validate pinlist internally,
     * here ommit this kind of checking.
     */
    param.pinlist = CommonUtil::trim(pinlist);
    param.expandedPins = PinUtility.expandDigitalPinNamesFromPinList(
                                                                    param.pinlist, 
                                                                    TM::O_PIN|TM::IO_PIN);
    /* create STRING of O and IO pins for pmuCurrentMeasurement */
    param.strPinList = PinUtility.createPinListFromPinNames(param.expandedPins);

    

    /* get the test suite name and test mode*/
    GET_TESTSUITE_NAME(param.testsuiteName);
    param.testMode = CommonUtil::getMode(); 
    param.forceVoltage = forceVoltage;
    param.relaySwitchMode = CommonUtil::trim(relaySwitchMode);
     /*check the range of settling time*/
    if ( settlingTime < 0.0 )
    {
      throw Error("CHighZTest::processParameters()",
                  "settlingTime must be postive!");
    }

    param.settlingTime = settlingTime;
  }
  
  /*
   *----------------------------------------------------------------------*
   * Routine: processLimit
   *
   * Purpose: process Limit(s)
   *
   *----------------------------------------------------------------------*
   * Description:
   *   
   * Note:
   *----------------------------------------------------------------------*
   */
  static void processLimit(const string& testname, 
    const bool testnameHasUnit, HighZTestLimit& testLimit)
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
 * Purpose: execute measurement by PPMU and store results
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
  static void doMeasurement(const HighZTestParam& param,
                            const HighZTestLimit& testLimit, 
                            HighZTestResult& result)
  {
    PPMU_MEASURE ppmuMeasureResult;
    
    ON_FIRST_INVOCATION_BEGIN()
      CONNECT();
      FunctionalUtil::functionalPreTest( "ALL", 0 );
    ON_FIRST_INVOCATION_END() ;
    /*get functional test result*/
    result.funcResult =  FunctionalUtil::getFunctionalTestResult();

    ON_FIRST_INVOCATION_BEGIN();
      PmuUtil::ppmuParallelCurrentMeasurement(param.strPinList,
                                                   param.testMode,
                                                   param.forceVoltage,
                                                   testLimit.limit,  
                                                   TURN_OFF,     
                                                   0.0,        
                                                   param.settlingTime,
                                                   param.relaySwitchMode,
                                                   TURN_OFF,     
                                                   ppmuMeasureResult);

    ON_FIRST_INVOCATION_END() ;

    PmuUtil::ppmuParallelCurrentGetResult(param.expandedPins,
                                               param.testMode,
                                               ppmuMeasureResult,
                                               result.measureResult);
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
  static void judgeAndDatalog(const HighZTestParam &param, 
                              const HighZTestLimit& testLimit, 
                              const HighZTestResult &result,
                              double _results[4])
  {
    
    TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                       param.pinlist,
                                       "FUNCTION TEST ",
                                       result.funcResult?TM::Pass:TM::Fail,
                                       0);
    STRING_VECTOR::size_type j = 0;
    Boolean bPass = TRUE;
    /*init _results*/
    _results[0]=_results[2]= DBL_MAX; 
    _results[1]=_results[3]= -DBL_MAX;
    
    pair<DOUBLE,DOUBLE> minmax;
    minmax.first  =  DBL_MAX;   //min val
    minmax.second = -DBL_MAX;   //max val
    ARRAY_D measuredValueArray(param.expandedPins.size());

    switch ( param.testMode )
    {
    case TM::PVAL:
      {
        double factor_ToDefaultUnit = 1;
        if (testLimit.use_uAAsDefaultUnit)
        {
          factor_ToDefaultUnit = 1e6;
        }

        for ( j = 0; j < param.expandedPins.size(); ++j )
        {
          DOUBLE dMeasValue = result.measureResult.getPinsValue(param.expandedPins[j]);
          measuredValueArray[j] = dMeasValue * factor_ToDefaultUnit;
          dMeasValue = dMeasValue * 1e6;                //[uA]
          minmax.first = MIN(minmax.first,dMeasValue);  //min value
          minmax.second = MAX(minmax.second,dMeasValue);//max value
        }
        _results[0] = minmax.first; //min leakage current[uA]
        _results[1] = minmax.second;//max leakage current[uA]
        //MPR record
        if (testLimit.isLimitTableUsed)
        {
          TestSet.cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                           param.expandedPins,
                                           testLimit.testname,
                                           V93kLimits::tmLimits,
                                           measuredValueArray);
        }
        else
        {
          TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                           param.expandedPins,
                                           testLimit.testname,
                                           measuredValueArray);
 
        } 
      }
      break;

    case TM::PPF:
      for ( j = 0; j < param.expandedPins.size(); ++j )
      {
        bool bOnePass = result.measureResult.getPinPassFail(param.expandedPins[j]);
        measuredValueArray[j] = bOnePass?MPR_LOG_PF_TEST_PASS:MPR_LOG_PF_TEST_FAIL;
        bPass = bPass && bOnePass;
      }
      //Special MPR for PPF test      
      if (testLimit.isLimitTableUsed)
      {     
        V93kLimits::TMLimits::LimitInfo& limitInfo = V93kLimits::tmLimits.getLimit(
          param.testsuiteName,testLimit.testname); 
        TESTSET().cont(TM::CONTINUE).testnumber(limitInfo.TestNumber)
               .judgeAndLog_ParametricTest(param.expandedPins,
                                           testLimit.testname,
                                           bPass?TM::Pass : TM::Fail,
                                           measuredValueArray);
        if (!bPass && !limitInfo.BinsNumString.empty())
        {
          SET_MULTIBIN(limitInfo.BinsNumString,limitInfo.BinhNum);
        }
      }
      else
      {
        TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                           param.expandedPins,
                                           testLimit.testname,
                                           bPass?TM::Pass : TM::Fail,
                                           measuredValueArray);
 
      }
      break;

    case TM::GPF:
      bPass = result.measureResult.getGlobalPassFail();
      if (testLimit.isLimitTableUsed)
      {
        V93kLimits::TMLimits::LimitInfo& limitInfo = V93kLimits::tmLimits.getLimit(
          param.testsuiteName,testLimit.testname);
        TESTSET().cont(TM::CONTINUE).testnumber(limitInfo.TestNumber)
               .judgeAndLog_ParametricTest(param.pinlist,
                                           testLimit.testname,
                                           bPass?TM::Pass : TM::Fail,
                                           0);
        if (!bPass && !limitInfo.BinsNumString.empty())
        {
          SET_MULTIBIN(limitInfo.BinsNumString,limitInfo.BinhNum);
        }
      }
      else
      {
        TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                           param.pinlist,
                                           testLimit.testname,
                                           bPass?TM::Pass : TM::Fail,
                                           0);

      }
      break;

    default:
      throw Error("HighZTest::judgeAndDatalog",
                  "Unknown Test Mode");
    } /*end switch*/        

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
 *           testLimit          - test limit container
 *           result             - result container
 *           output             - "None" or "ReportUI" 
 *   OUTPUT: 
 *   RETURN:  
 * Note:
 *----------------------------------------------------------------------*
 */ 
  static void reportToUI(const HighZTestParam &param, 
                         const HighZTestLimit& testLimit, 
                         const HighZTestResult &result,
                         const STRING& output)
  {
    STRING_VECTOR::size_type j = 0;
    Boolean bPass = TRUE;
    
    CommonUtil::printTestSuiteName(output, "HighZ Test '", "'");
    
    switch ( param.testMode )
    {
    case TM::PVAL:
      {
        for ( j = 0; j < param.expandedPins.size(); ++j )
        {
          DOUBLE dMeasValue = result.measureResult.getPinsValue(param.expandedPins[j]);
          CommonUtil::datalogToWindow(output,
                                    param.expandedPins[j],
                                    "A",
                                    dMeasValue, 
                                    testLimit.use_uAAsDefaultUnit?"uA":"", 
                                    testLimit.limit);
        }
      }
      break;

    case TM::PPF:
      for ( j = 0; j < param.expandedPins.size(); ++j )
      {
        bPass = result.measureResult.getPinPassFail(param.expandedPins[j]);
        CommonUtil::datalogToWindow(output, param.expandedPins[j], bPass);
      }
      break;

    case TM::GPF:
      bPass = result.measureResult.getGlobalPassFail();
      CommonUtil::datalogToWindow(output,param.testsuiteName,bPass);
      break;

    default:
      throw Error("HighZTest::reportToUI",
                  "Unknown Test Mode");
    }/*end switch*/

    CommonUtil::datalogToWindow(output,"Functional result: ",result.funcResult);
  }
private:
  HighZTest() {}//private constructor to prevent instantiation.
};

#endif /*HIGHZTEST_H_*/
