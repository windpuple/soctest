#ifndef INCLUDED_Functional
#define INCLUDED_Functional
#include "CommonUtil.hpp"
#include <algorithm>

#define UNBURST_OFF     0
#define UNBURST_ON_FAIL 1
#define UNBURST_ON      2

class FunctionalUtil
{
public:
  struct LogInfo
  {
    bool isLimitTableUsed;
    int testNumber;
    string testName;
    string softBinNumberString;
    int    hardBinNumber;
    LogInfo():isLimitTableUsed(false),testNumber(0),hardBinNumber(-1)
    {
    }
  };
  static void logFunctionalTest(const LogInfo& logInfo);
  static void unburstAndExecute(STRING strStartLabel, INT iUnburstMode, const LogInfo& logInfo);
  static void functionalPreTest(const STRING& type, 
                                const INT number = 0,
                                const STRING& portName = "");
  static void runSequencer(TM::RUN_STATUS status = TM::NORMAL);
  static Boolean isSequencerRunning();
  static void abortSequencer(); 
  static Boolean getFunctionalTestResult();
  static void funcEvaluateResult(
              const Boolean funcResult,
              const STRING& output,
              const INT stopVecOrCyc = 0,
              const STRING& info = "");
              
  static void evaluateFunctionalTestResult(
              const STRING& output,
              const STRING& pins,
              const TM::DCTEST_MODE& mode);
              
  static void maskFunctionalTest(
                 const STRING& runTo,
                 const Boolean isRunToCycle,
                 const STRING& maskBefore,
                 const Boolean isMaskBeforeCycle,
                 const STRING& maskAfter,
                 const Boolean isMaskAfterCycle,
                 const STRING& port,
                 STRING& setMaskModeFwCmd,
                 STRING& restoreMaskModeFwCmd);                          
};

/*
 *----------------------------------------------------------------------*
 * Routine: FunctionalUtil::logFunctionalTest
 *
 * Purpose: log functional test result.
 *
 *----------------------------------------------------------------------*
 * Description:
 *   support the limit defined in the limit cvs file and added in 
 *   limit section in property view of testsuite.
 *----------------------------------------------------------------------*
 */
inline void FunctionalUtil::logFunctionalTest(const LogInfo& logInfo)
{
  //use logInfo.testName to overwrite the default name in 
  //judgeAndLog_FunctionalTest(), i.e. "DEFAULT FUNCTIONAL TEST"
  if (logInfo.isLimitTableUsed)
  {
    bool pass = TESTSET().cont(true)
                         .testnumber(logInfo.testNumber)
                         .testname(logInfo.testName)
                         .combineMultiPortTests(true)
                         .judgeAndLog_FunctionalTest();
    if(!pass && logInfo.softBinNumberString.size() > 0)
    {
      SET_MULTIBIN(logInfo.softBinNumberString,logInfo.hardBinNumber);
    }
  }
  else
  {
    TESTSET().cont(true).testnumber(logInfo.testName,true).combineMultiPortTests(true).judgeAndLog_FunctionalTest();
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: FunctionalUtil::unburstAndExecute
 *
 * Purpose: Unburst the burst label and execute functional test.
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 *----------------------------------------------------------------------*
 */
inline void FunctionalUtil::unburstAndExecute(
  STRING strStartLabel, INT iUnburstMode,
  const LogInfo& logInfo)
{
  static STRING_VECTOR stvUnburstLabels;
  ON_FIRST_INVOCATION_BEGIN();
    stvUnburstLabels.clear();
    CommonUtil::getUnburstLabels(strStartLabel,stvUnburstLabels);
  ON_FIRST_INVOCATION_END();    

  if( (iUnburstMode == UNBURST_OFF) || (stvUnburstLabels.empty()) )
  {
    if(stvUnburstLabels.empty())
    {
      ON_FIRST_INVOCATION_BEGIN();
        CONNECT();
        Sequencer.reset();
        FUNCTIONAL_TEST();
      ON_FIRST_INVOCATION_END();
      logFunctionalTest(logInfo);
      return;          
    }
    else
    {
      throw Error("FunctionalUtil::unburstAndExecute",
                  "Unburst mode is not ON!");
    }
  }
  else if (iUnburstMode == UNBURST_ON)
  {
    for(STRING_VECTOR::iterator itUnbLabel= stvUnburstLabels.begin();
        itUnbLabel != stvUnburstLabels.end();
        itUnbLabel++)
    {
      ON_FIRST_INVOCATION_BEGIN();
        Primary.label(*itUnbLabel);
        CONNECT();
        Sequencer.reset();
        FUNCTIONAL_TEST();
      ON_FIRST_INVOCATION_END();
      logFunctionalTest(logInfo);
    }          
  }
  else if (iUnburstMode == UNBURST_ON_FAIL)
  {
    /* Hold the failed sites */
    static vector<INT> ivIndicator;
    ON_FIRST_INVOCATION_BEGIN();
      ivIndicator.clear();
      CONNECT();
      Sequencer.reset();
      FUNCTIONAL_TEST();
      FOR_EACH_SITE_BEGIN();
        if(!GET_FUNCTIONAL_RESULT())
        {
          ivIndicator.push_back(CURRENT_SITE_NUMBER());
        }
      FOR_EACH_SITE_END();
    ON_FIRST_INVOCATION_END();
    logFunctionalTest(logInfo);
               
    if(!ivIndicator.empty())
    {
      for(STRING_VECTOR::iterator itUnbLabel= stvUnburstLabels.begin();
          itUnbLabel != stvUnburstLabels.end();
          itUnbLabel++)
      {
        ON_FIRST_INVOCATION_BEGIN();
          Primary.label(*itUnbLabel);
          CONNECT();
          Sequencer.reset();
          FUNCTIONAL_TEST();
        ON_FIRST_INVOCATION_END();
        /* Get result for cache label infomation */
        GET_FUNCTIONAL_RESULT();
        /* Only log the failed sites results */
        if(binary_search(ivIndicator.begin(),ivIndicator.end(),CURRENT_SITE_NUMBER()))
        {
          logFunctionalTest(logInfo);
        }
      }
    }
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: FunctionalUtil::functionalPreTest
 *
 * Purpose: run a functional pattern with different sequencer setting
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *   Used in the Leakage testing
 *----------------------------------------------------------------------*
 */
inline void FunctionalUtil::functionalPreTest (
                                    const STRING& type, 
                                    const INT number,
                                    const STRING& portName )
{
  if ( type == "ALL" )
  {
    Sequencer.reset();
    FUNCTIONAL_TEST();
  } 
  else  if ( type == "ToStopVEC" )
  {
    Sequencer.stopVector(number);
    FUNCTIONAL_TEST();
  } 
  else if ( type == "ToStopCYC" )
  {
    if(portName.empty())
    {
      Sequencer.stopCycle(number);
    }
    else
    {
      Sequencer.stopCycle(number,portName);
    }
    FUNCTIONAL_TEST();
  } 
  else  if ( type == "NO" )
  {
    /* Do nothing here.*/
  } 
  else
  {
    throw Error("FunctionalUtil::functionalPreTest",
                "Wrong Pretest Type.");
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: FunctionalUtil::runSequencer
 *
 * Purpose: run the sequencer, in normal or endless mode
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *   Used in the operating current
 *----------------------------------------------------------------------*
 */
inline void FunctionalUtil::runSequencer(TM::RUN_STATUS status)
{
  Sequencer.run(status);
}

/*
 *----------------------------------------------------------------------*
 * Routine: FunctionalUtil::isSequencerRunning
 *
 * Purpose: is the sequencer running or not. if running, return true
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 *----------------------------------------------------------------------*
 */
inline Boolean FunctionalUtil::isSequencerRunning()
{
  return Sequencer.isRunning();
}

/*
 *----------------------------------------------------------------------*
 * Routine: FunctionalUtil::abortSequencer
 *
 * Purpose: abort the sequncer, that's, let the sequencer send break
 *          waveform to DUT
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 *----------------------------------------------------------------------*
 */
inline void FunctionalUtil::abortSequencer()
{
  Sequencer.abort();
}

/*
 *----------------------------------------------------------------------*
 * Routine: FunctionalUtil::getFunctionalTestResult
 *
 * Purpose: get functional test result.
 *
 *----------------------------------------------------------------------*
 * Description:
 *    
 *----------------------------------------------------------------------*
 */
inline Boolean FunctionalUtil::getFunctionalTestResult()
{
  return GET_FUNCTIONAL_RESULT();
}

/*
 *----------------------------------------------------------------------*
 * Routine: FunctionalUtil::funcEvaluateResult
 *
 * Purpose: evaluate functional test result of each site.
 *
 *----------------------------------------------------------------------*
 * Description:
 *    currently, it is used by 
 *      leakage,highZ,productionIddq
 *    to check if functional test pass or fail.
 *    @input parameters:
 *      funcResult: functional test result.
 *      stopVecOrCycNum: the stop number of vector or cycle to be logged.
 *      output: None or ReportUI.
 *      info: extra information to log,
 *            maybe "high","low" for leakage test; maybe "" for others.
 * Note: 
 *   
 *----------------------------------------------------------------------*
 */
inline void FunctionalUtil::funcEvaluateResult(
              const Boolean funcResult,
              const STRING& output,
              const INT stopVecOrCyc, /* default is 0 */
              const STRING& info   /* default is "" */
              )
{
  STRING testSuiteName;

  GET_TESTSUITE_NAME(testSuiteName);
  TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(
      testSuiteName,
      "FUNCTION TEST "+info,
      funcResult?TM::Pass:TM::Fail,
      stopVecOrCyc);
  CommonUtil::datalogToWindow(output,"Functional result: ",funcResult);

}

/*
 *----------------------------------------------------------------------*
 * Routine: FunctionalUtil::evaluateFunctionalTestResult
 *
 * Purpose: evaluate and log the result of generic functional test
 *
 *----------------------------------------------------------------------*
 * Description:
 *   Use TestSet system object to do datalog.
 *   Output test result to ReportUI window if requested.
 * Input parameters:
 *
 * Output: 
 *
 * Note: 
 *    
 *----------------------------------------------------------------------*
 */
inline void FunctionalUtil::evaluateFunctionalTestResult(
              const STRING& output,
              const STRING& pins,
              const TM::DCTEST_MODE& mode)
{
  Boolean isGlobalPass = GET_FUNCTIONAL_RESULT();
  STRING testsuiteName;
  GET_TESTSUITE_NAME(testsuiteName);
//  INT nSiteNumber = CURRENT_SITE_NUMBER();
  /*internally log PPF/GPF with flags of the testsuite*/
  TESTSET().cont(TM::CONTINUE).judgeAndLog_FunctionalTest();
  switch(mode)
  {
    case TM::PVAL:  /* same as PPF */
    case TM::PPF:            
      if ( output ==  "ReportUI" )
      {
        /*expand pinlist into vector of pins*/
        STRING_VECTOR vecPinList = PinUtility.getDigitalPinNamesFromPinList(
                                  pins,
                                  TM::O_PIN|TM::IO_PIN,
                                  TRUE,
                                  (pins=="@")?FALSE:TRUE);
        STRING_VECTOR::const_iterator it;
        for ( it = vecPinList.begin(); it != vecPinList.end(); ++it )
        {
          Boolean isPass = isGlobalPass?true:GET_FUNCTIONAL_RESULT(*it);
          CommonUtil::datalogToWindow( output, *it, isPass );
        }
      }
      break;

    case TM::GPF:
      CommonUtil::datalogToWindow( output, pins, isGlobalPass );
      break;

    default:
      throw Error("CommonUtil::evaluateFunctionalTestResult",
                  "Unknown Output Mode!",
                  "CommonUtil::evaluateFunctionalTestResult");
      break;
  }  
}

/*
 *----------------------------------------------------------------------*
 * Routine: FunctionalUtil::maskFunctionalTest
 *
 * Purpose: make fw command string for customizing functional test.
 *
 *----------------------------------------------------------------------*
 * Description:
 *   Fill the 'FSTM' fw command if runto,mask before,and mask after are
 *   available. And fw format:
 *   FTSM SET,[runto],[mask_b],[mask_a][,(port)]
 * 
 * Parameters:
 *   isRunToCycle: true for cycle, false for vector
 *   maskBefore: string of mask before number
 *   isMaskBeforeCycle: true for cycle, false for vector
 *   maskAfter: string of mask after number
 *   isMaskAfterCycle: true for cycle, false for vector
 *   port: port name
 *   setMaskModeFwCmd: generated fw command for mask functional test.
 *   restoreMaskModeFwCmd: fw command to restore mask functionalt test    
 *     
 * Output: 
 *   None.
 *
 * Note:
 * 
 *   Assume that runTo, maskBefore, maskAfter are trimed strings!!
 *----------------------------------------------------------------------*
 */
inline void FunctionalUtil::maskFunctionalTest(
                 const STRING& runTo,
                 const Boolean isRunToCycle,
                 const STRING& maskBefore,
                 const Boolean isMaskBeforeCycle,
                 const STRING& maskAfter,
                 const Boolean isMaskAfterCycle,
                 const STRING& port,
                 STRING& setMaskModeFwCmd,
                 STRING& restoreMaskModeFwCmd)
{
  Boolean isRunToGiven = runTo.empty()?false:true;
  Boolean isMaskBeforeGiven = maskBefore.empty()?false:true;
  Boolean isMaskAfterGiven = maskAfter.empty()?false:true; 
  
  /*initialize fw string of mask mode*/
  setMaskModeFwCmd = "";                  
  restoreMaskModeFwCmd = "";
  
  /*construct fw string of mask mode*/
  if ( isRunToGiven || isMaskBeforeGiven || isMaskAfterGiven )
  { 
    setMaskModeFwCmd = "FTSM SET,";

    /*run to*/
    if ( isRunToGiven )
    {
      setMaskModeFwCmd += isRunToCycle? 
         runTo : CommonUtil::convertVectorNumToCycleNum(runTo);        
    }
    setMaskModeFwCmd += ",";
    /*mask before*/
    if ( isMaskBeforeGiven )
    {
      setMaskModeFwCmd += isMaskBeforeCycle?
         maskBefore : CommonUtil::convertVectorNumToCycleNum(maskBefore);        
    }
    setMaskModeFwCmd += ",";
    /*mask after*/
    if ( isMaskAfterGiven )
    {
      setMaskModeFwCmd += isMaskAfterCycle?
         maskAfter : CommonUtil::convertVectorNumToCycleNum(maskAfter);             
    }
    
    /*port name*/
    if ( !port.empty() )
    {
      setMaskModeFwCmd += ",("+ port +")\n";
      restoreMaskModeFwCmd = "FTSM CLEAR,,,,(" + port + ")\n";
    }
    else
    {
      setMaskModeFwCmd += "\n";
      restoreMaskModeFwCmd = "FTSM CLEAR,,,\n";
    }
  }    
}

#endif
