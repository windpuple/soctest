//for testmethod framework interfaces
#include "testmethod.hpp"
//for testmethod API interfaces
#include "mapi.hpp"

#include "ContinuityUtil.hpp"
 /**
  *----------------------------------------------------------------------*
  * @testmethod class: Continuity
  *
  * @Purpose: It checks for continuity of the test signal paths,
  *          and for short circuits.
  *          It can be done by PPMU or Programmable load (active load)
  *          methods.
  *
  *----------------------------------------------------------------------*
  * @Description:
  *   string pinlist:              {@ | pinlist}
  *     Name of pins to be tested.
  *     valid pin types: I,O,IO
  *   testmethod::SpecValue  testCurrent           {uA}
  *     Value of test current with unit. The base unit is uA.
  *   testmethod::SpecValue  settlingTime          {ms} 
  *     The settling time will be in addition to the settling time 
  *     calculated by the system. The base unit is ms.
  *   string measurementMode       {PPMUpar | ProgLoad}
  *     Two measurement methods:
  *     PPMUpar  - use PPMU to measure in parallel.
  *     ProgLoad - use programmable load to measure.
  *   string polarity              {SPOL | BPOL}
  *     Measurement polarities:
  *     SPOL - single polarity.
  *     BPOL - double polarities.
  *   string prechargeToZeroVol    {ON | OFF}
  *     Force 0 V as the precharge voltage before the current forcing.
  *     (available for PPMU ONLY)
  *     ON  - enable precharge.
  *     OFF - disable precharge. 
  *   string output:               {ReportUI | None}
  *     Print message or not. 
  *     ReportUI - for debug mode
  *     None     - for production mode
  *
  * @Note:
  *     
  *
  *----------------------------------------------------------------------*
  */

class Continuity: public testmethod::TestMethod
{
protected:
  double _results[4];
  string  pinlist;
  testmethod::SpecValue  testCurrent;
  testmethod::SpecValue  settlingTime;
  string  measurementMode;
  string  polarity;
  string  prechargeToZeroVol;
  string  output;
  string  mTestName;
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
      .setDefault("@")
      .setComment("names of pins to be tested");
    addParameter("testCurrent",
                 "SpecValue",
                 &testCurrent)
      .setDefault("10[uA]")
      .setComment("value of test current");
    addParameter("settlingTime",
                 "SpecValue",
                 &settlingTime)
      .setDefault("1[ms]")
      .setComment("extra settling time to system.");
    addParameter("measurementMode",
                 "string",
                 &measurementMode)
      .setDefault("PPMUpar")
      .setOptions("PPMUpar:ProgLoad")
      .setComment("parallel PPMU or programmable load");
    addParameter("polarity",
                 "string",
                 &polarity)
      .setDefault("SPOL")
      .setOptions("SPOL:BPOL")
      .setComment("single polarity or both polarities.");
    addParameter("prechargeToZeroVol",
                 "string",
                 &prechargeToZeroVol)
      .setDefault("ON")
      .setOptions("ON:OFF")
      .setVisible(false) // this precharge is obsolete with single density
      .setComment("precharge to 0 V before forcing current.");
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
      .setOptions("None:ReportUI")
      .setComment("print out information on UI window");
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
    static ContinuityTest::ContinuityTestParam param;
    static ContinuityTest::ContinuityTestLimit limit;
    
    /*
     * For ContinuityTest with programmeable load, 
     * all sites' result are stored in 'result' locally, but not in API.
     * Here use 'static' direction to sustain these values of variables for all sites.
     */
    static ContinuityTest::ContinuityTestResult result;
   
    ON_FIRST_INVOCATION_BEGIN();
      /*
       * Process all parameters needed in test. Actually this function is
       * called once under multisite because all input parameters should be
       * the same through all sites. The processed parameters are stored into
       * the static variable 'param' for later reference on all sites.
       */ 
      ContinuityTest::processParameters(pinlist, 
                                        testCurrent.getValueAsTargetUnit("uA"),
                                        settlingTime.getValueAsTargetUnit("ms"),
                                        measurementMode,
                                        polarity,
                                        param);
      /*
       * Process the limit needed in test. Actually this function is
       * called once under multisite because all limits should be the 
       * same through all sites. The processed limit are stored into the 
       * static variable 'limit' for later reference on all sites.
       */
      ContinuityTest::processLimit(param, mTestName, (mTestName==sDefaultTestname), limit);
    ON_FIRST_INVOCATION_END();
    
    /*
     * Execute measurement with the specified 'param' and store results 
     * into the 'result' judged with 'limit'. The multisite handling, i.e. 
     * ON_FIRST_INVOCATION block are executed inside this function.
     */
    ContinuityTest::doMeasurement(param, limit, result);
    
    /*
     * Judge and datalog based on the 'result'. This function uses
     * testsuite name as test name, so if you'd like to use your own
     * test names for judgement and datalogging it's needed to modify
     * this funciton or create new one.
     */
    ContinuityTest::judgeAndDatalog(param,limit, result,_results);
    
    /*
     * Output contents of the 'result' to Report Window if specified by
     * the "output" parameter.
     */
    ContinuityTest::reportToUI(param, limit, result, output);
  
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

string Continuity::sDefaultTestname = "passVolt_mV";

REGISTER_TESTMETHOD("DcTest.Continuity", Continuity);
