#ifndef LEAKAGEUTIL_H_
#define LEAKAGEUTIL_H_

#include "CommonUtil.hpp"
#include "FunctionalUtil.hpp"
#include "PmuUtil.hpp"

/***************************************************************************
 *                    leakage test class 
 ***************************************************************************
 */ 
class LeakageTest
{
public:

  enum
  {
    TWO_LEVEL = 2     /*two kinds of leakage level*/
  };  
  enum MeasuredLevelType
  {
    LOW_LEVEL = 0, HIGH_LEVEL = 1
  };  

/*
 *----------------------------------------------------------------------*
 *         test parameters container                                    *
 *----------------------------------------------------------------------*
 */
  struct LeakageTestParam
  {
    /*original input parameters*/
    STRING pinlist;
    DOUBLE forceVoltage[TWO_LEVEL];
    DOUBLE spmuClampCurrent[TWO_LEVEL];
    STRING ppmuPrecharge;
    DOUBLE prechargeVoltage[TWO_LEVEL];
    DOUBLE settlingTime[TWO_LEVEL];
    STRING preFunction;
    INT stopCycVec[TWO_LEVEL];
    STRING measureMode;            /*PMU,PPMU,SPMU*/
    STRING relaySwitchMode;

    /*new generated parameter for convenience*/
    Boolean isMeasure[TWO_LEVEL];  /*if the measurement of low level or high level is done */ 
    STRING testsuiteName;          /*current testsuite name*/
    TM::DCTEST_MODE testMode;
    STRING_VECTOR expandedPins;    /*expanded and validated pins stored in Vector*/
    

    /*initialize stuff to some defaults.*/ 
    void init()
    {
      pinlist="";
      expandedPins.clear();
      forceVoltage[LOW_LEVEL] = forceVoltage[HIGH_LEVEL] = 0.0;
      spmuClampCurrent[LOW_LEVEL] = spmuClampCurrent[HIGH_LEVEL] = 0.0; 
      prechargeVoltage[LOW_LEVEL] = prechargeVoltage[HIGH_LEVEL] = 0.0;
      settlingTime[LOW_LEVEL] = settlingTime[HIGH_LEVEL] = 0.0;
      stopCycVec[LOW_LEVEL] = stopCycVec[HIGH_LEVEL] = 0.0; 

      isMeasure[LOW_LEVEL] = false;
      isMeasure[HIGH_LEVEL] = false;

      measureMode = "UNKNOWN";
      relaySwitchMode = "UNKNOWN";
      testMode = TM::GPF; 
    }

    /*default constructor for intializing parameters*/
    LeakageTestParam()
    {
      init();
    }
  };
  
  /*
   *----------------------------------------------------------------------*
   *         test limit(s) container                                      *
   *----------------------------------------------------------------------*
   */
  struct LeakageTestLimit
  {
    LIMIT limit[TWO_LEVEL];        /*LIMIT object arrays*/
    string testnames[TWO_LEVEL];   
    bool use_uAAsDefaultUnit[TWO_LEVEL];   
    bool isLimitTableUsed;
   
    /*initialize limit(s) to some defaults.*/ 
    void init()
    {
      limit[LOW_LEVEL] = TM::Fail;
      limit[HIGH_LEVEL] = TM::Fail;
      testnames[LOW_LEVEL] = ""; 
      testnames[HIGH_LEVEL] = "";
      use_uAAsDefaultUnit[LOW_LEVEL] = true;
      use_uAAsDefaultUnit[HIGH_LEVEL] = true;
      isLimitTableUsed = true; 
    }
     
    /*default constructor */
    LeakageTestLimit()
    {
      init();
    }

  };

  /*
  *----------------------------------------------------------------------*
  *         test results container                                       *
  *----------------------------------------------------------------------*
  */
  struct LeakageTestResult
  {

    /*result container per site:
     *for value, the unit is: A
     */
    Boolean funcResult[TWO_LEVEL];                    /*true: PASS, false: FAIL*/
    MeasurementResultContainer measureResult[TWO_LEVEL];

    /*initialize all stuffs to some defaults.*/
    void init()
    {
      funcResult[LOW_LEVEL] = true;
      funcResult[HIGH_LEVEL] = true;
      measureResult[LOW_LEVEL].init();
      measureResult[HIGH_LEVEL].init();
    }
    /*default constructor*/
    LeakageTestResult()
    {
      init();
    }


  };

/*
 *----------------------------------------------------------------------*
 *         public interfaces of leakage test                            *
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
                                const STRING& measure,
                                const DOUBLE forceVoltageLow,
                                const DOUBLE forceVoltageHigh,
                                const DOUBLE spmuClampCurrentLow,
                                const DOUBLE spmuClampCurrentHigh,
                                const STRING& ppmuPrecharge,
                                const DOUBLE prechargeVoltageLow,
                                const DOUBLE prechargeVoltageHigh,
                                const DOUBLE settlingTimeLow,
                                const DOUBLE settlingTimeHigh,
                                const STRING& preFunction,
                                const INT stopCycVecLow,
                                const INT stopCycVecHigh,
                                const STRING& measureMode,
                                const STRING& relaySwitchMode,
                                LeakageTestParam &param)
  {
    /*initial param*/
    param.init();
    /*validate pinlist and expand it*/
    param.pinlist = CommonUtil::trim(pinlist);
    param.expandedPins = PinUtility.expandDigitalPinNamesFromPinList(
                                                                    pinlist, 
                                                                    TM::ALL_DIGITAL);
    STRING measureString = CommonUtil::trim(measure);
    if ( (measureString == "LOW")  || (measureString == "BOTH") )
    {
      param.isMeasure[LOW_LEVEL] = true;      /* the low level measurement will be done*/
      param.forceVoltage[LOW_LEVEL] = forceVoltageLow;
      param.spmuClampCurrent[LOW_LEVEL] = spmuClampCurrentLow;
      param.settlingTime[LOW_LEVEL] = settlingTimeLow;
      param.stopCycVec[LOW_LEVEL] = stopCycVecLow;
      param.prechargeVoltage[LOW_LEVEL] = prechargeVoltageLow;

     
    }

    if ( (measureString == "HIGH") || (measureString == "BOTH") )
    {
      param.isMeasure[HIGH_LEVEL] = true;    /* the high level measurement will be done*/
      param.forceVoltage[HIGH_LEVEL] = forceVoltageHigh;

      param.spmuClampCurrent[HIGH_LEVEL] = spmuClampCurrentHigh;
      param.settlingTime[HIGH_LEVEL] = settlingTimeHigh;
      param.stopCycVec[HIGH_LEVEL] = stopCycVecHigh;
      param.prechargeVoltage[HIGH_LEVEL] = prechargeVoltageHigh;        
    }
 
    param.ppmuPrecharge = CommonUtil::trim(ppmuPrecharge);
    param.preFunction = CommonUtil::trim(preFunction);
    param.measureMode = CommonUtil::trim(measureMode);
    param.relaySwitchMode = CommonUtil::trim(relaySwitchMode);

    /*get test mode and test suite name*/
    GET_TESTSUITE_NAME ( param.testsuiteName );
    param.testMode = CommonUtil::getMode();   
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
  static void processLimit(const LeakageTestParam& param, const string& testname, LeakageTestLimit& testLimit)
  {
    /* init testLimit */
    testLimit.init();

    string validName = CommonUtil::trim(testname);
    string::size_type leadPos = validName.find_first_not_of(" ()");
    string::size_type postPos = validName.find_last_not_of(" ()");
    validName = validName.substr(leadPos,postPos-leadPos +1);
    vector<string> limitNames;
    CommonUtil::splitStr(validName,',',limitNames);

    bool isLimitNameValid = false;
    if (!limitNames.empty())
    {
      if (limitNames.size() == 1)
      {
        testLimit.testnames[HIGH_LEVEL] = 
          testLimit.testnames[LOW_LEVEL] = limitNames[0];
      }
      else
      {
        testLimit.testnames[LOW_LEVEL] = limitNames[0];
        testLimit.testnames[HIGH_LEVEL] = limitNames[1];
      }

      if ( (param.isMeasure[LOW_LEVEL] && param.isMeasure[HIGH_LEVEL])
           && limitNames.size() != TWO_LEVEL)
      { 
        isLimitNameValid = false;
      }
      else
      {
        isLimitNameValid = true;
      }
    }
    else
    {
      isLimitNameValid = false;
    }
 
    if (!isLimitNameValid)
    {
      throw Error("LeakageTest::processLimit","parameter \"testname\" value is invalid!",
        "LeakageTest::processLimit");
    }
    
    //if limit CSV file is load, get limit from it;otherwise get limit from testflow.
    TesttableLimitHelper ttlHelper(param.testsuiteName);
    if (ttlHelper.isLimitCsvFileLoad())
    {
      if (param.isMeasure[LOW_LEVEL])
      {
        ttlHelper.getLimit(testLimit.testnames[LOW_LEVEL],testLimit.limit[LOW_LEVEL]);
      }
      
      if (param.isMeasure[HIGH_LEVEL])
      {
        ttlHelper.getLimit(testLimit.testnames[HIGH_LEVEL],testLimit.limit[HIGH_LEVEL]);
      }
    }
      
    testLimit.isLimitTableUsed = ttlHelper.isAllInTable();

    //get limit from testflow  
    if (!testLimit.isLimitTableUsed)
    {
      if (param.isMeasure[LOW_LEVEL])
      {
        testLimit.limit[LOW_LEVEL] = GET_LIMIT_OBJECT(testLimit.testnames[LOW_LEVEL]);
      }

      if (param.isMeasure[HIGH_LEVEL])
      {
        testLimit.limit[HIGH_LEVEL] = GET_LIMIT_OBJECT(testLimit.testnames[HIGH_LEVEL]);
      }
    }
     
    //set default unit (uA) 
    if(param.isMeasure[LOW_LEVEL])
    {
      if (testLimit.testnames[LOW_LEVEL] == "passCurrentLow_uA"
          && testLimit.limit[LOW_LEVEL].unit().empty())
      {
        testLimit.limit[LOW_LEVEL].unit("uA");
        testLimit.use_uAAsDefaultUnit[LOW_LEVEL] = true;
      }
      else
      {
        testLimit.use_uAAsDefaultUnit[LOW_LEVEL] = false;
      }
    }
    
    if(param.isMeasure[HIGH_LEVEL])
    {
      if (testLimit.testnames[HIGH_LEVEL] == "passCurrentHigh_uA"
          && testLimit.limit[HIGH_LEVEL].unit().empty())
      {
        testLimit.limit[HIGH_LEVEL].unit("uA");
        testLimit.use_uAAsDefaultUnit[HIGH_LEVEL] = true;
      }
      else
      {
        testLimit.use_uAAsDefaultUnit[HIGH_LEVEL] = false;
      }
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
  static void doMeasurement(const LeakageTestParam& param, 
                            const LeakageTestLimit& testLimit, 
                            LeakageTestResult &result)
  {
    ON_FIRST_INVOCATION_BEGIN();
     CONNECT();
    ON_FIRST_INVOCATION_END();
    /* low measurement */
    if ( param.isMeasure[LOW_LEVEL] )
    {
      ON_FIRST_INVOCATION_BEGIN();
        FunctionalUtil::functionalPreTest( param.preFunction, param.stopCycVec[LOW_LEVEL] );
      ON_FIRST_INVOCATION_END();
      if ( param.preFunction != "NO" )
      {
        /* get the low level functional test result*/
        result.funcResult[LOW_LEVEL] = FunctionalUtil::getFunctionalTestResult();
      }

      if (param.measureMode  == "PPMUpar")
      {
        PPMU_MEASURE ppmuLowParallel;
        ON_FIRST_INVOCATION_BEGIN();
          PmuUtil::ppmuParallelCurrentMeasurement(
                                            param.pinlist,
                                            param.testMode,
                                            param.forceVoltage[LOW_LEVEL],
                                            testLimit.limit[LOW_LEVEL],
                                            param.ppmuPrecharge,
                                            param.prechargeVoltage[LOW_LEVEL],
                                            param.settlingTime[LOW_LEVEL],
                                            param.relaySwitchMode,
                                            TURN_OFF,    
                                            ppmuLowParallel);
        ON_FIRST_INVOCATION_END();       
        PmuUtil::ppmuParallelCurrentGetResult(
                                            param.expandedPins,
                                            param.testMode,
                                            ppmuLowParallel,
                                            result.measureResult[LOW_LEVEL]);    
      }
      else if (param.measureMode == "PPMUser")
      {
        PPMU_MEASURE ppmuLowSerial;
        ON_FIRST_INVOCATION_BEGIN();
           PmuUtil::ppmuSerialCurrentMeasurement(
                                              param.pinlist,
                                              param.testMode,
                                              param.forceVoltage[LOW_LEVEL],
                                              testLimit.limit[LOW_LEVEL],
                                              param.ppmuPrecharge,
                                              param.prechargeVoltage[LOW_LEVEL],
                                              param.settlingTime[LOW_LEVEL],
                                              param.relaySwitchMode,
                                              TURN_OFF,
                                              ppmuLowSerial);
        ON_FIRST_INVOCATION_END();
           PmuUtil::ppmuSerialCurrentGetResult(
                                              param.expandedPins,
                                              param.testMode,
                                              ppmuLowSerial,
                                              result.measureResult[LOW_LEVEL]);                                                    
      }
      else if (param.measureMode == "SPMUser")
      {
        SPMU_TASK spmuTaskLow;
        ON_FIRST_INVOCATION_BEGIN();
           PmuUtil::spmuSerialCurrentMeasurement(
                                              param.pinlist,
                                              param.testMode,
                                              param.forceVoltage[LOW_LEVEL],
                                              testLimit.limit[LOW_LEVEL],
                                              param.relaySwitchMode,
                                              param.spmuClampCurrent[LOW_LEVEL],
                                              param.ppmuPrecharge,
                                              param.prechargeVoltage[LOW_LEVEL],
                                              param.settlingTime[LOW_LEVEL],
                                              TURN_OFF,
                                              spmuTaskLow);
           
        ON_FIRST_INVOCATION_END();
           PmuUtil::spmuSerialCurrentGetResult(
                                              param.expandedPins,
                                              param.testMode,
                                              spmuTaskLow,
                                              result.measureResult[LOW_LEVEL]);
      }
      else
      {
        throw Error("LeakageTest::doMeasurement",
                    "Unknown measureMode.");
      }
    }

    /* high measurement */
    if ( param.isMeasure[HIGH_LEVEL] )
    {
      Boolean functionalPretestHigh = (!param.isMeasure[LOW_LEVEL]) || 
                                      (param.stopCycVec[LOW_LEVEL] != 
                                       param.stopCycVec[HIGH_LEVEL]);
      if ( functionalPretestHigh )
      {
        ON_FIRST_INVOCATION_BEGIN();
          FunctionalUtil::functionalPreTest ( param.preFunction, 
                                                   param.stopCycVec[HIGH_LEVEL] );
        ON_FIRST_INVOCATION_END();
        if ( param.preFunction != "NO" )
        {
          /* get the high level functional test*/
          result.funcResult[HIGH_LEVEL] = FunctionalUtil::getFunctionalTestResult();
        }

      }
      else if ( param.preFunction != "NO" ) 
      {
        /**
         * for the high level test, functional test will not run at current
         * condition(stopCycVec[LOW_LEVEL] equals to stopCycVec[High] and for 
         * the low level test, functional test has been executed.)
         * just give the low level function result to the high level function test
         */
        result.funcResult[HIGH_LEVEL] = result.funcResult[LOW_LEVEL];
      }
      
      if (param.measureMode  == "PPMUpar")
      {
        PPMU_MEASURE ppmuHighParallel;
        ON_FIRST_INVOCATION_BEGIN();
          PmuUtil::ppmuParallelCurrentMeasurement(
                                            param.pinlist,
                                            param.testMode,
                                            param.forceVoltage[HIGH_LEVEL],
                                            testLimit.limit[HIGH_LEVEL],
                                            param.ppmuPrecharge,
                                            param.prechargeVoltage[HIGH_LEVEL],
                                            param.settlingTime[HIGH_LEVEL],
                                            param.relaySwitchMode,
                                            TURN_OFF,    
                                            ppmuHighParallel);
        ON_FIRST_INVOCATION_END();       
        PmuUtil::ppmuParallelCurrentGetResult(
                                            param.expandedPins,
                                            param.testMode,
                                            ppmuHighParallel,
                                            result.measureResult[HIGH_LEVEL]);    
      }
      else if (param.measureMode == "PPMUser")
      {
        PPMU_MEASURE ppmuHighSerial;
        ON_FIRST_INVOCATION_BEGIN();
          PmuUtil::ppmuSerialCurrentMeasurement(
                                            param.pinlist,
                                            param.testMode,
                                            param.forceVoltage[HIGH_LEVEL],
                                            testLimit.limit[HIGH_LEVEL],
                                            param.ppmuPrecharge,
                                            param.prechargeVoltage[HIGH_LEVEL],
                                            param.settlingTime[HIGH_LEVEL],
                                            param.relaySwitchMode,
                                            TURN_OFF,
                                            ppmuHighSerial);
        ON_FIRST_INVOCATION_END();
          PmuUtil::ppmuSerialCurrentGetResult(
                                            param.expandedPins,
                                            param.testMode,
                                            ppmuHighSerial,
                                            result.measureResult[HIGH_LEVEL]);                                                  
      }
      else if (param.measureMode == "SPMUser")
      {
        SPMU_TASK spmuTaskHigh;
        ON_FIRST_INVOCATION_BEGIN();
          PmuUtil::spmuSerialCurrentMeasurement(
                                            param.pinlist,
                                            param.testMode,
                                            param.forceVoltage[HIGH_LEVEL],
                                            testLimit.limit[HIGH_LEVEL],
                                            param.relaySwitchMode,
                                            param.spmuClampCurrent[HIGH_LEVEL],
                                            param.ppmuPrecharge,
                                            param.prechargeVoltage[HIGH_LEVEL],
                                            param.settlingTime[HIGH_LEVEL],
                                            TURN_OFF,
                                            spmuTaskHigh);
         
        ON_FIRST_INVOCATION_END();
          PmuUtil::spmuSerialCurrentGetResult(
                                            param.expandedPins,
                                            param.testMode,
                                            spmuTaskHigh,
                                            result.measureResult[HIGH_LEVEL]);
      }
      else
      {
        throw Error("LeakageTest::doMeasurement",
                    "Unknown measureMode.");
      }
    }    

    if(param.preFunction == "ToStopVEC" || param.preFunction == "ToStopCYC")
    {
      ON_FIRST_INVOCATION_BEGIN();
        Sequencer.reset();
      ON_FIRST_INVOCATION_END();
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
  *           testLimit   - test Limit container
  *           result      - result container
  *           _results[4] - retrieved by built-in tf_result() in testflow.
  *   OUTPUT: 
  *   RETURN: 
  * Note:
  *----------------------------------------------------------------------*
  */ 
  static void judgeAndDatalog(const LeakageTestParam & param,
                              const LeakageTestLimit& testLimit,  
                              const LeakageTestResult &result, 
                              double _results[4])
  {
    STRING_VECTOR::size_type j = 0;
    DOUBLE dMeasValue = 0;
    /*init _results*/
    _results[0]=_results[2]= DBL_MAX; 
    _results[1]=_results[3]= -DBL_MAX;

    pair<DOUBLE,DOUBLE> minmax[TWO_LEVEL];
    minmax[LOW_LEVEL].first  =  DBL_MAX;   //low level min val
    minmax[LOW_LEVEL].second = -DBL_MAX;   //low level max val
    
    minmax[HIGH_LEVEL].first  =  DBL_MAX;   //high level min val
    minmax[HIGH_LEVEL].second = -DBL_MAX;   //high level max val

    ARRAY_D measuredValueArray(param.expandedPins.size());
    bool bPass = true;

    if ( param.isMeasure[LOW_LEVEL] ) /*judge and datalog low level measurement*/
    {
      switch ( param.testMode )
      {
      case TM::PVAL:
        {
          double factor = 1;
          if (testLimit.use_uAAsDefaultUnit[LOW_LEVEL])
          {
            factor = 1e6;
          }
 
          for ( j = 0; j < param.expandedPins.size(); ++j )
          {
            dMeasValue = result.measureResult[LOW_LEVEL].getPinsValue(param.expandedPins[j]);//[A]
            measuredValueArray[j] = dMeasValue * factor;

            minmax[LOW_LEVEL].first = MIN(minmax[LOW_LEVEL].first,dMeasValue*1e6);  //min value
            minmax[LOW_LEVEL].second = MAX(minmax[LOW_LEVEL].second,dMeasValue*1e6);//max value    
          }
          //MPR record
          if (testLimit.isLimitTableUsed)
          {
            TestSet.cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                           param.expandedPins,
                                           testLimit.testnames[LOW_LEVEL],
                                           V93kLimits::tmLimits,
                                           measuredValueArray);
          }
          else
          {
            TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                           param.expandedPins,
                                           testLimit.testnames[LOW_LEVEL],
                                           measuredValueArray);
 
          } 
        
          _results[0] = minmax[LOW_LEVEL].first;      /*low level minimum current [uA]*/
          _results[1] = minmax[LOW_LEVEL].second;     /*low level maximum current [uA]*/
        }
        break;

      case TM::PPF:
        for ( j = 0; j < param.expandedPins.size(); ++j )
        {
          bool bOnePass = result.measureResult[LOW_LEVEL].getPinPassFail(param.expandedPins[j]);
          measuredValueArray[j] = bOnePass?MPR_LOG_PF_TEST_PASS:MPR_LOG_PF_TEST_FAIL;
          bPass = bPass && bOnePass;
        }
        //Special MPR for PPF test      
        if (testLimit.isLimitTableUsed)
        {     
          V93kLimits::TMLimits::LimitInfo& limitInfo = V93kLimits::tmLimits.getLimit(
             param.testsuiteName,testLimit.testnames[LOW_LEVEL]); 
          TESTSET().cont(TM::CONTINUE).testnumber(limitInfo.TestNumber)
                 .judgeAndLog_ParametricTest(param.expandedPins,
                                             testLimit.testnames[LOW_LEVEL],
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
                                           testLimit.testnames[LOW_LEVEL],
                                           bPass?TM::Pass : TM::Fail,
                                           measuredValueArray);
 
        }
        break;

      case TM::GPF:
        bPass = result.measureResult[LOW_LEVEL].getGlobalPassFail();
        if (testLimit.isLimitTableUsed)
        {     
          V93kLimits::TMLimits::LimitInfo& limitInfo = V93kLimits::tmLimits.getLimit(
            param.testsuiteName,testLimit.testnames[LOW_LEVEL]); 
          TESTSET().cont(TM::CONTINUE).testnumber(limitInfo.TestNumber)
                 .judgeAndLog_ParametricTest(param.pinlist,
                                           testLimit.testnames[LOW_LEVEL],
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
                                           testLimit.testnames[LOW_LEVEL], 
                                           bPass?TM::Pass:TM::Fail, 
                                           0);
        }
        break;

      default:
        throw Error("LeakageTest::judgeAndDatalog",
                    "Unknown Test Mode");
      } /*end switch*/        

      if ( param.preFunction != "NO" )
      {
        TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                          param.testsuiteName,
                                          "FUNCTION TEST Low",
                                          result.funcResult[LOW_LEVEL]?TM::Pass:TM::Fail,
                                          param.stopCycVec[LOW_LEVEL]);
      }
    }

    if ( param.isMeasure[HIGH_LEVEL] ) /*judge and datalog high level measurement*/
    {
      switch ( param.testMode )
      {
      case TM::PVAL:
        {
          double factor = 1;
          if (testLimit.use_uAAsDefaultUnit[HIGH_LEVEL])
          {
            factor = 1e6;
          }

          for ( j = 0; j < param.expandedPins.size(); ++j )
          {
            dMeasValue = result.measureResult[HIGH_LEVEL].getPinsValue(param.expandedPins[j]); //[A]
            measuredValueArray[j] = dMeasValue * factor;
            
            minmax[HIGH_LEVEL].first = MIN(minmax[HIGH_LEVEL].first,dMeasValue*1e6);  //min value
            minmax[HIGH_LEVEL].second = MAX(minmax[HIGH_LEVEL].second,dMeasValue*1e6);//max value  
          }
          //MPR record
          if (testLimit.isLimitTableUsed)
          {
            TestSet.cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                           param.expandedPins,
                                           testLimit.testnames[HIGH_LEVEL],
                                           V93kLimits::tmLimits,
                                           measuredValueArray);
          }
          else
          {
            TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                           param.expandedPins,
                                           testLimit.testnames[HIGH_LEVEL],
                                           measuredValueArray);

          }
 
          _results[2] = minmax[HIGH_LEVEL].first;    /*high level minimum current [uA] */
          _results[3] = minmax[HIGH_LEVEL].second;   /*high level maximum current [uA] */
        }
        break;

      case TM::PPF:
        for ( j = 0; j < param.expandedPins.size(); ++j )
        {
          bool bOnePass = result.measureResult[HIGH_LEVEL].getPinPassFail(param.expandedPins[j]);
          measuredValueArray[j] = bOnePass?MPR_LOG_PF_TEST_PASS:MPR_LOG_PF_TEST_FAIL;
          bPass = bPass && bOnePass;
        }
        //Special MPR for PPF test      
        if (testLimit.isLimitTableUsed)
        {     
          V93kLimits::TMLimits::LimitInfo& limitInfo = V93kLimits::tmLimits.getLimit(
             param.testsuiteName,testLimit.testnames[HIGH_LEVEL]); 
          TESTSET().cont(TM::CONTINUE).testnumber(limitInfo.TestNumber)
                 .judgeAndLog_ParametricTest(param.expandedPins,
                                             testLimit.testnames[HIGH_LEVEL],
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
                                           testLimit.testnames[HIGH_LEVEL],
                                           bPass?TM::Pass : TM::Fail,
                                           measuredValueArray);
 
        }
        break;

      case TM::GPF:
        bPass = result.measureResult[HIGH_LEVEL].getGlobalPassFail();
        if (testLimit.isLimitTableUsed)
        {     
          V93kLimits::TMLimits::LimitInfo& limitInfo = V93kLimits::tmLimits.getLimit(
            param.testsuiteName,testLimit.testnames[HIGH_LEVEL]); 
          TESTSET().cont(TM::CONTINUE).testnumber(limitInfo.TestNumber)
                 .judgeAndLog_ParametricTest(param.pinlist,
                                           testLimit.testnames[HIGH_LEVEL],
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
                                           testLimit.testnames[HIGH_LEVEL], 
                                           bPass?TM::Pass:TM::Fail, 
                                           0);
        }
        break;

      default:
        throw Error("LeakageTest::judgeAndDatalog",
                    "Unknown Test Mode");
      }  /*end switch*/       

      if ( param.preFunction != "NO" )
      {
        TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                          param.testsuiteName,
                                          "FUNCTION TEST High",
                                          result.funcResult[HIGH_LEVEL]?TM::Pass:TM::Fail,
                                          param.stopCycVec[HIGH_LEVEL]);
      }
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
 *           testLimit          - test limit container
 *           result             - result container
 *           output             - "None" or "ReportUI" 
 *   OUTPUT: 
 *   RETURN:  
 * Note:
 *----------------------------------------------------------------------*
 */ 
  static void reportToUI(const LeakageTestParam& param,
                         const LeakageTestLimit& testLimit, 
                         const LeakageTestResult &result, 
                         const STRING& output)
  {

    STRING_VECTOR::size_type j = 0;
    Boolean bPass = TRUE;

    if ( param.isMeasure[LOW_LEVEL] )  /*report low level measurement result to UI window*/
    {
      CommonUtil::printTestSuiteName(output, "Leakage Low Test '", "'");

      switch ( param.testMode )
      {
      case TM::PVAL:
        {
          for ( j = 0; j < param.expandedPins.size(); ++j )
          {
            DOUBLE dMeasValue = result.measureResult[LOW_LEVEL].getPinsValue(param.expandedPins[j]);
            CommonUtil::datalogToWindow(output,
                                      param.expandedPins[j],
                                      "A",
                                      dMeasValue, 
                                      testLimit.use_uAAsDefaultUnit[LOW_LEVEL]?"uA":"",
                                      testLimit.limit[LOW_LEVEL]);
          } 
        }
        break;

      case TM::PPF:
        for ( j = 0; j < param.expandedPins.size(); ++j )
        {
          bPass = result.measureResult[LOW_LEVEL].getPinPassFail(param.expandedPins[j]);
          CommonUtil::datalogToWindow(output, param.expandedPins[j], bPass);
        }
        break;

      case TM::GPF:
        bPass = result.measureResult[LOW_LEVEL].getGlobalPassFail();
        CommonUtil::datalogToWindow(output,param.testsuiteName,bPass);
        break;

      default:
        throw Error("LeakageTest::reportToUI",
                    "Unknown Test Mode");
      }/*end switch*/

      if ( param.preFunction != "No" )
      {
        CommonUtil::datalogToWindow(output,
                                         "Functional result: ",
                                         result.funcResult[LOW_LEVEL]);

      }

    }

    if ( param.isMeasure[HIGH_LEVEL] ) /*report high level measurement result to UI window*/
    {
      CommonUtil::printTestSuiteName(output, "Leakage High Test '", "'");

      switch ( param.testMode )
      {
      case TM::PVAL:
        {
          for ( j = 0; j < param.expandedPins.size(); ++j )
          {
            DOUBLE dMeasValue = result.measureResult[HIGH_LEVEL].getPinsValue(param.expandedPins[j]);
            CommonUtil::datalogToWindow(output,
                                        param.expandedPins[j],
                                        "A",
                                        dMeasValue, 
                                        testLimit.use_uAAsDefaultUnit[HIGH_LEVEL]?"uA":"",
                                        testLimit.limit[HIGH_LEVEL]);
          }
        }
        break;

      case TM::PPF:
        for ( j = 0; j < param.expandedPins.size(); ++j )
        {
          bPass = result.measureResult[HIGH_LEVEL]
                        .getPinPassFail(param.expandedPins[j]);
          CommonUtil::datalogToWindow(output, param.expandedPins[j], bPass);
        }
        break;

      case TM::GPF:
        bPass = result.measureResult[HIGH_LEVEL].getGlobalPassFail();
        CommonUtil::datalogToWindow(output,param.testsuiteName,bPass);
        break;

      default:
        throw Error("LeakageTest::reportToUI",
                    "Unknown Test Mode");
      }/* end switch*/

      if ( param.preFunction != "No" )
      {
        CommonUtil::datalogToWindow(output,
                                         "Functional result: ", 
                                         result.funcResult[HIGH_LEVEL]);

      }
    }
  }
private:
  LeakageTest() {} //private constructor to prevent instantiation.
};    
#endif /*LEAKAGETEST_H_*/
