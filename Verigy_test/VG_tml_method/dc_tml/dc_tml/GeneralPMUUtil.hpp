#ifndef GENERALPMUUTIL_H_
#define GENERALPMUUTIL_H_

#include "CommonUtil.hpp"
#include "PmuUtil.hpp"
/***************************************************************************
 *                    generalPMU test class 
 ***************************************************************************
 */ 
class GeneralPMUTest
{
public:

/*
 *----------------------------------------------------------------------*
 *         test parameters container                                    *
 *----------------------------------------------------------------------*
 */
  struct GeneralPMUTestParam
  {
    /*original input parameters*/
    STRING pinlist;
    STRING forceMode;
    STRING precharge;
    STRING testerState;
    STRING termination;
    STRING measureMode;
    STRING relaySwitchMode;
    DOUBLE forceValue;
    DOUBLE spmuClamp;
    DOUBLE prechargeVoltage;
    DOUBLE settlingTime;
    DOUBLE ppmuClampLow;
    DOUBLE ppmuClampHigh;

    /*new generated parameter for convenience*/
    STRING_VECTOR expandedPins;   /*expanded and validated pins stored in Vector*/
    STRING testsuiteName;         /*current testsuite name*/
    TM::DCTEST_MODE testMode;     /*current test mode*/

    /*initialize stuff to some defaults.*/  
    void init()
    {
      pinlist = "";
      expandedPins.clear();
      forceMode = "UNKNOWN";
      forceValue = 0.0;
      spmuClamp = 0.0;
      prechargeVoltage = 0.0;
      settlingTime = 0.0;
      ppmuClampLow = 0.0;
      ppmuClampHigh = 0.0;    
      measureMode = "UNKNOWN";
      testMode = TM::GPF;
    }

    /*default constructor for intializing parameters*/
    GeneralPMUTestParam()
    {
      init();
    }
  };
  
  /*
   *----------------------------------------------------------------------*
   *         test limit container                                    *
   *----------------------------------------------------------------------*
   */
  struct GeneralPMUTestLimit
  {
    LIMIT limit;                  /*LIMIT object for this test*/
    bool isLimitTableUsed;  
    bool use_uAmVAsDefaultUnit;
    string testname;
    /*init limit*/
    void init()
    {
      limit = TM::Fail;
      isLimitTableUsed = false;
      use_uAmVAsDefaultUnit = true;
      testname.clear();
    }
    
    /*default constructor*/
    GeneralPMUTestLimit()
    {
      init();
    }    
  };

  /*
 *----------------------------------------------------------------------*
 *         test results container                                       *
 *----------------------------------------------------------------------*
 */
  struct GeneralPMUTestResult
  {
    /*result container 
    *for current value, the unit is: A
    *for voltage value, the unit is: V
    */
    MeasurementResultContainer measurementResult;

    /*initialize all stuffs to some defaults.*/
    void init()
    {
      measurementResult.init();            
    }

    /*default constructor*/
    GeneralPMUTestResult()
    {
      init();
    }
  };

/*
 *----------------------------------------------------------------------*
 *         public interfaces of General PMU test                        *
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
                                const STRING& forceMode,
                                const DOUBLE forceValue,
                                const DOUBLE spmuClamp,
                                const STRING& precharge,
                                const DOUBLE prechargeVoltage,
                                const DOUBLE settlingTime,
                                const STRING& testerState,
                                const STRING& termination,
                                const STRING& measureMode,
                                const STRING& relaySwitchMode,
                                const DOUBLE ppmuClampLow,
                                const DOUBLE ppmuClampHigh,
                                GeneralPMUTestParam& param)
  {
    /*init the parame*/
    param.init();
    /* 
     * To expand all digital pinlist.
     * since PMU_VFIM can validate pinlist internally,
     * here ommit this kind of checking.
     */
    param.pinlist = CommonUtil::trim(pinlist);
    param.expandedPins = PinUtility.expandDigitalPinNamesFromPinList(
                                                                    pinlist, 
                                                                    TM::ALL_DIGITAL);
    param.forceMode = CommonUtil::trim(forceMode);
    param.forceValue = forceValue;
    param.spmuClamp = spmuClamp;
    param.precharge = CommonUtil::trim(precharge);
    param.prechargeVoltage = prechargeVoltage;
    param.settlingTime = settlingTime;
    param.termination = CommonUtil::trim(termination);
    param.testerState = CommonUtil::trim(testerState);
    param.measureMode = CommonUtil::trim(measureMode);
    param.relaySwitchMode = CommonUtil::trim(relaySwitchMode);
    param.ppmuClampLow = ppmuClampLow;
    param.ppmuClampHigh = ppmuClampHigh;        

    /*get the test suite name and test mode*/
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
  static void processLimit(GeneralPMUTestParam& param, 
                           const string& testname,
                           const bool testnameHasUnit,
                           GeneralPMUTestLimit& testLimit)
  {
    /* init testLimit*/
    testLimit.init();

    //if limit CSV file is load, get limit from it;otherwise get limit from testflow
    testLimit.testname = testname;
    TesttableLimitHelper ttlHelper(param.testsuiteName);
 
    if (ttlHelper.isLimitCsvFileLoad())
    {
      ttlHelper.getLimit(testname,testLimit.limit);
    }
      
    testLimit.isLimitTableUsed = ttlHelper.isAllInTable();

    //get limit from testflow
    if (!testLimit.isLimitTableUsed)
    {
      testLimit.limit = GET_LIMIT_OBJECT(testLimit.testname);
    }

    if (testnameHasUnit && testLimit.limit.unit().empty())
    {
      testLimit.use_uAmVAsDefaultUnit = true;
      if ( param.forceMode == "VOLT" )
      {
        testLimit.limit.unit("uA");
      }
      else
      {
        testLimit.limit.unit("mV");
      }
    }
    else
    {
      testLimit.use_uAmVAsDefaultUnit = false;
    }

    if ( param.forceMode == "CURR" )
    {
      double highVal_V;
      testLimit.limit.getHigh(&highVal_V);
      if (testLimit.use_uAmVAsDefaultUnit) //mV is used in limit
      {
        highVal_V = highVal_V * 1e-3;
      }
      CommonUtil::clampSimulation(param.measureMode,
                                  param.spmuClamp, 
                                  highVal_V, 
                                  param.forceValue);
    }
  }

/*
 *----------------------------------------------------------------------*
 * Routine: doMeasurement
 *
 * Purpose: execute measurement by PMU and store results
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
  static void doMeasurement(
                           const GeneralPMUTestParam& param, 
                           const GeneralPMUTestLimit& limit, 
                           GeneralPMUTestResult& result)
  { 
    ON_FIRST_INVOCATION_BEGIN();
      CommonUtil::conditionalConnect(param.testerState);  
    ON_FIRST_INVOCATION_END();
       
    if(param.measureMode == "PPMUpar")
    { 
      doPpmuParallelMeasurement(param, limit, result);
    }
    else if(param.measureMode == "PPMUser")
    {
      doPpmuSerialMeasurement(param,limit, result);
      
    }
    else if(param.measureMode == "SPMUser")
    {
      doSpmuSerialMeasurement(param, limit, result);
    }
    else
    {
      throw Error ("GeneralPMUTest::doMeasurement",
                   "Unkown measureMode");
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
 *           testLimit   - test limit container
 *           result      - result container
 *           _results[4] - retrieved by built-in tf_result() in testflow.
 *   OUTPUT: 
 *   RETURN: 
 * Note:
 *----------------------------------------------------------------------*
 */ 
  static void judgeAndDatalog(const GeneralPMUTestParam& param, 
                              const GeneralPMUTestLimit& testLimit,
                              const GeneralPMUTestResult& result,
                              double _results[4])
  {

    STRING_VECTOR::size_type j = 0;
    bool bPass = true;
    /*init _results*/
    _results[0]=_results[2]= DBL_MAX; 
    _results[1]=_results[3]= -DBL_MAX;
    
    pair<DOUBLE,DOUBLE> minmax;
    minmax.first  =  DBL_MAX;   //min val
    minmax.second = -DBL_MAX;   //max val
    ARRAY_D measuredValArray(param.expandedPins.size());
 
    switch ( param.testMode )
    {
    case TM::PVAL:
      {
        double factor_To_mV_uA = SI_Value::getDiffValue(
          (param.forceMode == "VOLT")?"A":"V",(param.forceMode == "VOLT")?"uA":"mV");
        double factor_To_DefaultUnit = 1;
        if (testLimit.use_uAmVAsDefaultUnit)
        {
          factor_To_DefaultUnit = (param.forceMode == "VOLT")?1e6:1e3;
        }
 
        for ( j = 0; j < param.expandedPins.size(); ++j )
        {
          //get value from API with unit of [A] or [V]
          double dMeasValue = result.measurementResult.getPinsValue(param.expandedPins[j]);
          measuredValArray[j] = dMeasValue * factor_To_DefaultUnit; // convert value to limit's unit 
          minmax.first = MIN(minmax.first, dMeasValue * factor_To_mV_uA );  //min value
          minmax.second = MAX(minmax.second, dMeasValue * factor_To_mV_uA); //max value   
        }
        _results[0] = minmax.first;//min voltage [mV] or min current [uA]
        _results[1] = minmax.second;//max voltage [mV] or max current [uA]

        if (testLimit.isLimitTableUsed)
        {
          TestSet.cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                           param.expandedPins,
                                           testLimit.testname,
                                           V93kLimits::tmLimits,
                                           measuredValArray);
        }
        else
        {
          TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                           param.expandedPins,
                                           testLimit.testname,
                                           measuredValArray);
 
        }
      }
      break;

    case TM::PPF:
      for ( j = 0; j < param.expandedPins.size(); ++j )
      {
        bPass &= result.measurementResult.getPinPassFail(param.expandedPins[j]);
        measuredValArray[j] = bPass?MPR_LOG_PF_TEST_PASS:MPR_LOG_PF_TEST_FAIL;
      }      
      if (testLimit.isLimitTableUsed)
      {     
        V93kLimits::TMLimits::LimitInfo& limitInfo = V93kLimits::tmLimits.getLimit(
           param.testsuiteName,testLimit.testname); 
        TESTSET().cont(TM::CONTINUE).testnumber(limitInfo.TestNumber)
               .judgeAndLog_ParametricTest(param.expandedPins,
                                           testLimit.testname,
                                           bPass?TM::Pass : TM::Fail,
                                           measuredValArray);
        if (!bPass)
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
                                           measuredValArray);
 
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
        if (!bPass)
        {
          SET_MULTIBIN(limitInfo.BinsNumString,limitInfo.BinhNum);
        }
      }
      else
      {
        TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                           param.pinlist,
                                           testLimit.testname, 
                                           bPass?TM::Pass:TM::Fail, 
                                           0);
      }
      break;

    default:
      throw Error("GeneralPMUTest::judgeAndDatalog",
                  "Unknown Test Mode");
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
 *           testLimit          - test limit container
 *           output             - "None" or "ReportUI" 
 *           result             - result container
 *   OUTPUT: 
 *   RETURN:  
 * Note:
 *----------------------------------------------------------------------*
 */  
  static void reportToUI(const GeneralPMUTestParam& param, 
                         const GeneralPMUTestLimit& testLimit, 
                         const GeneralPMUTestResult& result,
                         const STRING& output)
  {
    CommonUtil::printTestSuiteName(output, "General PMU Test '", "'");

    STRING_VECTOR::size_type j = 0;
    STRING unit;
    Boolean bPass = TRUE;

    if ( output != "ReportUI" )
    {
      return;
    }

    switch ( param.testMode )
    {
    case TM::PVAL:
      {
        string defaultUnit;
        if (testLimit.use_uAmVAsDefaultUnit)
        {
          defaultUnit = param.forceMode == "VOLT" ? "uA":"mV";
        }

        for ( j = 0; j < param.expandedPins.size(); ++j )
        {
          DOUBLE dMeasValue = result.measurementResult.getPinsValue(param.expandedPins[j]);
          
          CommonUtil::datalogToWindow(output,
                                    param.expandedPins[j],
                                    (param.forceMode == "VOLT" ? "A":"V"),
                                    dMeasValue,
                                    defaultUnit, 
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
      CommonUtil::datalogToWindow(output,param.testsuiteName,bPass);
      break;

    default:
      throw Error("GeneralPMUTest::reportToUI",
                  "Unknown Test Mode");
    }/* end switch*/
  }
  
private:
  GeneralPMUTest() {}//private constructor to prevent instantiation.   
  
  static void doPpmuParallelMeasurement(
                     const GeneralPMUTestParam& param, 
                     const GeneralPMUTestLimit& testLimit, 
                     GeneralPMUTestResult& result);
                                        
  static void doPpmuSerialMeasurement(
                     const GeneralPMUTestParam& param,
                     const GeneralPMUTestLimit& testLimit, 
                     GeneralPMUTestResult& result);
  
  static void doSpmuSerialMeasurement(
                     const GeneralPMUTestParam& param, 
                     const GeneralPMUTestLimit& testLimit, 
                     GeneralPMUTestResult& result);
};
  
/*
*----------------------------------------------------------------------*
* Routine: GeneralPMUTest::doPpmuParallelMeasurement
*
* Purpose: measure with parellel ppmu and get the test results
*
*----------------------------------------------------------------------*
* Description:
*
* Note:
*
*----------------------------------------------------------------------*
*/  
inline void GeneralPMUTest::doPpmuParallelMeasurement(
                                  const GeneralPMUTestParam& param, 
                                  const GeneralPMUTestLimit& testLimit, 
                                  GeneralPMUTestResult& result)
{
  PPMU_MEASURE PPMUMeasure;
  Boolean needSetSequencerOff = false;   
  ON_FIRST_INVOCATION_BEGIN();             
    /* to eliminate side-effect of sequencer's state after PPMU-paralle measurement,
     * after PPMU-parallel(FlexDC) measurement. we should do:
     * if connectState==DISCONNECT, 
     *    must set: needSetSequencerOff = TRUE
     * if connectState==UNCHANGED and the state of sequencer is OFF, 
     *    must set: needSetSequencerOff = TRUE
     * otherwise needSetSequencerOff = FALSE.
     * The result is TRUE, that means to set sequencer to OFF state.
     */    
    if (param.testerState == "DISCONNECTED" )
    {
      needSetSequencerOff = true;
    }
    else if (param.testerState == "UNCHANGED" )
    {
      STRING fwAnswer;
      FW_TASK("SQST?\n",fwAnswer);
      if ( fwAnswer.find("OFF",5) != string::npos )
      {
        needSetSequencerOff = true;
      }
    }
  ON_FIRST_INVOCATION_END();
    
  if ( param.forceMode == "VOLT" )
  {
    ON_FIRST_INVOCATION_BEGIN();
      PmuUtil::ppmuParallelCurrentMeasurement(                                          
                                         param.pinlist,
                                         param.testMode,
                                         param.forceValue,
                                         testLimit.limit,                                          
                                         param.precharge,
                                         param.prechargeVoltage,
                                         param.settlingTime,
                                         param.relaySwitchMode,
                                         param.termination,
                                         PPMUMeasure
                                        );
    ON_FIRST_INVOCATION_END();
    PmuUtil::ppmuParallelCurrentGetResult(
                             param.expandedPins,
                             param.testMode,
                             PPMUMeasure,
                             result.measurementResult);
  }
  else if ( param.forceMode == "CURR" )
  {
    ON_FIRST_INVOCATION_BEGIN();
      PmuUtil::ppmuParallelVoltageMeasurement(                                            
                                         param.pinlist,
                                         param.testMode,
                                         param.forceValue,
                                         testLimit.limit,                                             
                                         param.precharge,
                                         param.prechargeVoltage,
                                         param.settlingTime,
                                         param.relaySwitchMode,
                                         param.termination,
                                         param.ppmuClampLow,
                                         param.ppmuClampHigh,
                                         PPMUMeasure                                            
                                      );
    ON_FIRST_INVOCATION_END();
    PmuUtil::ppmuParallelVoltageGetResult(
                             param.expandedPins,
                             param.testMode,
                             PPMUMeasure,
                             result.measurementResult);                       
  }

  else
  {
    throw Error("GeneralPMUTest::doPpmuParallelMeasurement", "Wrong Force Mode.");
  }
  ON_FIRST_INVOCATION_BEGIN();
    /* To eliminate the side-effect after PPMU-paralle measurement, */
    /* the sequencer must be set to OFF in some cases.              */
    if ( needSetSequencerOff )
    {
      FW_TASK("SQST OFF\n");
    }
  ON_FIRST_INVOCATION_END() ;
}

/*
*----------------------------------------------------------------------*
* Routine: GeneralPMUTest::doPpmuSerialMeasurement
*
* Purpose: measure with serial ppmu and get the test results
*
*----------------------------------------------------------------------*
* Description:
*
* Note:
*
*----------------------------------------------------------------------*
*/ 

inline void GeneralPMUTest::doPpmuSerialMeasurement(
                                    const GeneralPMUTestParam& param,
                                    const GeneralPMUTestLimit& testLimit, 
                                    GeneralPMUTestResult& result)
{           
  PPMU_MEASURE PPMUMeasure;
  Boolean needSetSequencerOff = false;   

  ON_FIRST_INVOCATION_BEGIN();             
    /* to eliminate side-effect of sequencer's state after PPMU measurement,
     * after PPMU(FlexDC) measurement. we should do:
     * if connectState==DISCONNECT, 
     *    must set: needSetSequencerOff = TRUE
     * if connectState==UNCHANGED and the state of sequencer is OFF, 
     *    must set: needSetSequencerOff = TRUE
     * otherwise needSetSequencerOff = FALSE.
     * The result is TRUE, that means to set sequencer to OFF state.
     */    
    if (param.testerState == "DISCONNECTED" )
    {
      needSetSequencerOff = true;
    }
    else if (param.testerState == "UNCHANGED" )
    {
      STRING fwAnswer;
      FW_TASK("SQST?\n",fwAnswer);
      if ( fwAnswer.find("OFF",5) != string::npos )
      {
        needSetSequencerOff = true;
      }
    }
  ON_FIRST_INVOCATION_END();

  if ( param.forceMode == "VOLT" )
  {
    
    ON_FIRST_INVOCATION_BEGIN();
      PmuUtil::ppmuSerialCurrentMeasurement(                                         
                                         param.pinlist,
                                         param.testMode,
                                         param.forceValue,
                                         testLimit.limit,                                         
                                         param.precharge,
                                         param.prechargeVoltage,
                                         param.settlingTime,                                          
                                         param.relaySwitchMode,
                                         param.termination,
                                         PPMUMeasure);
    ON_FIRST_INVOCATION_END();
    PmuUtil::ppmuSerialCurrentGetResult(
                                       param.expandedPins,
                                       param.testMode,
                                       PPMUMeasure,
                                       result.measurementResult);
  }
  else if ( param.forceMode == "CURR" )
  {
    ON_FIRST_INVOCATION_BEGIN();
      PmuUtil::ppmuSerialVoltageMeasurement(                                         
                                         param.pinlist,
                                         param.testMode,
                                         param.forceValue,
                                         testLimit.limit,                                         
                                         param.precharge,
                                         param.prechargeVoltage,
                                         param.settlingTime,
                                         param.relaySwitchMode,                                          
                                         param.termination,
                                         param.ppmuClampLow,
                                         param.ppmuClampHigh,
                                         PPMUMeasure);
    ON_FIRST_INVOCATION_END();
    PmuUtil::ppmuSerialVoltageGetResult(
                                       param.expandedPins,
                                       param.testMode,
                                       PPMUMeasure,
                                       result.measurementResult);
  }
  else
  {
    throw Error("GeneralPMUTest::doPpmuSerialMeasurement", "Wrong Force Mode.");
  }
  
  ON_FIRST_INVOCATION_BEGIN();
    /* To eliminate the side-effect after PPMU-paralle measurement, */
    /* the sequencer must be set to OFF in some cases.              */
    if ( needSetSequencerOff )
    {
     FW_TASK("SQST OFF\n");
    }
  ON_FIRST_INVOCATION_END() ;
}

/*
*----------------------------------------------------------------------*
* Routine: GeneralPMUTest::doPpmuSerialMeasurement
*
* Purpose: measure with serial ppmu and get the test results
*
*----------------------------------------------------------------------*
* Description:
*
* Note:
*
*----------------------------------------------------------------------*
*/   
inline void GeneralPMUTest::doSpmuSerialMeasurement(
                                    const GeneralPMUTestParam& param, 
                                    const GeneralPMUTestLimit& testLimit, 
                                    GeneralPMUTestResult& result)
{
  SPMU_TASK spmuTask;
  
  if ( param.forceMode == "VOLT" )
  {          
    ON_FIRST_INVOCATION_BEGIN();
      PmuUtil::spmuSerialCurrentMeasurement(                                         
                                         param.pinlist,
                                         param.testMode,
                                         param.forceValue,
                                         testLimit.limit,
                                         param.relaySwitchMode,
                                         param.spmuClamp,                                         
                                         param.precharge,
                                         param.prechargeVoltage,
                                         param.settlingTime,                                          
                                         param.termination,
                                         spmuTask);
    ON_FIRST_INVOCATION_END();
    PmuUtil::spmuSerialCurrentGetResult(
                                       param.expandedPins,
                                       param.testMode,
                                       spmuTask,
                                       result.measurementResult);
  }
  else if ( param.forceMode == "CURR" )
  {
    PMU_IFVM pmuIfvm(param.pinlist, TM::PPMU);
    ON_FIRST_INVOCATION_BEGIN();
      PmuUtil::spmuSerialVoltageMeasurement(                                         
                                         param.pinlist,
                                         param.testMode,
                                         param.forceValue,
                                         testLimit.limit,
                                         param.relaySwitchMode,
                                         param.spmuClamp,                                         
                                         param.precharge,
                                         param.prechargeVoltage,
                                         param.settlingTime,                                          
                                         param.termination,
                                         spmuTask);
    ON_FIRST_INVOCATION_END();
    PmuUtil::spmuSerialVoltageGetResult(
                                       param.expandedPins,
                                       param.testMode,
                                       spmuTask,
                                       result.measurementResult);
  }
  else
  {
    throw Error("GeneralPMUTest::doMeasurement", "Wrong Force Mode.");
  } 
}


#endif /*GENERALPMUTEST_H_*/
