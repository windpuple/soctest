#ifndef TMUJITTER_H_
#define TMUJITTER_H_

#include "TMUTask.h"
#include <float.h>

class TmuJitterUtil
{
public:

  /*
   *----------------------------------------------------------------------*
   * Struct: JitterParameter
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
   *     # if omitted prescaler and deglitcher are used
   *   INT     prescaler;            {0..16}
   *   INT samples_per_meas:             {}
   *     # of samples to be taken
   *   INT ftstResult:               {0 | 1}
   *     Flag to specify whether taking functional test result into account
   *     for pass/fail judging and datalogging.
   *   
   *   The following parameters belong to group 2.
   *
   *   The following parameters belong to group 3.
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
  struct JitterParameter
  {
      // Parameters specified by a user
      STRING  pins;
      STRING  mode;
      double  datarate;
      double  ui_ns;
      UINT32  prescaler;
      UINT64  samples_per_meas;
      Boolean exitOnNotFinished;
      double  waitTimeout;
      Boolean ftstResult;
      Boolean histogram;

      // Parameters specified by a user, possibly modified by processParameters

      // Parameters set by processParameters()
      TMU::EdgeType slope;
      


      // Default constructor
      // all parameters are set to default values
      JitterParameter()
      : pins(""),
        mode(""),
        datarate(0.0),
        ui_ns(0.0),
        prescaler(0),
        samples_per_meas(1),
	    exitOnNotFinished(FALSE),
        waitTimeout(0.0),
        ftstResult(FALSE),
        histogram(FALSE),
        slope(TMU::RISE)
        {}
  };
  
  /*
   *----------------------------------------------------------------------*
   * Struct: JitterResult
   *
   * Purpose: Container to store jitter measurement results
   *
   *----------------------------------------------------------------------*
   */

  class JitterWaves
  {
  public:
    ARRAY_D wave_meas;
    Wave<DOUBLE> wave_jitter;
    Wave<INT>    wave_histo;
    DOUBLE min;
    DOUBLE max;
    DOUBLE stddev;
  };

  class JitterResult : public TMU_TML_RESULT_BASE
  {
  public:
	Boolean periods;
    TMU::ApplicationType appType;
    Boolean histogram;
    UINT64 excpectedNumSamples;
    std::map < STRING, JitterWaves > pinWaveDatamap;
    JitterResult() : TMU_TML_RESULT_BASE(), periods(FALSE), appType(TMU::APP_RAW),histogram(FALSE),excpectedNumSamples(0) {}
    ~JitterResult()
    {
      pinWaveDatamap.clear();
    }

  };
  
  
/*
 *----------------------------------------------------------------------*
 * Utility functions for Jitter measurements
 *----------------------------------------------------------------------*
 */

  /*
   *----------------------------------------------------------------------*
   * Routine:TmuJitterUtil::processParameters
   *
   * Purpose: Store given measurement parameters to the specified
   *          placeholder and determine addtional parameters
   *          which is necessary to execute measurement.
   *          Also performs some error checks on parameters.
   *
   *----------------------------------------------------------------------*
   * Description:
   *   const STRING& pins:
   *     Measurement mode.
   *   double datarate                     {}
   *     # max expected datarate of signal to be maesured
   *     # if omitted prescaler and deglitcher are used
   *   INT     prescaler;            {0..16}
   *
   *   INT samples_per_meas:             {}
   *     # of samples to be taken
   *   double  treshold_a;               {V}
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

  static void processParameters(const STRING& pins,
							    const STRING& mode,
                                DOUBLE datarate,
                                DOUBLE UI_ns,
                                UINT32 prescaler,
                                UINT64 samples_per_meas,
                                INT exitOnNotFinished,
                                double waitTimeout,
                                INT ftstResult,
                                Boolean histogram,
                                JitterParameter& params)
  {
    // If no pin for pins is specified, an exception is thrown.
     if (pins.size() == 0)
     {
       throw Error("TmuJitterUtil::processParameters()",
                     "Empty pins parameter.");
     }

     if ((datarate <= 0.0) && (prescaler == 0))
     {
       throw Error("TmuJitterUtil::processParameters()",
                    "Wrong datarate and prescaler parameter.");
     }

     if (((mode == "DATA") || (mode == "DATA_PERIOD"))  && (UI_ns <= 0.0))
     {
    	 throw Error("TmuJitterUtil::processParameters()",
    	             "valid UI value expected for jitter mode DATA");
     }


     // TODO replace with tmu own definitions
       params.ui_ns = UI_ns; // ns
       params.mode  = mode;
       params.slope = TMU::RISE;
       params.pins  = pins;
       params.datarate = datarate;
       params.samples_per_meas = samples_per_meas;
       params.ftstResult = (ftstResult == 0)? FALSE:TRUE;
       params.prescaler = prescaler;
       params.histogram = histogram;
       params.exitOnNotFinished = (exitOnNotFinished == 1)?TRUE:FALSE;
       params.waitTimeout		= waitTimeout;
  }
  
  /*
   *----------------------------------------------------------------------*
   * Routine: TmuJitterUtil::doMeasurement
   *
   * Purpose: Perform setup, execution and result retrieval
   *          for rise/fall time measurement with per pin TIA
   *
   *----------------------------------------------------------------------*
   *
   * Description:
   *   const JitterParameter& params:
   *     Container to hold parameters for the measurement.
   *   JitterResult& results:
   *     Container to store measurement results
   *
   * Note:
   *   'static' variables are used in this function to keep some information
   *   which is refered in the execution for sites where ON_FIRST_INVOCATION 
   *   block is not executed.
   *
   *----------------------------------------------------------------------*
   */
  static void doMeasurement(const JitterParameter& params,
                            JitterResult& results)
  {
      // Measurement setup, execution and result retrieval are done
      // through task object
      TMU_TASK task;
      TMU_RESULTS siteResults;
      results.periods = FALSE;
      if((params.mode == "PERIOD")
 		  || (params.mode == "DATA_PERIOD"))
      {
    	  results.periods = TRUE;
      }
      INT site;
      STRING ErrorString;
      Boolean exception_caught = FALSE;
      Boolean finished = TRUE;
      results.histogram = params.histogram;

      try
      {
        CONNECT();
        site = CURRENT_SITE_NUMBER();
        ON_FIRST_INVOCATION_BEGIN();
        //avoid ON_FIRST_INVOCATION_END executed so catch exceptions
        try
        {
        	if(params.datarate > 0.0)
        	{
        		task.pin(params.pins).setDatarate(params.datarate MBps);
        	}
        	else if((params.prescaler > 0))
        	{
        		task.pin(params.pins)
                    		  .setPreScaler(params.prescaler)
                    		  .setInterSampleDiscard(0);
        	}


        	task.pin(params.pins).setEdgeSelect(params.slope)
                            		   .setNumSamples(params.samples_per_meas)
                            		   .setInitialDiscard(0)
                            		   .setNumMeasurements(1)
                            		   .setNumShots(1);

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
        catch(Error& e)  // catch here and set errorflag so     ON_FIRST_INVOCATION_END will be called
        {
        	ErrorString = (e.msg()).c_str();
        	exception_caught = TRUE;
        }

        ON_FIRST_INVOCATION_END();
        results.funcTestResult  = task.getStatus();
        results.pinlist = params.pins;
        results.excpectedNumSamples = params.samples_per_meas;
        /*
         * TMU measurement result retrieval
         */
        if((params.exitOnNotFinished == TRUE)
         		&& (finished == FALSE))
        {
        	throw TESTMETHOD_API::TMException("TmuJitterUtil::doMeasurement()","TMU has not finished","");
        }
        if(exception_caught)
        {
          throw TESTMETHOD_API::TMException("TmuJitterUtil::doMeasurement()",ErrorString.c_str(),"");
        }

      }
      catch(TESTMETHOD_API::TMException& e)
      {
        throw Error("TmuJitterUtil::doMeasurement() ",(e.msg()).c_str());
      }

      if (FALSE == siteResults.checkSiteAppTypeStored(TMU::APP_RAW))
      {
    	 results.noResultsForSite = TRUE;
    	 sprintf(results.errorStr,"No jitter measurement results  available for site %d",site);
      }
      else
      {
        // Get the status result from functional test if specified.
        results.funcTestResult = task.getStatus();

        STRING_VECTOR& pinvector = results.pinvector;
        if (siteResults.getNumberOfMeasuredPins(pinvector, results.appType,site) == 0)
        {
          results.noPinsHasResults = TRUE;
          sprintf(results.errorStr,"No pinresults available for Jitter measurement for site %d",CURRENT_SITE_NUMBER() );
          return;
        }
        getMissingPins(results.missingpins,pinvector,results.pinlist);

        for(size_t pinindex = 0; pinindex < pinvector.size(); pinindex++)
        {

          STRING& pin = pinvector[pinindex];
          UINT32 maes = 1; // armingID
          JitterWaves& waves = results.pinWaveDatamap[pin];
          siteResults.getPinMeasuredResults(pin,waves.wave_meas,TMU::APP_RAW,maes);
//          siteResults.getRawData(pin,&waves.wave_meas,maes);
          if(calculateJitterValues(waves.wave_meas,waves.wave_jitter,params))
          {
            INT size = waves.wave_jitter.getSize();

            waves.min  = waves.wave_jitter.min();
            waves.max  = waves.wave_jitter.max();
            waves.stddev     = waves.wave_jitter.stddev();

            DOUBLE jittermean = waves.wave_jitter.mean();  // determine zero point
            for(int ii = 0; ii < size;ii++)
            {
              waves.wave_jitter[ii] -= jittermean;         // allign around zero
            }
            waves.min -= jittermean;                   // allign around zero
            waves.max -= jittermean;
            if(results.histogram == TRUE)
            {
              INT nBins = (INT) (floor(sqrt((DOUBLE)(size - 1))) - 1);
              waves.wave_histo.histogram(waves.wave_jitter,nBins ,waves.min,waves.max);
            }
          }
        }
      }
  }

  /*
   *----------------------------------------------------------------------*
   * Routine: TmuJitterUtil::judgeAndDatalog
   *
   * Purpose: Perform pass / fail judgement and datalogging
   *          for jitter measurement
   *
   *----------------------------------------------------------------------*
   * Description:
   *   JitterResult& results:
   *     Container to store measurement results
   *   INT jitterHistogram:          {0 | 1}
   *     Flag to specify whether jitter histogram is logged
   *   INT ftstResult:               {0 | 1}
   *     Flag to specify whether taking functional test result into account
   *     for pass/fail judging and datalogging.
   *   
   * Note:
   *   Jitter histogram is logged first.
   *   judgeAndLog_ParametricTest() is called for results in the order of
   *   TM::STDEV, TM::MINMAX, and finally functional test
   *   results. Even if one of results is FAIL, test method is executed
   *   until all avaiable results are logged.
   *
   *----------------------------------------------------------------------*
   */
  static void judgeAndDatalog(const string& testname, JitterResult& results,
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
     try
     {
       if(results.hasFailed())
       {
    	   TESTSET().cont(TM::CONTINUE).
   				judgeAndLog_ParametricTest("", "", TM::Fail, 0);
    	   PUT_DATALOG(results.errorStr);
    	   return;
       }
       logMissingPins(results.missingpins);
       STRING_VECTOR& pinvector = results.pinvector;
       for(size_t pinindex = 0; pinindex < pinvector.size(); pinindex++)
       {
         STRING& pin = pinvector[pinindex];
         Boolean failed = FALSE;
         JitterWaves& waves = results.pinWaveDatamap[pin];
         INT size = waves.wave_jitter.getSize();
         if(size <= 0)
         {
           failed = TRUE;
         }
         else
         {
           double RMS = waves.wave_jitter.rms();
           TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,"rmsJitter", RMS* factor);
           double peek2peek = waves.max - waves.min;
           TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,"p2pJitter", peek2peek* factor);
           double max = waves.max;
           TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,"maxJitter", max* factor);
           double min = waves.min;
           TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,"minJitter", min* factor);
           double stddev = waves.stddev;
           TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,"stddevJitter", stddev* factor);
         }
         INT64 checksize = (results.periods == TRUE)?
        		 results.excpectedNumSamples -1: results.excpectedNumSamples;
         if(waves.wave_jitter.getSize() != checksize)
         {
           failed = true;
         }
         if(failed == TRUE)
         {
           if(sIsLimitTableUsed)
           {
        	   TESTSET().cont(TM::CONTINUE).reliability(TM::UNRELIABLE).
        			   judgeAndLog_ParametricTest(pin,testname,V93kLimits::tmLimits, 0);
           }
           else
           {
        	   TESTSET().cont(TM::CONTINUE).reliability(TM::UNRELIABLE).
        			   judgeAndLog_ParametricTest(pin,testname, 0);

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
       throw Error("TmuJitterUtil::judgeAndDatalog()",(e.msg()).c_str());
     }
  }
  
  /*
   *----------------------------------------------------------------------*
   * Routine: TmuJitterUtil::reportToUI
   *
   * Purpose: Output jitter measurement results to Report Window
   *
   *----------------------------------------------------------------------*
   * Description:
   *   JitterResult& results:
   *     Container to store measurement results
   *   INT ftstResult:               {0 | 1}
   *     Flag to specify whether to output functional test result
   *   const STRING& output          {None | ReportUI}
   *     Flag to specify whether to output 
   *     "None" means no output and "ReportUI" means to do output
   *
   * Note:
   *   "debug_analog" testflow flag should be set to see the jitter histogram
   *   on the singal analyzer
   *
   *----------------------------------------------------------------------*
   */
  static void reportToUI(JitterResult& results,
                         const STRING& output,
                         INT ftstResult)                  
  {
    TMU_RESULTS siteResults;
	try
    {
      // If output parameter is different from "ReportUI", just retun
      if (( output != "ReportUI" ) && ( output != "ReportUI_nosamples" )) {
        return;
      }
      printMissingPins(results.missingpins);

      // ------!!!----- allways avoid nullpointer access --------------
      // If nothing stored in results for this applicationtype return
      if(results.hasFailed())
      {
   	   printf(results.errorStr);
   	   return;
      }
      STRING_VECTOR& pinvector = results.pinvector;

      for(size_t pinindex = 0; pinindex < pinvector.size(); pinindex++)
      {
        STRING& pin = pinvector[pinindex];
        Boolean failed = FALSE;
        JitterWaves& waves = results.pinWaveDatamap[pin];
        INT size = waves.wave_jitter.getSize();
        if(size <= 0)
        {
          failed = TRUE;
          printf("Pin: [%s]  \n", pin.c_str());
          printf( "rmsJitter    **********   sec\t*** NO VALID RESULT ***\n");
          printf( "p2pJitter     **********   sec\t*** NO VALID RESULT ***\n");
        }
        else
        {
          printf("Pin: [%s]  \n", pin.c_str());
          if( output != "ReportUI_nosamples" )
          {
        	  for(int k = 0; k < size;k++)
        	  {
        		  printf("sampleJitter %-6d\t= % 13g   sec\n",k,waves.wave_jitter[k]);
        	  }
          }
          printf( "------------------------------------\n");
          printf( "rmsJitter          \t= % 13g   sec\n",waves.wave_jitter.rms());
          printf( "p2pJitter          \t= % 13g   sec\n",waves.max - waves.min);
          printf( "maxJitter          \t= % 13g   sec\n",waves.max);
          printf( "minJitter          \t= % 13g   sec\n",waves.min);
          printf( "stddevJitter       \t= % 13g   sec\n",waves.stddev);
          if(results.histogram == TRUE)
          {    // calculate bins and ARRAYs for signal analyzer
            INT nBins = (INT) (floor(sqrt((DOUBLE)(size - 1))) - 1);
            DOUBLE binWidth = (waves.max - waves.min) / nBins;
            ARRAY_D xseries;
            ARRAY_D histogram;
            xseries.resize(nBins);
            xseries[0] = waves.min;
            for(int i = 0;i < nBins;i++)
              xseries[i] = 1e12*(waves.min + (i*binWidth)); // bins as pico sec.

            char buf[64];
            sprintf(buf, "Jitter Histogram [%d]", (int)pinindex);
            histogram.resize(waves.wave_histo.getSize());
            for (int i = 0; i < waves.wave_histo.getSize(); i++)
              histogram[i] = 1.0*waves.wave_histo[i];
            PUT_DEBUG(pin.c_str(), buf,xseries,histogram);
          }

        }
          // are there all asumed samples measured
        INT64 checksize = (results.periods == TRUE)?
       		 results.excpectedNumSamples -1: results.excpectedNumSamples;
        if(size != checksize)
        {
          printf("           **********   not all samples measured\n");
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
      throw Error("TmuJitterUtil::reportToUI()",(e.msg()).c_str());
    }

  }

  static Boolean calculateJitterValues(ARRAY_D& wave_meas,Wave<DOUBLE>& wave_jitter,const JitterParameter& params)
  {
    Boolean ret = true;
    ARRAY_D& source = wave_meas;
    Wave<DOUBLE>& result = wave_jitter;
    TMU_RESULTS siteResults;
    try
    {
      if(source.size() <= 1)
        throw TESTMETHOD_API::TMException("TmuPropDelayUtil","calculateJitterValues()",
            "source array empty or only one sample");

      DOUBLE dummy,modTmp,periodMean;
      periodMean = 0.0;
      // determine period
      INT size = source.size();
      if(size < 2) return false;
      DOUBLE halfperiod;
      if((params.mode == "DATA")
         || (params.mode == "DATA_PERIOD"))
      {
    	  periodMean = params.ui_ns * 2e-9;
          halfperiod = params.ui_ns * 1e-9;
      }
      else
      {
    	  DSP_REG1(source,&periodMean,&dummy,0,size-1);
		  halfperiod = periodMean / 2.0;
      }

      // compute sample differences periodMean
      if(params.mode == "DATA")
      {
          result.setSize(size );
    	  for(int i = 0;i < size ;i++)
    	  {
    		  modTmp = remainder(source[i],halfperiod);
    		  result[i] = modTmp;
    	  }
      }
      else
      {
    	  if(params.mode == "CLK")
    	  {
    	      result.setSize(size );
        	  for(int i = 0;i < size ;i++)
        	  {
        		  modTmp = remainder(source[i],periodMean);
        		  result[i] = (modTmp > halfperiod)? modTmp - periodMean : modTmp;
        	  }
    	  }
    	  else // PERIOD || DATAPERIOD   -> PPTia correlating results
    	  {
    	      result.setSize(size -1);
        	  for(int i = 1;i < size ;i++)
        	  {
        		  modTmp = remainder(source[i]-source[i-1],periodMean);
        		  result[i-1] = modTmp;
        	  }
    	  }
      }
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
  
#endif /*TMUJITTER_H_*/
