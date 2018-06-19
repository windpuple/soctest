//for testmethod framework interfaces
#include "testmethod.hpp"
//for testmethod API interfaces
#include "mapi.hpp"

#include "TMU_Commons.hpp"
#include "TmuFrequencyUtil.hpp"
/**
 *----------------------------------------------------------------------*
 * @Testmethod Class: Frequency
 *
 * @Purpose: frequency and period measurement with tmu
 *
 *----------------------------------------------------------------------*
 *
 * @Description:
 *   STRING& pins:                 {@ | pin and/or pin group list}
 *     Name of pin(s) and/or pin group(s) to be measured
 *     Valid pins: all digital pins.
 *   STRING& mode:                 {FREQ+ | FREQ- | PER+ | PER-}
 *     Measurement mode.
 *     FREQ+: frequency (from rising edge to rising edge)
 *     FREQ-: frequency (from falling edge to falling edge)
 *     PER+: period (from rising edge to rising edge)
 *     PER-: period (from falling edge to falling edge)
 *     PERALL+: all periods (from rising edge to rising edge)
 *     PERALL-: all periods (from falling edge to falling edge)
 *   double datarate              {}
 *      max expected datarate of signal to be maesured
 *   INT sample_per_meas:              {}
 *     # of samples to be taken
 *   int     initial_discard;          {}
 *     # number od sample to discard at beginning
 *   int     exitOnNotFinished;
 *     if TRUE  throw exception if not finished
 *   double  waitTimeout;
 *     timeout value for waitTMUfinished
 *   int     inter_sample_discard;     {}
 *     # number od events to discard between to samples
*   INT ftstResult:               {0 | 1}
 *     Flag to specify whether taking functional test result into account
 *     for pass/fail judging and datalogging.
 *     
 * @Note:
 *   the naumber of armings depends on the equation based wavetable setup.
 *   it is not free configurable.
 *
 *----------------------------------------------------------------------*
 */

class Frequency: public testmethod::TestMethod
{
protected:
  string pins;
  string mode;
  double  datarate;
  int     sample_per_meas; // samplecount
  int     initial_discard;
  int     inter_sample_discard;
  int     exitOnNotFinished;
  double  waitTimeout;
  int     ftstResult;
  string  output;
  string  mTestName;

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */

// TODO replace with tmu own definitions
  virtual void initialize()
  {
    // TODO complete this prameterlist for the testflow
    addParameter("pins",
                 "PinString",
                 &pins)
      .setComment("Pin(s) and/or pin group(s) to be measured");
    addParameter("mode",
                 "string",
                 &mode)
      .setOptions("FREQ+:FREQ-:PER+:PER-:PERALL+:PERALL-")
      .setComment("Measurement mode");
    addParameter("datarate [MBps]",
               "double",
               &datarate)
    .setDefault("0")
    .setComment("max expected datarate of signal to be maesured");
    addParameter("sample_per_meas",
              "int",
              &sample_per_meas)
    .setDefault("2")
    .setComment("number of samples to be taken ");
    addParameter("initial_discard",
              "int",
              &initial_discard)
    .setDefault("0")
    .setComment("number of events to ignore before start sampling");
    addParameter("inter_sample_discard",
                "int",
                &inter_sample_discard)
    .setDefault("100")
    .setComment("number of events to ignore between samples");
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
      .setDefault("meanValue")
      .setComment("test limit's name, default is \"meanValue\"\n"
             "if test table is used, the limit is defined in file\n"
             "if predefined limit is used, the limit name must be as same as default.");
     addLimit("meanValue");
  }
  
  /**
   *This test is invoked per site.
   */
  virtual void run()
  {
      static TmuFrequencyUtil::FrequencyParameter params;
      TmuFrequencyUtil::FrequencyResult results;
  
      ON_FIRST_INVOCATION_BEGIN();
        /*
         * Process all parameters needed in test. Actually this function is
         * called once under multisite because all input parameters should be
         * the same through all sites. The processed parameters are stored into
         * the static variable 'params' and it's refered by the doMeasurement() 
         * for every site.
         */

      TmuFrequencyUtil::processParameters(pins,
                                              mode,
                                              datarate,
                                              sample_per_meas,
                                              initial_discard,
                                              exitOnNotFinished,
                                              waitTimeout,
                                              inter_sample_discard,
                                              ftstResult, 
                                              params);
      ON_FIRST_INVOCATION_END();
  
      /*
       * Execute measurement with the specified 'params' and store results
       * into the 'results'. The multisite handling, i.e. ON_FIRST_INVOCATION
       * block are executed inside this function.
       */
      TmuFrequencyUtil::doMeasurement(params, results);
  
      /*
       * Judge and datalog based on the 'results'. This function contains
       * pre-defined and hard-coded test name (e.g. "minValue"), so if
       * you'd like to use your own test names for judgement and datalogging
       * it's needed to modify this funciton or create new one.
       */
      TmuFrequencyUtil::judgeAndDatalog(mTestName,results, params.ftstResult);
  
      /*
       * Output contents of the 'results' to Report Window if specified by
       * the "output" parameter.
       */
      TmuFrequencyUtil::reportToUI(results, 
                                     output, 
                                     params.isFrequency,
                                     params.ftstResult);

      // free pins memory
      pins = "";
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

REGISTER_TESTMETHOD("TmuTest.Frequency", Frequency);
