//for testmethod framework interfaces
#include "testmethod.hpp"
//for testmethod API interfaces
#include "mapi.hpp"

#include "TMU_Commons.hpp"
#include "TmuRiseFallTimeUtil.hpp"

/**
  *----------------------------------------------------------------------*
  * @Testmethod Class: RiseFallTime
  *
  * @Purpose: rise time and fall time measurement with tmu
  *
  *----------------------------------------------------------------------*
  *
  * @Description:
  *   STRING& pins:                 {@ | pin and/or pin group list}
  *     Name of pin(s) and/or pin group(s) to be measured
  *     Valid pins: all digital pins.
  *   LONG datarate              {}
  *      max expected datarate of signal to be maesured
  *   INT     prescaler;            {1...16}
  *   INT sample_per_meas:              {}
  *     # of samples to be taken
  *   double  treshold_a;               {mV}
  *   double  treshold_b;               {mV}
  *     # the treshold values for the shot 1 and shot 2
  *   int     initial_discard;          {}
  *     # number od sample to discard at beginning
  *   int     inter_sample_discard;     {}
  *     # number od events to discard between to samples
  *   int     exitOnNotFinished;
  *     if TRUE  throw exception if not finished
  *   double  waitTimeout;
  *     timeout value for waitTMUfinished
  *   DOUBLE rise_min
  *   DOUBLE rise_max
  *   DOUBLE fall_min
  *   DOUBLE fall_max
  *     limits to generate pass/fail. All samples taken must fit in these
  *     limits to get PASS result
  *   INT ftstResult:                       {0 | 1}
  *     Flag to specify whether taking functional test result into account
  *     for pass/fail judging and datalogging.
  *   
  * @Note:
  *
  *----------------------------------------------------------------------*
  */

class RiseFallTime: public testmethod::TestMethod
{
protected:
  string  pins;
  int     datarate;
  int     prescaler;
  int     samples_per_meas;
  double  treshold_a;
  double  treshold_b;
  int     initial_discard;
  int     inter_sample_discard;
  int     exitOnNotFinished;
  double  waitTimeout;
  double  rise_min;
  double  rise_max;
  double  fall_min;
  double  fall_max;
  int     ftstResult;
  string  output;
  string  mTestNameRise;
  string  mTestNameFall;


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
    addParameter("datarate [Bps]",
               "int",
               &datarate)
    .setDefault("0")
    .setComment("max expected datarate of signal to be maesured");
     addParameter("prescaler",
               "int",
               &prescaler)
    .setDefault("2")
    .setComment("Prescaler configuration 2-16");
    addParameter("samples_per_meas",
              "int",
              &samples_per_meas)
    .setDefault("10")
    .setComment("number of samples to be taken");
    addParameter("treshold_a [V]",
                 "double",
                 &treshold_a)
    .setDefault("0.250")
    .setComment("first shot treshold");
    addParameter("treshold_b [V]",
                 "double",
                 &treshold_b)
    .setDefault("1.250")
    .setComment("second shot treshold");
    addParameter("initial_discard",
              "int",
              &initial_discard)
    .setDefault("0")
    .setComment("number of events to ignore before start sampling");
    addParameter("inter_sample_discard",
                "int",
                &inter_sample_discard)
    .setDefault("0")
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
    addParameter("testName Rise",
                 "string",
                 &mTestNameRise)
      .setDefault("(minRiseValue,maxRiseValue,meanRiseValue)")
      .setComment("test limits' names in pairs, "
         "like \"(min,max,mean,...)\"\n"
         "pair is ordered:\n"
         "  pair.first is for minimum value\n"
         "  pair.second is for maximum value\n"
         "  pair.third is for mean value\n"
         "if test table is used, the limit is defined in file\n"
         "if predefined limit is used, the limit name must be as same as default.");
    addParameter("testName Fall",
                 "string",
                 &mTestNameFall)
      .setDefault("(minFallValue,maxFallValue,meanFallValue)")
      .setComment("test limits' names in pairs, "
         "like \"(min,max,mean,...)\"\n"
         "pair is ordered:\n"
         "  pair.first is for minimum value\n"
         "  pair.second is for maximum value\n"
         "  pair.third is for mean value\n"
         "if test table is used, the limit is defined in file\n"
         "if predefined limit is used, the limit name must be as same as default.");
    addLimit("minFallValue");
    addLimit("maxFallValue");
    addLimit("meanFallValue");
    addLimit("minRiseValue");
    addLimit("maxRiseValue");
    addLimit("meanRiseValue");

  }
  
  /**
   *This test is invoked per site.
   */
  virtual void run()
  {
	  static TmuRiseFallTimeUtil::RiseFallTimeParameter params;
	  TmuRiseFallTimeUtil::RiseFallTimeResult results;
      
      ON_FIRST_INVOCATION_BEGIN();
        /*
         * Process all parameters needed in test. Actually this function is
         * called once under multisite because all input parameters should be
         * the same through all sites. The processed parameters are stored into
         * the static variable 'params' and it's refered by the doMeasurement() 
         * for every site.
         */
      TmuRiseFallTimeUtil::processParameters(pins,
                                              datarate,
                                              prescaler,
                                              samples_per_meas,
                                              treshold_a,
                                              treshold_b,
                                              initial_discard,
                                              inter_sample_discard,
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
      TmuRiseFallTimeUtil::doMeasurement(params, results);
  
      /*
       * Judge and datalog based on the 'results'. This function contains
       * pre-defined and hard-coded test name (e.g. "minValue"), so if
       * you'd like to use your own test names for judgement and datalogging
       * it's needed to modify this funciton or create new one.
       */
      TmuRiseFallTimeUtil::judgeAndDatalog(mTestNameRise,mTestNameFall,results, params.ftstResult);
                                      
      /*
       * Output contents of the 'results' to Report Window if specified by
       * the "output" parameter.
       */
      TmuRiseFallTimeUtil::reportToUI(results, output, params.ftstResult);
  
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

REGISTER_TESTMETHOD("TmuTest.RiseFallTime", RiseFallTime);
