//for testmethod framework interfaces
#include "testmethod.hpp"
//for testmethod API interfaces
#include "mapi.hpp"

#include "HighZUtil.hpp"

/**
 *----------------------------------------------------------------------*
 * @testmethod class: HighZ
 *
 * @Purpose: highZ testing
 *
 *----------------------------------------------------------------------*
 * @Description:
 *   string pinlist:              {@ | pinlist}
 *     Name of pins to be tested
 *     Valid pins: O or IO pins.
 *   testmethod::SpecValue forceVoltage   {mV}
 *     Value of force voltage with unit. The base unit is mV.
 *   testmethod::SpecValue settlingTime           {ms}
 *     Value of settling time with unit. The base unit is ms.
 *   string relaySwitchMode:      {DEFAULT(BBM) | MBB | PARALLEL}
 *     BBM - Break before Make 
 *     MBB - Make before break
 *     PARALLEL - Open AC relay and Close PMU relay in parallel
 *   string output:               {ReportUI | None}
 *     Print message or not. 
 *     ReportUI - for debug mode
 *     None - for production mode
 *
 * @Note:
 *
 *----------------------------------------------------------------------*
 */

class HighZ: public testmethod::TestMethod
{
protected:
  double _results[4];
  string  pinlist;
  testmethod::SpecValue  forceVoltage;
  testmethod::SpecValue  settlingTime;
  string  relaySwitchMode;
  string  output;
  string mTestName;
  static string sDefaultTestname;

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
  virtual void initialize()
  {
    addParameter("result0",
                 "double",
                 &_results[0],
                 testmethod::TM_PARAMETER_OUTPUT)
      .setDefault("0.0");
    addParameter("result1",
                 "double",
                 &_results[1],
                 testmethod::TM_PARAMETER_OUTPUT)
      .setDefault("0.0");
    addParameter("result2",
                 "double",
                 &_results[2],
                 testmethod::TM_PARAMETER_OUTPUT)
      .setDefault("0.0");
    addParameter("result3",
                 "double",
                 &_results[3],
                 testmethod::TM_PARAMETER_OUTPUT)
      .setDefault("0.0");
    addParameter("pinlist",
                 "PinString",
                 &pinlist)
      .setDefault("@");
    addParameter("forceVoltage",
                 "SpecValue",
                 &forceVoltage)
      .setDefault("2500[mV]")
      .setComment("force voltage,e.g. 10[mV]");
    addParameter("settlingTime",
                 "SpecValue",
                 &settlingTime)
      .setDefault("0[ms]")
      .setComment("settling time,e.g. 1[ms]");
    addParameter("relaySwitchMode",
                 "string",
                 &relaySwitchMode)
      .setDefault("DEFAULT(BBM)")
      .setOptions("DEFAULT(BBM):MBB:PARALLEL")
      .setComment("used for PPMU Only.\nonly'PARALLEL' works for PS400 pin.");
    addParameter("testName",
                 "string",
                 &mTestName)
      .setDefault(sDefaultTestname)
      .setComment("test limit's name, default is \""+sDefaultTestname+"\"\n"
         "if test table is used, the limit name is defined in table\n"
         "if predefined limit is used, the limit name must be as same as default.");
    addParameter("output",
                 "string",
                 &output)
      .setDefault("None")
      .setOptions("ReportUI:None");

    addLimit(sDefaultTestname);
  }
  
  /**
   *This test is invoked per site.
   */
  virtual void run()
  {
    /*
     * Assume all sites' parameters and limit are the same,
     * here use 'static' direction to keep the content for later reference on all sites.
     */
    static HighZTest::HighZTestParam param;
    static HighZTest::HighZTestLimit limit;
    
    HighZTest::HighZTestResult result;
    
    ON_FIRST_INVOCATION_BEGIN();
      /*
       * Process all parameters needed in test. Actually this function is
       * called once under multisite because all input parameters should be
       * the same through all sites. The processed parameters are stored into
       * the static variable 'param' for later reference on all sites.
       */   
      HighZTest::processParameters(pinlist,
                                   forceVoltage.getValueAsTargetUnit("mV"),
                                   settlingTime.getValueAsTargetUnit("ms"),
                                   relaySwitchMode,
                                   param);
      
      /*
       * Process the limit needed in test. Actually this function is
       * called once under multisite because all limits should be the 
       * same through all sites. The processed limit are stored into the 
       * static variable 'limit' for later reference on all sites.
       */
      HighZTest::processLimit(mTestName, (mTestName == sDefaultTestname), limit);
    ON_FIRST_INVOCATION_END();
    
    /*
     * Execute measurement with the specified 'param' and store results 
     * into the 'result' judged with 'limit'. The multisite handling, i.e. 
     * ON_FIRST_INVOCATION block are executed inside this function.
     */
    HighZTest::doMeasurement(param, limit, result);
    
    /*
     * Judge and datalog based on the 'result'. This function uses
     * testsuite name as test name, so if you'd like to use your own
     * test names for judgement and datalogging it's needed to modify
     * this funciton or create new one.
     */
    HighZTest::judgeAndDatalog(param, limit, result,_results);
   
    /*
     * Output contents of the 'result' to Report Window if specified by
     * the "output" parameter.
     */
    HighZTest::reportToUI(param, limit, result,output);
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

string HighZ::sDefaultTestname = "passCurrent_uA";

REGISTER_TESTMETHOD("DcTest.HighZ", HighZ);
