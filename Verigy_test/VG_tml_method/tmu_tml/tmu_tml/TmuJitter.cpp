//for testmethod framework interfaces
#include "testmethod.hpp"
//for testmethod framework interfaces
#include "mapi.hpp"

#include "TMU_Commons.hpp"
#include "TmuJitterUtil.hpp"
/**
 *----------------------------------------------------------------------*
 * @Testmethod Class: Jitter
 *
 * @Purpose: jitter measurement with TMU
 *
 *----------------------------------------------------------------------*
 *
 * @Description:
  *   STRING& pins:                 {@ | pin and/or pin group list}
  *     Name of pin(s) and/or pin group(s) to be measured
  *     Valid pins: all digital pins.
  *   double datarate                     {}
  *     # max expected datarate of signal to be maesured
  *     # if omitted prescaler are used
  *   INT     prescaler;            {1...16}
  *     #
  *   STRING& signaltype            {CLK | DATA}
  *     # the signal type
  *     # wavetable equation based setup
  *   INT sample_per_meas:              {}
  *     # of samples to be taken
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

class Jitter: public testmethod::TestMethod
{
protected:
  string  pins;
  double  datarate;
  double  mUI_ns;
  int     prescaler;
  int     sample_per_meas; // samplecount
  int     exitOnNotFinished;
  double  waitTimeout;
  int     ftstResult;
  int     mHistogram;
  string  output;
  string  mTestName;
  string  mMode;

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
    addParameter("datarate [MBps]",
               "double",
               &datarate)
    .setDefault("0")
    .setComment("max expected datarate of signal to be maesured");
    addParameter("UI [ns]",
               "double",
               &mUI_ns)
    .setDefault("0")
    .setComment("UI of the signal");
    addParameter("prescaler",
               "int",
               &prescaler)
    .setDefault("0")
    .setComment("Prescaler configuration");
    addParameter("jitter mode",
                 "string",
                 &mMode)
      .setDefault("CLK")
      .setOptions("CLK:DATA:PERIOD:DATA_PERIOD");
    addParameter("sample_per_meas",
              "int",
              &sample_per_meas)
    .setDefault("1000")
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
    addParameter("ftstResult",
               "int",
               &ftstResult)
    .setDefault("1")
    .setOptions("0:1")
    .setComment("Check p/f result of functional test?");
    addParameter("histogram",
               "int",
               &mHistogram)
    .setDefault("1")
    .setOptions("0:1")
    .setComment("Check p/f result of functional test?");
    addParameter("output",
                 "string",
                 &output)
      .setDefault("None")
      .setOptions("None:ReportUI:ReportUI_nosamples")
      .setComment("Print results to UI report window");
    addParameter("testName",
                  "string",
                  &mTestName)
      .setDefault("rmsJitter")
      .setComment("test limit's name, default is \"rmsJitter\"\n"
             "if test table is used, the limit is defined in file\n"
             "if predefined limit is used, the limit name must be as same as default.");
    addLimit("rmsJitter");
    addLimit("p2pJitter");
    addLimit("stddevJitter");
    addLimit("maxJitter");
    addLimit("minJitter");
  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
  {
      static TmuJitterUtil::JitterParameter params;
      TmuJitterUtil::JitterResult results;
  
      ON_FIRST_INVOCATION_BEGIN();
        /*
         * Process all parameters needed in test. Actually this function is
         * called once under multisite because all input parameters should be
         * the same through all sites. The processed parameters are stored into
         * the static variable 'params' and it's refered by the doMeasurement() 
         * for every site.
         */
      TmuJitterUtil::processParameters(pins,
									   mMode,
                                       datarate,
                                       mUI_ns,
                                       prescaler,
                                       sample_per_meas,
                                       exitOnNotFinished,
                                       waitTimeout,
                                       ftstResult,
                                       (mHistogram == 1)?TRUE:FALSE,
                                       params);
      ON_FIRST_INVOCATION_END();
  
      /*
       * Execute measurement with the specified 'params' and store results
       * into the 'results'. The multisite handling, i.e. ON_FIRST_INVOCATION
       * block are executed inside this function.
       */
      TmuJitterUtil::doMeasurement(params, results);
  
      /*
       * Judge and datalog based on the 'results'. This function contains
       * pre-defined and hard-coded test name (e.g. "rmsJitter"), so if
       * you'd like to use your own test names for judgement and datalogging
       * it's needed to modify this funciton or create new one.
       */
      TmuJitterUtil::judgeAndDatalog(mTestName,results,
                                       params.ftstResult);
                             
      /*
       * Output contents of the 'results' to Report Window if specified by
       * the "output" parameter.
       */
      TmuJitterUtil::reportToUI(results,
                                  output,
                                  params.ftstResult);
  
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

REGISTER_TESTMETHOD("TmuTest.Jitter", Jitter);
