#ifndef TMUPROPDELAY_H_
#define TMUPROPDELAY_H_

#include "TMUTask.h"
#include <float.h>

class TmuPropDelayUtil
{
public:

  /*
   *----------------------------------------------------------------------*
   * Struct: PropDelayParameter
   *
   * Purpose: Container to store propagation delay measurement parameters
   *
   *----------------------------------------------------------------------*
   * Description:
   *   The members in this structure can be categorized into 2 groups
   *     1. specified by a user
   *     2. set by processParameters()
   * 
   *   The following parameters belong to group 1.
   *
   *   STRING drivepins:                 {@ | pin and/or pin group list}
   *     Name of pin(s) and/or pin group(s) to be measured
   *     Duration from the event for drivepins to the event for receivepins is measured.
   *     The n-th pin in the pins1 and the receivepins make a pair.
   *     Valid pins: all digital pins.
   *   STRING receivepins:                 {@ | pin and/or pin group list}
   *     Name of pin(s) and/or pin group(s) to be measured
   *     Duration from the event for drivepins to the event for receivepins is measured.
   *     The n-th pin in the pins1 and the receivepins make a pair.
   *     Valid pins: all digital pins.
   *   STRING mode:                  {PD++ | PD--}
   *     Measurement mode.
   *     PD++: from  rising edge of drivepins to  rising edge of receivepins
   *     PD--: from falling edge of drivepins to falling edge of receivepins
   *   double testperiod                     {}
   *     # min expected period of signal to be maesured
   *   INT samples_per_meas:             {}
   *     # of samples to be taken
   *     # this value is for setup
   *   double  treshold_a;               {mV}
   *   int     initial_discard;          {}
   *     # number od sample to discard at beginning
   *   INT ftstResult:               {0 | 1}
   *     Flag to specify whether taking functional test result into account
   *     for pass/fail judging and datalogging.
   *   
   *   The following parameters belong to group 2.
   *
   *   The following parameters belong to group 3.
   *
   *   TMU::EdgeType slope:
   *     Event slope of event for pins1. It's determined from the mode
   *     parameter
   *
   * Note:
   *   When you create an instance of this class, appropriate default values
   *   except pins are set to each variables in the class.
   *
   *----------------------------------------------------------------------*
   */
  struct PropDelayParameter
  {
      // Parameters specified by a user
      STRING  drivepins;
      STRING  receivepins;
      STRING  mode;
      DOUBLE  testperiod;
      UINT64  samples_per_meas;
      DOUBLE  treshold_a;
      UINT64  initial_discard;
      Boolean exitOnNotFinished;
      double  waitTimeout;
      Boolean ftstResult;

      // Parameters specified by a user, possibly modified by processParameters

      // Parameters set by processParameters()
      TMU::ApplicationType appType;
      TMU::EdgeType slope;

      // Default constructor
      // all parameters are set to default values
      PropDelayParameter()
      : drivepins(""),
        receivepins(""),
        mode("PD++"),
        testperiod(0),
        samples_per_meas(1),
        treshold_a(0.0),
        initial_discard(0),
        exitOnNotFinished(FALSE),
        waitTimeout(1e-6),
        ftstResult(FALSE),
        appType(TMU::APP_RAW),
        slope(TMU::RISE){}
  };

  /*
   *----------------------------------------------------------------------*
   * Struct: PropDelayResult
   *
   * Purpose: Container to store propagation delay measurement results
   *
   *----------------------------------------------------------------------*
   */



  class PropDelayResult : public TMU_TML_RESULT_BASE
  {
  public:
    STRING drvpinstr;
    STRING_VECTOR drvpins;
    STRING_VECTOR rcvpins;
    UINT32 drvPinCnt;
    UINT32 rcvPinCnt;

    struct PinResult {
        DOUBLE min;
        DOUBLE max;
        DOUBLE mean;

        // all values are initialized with extrem value of DOUBLE
        PinResult() : min(DBL_MAX), max(DBL_MIN), mean(DBL_MAX) {}

              // function to return if the result is valid or not
        Boolean isValid() const {
          return min  != DBL_MAX ||
                 max  != DBL_MIN ||
                 mean != DBL_MAX;
        }
    };

    std::map<STRING, PinResult> valueResult;
    PropDelayResult() : TMU_TML_RESULT_BASE() {}
  };

/*
 *----------------------------------------------------------------------*
 * Utility functions for Propagation Delay measurements
 *----------------------------------------------------------------------*
 */

  /*
   *----------------------------------------------------------------------*
   * Routine: TmuPropDelayUtil::processParameters
   *
   * Purpose: Store given measurement parameters to the specified
   *          placeholder and determine addtional parameters
   *          which is necessary to execute measurement.
   *          Also performs some error checks on parameters.
   *
   *----------------------------------------------------------------------*
   * Description:
   *   const STRING& drivepins:
   *   const STRING& receivepins:
   *   const STRING& mode:              {PD++ | PD--}
   *   double testperiod:               {n}
   *   UINT64 samples_per_meas:         {n}
   *   DOUBLE treshold_low:             {mV}
   *   UINT64 initial_discard:          {n}
   *   INT ftstResult:                  {0 | 1}
   *     These are parameters for propagation delay measurement
   *     given externally.
   *     See the descriptions in PropDelayParameter definition.
   *   PropDelayParameter& params:
   *     Container to hold parameters for the measurement.
   *   
   * Note:
   *
   *----------------------------------------------------------------------*
   */


  static void processParameters(
                                  const STRING& drivepins,
                                  const STRING& receivepins,
                                  const STRING mode,
                                  DOUBLE testperiod,
                                  UINT64 samples_per_meas,
                                  DOUBLE treshold_a,
                                  UINT64 initial_discard,
                                  INT exitOnNotFinished,
                                  double  waitTimeout,
                                  INT    ftstResult,
                                  PropDelayParameter& params)
  {
    // If no pin for pins is specified, an exception is thrown.
     if (drivepins.size() == 0)
     {
       throw Error("TmuPropDelayUtil::processParameters()",
                     "Empty drivepins parameter.");
     }

     if (receivepins.size() == 0)
     {
       throw Error("TmuPropDelayUtil::processParameters()",
                     "Empty receivepins parameter.");
     }


    /*
    *   STRING& mode:                 {PD++ | PD--}
    *     Measurement mode.
    *     PD++: from  rising edge of drivepins to  rising edge of receivepins
    *     PD--: from falling edge of drivepins to falling edge of receivepins
    */

     // propagation delay is of ApplicationType TMU::APP_RAW
     // because raw data will be used for postprocessing.
     // other Applicationtypes will remain in sorting the resultcaches
     // pinvector. That is ugly because the 2 pinvectors should correspond.
     params.appType = TMU::APP_RAW;
     if (mode == "PD++")
     {
        params.slope = TMU::RISE;
     }
     else if (mode == "PD--")
     {
        params.slope = TMU::FALL;
     }
     else
     {
        throw Error("TmuPropDelayUtil::processParameters()",
                     "illegal mode parameter.");
     }

     params.drivepins = drivepins;
     params.receivepins = receivepins;
     params.testperiod = testperiod;
     params.samples_per_meas = samples_per_meas;
     params.ftstResult = (ftstResult == 0)? FALSE:TRUE;
     params.treshold_a = treshold_a;
     params.initial_discard = initial_discard;
     params.exitOnNotFinished = (exitOnNotFinished == 1)?TRUE:FALSE;
     params.waitTimeout		= waitTimeout;

  }
  
  /*
   *----------------------------------------------------------------------*
   * Routine: TMUPropDelayUtil::doMeasurement
   *
   * Purpose: Perform setup, execution and result retrieval
   *          for propagation delay measurement with per pin TIA
   *
   *----------------------------------------------------------------------*
   * Description:
   *   const PropDelayParameter& params:
   *     Container to hold parameters for the measurement.
   *   PropDelayResult& results:
   *     Container to store measurement results
   *   
   * Note:
   *   'static' variables are used in this function to keep some information
   *   which is refered in the execution for sites where ON_FIRST_INVOCATION 
   *   block is not executed.
   *
   *----------------------------------------------------------------------*
   */
  static void doMeasurement(
      const PropDelayParameter& params,
      PropDelayResult& results)
  {
    // Measurement setup, execution and result retrieval are done
    // through task object
    TMU_TASK task;
    TMU_RESULTS siteResults;

    INT site;
    STRING ErrorString;

    Boolean exception_caught = FALSE;
    Boolean finished = TRUE;
    STRING allPins = params.drivepins+","+params.receivepins;
    try
    {
    	site = CURRENT_SITE_NUMBER();
    	CONNECT();
    	ON_FIRST_INVOCATION_BEGIN();
    	//avoid ON_FIRST_INVOCATION_END executed so catch exceptions
    	try
    	{

    		// take minimum 1000 samples
    		UINT64 samplestomeas = (params.samples_per_meas > 0)?params.samples_per_meas:1000;
    		double datarate =  1.0 / (params.testperiod ns);

    		task.pin(allPins).setDatarate(datarate)
                            		  .setEdgeSelect(params.slope)
                            		  .setNumMeasurements(1)
                            		  .setNumShots(1)
                            		  .setNumSamples(samplestomeas)
                            		  .setSignalType(TMU::CLK)
                            		  .setInitialDiscard(params.initial_discard);

    		if(params.treshold_a != 0)
    		{
    			task.pin(allPins).setAThreshold(params.treshold_a);
    		}

    		/*
    		 * TMU measurement execution
    		 */
    		task.setup();
    		task.setAsPrimary();
    		task.execute();

    		task.pin(allPins).waitTMUfinished(params.waitTimeout,finished);

        	siteResults["drv"].numSamples(params.samples_per_meas);
        	siteResults["rcv"].numSamples(params.samples_per_meas);

        	if((params.exitOnNotFinished == FALSE)
        			|| (finished == TRUE))
        	{
        		task.pin(params.drivepins).uploadRawResults(siteResults["drv"]);
        		task.pin(params.receivepins).uploadRawResults(siteResults["rcv"]);
        	}
    	}
    	catch(Error& e)
    	{
    		ErrorString = (e.msg()).c_str();
    		exception_caught = TRUE;
    	}
    	//     task.disableTMU();
      ON_FIRST_INVOCATION_END();
      /*
       * TMU measurement result retrieval
       */
      if((params.exitOnNotFinished == TRUE)
       		&& (finished == FALSE))
      {
      	throw TESTMETHOD_API::TMException("TmuPropDelayUtil::doMeasurement()","TMU has not finished","");
      }

      if(exception_caught == TRUE)
      {
        throw TESTMETHOD_API::TMException("TmuPropDelayUtil::doMeasurement()",ErrorString.c_str(),"");
      }
      /*
       * retrieve results
       */

      // fill the STRING_VECTOR variables and number of pins measured
      results.funcTestResult  = task.getStatus();
      results.pinlist		  = params.receivepins;
      results.drvpinstr		  = params.drivepins;
      results.drvpins.clear();
      results.rcvpins.clear();
      results.drvPinCnt = siteResults["drv"].getNumberOfMeasuredPins(results.drvpins,TMU::APP_RAW);
      getMissingPins(results.missingpins,results.drvpins,results.drvpinstr);
      results.rcvPinCnt = siteResults["rcv"].getNumberOfMeasuredPins(results.rcvpins,TMU::APP_RAW);
      getMissingPins(results.missingpins,results.rcvpins,results.pinlist);

      if(results.rcvPinCnt == 0)
      {
    	  results.noPinsHasResults = TRUE;
    	  sprintf(results.errorStr,"NO Results for pinlist %s, on site %d\n",
    			  results.pinlist.c_str(),CURRENT_SITE_NUMBER());
    	  return;
      }
      if(results.drvPinCnt == 0)
      {
    	  results.noPinsHasResults = TRUE;
    	  sprintf(results.errorStr,"NO Results for pinlist %s, on site %d\n",
    			  results.drvpinstr.c_str(),CURRENT_SITE_NUMBER());
		  return;
      }
      for (UINT32 i= 0; i < MAX(results.drvPinCnt,results.rcvPinCnt) ; i++)
      {
    	  STRING errstr_i, errstr_o;
    	  if(i < results.rcvPinCnt)
    	  {
    		  UINT32 ErrorState_i = task.pin(allPins).errorState(results.rcvpins[i]);
    		  if(ErrorState_i != 0)
    		  {
    			  errorStatesToString(ErrorState_i,errstr_i);
    			  std::cout << "\nerrors for " << results.rcvpins[i]  << "\t" << errstr_i << std::endl;
    		  }
    	  }
    	  if(i < results.drvPinCnt)
    	  {
    		  UINT32 ErrorState_o = task.pin(allPins).errorState(results.drvpins[i]);
    		  if(ErrorState_o != 0)
    		  {
    			  errorStatesToString(ErrorState_o,errstr_o);
    			  std::cout << "\nerrors for " << results.drvpins[i]  << "\t" << errstr_o << std::endl;
    		  }
    	  }
      }
    }
    catch(TESTMETHOD_API::TMException& e)
    {
    	TESTSET().cont(TM::CONTINUE).
    	    	  		judgeAndLog_ParametricTest("", "", TM::Fail, 0);
    	throw Error("TmuPropDelayUtil::doMeasurement() ",(e.msg()).c_str());
    }

    if ((FALSE == siteResults["drv"].checkSiteAppTypeStored(TMU::APP_RAW))
    		|| (FALSE == siteResults["rcv"].checkSiteAppTypeStored(TMU::APP_RAW)))
    {
    	results.noResultsForSite = TRUE;
    	sprintf(results.errorStr,"No measurement results  available for site %d",site);
    }
    else
    {
      computeDelays(results,1);
    }

  }
  
  /*
   *----------------------------------------------------------------------*
   * Routine: TmuPropDelayUtil::judgeAndDatalog
   *
   * Purpose: Perform pass / fail judgement and datalogging
   *          for propagation delay measurement
   *
   *----------------------------------------------------------------------*
   * Description:
   *   const PropDelayResult& results:
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
      PropDelayResult& results,
      INT ftstResult)
  {
    TMU_RESULTS siteResults;

    static bool sIsLimitTableUsed = false;
    static double factor = 1.0;
    ON_FIRST_INVOCATION_BEGIN();
    //check whether limit table is used.
    TestTable* pLimitTable = TestTable::getDefaultInstance();
    pLimitTable->readCsvFile();
    //sIsLimitTableUsed = pLimitTable->isTmLimitsCsvFile();
	if (sIsLimitTableUsed) {

		LIMIT limit;
		try{
			string testsuiteName;
			GET_TESTSUITE_NAME(testsuiteName);
			string key = testsuiteName + ":" + testname;
			V93kLimits::TMLimits::LimitInfo limitInfo = V93kLimits::tmLimits.getLimit(key);
			limit = limitInfo.TEST_API_LIMIT;
		}
		catch (Error e)
		{
			sIsLimitTableUsed = false;
			limit = GET_LIMIT_OBJECT(testname);
		}

		testmethod::SpecValue siValue;
		siValue.setBaseUnit("nsec");
		siValue.inputValueUnit("1[sec]");
		factor =  siValue.getValueAsTargetUnit(limit.unit());
	}
    ON_FIRST_INVOCATION_END();

    // If there is no pin contained in results return
    if(results.hasFailed())
    {
    	TESTSET().cont(TM::CONTINUE).
				judgeAndLog_ParametricTest("", "", TM::Fail, 0);
    	PUT_DATALOG(results.errorStr);
    	return;
    }

    logMissingPins(results.missingpins);
    std::map<STRING, PropDelayResult::PinResult>::iterator pinIter;
    try
    {
      for(pinIter = results.valueResult.begin();pinIter != results.valueResult.end();pinIter++)
      {
    	STRING pin = pinIter->first;
    	if(pinIter->second.isValid())
    	{
    		double max  = pinIter->second.max;
    		if(sIsLimitTableUsed)
    		{
    			TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,
                                testname,V93kLimits::tmLimits,factor *  max);
    		}
    		else
    		{
    			TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,
                                testname,max);
    		}

        }
        else
        {
          TESTSET().cont(TM::CONTINUE).reliability(TM::UNRELIABLE).
                          judgeAndLog_ParametricTest(pin,testname, TM::Fail, 0);
        }
      }

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
      throw Error("TmuPropDelayUtil::judgeAndDatalog()",(e.msg()).c_str());
    }

  }

  /*
   *----------------------------------------------------------------------*
   * Routine: TmuPropDelayUtil::reportToUI
   *
   * Purpose: Output propagation delay measurement results to Report Window
   *
   *----------------------------------------------------------------------*
   * Description:
   *   const PropDelayResult& results:
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
      PropDelayResult& results,
      const STRING& output,
      INT ftstResult)
  {
	TMU_RESULTS siteResults;
	// If output parameter is different from "ReportUI", just retun
    if ( output != "ReportUI" ) {
      return;
    }
    logMissingPins(results.missingpins);

    if(results.hasFailed())
    {
    	printf(results.errorStr);
    	return;
    }
    printMissingPins(results.missingpins);
    try
    {
      std::map<STRING, PropDelayResult::PinResult>::iterator pinIter;
      for(pinIter = results.valueResult.begin();pinIter != results.valueResult.end();pinIter++)
      {
        STRING pin = pinIter->first;
        if(pinIter->second.isValid())
        {
          double mean = pinIter->second.mean;
          double max  = pinIter->second.max;
          double min  = pinIter->second.min;

          printf("Pin: [%s]  -------------\n",pin.c_str() );
          printf( "minValue  = %-10g   sec\n",min);
          printf( "maxValue  = %-10g   sec\n",max);
          printf( "meanValue = %-10g   sec\n",mean);

        } // if(pinIter->second.isValid())
        else
        {
          printf("Pin: [%s] --------------\n",pin.c_str());
          // output the "meanValue" result with NO VALID RESULT
          printf("minValue  = **********   sec\t*** NO VALID RESULT ***\n");
          printf("maxValue  = **********   sec\t*** NO VALID RESULT ***\n");
          printf("meanValue = **********   sec\t*** NO VALID RESULT ***\n");
        }

      } // for(pinIter =

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
      throw Error("TmuPropDelayUtil::reportToUI()",(e.msg()).c_str());
    }

  }
  static void errorStatesToString(UINT32 error, STRING& errstr)
  {
    errstr.clear();
    if(error == 0)
    {
      errstr = "NO_OVERRUN";
    }
    else
    {
      UINT32 tmp = error & TMU::ARM_OVERRUN;
      if(tmp != 0) errstr += TMU::toStr((TMU::ERROR_TYPE)tmp) ;
      tmp = error & TMU::VDL_OVERRUN;
      if(tmp != 0) errstr += " " + TMU::toStr((TMU::ERROR_TYPE)tmp) ;
      tmp = error & TMU::DEGLITCH_OVERRUN;
      if(tmp != 0) errstr += " " + TMU::toStr((TMU::ERROR_TYPE)tmp) ;
      tmp = error & TMU::OUT_OF_MEMORY;
      if(tmp != 0) errstr += " " + TMU::toStr((TMU::ERROR_TYPE)tmp) ;
    }
  }



  static void computeDelays(PropDelayResult& results,UINT32 meas)
  {
    TMU_RESULTS siteResults;
    // build difference between the results.
    if(results.drvPinCnt != results.rcvPinCnt)
    {
      throw Error("TmuPropDelayUtil::computeDelays()",
          "Not all pins mesured!");
    }

    for (UINT32 i= 0; i < results.drvPinCnt; i++)
    {
      Wave<DOUBLE> wave_drv;
      Wave<DOUBLE> wave_rcv;

      siteResults["drv"].getRawData(results.drvpins[i], &wave_drv, meas);
      siteResults["rcv"].getRawData(results.rcvpins[i], &wave_rcv, meas);
      try
      {
        wave_rcv -= wave_drv;
      }
      catch(Error& e)
      {
        std::cout << "wave error ########" << std::endl;
        // throw e;
      }

      if(wave_rcv.getSize() > 0)
      {
        PropDelayResult::PinResult& pinResult = results.valueResult[results.rcvpins[i]];
        pinResult.max = wave_rcv.max();
        pinResult.min = wave_rcv.min();
        pinResult.mean = wave_rcv.mean();
      }
    }
  }

};

#endif /*TMUPROPDELAY_H_*/
