#ifndef CONTINUITYUTIL_H_
#define CONTINUITYUTIL_H_

#include "CommonUtil.hpp"
#include "DpsUtil.hpp"
#include "PmuUtil.hpp"

/***************************************************************************
 *                    continuity test class 
 ***************************************************************************
 */ 
class ContinuityTest
{
public:

  enum
  {
    TWO_POL = 2
  };  /*two polarities of continuity test*/
  enum MeasuredPol
  {
    FIRST_POL = 0, SECOND_POL = 1
  };

  /*
   *----------------------------------------------------------------------*
   *         test parameters container                                    *
   *----------------------------------------------------------------------*
   */   
  struct ContinuityTestParam
  {
    /*original input parameters*/
    STRING pinlist;
    DOUBLE forceCurrent[TWO_POL];
    DOUBLE settlingTime;
    STRING measurementMode;
    STRING polarity;

    /*new generated parameter for convenience*/
    STRING testsuiteName;         /*current testsuite name*/
    STRING_VECTOR expandedPins;   /*expanded and validated pins stored in Vector*/  
    TM::DCTEST_MODE testMode;     /*current test mode*/

    /*initialize stuff to some defaults.*/
    void init()
    {
      pinlist = "";
      forceCurrent[FIRST_POL] = 0.0;
      forceCurrent[SECOND_POL] = 0.0;
      settlingTime = 0.0;
      measurementMode = "UNKNOWN";
      expandedPins.clear();     
      polarity = "";
      testMode = TM::GPF;
    }

    /*default constructor for intializing parameters*/
    ContinuityTestParam()
    {
      init();
    }               
  };
  
  /*
   *----------------------------------------------------------------------*
   *         test limit(s) container                                      *
   *----------------------------------------------------------------------*
   */ 
  struct ContinuityTestLimit
  {
    LIMIT limit[TWO_POL];         /*LIMIT object arrays*/ 
    string testname; 
    bool isLimitTableUsed;
    //the unit is contained in testname (i.e. mV) 
    bool use_mVAsDefaultUnit;

    /*initialize limit(s) to some defaults.*/ 
    void init()
    {
      limit[FIRST_POL] = TM::Fail;
      limit[SECOND_POL] = TM::Fail;
      testname.clear(); 
      isLimitTableUsed = true;
      use_mVAsDefaultUnit = true;
    }
    
    /*default constructor */
    ContinuityTestLimit()
    {
      init();
    }
  };

  /*
   *----------------------------------------------------------------------*
   *         test results container                                       *
   *----------------------------------------------------------------------*
   */
  struct ContinuityTestResult
  {
    /*result container 
     *for voltage value, the unit is: V
     */
    MeasurementResultContainer measurementResult[TWO_POL];

    /*initialize all stuffs to some defaults.*/
    void init()
    {
      measurementResult[FIRST_POL].init();
      measurementResult[SECOND_POL].init();
    }

    /*default constructor*/
    ContinuityTestResult()
    {
      init();
    }
  };
  
  
  

 /*
 *----------------------------------------------------------------------*
 *         public interfaces of continuity test                         * 
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
                                const DOUBLE forceCurrent,
                                const DOUBLE settlingTime,
                                const STRING& measurementMode,
                                const STRING& polarity,
                                ContinuityTestParam& param)
  {
    /*initial param*/
    param.init();
    param.measurementMode = measurementMode;
    if ( settlingTime < 0.0 )
    {
      throw Error("ContinuityTest::processParameters",
                  "settlingTime must be postive!");
    }

    param.settlingTime = settlingTime;
    /*
     ********************************************************* 
     * Validate and expand pin names from input pinlist, 
     * and meanwhile it detects DC pins, 
     * if any DC pins exist, it throws an error. 
     *********************************************************
     */

    param.pinlist = pinlist;
    param.expandedPins = PinUtility.getDigitalPinNamesFromPinList(
                                             param.pinlist,
                                             TM::I_PIN|TM::O_PIN|TM::IO_PIN,
                                             true, /* check wrong type pins */
                                             true  /* check missing pins */
                                             );
                                                                 
    param.testMode = CommonUtil::getMode();
    GET_TESTSUITE_NAME(param.testsuiteName);
     
    param.forceCurrent[FIRST_POL] = forceCurrent;

    param.polarity = CommonUtil::trim(polarity);
    if ( param.polarity == "BPOL" )
    {
      param.forceCurrent[SECOND_POL] = -forceCurrent;               
    }

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
  static void processLimit(const ContinuityTestParam& param,
                           const string& testname,
                           const bool testnameHasDefaultUnit, 
                           ContinuityTestLimit& testLimit)
  {    
    /*init limit*/
    testLimit.init();
    testLimit.testname = CommonUtil::trim(testname);
   
    //if limit CSV file is load, get limit from it;otherwise get limit from testflow
    TesttableLimitHelper ttlHelper(param.testsuiteName);
    if (ttlHelper.isLimitCsvFileLoad())
    {
      ttlHelper.getLimit(testLimit.testname,testLimit.limit[FIRST_POL]);
    }

    testLimit.isLimitTableUsed = ttlHelper.isAllInTable();
    
    if (!testLimit.isLimitTableUsed)
    {
      testLimit.limit[FIRST_POL] = GET_LIMIT_OBJECT(testLimit.testname);
    }
    
    //if the testname is default one containing unit, e.g passVolt_mV
    if (testnameHasDefaultUnit && testLimit.limit[FIRST_POL].unit().empty())
    {
      testLimit.use_mVAsDefaultUnit = true;
      testLimit.limit[FIRST_POL].unit("mV");
    }
    else
    {
      testLimit.use_mVAsDefaultUnit = false;
    }    
 
    //limit's value should be adapted to the polarity of forceCurrent 
    processLimitForSinglePolarity(testLimit.limit[FIRST_POL],
                                  param.forceCurrent[FIRST_POL]);
    //process limit for 2nd Polarity
    if(param.polarity == "BPOL")
    {
      testLimit.limit[SECOND_POL] = testLimit.limit[FIRST_POL]; 
      processLimitForSinglePolarity(testLimit.limit[SECOND_POL], 
                                    param.forceCurrent[SECOND_POL]);
    }    
  }
/*
 *----------------------------------------------------------------------*
 * Routine: doMeasurement
 *
 * Purpose: execute measurement by PPMU or Program Load and store results
 *
 *----------------------------------------------------------------------*
 * Description:
 *   INPUT:  param       - test parameters
 *           testLimit   - test limit(s) container
 *           result      - result container
 *
 *   OUTPUT: 
 *   RETURN: 
 *----------------------------------------------------------------------*
 */
  static void doMeasurement(const ContinuityTestParam& param,
                            const ContinuityTestLimit& testLimit, 
                            ContinuityTestResult& result)
  {
    PPMU_MEASURE ppmuMeasureResult[TWO_POL];
    
    ON_FIRST_INVOCATION_BEGIN();
      /*init result*/
      result.init();
      DISCONNECT();
      /*
       *********************************
       * Do measurements
       ********************************* 
       */ 
      if ( param.measurementMode == "PPMUpar" )  /* PPMU parallel */
      {
        ppmuParallelVoltageMeasurement(param, 
                                       testLimit,
                                       ppmuMeasureResult[FIRST_POL],
                                       ppmuMeasureResult[SECOND_POL]);
      }
      else if ( param.measurementMode == "ProgLoad" ) /* Program Load*/
      {
        programmableLoad(param, 
                         testLimit, 
                         result.measurementResult[SECOND_POL]);
      }
      else
      {
        throw Error("ContinuityTest::doMeasurement",
                    "Unknown measurement mode for continuity.");
      }
    ON_FIRST_INVOCATION_END();

    /*
     *************************************
     * Get the results
     *************************************
     */
    if ( param.measurementMode == "PPMUpar" ) 
    { /* PPMU parallel */ 
      PmuUtil::ppmuParallelVoltageGetResult(
                                        param.expandedPins,
                                        param.testMode,
                                        ppmuMeasureResult[FIRST_POL],
                                        result.measurementResult[FIRST_POL]
                                        );
      if ( param.polarity == "BPOL" )
      {
        PmuUtil::ppmuParallelVoltageGetResult(
                                        param.expandedPins,
                                        param.testMode,
                                        ppmuMeasureResult[SECOND_POL],
                                        result.measurementResult[SECOND_POL]
                                        );
      }
    }
    else if ( param.measurementMode == "ProgLoad" ) 
    {
       /* Programmable Load:
        * result has been got and processed in measurement phase 
        * for better performance.
        * noting to be done here.
        */
    }
    else
    {
      throw Error("ContinuityTest::doMeasurement",
                  "Unknown measurement mode for continuity.");
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
 *   INPUT:  param       - test parameters container
 *           testLimit   - test limit container
 *           result      - result container
 *           _results[4] - retrieved by built-in tf_result() in testflow.
 *   OUTPUT: 
 *   RETURN: 
 * Note:
 *----------------------------------------------------------------------*
 */
  static void judgeAndDatalog(const ContinuityTestParam& param,
                              const ContinuityTestLimit& testLimit, 
                              const ContinuityTestResult &result,
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
    minmax.second = -DBL_MAX; //max val

    ARRAY_D measuredValueArray(param.expandedPins.size());
    double factor_V2mV = 1.0;
    if (testLimit.use_mVAsDefaultUnit)
    {
      factor_V2mV = 1e3; 
    }

    if ( param.measurementMode == "PPMUpar" )
    { /* PPMU parallel */ 

      /*
       ****************************
       *  single polarity
       ****************************
       */
      if ( param.polarity == "SPOL" )
      {
        switch ( param.testMode )
        {
        case TM::PVAL:
          for ( j = 0; j< param.expandedPins.size(); ++j )
          {
            dMeasValue = result.measurementResult[FIRST_POL]
                               .getPinsValue(param.expandedPins[j]); //[V]
            measuredValueArray[j] = factor_V2mV * dMeasValue;
            //for _results[4] which is 'mV' based
            dMeasValue = dMeasValue * 1e3;                //[mV]
            minmax.first = MIN(minmax.first,dMeasValue);  //min value
            minmax.second = MAX(minmax.second,dMeasValue);//max value
          }
          //MPR record
          if (testLimit.isLimitTableUsed)
          {
            //because the limit has been adapted to ploarity of forceCurrent
            //a special datalog is used instead of direct testtable limit:
            V93kLimits::TMLimits::LimitInfo& limitInfo = V93kLimits::tmLimits.getLimit(
              param.testsuiteName,testLimit.testname);
            bPass = TESTSET().cont(TM::CONTINUE).testnumber(limitInfo.TestNumber)
                           .judgeAndLog_ParametricTest(param.expandedPins,
                                               testLimit.testname,
                                               testLimit.limit[FIRST_POL],
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
                                           testLimit.limit[FIRST_POL],
                                           measuredValueArray);
 
          } 
          break;

        case TM::PPF:
          
          for ( j = 0; j < param.expandedPins.size(); ++j )
          {
            bool bOnePass = result.measurementResult[FIRST_POL]
                                  .getPinPassFail(param.expandedPins[j]);
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
          bPass = result.measurementResult[FIRST_POL].getGlobalPassFail();
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
                                           bPass?TM::Pass:TM::Fail, 
                                           0);
          }
          break;

        default:
          throw Error("ContinuityTest::judgeAndDatalog",
                      "Unknown Test Mode");                                                                               
        } /*end switch*/
      }
      else if ( param.polarity == "BPOL" )
      {
        /*
         *********************************************
         * Both polarity:
         * combine the two measurements with different
         * polarities.
         * If both pass, total results pass.
         *********************************************
         */
        DOUBLE dMeasValue1 = 0;
        DOUBLE dMeasValue2 = 0;
        Boolean bPass1 = true;
        Boolean bPass2 = true;   
        ARRAY_D measuredValueArray2(param.expandedPins.size());
        switch ( param.testMode )
        {
        case TM::PVAL:
          for ( j = 0; j< param.expandedPins.size(); ++j )
          {
            dMeasValue1 = result.measurementResult[FIRST_POL]
                                .getPinsValue(param.expandedPins[j]);
            dMeasValue2 = result.measurementResult[SECOND_POL]
                                .getPinsValue(param.expandedPins[j]);
            measuredValueArray[j] = factor_V2mV * dMeasValue1;
            measuredValueArray2[j] = factor_V2mV *dMeasValue2;

            dMeasValue1 = dMeasValue1 * 1e3;               //[mV]  
            dMeasValue2 = dMeasValue2 * 1e3; 
            minmax.first = MIN(minmax.first,dMeasValue1);  //min value
            minmax.first = MIN(minmax.first,dMeasValue2);  //min value
            minmax.second = MAX(minmax.second,dMeasValue1);//max value
            minmax.second = MAX(minmax.second,dMeasValue2);//max value
          }
          //MPR record
          if (testLimit.isLimitTableUsed)
          {
            V93kLimits::TMLimits::LimitInfo& limitInfo = V93kLimits::tmLimits.getLimit(
              param.testsuiteName,testLimit.testname); 
            //1st polarity use normal datalog API:
            //because the limit has been adapted to ploarity of forceCurrent
            //a special datalog is used instead of direct testtable limit:
            bool bPass1 = TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                           param.expandedPins,
                                           testLimit.testname,
                                           testLimit.limit[FIRST_POL],
                                           measuredValueArray);

            if (!bPass1 && !limitInfo.BinsNumString.empty())
            {
              SET_MULTIBIN(limitInfo.BinsNumString,limitInfo.BinhNum);
            }

            //for 2nd polarity: because limit in testtable is specified for 1st polarity test
            //a special datalog is needed for 2nd polarity test:
            bool bPass2 = TESTSET().cont(TM::CONTINUE).testnumber(limitInfo.TestNumber)
                                 .judgeAndLog_ParametricTest(param.expandedPins,
                                               testLimit.testname,
                                               testLimit.limit[SECOND_POL],
                                               measuredValueArray2);
            if (bPass1 && !bPass2 && !limitInfo.BinsNumString.empty())
            {
              SET_MULTIBIN(limitInfo.BinsNumString,limitInfo.BinhNum);
            } 
          }
          else
          {
            //1st polarity test datalog  
            TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                           param.expandedPins,
                                           testLimit.testname,
                                           testLimit.limit[FIRST_POL],
                                           measuredValueArray);
            //2nd polarity test datalog  
            TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                           param.expandedPins,
                                           testLimit.testname,
                                           testLimit.limit[SECOND_POL],
                                           measuredValueArray2);
          }
          break;

        case TM::PPF:
          for ( j = 0; j < param.expandedPins.size(); ++j )
          {
            bPass1 = result.measurementResult[FIRST_POL]
                           .getPinPassFail(param.expandedPins[j]);
            bPass2 = result.measurementResult[SECOND_POL]
                           .getPinPassFail(param.expandedPins[j]);
            bool combinePass = bPass1 && bPass2;
            measuredValueArray[j] = combinePass?MPR_LOG_PF_TEST_PASS:MPR_LOG_PF_TEST_FAIL;
            bPass = bPass && combinePass;
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
          bPass1 = result.measurementResult[FIRST_POL].getGlobalPassFail();
          bPass2 = result.measurementResult[SECOND_POL].getGlobalPassFail();
          bPass = bPass1 && bPass2;
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
                                           bPass?TM::Pass:TM::Fail, 
                                           0);
          }
          break;

        default:
          throw Error("ContinuityTest::judgeAndDatalog",
                      "Unknown Test Mode");
        } /*end switch*/

      }
      _results[0]=minmax.first; //minimum voltage [mV] 
      _results[1]=minmax.second;//maximum voltage [mv]
    }
    else if ( param.measurementMode == "ProgLoad" )
    {
      INT nSite = CURRENT_SITE_NUMBER();
      bPass = true;
       /* Programmable Load */
      switch ( param.testMode )
      {
      case TM::PVAL: /* same as PPF */
      case TM::PPF:
        for ( j = 0; j < param.expandedPins.size(); ++j )
        {
          bool bOnePass = result.measurementResult[SECOND_POL]
                        .getPinPassFail(param.expandedPins[j],nSite);
          measuredValueArray[j] = bOnePass?MPR_LOG_PF_TEST_PASS:MPR_LOG_PF_TEST_FAIL;
          bPass = bPass && bOnePass;
        }
        //Special MPR for PVAL/PPF test with programmable load instrument
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
        bPass = result.measurementResult[SECOND_POL].getGlobalPassFail(nSite);
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
                                           bPass?TM::Pass:TM::Fail,
                                           0);
        }
        break;

      default:
        throw Error("continuityTest::judgeAndDatalog",
                    "Unknown Test Mode");
      } /*end switch*/
    }
    else
    {
      throw Error("ContinuityTest::judgeAndDatalog",
                  "Unknown measurement mode for continuity.");
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
 *           testLimit          - test limit(s) container
 *           output             - "None" or "ReportUI" 
 *           result             - result container
 *   OUTPUT: 
 *   RETURN:  
 * Note:
 *----------------------------------------------------------------------*
 */
  static void reportToUI(const ContinuityTestParam& param,
                         const ContinuityTestLimit& testLimit, 
                         const ContinuityTestResult& result,
                         const STRING& output)
  {
    CommonUtil::printTestSuiteName(output, "Continuity '", "'");

    STRING_VECTOR::size_type j = 0;
    Boolean bPass = TRUE;

    if ( param.measurementMode == "PPMUpar" )
    { /* PPMU parallel */ 

      /*
       ****************************
       *  single polarity
       ****************************
       */
      if ( param.polarity == "SPOL" )
      {
        switch ( param.testMode )
        {
        case TM::PVAL:
          {
            for ( j = 0; j< param.expandedPins.size(); ++j )
            {
              DOUBLE dMeasValue = result.measurementResult[FIRST_POL]
                                        .getPinsValue(param.expandedPins[j]); /*[V]*/
              CommonUtil::datalogToWindow(output,
                                        param.expandedPins[j],
                                        "V", 
                                        dMeasValue,
                                        testLimit.use_mVAsDefaultUnit?"mV":"",
                                        testLimit.limit[FIRST_POL]);
            }
          }
          break;

        case TM::PPF:

          for ( j = 0; j < param.expandedPins.size(); ++j )
          {
            bPass = result.measurementResult[FIRST_POL]
                          .getPinPassFail(param.expandedPins[j]);
            CommonUtil::datalogToWindow(output, param.expandedPins[j], bPass);
          }
          break;

        case TM::GPF:
          
          bPass = result.measurementResult[FIRST_POL].getGlobalPassFail();
          CommonUtil::datalogToWindow(output,param.testsuiteName, bPass);
          break;

        default:
          throw Error("ContinuityTest::reportToUI",
                      "Unknown Test Mode");
        }/* end switch*/
      }
      else if ( param.polarity == "BPOL" )
      {
        /*
         *********************************************
         * Both polarity:
         * combine the two measurements with different
         * polarities.
         * If both pass, total results pass.
         *********************************************
         */
        DOUBLE dMeasValue1 = 0;
        DOUBLE dMeasValue2 = 0;
        Boolean bPass1 = true;
        Boolean bPass2 = true;   
        switch ( param.testMode )
        {
        case TM::PVAL:
          {
            for ( j = 0; j< param.expandedPins.size(); ++j )
            {
              dMeasValue1 = result.measurementResult[FIRST_POL]
                                  .getPinsValue(param.expandedPins[j]);
              dMeasValue2 = result.measurementResult[SECOND_POL]
                                  .getPinsValue(param.expandedPins[j]);
              CommonUtil::datalogToWindow(output,
                                        param.expandedPins[j],
                                        "V", 
                                        dMeasValue1,
                                        testLimit.use_mVAsDefaultUnit?"mV":"",
                                        testLimit.limit[FIRST_POL]);
   
              CommonUtil::datalogToWindow(output,
                                        param.expandedPins[j], 
                                        "V",
                                        dMeasValue2,
                                        testLimit.use_mVAsDefaultUnit?"mV":"",
                                        testLimit.limit[SECOND_POL]);
            }
          }
          break;

        case TM::PPF:

          for ( j = 0; j < param.expandedPins.size(); ++j )
          {
            bPass1 = result.measurementResult[FIRST_POL]
                           .getPinPassFail(param.expandedPins[j]);
            bPass2 = result.measurementResult[SECOND_POL]
                           .getPinPassFail(param.expandedPins[j]);
            bPass = bPass1 && bPass2;
            CommonUtil::datalogToWindow(output, param.expandedPins[j], bPass);        
          }
          break;

        case TM::GPF:
        
          bPass1 = result.measurementResult[FIRST_POL].getGlobalPassFail();
          bPass2 = result.measurementResult[SECOND_POL].getGlobalPassFail();
          bPass = bPass1 && bPass2;
          CommonUtil::datalogToWindow(output,param.testsuiteName, bPass);
          break;

        default:
          throw Error("ContinuityTest::reportToUI",
                      "Unknown Test Mode");
        }/* end switch*/
      }
    }
    else if ( param.measurementMode == "ProgLoad" )
    { 
      INT nSite = CURRENT_SITE_NUMBER();
      /* Programmable Load */
      switch ( param.testMode )
      {
      case TM::PVAL: /* same as PPF */
      case TM::PPF:

        for ( j = 0; j < param.expandedPins.size(); ++j )
        {
          bPass = result.measurementResult[SECOND_POL]
                        .getPinPassFail(param.expandedPins[j],nSite);
          CommonUtil::datalogToWindow(output, param.expandedPins[j], bPass);
        }
        break;

      case TM::GPF:

        bPass = result.measurementResult[SECOND_POL].getGlobalPassFail(nSite);
        CommonUtil::datalogToWindow(output,param.testsuiteName, bPass);
        break;

      default:
        throw Error("ContinuityTest::reportToUI",
                    "Unknown Test Mode");
      }/* end switch*/

    }
    else
    {
      throw Error("ContinuityTest::reportToUI",
                  "Unknown measurement mode for continuity.");
    }
  }

private:
  ContinuityTest() {} //private constructor to prevent instantiation.
  
  static void processLimitForSinglePolarity(
                 LIMIT& limit,  
                 const DOUBLE forceCurrent);
                 
  static void ppmuParallelVoltageMeasurement(
                 const ContinuityTestParam& param,
                 const ContinuityTestLimit& testLimit,
                 PPMU_MEASURE&              measureResultFirst,
                 PPMU_MEASURE&              measureResultSecond);
  static void programmableLoad(
                 const ContinuityTestParam& param,
                 const ContinuityTestLimit& testLimit,
                 MeasurementResultContainer& continResult);

  static void parseAndRecordForPPF(
                 const INT               pinsSize,
                 const STRING&           fwAnswer,
                 MeasurementResultContainer& continResult);

  static void parseAndRecordPRLTResult(
                 const STRING&           fwAnswer,
                 MeasurementResultContainer& measResult);

  static void getProgramableLoadResult(
                 const INT               pinsSize,
                 const TM::DCTEST_MODE&  mode,
                 const STRING&           fwAnswer,
                 MeasurementResultContainer& continResult);
};

/*
 *----------------------------------------------------------------------*
 * Routine: processLimitForSinglePolarity
 *
 * Purpose: change the limit according to the forceCurrent.
 *
 *----------------------------------------------------------------------*
 * Description:
 *    Update the limit value according to the polarity of force current.
 *
 * Note: 
 *
 *----------------------------------------------------------------------*
 */
inline void ContinuityTest::processLimitForSinglePolarity(
                                   LIMIT& limit, 
                                   const DOUBLE forceCurrent)
{
  TM::COMPARE lowCmp,highCmp;
  DOUBLE dLowVal = 0.0, dHighVal = 0.0;
  DOUBLE dUpdatedLow = 0.0, dUpdatedHigh = 0.0;
  limit.get(lowCmp,dLowVal,highCmp,dHighVal);

  if ( forceCurrent < 0.0 )
  {  /* negative */
    dUpdatedLow  = MIN(-fabs(dLowVal),-fabs(dHighVal));
    dUpdatedHigh = MAX(-fabs(dLowVal),-fabs(dHighVal));
  }
  else
  {  /* positive */
    dUpdatedLow  = MIN(fabs(dLowVal),fabs(dHighVal));
    dUpdatedHigh = MAX(fabs(dLowVal),fabs(dHighVal));
  }
  /* update the limit */
  limit.high(highCmp,dUpdatedHigh);
  limit.low(lowCmp,dUpdatedLow);
}


/*
 *----------------------------------------------------------------------*
 * Routine: ppmuParallelVoltageMeasurement
 *
 * Purpose: execute ppmu Parallel Voltage Measurement
 *          for continuity.
 *
 *----------------------------------------------------------------------*
 * Description: 
 *   @input parameters:
 *     param:             parameters container
 *     testLimit:         test limit(s)
 *     measureResultFirst:
 *        for the first polarity testing.
 *     measureResultSecond:
 *        for the second polarity testing.
 *
 * Note:
 *     use Flex-DC
 *----------------------------------------------------------------------*
 */
inline void ContinuityTest::ppmuParallelVoltageMeasurement(
                               const ContinuityTestParam&   param,
                               const ContinuityTestLimit&   testLimit,
                               PPMU_MEASURE&                measureResultFirst,
                               PPMU_MEASURE&                measureResultSecond)
{


  /* swtich DPS state */
  STRING fwCmdToRestoreDpsState = "";
  DpsUtil::switchDpsStateToLOZ(fwCmdToRestoreDpsState);

  /* measuring */
  DOUBLE dCurrentRange = fabs(param.forceCurrent[FIRST_POL]);
  try
  {
    PPMU_SETTING   settingPpmuFirst;
    PPMU_SETTING   settingPpmuSecond;
    TASK_LIST   tasklist;
    
    PPMU_CLAMP ppmuClampOn;
    PPMU_CLAMP ppmuClampOff;
    
    DOUBLE settling = param.settlingTime + 1; /* 1 PPMU settling time */
    
    /*  1. set the clamp (PinScale)*/
    DOUBLE low, high;

    testLimit.limit[FIRST_POL].getLow(&low);
    testLimit.limit[FIRST_POL].getHigh(&high);
    if (testLimit.use_mVAsDefaultUnit)
    {
      //convert value from 'mV' base to 'V' base for PPMU_SETTING
      low *= 1e-3;
      high *= 1e-3;
    }

    settingPpmuFirst.pin(param.pinlist)
    .iRange(dCurrentRange uA)
    .min(low)
    .max(high)
    .iForce(param.forceCurrent[FIRST_POL] uA);


    tasklist.add(settingPpmuFirst);
        
    /* clamp low/high are fixed to -0.1V/0.1V */
    ppmuClampOn.pin(param.pinlist).status("CLAMP_ON").low(-0.1 V).high(0.1 V);  
    ppmuClampOn.wait(1 ms);
    tasklist.add(ppmuClampOn);
    
    /*  2.relay switch */
    /*  for continuity test, we use PARALLEL option to control relay */
    PPMU_RELAY     relayPpmuOn;
    relayPpmuOn.pin(param.pinlist).status("PPMU_ON");
    relayPpmuOn.wait(0.3 ms);
    tasklist.add(relayPpmuOn);
    
    /* 3. switch clamp off (PinScale)*/

    ppmuClampOff.pin(param.pinlist).status("CLAMP_OFF");
    ppmuClampOff.wait(settling ms);
    tasklist.add(ppmuClampOff);
        
    /*  4. determine measure */
    /* PVAL or PPF or GPF is defined by TestSuite Flag */
    measureResultFirst.pin(param.pinlist).execMode(CommonUtil::getMode());
    tasklist.add(measureResultFirst);

    if ( param.polarity == "BPOL" )
    {
      /*
       **************************************************
       * do opposite polarity measurement:
       * Before testing, change the polarity of current 
       * and update the limit for judgement.
       **************************************************
       */    

      testLimit.limit[SECOND_POL].getLow(&low);
      testLimit.limit[SECOND_POL].getHigh(&high);

      if (testLimit.use_mVAsDefaultUnit)
      {
        //convert value from 'mV' base to 'V' base for PPMU_SETTING
        low *= 1e-3;
        high *= 1e-3;
      }

      settingPpmuSecond.pin(param.pinlist)
                       .iRange(dCurrentRange uA)
                       .min(low)
                       .max(high)
                       .iForce(param.forceCurrent[SECOND_POL] uA);
      tasklist.add(settingPpmuSecond);

      /* PVAL or PPF or GPF is defined by TestSuite Flag */
      measureResultSecond.pin(param.pinlist).execMode(param.testMode);
      tasklist.add(measureResultSecond);
    }

    /* 5.relay restore */

    PPMU_RELAY     relayPpmuOff;
    relayPpmuOff.pin(param.pinlist).status("PPMU_OFF");
    relayPpmuOff.wait(0.3 ms);
    tasklist.add(relayPpmuOff);

    /*  6.task list execute */
    tasklist.execute();
  }
  catch ( ... )
  {
    /* let sequencer into right mode after continuity test*/
    FW_TASK("SQST OFF\n");
    /* DPS must be restored in exceptional cases */
    DpsUtil::restoreDpsStateFromLozToHIZ(fwCmdToRestoreDpsState);
    throw;
  }

  FW_TASK("SQST OFF\n");

  /* restore DPS state */
  DpsUtil::restoreDpsStateFromLozToHIZ(fwCmdToRestoreDpsState);
}

/*
 *----------------------------------------------------------------------*
 * Routine: programmableLoad
 *
 * Purpose: do continuity test by using programmable load (active load). 
 * 
 *----------------------------------------------------------------------*
 * Description:
 *        call FW_TASK to execute the test.
 *    INPUT:  param       - test parameters
 *            testLimit   - test limit(s)
 *   
 *    OUTPUT:           
 *    RETURN: the result string that can be parsed to get the result.        
 *
 * Note:
 *   1.must be called inside ON_FIRST_INVOCATION_BEGIN/END block,
 *     because FOR_EACH_SITE_BEGIN/END block is used below.
 *----------------------------------------------------------------------*
 */
inline void ContinuityTest::programmableLoad(
                                   const ContinuityTestParam& param,
                                   const ContinuityTestLimit& testLimit,
                                   MeasurementResultContainer& continResult)
{
  DOUBLE dLowVal = 0.0,dHighVal = 0.0;
  testLimit.limit[FIRST_POL].getLow(&dLowVal);
  testLimit.limit[FIRST_POL].getHigh(&dHighVal);
  /*
   ************************************
   * DFCT FW command's internal units:
   * current [uA] 
   * voltage [mV]
   ************************************
   */
  
  //convert value to 'mV' based
  if (!testLimit.use_mVAsDefaultUnit)
  {
    dLowVal   *= 1e3; 
    dHighVal  *= 1e3; 
  }

  /*
   *********************************
   * set commutation voltage,
   * 1000mV is the minimum value. 
   *********************************
   */
  DOUBLE dCommutVolt = (param.forceCurrent[FIRST_POL] < 0.0)
                       ?(-1000.0 + dLowVal):(1000.0 + dHighVal); 

  /*
   ***************************************
   * create and send fw command string
   * of defining continuity test.
   ***************************************
   */
  STRING strFwCmd, polarity;
  /* precondition FW command string */
  strFwCmd = "SQGB ACQF,0\n";

  /* create DFCT FW command string */
  strFwCmd += "DFCT ";
  CHAR fwCmdStringValue[20] = "\0";

  /* force current */
  snprintf(fwCmdStringValue,20,"%.9f,",param.forceCurrent[FIRST_POL]);
  strFwCmd += fwCmdStringValue; 

  /* commutation voltage */
  snprintf(fwCmdStringValue,20,"%.9f,",dCommutVolt);
  strFwCmd += fwCmdStringValue; 

  /* pass min. voltage */
  snprintf(fwCmdStringValue,20,"%.9f,",dLowVal);
  strFwCmd += fwCmdStringValue; 

  /* pass max. voltage */
  snprintf(fwCmdStringValue,20,"%.9f,",dHighVal);
  strFwCmd += fwCmdStringValue; 

  /* polarity */


  strFwCmd += param.polarity + ","; 

  /* pin list */
  STRING strPins = PinUtility.createPinListFromPinNames(param.expandedPins);
  strFwCmd += "(" + strPins +")\n"; 

  /*
   *************************************** 
   * append query commands for 
   * all active sites.
   ***************************************
   */
  switch ( param.testMode )
  {
  case TM::PPF:
  case TM::PVAL: /* consider PVAL as PPF for 'CTST?' */
    FOR_EACH_SITE_BEGIN();
    /* set query focus site */
    strFwCmd += "PQFC ";
    sprintf(fwCmdStringValue,"%d",CURRENT_SITE_NUMBER());
    strFwCmd += fwCmdStringValue;
    strFwCmd += "\n";
    /* CTST? command */
    strFwCmd += "CTST? PPF\n";
    FOR_EACH_SITE_END();
    break;
  case TM::GPF:
    strFwCmd += "CTST? GPF\nPRLT? ALL\n";
    break;
  default:
    throw Error("ContinuityTest::programmableLoad",
                "Unknown test mode.");
    break; 
  }

  /* send FW */ 
  STRING strFwAnswer = "";
  FW_TASK(strFwCmd,strFwAnswer);
  getProgramableLoadResult(param.expandedPins.size(),
                           param.testMode,
                           strFwAnswer,
                           continResult);

}

/*
 *----------------------------------------------------------------------*
 * Routine: getProgramableLoadResult
 *
 * Purpose: parse fwAnswer and record continuity test results. 
 *
 *----------------------------------------------------------------------*
 * Description:
 *      support multi-site.
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline void ContinuityTest::getProgramableLoadResult(
                                  const INT               pinsSize,
                                  const TM::DCTEST_MODE&  mode,
                                  const STRING&           fwAnswer,
                                  MeasurementResultContainer& continResult)
{
  switch ( mode )
  {
  case TM::PPF:
  case TM::PVAL: 
    parseAndRecordForPPF(pinsSize,fwAnswer,continResult);
    break;
  case TM::GPF:
    parseAndRecordPRLTResult(fwAnswer,continResult);
    break;
  default:
    throw Error("ContinuityTest::evaluateProgramableLoadResult",
                "Unknown execute mode.");
    break; 
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: ContinuityTest::parseAndRecordForPPF
 *
 * Purpose: parse and record continuity test results for PPF mode
 *
 *----------------------------------------------------------------------*
 * Description:
 *      support multi-site.
 *
 * Note:
 *   1. must be called inside ON_FIRST_INVOCATION_BEGIN/END block,
 *      because FOR_EACH_SITE_BEGIN/END block is used below.
 *----------------------------------------------------------------------*
 */
inline void ContinuityTest::parseAndRecordForPPF(
                                        const INT               pinsSize,
                                        const STRING&           fwAnswer,
                                        MeasurementResultContainer& continResult)
{

  /*
   ************************************
   * Analyze the query result
   * examples of the answer string:
   * (1) CTST F,(I/O0,I/O1,I/O2)\n
   * (2) CTST P,(I/O0)\n CTST F,(I/O1,I/O2)\n
   ************************************
   */

  STRING::size_type  iPosOne = 0;
  STRING::size_type  iPosTwo = 0;
  iPosOne = fwAnswer.find( "CTST" );

  /* retrieve and record results for each site */
  FOR_EACH_SITE_BEGIN();    
    STRING resToken = "";
    INT nSite = CURRENT_SITE_NUMBER();
    Boolean isPass = TRUE;       /*define for per pin result*/
    Boolean bGlobalResult = TRUE; /*define for global result*/
    INT nPins = pinsSize;         /*to identify results for each site*/
  
    while ( (iPosOne != string::npos) && (nPins > 0) )
    {
      /*retrieve the result of checking states*/
      iPosOne = fwAnswer.find( ',',iPosOne+5 );
      resToken = fwAnswer[iPosOne-1];
      /*get pass/fail*/
      isPass = ( (resToken == "P") || (resToken == "p") )?TRUE:FALSE;
      /*update GPF result*/
      if ( (!isPass) )
      {
        bGlobalResult = FALSE; 
      }
      iPosOne = fwAnswer.find( '(',iPosOne+1 );
      if (string::npos == iPosOne)
      {
        break;
      }
      ++iPosOne;
      iPosTwo = fwAnswer.find( ')',iPosOne+1 );
      if (string::npos == iPosTwo)
      {
        break;
      }
  
      /*retrieve the pin name*/
      resToken = fwAnswer.substr( iPosOne,iPosTwo-iPosOne );
      /*Parse this substring and record the results*/
      STRING_VECTOR arrayOfPinNames;
      arrayOfPinNames.clear();
      CommonUtil::splitStr( resToken,',',arrayOfPinNames );
      for ( STRING_VECTOR::const_iterator it = arrayOfPinNames.begin(); 
          it != arrayOfPinNames.end(); ++it )
      {
        continResult.setPinPassFail( *it,isPass,nSite );
        --nPins;
      }
      /* try to get the next line starting with 'CTST' */
      iPosOne = fwAnswer.find( "CTST",iPosTwo+1 );
    }
    /* set global result for the site */
    continResult.setGlobalPassFail(bGlobalResult,nSite);
  FOR_EACH_SITE_END();
 
}

/*
 *----------------------------------------------------------------------*
 * Routine: parseAndRecordPRLTResult
 *
 * Purpose: parse and record test results by PRLT
 *
 *----------------------------------------------------------------------*
 * Description:
 *      support multi-site.
 *
 * Note:
 *   1. must be called inside ON_FIRST_INVOCATION_BEGIN/END block,
 *      because FOR_EACH_SITE_BEGIN/END block is used below. 
 *----------------------------------------------------------------------*
 */
inline void ContinuityTest::parseAndRecordPRLTResult(
                                        const STRING&           fwAnswer,
                                        MeasurementResultContainer& measResult)
{

  /*
   ************************************
   * Analyze the query result
   * examples of the answer string:
   * (1) PRLT ALL, "PF0"
   ************************************
   */

  STRING::size_type  iPosOne = 0;
  iPosOne = fwAnswer.find( "PRLT" );
  iPosOne = fwAnswer.find( '"',iPosOne+5 );

  /* retrieve and record results for each site */
  FOR_EACH_SITE_BEGIN();    
    STRING resToken = "";
    INT nSite = CURRENT_SITE_NUMBER();
    Boolean bGlobalResult = TRUE; /*define for global result*/
  
    /*retrieve the result of checking states*/
    resToken = fwAnswer[iPosOne+nSite];
    /*get pass/fail*/
    bGlobalResult = ( (resToken == "P") || (resToken == "p") )?TRUE:FALSE;
    /* set global result for the site */
    measResult.setGlobalPassFail(bGlobalResult,nSite);
  FOR_EACH_SITE_END();

}


#endif /*CONTINUITYTEST_H_*/
