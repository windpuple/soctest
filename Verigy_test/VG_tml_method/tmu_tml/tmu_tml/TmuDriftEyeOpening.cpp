//for testmethod framework interfaces
#include "testmethod.hpp"
//for testmethod API interfaces
#include "mapi.hpp"

#include "TMU_Commons.hpp"
#include "TmuDriftEyeOpeningUtil.hpp"

/**
  *----------------------------------------------------------------------*
  * @Testmethod Class: DriftEyeOpening
  *
  * @Purpose: rise time and fall time measurement with per pin TIA
  *
  *----------------------------------------------------------------------*
  *
  *----------------------------------------------------------------------*
 *
 * @Description:
  *   STRING& pins:                 {@ | pin and/or pin group list}
  *     Name of pin(s) and/or pin group(s) to be measured
  *     Valid pins: all digital pins.
  *   double datarate                     {}
  *     # max expected datarate of signal to be maesured
  *     # if omitted prescaler and deglitcher are used
  *   INT     deglitcher;           {3.3 < n < 53 } (ns)
  *   INT     prescaler;            {1...16}
  *     #
  *   INT meas_per_shot                 {}
  *     # number of armings(measurements) that should be made according to the
  *     # wavetable equation based setup
  *   INT sample_per_meas:              {}
  *     # of samples to be taken
  *   int     exitOnNotFinished;
  *     if TRUE  throw exception if not finished
  *   double  waitTimeout;
  *     timeout value for waitTMUfinished
  *   double  treshold;               {mV}
  *     # the treshold values for the shot
  *   INT ftstResult:               {0 | 1}
  *     Flag to specify whether taking functional test result into account
  *     for pass/fail judging and datalogging.
 *
 * @Note:
 *
 *----------------------------------------------------------------------*
 */

class DriftEyeOpening: public testmethod::TestMethod
{
protected:
	  string  pins;
	  string  mode;
	  double  datarate;
	  int     sample_per_meas; // samplecount
	  int     exitOnNotFinished;
	  double  waitTimeout;
	  double  threshold;
	  int     ftstResult;
	  string  output;
	  string  mTestName;
protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
  virtual void initialize()
  {
	    addParameter("pins",
	                 "PinString",
	                 &pins)
	      .setComment("Pin(s) and/or pin group(s) to be measured");
	    addParameter("mode",
	    	          "string",
	    	          &mode)
			      .setOptions("EYE:DRIFT")
	    	      .setComment("what to measured EYE or DRIFT");
	    addParameter("datarate [MBps]",
	               "double",
	               &datarate)
	    .setDefault("20.0")
	    .setComment("max expected datarate in MBps of signal to be maesured");
	    addParameter("sample_per_meas",
	              "int",
	              &sample_per_meas)
	    .setDefault("100")
	    .setComment("number of samples to be taken ");
	    addParameter("exit if TMU has not finished",
	    		     "int",
	    		     &exitOnNotFinished)
	        .setDefault("0")
	        .setOptions("0:1")
	        .setComment("exit the test if TMU has not finihed.");
	    addParameter("wait timeout [s]",
	               "double",
	               &waitTimeout)
	    .setDefault("1.e-6")
	    .setComment("timeout value the TMU should have completed");
	    addParameter("threshold [V]",
	                 "double",
	                 &threshold)
	    .setDefault("0.0")
	    .setComment("the threshold");
	    addParameter("ftstResult",
	               "int",
	               &ftstResult)
	    .setDefault("1")
	    .setOptions("0:1")
	    .setComment("Check p/f result of functional test?");
	    addParameter("output",
	                 "string",
	                 &output)
	      .setDefault("None")
	      .setOptions("None:ReportUI")
	      .setComment("Print results to UI report window");
	    addParameter("testName",
	                  "string",
	                  &mTestName)
	      .setDefault("eyeValue")
	      .setComment("test limit's name, default is \"meanValue\"\n"
	             "if test table is used, the limit is defined in file\n"
	             "if predefined limit is used, the limit name must be as same as default.");
    addLimit("eyeValue");
    addLimit("driftValue");
  }
  
  /**
   *This test is invoked per site.
   */
  virtual void run()
  {
      static TmuDriftEyeOpeningUtil::DriftEyeOpeningParameter params;
      TmuDriftEyeOpeningUtil::DriftEyeOpeningResult results;
      
      ON_FIRST_INVOCATION_BEGIN();
        /*
         * Process all parameters needed in test. Actually this function is
         * called once under multisite because all input parameters should be
         * the same through all sites. The processed parameters are stored into
         * the static variable 'params' and it's refered by the doMeasurement() 
         * for every site.
         */
        TmuDriftEyeOpeningUtil::processParameters(
                                              pins,
                                              mode,
                                              datarate,
                                              sample_per_meas,
                                              exitOnNotFinished,
                                              waitTimeout,
                                              threshold,
                                              ftstResult,
                                              params);
      ON_FIRST_INVOCATION_END();
  
      /*
       * Execute measurement with the specified 'params' and store results
       * into the 'results'. The multisite handling, i.e. ON_FIRST_INVOCATION
       * block are executed inside this function.
       */
      TmuDriftEyeOpeningUtil::doMeasurement(params, results);
  
      /*
       * Judge and datalog based on the 'results'. This function contains
       * pre-defined and hard-coded test name (e.g. "minValue"), so if
       * you'd like to use your own test names for judgement and datalogging
       * it's needed to modify this funciton or create new one.
       */
      TmuDriftEyeOpeningUtil::judgeAndDatalog(mTestName,results, params.ftstResult);
                                      
      /*
       * Output contents of the 'results' to Report Window if specified by
       * the "output" parameter.
       */
      TmuDriftEyeOpeningUtil::reportToUI(results, output, params.ftstResult);
  
      return ;
  }

  /**
   *This function will be invoked once the specified parameter's value is changed.
   *@param parameterIdentifier
   *
   *Note: TestMethod API should not be used in this method.
   */
  virtual void postParameterChange(const string& parameterIdentifier)
  {
    return;
  }

  /**
   *This function will be invoked once the Select Test Method Dialog is opened.
   */
  virtual const string  getComment() const
  {
     static string comment = _TML_VERSION;
     return comment;
  }

};

REGISTER_TESTMETHOD("TmuTest.DriftEyeOpening", DriftEyeOpening);
