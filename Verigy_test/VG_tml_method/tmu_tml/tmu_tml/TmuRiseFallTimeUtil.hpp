#ifndef TMURISEFALLTIME_H_
#define TMURISEFALLTIME_H_

#include "TMUTask.h"
#include <float.h>





class TmuRiseFallTimeUtil
{

public:  

  /*
   *----------------------------------------------------------------------*
   * Struct: RiseFallTimeParameter
   *
   * Purpose: Container to store rise/fall time measurement parameters
   *
   *----------------------------------------------------------------------*
   * Description:
   *   The members in this structure can be categorized into 3 groups
   *     1. specified by a user
   *     2. specified by a user, possibly modified by processParameters()
   *     3. set by processParameters()
   * 
   *   The following parameters belong to group 1.
   *
   *   STRING pins:                  {@ | pin and/or pin group list}
   *     Name of pin(s) and/or pin group(s) to be measured
   *     Valid pins: all digital pins.
   *   LONG datarate              {}
   *      max expected datarate of signal to be maesured
   *   INT samples_per_maes:                     {}
   *     # of samples to be taken
   *   DOUBLE aThreshold:               {mV}
   *     Value of low threshold voltage.
   *   DOUBLE bThreshold:               {mV}
   *     Value of high threshold voltage.
   *   INT ftstResult:                       {0 | 1}
   *     Flag to specify whether taking functional test result into account
   *     for pass/fail judging and datalogging.
   *
   * Note:
   *   When you create an instance of this class, appropriate default values
   *   except pins are set to each variables in the class.
   *
   *----------------------------------------------------------------------*
   */
  struct RiseFallTimeParameter
  {
    // Parameters specified by a user
    STRING pins;
    UINT64 datarate;
    UINT32 prescaler;
    UINT64 samples_per_meas;
    DOUBLE treshold_a;
    DOUBLE treshold_b;
    UINT64 initial_discard;
    UINT64 inter_sample_discard;
    Boolean exitOnNotFinished;
    double  waitTimeout;
    INT ftstResult;

    // Parameters specified by a user, possibly modified by processParameters

    // Parameters set by processParameters()

    // Default constructor
    // all parameters are set to default values
    RiseFallTimeParameter()
    : pins(""),
      datarate(0),
      prescaler(2),
      samples_per_meas(2),
      treshold_a(250.0),
      treshold_b(1250.0),
      initial_discard(0),
      inter_sample_discard(0),
      exitOnNotFinished(FALSE),
      waitTimeout(0.0),
      ftstResult(1)
      {}
  };

  class RiseFallData
  {
  public:
    Wave<DOUBLE> risewave;
    Wave<DOUBLE> fallwave;

    // all real values are always lower then DBL_MAX so init with DBL_MAX
    RiseFallData(): mMinRise(DBL_MAX),mMaxRise(DBL_MAX), mMinFall(DBL_MAX),mMaxFall(DBL_MAX)
                    ,mMeanrise(DBL_MAX),mMeanfall(DBL_MAX),mRms_rise(DBL_MAX),mRms_fall(DBL_MAX)
                    ,mPp_rise(DBL_MAX),mPp_fall(DBL_MAX){}

    // use this methods to access the results
    DOUBLE min_Rise()  { if(mMinRise < DBL_MAX) return mMinRise; else mMinRise  = risewave.min(); return mMinRise; }
    DOUBLE max_Rise()  { if(mMaxRise < DBL_MAX) return mMaxRise; else mMaxRise  = risewave.max(); return mMaxRise; }
    DOUBLE min_Fall()  { if(mMinFall < DBL_MAX) return mMinFall; else mMinFall  = fallwave.min(); return mMinFall; }
    DOUBLE max_Fall()  { if(mMaxFall < DBL_MAX) return mMaxFall; else mMaxFall  = fallwave.max(); return mMaxFall; }
    DOUBLE mean_Rise() { if(mMeanrise< DBL_MAX) return mMeanrise;else mMeanrise = risewave.mean();return mMeanrise; }
    DOUBLE mean_Fall() { if(mMeanfall< DBL_MAX) return mMeanfall;else mMeanfall = fallwave.mean();return mMeanfall; }
    DOUBLE rms_Rise()  { if(mRms_rise< DBL_MAX) return mRms_rise;else mRms_rise = risewave.rms(); return mRms_rise; }
    DOUBLE rms_Fall()  { if(mRms_fall< DBL_MAX) return mRms_fall;else mRms_fall = fallwave.rms(); return mRms_fall; }
    DOUBLE pp_Rise()   { if(mPp_rise < DBL_MAX) return mPp_rise; else mPp_rise  = risewave.max() -risewave.min(); return mPp_rise; }
    DOUBLE pp_Fall()   { if(mPp_fall < DBL_MAX) return mPp_fall; else mPp_fall  = fallwave.max() -fallwave.min(); return mPp_fall; }

  private:
    DOUBLE mMinRise;
    DOUBLE mMaxRise;
    DOUBLE mMinFall;
    DOUBLE mMaxFall;
    DOUBLE mMeanrise;
    DOUBLE mMeanfall;
    DOUBLE mRms_rise;
    DOUBLE mRms_fall;
    DOUBLE mPp_rise;
    DOUBLE mPp_fall;

  };

  class RiseFallTimeResult :  public TMU_TML_RESULT_BASE
  {
  public:
    std::map < STRING, RiseFallData > pinRiseFallResults;
    TMU::ApplicationType appType;

    RiseFallTimeResult() : TMU_TML_RESULT_BASE(),appType(TMU::APP_RAW) {}
  };



  /*
   *----------------------------------------------------------------------*
   * Utility functions for Pulse Width measurements
   *----------------------------------------------------------------------*
   */

  /*
   *----------------------------------------------------------------------*
   * Routine: TmuRiseFallTimeUtil::processParameters
   *
   * Purpose: Store given measurement parameters to the specified
   *          placeholder and determine addtional parameters
   *          which is necessary to execute measurement.
   *          Also performs some error checks on parameters.
   *
   *----------------------------------------------------------------------*
   * Description:
   *   const STRING& pins:
   *   const STRING& mode:           {TR | TF}
   *   LONG datarate                     {}
   *     # max expected datarate of signal to be maesured
   *     # if omitted prescaler and deglitcher are used
   *   INT     prescaler;            {0..16}
   *   STRING& signaltype            {CLK | DATA}
   *
   *   INT meas_per_shot                 {}
   *     # number of armings that should be made according to the
   *     # wavetable equation based setup
   *   INT samples_per_meas:             {}
   *     # of samples to be taken
   *     # this value is for setup
   *   INT& numshots                     {}
   *     number of shots to be make
   *   double  treshold_a;               {V}
   *   double  treshold_b;               {V}
   *     # the treshold values for the shot 1 and shot 2
   *   int     initial_discard;          {}
   *     # number od sample to discard at beginning
   *   int     inter_sample_discard;     {}
   *     # number od events to discard between to samples
   *   DOUBLE rise_min
   *   DOUBLE rise_max
   *   DOUBLE fall_min
   *   DOUBLE fall_max
   *   INT ftstResult:               {0 | 1}
   *     Those are parameters for rise/fall time measurement.
   *     See the descriptions in RiseFallTimeParameter definition.
   *   RiseFallTimeParameter& params:
   *     Container to hold parameters for the measurement.
   *   
   * Note:
   *
   *----------------------------------------------------------------------*
   */
  static void processParameters(
                          const STRING& pins,
                          UINT64 datarate,
                          UINT32 prescaler,
                          UINT64 samples_per_meas,
                          DOUBLE treshold_a,
                          DOUBLE treshold_b,
                          UINT64 initial_discard,
                          UINT64 inter_sample_discard,
                          INT    exitOnNotFinished,
                          double waitTimeout,
                          INT ftstResult,
                          RiseFallTimeParameter& params)
  {

    // If no pin for pins is specified, an exception is thrown.
    if (pins.size() == 0)
    {
      throw Error("TmuRiseFallTimeUtil::processParameters()",
          "Empty pins parameter.");
    }

    // Copy other parameters
    params.pins              = pins;
    params.datarate          = datarate;
    params.samples_per_meas  = samples_per_meas;
    params.ftstResult        = ftstResult;
    params.prescaler         = prescaler;
    params.treshold_a      	 = treshold_a;
    params.treshold_b     	 = treshold_b;
    params.initial_discard   = initial_discard;
    params.inter_sample_discard = inter_sample_discard;
    params.ftstResult        = ftstResult;
    params.exitOnNotFinished = (exitOnNotFinished == 1)?TRUE:FALSE;
    params.waitTimeout		= waitTimeout;


    // params.measmode1 and  params.measmode2 
    // are set default by constructor 

  }

  /*
   *----------------------------------------------------------------------*
   * Routine: TmuRiseFallTimeUtil::doMeasurement
   *
   * Purpose: Perform setup, execution and result retrieval
   *          for rise/fall time measurement with per pin TIA
   *
   *----------------------------------------------------------------------*
   *
   * Description:
   *   const RiseFallTimeParameter& params:
   *     Container to hold parameters for the measurement.
   *   RiseFallTimeResult& results:
   *     Container to store measurement results
   *   
   * Note:
   *   'static' variables are used in this function to keep some information
   *   which is refered in the execution for sites where ON_FIRST_INVOCATION 
   *   block is not executed.
   *
   *----------------------------------------------------------------------*
   */
  static void doMeasurement(const RiseFallTimeParameter& params,
      RiseFallTimeResult& results)
  {
    // Measurement setup, execution and result retrieval are done
    // through task object
	TMU_RESULTS siteResults;
    STRING ErrorString;
    Boolean exception_caught = FALSE;
    Boolean finished[2] = {FALSE,FALSE};
    try
    {
      TMU_TASK task;
      CONNECT();

      ON_FIRST_INVOCATION_BEGIN();
      //avoid ON_FIRST_INVOCATION_END executed so catch exceptions
      try
      {

        if(params.datarate > 0)
          task.pin(params.pins).setDatarate(params.datarate);
        else
        {
          task.pin(params.pins).setPreScaler(params.prescaler)
                               .setInterSampleDiscard(params.inter_sample_discard);
        }

        task.pin(params.pins).setEdgeSelect(TMU::RISE_FALL)
                             .setNumMeasurements(1)
                             .setNumShots(2)
                             .setNumSamples(params.samples_per_meas)
                             .setInitialDiscard(params.initial_discard);

        if(params.treshold_a != 0)
          task.pin(params.pins).setAThreshold(params.treshold_a);
        if((params.treshold_b != 0))
          task.pin(params.pins).setBThreshold(params.treshold_b);

        /*
         * TMU measurement execution
         */
        task.setup();
        task.setAsPrimary();
        STRING answer;
        // loop over shots
        Boolean allfinished = TRUE;
        for(UINT32 i = 0;i < 2;i++)
        {
          FUNCTIONAL_TEST();//task.execute();
          task.pin(params.pins).waitTMUfinished(params.waitTimeout,finished[i]);
          if(!finished[i])
          {
        	  allfinished = FALSE;
          }
        }
        if((params.exitOnNotFinished == TRUE)
        		&& (allfinished == FALSE))
        {
        	// trow execption and stop computing
        	throw TESTMETHOD_API::TMException("TmuRiseFallTimeUtil::doMeasurement()",
											  "TMU has not finished", "");
        }

        if((params.exitOnNotFinished == FALSE)
        		|| (allfinished == TRUE))
        {
        	// init the caches
        	// load all sites at once
        	siteResults.shot(1).measurement(1);
        	siteResults.shot(2).measurement(1);
        	task.pin(params.pins).uploadRawResults(siteResults.shot(1),1);  // shot1
        	task.pin(params.pins).uploadRawResults(siteResults.shot(2),2);  // shot2
        }

      }
      catch(TESTMETHOD_API::TMException& e)
      {
    	  ErrorString = (e.msg()).c_str();
    	  exception_caught = TRUE;
      }
      catch(Error& e)
      {
        ErrorString = (e.msg()).c_str();
        exception_caught = TRUE;
      }

      ON_FIRST_INVOCATION_END();
      /*
       * TMU measurement result retrieval
       */

      results.funcTestResult = task.getStatus();
      results.pinlist        = params.pins;


      if(exception_caught)
      {
        throw TESTMETHOD_API::TMException("TmuRiseFallTimeUtil::doMeasurement()",ErrorString.c_str(),"");
      }

    }
    catch(TESTMETHOD_API::TMException& e)
    {
       TESTSET().cont(TM::CONTINUE).
    	  		judgeAndLog_ParametricTest("", "", TM::Fail, 0);
       throw Error("TmuRiseFallTimeUtil::doMeasurement() ",(e.msg()).c_str());
    }
    // check if results for appType available
    if((FALSE == siteResults.shot(1).checkSiteAppTypeStored(results.appType))
    	|| (FALSE == siteResults.shot(2).checkSiteAppTypeStored(results.appType)))
    {
    	results.noResultsForSite = TRUE;
    	sprintf(results.errorStr,"RiseFallTime measurement failed for site %d",CURRENT_SITE_NUMBER());
    	return;
    }
    else
    {
      UINT32 pincnt1 = 0;
      UINT32 pincnt2 = 0;
      STRING finishedpins;					// store finished pin in string
      STRING_VECTOR pinvector2;  // store finished pin in vector
      if(((pincnt1 = siteResults.shot(1).getNumberOfMeasuredPins(results.pinvector, results.appType)) == 0) ||
         ((pincnt2 = siteResults.shot(2).getNumberOfMeasuredPins(pinvector2, results.appType)) == 0))
      {
      	results.noPinsHasResults = TRUE;
      	sprintf(results.errorStr,"RiseFallTime measurement failed for site %d",CURRENT_SITE_NUMBER());
      	return;
      }
      getMissingPins(results.missingpins,results.pinvector,results.pinlist);
      getMissingPins(results.missingpins,pinvector2,results.pinlist);

      for(unsigned int i = 0; i < results.pinvector.size();i++)
      {
    	  if(FALSE == calculateRiseFallTimeValues(results.pinvector[i],results))
    	  {
    		  TESTSET().cont(TM::CONTINUE).
    				  judgeAndLog_ParametricTest(results.pinvector[i], "", TM::Fail, 0);
    		  STRING msg = 	" RiseFallTime calculation for this site failed on pin ";
    		  msg += results.pinvector[i].c_str();
    		  cout << "TmuRiseFallTimeUtil::doMeasurement()" << msg << endl;
    	  }
      }
    }
  }

  /*
   *----------------------------------------------------------------------*
   * Routine: TmuRiseFallTimeUtil::judgeAndDatalog
   *
   * Purpose: Perform pass / fail judgement and datalogging
   *          for tmu measurement
   *
   *----------------------------------------------------------------------*
   * Description:
   *   const RiseFallTimeResult& results:
   *     Container to store measurement results
   *   INT ftstResult:               {0 | 1}
   *     Flag to specify whether taking functional test result into account
   *     for pass/fail judging and datalogging.
   *   
   * Note:
   *   judgeAndLog_ParametricTest() is called for results in the order of
   *   TM::MIN, TM::MAX, TM::MEAN, and finally functional test
   *   results. Even if one of results is FAIL, test method is executed
   *   until all avaiable results are logged.
   *
   *----------------------------------------------------------------------*
   */
  static void judgeAndDatalog( const string& testnameRise,
                               const string& testnameFall,
                                RiseFallTimeResult& results,
                               INT ftstResult)
  {
	TMU_RESULTS siteResults;
	const string funcName = "TmuRiseFallTimeUtil::judgeAndDatalog()";

    static bool sIsLimitTableUsed = false;
    static string minRiseTestname;
    static string maxRiseTestname;
    static string meanRiseTestname;
    static string minFallTestname;
    static string maxFallTestname;
    static string meanFallTestname;
    static double factorRiseMin = 1.0;
    static double factorRiseMax = 1.0;
    static double factorRiseMean = 1.0;
    static double factorFallMin = 1.0;
    static double factorFallMax = 1.0;
    static double factorFallMean = 1.0;
    ON_FIRST_INVOCATION_BEGIN();
      string::size_type leadPos = testnameRise.find("(");
      string::size_type postPos = testnameRise.find(")");
      if (leadPos != string::npos && postPos != string::npos && leadPos < postPos) {
         string validName = testnameRise.substr(leadPos+1,postPos-leadPos-1);
         vector<string> names;
         splitStr(validName,',',names,true);
         if (names.size() != 3) {
           throw Error("TmuRiseFallTimeUtil::judgeAndDatalog()",
                       "Invalid test name parameter.");
         }
         minRiseTestname = names.at(0);
         maxRiseTestname = names.at(1);
         meanRiseTestname = names.at(2);
      }

      leadPos = testnameFall.find("(");
      postPos = testnameFall.find(")");
      if (leadPos != string::npos && postPos != string::npos && leadPos < postPos) {
         string validName = testnameFall.substr(leadPos+1,postPos-leadPos-1);
         vector<string> names;
         splitStr(validName,',',names,true);
         if (names.size() != 3) {
           throw Error("TmuRiseFallTimeUtil::judgeAndDatalog()",
                       "Invalid test name parameter.");
         }
         minFallTestname = names.at(0);
         maxFallTestname = names.at(1);
         meanFallTestname = names.at(2);
      }
      //check whether limit table is used.
      TestTable* pLimitTable = TestTable::getDefaultInstance();
      pLimitTable->readCsvFile();
      sIsLimitTableUsed = pLimitTable->isTmLimitsCsvFile();

	  //----------------------------
	  enum Limit_states{ ALLVALID = 100, PARTVALID = 200, ALLFAILED = 300, UNKNOWSTATE = 500}; 
	  Limit_states temp_states;
	  temp_states = ALLFAILED;
	  bool b_at_least_one_limit_valid;
	  bool b_at_least_one_limit_failed;
	  b_at_least_one_limit_valid = false;
	  b_at_least_one_limit_failed = false;
	  //----------------------------

	  LIMIT limitMinRise;
	  LIMIT limitMaxRise;
	  LIMIT limitMeanRise;
	  LIMIT limitMinFall;
	  LIMIT limitMaxFall;
	  LIMIT limitMeanFall;

	  if (sIsLimitTableUsed) {

		  string testsuiteName;
		  GET_TESTSUITE_NAME(testsuiteName);

		  string key;
		  V93kLimits::TMLimits::LimitInfo limitInfo;

		  try{
			  key = testsuiteName + ":" + minRiseTestname;
			  limitInfo = V93kLimits::tmLimits.getLimit(key);
			  limitMinRise = limitInfo.TEST_API_LIMIT;
			  b_at_least_one_limit_valid = true;
		  }
		  catch(Error& e){
			  b_at_least_one_limit_failed = false;
		  }

		  try{
			  key = testsuiteName + ":" + maxRiseTestname;
			  limitInfo = V93kLimits::tmLimits.getLimit(key);
			  limitMaxRise = limitInfo.TEST_API_LIMIT;
			  b_at_least_one_limit_valid = true;
		  }
		  catch(Error& e){
			  b_at_least_one_limit_failed = false;
		  }

		  try{
			  key = testsuiteName + ":" + meanRiseTestname;
			  limitInfo = V93kLimits::tmLimits.getLimit(key);
			  limitMeanRise = limitInfo.TEST_API_LIMIT;
			  b_at_least_one_limit_valid = true;
		  }
		  catch(Error& e){
			  b_at_least_one_limit_failed = false;
		  }

		  try{
			  key = testsuiteName + ":" + minFallTestname;
			  limitInfo = V93kLimits::tmLimits.getLimit(key);
			  limitMinFall = limitInfo.TEST_API_LIMIT;
			  b_at_least_one_limit_valid = true;
		  }
		  catch(Error& e){
			  b_at_least_one_limit_failed = false;
		  }

		  try{
			  key = testsuiteName + ":" + maxFallTestname;
			  limitInfo = V93kLimits::tmLimits.getLimit(key);
			  limitMaxFall = limitInfo.TEST_API_LIMIT;
			  b_at_least_one_limit_valid = true;
		  }
		  catch(Error& e){
			  b_at_least_one_limit_failed = false;
		  }

		  try{
			  key = testsuiteName + ":" + meanFallTestname;
			  limitInfo = V93kLimits::tmLimits.getLimit(key);
			  limitMeanFall = limitInfo.TEST_API_LIMIT;
			  b_at_least_one_limit_valid = true;
		  }
		  catch(Error& e){
			  b_at_least_one_limit_failed = false;
		  }

		  if((true == b_at_least_one_limit_valid)&&(false == b_at_least_one_limit_failed)){
			  temp_states = ALLVALID;
		  }

		  else if((false == b_at_least_one_limit_valid)&&(true == b_at_least_one_limit_failed)){
			  temp_states = ALLFAILED;
		  }

		  else if((true == b_at_least_one_limit_valid)&&(true == b_at_least_one_limit_failed)){
			  temp_states = PARTVALID;
		  }
		  else{
			  temp_states = UNKNOWSTATE;
		  }

	  }
		
	  if (ALLVALID == temp_states){

	  }
	  else if(ALLFAILED == temp_states) {
		  sIsLimitTableUsed = false;
	  }
	  else if(PARTVALID == temp_states){
		  throw Error(funcName,
			  "Can't get all the limits from limit table.",
			  funcName);
	  }

	  if(!sIsLimitTableUsed)
	  {
		  limitMinRise = GET_LIMIT_OBJECT(minRiseTestname);
		  limitMaxRise = GET_LIMIT_OBJECT(maxRiseTestname);
		  limitMeanRise = GET_LIMIT_OBJECT(meanRiseTestname);
		  limitMinFall = GET_LIMIT_OBJECT(minFallTestname);
		  limitMaxFall = GET_LIMIT_OBJECT(maxFallTestname);
		  limitMeanFall = GET_LIMIT_OBJECT(meanFallTestname);

	  }
	  else
	  {
		  //set scalor
		  testmethod::SpecValue siValue;
		  siValue.setBaseUnit("nsec");
		  siValue.inputValueUnit("1[sec]");
		  factorRiseMin  = siValue.getValueAsTargetUnit(limitMinRise.unit());
		  factorRiseMax  = siValue.getValueAsTargetUnit(limitMaxRise.unit());
		  factorRiseMean = siValue.getValueAsTargetUnit(limitMeanRise.unit());
		  factorFallMin  = siValue.getValueAsTargetUnit(limitMinFall.unit());
		  factorFallMax  = siValue.getValueAsTargetUnit(limitMaxFall.unit());
		  factorFallMean = siValue.getValueAsTargetUnit(limitMeanFall.unit());
	  }

    ON_FIRST_INVOCATION_END();

    try
    {
      STRING_VECTOR & pinvector1 = results.pinvector;
      if(results.hasFailed())
      {
    	  TESTSET().cont(TM::CONTINUE).
    				judgeAndLog_ParametricTest("", "", TM::Fail, 0);
    	  PUT_DATALOG(results.errorStr);
    	  return;
      }
      logMissingPins(results.missingpins);

      for(size_t pinindex = 0; pinindex < pinvector1.size(); pinindex++)
      {
        STRING& pin = pinvector1[pinindex];
        if((results.pinRiseFallResults[pin].risewave.getSize() == 0)
           || (results.pinRiseFallResults[pin].fallwave.getSize() == 0))
        	continue;
        double meanfall = results.pinRiseFallResults[pin].mean_Fall();
        double meanrise = results.pinRiseFallResults[pin].mean_Rise();
        if (sIsLimitTableUsed)
        {
         if(meanfall != DBL_MAX)
          {
            TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,meanFallTestname,
                V93kLimits::tmLimits,factorFallMean * meanfall);
            double max = results.pinRiseFallResults[pin].max_Fall();
            TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,maxFallTestname,
                V93kLimits::tmLimits,factorFallMax *  max);
            double min = results.pinRiseFallResults[pin].min_Fall();
            TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,minFallTestname,
                V93kLimits::tmLimits,factorFallMin *  min);
          }
          else
          {
            TESTSET().cont(TM::CONTINUE).reliability(TM::UNRELIABLE).
                judgeAndLog_ParametricTest(pin,meanFallTestname, TM::Fail, 0);
            TESTSET().cont(TM::CONTINUE).reliability(TM::UNRELIABLE).
                judgeAndLog_ParametricTest(pin,maxFallTestname, TM::Fail, 0);
            TESTSET().cont(TM::CONTINUE).reliability(TM::UNRELIABLE).
                judgeAndLog_ParametricTest(pin,minFallTestname, TM::Fail, 0);
          }
          if(meanrise != DBL_MAX)
          {
            TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,meanRiseTestname,
                V93kLimits::tmLimits,factorRiseMean * meanrise);
            double max = results.pinRiseFallResults[pin].max_Rise();
            TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,maxRiseTestname,
                V93kLimits::tmLimits,factorRiseMax * max);
            double min = results.pinRiseFallResults[pin].min_Rise();
            TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,minRiseTestname,
                V93kLimits::tmLimits,factorRiseMin * min);
          }
          else
          {
            TESTSET().cont(TM::CONTINUE).reliability(TM::UNRELIABLE).
                judgeAndLog_ParametricTest(pin,meanRiseTestname, TM::Fail, 0);
            TESTSET().cont(TM::CONTINUE).reliability(TM::UNRELIABLE).
                judgeAndLog_ParametricTest(pin,maxRiseTestname, TM::Fail, 0);
            TESTSET().cont(TM::CONTINUE).reliability(TM::UNRELIABLE).
                judgeAndLog_ParametricTest(pin,minRiseTestname, TM::Fail, 0);
          }
        }
        else
        {
          if(meanfall != DBL_MAX)
           {
             TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,meanFallTestname, meanfall);
             double max = results.pinRiseFallResults[pin].max_Fall();
             TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,maxFallTestname, max);
             double min = results.pinRiseFallResults[pin].min_Fall();
             TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,minFallTestname, min);
           }
           else
           {
             TESTSET().cont(TM::CONTINUE).reliability(TM::UNRELIABLE).
                 judgeAndLog_ParametricTest(pin,meanFallTestname, TM::Fail, 0);
             TESTSET().cont(TM::CONTINUE).reliability(TM::UNRELIABLE).
                 judgeAndLog_ParametricTest(pin,maxFallTestname, TM::Fail, 0);
             TESTSET().cont(TM::CONTINUE).reliability(TM::UNRELIABLE).
                 judgeAndLog_ParametricTest(pin,minFallTestname, TM::Fail, 0);
           }
           if(meanrise != DBL_MAX)
           {
             TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,meanRiseTestname, meanrise);
             double max = results.pinRiseFallResults[pin].max_Rise();
             TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,maxRiseTestname, max);
             double min = results.pinRiseFallResults[pin].min_Rise();
             TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,minRiseTestname, min);
           }
           else
           {
             TESTSET().cont(TM::CONTINUE).reliability(TM::UNRELIABLE).
                 judgeAndLog_ParametricTest(pin,meanRiseTestname, TM::Fail, 0);
             TESTSET().cont(TM::CONTINUE).reliability(TM::UNRELIABLE).
                 judgeAndLog_ParametricTest(pin,maxRiseTestname, TM::Fail, 0);
             TESTSET().cont(TM::CONTINUE).reliability(TM::UNRELIABLE).
                 judgeAndLog_ParametricTest(pin,minRiseTestname, TM::Fail, 0);
           }
        }

      } // for(int pinindex ......


      // Finally judge and log the functional test result if available
      if (ftstResult)
      {
        if (results.funcTestResult == TMU::SUCCESS)
        {
          TESTSET().cont(TM::CONTINUE).
              judgeAndLog_ParametricTest("", "", TM::Pass, 0);
        }
        else
        {
          TESTSET().cont(TM::CONTINUE).
              judgeAndLog_ParametricTest("", "", TM::Fail, 0);
        }
      }
    }
    catch(TESTMETHOD_API::TMException& e)
    {
      throw Error("TmuRiseFallTimeUtil::judgeAndDatalog()",(e.msg()).c_str());
    }

  }

  /*
   *----------------------------------------------------------------------*
   * Routine: TmuRiseFallTimeUtil::reportToUI
   *
   * Purpose: Output TMU measurement results to Report Window
   *
   *----------------------------------------------------------------------*
   * Description:
   *   const RiseFallTimeResult& results:
   *     Container to store measurement results
   *   INT ftstResult:               {0 | 1}
   *     Flag to specify whether to output functional test result
   *   const STRING& output          {None | ReportUI}
   *     Flag to specify whether to output 
   *     "None" means no output and "ReportUI" means to do output
   *
   * Note:
   *
   *----------------------------------------------------------------------*
   */
  static void reportToUI(
      RiseFallTimeResult& results,
      const STRING& output,
      INT ftstResult)
  {
	TMU_RESULTS siteResults;
    try
    {
      // If output parameter is different from "ReportUI", just retun
      if ( output != "ReportUI" ) {
        return;
      }
      // If there is no pin contained in results return
      STRING_VECTOR & pinvector1 = results.pinvector;
      if(results.hasFailed())
      {
    	  printf(results.errorStr);
    	  return;
      }
      printMissingPins(results.missingpins);

      for(size_t pinindex = 0; pinindex < pinvector1.size(); pinindex++)
      {
        STRING& pin = pinvector1[pinindex];
        if((results.pinRiseFallResults[pin].risewave.getSize() == 0)
           || (results.pinRiseFallResults[pin].fallwave.getSize() == 0))
           continue;
        double meanfall = results.pinRiseFallResults[pin].mean_Fall();
        double meanrise = results.pinRiseFallResults[pin].mean_Rise();

        if(meanfall != DBL_MAX)
        {
          double max = results.pinRiseFallResults[pin].max_Fall();
          double min = results.pinRiseFallResults[pin].min_Fall();
          printf("Pin: [%s]  falltime \n",pin.c_str());
          printf( "minFall  = %-10g   sec\n",min);
          printf( "maxFall  = %-10g   sec\n",max);
          printf( "meanFall = %-10g   sec\n",meanfall);
        }
        else
        {
          printf("Pin: [%s]  falltime \n",pin.c_str());
          printf("minFall  = **********   sec\t*** NO VALID RESULT ***\n");
          printf("maxFall  = **********   sec\t*** NO VALID RESULT ***\n");
          printf("meanFall = **********   sec\t*** NO VALID RESULT ***\n");
        }
        if(meanrise != DBL_MAX)
        {
          double max = results.pinRiseFallResults[pin].max_Rise();
          double min = results.pinRiseFallResults[pin].min_Rise();
          printf("Pin: [%s]  risetime \n",pin.c_str());
          printf( "minRise  = %-10g   sec\n",min);
          printf( "maxRise  = %-10g   sec\n",max);
          printf( "meanRise = %-10g   sec\n",meanrise);
        }
        else
        {
          printf("Pin: [%s]  risetime \n",pin.c_str());
          printf("minRise  = **********   sec\t*** NO VALID RESULT ***\n");
          printf("maxRise  = **********   sec\t*** NO VALID RESULT ***\n");
          printf("meanRise = **********   sec\t*** NO VALID RESULT ***\n");
        }
      } // for(int pinindex ......


      // Finally output functional test result if available
      if (ftstResult)
      {
        printf("Functional Test:\n"
            "%-16s\t%c\n", "@",
            (results.funcTestResult == TMU::SUCCESS)? 'P' : 'F');
      }

      fflush(stdout);
    }
    catch(Error& e)
    {
      fflush(stdout);
      throw Error("TmuRiseFallTimeUtil::reportToUI()",(e.msg()).c_str());
    }

  }

  static Boolean calculateRiseFallTimeValues(STRING& pin,RiseFallTimeResult& results)
  {
	TMU_RESULTS siteResults;
    Boolean ret = true;
    try
    {

      RiseFallData& waves = results.pinRiseFallResults[pin];
      Wave < double > shot1wave;        // tmp wave objects.
      Wave < double > shot2wave;

      siteResults[(unsigned short)0].getRawData(pin,&shot1wave);
      siteResults[1].getRawData(pin,&shot2wave);
      cout << shot2wave.getSize() << " " <<  shot1wave.getSize() << endl;
      if(shot2wave.getSize() != shot1wave.getSize())
        throw TESTMETHOD_API::TMException("TmuRiseFallTimeUtil",
        			"calculateRiseFallTimeValues():number of taken samples not equal", "");

      shot2wave -= shot1wave;
      INT r,f;
      r = f = 0;
      waves.risewave.setSize(shot2wave.getSize());      // create capacity
      waves.fallwave.setSize(shot2wave.getSize());
      for(int i = 0; i < shot2wave.getSize(); i++)
      {
        if(shot2wave[i] < 0.0)
        {
          waves.fallwave[f++] = -shot2wave[i];
        }
        else
        {
          waves.risewave[r++] = shot2wave[i];
        }
      }
      waves.risewave.setSize(r);                       // resize to real size
      waves.fallwave.setSize(f);

    }
    catch(TESTMETHOD_API::TMException & e)
    {
      ret = FALSE;
      printf("%s: %s\n",pin.c_str(),e.msg().c_str());
    }
    catch(Error& e)
    {
      printf("%s: TmuRiseFallTimeUtil::calculateRiseFallTimeValues(): %s\n",pin.c_str(),e.msg().c_str());
      ret = FALSE;
    }
    catch(...)
    {
      printf("%s: TmuRiseFallTimeUtil::calculateRiseFallTimeValues(): unknown execption!!\n",pin.c_str());
      ret = FALSE;
    }
    return ret;
  }

  static INT splitStr (
        const STRING&   origStr,
        const CHAR      sepCh,
        STRING_VECTOR&  array,
        const Boolean   bSupportBracket)
    {
      Boolean bIsWithinBracket = FALSE;
      STRING word;
      const CHAR* str = origStr.c_str();

      for ( INT i = 0 ; *(str+i) != 0 ; ++i )
      {
        CHAR ch = str[i];

        /* seperator CHAR '\n' */
        if ( sepCh == '\n' )
        {
          if ( ch == sepCh )
          {
            array.push_back(word);
            word = "";
          }
          else
          {
            word += ch;
          }
          continue;
        }

        /* for other seperator char, like ',', '"',' ", etc.*/
        if( ch == sepCh )
        {
          if ( bIsWithinBracket && bSupportBracket )
          {
            word += ch;
          }
          else
          {
            array.push_back(word);
            word = "";
          }
        }
        else if ( ch == '(' )
        {
          bIsWithinBracket = TRUE;
        }
        else if ( ch == ')' )
        {
          bIsWithinBracket = TRUE;
        }
        else if ( ch == ' ' )
        {
          if ( i == 4)
          {
            /* this space is after the FW command name, eg. "DFPN " */
            array.push_back(word);
            word = "";
          }
        }
        else
        {
          word += ch;
        }
      }

      /* origStr is like "abc,def", we pick the rest one */
      if ( word.size() )
      {
        array.push_back(word);
      }

      return 0;
    }

};
#endif /*TMURISEFALLTIME_H_*/
