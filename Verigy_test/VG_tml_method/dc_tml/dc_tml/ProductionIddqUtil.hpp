#ifndef PRODUCTIONIDDQUTIL_H_
#define PRODUCTIONIDDQUTIL_H_

#include "CommonUtil.hpp"
#include "FunctionalUtil.hpp"
#include "DpsUtil.hpp"

/***************************************************************************
 *                    productionIddq test class 
 ***************************************************************************
 */  
class ProductionIddqTest
{
public:

/*
 *----------------------------------------------------------------------*
 *         test parameters container                                    *
 *----------------------------------------------------------------------*
 */ 
  struct ProductionIddqTestParam
  {
    /*original input parameters*/
    STRING dpsPins;
    STRING disconnectPins;
    DOUBLE settlingTime;
    STRING stopMode;
    STRING strStopVecCycNum;
    INT samples;
    STRING checkFunctional;
    STRING gangedMode;

    /*new generated parameter for convenience*/
    STRING_VECTOR expandedDpsPins;             /*expanded and validated pins stored in Vector*/
    
    TM::DCTEST_MODE testMode;                  /*current test mode*/ 
    STRING testsuiteName;                      /*current testsuite name*/
    ARRAY_I vectorOrCycleNumberToStopSequnecer;/*integer array of stop vector/cycle number*/
    STRING portName;

    /*initialize stuff to some defaults.*/  
    void init()
    {
      dpsPins = "";
      disconnectPins = "";
      settlingTime = 0.0;
      portName = "";
      stopMode = "UNKNOWN";
      samples = 0;
      expandedDpsPins.clear();
      testMode = TM::GPF;     
    }

    /*default constructor for intializing parameters*/
    ProductionIddqTestParam()
    {
      init();
    }
  };
  
  /*
   *----------------------------------------------------------------------*
   *         test limit container                                         *
   *----------------------------------------------------------------------*
   */ 
  struct ProductionIddqTestLimit
  {
    LIMIT limit;                               /*LIMIT object for this test*/
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
    ProductionIddqTestLimit()
    {
      init();
    }
  };

  /*
 *----------------------------------------------------------------------*
 *         test results container                                       *
 *----------------------------------------------------------------------*
 */
  struct ProductionIddqTestResult
  {
    /*result container per site:
    *for value, the unit is: A
    */
    MeasurementResultContainer *dpsResult;
    Boolean *funcResult;  /*true: PASS, false: FAIL*/  

    /*initialize all stuffs to some defaults.*/
    void init()
    {
      delete[] dpsResult;
      dpsResult = NULL;
      delete[] funcResult;
      funcResult = NULL;
    }

    /*default constructor*/
    ProductionIddqTestResult()
    {
      dpsResult = NULL;
      funcResult = NULL;      
    }

/*destructor*/
    ~ProductionIddqTestResult()
    {
      delete[] dpsResult;
      dpsResult = NULL;
      delete[] funcResult;
      funcResult = NULL;
    }
  };



/*
 *----------------------------------------------------------------------*
 *         public interfaces of production iddq test                    *
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
                                const STRING& disconnectPins,
                                const DOUBLE settlingTime,
                                const STRING& stopMode,
                                const STRING& strStopVecCycNum,
                                const INT samples,
                                const STRING& checkFunctional,
                                const STRING& gangedMode,
                                ProductionIddqTestParam& param)
  {
    param.init();
    param.checkFunctional = CommonUtil::trim(checkFunctional);
    /*check the range of settling time*/
    if ( settlingTime < 0.0 )
    {
      throw Error("ProductionIddqTest::processParameters()",
                  "settlingTime must be postive!");
    }
    param.settlingTime = settlingTime;
    
    
    param.testMode = CommonUtil::getMode();
    GET_TESTSUITE_NAME(param.testsuiteName);
  
    CONNECT();
    param.dpsPins = CommonUtil::trim(dpsPins);
    param.expandedDpsPins = PinUtility.getDpsPinNamesFromPinList(param.dpsPins,TRUE);

    /*
     ********************************************************* 
     * Validate disconnectPins if it's not empty, 
     * and meanwhile it detects DC pins, 
     * if any DC pins exist, it throws an error. 
     *********************************************************
     */
    param.disconnectPins = CommonUtil::trim(disconnectPins);
    if ( !param.disconnectPins.empty() )
    {
      PinUtility.getDigitalPinNamesFromPinList(param.disconnectPins,
                                               TM::I_PIN|TM::O_PIN|TM::IO_PIN,
                                               TRUE, /* check missing pins */
                                               TRUE  /* check mismatched-type pins*/
                                               );
    }
    /*
     * Convert string of stop number into integer array.
     */

    param.strStopVecCycNum = CommonUtil::trim(strStopVecCycNum);
    param.stopMode = CommonUtil::trim(stopMode);
    STRING portData;
    if ( !param.strStopVecCycNum.empty() )
    {
      CommonUtil::splitPortDataAndPortName(param.strStopVecCycNum,
                                           portData,
                                           param.portName,
                                           false);
      CommonUtil::validateAndConvertStrToInt(portData,
                                             param.vectorOrCycleNumberToStopSequnecer);
    }
    else
    {
      /*
       * If input of vector/cycle number is empty, change the stopMode
       * to "ALL" in order to run to end of the label.
       */
      param.stopMode = "ALL";
      param.vectorOrCycleNumberToStopSequnecer.resize(1);
      param.vectorOrCycleNumberToStopSequnecer[0] = 0;
    }
    param.samples = samples;
    param.gangedMode = CommonUtil::trim(gangedMode);            
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
    const bool testnameHasUnit, ProductionIddqTestLimit& testLimit)
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
 *           testLimt    - test limit container
 *           result      - result container
 *
 *   OUTPUT: 
 *   RETURN: 
 *----------------------------------------------------------------------*
 */
  static void doMeasurement(const ProductionIddqTestParam& param, 
                            const ProductionIddqTestLimit& testLimit, 
                            ProductionIddqTestResult& result)
  {
    DPS_TASK dpsTask;
    INT stopNumSize = param.vectorOrCycleNumberToStopSequnecer.size();
    
    result.dpsResult = new MeasurementResultContainer[stopNumSize];
    result.funcResult = new Boolean[stopNumSize];
    
    ON_FIRST_INVOCATION_BEGIN();
      /* setup dpsTask */
      DpsUtil::setDpsTask(param.dpsPins,
                          testLimit.limit,
                          param.samples,
                          param.gangedMode,
                          param.testMode,
                          dpsTask,
                          param.settlingTime);
    ON_FIRST_INVOCATION_END();            

    /*
     * Do Measurements:
     * Measure the current at different specified vector or cycle. 
     */
    for ( INT index = 0; index < stopNumSize; ++index )
    {
      ON_FIRST_INVOCATION_BEGIN(); 
        /*set sequencer's stop mode and run functional test*/
        FunctionalUtil::functionalPreTest(
                                 param.stopMode,
                                 param.vectorOrCycleNumberToStopSequnecer[index],
                                 param.portName);
      ON_FIRST_INVOCATION_END(); 
      /*check functional test result */
      if ( param.checkFunctional == TURN_ON )
      {
        result.funcResult[index] = FunctionalUtil::getFunctionalTestResult();
      }

      ON_FIRST_INVOCATION_BEGIN(); 
        /*disconnect pins before measurement, if any*/
        CommonUtil::disconnectPins(param.disconnectPins);
  
        /*do current measurement using dps task*/
        DpsUtil::currentMeasurement(dpsTask);
      ON_FIRST_INVOCATION_END(); 

      /* get measurement results*/

      DpsUtil::currentGetResult(param.expandedDpsPins,
                                param.testMode,
                                dpsTask,
                                result.dpsResult[index],
                                param.gangedMode);    
      ON_LAST_INVOCATION_BEGIN(); 
        CommonUtil::connectPins(param.disconnectPins);
        if(param.stopMode == "ToStopVEC" || param.stopMode == "ToStopCYC")
        {
          Sequencer.reset();
          FLUSH();
        } 
      ON_LAST_INVOCATION_END() ;
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
 *   judge results of 'result' with 'testLimit' from 'param'
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
  static void judgeAndDatalog(const ProductionIddqTestParam& param, 
                              const ProductionIddqTestLimit& testLimit, 
                              const ProductionIddqTestResult& result, 
                              double _results[4])
  {
    INT  stopNumSize = param.vectorOrCycleNumberToStopSequnecer.size();
    Boolean isPass = true;
    STRING_VECTOR::const_iterator it;
    double dVal = 0.0;
    double dSumVal = 0.0;
    /*init _results*/
    _results[0]=_results[2]= DBL_MAX; 
    _results[1]=_results[3]= -DBL_MAX;
    
    pair<DOUBLE,DOUBLE> minmax;
    minmax.first  = DBL_MAX;  //min val
    minmax.second = -DBL_MAX;//max val

    double factor_ToDefaultUnit = 1;
    if (testLimit.use_uAAsDefaultUnit)
    {
      factor_ToDefaultUnit = 1e6;
    }
 
    for ( INT index = 0; index < stopNumSize; ++index )
    {
      dSumVal = 0.0;

      if ( param.gangedMode == TURN_ON )
      { /* for gang mode case */
        for ( it = param.expandedDpsPins.begin(); it != param.expandedDpsPins.end(); ++it )
        {
          dSumVal += result.dpsResult[index].getPinsValue( *it );
        }
        dSumVal = dSumVal * factor_ToDefaultUnit;
        isPass = testLimit.limit.pass(dSumVal);

        switch ( param.testMode )
        {
        case TM::PVAL:
          minmax.first  = MIN(minmax.first,  result.dpsResult[index].getMinValue());
          minmax.second = MAX(minmax.second, result.dpsResult[index].getMaxValue());
          if (testLimit.isLimitTableUsed)
          {
            TestSet.cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                           param.dpsPins,
                                           testLimit.testname,
                                           V93kLimits::tmLimits,
                                           dSumVal);
          }
          else
          {
            TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                           param.dpsPins,
                                           testLimit.testname,
                                           dSumVal);
 
          }               
          break;

        case TM::PPF: // for ganged mode, each pin has same pass/fail result which is as same as GPF mode
        case TM::GPF:
          if (testLimit.isLimitTableUsed)
          {
            V93kLimits::TMLimits::LimitInfo& limitInfo = V93kLimits::tmLimits.getLimit(
              param.testsuiteName,testLimit.testname);
            TESTSET().cont(TM::CONTINUE).testnumber(limitInfo.TestNumber)
                   .judgeAndLog_ParametricTest(param.dpsPins,
                                           testLimit.testname,
                                           isPass?TM::Pass : TM::Fail,
                                           0);
            if (!isPass&& !limitInfo.BinsNumString.empty())
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
          throw Error("ProductionIddqTest::judgeAndDatalog",
                      "Unknown Measure Mode");
        }
      }
      else
      { /*for ungang mode*/
        ARRAY_D measuredValueArray(param.expandedDpsPins.size());
        switch ( param.testMode )
        {
        case TM::PVAL:
          for ( STRING_VECTOR::size_type j = 0; j < param.expandedDpsPins.size(); ++j )
          {
            dVal = result.dpsResult[index].getPinsValue(param.expandedDpsPins[j] );
            measuredValueArray[j] = dVal * factor_ToDefaultUnit;
            minmax.first  = MIN(minmax.first,dVal);  
            minmax.second = MAX(minmax.second,dVal);
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
          break;
        case TM::PPF:
          for ( STRING_VECTOR::size_type j = 0; j < param.expandedDpsPins.size(); ++j )
          {
            bool isOnePass = result.dpsResult[index].getPinPassFail( param.expandedDpsPins[j] );
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
          isPass = result.dpsResult[index].getGlobalPassFail();
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
          throw Error("ProductionIddqTest::judgeAndDatalog",
                      "Unknown Measure Mode");
        }  /*end of switch*/
      } 

      if ( param.checkFunctional == TURN_ON )
      {
        if (testLimit.isLimitTableUsed)
        {
          V93kLimits::TMLimits::LimitInfo& limitInfo = 
            V93kLimits::tmLimits.getLimit(param.testsuiteName,testLimit.testname);
          bool pass = TESTSET().cont(TM::CONTINUE)
                               .testnumber(limitInfo.TestNumber)
                               .testname(testLimit.testname)
                               .combineMultiPortTests(true)
                               .judgeAndLog_FunctionalTest(result.funcResult[index]);
          if(!pass && !limitInfo.BinsNumString.empty() > 0)
          {
            SET_MULTIBIN(limitInfo.BinsNumString,limitInfo.BinhNum);
          }
        }
        else
        {
          TESTSET().cont(true)
                   .testnumber(testLimit.testname,true)
                   .combineMultiPortTests(true)
                   .judgeAndLog_FunctionalTest(result.funcResult[index]);
        }
      }
    } 
     /* Fill in _result[] for tf_result access */
    _results[0] = minmax.first*1e6; // min current [uA]
    _results[1] = minmax.second*1e6;// max current [uA]
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
 *            output             - "None" or "ReportUI" 
 *   OUTPUT: 
 *   RETURN:  
 * Note:
 *----------------------------------------------------------------------*
 */ 
  static void reportToUI(const ProductionIddqTestParam& param, 
                         const ProductionIddqTestLimit& testLimit, 
                         const ProductionIddqTestResult& result, 
                         const STRING& output)
  {
    INT  stopNumSize = param.vectorOrCycleNumberToStopSequnecer.size();
    Boolean isPass;
    double dVal = 0.0;
    double dSumVal = 0.0;
    STRING dpsPinsString = "";
    STRING_VECTOR::const_iterator it;

    for ( INT index = 0; index < stopNumSize; ++index )
    {
      CommonUtil::printTestSuiteName(output, 
                                          "production Iddq current '", 
                                          "'");
      dSumVal = 0.0;

      if ( param.gangedMode == TURN_ON )
      { /* for gang mode case */
        for ( it = param.expandedDpsPins.begin(); it != param.expandedDpsPins.end(); ++it )
        {
          dSumVal += result.dpsResult[index].getPinsValue((*it));
        }
        switch ( param.testMode )
        {
        case TM::PVAL:
          dpsPinsString = PinUtility.createPinListFromPinNames(param.expandedDpsPins);   
          CommonUtil::datalogToWindow(output, 
                                      "sum of " + dpsPinsString, 
                                      "A",
                                      dSumVal, 
                                      testLimit.use_uAAsDefaultUnit?"uA":"",
                                      testLimit.limit);
          break;

        case TM::PPF:
          isPass = testLimit.limit.pass(dSumVal);
          /*judge the sum of current if pass or fail in gang mode*/
          for ( it = param.expandedDpsPins.begin(); it != param.expandedDpsPins.end(); ++it )
          {

            CommonUtil::datalogToWindow(output, *it, isPass);
          } 
          break;

        case TM::GPF:
          isPass = testLimit.limit.pass(dSumVal);          
          CommonUtil::datalogToWindow(output, param.testsuiteName, isPass);      
          break;

        default:
          throw Error("ProductionIddqTest::reportToUI" , "Unknown Measure Mode");
        }
      }
      else
      { /*for ungang mode*/
        switch ( param.testMode )
        {
        case TM::PVAL:
          for ( it = param.expandedDpsPins.begin(); it != param.expandedDpsPins.end(); ++it )
          {
            dVal = result.dpsResult[index].getPinsValue((*it));
            CommonUtil::datalogToWindow(output, 
                                        *it,
                                        "A", 
                                        dVal, 
                                        testLimit.use_uAAsDefaultUnit?"uA":"",
                                        testLimit.limit); 
          }
          break;
        case TM::PPF:
          for ( vector<string>::size_type iPin = 0;
                iPin < param.expandedDpsPins.size(); 
                iPin++)
          {
            const string& pin = param.expandedDpsPins[iPin];
            isPass = result.dpsResult[index].getPinPassFail(pin);
            CommonUtil::datalogToWindow( output, pin, isPass );
          }
          break;
        case TM::GPF:
          isPass = result.dpsResult[index].getGlobalPassFail();         
          CommonUtil::datalogToWindow(output, param.testsuiteName, isPass);  
          break;
        default:
          throw Error("ProductionIddqTest::reportToUI",
                      "Unknown Measure Mode");
        }  /*end of switch*/
      } /*end of if-else*/

      if ( param.checkFunctional == TURN_ON )
      {
        CommonUtil::datalogToWindow(output,"Functional result: ", result.funcResult[index]);
      }
    } 
  }
private:
  ProductionIddqTest() {}  //private constructor to prevent instantiation.
};                              

#endif /*PRODUCTIONIDDQTEST_H_*/
