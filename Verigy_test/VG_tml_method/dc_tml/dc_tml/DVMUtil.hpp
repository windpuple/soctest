#ifndef DVMUTIL_H_
#define DVMUTIL_H_

#include "CommonUtil.hpp"
#include "PmuUtil.hpp"

/***************************************************************************
 *                    DVM test class 
 ***************************************************************************
 */ 
class DVMTest
{
public:

/*
 *----------------------------------------------------------------------*
 *         test parameters container                                    *
 *----------------------------------------------------------------------*
 */
  struct DVMTestParam
  {
    /*original input parameters*/
    STRING pinlist;
    DOUBLE settlingTime;
    STRING measureMode;
    STRING ACRelay;

    /*new generated parameter for convenience*/
    STRING_VECTOR expandedPins;       /*expanded and validated pins stored in Vector*/
    TM::DCTEST_MODE testMode;         /*current test mode*/
    STRING testsuiteName;             /*current testsuite name*/

    /*initialize stuff to some defaults.*/ 
    void init()
    {
      pinlist = "";
      settlingTime = 0.0;
      expandedPins.clear();
      testMode = TM::GPF;
    }

    /*default constructor for intializing parameters*/
    DVMTestParam()
    {
      init();
    }

  };
  
  /*
   *----------------------------------------------------------------------*
   *         test limit container                                    *
   *----------------------------------------------------------------------*
   */
  struct DVMTestLimit
  {
    LIMIT limit;                      /*LIMIT object for this test*/
    string testname;
    bool use_mVAsDefaultUnit;
    bool isLimitTableUsed; 
    /*init limit*/
    void init()
    {
      limit = TM::Fail;
      testname.clear();
      isLimitTableUsed = true;
      use_mVAsDefaultUnit = true;
    }
    
    /*default constructor */
    DVMTestLimit()
    {
      init();
    }
  };

  /*
  *----------------------------------------------------------------------*
  *         test results container                                       *
  *----------------------------------------------------------------------*
  */
  struct DVMTestResult
  {
    /*result container 
     *for voltage value, the unit is: V
     */
    MeasurementResultContainer measurementResult;

    /*initialize all stuffs to some defaults.*/
    void init()
    {
      measurementResult.init();       
    } 

    /*default constructor*/
    DVMTestResult()
    {
      init();
    }
  };

/*
 *----------------------------------------------------------------------*
 *         public interfaces of DVM  test                               *
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
                                const DOUBLE settlingTime, 
                                const STRING& measureMode,
                                const STRING& ACRelay, 
                                DVMTestParam& param)
  {    
    /*Init param*/
    param.init();
    if ( settlingTime < 0.0 )
    {
      throw Error("DcTest::DVMTest()",
                  "settlingTime must be postive!");
    }
    param.settlingTime = settlingTime;
    param.measureMode = CommonUtil::trim(measureMode);
    param.ACRelay = CommonUtil::trim(ACRelay);

    /*
     ********************************************************* 
     * Since PMU_IFVM constructor does validate illegal 
     * pin names internally,and here ommit this check.
     * Meanwhile it check wrong type pins, 
     * if any, it throws an error. 
     *********************************************************
     */
    param.pinlist = CommonUtil::trim(pinlist);
    vector<string> pins = PinUtility.getDigitalPinNamesFromPinList(
                                                                 pinlist,
                                                                 TM::I_PIN|TM::O_PIN|TM::IO_PIN,
                                                                 FALSE, /* check missing pins */
                                                                 TRUE  /* check mismatched-type pins */
                                                                 );   
    param.expandedPins.resize(pins.size());
    copy(pins.begin(), pins.end(), param.expandedPins.begin());

    /*get the test suite name and test mode*/
    GET_TESTSUITE_NAME(param.testsuiteName);
    param.testMode = CommonUtil::getMode();
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
  static void processLimit(const string& testname, 
    const bool testnameHasUnit, DVMTestLimit& testLimit)
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
    //set default unit as "mV" 
    if (testnameHasUnit && testLimit.limit.unit().empty())
    {
      testLimit.use_mVAsDefaultUnit = true;
      testLimit.limit.unit("mV");
    }   
    else
    {
      testLimit.use_mVAsDefaultUnit = false;
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
  static void doMeasurement(const DVMTestParam& param, 
                            const DVMTestLimit& testLimit, 
                            DVMTestResult& result)
  {

    PPMU_MEASURE ppmuMeasure;
    SPMU_TASK badcTask;   
    ON_FIRST_INVOCATION_BEGIN();
      CONNECT();
      if (param.measureMode == "PPMUpar")
      {
        PmuUtil::ppmuParallelVoltageMeasurement(param.pinlist,
                                              param.testMode,
                                              1.0,         /* default value of forceCurrent */
                                              testLimit.limit,
                                              TURN_OFF,    /* precharge */
                                              0.0,         /* prechargeVoltage */
                                              param.settlingTime,
                                              "PARALLEL",  /* relaySwitchMode */
                                              /*****************************
                                               * set termination on to
                                               * just switch PPMU_relay only 
                                               * inside the sub-function.
                                               *****************************
                                               */
                                              TURN_ON,        /* termination */ 
                                              -0.1,           /* ppmuClamp Low/High */
                                              0.1,              
                                              ppmuMeasure
                                              );
      }
      else if (param.measureMode == "BoardADC")
      {
        PmuUtil::boardADCVoltageMeasurement(param.pinlist,
                                            param.testMode,
                                            1.0,       /* default value of forceCurrent */
                                            testLimit.limit,
                                            "PARALLEL",
                                            TURN_OFF,  /* precharge */
                                            0,         /* prechargeVoltage */
                                            param.settlingTime,
                                            param.ACRelay,   /* termination */
                                            true,      /* pin parallel execution */
                                            badcTask);
      }
      else
      {
        throw Error("DVMTest::doMeasurement","Unsupport measurement mode.");
      }
    ON_FIRST_INVOCATION_END();
    if (param.measureMode == "PPMUpar")
    {
      PmuUtil::ppmuParallelVoltageGetResult(param.expandedPins,
                                            param.testMode,
                                            ppmuMeasure,                               
                                            result.measurementResult);
    }
    else
    {
      PmuUtil::spmuVoltageGetResult(param.expandedPins,
                                    param.testMode,
                                    badcTask,
                                    result.measurementResult);
    }
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
  static void judgeAndDatalog(const DVMTestParam& param, 
                              const DVMTestLimit& testLimit, 
                              const DVMTestResult &result, 
                              double _results[4])
  {
    STRING_VECTOR::size_type j = 0;
    DOUBLE dMeasValue = 0;
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
        if (testLimit.use_mVAsDefaultUnit)
        {
          factor_ToDefaultUnit = 1e3;
        }
        for ( j = 0; j< param.expandedPins.size(); ++j )
        {
          dMeasValue = result.measurementResult.getPinsValue(param.expandedPins[j]);
          measuredValueArray[j] = factor_ToDefaultUnit * dMeasValue;
        
          dMeasValue = dMeasValue*1e3;                  //[mV]
          minmax.first = MIN(minmax.first,dMeasValue);  //min value
          minmax.second = MAX(minmax.second,dMeasValue);//max value
        }
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
        _results[0] = minmax.first; //min voltage [mV]
        _results[1] = minmax.second; //max voltage [mV]
      }
      break;

    case TM::PPF:
      for ( j = 0; j < param.expandedPins.size(); ++j )
      {
        bool bOnePass = result.measurementResult.getPinPassFail(param.expandedPins[j]);
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
      bPass = result.measurementResult.getGlobalPassFail();
      
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
      throw Error("DVMTest::judgeAndDatalog",
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
 *           result             - result container
 *           output             - "None" or "ReportUI" 
 *   OUTPUT: 
 *   RETURN:  
 * Note:
 *----------------------------------------------------------------------*
 */  
  static void reportToUI(const DVMTestParam& param,
                         const DVMTestLimit& testLimit, 
                         const DVMTestResult& result, 
                         const STRING& output)
  {
    CommonUtil::printTestSuiteName(output, "DVM '", "'");

    STRING_VECTOR::size_type j = 0;
    Boolean bPass = TRUE;

    if ( output != "ReportUI" )
      return;

    switch ( param.testMode )
    {
    case TM::PVAL:
      {
        for ( j = 0; j< param.expandedPins.size(); ++j )
        {
          DOUBLE dMeasValue = result.measurementResult
                                    .getPinsValue(param.expandedPins[j]); /*[V]*/
          CommonUtil::datalogToWindow(output,
                                    param.expandedPins[j], 
                                    "V",
                                    dMeasValue,
                                    testLimit.use_mVAsDefaultUnit?"mV":"",
                                    testLimit.limit);
        }
      }
      break;

    case TM::PPF:
      for ( j = 0; j < param.expandedPins.size(); ++j )
      {
        bPass = result.measurementResult.getPinPassFail(param.expandedPins[j]);
        CommonUtil::datalogToWindow(output, param.expandedPins[j], bPass);
      }
      break;

    case TM::GPF:
      bPass = result.measurementResult.getGlobalPassFail();
      CommonUtil::datalogToWindow(output,param.testsuiteName, bPass);
      break;

    default:
      throw Error("DVMTest::reportToUI",
                  "Unknown Test Mode");
    }/* end switch*/
  }
private:
  DVMTest() {}//private constructor to prevent instantiation.
};
#endif /*DVM_H_*/
