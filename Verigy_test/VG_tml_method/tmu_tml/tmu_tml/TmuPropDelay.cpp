//for testmethod framework interfaces
#include "testmethod.hpp"
//for testmethod API interfaces
#include "mapi.hpp"

#include "TMU_Commons.hpp"
#include "TmuPropDelayUtil.hpp"
/**
 *----------------------------------------------------------------------*
 * @Testmethod Class: PropDelay
 *
 * @Purpose: propagation delay measurement with TMU
 *
 *----------------------------------------------------------------------*
 * @Description:
 *   STRING& pins1:                {@ | pin and/or pin group list}
 *     Name of pin(s) and/or pin group(s) to be measured
 *     Duration from the event for pins1 to the event for pins2 is measured.
 *     The n-th pin in the pins1 and the pins2 make a pair.
 *     Valid pins: all digital pins.
 *   STRING& pins2:                {@ | pin and/or pin group list}
 *     Name of pin(s) and/or pin group(s) to be measured
 *     Duration from the event for pins1 to the event for pins2 is measured.
 *     The n-th pin in the pins1 and the pins2 make a pair.
 *     Valid pins: all digital pins.
 *   STRING& mode:                 {PD++ | PD-- | PD+- | PD-+}
 *     Measurement mode.
 *     PD++: from  rising edge of pins1 to  rising edge of pins2
 *     PD--: from falling edge of pins1 to falling edge of pins2
 *     PD+-: from  rising edge of pins1 to falling edge of pins2
 *     PD-+: from falling edge of pins1 to  rising edge of pins2
  *   LONG testperiod                     {}
  *     # period  of the signal to be maesured
  *   INT sample_per_meas:              {}
  *     # of samples to be taken
  *   int     initial_discard;          {}
  *     # number od sample to discard at beginning
 *   int     exitOnNotFinished;
 *     if TRUE  throw exception if not finished
 *   double  waitTimeout;
 *     timeout value for waitTMUfinished
  *   INT ftstResult:               {0 | 1}
  *     Flag to specify whether taking functional test result into account
  *     for pass/fail judging and datalogging.
 *
 * @Note:
 *
 *----------------------------------------------------------------------*
 */

class PropDelay: public testmethod::TestMethod
{
protected:
  string  drivepins;
  string  receivepins;
  string  mode;
  double  testperiod;
  int     sample_per_meas; // samplecount
  double  treshold_a;
  int     initial_discard;
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
  virtual void initialize()
  {
    addParameter("drivepins",
                 "PinString",
                 &drivepins)
      .setComment("Pin(s) and/or pin group(s) to be measured");
    addParameter("receivepins",
                 "PinString",
                 &receivepins)
      .setComment("Pin(s) and/or pin group(s) to be measured");
    addParameter("mode",
                 "string",
                 &mode)
    .setOptions("PD++:PD--")
    .setDefault("PD++")
    .setComment("Measurement modetype ");
    addParameter("testperiod [ns]",
               "double",
               &testperiod)
    .setDefault("20.0")
    .setComment("the min expected period of the testsignal");
    addParameter("sample_per_meas",
              "int",
               &sample_per_meas)
    .setDefault("10")
    .setComment("number of samples to be taken ");
    addParameter("treshold_a [V]",
               "double",
               &treshold_a)
    .setDefault("0.0")
    .setComment("the treshold [V]");
    addParameter("initial_discard",
               "int",
               &initial_discard)
    .setDefault("0")
    .setComment("number of events to ignore before start sampling");
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
      .setDefault("maxValue")
      .setComment("test limit's name, default is \"maxValue\"\n"
             "if test table is used, the limit is defined in file\n"
             "if predefined limit is used, the limit name must be as same as default.");
    addLimit("maxValue");
  }
  
  /**
   *This test is invoked per site.
   */
  virtual void run()
  {
      static TmuPropDelayUtil::PropDelayParameter params;
      TmuPropDelayUtil::PropDelayResult results;
  
      ON_FIRST_INVOCATION_BEGIN();
        /*
         * Process all parameters needed in test. Actually this function is
         * called once under multisite because all input parameters should be
         * the same through all sites. The processed parameters are stored into
         * the static variable 'params' and it's refered by the doMeasurement() 
         * for every site.
         */
        TmuPropDelayUtil::processParameters(
            drivepins,
            receivepins,
            mode,
            testperiod,
            sample_per_meas,
            treshold_a,
            initial_discard,
            exitOnNotFinished,
            waitTimeout,
            ftstResult,
            params);

      ON_FIRST_INVOCATION_END();
  
      /*
       * Execute measurement with the specified 'params' and store results
       * into the 'results'. The multisite handling, i.e. ON_FIRST_INVOCATION
       * block are executed inside this function.
       */
      TmuPropDelayUtil::doMeasurement(params, results);
  
      /*
       * Judge and datalog based on the 'results'. This function contains
       * pre-defined and hard-coded test name (e.g. "minValue"), so if
       * you'd like to use your own test names for judgement and datalogging
       * it's needed to modify this funciton or create new one.
       */
      TmuPropDelayUtil::judgeAndDatalog(mTestName,results, params.ftstResult);
                                                
      /*
       * Output contents of the 'results' to Report Window if specified by
       * the "output" parameter.
       */
      TmuPropDelayUtil::reportToUI(results, output, params.ftstResult);
  
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

REGISTER_TESTMETHOD("TmuTest.PropDelay", PropDelay);
