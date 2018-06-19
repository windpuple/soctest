#ifndef TMUEYEOPENINGTIME_H_
#define TMUEYEOPENINGTIME_H_

#include "TMUTask.h"
#include <float.h>

#include "TMU_Commons.hpp"

class TmuDriftEyeOpeningUtil
{
public:  

  /*
   *----------------------------------------------------------------------*
   * Struct: EyeDriftOpeningParameter
   *
   * Purpose: Container to store jitter measurement parameters
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
   *   STRING pins:                 {@ | pin and/or pin group list}
   *     Name of pin(s) and/or pin group(s) to be measured
   *     Valid pins: all digital pins.
   *   double datarate                     {}
   *     # max expected datarate of signal to be maesured
   *   INT samples_per_meas:             {}
   *     # of samples to be taken
   *     #  the result samples to be taken
   *   double  treshold_a;               {mV}
   *     # the treshold value
   *   INT ftstResult:               {0 | 1}
   *     Flag to specify whether taking functional test result into account
   *     for pass/fail judging and datalogging.
   *   
   *   The following parameters belong to group 2.
   *
   *   The following parameters belong to group 3.
   *
   *   Boolean samplerange
   *
   *   TMU::EDGE_TYPE slope:
   *     Event slope of event . It's determined from the mode parameter
   *
   * Note:
   *   When you create an instance of this class, appropriate default values
   *   except pins are set to each variables in the class.
   *
   *----------------------------------------------------------------------*
   */

  struct DriftEyeOpeningParameter
  {
      // Parameters specified by a user
      STRING  pins;
      STRING  mode;
      double  datarate;
      UINT64  samples_per_meas;
      DOUBLE  threshold;
      Boolean ftstResult;
      Boolean exitOnNotFinished;
      double waitTimeout;

      // Parameters specified by a user, possibly modified by processParameters

      // Parameters set by processParameters()
      TMU::ApplicationType appType;
      TMU::EdgeType slope;
      Boolean samplesrange;


      // Default constructor
      // all parameters are set to default values
      DriftEyeOpeningParameter()
      : pins(""),
        mode(""),
        datarate(0.0),
        samples_per_meas(1),
        threshold(0.0),
        ftstResult(FALSE),
        exitOnNotFinished(FALSE),
        waitTimeout(0.0),
        appType(TMU::APP_NONE),
        slope(TMU::RISE),
        samplesrange(FALSE){}
  };
  
   /*
   *----------------------------------------------------------------------*
   * Struct: DriftEyeOpeningResult
   *
   * Purpose: Container to store pptia measurement results
   *
   *----------------------------------------------------------------------*
   */

  class DriftEyeOpeningResult : public TMU_TML_RESULT_BASE
  {
  public:
    UINT64  interdiscard;
    UINT32  prescaler;
    Wave < double > fallJitterWave;
    Wave < double > riseJitterWave;
    std::map < std::string, DOUBLE > eyePinsMap;
    std::map < std::string, DOUBLE > periodPinsMap;
    DriftEyeOpeningResult() : TMU_TML_RESULT_BASE() {}
  };
  

/*
 *----------------------------------------------------------------------*
 * Utility functions for EyeOpening measurements
 *----------------------------------------------------------------------*
 */

  /*
   *----------------------------------------------------------------------*
   * Routine: TmuDriftEyeOpeningUtil::processParameters
   *
   * Purpose: Store given measurement parameters to the specified
   *          placeholder and determine addtional parameters
   *          which is necessary to execute measurement.
   *          Also performs some error checks on parameters.
   *
   *----------------------------------------------------------------------*
   * Description:
   *   const STRING& pins:
   *   const STRING& mode:                 {EYE|DRIFT}
   *     Measurement mode.
   *   LONG datarate                     {}
   *     # max expected datarate of signal to be maesured
   *   INT samples_per_meas:             {}
   *     # of samples to be taken
   *   double  threshold_a;               {mV}
   *   INT ftstResult:               {0 | 1}
   *     Those are parameters for jitter measurement.
   *     See the descriptions in JitterParameter definition.
   *   JitterParameter& params:
   *     Container to hold parameters for the measurement.
   *   
   * Note:
   *
   *----------------------------------------------------------------------*
   */
  static void processParameters(
                            const STRING& pins,
                            const STRING& mode,
                            double datarate,
                            UINT64 samples_per_meas,
                            INT exitOnNotFinished,
                            double waitTimeout,
                            DOUBLE threshold,
                            INT ftstResult,
                            DriftEyeOpeningParameter& params)
  {
    // If no pin for pins is specified, an exception is thrown.
     if (pins.size() == 0)
     {
       throw Error("TmuDriftEyeOpeningUtil::processParameters()",
                     "Empty pins parameter.");
     }

     if (datarate <= 0.0)
     {
       throw Error("TmuDriftEyeOpeningUtil::processParameters()",
                              "Wrong datarate parameter.");
     }


     if (mode == "EYE")
     {
    	 params.slope = TMU::RISE_FALL;
    	 params.appType = TMU::APP_EYE;
     }
     else if (mode == "DRIFT")
     {
    	 params.slope = TMU::RISE;
    	 params.appType = TMU::APP_DRIFT;
     }
     else
     {
    	 // If the specified mode is not supported in this function,
    	 // an exception is thrown.
    	 throw Error("TmuDriftEyeOpeningUtil::processParameters()",
    			 "Illegal mode parameter.");
     }

     params.pins 				= pins;
     params.datarate 			= datarate;
     params.samples_per_meas  	= samples_per_meas;
     params.ftstResult	    	= ftstResult;
     params.threshold	    	= threshold;
     params.ftstResult 			= ftstResult;
     params.exitOnNotFinished = (exitOnNotFinished == 1)?TRUE:FALSE;
     params.waitTimeout		= waitTimeout;

  }
  
  /*
   *----------------------------------------------------------------------*
   * Routine: TmuDriftEyeOpeningUtil::doMeasurement
   *
   * Purpose: Perform setup, execution and result retrieval
   *          for rise/fall time measurement with per pin TIA
   *
   *----------------------------------------------------------------------*
   *
   * Description:
   *   const EyeOpeningParameter& params:
   *     Container to hold parameters for the measurement.
   *   PPTiaMeasurementResult& results:
   *     Container to store measurement results
   *   
   * Note:
   *   'static' variables are used in this function to keep some information
   *   which is refered in the execution for sites where ON_FIRST_INVOCATION 
   *   block is not executed.
   *
   *----------------------------------------------------------------------*
   */
  static void doMeasurement( const DriftEyeOpeningParameter& params,
      DriftEyeOpeningResult& results)
  {
      // Measurement setup, execution and result retrieval are done
      // through task object
      TMU_TASK task;
      TMU_RESULTS siteResults;
      INT site;
      STRING ErrorString;
      Boolean exception_caught = FALSE;
      Boolean finished = TRUE;
      if(params.mode == "DRIFT")
      {
        ErrorString = "!!! drift measurement is not yet implemented!!\n";
        throw TESTMETHOD_API::TMException(
                            "TmuDriftEyeOpeningUtil::doMeasurement()",
                            ErrorString.c_str(),"");
      }
      // we need prescaler and interdiscard for calculation
      TMU::DATARATE_DEPENDENT_VALUES_TYPE dr_dep_settings;
      try
      {
        site = CURRENT_SITE_NUMBER();
        CONNECT();
        ON_FIRST_INVOCATION_BEGIN();
        //avoid ON_FIRST_INVOCATION_END executed so catch exceptions
        try
        {
        	task.pin(params.pins).setEdgeSelect(params.slope)
        							   .setDatarate(params.datarate MBps)
        							   .setNumSamples(params.samples_per_meas)
        							   .setNumShots(1)
        							   .setNumMeasurements(1)
        							   .getDatarateDependendSettings(dr_dep_settings);

        	results.prescaler    = dr_dep_settings.prescaler;
        	results.interdiscard = dr_dep_settings.interdiscard;
        	if(params.threshold != 0)
        		task.pin(params.pins).setAThreshold(params.threshold);

        	/*
        	 * TMU measurement execution
        	 */
        	task.setAsPrimary();
        	task.execute();

        	task.pin(params.pins).waitTMUfinished(params.waitTimeout,finished);

        	if((params.exitOnNotFinished == FALSE)
        			|| (finished == TRUE))
        	{
       			task.pin(params.pins).uploadRawResults(siteResults);
        	}

        }
        catch(Error& e)
        {
        	ErrorString = (e.msg()).c_str();
        	exception_caught = TRUE;
        }
        ON_FIRST_INVOCATION_END();
        results.funcTestResult = task.getStatus();
       	results.pinlist			= params.pins;


        /*
         * TMU measurement result retrieval
         */
        if((params.exitOnNotFinished == TRUE)
         		&& (finished == FALSE))
        {
        	throw TESTMETHOD_API::TMException("TmuDriftEyeOpeningUtil::doMeasurement()","TMU has not finished","");
        }    logMissingPins(results.missingpins);
        if(exception_caught)
        {
          throw TESTMETHOD_API::TMException("TmuDriftEyeOpeningUtil::doMeasurement()",ErrorString.c_str(),"");
        }

        // Iterate all pins
        int anzPins = siteResults.getNumberOfMeasuredPins(results.pinvector,
															TMU::APP_RAW,site);
        if (anzPins != 0)
        {
        	for(int i = 0; i < anzPins; i++)
        	{
        		double tmp = 0.0;
        		results.eyePinsMap.insert(std::make_pair(results.pinvector[i],tmp));
        		results.periodPinsMap.insert(std::make_pair(results.pinvector[i],tmp));
        		// ##########  calculate the values. ##########
        		if(FALSE == calculateEyeValues(results,results.pinvector[i]))
        		{
        			TESTSET().cont(TM::CONTINUE).
        			    	 judgeAndLog_ParametricTest(results.pinvector[i].c_str(), "", TM::Fail, 0);
        		}
        	}
        }
        else
        {
        	results.noPinsHasResults = TRUE;
        	sprintf(results.errorStr,"No pinresults available for  measurement on this site %d\n",CURRENT_SITE_NUMBER());
        }
        getMissingPins(results.missingpins,results.pinvector,results.pinlist);

      }
      catch(TESTMETHOD_API::TMException& e)
      {
    	 TESTSET().cont(TM::CONTINUE).
    	   			judgeAndLog_ParametricTest("", "", TM::Fail, 0);
    	 throw Error("TmuDriftEyeOpeningUtil::doMeasurement() ",(e.msg()).c_str());
      }

      // check if results for appType available
      if(FALSE == siteResults.checkSiteAppTypeStored(TMU::APP_RAW,site))
      {
    	 results.noResultsForSite = TRUE;
     	 sprintf(results.errorStr,"No frequency measurement results  available for site %d",site);
      }
  }

  /*
   *----------------------------------------------------------------------*
   * Routine: TmuDriftEyeOpeningUtil::judgeAndDatalog
   *
   * Purpose: Perform pass / fail judgement and datalogging
   *          for pptia measurement
   *
   *----------------------------------------------------------------------*
   * Description:
   *   const EyeOpeningResult& results:
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
  static void judgeAndDatalog(const string& testname,
                              DriftEyeOpeningResult& results,
                              INT ftstResult)
  {
	TMU_RESULTS siteResults;
	static bool sIsLimitTableUsed = false;
    static double factor = 1.0;
    ON_FIRST_INVOCATION_BEGIN();
    //check whether limit table is used.
    TestTable* pLimitTable = TestTable::getDefaultInstance();
    pLimitTable->readCsvFile();
    sIsLimitTableUsed = pLimitTable->isTmLimitsCsvFile();
	if (sIsLimitTableUsed) {
		LIMIT limit;
		try{
			string testsuiteName;
			GET_TESTSUITE_NAME(testsuiteName);
			string key = testsuiteName + ":" + testname;
			V93kLimits::TMLimits::LimitInfo limitInfo = V93kLimits::tmLimits.getLimit(key);
			limit = limitInfo.TEST_API_LIMIT;
		}
		catch (Error& e)
		{
			sIsLimitTableUsed = false;
			limit = GET_LIMIT_OBJECT(testname);
		}

		if (limit.unit().empty())
		{
			factor =  1.0;
		}
		testmethod::SpecValue siValue;
		siValue.setBaseUnit("nsec");
		siValue.inputValueUnit("1[sec]");
		factor =  siValue.getValueAsTargetUnit(limit.unit());

	}
    ON_FIRST_INVOCATION_END();

    logMissingPins(results.missingpins);
    if (results.hasFailed())
    {
    	printf("%s",results.errorStr);
    	return;
    }

    for(unsigned int i = 0; i < results.pinvector.size();i++)
    {
      double eye = results.eyePinsMap[results.pinvector[i]];
      if (sIsLimitTableUsed)
      {
        if(eye == 0.0)
        {
          TESTSET().cont(TM::CONTINUE).reliability(TM::UNRELIABLE).
            judgeAndLog_ParametricTest(results.pinvector[i],
                                       testname, V93kLimits::tmLimits,0);
        }
        else
        {
          TESTSET().cont(TM::CONTINUE).
            judgeAndLog_ParametricTest(results.pinvector[i],testname,
                                                  V93kLimits::tmLimits,
                                                  eye * factor);
        }
      }
      else
      {
        if(eye == 0.0)
        {
          TESTSET().cont(TM::CONTINUE).reliability(TM::UNRELIABLE).
            judgeAndLog_ParametricTest(results.pinvector[i],testname, TM::Fail, 0);
        }
        else
        {
          TESTSET().cont(TM::CONTINUE).
            judgeAndLog_ParametricTest(results.pinvector[i],testname,eye);
        }
      }
    }
  }
  
  /*
   *----------------------------------------------------------------------*
   * Routine: TmuDriftEyeOpeningUtil::reportToUI
   *
   * Purpose: Output pptia measurement results to Report Window
   *
   *----------------------------------------------------------------------*
   * Description:
   *   const EyeOpeningResult& results:
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
                                  /*const*/ DriftEyeOpeningResult& results,
                                  const STRING& output,
                                  INT ftstResult)                  
  {
	  TMU_RESULTS siteResults;

      printMissingPins(results.missingpins);
      if (results.hasFailed())
      {
    	  printf("%s",results.errorStr);
    	  return;
      }
	  for(unsigned int i = 0; i < results.pinvector.size();i++)
	  {
		  double eye 	= results.eyePinsMap[results.pinvector[i]];
		  double period = results.periodPinsMap[results.pinvector[i]];
		  printf("drift / eyeopening for pin: %s\n",results.pinvector[i].c_str());
		  if(eye != 0.0)
		  {
			  printf("eyeValue    \t\t= %-13g   sec\n",eye);
			  printf("halfperiod / period\t= %-13g   sec / %-13g   sec\n",period /2.0,period);
		  }
		  else
		  {
			  printf("eyeValue = **********   sec\t*** NO VALID RESULT ***\n");

		  }
	  }


	  // Finally output functional test result if available
	  if (ftstResult)
	  {
         printf("Functional Test:\n"
             "%-16s\t%c\n", "@",
             (results.funcTestResult == TMU::SUCCESS)? 'P' : 'F');
	  }

      fflush(stdout);
  }
  
  /*
   * calculateJitterValues 			calculates the minimum eye opening
   *
   * @ return  true if succeed.
   */
  static Boolean calculateEyeValues(DriftEyeOpeningResult& result,
							 	    STRING pin)
  {
	TMU_RESULTS siteResults;
    Boolean ret = true;
    Wave<DOUBLE> source;
    Wave<DOUBLE>& resultfall = result.fallJitterWave;
    Wave<DOUBLE>& resultrise = result.riseJitterWave;
    DOUBLE period;
    DOUBLE eye;
    UINT64  interdiscard = result.interdiscard;
    UINT32  prescaler    = result.prescaler;


    try
    {
      siteResults.getRawData(pin,&source);

      if(source.getSize() <= 1)
        throw TESTMETHOD_API::TMException("TmuPropDelayUtil","calculateJitterValues()",
            "source array empty or only one sample");

      DOUBLE modTmp,highMean, lowMean,riseperiodMean;

      modTmp = highMean = lowMean = riseperiodMean = 0.0;
      // determine period
      INT size = source.getSize();
      // edgetype is RISE_FALL so add 0.5 to prescaler
      // we have 3 stamp / period   ( rise , fall, rise)

      int cnter = 0;
      for(int i = 0;i < size-2 ;i+=2)
      {
    //      ______________
    //	   /              \                /
    //	  /                \______________/
    //	  |---- highMean---|----lowMean---|
    //	  |----------riseperiodMean ------|

    	  cnter++;
    	  highMean  		+= source[i+1] - source[i];
    	  lowMean 		 	+= source[i+2] - source[i+1];
    	  riseperiodMean 	+= source[i+2] - source[i];
      }

      riseperiodMean /= (DOUBLE)(size-2)* (prescaler+0.5)*(interdiscard+1);
      highMean	 	 /= (DOUBLE)(size-2)*  (prescaler+0.5)*(interdiscard+1);
      lowMean 		 /= (DOUBLE)(size-2)*  (prescaler+0.5)*(interdiscard+1);

      DOUBLE halfperiod    = riseperiodMean / 2.0;
      DOUBLE quarterperiod = riseperiodMean / 4.0;

      // compute sample differences periodMean
      resultfall.setSize(size / 2);
      resultrise.setSize(size / 2);
      cnter = -1;
      for(int i = 0;i < size ;i++)
      {
  		modTmp = remainder(source[i],halfperiod);
    	if((i % 2) == 0)
    	{   // rising edge
    		cnter++;
//    		modTmp = remainder(source[i],halfperiod);
    		resultrise[cnter] = (modTmp > quarterperiod)? modTmp - halfperiod : modTmp;
    	}
    	else
    	{   // falling edge
//    		modTmp = remainder(source[i]-highMean,halfperiod);
    		resultfall[cnter] = (modTmp > quarterperiod)? modTmp - halfperiod : modTmp;
    	}
      } // for all samples
      period = riseperiodMean;
      eye    = MIN((highMean + resultfall.min() - resultrise.max()),
				   (lowMean  - resultfall.max() + resultrise.min()));

      result.eyePinsMap[pin]= eye;
      result.periodPinsMap[pin]= period;
    }
    catch(Error& e)
    {
      ret = FALSE;
    }
    catch(...)
    {
      ret = FALSE;
    }
    return ret;
  }
};
#endif /*TMUEYEOPENINGTIME_H_*/
