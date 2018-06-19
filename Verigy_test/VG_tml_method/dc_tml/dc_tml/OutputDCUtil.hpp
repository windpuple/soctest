#ifndef OUTPUTDCTEST_H_
#define OUTPUTDCTEST_H_
#include "CommonUtil.hpp"
#include "PmuUtil.hpp"
#include "FunctionalUtil.hpp"
#include <float.h>

/***************************************************************************
 *                    output dc test class 
 ***************************************************************************
 */  
class OutputDcTest
{
public:

  enum {TWO_LEVEL = 2};  /*two kinds of output level*/
  enum MeasuredLevelType { LOW_LEVEL = 0, HIGH_LEVEL = 1 };                 

/*
 *----------------------------------------------------------------------*
 *         test parameters container                                    *
 *----------------------------------------------------------------------*
 */ 

  struct TestParam
  {
    /*original input parameters*/
    STRING pinlist;
    STRING measuredLevel;               /*LOW,HIGH,BOTH*/
    DOUBLE forceCurrent_uA[TWO_LEVEL];
    DOUBLE minPassVal_V[TWO_LEVEL];
    DOUBLE maxPassVal_V[TWO_LEVEL];  
    STRING mode;                  /*ProgramLoad,PPMU,PPMUTerm,SPMU,SPMUTerm*/
    DOUBLE settlingTime_ms[TWO_LEVEL];
    DOUBLE spmuClampVoltage_V[TWO_LEVEL];
    STRING termination;
    STRING vectorRange;
    
    /*new generated parameter for convience*/
    STRING testsuiteName;         /*current testsuite name*/
    STRING_VECTOR expandedPins;   /*expanded and validated pins stored in Vector*/
    Boolean isValueTest;          /*value test or pass/fail test*/
    Boolean isPerPinResult;       /*result(PF or Value) per pin or per pinlist*/

   /*initialize stuff to some defaults.*/    
    void init()
    {
      forceCurrent_uA[LOW_LEVEL] = forceCurrent_uA[HIGH_LEVEL] = 0.0;
      minPassVal_V[LOW_LEVEL] = minPassVal_V[HIGH_LEVEL] = DBL_MAX;
      maxPassVal_V[LOW_LEVEL] = maxPassVal_V[HIGH_LEVEL] = -DBL_MAX;
      expandedPins.clear();
      pinlist = "";
      mode = "UNKNOWN";
      memset(settlingTime_ms, 0, sizeof(DOUBLE)*TWO_LEVEL);
      memset(spmuClampVoltage_V, 0, sizeof(DOUBLE)*TWO_LEVEL);
      isValueTest = false;
      isPerPinResult = false;
    }
    /*default constructor for intializing parameters*/
    TestParam()
    {        
      init();
    }
  };

  /*
   *----------------------------------------------------------------------*
   *         test limit(s) container                                      *
   *----------------------------------------------------------------------*
   */
  struct TestLimit
  {    
    LIMIT passLimit[TWO_LEVEL];   /*LIMIT object arrays*/ 
    string testname[TWO_LEVEL];
    bool isLimitTableUsed;
 
    /*initialize limit(s) to some defaults.*/
    void init()
    {
      passLimit[LOW_LEVEL] = TM::Fail;
      passLimit[HIGH_LEVEL] = TM::Fail;
      testname[LOW_LEVEL] = "";
      testname[HIGH_LEVEL] = "";
      isLimitTableUsed = true;
    }

    /*default constructor */
    TestLimit()
    {
      init();
    }

  };

/*
 *----------------------------------------------------------------------*
 *         test results container                                       *
 *----------------------------------------------------------------------*
 */
  struct TestResult
  {
    /*result container per site:
     *for value, the unit is: V
     */
    MeasurementResultContainer resultPerSite[TWO_LEVEL];
    Boolean funcResult; /*true: PASS, false: FAIL*/
    
   /*initialize all stuffs to some defaults.*/
    void init()
    {
      resultPerSite[0].init();
      resultPerSite[1].init();
      funcResult =  true;
    }
    /*default constructor*/
    TestResult()
    {
      init();
    }
  };
  

/*
 *----------------------------------------------------------------------*
 *         public interfaces of output dc test                          *
 *----------------------------------------------------------------------*
 */
 
/*
 *----------------------------------------------------------------------*
 * Routine: processParameters
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
                              const STRING& measuredLevel,
                              DOUBLE forceCurrentLow_uA,
                              DOUBLE forceCurrentHigh_uA,
                              DOUBLE minPassLow_V,
                              DOUBLE maxPassLow_V,
                              DOUBLE minPassHigh_V,
                              DOUBLE maxPassHigh_V,
                              const STRING& mode,
                              DOUBLE settlingTimeLow_ms,
                              DOUBLE settlingTimeHigh_ms,
                              DOUBLE spmuClampLow_V,
                              DOUBLE spmuClampHigh_V,
                              const STRING& vectorRange,
                              TestParam& param)
{
    /*initial param*/
    param.init();
    /*validate pinlist and expand it*/
    param.pinlist = CommonUtil::trim(pinlist);
    if (param.pinlist.empty())
    {
      throw Error("OutputDcTest::processParameter()",
                  "pinlist is not specified.",
                  "OutputDcTest::processParameter()");
    }
    param.expandedPins = PinUtility.getDigitalPinNamesFromPinList(
                                       param.pinlist,
                                       TM::O_PIN|TM::IO_PIN,
                                       TRUE);

    param.measuredLevel = CommonUtil::trim(measuredLevel);
    param.mode = CommonUtil::trim(mode);
    
    if (param.mode == "PPMUTerm" || param.mode == "SPMUTerm")
    {
      param.termination = TURN_ON;
      /*If termination is ON for io pins, and for o pins with term type termination,
       *the following formula applies:
       *DUT output current = (PMU current) + (test system driver current)
       *Because the DUT output current in this case can be high enough to damage your device, 
       *a warning is generated, reminding you to set the force current to a low value,
       *if the specified current limit is > 1 uA.
       */
      if (abs(forceCurrentLow_uA) > 1.0 || abs(forceCurrentHigh_uA) > 1.0)
      {
        cout<<"WARNING:\n"
            <<"Force current should be set to a small value,"
            <<" if termination remains connected during measurement."
            <<endl;
      }
    }
    else if (param.mode == "PPMU" || param.mode == "SPMU")
    {
      param.termination = TURN_OFF;
    }
    else if (param.mode != "ProgramLoad")
    {
      throw Error("OutputDcTest::processParameter()",
                  "unknow mode: "+param.mode+".",
                  "OutputDcTest::processParameter()");
    }
       
    /*get test mode*/
    STRING testSuiteName = "";
    INT valueOnPass = 0;
    INT perPinOnPass = 0;
    INT valueOnFail = 0;
    INT perPinOnFail = 0;
    param.isValueTest = false;
    param.isPerPinResult = false;
    GET_TESTSUITE_NAME(testSuiteName);
    param.testsuiteName = testSuiteName;
    
    GET_TESTSUITE_FLAG(testSuiteName, "value_on_pass",   &valueOnPass);
    GET_TESTSUITE_FLAG(testSuiteName, "value_on_fail",   &valueOnFail);
    GET_TESTSUITE_FLAG(testSuiteName, "per_pin_on_pass", &perPinOnPass);
    GET_TESTSUITE_FLAG(testSuiteName, "per_pin_on_fail", &perPinOnFail);
    if ( 1 == valueOnPass || 1 == valueOnFail )
    {
      param.isValueTest = true;
    }
    if ( 1 == perPinOnPass || 1 == perPinOnFail )
    {
      param.isPerPinResult = true;
    }

    if (param.measuredLevel == "LOW" || param.measuredLevel == "BOTH") 
    {
      param.forceCurrent_uA[LOW_LEVEL] = forceCurrentLow_uA;
      if ( settlingTimeLow_ms < 0.0 ) 
      {
        throw Error("OutputDcTest::processParameter()",
                    "settlingTimeLow_ms must be postive!",
                    "OutputDcTest::processParameter()");
      }
      else
      {
        param.settlingTime_ms[LOW_LEVEL] = settlingTimeLow_ms;
      }

      param.spmuClampVoltage_V[LOW_LEVEL] = spmuClampLow_V;
      param.minPassVal_V[LOW_LEVEL] = minPassLow_V;
      param.maxPassVal_V[LOW_LEVEL] = maxPassLow_V;
    }

    if (param.measuredLevel == "HIGH" || param.measuredLevel == "BOTH") 
    {
      param.forceCurrent_uA[HIGH_LEVEL] = forceCurrentHigh_uA;
      if ( settlingTimeHigh_ms < 0.0 ) 
      {
        throw Error("OutputDcTest::processParameter()",
                     "settlingTimeHigh_ms must be postive!",
                     "OutputDcTest::processParameter()");
      }
      else
      {
        param.settlingTime_ms[HIGH_LEVEL] = settlingTimeHigh_ms;
      }

      param.spmuClampVoltage_V[HIGH_LEVEL] = spmuClampHigh_V;
      param.minPassVal_V[HIGH_LEVEL] = minPassHigh_V;
      param.maxPassVal_V[HIGH_LEVEL] = maxPassHigh_V;
    }

    param.vectorRange = CommonUtil::trim(vectorRange);
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
  static void processLimit(TestParam& param, const string& testname, TestLimit& testLimit)
  {
    /* init testLimit */
    testLimit.init();

    //process input limits
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
      { //for one input, both of testname share this input name
        testLimit.testname[HIGH_LEVEL] =
          testLimit.testname[LOW_LEVEL] = limitNames[0];  
      }
      else 
      {
        testLimit.testname[LOW_LEVEL] = limitNames[0];
        testLimit.testname[HIGH_LEVEL] = limitNames[1];
      }

      if (param.measuredLevel == "BOTH" && limitNames.size() != TWO_LEVEL)
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
      throw Error("OutputDcTest::processLimit()","parameter \"testname\" value is invalid!",
        "OutputDcTest::processLimit()");
    }

    //if limit CSV file is load, get limit from it;otherwise get limit from testflow.
    bool isLowLevelTestRequired 
      = (param.measuredLevel == "LOW" || param.measuredLevel == "BOTH");
    bool isHighLevelTestRequired 
      = (param.measuredLevel == "HIGH" || param.measuredLevel == "BOTH");

    TesttableLimitHelper ttlHelper(param.testsuiteName);
    if (ttlHelper.isLimitCsvFileLoad())
    {
      if (isLowLevelTestRequired)
      {
        ttlHelper.getLimit(testLimit.testname[LOW_LEVEL],testLimit.passLimit[LOW_LEVEL]);
      }

      if (isHighLevelTestRequired)
      {
        ttlHelper.getLimit(testLimit.testname[HIGH_LEVEL],testLimit.passLimit[HIGH_LEVEL]);
      }
    }
      
    testLimit.isLimitTableUsed = ttlHelper.isAllInTable();

    if (!testLimit.isLimitTableUsed)
    {
      if (isLowLevelTestRequired)
      {
        testLimit.passLimit[LOW_LEVEL] = GET_LIMIT_OBJECT(testLimit.testname[LOW_LEVEL]);
      }

      if (isHighLevelTestRequired)
      {
        testLimit.passLimit[HIGH_LEVEL] = GET_LIMIT_OBJECT(testLimit.testname[HIGH_LEVEL]);
      }
    }

    //set default unit (V)
    if(isLowLevelTestRequired)
    {
      if (testLimit.passLimit[LOW_LEVEL].unit().empty())
      {
        testLimit.passLimit[LOW_LEVEL].unit("V");
      }
    }

    if(isHighLevelTestRequired)
    {
      if (testLimit.passLimit[HIGH_LEVEL].unit().empty())
      {
        testLimit.passLimit[HIGH_LEVEL].unit("V");
      }
    }

    /*For PMU method, need clamp voltage simulation*/
    Boolean needSimulationClampVot = false;
    if (param.mode == "PPMUTerm" || param.mode == "PPMU")
    {
      needSimulationClampVot = true;
    }

    
    //to make compatible with the obsolete way: use min/max parameters as limit:
    //if LIMIT object is defined as "DON'T CARE (i.e. NA)", apply min/max parameter value to limit
    if (isLowLevelTestRequired)
    {
      /*set pass limits.*/
      TM::COMPARE lowerCmp,upperCmp;
      DOUBLE lowerVal, upperVal;
      testLimit.passLimit[LOW_LEVEL].get(lowerCmp, lowerVal, upperCmp, upperVal);
      if (!testLimit.isLimitTableUsed && lowerCmp == TM::NA && upperCmp == TM::NA) 
      {
        lowerCmp = TM::GE;
        lowerVal = param.minPassVal_V[LOW_LEVEL];
        upperCmp = TM::LE;
        upperVal = param.maxPassVal_V[LOW_LEVEL];
        testLimit.passLimit[LOW_LEVEL].low(lowerCmp,lowerVal);
        testLimit.passLimit[LOW_LEVEL].high(upperCmp,upperVal);
        testLimit.passLimit[LOW_LEVEL].unit("V");
      }
      
      param.minPassVal_V[LOW_LEVEL] = lowerVal;
      param.maxPassVal_V[LOW_LEVEL] = upperVal;

      /*please see "CommonUtil::clampSimulation" comments*/
      if (needSimulationClampVot)
      {
        double highVal_V;
        testLimit.passLimit[LOW_LEVEL].getHigh(&highVal_V);
        CommonUtil::clampSimulation("PPMUpar",
                                    param.spmuClampVoltage_V[LOW_LEVEL], 
                                    highVal_V,
                                    param.forceCurrent_uA[LOW_LEVEL]);
      }                                
    }
  
    if (isHighLevelTestRequired)
    {
      /*set pass limits.*/
      TM::COMPARE lowerCmp,upperCmp;
      DOUBLE lowerVal, upperVal;
      testLimit.passLimit[HIGH_LEVEL].get(lowerCmp, lowerVal, upperCmp, upperVal);
      if (!testLimit.isLimitTableUsed && lowerCmp == TM::NA && upperCmp == TM::NA)
      {
        lowerCmp = TM::GE;
        lowerVal = param.minPassVal_V[HIGH_LEVEL];
        upperCmp = TM::LE;
        upperVal = param.maxPassVal_V[HIGH_LEVEL];
        testLimit.passLimit[HIGH_LEVEL].low(lowerCmp,lowerVal);
        testLimit.passLimit[HIGH_LEVEL].high(upperCmp,upperVal);
        testLimit.passLimit[HIGH_LEVEL].unit("V");
      }

      param.minPassVal_V[HIGH_LEVEL] = lowerVal;
      param.maxPassVal_V[HIGH_LEVEL] = upperVal;        

      /*please see "CommonUtil::clampSimulation" comments*/
      if (needSimulationClampVot)
      {     
        double highVal_V;
        testLimit.passLimit[HIGH_LEVEL].getHigh(&highVal_V);
        CommonUtil::clampSimulation("PPMUpar",
                                    param.spmuClampVoltage_V[HIGH_LEVEL], 
                                    highVal_V,
                                    param.forceCurrent_uA[HIGH_LEVEL]);
      }                                  
    }
  }
/*
 *----------------------------------------------------------------------*
 * Routine: doMeasurementByPMU
 *
 * Purpose: execute measurement by PMU/SPMU and store results
 *
 *----------------------------------------------------------------------*
 * Description:
 *   INPUT:  param       - test parameters
 *           testLimit   - test limit used by setup measurement
 *           result      - result container
 *
 *   OUTPUT: 
 *   RETURN: 
 * Note:
 *   OUTS? cannot support "burst" label!
 *   So if you know the measurement points of burst label, please
 *   directly fill in measurePosition maps 
 *   and skip searchOutputLevelByOUTS() function.
 *----------------------------------------------------------------------*
 */    
static void doMeasurementByPMU(const TestParam& param,const TestLimit& testLimit, TestResult& result)
{
  /* measurement points map contain:
   * first:   the n-th cycle number.
   * second:  low or high level {0: low, 1: high}
   * third:   pin which should be measured at that cycle number.
   * e.g. pins: I/O1 ~I/O5
   * cycle 1:
   * measurePositions[1][0]   = "I/O0,I/O1,I/O2,I/O3"
   * cycle 12:
   * measurePositions[12][1]  = "I/O0,I/O1,I/O2"
   * measurePositions[12][0]  = "I/O4,I/O5"
   * cycle 33:
   * measurePositions[33][1]  = "I/O3,I/O4,I/O5"
   */
  static map< INT,map<INT,STRING> > measurePositions;
  static STRING xpmuMode;
 
  ON_FIRST_INVOCATION_BEGIN();
    /*Init result*/
    result.init();
    
    /*CONNECT: firstly try to connect*/
    CONNECT();  
    
    /*SETUP: measurement positions*/
    measurePositions.clear();
    /*Default is to use OUTS? command to query the cycle number
     *for Low/High measurements.
     *In case of knowing those points, this function can be 
     *omitted and directly fill in the map according to 
     *the map definitions described above.
     */
    searchOutputLevelByOUTS(param,measurePositions);
    
    /*To call the existing PMU measurement utilities,
     *need set new xpmuMode according to the param.mode:
     * PPMUpar or SPMUser
     */    
    if (param.mode == "PPMUTerm" || param.mode == "PPMU")
    {
      xpmuMode = "PPMUpar";
    } 
    else if (param.mode == "SPMUTerm" || param.mode == "SPMU")
    {
      xpmuMode = "SPMUser";
    }
    else
    {
      throw Error("OutputDcTest::doMeasurementByPMU()",
                  "Unknow param.mode.",
                  "OutputDcTest::doMeasurementByPMU()");
    }
  ON_FIRST_INVOCATION_END();


  /*EXECUTE & RESULT HANDLING*/ 
  /*init GPF and funcResult for this site*/     
  result.resultPerSite[LOW_LEVEL].setGlobalPassFail(true);  
  result.resultPerSite[HIGH_LEVEL].setGlobalPassFail(true);  
  result.funcResult = true;    
  
  /*measure for all measurement positions*/
  for (map<INT,map<INT,STRING> >::const_iterator it = measurePositions.begin();
       it != measurePositions.end();
       ++it )
  {
    
    if ((param.measuredLevel == "LOW" || param.measuredLevel == "BOTH")
        && it->second.find(LOW_LEVEL) != it->second.end())  
    {
      excuteXPMUMeasurement(it->first,
                            param,
                            testLimit,
                            LOW_LEVEL,
                            measurePositions[it->first][LOW_LEVEL],
                            xpmuMode,
                            result);
    }
  
    if ((param.measuredLevel == "HIGH" || param.measuredLevel == "BOTH")
        && it->second.find(HIGH_LEVEL) != it->second.end())  
    {  
      excuteXPMUMeasurement(it->first,
                            param,
                            testLimit,
                            HIGH_LEVEL,
                            measurePositions[it->first][HIGH_LEVEL],
                            xpmuMode,
                            result);
    }     
  }/*end-for*/  

  ON_FIRST_INVOCATION_BEGIN();
    Sequencer.reset();
  ON_FIRST_INVOCATION_END();
}


/*
 *----------------------------------------------------------------------*
 * Routine: doMeasurementByProgramLoad
 *
 * Purpose: execute measurement by active load and store results
 *
 *----------------------------------------------------------------------*
 * Description:
 *   INPUT:  param       - test parameters
 *   OUTPUT: result      - result container
 *   RETURN: 
 * Note:
 *   In case of GPF, please refer to following test setup tips!
 *----------------------------------------------------------------------*
 */   
static void doMeasurementByProgramLoad(const TestParam& param,TestResult& result)
{
  STRING testCmd = "",answer = "";
  STRING originalStatus = "";
/*
 * In case of GPF, you can make it faster:
 * HOW-TO:
 * 1.create new level sets with:
 *    primary VOH = pass min voltage.
 *    primary VOL = pass max voltage.
 * 2.select the new primary level set for testsuite.
 * 3.setup SPRM with new level sets(see example code below)
 *  3.1) for both levels test, input two level sets
 *  3.2) for single level test, input the specific level set only. 
 * 4.enable this flowing code by '#define USE_OPTIMIZED_OUTPUTDC_TF_WAY'
 * 5.rebuild testmethod and run
 */

//#define USE_OPTIMIZED_OUTPUTDC_TF_WAY

#ifdef  USE_OPTIMIZED_OUTPUTDC_TF_WAY
if ( !param.isValueTest && !param.isPerPinResult)
{
  ON_FIRST_INVOCATION_BEGIN();
    /*Init result*/
    result.init();
    
    /*CONNECT: firstly try to connect*/  
    CONNECT();
    
    /*SETUP: 
     *apply optimized commands stuffs
     *it depends on your own output dc test parameters
     */
    MeasuredLevelType level =  LOW_LEVEL;
    STRING optimizedOutputDCTFStuffs;
    if (param.measuredLevel == "BOTH")
    {
      /*case3.1 both levles:
       * THIS IS JUST AN EXAMPLE CODE!!! Please refer to above tips.
       */
      optimizedOutputDCTFStuffs = 
        "SPRM 1,1,6,1;FTST? ;PRLT? ALL;SPRM 1,1,7,1;FTST? ;PRLT? ALL;";
    }
    else
    {
      /*case3.2 single level*/
      optimizedOutputDCTFStuffs = "FTST?;PRLT? ALL;";
      
      if (param.measuredLevel == "HIGH")
      {
        level = HIGH_LEVEL;
      }    
    }
    
    /*EXECUTE*/
    FW_TASK(optimizedOutputDCTFStuffs,answer);
    
    /*RESULT: get,parse and cache the results*/
    STRING::size_type  pos = answer.find("PRLT",0);
    STRING prltResult;
    while (pos != STRING::npos)
    {
      retrieveStringWithinDelimiters(answer,"\"","\"",pos+1,prltResult);
      for (STRING::size_type index = 0; index < prltResult.length(); ++index)
      { 
        if (prltResult[index] != '0')
        {
          Boolean isPass = (prltResult[index] == 'P')?true:false;  
          /*the "index+1" of rclsResult is related to site number*/
          result.resultPerSite[level].setGlobalPassFail(isPass,index+1);           
        }
        else
        {
          /*otherwise: the site is masked out*/
        }
      }
      level =  HIGH_LEVEL;
      pos = answer.find("PRLT",pos+1);   
    } 
 ON_FIRST_INVOCATION_END();
}
else
#undef USE_OPTIMIZED_OUTPUTDC_TF_WAY
#endif
{
  ON_FIRST_INVOCATION_BEGIN();                  
    /*Init result*/
    result.init();
    
    /*CONNECT: firstly try to connect*/
    CONNECT();  
  
    /*QUERY: current status*/
    FW_TASK("TERM? PRM,(" + param.pinlist +");"+"SPST? LEV,PRM,;UPTD? LEV\n",
      originalStatus);

    /*QUERY: primary receive level*/
    FW_TASK("RCLV? PRM,(" + param.expandedPins[0] +")\n",answer);

    /* Parse the primary receive level
     * For above command, the answer should be in format:
     * RCLV PRM,logic0_level,logic1_level,(pin)
     */
    STRING primaryLogic0Level,primaryLogic1Level;
    STRING::size_type pos = 
      retrieveStringWithinDelimiters(answer,",",",",0,primaryLogic0Level);

    retrieveStringWithinDelimiters(answer,",",",",pos,primaryLogic1Level);
      

    if (param.measuredLevel == "LOW" || param.measuredLevel == "BOTH")
    {
      /*SETUP: RCLS or RCLV,FTST,PRLT commands*/
      testCmd = setupActiveLoadMeasurement(param,
                                       LOW_LEVEL,      
                                       primaryLogic0Level,
                                       primaryLogic1Level);
      /*EXECUTE:  send commands and get answer*/
      FW_TASK(testCmd,answer);
    
      /*RESULT: get,parse and cache the results*/                                
      processResultForActiveLoad(param,answer,LOW_LEVEL,result);                                                                                                        
    }
  
    if (param.measuredLevel == "HIGH" || param.measuredLevel == "BOTH")
    {
      /*SETUP: RCLS or RCLV,FTST,PRLT commands*/
      testCmd = setupActiveLoadMeasurement(param,
                                           HIGH_LEVEL,           
                                           primaryLogic0Level,
                                           primaryLogic1Level);
      /*EXECUTE:  send commands and get answer*/
      FW_TASK(testCmd,answer);
    
      /*RESULT: get,parse and cache the results*/                                
      processResultForActiveLoad(param,answer,HIGH_LEVEL,result);  
    }
  
    /*RESTORE: restore to original settings*/
    if (!param.isPerPinResult && !param.isValueTest)
    {
      /*
       * In case of GPF, Level set is changed and restored.
       * Please refer to setupActiveLoadMeasurement().
       * Now, we MUST set "UPTD LEV,1\n" to eliminate side-effect.
       */
      originalStatus.replace(originalStatus.size()-2,2,"1\n");    
    }
    FW_TASK(originalStatus);
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
 *           testLimit   - test limits
 *           result      - result container
 *           _results[4] - retrieved by built-in tf_result() in testflow.
 *   OUTPUT: 
 *   RETURN: 
 * Note:
 *----------------------------------------------------------------------*
 */ 
static void judgeAndDatalog(const TestParam& param,
                            const TestLimit& testLimit,
                            TestResult& result,
                            double _results[4])
{
  INT siteNumber;
  MeasuredLevelType level;
  /*init _results*/
  _results[0]=_results[2]= DBL_MAX; 
  _results[1]=_results[3]= -DBL_MAX;
    
  /*In case of ProgrammableLoad method, all sites' results are stored 
   *in result. ==> sitenumber = CURRENT_SITE_NUMBER();
   *Otherwise (PPMU and SPMU method), all sites' results are cached in
   *respective TestMethod APIs, and only the current site's is stored
   *in result. ==> sitenumber always is 0 for storing and retrieving 
   *in/from result.
   */
  siteNumber = (param.mode != "ProgramLoad")? 0 : CURRENT_SITE_NUMBER();    
  
  INT times = 1; /*measurement times*/
  if(param.measuredLevel == "LOW")
  {
    level = LOW_LEVEL;
    times = 1;
  }
  else if (param.measuredLevel == "HIGH")
  {
    times = 1;
    level = HIGH_LEVEL;
  }
  else /*BOTH*/
  {
    level = LOW_LEVEL;
    times = 2;  
  }

  for (INT i = 0; i < times; i++,level = HIGH_LEVEL)
  {  
    if (param.isValueTest)
    {
      DOUBLE value = 0.0;
      /* For PMU/SPMU value test, there is only "TM::PVAL" option available for value test,
       * and no Value per pinlist measurement.
       * To be consistent with Standard TestFunction's way, 
       * should always log value per pin for this case.
       */
      ARRAY_D measuredValueArray(param.expandedPins.size());
      if(param.isPerPinResult || param.mode != "ProgramLoad") 
      {
        for(STRING_VECTOR::size_type index = 0; 
            index < param.expandedPins.size(); 
            ++index)
        {
          value = result.resultPerSite[level]
                        .getPinsValue(param.expandedPins[index],siteNumber);
          measuredValueArray[index] = value;
        }
        //MPR record
        if (testLimit.isLimitTableUsed)
        {
          TestSet.cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                           param.expandedPins,
                                           testLimit.testname[level],
                                           V93kLimits::tmLimits,
                                           measuredValueArray);
        }
        else
        {
          TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                           param.expandedPins,
                                           testLimit.testname[level],
                                           measuredValueArray);
 
        } 
        /*fill in _results[4]
         *_results[0]: min val for Low Level
         *_results[1]: max val for Low Level 
         *_results[2]: min val for High Level
         *_results[3]: max val for High Level 
         */
        _results[level*2] = 
          result.resultPerSite[level].getMinValue(siteNumber) *1e+3;  //'mV' 
        _results[level*2+1] = 
          result.resultPerSite[level].getMaxValue(siteNumber) *1e+3;//'mV'
      }
      else if (param.mode == "ProgramLoad" && !param.isPerPinResult)
      {
        value = result.resultPerSite[level]
                      .getPinsValue(param.pinlist,siteNumber);
        if (testLimit.isLimitTableUsed)
        {
          TestSet.cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                           param.pinlist,
                                           testLimit.testname[level],
                                           V93kLimits::tmLimits,
                                           value);
        }
        else
        {
          TESTSET().cont(TM::CONTINUE)
                 .judgeAndLog_ParametricTest(param.pinlist,
                                             testLimit.testname[level],
                                             testLimit.passLimit[level],
                                             value); 
        }       
        /*fill in _results[4]*/
        _results[level*2] = _results[level*2+1] = value*1e+3; //'mV' based value
      }
      else
      {
        cout<<"ERROR: OutputDCUtil::judgeAndDatalog() "
              "doesn't know to log values with unknown case."
            <<endl;
      }
    }
    else /*Pass/Fail Measurement*/
    {
      Boolean isPass = true;
      ARRAY_D measuredValueArray(param.expandedPins.size());
      if(param.isPerPinResult)
      {
        for(STRING_VECTOR::size_type index = 0; 
            index < param.expandedPins.size(); 
            ++index)
        {
          bool isOnePass = result.resultPerSite[level]
                         .getPinPassFail(param.expandedPins[index],siteNumber);
          measuredValueArray[index] = isOnePass?MPR_LOG_PF_TEST_PASS:MPR_LOG_PF_TEST_FAIL;
          isPass= isPass && isOnePass;
        }
        //Special MPR for PPF test      
        if (testLimit.isLimitTableUsed)
        {     
          V93kLimits::TMLimits::LimitInfo& limitInfo = V93kLimits::tmLimits.getLimit(
            param.testsuiteName,testLimit.testname[level]); 
          TESTSET().cont(TM::CONTINUE).testnumber(limitInfo.TestNumber)
                 .judgeAndLog_ParametricTest(param.expandedPins,
                                           testLimit.testname[level],
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
                                           testLimit.testname[level],
                                           isPass?TM::Pass : TM::Fail,
                                           measuredValueArray);
 
        }
      }
      else
      {
        isPass = result.resultPerSite[level].getGlobalPassFail(siteNumber);
        if (testLimit.isLimitTableUsed)
        {
          V93kLimits::TMLimits::LimitInfo& limitInfo = V93kLimits::tmLimits.getLimit(
            param.testsuiteName,testLimit.testname[level]);
          TESTSET().cont(TM::CONTINUE).testnumber(limitInfo.TestNumber)
                 .judgeAndLog_ParametricTest(param.pinlist,
                                             testLimit.testname[level],
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
                                           param.pinlist,
                                           testLimit.testname[level],
                                           isPass?TM::Pass : TM::Fail,
                                           0);

        }
      }/*end-if*/    
    }/*end-if*/
  }/*end-for*/

  /*Log functional result for whole test.*/
  TESTSET().cont(TM::CONTINUE)
           .judgeAndLog_ParametricTest(param.testsuiteName,
                                       "FUNCTIONAL TEST ",
                                       result.funcResult?TM::Pass:TM::Fail,
                                       0.0);
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
 *           testLimit          - test limits
 *           output             - "None" or "ReportUI" 
 *           result             - result container
 *   OUTPUT: 
 *   RETURN:  
 * Note:
 *----------------------------------------------------------------------*
 */ 
static void reportToUI(const TestParam& param,
                       const TestLimit& testLimit,
                       const STRING& output,
                       const TestResult& result)
{
  MeasuredLevelType level;
  INT times,siteNumber;
  Boolean isPass = true;
   
  if (output == "None")
  {
    return;
  }
  /*In case of ProgrammableLoad method, all sites' results are stored 
   *in result. ==> sitenumber = CURRENT_SITE_NUMBER();
   *Otherwise (PPMU and SPMU method), all sites' results are cached in
   *respective TestMethod APIs, and only the current site's is stored
   *in result. ==> sitenumber always is 0 for storing and retrieving 
   *in/from result.resultPerSite.
   */  
  if (param.mode != "ProgramLoad")
  {
    siteNumber = 0;
    CommonUtil::datalogToWindow(output,
                                     "outputDC (Functional result): ",
                                     result.funcResult);     
  }
  else
  {
    siteNumber =  CURRENT_SITE_NUMBER();    
  }
  
  if(param.measuredLevel == "LOW")
  {
    level = LOW_LEVEL;
    times = 1;
  }
  else if (param.measuredLevel == "HIGH")
  {
    times = 1;
    level = HIGH_LEVEL;
  }
  else /*BOTH*/
  {
    level = LOW_LEVEL;
    times = 2;  
  }

  for (INT i = 0; i < times; i++,level = HIGH_LEVEL)
  {
    if (level == LOW_LEVEL)
    {
      CommonUtil::printTestSuiteName(output, "outputDC (VOL)'", "'");
    }
    else
    {
      CommonUtil::printTestSuiteName(output, "outputDC (VOH)'", "'");
    }
    
    if (param.isValueTest)
    {
      DOUBLE value = 0.0;
      /* For PMU/SPMU value test, 
       * there is only "TM::PVAL" option available for value test,
       * and no Value per pinlist measurement.
       * Here always output value per pin for this case.
       */    
      if(param.isPerPinResult || param.mode != "ProgramLoad")
      {
        for(STRING_VECTOR::size_type index = 0; 
            index < param.expandedPins.size(); 
            ++index)
        {
          value = result.resultPerSite[level]
                        .getPinsValue(param.expandedPins[index],siteNumber); 
          CommonUtil::datalogToWindow(output,
                                      param.expandedPins[index],
                                      "V",
                                      value, 
                                      "",
                                      testLimit.passLimit[level]);
        }
      }
      else if (param.mode == "ProgramLoad" && !param.isPerPinResult)
      {
        value = result.resultPerSite[level]
                      .getPinsValue(param.pinlist,siteNumber);
        CommonUtil::datalogToWindow(output,
                                    param.pinlist,
                                    "V",
                                    value,
                                    "",
                                    testLimit.passLimit[level]);
      }
      else
      {
        cout<<"ERROR: OutputDCTest::reportToUI() "
              "doesn't know to output values with unknown case."
            <<endl;        
      }
    }
    else /*pass fail test*/
    {
      if(param.isPerPinResult)
      {
        for(STRING_VECTOR::size_type index = 0; 
            index < param.expandedPins.size(); 
            ++index)
        {
          isPass = result.resultPerSite[level]
                         .getPinPassFail(param.expandedPins[index],siteNumber);
          CommonUtil::datalogToWindow(output,param.expandedPins[index], isPass);                                           
        } 
      }
      else
      {
        isPass = result.resultPerSite[level].getGlobalPassFail(siteNumber);
        CommonUtil::datalogToWindow(output,param.pinlist, isPass);                                           
      }/*end-if*/    
    }/*end-if*/    
  }/*end-for*/
}

private:
/*
 *----------------------------------------------------------------------*
 * Routine: OutputDcTest()
 *
 * Purpose: private default constructor of OutputDcTest Class 
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *----------------------------------------------------------------------*
 */ 
OutputDcTest();

/*
 *----------------------------------------------------------------------*
 * Routine: setupActiveLoadMeasurement
 *
 * Purpose: construct FW commands string for active load measurement 
 *
 *----------------------------------------------------------------------*
 * Description:
 *   INPUT:  param              - test parameters
 *           measuredLevel      - LOW_LEVEL or HIGH_LEVEL
 *           primaryLogic0Level - primary VOL
 *           primaryLogic1Level - primary VOH
 *   OUTPUT: 
 *   RETURN: FW commands string for measurement
 * Note:
 *   1. works for each specific level: LOW_LEVEL or HIGH_LEVEL.
 *   2. must be called inside ON_FIRST_INVOCATION_BEGIN/END block,
 *      because FOR_EACH_SITE_BEGIN/END block is used below.
 *----------------------------------------------------------------------*
 */ 
static const STRING setupActiveLoadMeasurement(const TestParam& param, 
                                               const MeasuredLevelType measuredLevel,
                                               const STRING& primaryLogic0Level,
                                               const STRING& primaryLogic1Level)
{
  STRING rclsCmd = ""; /*local variable*/
  STRING testCmd = ""; /*returned variable*/
  DOUBLE commVoltage_mV = 0.0;
  DOUBLE Iol_uA = 0.0;
  DOUBLE Ioh_uA = 0.0;

  /*input measure leve*/
  if (param.measuredLevel == "LOW") 
  {
    Iol_uA = abs(param.forceCurrent_uA[LOW_LEVEL]);
    /*since if input measuredLevel is "LOW",
     *set Ioh same as Iol for TERM commands.
     */
    Ioh_uA = Iol_uA;
  }
  else if (param.measuredLevel == "HIGH")
  {
    Ioh_uA = abs(param.forceCurrent_uA[HIGH_LEVEL]);
    /*since if input measuredLevel is "HIGH",
     *set Iol same as Ioh for TERM commands.
     */
    Iol_uA = Ioh_uA;
  }
  else /*BOTH*/
  {
    Iol_uA = abs(param.forceCurrent_uA[LOW_LEVEL]);
    Ioh_uA = abs(param.forceCurrent_uA[HIGH_LEVEL]);
  }

  /*currently actual measure level*/
  if (measuredLevel == LOW_LEVEL)
  {
    commVoltage_mV = (param.forceCurrent_uA[LOW_LEVEL] < 0)?
                      (param.maxPassVal_V[LOW_LEVEL]*1000 - 1000)
                      :(param.maxPassVal_V[LOW_LEVEL]*1000 + 1000);
  }
  else
  {
    commVoltage_mV = (param.forceCurrent_uA[HIGH_LEVEL] < 0)?
                      (param.minPassVal_V[HIGH_LEVEL]*1000 - 1000)
                      :(param.minPassVal_V[HIGH_LEVEL]*1000 + 1000);
  }
  
  testCmd = "TERM PRM,A,";
  testCmd += CommonUtil::double2String(commVoltage_mV) + ","; 
  testCmd += CommonUtil::double2String(Iol_uA) + ",";         
  testCmd += CommonUtil::double2String(Ioh_uA) + ",";         
  testCmd += "AUTO,AUTO,("+ param.pinlist + ")\n"; 

  /*SETUP: RCLS or RCLV,FTST,PRLT*/     
  if (param.isValueTest && param.isPerPinResult) /*Valuer Per Pin*/
  {
    if (measuredLevel == LOW_LEVEL)
    {
      for (STRING::size_type index = 0; 
           index < param.expandedPins.size(); 
           ++index)
      {
        rclsCmd += "RCLS? ";
        rclsCmd += "L," + primaryLogic0Level + "," + primaryLogic0Level + ",";
        rclsCmd += "(" + param.expandedPins[index] + ")\n";  
      }
    }  
    else
    {
      for (STRING::size_type index = 0; 
           index < param.expandedPins.size(); 
           ++index)
      {
        rclsCmd += "RCLS? ";
        rclsCmd += "H," + primaryLogic1Level + "," + primaryLogic1Level + ",";
        rclsCmd += "(" + param.expandedPins[index] + ")\n";  
      }              
    }
    /*combine command with query site focus*/                                
    FOR_EACH_SITE_BEGIN();
      testCmd += "PQFC " 
                 +CommonUtil::double2String(CURRENT_SITE_NUMBER()) 
                 +"\n";
      testCmd += rclsCmd;                
    FOR_EACH_SITE_END();        
  }
  else if(param.isValueTest && !param.isPerPinResult) /*Valuer Per Pinlist*/
  {
    rclsCmd += "RCLS? ";
    if (measuredLevel == LOW_LEVEL)
    {
      rclsCmd += "L," + primaryLogic0Level + "," + primaryLogic0Level + ",";  
    }
    else
    {
      rclsCmd += "H," + primaryLogic1Level + "," + primaryLogic1Level + ",";  
    }        
    rclsCmd += "(" + param.pinlist + ")\n";
    /*combine command with query site focus*/                                
    FOR_EACH_SITE_BEGIN();
      testCmd += "PQFC " 
                 +CommonUtil::double2String(CURRENT_SITE_NUMBER()) 
                 +"\n";
      testCmd += rclsCmd;                
    FOR_EACH_SITE_END();        
  
  }
  else if(!param.isValueTest && param.isPerPinResult) /*PassFail Per Pin*/
  {
    if (measuredLevel == LOW_LEVEL)
    {
      for (STRING_VECTOR::size_type index = 0; 
           index < param.expandedPins.size(); 
           ++index)
      {
        rclsCmd += "RCLS? ";
        rclsCmd += "L," 
                   +CommonUtil::double2String(param.maxPassVal_V[LOW_LEVEL]*1e3) 
                   +",,";
        rclsCmd += "(" + param.expandedPins[index] + ")\n";
      } 
    }
    else
    {
      for (STRING_VECTOR::size_type index = 0; 
           index < param.expandedPins.size(); 
           ++index)
      {
        rclsCmd += "RCLS? ";
        rclsCmd += "H," 
                   +CommonUtil::double2String(param.minPassVal_V[HIGH_LEVEL]*1e3) 
                   +",,";
        rclsCmd += "(" + param.expandedPins[index] + ")\n";  
      }
    }
    /*combine command with query site focus*/ 
    FOR_EACH_SITE_BEGIN();
      testCmd += "PQFC " 
                 +CommonUtil::double2String(CURRENT_SITE_NUMBER()) 
                 +"\n";
      testCmd += rclsCmd;                
    FOR_EACH_SITE_END();            
         
  }
  else    /*GPF: Gloabl Pass Fail*/
  {
    if (measuredLevel == LOW_LEVEL)
    {
      testCmd += "RCLV PRM,";
      testCmd += CommonUtil::double2String(param.maxPassVal_V[LOW_LEVEL]*1e3)
                 +",";
      testCmd += primaryLogic1Level + ",";
    }
    else
    {
      testCmd += "RCLV PRM,";
      testCmd += primaryLogic0Level + ",";
      testCmd += CommonUtil::double2String(param.minPassVal_V[HIGH_LEVEL]*1e3) 
                 +",";      
    }
    testCmd += "("+param.pinlist+");";
    testCmd += "FTST?;PRLT? ALL\n";
    /*restore receive level settings*/
    testCmd += "RCLV PRM,"+primaryLogic0Level+","+primaryLogic1Level
               +",("+param.pinlist+")\n";
    
    /* NOTE!!!
     * Level set is changed by RCLV command which MUST be restored after this test,
     * and should set "UPTD LEV,1\n" anyway to avoid interruption to others.
     * NOTE!!!
     * this is done in doMeasurementByActiveLoad()
     */          
  }
  return testCmd;   
}

/*
 *----------------------------------------------------------------------*
 * Routine: processResultForActiveLoad
 *
 * Purpose: parse and record test result 
 *
 *----------------------------------------------------------------------*
 * Description:
 *   INPUT:  param  - test parameters
 *           answer - FW answer after measurement which is parsed.
 *           level  - LOW_LEVEL or HIGH_LEVEL
 *           
 *   OUTPUT: result - result container in which results are stored.
 *   RETURN: none
 * Note:
 *   1. works for each specific level: LOW_LEVEL or HIGH_LEVEL.
 *   2. must be called inside ON_FIRST_INVOCATION_BEGIN/END block,
 *      because FOR_EACH_SITE_BEGIN/END block is used below.
 *----------------------------------------------------------------------*
 */ 
static void processResultForActiveLoad(const TestParam& param,
                                       const STRING& answer,
                                       const MeasuredLevelType level,
                                       TestResult& result)
{
  STRING::size_type pos = 0, pos_end = 0;
  STRING resultToken = "";

  if (param.isValueTest && param.isPerPinResult)  /*Value Per Pin*/
  {
    FOR_EACH_SITE_BEGIN();
      for(STRING::size_type index = 0; index < param.expandedPins.size(); ++index)
      {
        pos = answer.find("RCLS",pos);
        if (pos != STRING::npos)
        {
          pos = static_cast<STRING::size_type>(answer.find_first_not_of(" \t",pos+4));
          pos_end = answer.find_first_of(",",pos+1);
          resultToken = answer.substr(pos,pos_end-pos);
          DOUBLE value = 0.0;
          if (resultToken == "EQ")
          {
            pos = retrieveStringWithinDelimiters(answer,
                                                ",",
                                                "\n",
                                                pos_end,
                                                resultToken);            
            /*put result value*/
            if (pos != STRING::npos)
            {
              value = CommonUtil::string2Double(resultToken,"resultToken");
              value = value * 1e-3; //convert value from 'mV' to 'V'                                      
            }                                    
          }
          else
          {           
            cout<<"RCLS? cannot get test value for pin(s): "
                <<param.expandedPins[index]<<endl;
            value = -DBL_MAX;
          }           
          result.resultPerSite[level].setPinsValue(param.expandedPins[index],
                                                   value,
                                                   CURRENT_SITE_NUMBER());
        }
      }  
    FOR_EACH_SITE_END();  
  }
  else if (param.isValueTest && !param.isPerPinResult)  /*Value Per Pinlist*/
  {
    FOR_EACH_SITE_BEGIN();        
      pos = answer.find("RCLS",pos);
      if (pos != STRING::npos)
      {
        pos = answer.find_first_not_of(" \t",pos+4);
        pos_end = answer.find_first_of(",",pos+1);
        resultToken = answer.substr(pos,pos_end-pos);
        DOUBLE value = 0.0;
        if (resultToken == "EQ")
        {
          pos = retrieveStringWithinDelimiters(answer,",","\n",pos_end,resultToken);            
          /*put result value*/
          if (pos != STRING::npos)
          {
            value = CommonUtil::string2Double(resultToken,"resultToken");                                      
            value = value * 1e-3; //convert value from 'mV' to 'V'                                      
          }                                    
        }
        else
        {           
          cout<<"RCLS? cannot get test value for pin(s): "
              <<param.pinlist<<endl;
          value = -DBL_MAX;
        }           
        result.resultPerSite[level].setPinsValue(param.pinlist,
                                                 value,
                                                 CURRENT_SITE_NUMBER());
      }
    FOR_EACH_SITE_END();  
  }
  else if (!param.isValueTest && param.isPerPinResult)  /*PPF: Pass Fail Per Pin*/
  {
    FOR_EACH_SITE_BEGIN();
      for(STRING_VECTOR::size_type index = 0; index < param.expandedPins.size(); ++index)
      {
        pos = answer.find("RCLS",pos);
        if (pos != STRING::npos)
        {
          pos = answer.find_first_of(" \t",pos+1);
          pos = answer.find_first_not_of(" \t",pos+1);
          pos_end = answer.find_first_of(",",pos+1);
          resultToken = answer.substr(pos,pos_end-pos);
          result.resultPerSite[level].setPinPassFail(param.expandedPins[index],
                                                     (resultToken == "P")?true:false,
                                                     CURRENT_SITE_NUMBER());
        }
      }  
    FOR_EACH_SITE_END();      
  }
  else                     /*GPF: Gloabl Pass Fail*/
  {
    pos = answer.find("PRLT",0);
    if (pos != STRING::npos)
    {
      retrieveStringWithinDelimiters(answer,"\"","\"",pos+1,resultToken);
      for (STRING::size_type index = 0; index < resultToken.length(); ++index)
      { 
        if (resultToken[index] != '0')
        {
          Boolean isPass = (resultToken[index] == 'P')?true:false;  
          /*the "index+1" of rclsResult is related to site number*/
          result.resultPerSite[level].setGlobalPassFail(isPass,index+1);
        }
        else
        {
          /*otherwise: the site is masked out*/
        }
      }    
    }
    else
    {        
      throw Error("OutputDcTest::processResultForActiveLoad()",
                  "cannot find result of PRLT? command",
                  "OutputDcTest::processResultForActiveLoad()");
    }    
  }
}
/*
 *----------------------------------------------------------------------*
 * Routine: excuteXPMUMeasurement
 *
 * Purpose: do PMU or SPMU measurement at the specific cycle.
 *          and store results for post-processing. 
 *
 *----------------------------------------------------------------------*
 * Description:
 *   INPUT:  cycleNumber - the n-th cycle.
 *           param       - test parameters
 *           testLimit   - test limit
 *           level       - LOW_LEVEL or HIGH_LEVEL
 *           pinlist     - pinlist to be measured
 *           xpmuMode    - "PPMUpar" or "SPMUser" 
 *   OUTPUT: result  - result container in which results are stored.
 *   RETURN: none
 * Note:
 *   works for each specific level: LOW_LEVEL or HIGH_LEVEL.
 *----------------------------------------------------------------------*
 */ 
static void excuteXPMUMeasurement(const INT cycleNumber,
                                  const TestParam& param,
                                  const TestLimit& testLimit,
                                  const MeasuredLevelType level,
                                  const STRING& pinlist,
                                  const STRING& xpmuMode,
                                  TestResult& result)
{
  /*measure for all measurement positions*/
  PPMU_MEASURE ppmuMeasure;
  SPMU_TASK spmuTask;

  ON_FIRST_INVOCATION_BEGIN();
    /*run to the specified cycle by cycleNumber*/ 
    FunctionalUtil::functionalPreTest("ToStopCYC",cycleNumber);
  ON_FIRST_INVOCATION_END(); 
     
  /*record functional result*/
  result.funcResult &= FunctionalUtil::getFunctionalTestResult();
    
  /*xPMU MEASUREMENT: at the specified position*/      
  ON_FIRST_INVOCATION_BEGIN();
    if (param.mode != "PPMU")
    {
      PmuUtil::pmuVoltageMeasurement(xpmuMode,
                                   pinlist,
                                   CommonUtil::getMode(),                    
                                   param.forceCurrent_uA[level],
                                   testLimit.passLimit[level],
                                   param.spmuClampVoltage_V[level],
                                   TURN_ON, /*precharge*/
                                   0.0,     /*precharge voltage value*/
                                   param.settlingTime_ms[level],
                                   "DEFAULT(BBM)",
                                   param.termination,
                                   ppmuMeasure,
                                   spmuTask,
                                   0.0,
                                   0.0);    
    }
    else
    {
      //use specific relay switch sequence for this PMU untermination mode.
      pmuUntermVoltageMeasurement(pinlist,
                                  CommonUtil::getMode(),
                                  param.forceCurrent_uA[level],
                                  testLimit.passLimit[level], 
                                  param.settlingTime_ms[level],
                                  0.0,     /*ppmu clamp low*/
                                  0.0,     /*ppmu clamp high*/
                                  ppmuMeasure);
    }
  ON_FIRST_INVOCATION_END();
    
  /*RESULT: retrieve results.*/      
  if (param.mode == "PPMU" || param.mode == "PPMUTerm")
  {
    processResultForXPMU(pinlist,param,ppmuMeasure,result,level);
  }
  else if (param.mode == "SPMU" || param.mode == "SPMUTerm")
  {
    processResultForXPMU(pinlist,param,spmuTask,result,level);
  }
  else
  {
    throw Error("OutputDcTest::excuteXPMUMeasurement()",
                "unknow meausrement mode: "+param.mode+".",
                "OutputDcTest::excuteXPMUMeasurement()"); 
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: processResultForXPMU
 *
 * Purpose: retrieve and store test results for PMU or SPMU measurement. 
 *
 *----------------------------------------------------------------------*
 * Description:
 *   INPUT:  pinlist  - the measured pinlist
 *           param    - test parameters
 *           xpmuTask - object of PPMU_MEASURE or SPMU_TASK.
 *           level    - LOW_LEVEL or HIGH_LEVEL 
 *   OUTPUT: results  - result container in which results are stored.
 *   RETURN: none
 * Note:
 *   xpmutTaskType must be 
 *          PPMU_MEASURE or SPMU_TASK, 
 *   which have same operation interfaces for retrieving test result.
 *----------------------------------------------------------------------*
 */ 
template<class xpmuTaskType>
static void processResultForXPMU(const STRING& pinlist,
                                 const TestParam& param,
                                 const xpmuTaskType&  xpmuTask,
                                 TestResult& results,
                                 MeasuredLevelType level)
{
  Boolean isPass = TRUE;
  STRING_VECTOR pinVector;

  switch ( CommonUtil::getMode() )
  {
    case TM::PVAL:
      CommonUtil::splitStr (pinlist,',',pinVector,false);
      for ( STRING_VECTOR::size_type j = 0; j< pinVector.size(); ++j )
      {
        DOUBLE dMeasValue = xpmuTask.getValue(pinVector[j]); /*[V]*/
        results.resultPerSite[level].setPinsValue(pinVector[j],dMeasValue);
      }
      break;

    case TM::PPF:
      CommonUtil::splitStr (pinlist,',',pinVector,false);
      for ( STRING_VECTOR::size_type j = 0; j< pinVector.size(); ++j )
      {
        results.resultPerSite[level].setPinPassFail(pinVector[j],
                                                    xpmuTask.getPassFail(pinVector[j]));    
      }
      break;

    case TM::GPF:
      /*aggregate total pass fail from different pinlist*/
      isPass = results.resultPerSite[level].getGlobalPassFail() && xpmuTask.getPassFail();
      results.resultPerSite[level].setGlobalPassFail(isPass);
      break;

    default:
      throw Error("OutputDcTest::processResultForXPMU()",
                  "Unknown Measure Mode.",
                  "OutputDcTest::processResultForXPMU()");
  }/*end-switch*/
}

/*
 *----------------------------------------------------------------------*
 * Routine: retrieveStringWithinDelimiters
 *
 * Purpose: retrieve substring within 'start' and 'stop' delimiters.
 *
 *----------------------------------------------------------------------*
 * Description:
 *   INPUT:  originalString - parsed string.
 *           startDelimiter - test parameters
 *           stopDelimiter  - LOW_LEVEL or HIGH_LEVEL
 *           startPos       - starting pos for searching startDelimiter. 
 *   OUTPUT: returnString   - target substring between start and stop
 *                            delimiters.
 *   RETURN: pos            - the position of stop delimiter.
 * Note:
 *   
 *----------------------------------------------------------------------*
 */ 
static STRING::size_type retrieveStringWithinDelimiters(const STRING& originalString,
                                                        const STRING& startDelimiter,
                                                        const STRING& stopDelimiter,
                                                        const STRING::size_type startPos, 
                                                        STRING& returnString)
{
  STRING::size_type startDelimPos = STRING::npos;
  STRING::size_type stopDelimPos = STRING::npos;
  returnString = "";
  startDelimPos = originalString.find_first_of(startDelimiter,startPos);
  stopDelimPos = originalString.find_first_of(stopDelimiter,startDelimPos+1);
  if (startDelimPos != STRING::npos && stopDelimPos != STRING::npos)
  {
    returnString = originalString.substr(startDelimPos+1,stopDelimPos-startDelimPos-1);    
  }
  return stopDelimPos;    
}

/*
 *----------------------------------------------------------------------*
 * Routine: searchOutputLevelByOUTS
 *
 * Purpose: search Low and/or High output level by OUTS?.
 *
 *----------------------------------------------------------------------*
 * Description:
 *   INPUT:  param                - test parameters
 *
 *   OUTPUT: measurePositions      - map of measurement positions of pins
 *   RETURN: None.
 * Note:
 *   
 *----------------------------------------------------------------------*
 */ 
static void searchOutputLevelByOUTS(const TestParam& param,
                                    map<INT,map<INT,STRING> >& measurePositions)
{  
  STRING testCmd,answer;  
  STRING::size_type pos = 0;
  STRING token;
  INT cycNum = 0;
  
  STRING vecNum1,vecNum2;
  STRING searchType;

  /*process vector range*/
  if (param.mode != "ProgramLoad") /*means to use PMU or SPMU*/
  {
    STRING_VECTOR numVec;
    if (param.vectorRange.empty())
    {
      throw Error("OutputDcTest::searchOutputLevelByOUTS()",
                  "Miss searched vector range parameter.",
                  "OutputDcTest::searchOutputLevelByOUTS()");
    }
    /*Just separate the input by ',', '-'. OUTS? can validate the number.*/      
    else if ((pos = param.vectorRange.find_first_of(",",0)) != STRING::npos)
    {
      /*two number: num1,num2*/
      CommonUtil::splitStr (param.vectorRange,',',numVec,false);
      vecNum1 = CommonUtil::trim(numVec[0]);
      vecNum2 = CommonUtil::trim(numVec[1]);
      searchType = "SNGL";
    }
    else if ((pos = param.vectorRange.find_first_of("-",0)) != STRING::npos)
    {
      /*range from num1 to num2: num1-num2*/
      CommonUtil::splitStr (param.vectorRange,'-',numVec,false);
      vecNum1 = CommonUtil::trim(numVec[0]);
      vecNum2 = CommonUtil::trim(numVec[1]);
      searchType = "RNG";        
    }
    else
    {
      /*only start number: num1*/
      vecNum1 = param.vectorRange;
      vecNum2 = "";
      searchType = "DPTH";                
    } 
  }
  
  /*QUERY: measurement positions*/
  testCmd = "OUTS? ";
  testCmd += vecNum1 + ",";
  testCmd += vecNum2 + ",";
  testCmd += searchType+",";
  testCmd += param.measuredLevel+",";
  testCmd += "("+PinUtility.createPinListFromPinNames(param.expandedPins)+")";
  testCmd += "\n";
  FW_TASK(testCmd,answer);
  
  /*PARSE: measurement points*/
  pos = 0;
  MeasuredLevelType level;
  while((pos = answer.find("OUTS",pos)) != STRING::npos)
  {
    /*get cycle number*/
    pos = retrieveStringWithinDelimiters(answer," ",",",pos,token);    
    cycNum = (INT) CommonUtil::string2Double(token,"OUTS amount number till this cycles");
    cycNum = cycNum -1;  /*convert amount of cycles to the cycle number*/
    /*get: LOW,HIGH*/
    pos = retrieveStringWithinDelimiters(answer,",",",",pos,token);
    if (token == "LOW")
    {
      level = LOW_LEVEL;   
    }
    else if(token == "HIGH")
    {
      level = HIGH_LEVEL;   
    }
    else
    {
      throw Error("OutputDcTest::searchOutputLevelByOUTS()",
                  "invalid OUTS results.",
                  "OutputDcTest::searchOutputLevelByOUTS()");
    }          
    /*get pinlist*/
    pos = retrieveStringWithinDelimiters(answer,"(",")",pos,token);
    if ((measurePositions[cycNum][level]).size() != 0)    
    {
      /*combine pins with same cycle number*/
      measurePositions[cycNum][level] += ","+token;
    }
    else
    {
      measurePositions[cycNum][level] = token;
    }  
  }/*end-while*/
}

/*
 *----------------------------------------------------------------------*
 * Routine: pmuUntermVoltageMeasurement 
 *
 * Purpose: PMU unterminated voltage measurement. 
 *
 *----------------------------------------------------------------------*
 * Description:
 *   INPUT:  
 *
 *   OUTPUT: 
 *   RETURN: None.
 * Note:
 *  To support output_dc on PS400 card with PMU unterminated,
 *  this function use specific relay switch sequency (BBM) :
 *           AC_OFF
 *           PPMU_ON
 *        ...measure...
 *           PPMU_OFF
 *           AC_ON
 *  instead of OAC_OFF/ON OPM_ON/OFF which are not supported on PS400.
 *----------------------------------------------------------------------*
 */
static void pmuUntermVoltageMeasurement(const STRING&         pinList,
                                        const TM::DCTEST_MODE& testMode,
                                        const DOUBLE          forceCurrent,
                                        const LIMIT&          limit,
                                        const DOUBLE          settlingTime,
                                        const DOUBLE          ppmuClampLow,
                                        const DOUBLE          ppmuClampHigh,
                                        PPMU_MEASURE&         measurePpmu)
{
  PPMU_SETTING   settingPpmu;
  TASK_LIST   tasklist;
  DOUBLE currentRange = fabs(forceCurrent);
  DOUBLE settling = settlingTime + 1; /* 1 PPMU settling time */
  
  PPMU_CLAMP ppmuClampOn;
  PPMU_CLAMP ppmuClampOff;
  PPMU_SETTING settingPpmuPrecharge;

  /*  1. set the clamp (PinScale) */
  DOUBLE low, high, clampLow, clampHigh;

  limit.getLow(&low);
  limit.getHigh(&high);

  settingPpmu.pin(pinList)
             .iRange(currentRange uA)
             .min(low)
             .max(high)
             .iForce(forceCurrent uA);


  
  tasklist.add(settingPpmu);
  /* if input clamp is 0 (default), then set them as limit high/low */
  clampLow = (fabs(ppmuClampLow) < 1e-38)?low : ppmuClampLow;
  clampHigh = (fabs(ppmuClampHigh) < 1e-38)?high : ppmuClampHigh;
  ppmuClampOn.pin(pinList).status("CLAMP_ON").low(clampLow V).high(clampHigh V);  
  ppmuClampOn.wait(1 ms);
  tasklist.add(ppmuClampOn);
  
  /*  2.relay switch */

  PPMU_RELAY     relayPpmuOn;
  PPMU_RELAY     relayAcOff;
  relayAcOff.pin(pinList).status("AC_OFF");
  relayPpmuOn.pin(pinList).status("PPMU_ON");
  relayAcOff.wait(0.3 ms);
  relayPpmuOn.wait((settlingTime + 0.3) ms);
  tasklist.add(relayAcOff).add(relayPpmuOn);

  /* 3. switch clamp off */
  ppmuClampOff.pin(pinList).status("CLAMP_OFF");
  ppmuClampOff.wait(settling ms);
  tasklist.add(ppmuClampOff);
  
  /*  4. determine measure */
  /* PVAL or PPF or GPF is defined by TestSuite Flag */
  measurePpmu.pin(pinList).execMode(  testMode );
  tasklist.add(measurePpmu);

  /* 5.relay restore */

  PPMU_RELAY     relayPpmuOff;
  PPMU_RELAY     relayAcOn;
  relayPpmuOff.pin(pinList).status("PPMU_OFF");
  relayPpmuOff.wait(0.3 ms);
  relayAcOn.pin(pinList).status("AC_ON");
  relayAcOn.wait(0.3 ms);
  tasklist.add(relayPpmuOff).add(relayAcOn);

  /*  6.task list execute */
  tasklist.execute();
}
};

#endif /*OUTPUTDCTEST_H_*/
