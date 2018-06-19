#ifndef TMUFREQUENCY_H_
#define TMUFREQUENCY_H_

#include "TMUTask.h"
#include <float.h>

#include "TMU_Commons.hpp"
#include <iostream>

class TmuFrequencyUtil
{
public:


  /*
   *----------------------------------------------------------------------*
   * @Struct: FrequencyParameter
   *
   * @Purpose: Container to store frequency/period measurement parameters
   *
   *----------------------------------------------------------------------*
   * @Description:
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
   *   STRING mode:                  {FREQ+ | FREQ- | PER+ | PER-}
   *     Measurement mode.
   *     FREQ+: frequency (from rising edge to rising edge)
   *     FREQ-: frequency (from falling edge to falling edge)
   *     PER+: period (from rising edge to rising edge)
   *     PER-: period (from falling edge to falling edge)
   *     PERALL+ period (from rising edge to rising edge)
   *     PERALL- period (from falling edge to falling edge)
   *   double datarate                     {}
   *     # max expected datarate of signal to be maesured
   *     # if omitted prescaler and deglitcher are used
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
   *   The following parameters belong to group 2.
   *
   *   The following parameters belong to group 3.
   *
   *   TMU::EdgeType slopeA:
   *     Event slope of event A. It's determined from the mode parameter
   *   Boolean isFrequency:
   *     Flag to indicate whether frequency or period is measured or not
   *     It's determined from the mode parameter
   *
   * @Note:
   *   When you create an instance of this class, appropriate default values
   *   except pins are set to each variables in the class.
   *
   *
   *----------------------------------------------------------------------*
   */

  struct FrequencyParameter
  {
      // TODO complete this definitions and do documentation
      // Parameters specified by a user
      STRING  pins;
      STRING  mode;
      double  datarate;
      UINT64  samples_per_meas;
      UINT64  initial_discard;
      Boolean exitOnNotFinished;
      double  waitTimeout;
      UINT64  inter_sample_discard;
      INT     ftstResult;

      // Parameters specified by a user, possibly modified by processParameters

      // Parameters set by processParameters()
      TMU::ApplicationType appType;
      Boolean allSamples;
      TMU::EdgeType slopeA;
      Boolean isFrequency;

      // Default constructor
      // all parameters are set to default values
      FrequencyParameter()
      : pins(""),
        mode("FREQ+"),
        datarate(0.0),
        samples_per_meas(1),
        initial_discard(0),
        exitOnNotFinished(FALSE),
        waitTimeout(1e-6),
        inter_sample_discard(0),
        ftstResult(0),
        appType(TMU::APP_PERIOD),
        allSamples(FALSE),
        slopeA(TMU::RISE),
        isFrequency(TRUE) {}
  };
  
    
  /*
   *----------------------------------------------------------------------*
   * @class: FrequencyResult
   *
   * @Purpose: Container to store tmu measurement results
   *
   *----------------------------------------------------------------------*
   */


  class FrequencyResult : public TMU_TML_RESULT_BASE
  {
  public:
	 TMU::ApplicationType appType;
     Boolean isFrequency;
     Boolean allSamples;
     UINT64 excpectedNumSamples;
	 FrequencyResult():TMU_TML_RESULT_BASE()
					 ,appType(TMU::APP_NONE),isFrequency(FALSE),allSamples(FALSE),excpectedNumSamples(0){}
  };


/*
 *----------------------------------------------------------------------*
 * Utility functions for Frequency/Period measurements
 *----------------------------------------------------------------------*
 */

  /*
   *----------------------------------------------------------------------*
   * Routine: TmuFrequencyUtil::processParameters
   *
   * Purpose: Store given measurement parameters to the specified
   *          placeholder and determine addtional parameters
   *          which is necessary to execute measurement.
   *          Also performs some error checks on parameters.
   *
   *----------------------------------------------------------------------*
   * Description:
   *   const STRING& pins:
   *   const STRING& mode:           {FREQ+ | FREQ- | PER+ | PER-}
   *   double datarate                     {}
   *     # max expected datarate of signal to be maesured
   *     # if omitted prescaler and deglitcher are used
   *   INT samples_per_meas:             {}
   *     # of samples to be taken
   *     # this value is for setup
   *   int     initial_discard;          {}
   *     # number od sample to discard at beginning
   *   int     inter_sample_discard;     {}
   *     # number od events to discard between to samples
   *   INT ftstResult:               {0 | 1}
   *     Those are parameters for frequency/period measurement.
   *     See the descriptions in FrequencyParameter definition.
   *   FrequencyParameter& params:
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
                                UINT64 initial_discard,
                                INT exitOnNotFinished,
                                double  waitTimeout,
                                UINT64 inter_sample_discard,
                                INT ftstResult,
                                FrequencyParameter& params)
  {

       // If no pin for pins is specified, an exception is thrown.
       if (pins.size() == 0)
       {
         throw Error("TmuFrequencyUtil::processParameters()",
                       "Empty pins parameter.");
       }
  
       if (datarate <= 0.0)
       {
         throw Error("TmuFrequencyUtil::processParameters()",
                                "Wrong datarate parameter.");
       }


      // TODO replace with tmu own definitions
       params.appType = TMU::APP_PERIOD;
       params.allSamples = FALSE;
       if (mode == "FREQ+")
       {
           params.slopeA = TMU::RISE;
           params.isFrequency = TRUE;
       }
       else if (mode == "PER+")
       {
           params.slopeA = TMU::RISE;
           params.isFrequency = FALSE;
       }
       else if (mode == "FREQ-")
       {
           params.slopeA = TMU::FALL;
           params.isFrequency = TRUE;
       }
       else if (mode == "PER-")
       {
           params.slopeA = TMU::FALL;
           params.isFrequency = FALSE;
       }
       else if (mode == "PERALL+")
       {
           params.slopeA = TMU::RISE;
           params.isFrequency = FALSE;
           params.allSamples = TRUE;

       }
       else if (mode == "PERALL-")
       {
           params.slopeA = TMU::FALL;
           params.isFrequency = FALSE;
           params.allSamples = TRUE;
       }
       else
       {
           // If the specified mode is not supported in this function,
           // an exception is thrown.
           throw Error("TmuFrequencyUtil::processParameters()",
                       "Illegal mode parameter.");
       }




       // Copy other parameters
       params.pins = pins;
       params.mode = mode;
       params.datarate = datarate;
       params.samples_per_meas  = samples_per_meas;
       params.ftstResult        = ftstResult;
       params.initial_discard   = initial_discard;
       params.inter_sample_discard = inter_sample_discard;
       params.ftstResult 		= ftstResult;
       params.exitOnNotFinished = (exitOnNotFinished == 1)?TRUE:FALSE;
       params.waitTimeout		= waitTimeout;
  }
  
  /*
   *----------------------------------------------------------------------*
   * Routine: TmuFrequencyUtil::doMeasurement
   *
   * Purpose: Perform setup, execution and result retrieval
   *          for frequency/period measurement with tmu
   *
   *----------------------------------------------------------------------*
   *
   * Description:
   *   const FrequencyParameter& params:
   *     Container to hold parameters for the measurement.
   *   TmuMeasurementResult& results:
   *     Container to store measurement results
   *     
   * Note:
   *
   *----------------------------------------------------------------------*
   */
  static void doMeasurement(
                            const FrequencyParameter& params,
                            FrequencyResult& results)
  {
	  TMU_TASK task;
	  TMU_RESULTS siteResults;
      // Measurement setup, execution and result retrieval are done
      // through task object
      STRING ErrorString;
      Boolean exception_caught = FALSE;
      Boolean finished = TRUE;
      INT site = CURRENT_SITE_NUMBER();

      try
      {
        ON_FIRST_INVOCATION_BEGIN();
        CONNECT() ;
        //avoid ON_FIRST_INVOCATION_END executed so catch exceptions
        try
        {

          // NOTE!!
          // inter_sample_discard will be ignored if it is lower then
          // calculated by setDatarate
          // prescaler will allways be ignored if
          // datarate > 0
          UINT64 interdiscard = params.inter_sample_discard;

          task.pin(params.pins).setDatarate(params.datarate MBps)
			       .setNumShots(1)
			       .setNumMeasurements(1)
			       .setEdgeSelect(params.slopeA)
			       .setNumSamples(params.samples_per_meas)
			       .setInitialDiscard(params.initial_discard)
			       .setInterSampleDiscard(interdiscard);

          /*
           * TMU measurement execution
           */
          task.setup();
          task.setAsPrimary();
          task.execute();

          task.pin(params.pins).waitTMUfinished(params.waitTimeout,finished);

          if((params.exitOnNotFinished == FALSE)
          		|| (finished == TRUE))
          {
              siteResults.firstSample(0);
              siteResults.numSamples(params.samples_per_meas);
        	  if(params.allSamples == FALSE)
        	  {
        		  // load single averaged period value  (2 samples and high interdiscard)
        		  task.pin(params.pins).uploadPeriodResults(siteResults);
        	  }
        	  else if(params.allSamples == TRUE)
        	  {
        		  // load all periods and averaged values (max min etc.) if needed
        		  task.pin(params.pins).uploadPeriodResults(siteResults,false);
        	  }
          }


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

        if((params.exitOnNotFinished == TRUE)
        		&& (finished == FALSE))
        {
        	throw TESTMETHOD_API::TMException("TmuFrequencyUtil::doMeasurement()","TMU has not finished","");
        }
        if(exception_caught)
        {
        	throw TESTMETHOD_API::TMException("TmuFrequencyUtil::doMeasurement()",ErrorString.c_str(),"");
        }

        // initialize siteResults
        results.funcTestResult  = task.getStatus();
        results.appType         = params.appType;
        results.isFrequency     = params.isFrequency;
        results.allSamples      = params.allSamples;
        results.pinlist			= params.pins;
        results.excpectedNumSamples = params.samples_per_meas;
      }
      catch(TESTMETHOD_API::TMException& e)
      {
    	TESTSET().cont(TM::CONTINUE).
    	  		judgeAndLog_ParametricTest("", "", TM::Fail, 0);
    	throw Error("TmuFrequencyUtil::doMeasurement() ",(e.msg()).c_str());
      }
      if (FALSE == siteResults.checkSiteAppTypeStored(params.appType))
      {
    	 results.noResultsForSite = TRUE;
    	 sprintf(results.errorStr,"No frequency measurement results  available for site %d",site);
      }
  }
  

  /*
   *----------------------------------------------------------------------*
   * Routine: TmuFrequencyUtil::judgeAndDatalog
   *
   * Purpose: Perform pass / fail judgement and datalogging
   *          for frequency/period measurement
   *
   *----------------------------------------------------------------------*
   * Description:
   *     FrequencyResult& results:
   *     Container to store measurement results
   *   INT ftstResult:               {0 | 1}
   *     Flag to specify whether taking functional test result into account
   *     for pass/fail judging and datalogging.
   *   
   * Note:
   *   judgeAndLog_ParametricTest() is called for results in the order of
   *   TM::MEAN, and then functional test results.
   *   Even if one of results is FAIL, test method is executed
   *   until all avaiable results are logged.
   *
   *----------------------------------------------------------------------*
   */
  static void judgeAndDatalog(const string& testname,
                              FrequencyResult& results,
                              INT ftstResult)
  {
	TMU_RESULTS siteResults;
    INT site = CURRENT_SITE_NUMBER();
    TMU::ApplicationType appType =  results.appType;

    try
    {
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
		  if (results.isFrequency > 0) {
			  siValue.setBaseUnit("MHz");
			  siValue.inputValueUnit("1[MHz]");
		  } else {
			  siValue.setBaseUnit("nsec");
			  siValue.inputValueUnit("1[sec]");
		  }
		  factor =  siValue.getValueAsTargetUnit(limit.unit());

	  }
      ON_FIRST_INVOCATION_END();
      // ------!!!----- allways avoid nullpointer access --------------
      // If nothing stored in results for this applicationtype return
      if (results.hasFailed())
      {
    	TESTSET().cont(TM::CONTINUE).
				judgeAndLog_ParametricTest("", "", TM::Fail, 0);
        PUT_DATALOG(results.errorStr);
    	return;
      }

      // if there is no pinresult contained in results just return
      STRING_VECTOR& pinvector = results.pinvector;
      if (siteResults.getNumberOfMeasuredPins(pinvector, appType,site) == 0)
      {
      	TESTSET().cont(TM::CONTINUE).
  				judgeAndLog_ParametricTest("", "", TM::Fail, 0);
      	results.noPinsHasResults = TRUE;
      	sprintf(results.errorStr,"No TMU Results for site : %d\n" ,site);
      	return;
      }
      getMissingPins(results.missingpins,pinvector,results.pinlist);
      logMissingPins(results.missingpins);

      for(size_t pinindex = 0; pinindex < pinvector.size(); pinindex++)
       {
         STRING& pin = pinvector[pinindex];
         double mean = siteResults.getApplicationMean(pin,appType,site);
         if (sIsLimitTableUsed)
         {
           if(mean > 0.0)
           {
             double resultvalue = (results.isFrequency)? 1/mean:mean;
             TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,testname,
             V93kLimits::tmLimits,factor * resultvalue);
           }
           else if(results.allSamples == FALSE)
           {
             TESTSET().cont(TM::CONTINUE).reliability(TM::UNRELIABLE).
               judgeAndLog_ParametricTest(pin,testname,V93kLimits::tmLimits, 0);
           }
         }
         else
         {
           if(mean > 0.0)
           {
             double resultvalue = (results.isFrequency)? 1/mean:mean ;
             TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,
                                                          testname,resultvalue);
           }
           else if(results.allSamples == FALSE)
           {
             TESTSET().cont(TM::CONTINUE).reliability(TM::UNRELIABLE).
                 judgeAndLog_ParametricTest(pin,testname,TM::Fail, 0);
           }
         }
         if(results.allSamples == TRUE)
         {
           ARRAY_D values;
           UINT32 numArmings = 1; //siteResults.getNumberOfMeasurements(pin,appType,site);
           if (sIsLimitTableUsed)
           {
             if(siteResults.getPinMeasuredResults(pin,values,appType,numArmings))
             {
               for(int k = 0; k < values.size();k++)
               {
                 double resultvalue = (results.isFrequency)? 1/values[k]:values[k];
                 TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,
                     testname,V93kLimits::tmLimits,factor * resultvalue);
               }
             }

             // are there all asumed samples measured
             if(values.size() != (INT64)(results.excpectedNumSamples-1))
             {
               TESTSET().cont(TM::CONTINUE).reliability(TM::UNRELIABLE).
                   judgeAndLog_ParametricTest(pin,testname, V93kLimits::tmLimits, 0);
             }
           }
           else
           {
             if(siteResults.getPinMeasuredResults(pin,values,appType,numArmings))
             {
               for(int k = 0; k < values.size();k++)
               {
                 double resultvalue = (results.isFrequency)? 1/values[k]:values[k];
                 TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(pin,
                     testname,resultvalue );
               }
             }

             // are there all asumed samples measured
             if(values.size() != ((INT64)results.excpectedNumSamples-1))
             {
                TESTSET().cont(TM::CONTINUE).reliability(TM::UNRELIABLE).
                  judgeAndLog_ParametricTest(pin,testname, TM::Fail, 0);
             }
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
    catch(Error& e)
    {
      throw Error("TmuFrequencyUtil::judgeAndDatalog()",(e.msg()).c_str());
    }
  }


  
  /*
   *----------------------------------------------------------------------*
   * Routine: TmuFrequencyUtil::reportToUI
   *
   * Purpose: Output frequency/period measurement results to Report Window
   *
   *----------------------------------------------------------------------*
   * Description:
   *     const FrequencyResult& results:
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
                        FrequencyResult& results,
                        const STRING& output,
                        INT isFrequency,
                        INT ftstResult)
  {
	TMU_RESULTS siteResults;
    try
    {
      // If output parameter is different from "ReportUI", just retun
      if ( output != "ReportUI" ) {
        return;
      }
      INT site = CURRENT_SITE_NUMBER();
      TMU::ApplicationType appType =  results.appType;

      // ------!!!----- allways avoid nullpointer access --------------
      // If nothing stored in results for this applicationtype return
      if (results.hasFailed())
      {
    	printf("%s",results.errorStr);
        return;
      }

      printMissingPins(results.missingpins);

      // Determine the unit to display results
      const char* unitString = isFrequency ? "MHz" : "sec";
      // const DOUBLE unitScale = isFrequency ? 1e6 : 1e-9;

      STRING_VECTOR& pinvector = results.pinvector;
      for(size_t pinindex = 0; pinindex < pinvector.size(); pinindex++)
       {
         STRING& pin = pinvector[pinindex];
         UINT32 numArmings = 1; //siteResults.getNumberOfMeasurements(pin,appType,site);

         double mean = siteResults.getApplicationMean(pin,appType,site);
         if(mean > 0.0)
         {
           printf( "Pin: %s - meanValue = %-12.9g  %s\n",pin.c_str(),((results.isFrequency)? 1/mean/1e6:mean),unitString);
         }
         else if(results.allSamples == FALSE)
         {
           printf("Pin: [%s]  Measurement: [%d]\n",
               pin.c_str(), numArmings);
           // output the "meanValue" result with NO VALID RESULT
           printf("meanValue = **********   sec\t*** NO VALID RESULT ***\n");
         }
         if(results.allSamples == TRUE)
         {
           ARRAY_D values;
           ARRAY_LL indexes;

           if(siteResults.getPinMeasuredResults(pin,values,indexes,appType,numArmings))
           {
             if(siteResults.numSamples() > 2)
             {
               printf("max        = %-12.9g  %s\n",siteResults.getPeriodMax(pin,numArmings),unitString);
               printf("min        = %-12.9g  %s\n",siteResults.getPeriodMin(pin,numArmings),unitString);
               printf("median     = %-12.9g  %s\n",siteResults.getPeriodMedian(pin,numArmings),unitString);
               printf("stddev     = %-12.9g  %s\n",siteResults.getPeriodStddev(pin,numArmings),unitString);
               printf("pp         = %-12.9g  %s\n",siteResults.getPeriodPP(pin,numArmings),unitString);
             }
             for(int k = 0; k < values.size();k++)
             {
               printf("sample %-10ld:  sampleValue = %-12.9g   %s\n",
                   indexes[k],((results.isFrequency)? 1/values[k]:values[k]),unitString);
             }
           }
           // are there all asumed samples measured
           if(values.size() != ((INT64)results.excpectedNumSamples-1))
           {
             printf("*** NOT ALL ALL SAMPOLES MEASURED for pin %s ***\n",pin.c_str());

           }
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
      throw Error("TmuFrequencyUtil::reportToUI()",(e.msg()).c_str());
    }

  }
};

#endif /*TMUFREQUENCY_H_*/
