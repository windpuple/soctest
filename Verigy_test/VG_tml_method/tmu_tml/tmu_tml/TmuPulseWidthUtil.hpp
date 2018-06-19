#ifndef TMUPULSEWIDTH_H_
#define TMUPULSEWIDTH_H_

#include "TMUTask.h"
#include <float.h>

class TmuPulseWidthUtil
{
public:

  /*
   *----------------------------------------------------------------------*
   * Struct: PulseWidthParameter
   *
   * Purpose: Container to store pulse width measurement parameters
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
   *   STRING& pins:                 {@ | pin and/or pin group list}
   *     Name of pin(s) and/or pin group(s) to be measured
   *     Valid pins: all digital pins.
   *   DOUBLE daatarate;
   *   INT samples_per_meas:             {}
   *     # of samples to be taken
   *     # this value is for setup
   *   int     initial_discard;          {}
   *     # number od sample to discard at beginning
   *   int     inter_sample_discard;     {}
   *     # number od events to discard between to samples
   *   INT ftstResult:               {0 | 1}
   *     Flag to specify whether taking functional test result into account
   *     for pass/fail judging and datalogging.
   *
   * Note:
   *   When you create an instance of this class, appropriate default values
   *   except pins are set to each variables in the class.
   *
   *----------------------------------------------------------------------*
   */
  struct PulseWidthParameter
  {
    // Parameters specified by a user
    STRING  pins;
    double  datarate;
    int     prescaler;
    UINT64  samples_per_meas;
    UINT64  initial_discard;
    UINT64  inter_sample_discard;
    Boolean exitOnNotFinished;
    double  waitTimeout;
    INT     ftstResult;

    // Parameters specified by a user, possibly modified by processParameters

    // Parameters set by processParameters()
    TMU::ApplicationType appType;
    TMU::EdgeType slope;

    // Default constructor
    // all parameters are set to default values
    PulseWidthParameter()
    : pins(""),
      datarate(20),
      prescaler(0),
      samples_per_meas(1),
      initial_discard(0),
      inter_sample_discard(0),
      exitOnNotFinished(FALSE),
      waitTimeout(0.0),
      ftstResult(0),
      appType(TMU::APP_RAW),
      slope(TMU::RISE_FALL)
      {}
  };

  /*
   *----------------------------------------------------------------------*
   * @Struct: PulseWidthResult
   *
   * @Purpose: Container to store tmu measurement results
   *
   *----------------------------------------------------------------------*
   */

  class PulseWidthResult  : public TMU_TML_RESULT_BASE
  {
  public:
    std::map < STRING, Wave < DOUBLE > > highPulses;
    std::map < STRING, Wave < DOUBLE > > lowPulses;
    std::map < STRING, double > meanHighPulse;
    std::map < STRING, double > meanLowPulse;
    PulseWidthResult():TMU_TML_RESULT_BASE() {}
  };

  /*
   *----------------------------------------------------------------------*
   * Utility functions for Pulse Width measurements
   *----------------------------------------------------------------------*
   */

  /*
   *----------------------------------------------------------------------*
   * Routine: TmuPulseWidthUtil::processParameters
   *
   * Purpose: Store given measurement parameters to the specified
   *          placeholder and determine addtional parameters
   *          which is necessary to execute measurement.
   *          Also performs some error checks on parameters.
   *
   *----------------------------------------------------------------------*
   * Description:
   *
   *    const STRING&  pins, 
   *    const STRING&  mode,          {PW+ | PW-}
   *   INT     prescaler;            {0..16}
   *   STRING& signaltype            {CLK | DATA}
   *
   *   INT meas_per_shot                 {}
   *     # number of armings that should be made according to the
   *     # wavetable equation based setup
   *   INT samples_per_meas:             {}
   *     # of samples to be taken
   *     # this value is for setup
   *   int     initial_discard;          {}
   *     # number od sample to discard at beginning
   *   int     inter_sample_discard;     {}
   *     # number od events to discard between to samples
   *    INT ftstResult:               {0 | 1}
   *      Those are parameters for pulse width measurement.
   *      See the descriptions in PulseWidthParameter definition.
   *    PulseWidthParameter& params:
   *      Container to hold parameters for the measurement.
   *   
   * Note:
   *
   *----------------------------------------------------------------------*
   */
  static void processParameters(
                         const STRING& pins,
                                double datarate,
                                UINT64 samples_per_meas,
                                UINT64 initial_discard,
                                UINT64 inter_sample_discard,
                                int     exitOnNotFinished,
                                double  waitTimeout,
								INT    ftstResult,
                                PulseWidthParameter& params)
  {

    // If no pin for pins is specified, an exception is thrown.
    if (pins.size() == 0)
    {
      throw Error("TmuPulseWidthUtil::processParameters()",
          "Empty pins parameter.");
    }

    // Copy other parameters
    params.pins = pins;
    params.samples_per_meas  = samples_per_meas;
    params.ftstResult        = ftstResult;
    params.datarate          = datarate;
    params.initial_discard   = initial_discard;
    params.inter_sample_discard = inter_sample_discard;
    params.exitOnNotFinished = (exitOnNotFinished == 1)?TRUE:FALSE;
    params.waitTimeout		= waitTimeout;

  }

  /*
   *----------------------------------------------------------------------*
   * Routine: TmuPulseWidthUtil::doMeasurement
   *
   * Purpose: Perform setup, execution and result retrieval
   *          for pulse width measurement with tmu
   *
   *----------------------------------------------------------------------*
   *
   * Description:
   *   const PulseWidthParameter& params:
   *     Container to hold parameters for the measurement.
   *   TmuMeasurementResult& results:
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
      PulseWidthParameter& params,
      PulseWidthResult& results)
  {
    // Measurement setup, execution and result retrieval are done
    // through task object
    TMU_TASK task;
    TMU_RESULTS siteResults;
    CONNECT();
    INT site = CURRENT_SITE_NUMBER();
    STRING ErrorString;
    Boolean exception_caught = FALSE;
    Boolean finished = TRUE;
    try
    {
      site = CURRENT_SITE_NUMBER();
      ON_FIRST_INVOCATION_BEGIN();

      siteResults.firstSample(0).numSamples(params.samples_per_meas);
      //avoid ON_FIRST_INVOCATION_END executed so catch exceptions
      try
      {
        TMU::DATARATE_DEPENDENT_VALUES_TYPE dpsettings;
        task.pin(params.pins).setEdgeSelect(params.slope)
                             .setDatarate(params.datarate MBps)
                             .setNumMeasurements(1)
                             .setNumShots(1)
                             .setNumSamples(params.samples_per_meas)
                             .setInitialDiscard(params.initial_discard)
                             .setInterSampleDiscard(params.inter_sample_discard)
                             .getDatarateDependendSettings(dpsettings);

        params.prescaler = dpsettings.prescaler;
        params.inter_sample_discard = dpsettings.interdiscard;

        /*
         * TMU measurement execution
         */
        task.setAsPrimary();
        task.setup();
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
        // catch only to be able to finnish the  ON_FIRST_INVOCATION block
        ErrorString = (e.msg()).c_str();
        exception_caught = TRUE;
      }

      ON_FIRST_INVOCATION_END();
      results.funcTestResult  = task.getStatus();
      results.pinlist		  = params.pins;

      if((params.exitOnNotFinished == TRUE)
       		&& (finished == FALSE))
      {
      	throw TESTMETHOD_API::TMException("TmuPulseWidthUtil::doMeasurement()","TMU has not finished","");
      }

      if(exception_caught)
      {
        throw TESTMETHOD_API::TMException("TmuPulseWidthUtil::doMeasurement()",ErrorString.c_str(),"");
      }
    }
    catch(TESTMETHOD_API::TMException& e)
    {
      throw Error("TmuPulseWidthUtil::doMeasurement() ",(e.msg()).c_str());
    }

    // check if results for appType available
   	if (FALSE == siteResults.checkSiteAppTypeStored(params.appType,site))
   	{
   		results.noResultsForSite = TRUE;
   		sprintf(results.errorStr,"No PulseWidth measurement results  available for site %d",site);
        return;
    }
    // If there is no pin contained in results return

    if(0 == siteResults.getNumberOfMeasuredPins(results.pinvector, params.appType))
    {
       results.noPinsHasResults = TRUE;
       sprintf(results.errorStr,"No PulseWidth measurement results  available for site %d",site);
       return;
    }
    getMissingPins(results.missingpins,results.pinvector,results.pinlist);

    if(!calculatePulseWith(params,results,site))
    {
       sprintf(results.errorStr,"calculatePulseWith failed for site %d",site);
       TESTSET().cont(TM::CONTINUE).
     	    	 judgeAndLog_ParametricTest("", "", TM::Fail, 0);
       PUT_DATALOG(results.errorStr);
       return;
    }

  }

  /*
   *----------------------------------------------------------------------*
   * Routine: TmuPulseWidthUtil::judgeAndDatalog
   *
   * Purpose: Perform pass / fail judgement and datalogging
   *          for Tmu measurement
   *
   *----------------------------------------------------------------------*
   * Description:
   *   const PulseWidthResult& results:
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
                              PulseWidthResult& results,
                              INT ftstResult)
  {
     static bool sIsLimitTableUsed = false;
     static double factor = 1.0;
     TMU_RESULTS siteResults;
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

		 testmethod::SpecValue siValue;
		 siValue.setBaseUnit("nsec");
		 siValue.inputValueUnit("1[sec]");
		 factor =  siValue.getValueAsTargetUnit(limit.unit());
	 }
     ON_FIRST_INVOCATION_END();
     logMissingPins(results.missingpins);
     if(results.hasFailed())
     {
     	TESTSET().cont(TM::CONTINUE).
 				judgeAndLog_ParametricTest("", "", TM::Fail, 0);
     	PUT_DATALOG(results.errorStr);
     	return;
     }

     try
     {

       for(size_t pinindex = 0; pinindex < results.pinvector.size(); pinindex++)
       {
         STRING& pin = results.pinvector[pinindex];


         if((results.lowPulses[pin].getSize() > 0) && (results.highPulses[pin].getSize() > 0) )
         {
           DOUBLE& lowmean = results.meanLowPulse[pin];
           DOUBLE& highmean = results.meanHighPulse[pin];
           TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,"meanHighPulse",highmean *factor);

           TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,"meanLowPulse", lowmean * factor);
         }
         else
         {
           TESTSET().cont(TM::CONTINUE).reliability(TM::UNRELIABLE).
               judgeAndLog_ParametricTest(pin,testname,V93kLimits::tmLimits, 0);
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
     catch(Error& e)
     {
       throw Error("TmuPulseWidthUtil::judgeAndDatalog()",(e.msg()).c_str());
     }
  }

  /*
   *----------------------------------------------------------------------*
   * Routine: TmuPulseWidthUtil::reportToUI
   *
   * Purpose: Output Tmu measurement results to Report Window
   *
   *----------------------------------------------------------------------*
   * Description:
   *   const PulseWidthResult& results:
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
      PulseWidthResult& results,
      const STRING& output,
      INT ftstResult)
  {

    try
    {
      // If output parameter is different from "ReportUI", just retun
      if ( output != "ReportUI" ) {
        return;
      }
      printMissingPins(results.missingpins);
      if(results.hasFailed())
      {
      	printf(results.errorStr);
      	return;
      }

      for(size_t pinindex = 0; pinindex < results.pinvector.size(); pinindex++)
       {
         STRING& pin = results.pinvector[pinindex];
         Wave < DOUBLE > & lowPulses  = results.lowPulses[pin];
         Wave < DOUBLE > & highPulses = results.highPulses[pin];
         if((lowPulses.getSize() > 0) && (highPulses.getSize() > 0) )
         {
           DOUBLE& lowmean = results.meanLowPulse[pin];
           DOUBLE& highmean = results.meanHighPulse[pin];
           printf("Pin: [%s]\n",pin.c_str());
           printf( "LowPulse mean  = %-10g   sec\n",lowmean);
           printf( "HighPulse mean = %-10g   sec\n",highmean);
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
      throw Error("TmuPulseWidthUtil::reportToUI()",(e.msg()).c_str());
    }

  }


  static bool calculatePulseWith(const PulseWidthParameter& params,PulseWidthResult& results, int site)
  {
    bool returnval = true;
    TMU_RESULTS siteResults;
    for(size_t pinindex = 0; pinindex < results.pinvector.size(); pinindex++)
    {
      STRING& pin = results.pinvector[pinindex];
      Wave<DOUBLE> samples,high,low;
      results.highPulses.insert(std::make_pair(pin,high));
      results.lowPulses.insert(std::make_pair(pin,low));
      try
      {
        if(siteResults.getRawData(pin,&samples,1,site))
        {
          INT numsamples = samples.getSize();
          printf("############### samples = %d\n",numsamples);
          INT k = 0;
		  results.highPulses[pin].setSize(numsamples / 2 );
		  results.lowPulses[pin].setSize(numsamples / 2 );
          for(INT i = 2; (i < numsamples) && (k < (numsamples / 2));i++)
          {
            DOUBLE period;
            if((i % 2) == 0) // every rising edge
            {
              //  factor = 2 periods * (prescaler + 0.5) * (interdiscard + 1)
			  double realPrescaler = params.prescaler + 0.5;
              double factor = 2.0 * realPrescaler * (double)(params.inter_sample_discard+1); 
              // calc period between 2 rising edges. 
              period = (samples[i] - samples[i-2]) / factor;
			  
			  				   
              // calc high pulse  falling edge - n periods
              results.highPulses[pin][k] = (samples[i-1] - samples[i-2]) 
							   - (period * params.prescaler)
							   - (period * realPrescaler * params.inter_sample_discard) ;
              results.lowPulses[pin][k]  = period -results.highPulses[pin][k];
              k++;
            }
          }
          results.highPulses[pin].setSize(k);
          results.lowPulses[pin].setSize(k);
          results.meanHighPulse[pin] = results.highPulses[pin].mean();
          results.meanLowPulse[pin]  = results.lowPulses[pin].mean();
        }
      }
      catch(Error& e)
      {
        printf("########### error in calculatePulseWith %s\n",e.msg().c_str());
        returnval = false;
      }
    }

    return true;
  }


};
#endif /*TMUPULSEWIDTH_H_*/
