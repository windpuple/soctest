 //for testmethod framework interfaces
#include "testmethod.hpp"
//for testmethod API interfaces
#include "mapi.hpp"

#include "OutputDCUtil.hpp"

 /**
  *----------------------------------------------------------------------*
  * @testmethod class: OutputDC
  *
  * @Purpose: measure output level 
  *
  *----------------------------------------------------------------------*
  * @Description:
  *   string pinlist:              {@ | pinlist}
  *     Name of pins to be tested.
  *     valid pin types: O,IO
  *   string mode                  {ProgramLoad|PPMU|SPMU|PPMUTerm|SPMUTerm}
  *     ProgramLoad - use programmable load
  *     PPMU        - use PPMU 
  *     SPMU        - use SPMU 
  *     PPMUTerm    - use PPMU with termination ON
  *     SPMUTerm    - use SPMU with termination ON
  *   string measuredLevel         {LOW,HIGH,BOTH}
  *     LOW         - output low level
  *     HIGH        - output high level
  *     BOTH        - output low and high level
  *   testmethod::SpecValue  forceCurrentLow    {uA}
  *     force current for low levle test, base unit is uA.
  *   testmethod::SpecValue  forceCurrentHigh   {uA}
  *     force current for high levle test, base unit is uA.
  *   testmethod::SpecValue  maxPassLow          {V}
  *     max pass threshold for low level, base unit is V.
  *   testmethod::SpecValue  minPassLow          {V}
  *     min pass threshold for low level, base unit is V.
  *   testmethod::SpecValue  maxPassHigh         {V}
  *     max pass threshold for high level, base unit is V.
  *   testmethod::SpecValue  minPassHigh         {V}
  *     min pass threshold for high level, base unit is V.
  *   testmethod::SpecValue  settlingTimeLow    {ms} 
  *     The settling time before PMU/SPMU measurement for low level, 
  *     base unit is ms. 
  *   testmethod::SpecValue  settlingTimeHigh   {ms} 
  *     The settling time before PMU/SPMU measurement for high level,
  *     base unit is ms. 
  *   testmethod::SpecValue spmuClampVoltageLow_V  {V}
  *     clamp voltage for spmu measurement for low level, base unit is V.
  *   DOUBLE spmuClampVoltageHigh_V {V}
  *     clamp voltage for spmu measurement for high level, base unit is V.
  *   string vectorRange
  *     vector range to be scanned for low or high output.
  *     valid input format:
  *       number
  *       number1,number2
  *       number1-number2    
  *   string output:               {ReportUI | None}
  *     Print message or not. 
  *     ReportUI - for debug mode
  *     None     - for production mode
  *
  * @Note:
  *   1. In case of GPF mode and production test, please use 'ProgramLoad' mode,
  *      and look into OutputDCTest::doMeasurementByActiveLoad() to 
  *      make some modifications there for high throughput.
  *   2. To use multiport burst label with PMU/SPMU method,
  *      please look into OutputDCTest::doMeasurementByPMU()
  *      and make some modifications there.
  *   3. When working in PinScale system and using the "PPMU" mode, ppmuClamp 
  *      by default is set to pass limit low/high each measure level respectively.  
  *----------------------------------------------------------------------*
  */

class OutputDC: public testmethod::TestMethod
{
protected:
  double _results[4];
  string  pinlist;
  string  mode;
  string  measuredLevel;
  testmethod::SpecValue  forceCurrentLow;
  testmethod::SpecValue  forceCurrentHigh;
  testmethod::SpecValue  maxPassLow;
  testmethod::SpecValue  minPassLow;
  testmethod::SpecValue  maxPassHigh;
  testmethod::SpecValue  minPassHigh;
  testmethod::SpecValue  settlingTimeLow;
  testmethod::SpecValue  settlingTimeHigh;
  testmethod::SpecValue  spmuClampVoltageLow;
  testmethod::SpecValue  spmuClampVoltageHigh;
  string  vectorRange;
  string  output;
  string mTestName;
  static string sDefaultTestname_Low;
  static string sDefaultTestname_High;

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
      .setComment("O or IO pins");
    addParameter("mode",
                 "string",
                 &mode)
      .setDefault("ProgramLoad")
      .setOptions("PPMU:SPMU:PPMUTerm:SPMUTerm:ProgramLoad");
    addParameter("measuredLevel",
                 "string",
                 &measuredLevel)
      .setDefault("BOTH")
      .setOptions("LOW:HIGH:BOTH");
    addParameter("forceCurrentLow",
                 "SpecValue",
                 &forceCurrentLow)
      .setDefault("0[uA]")    
      .setComment("force current for low level meas., e.g. 10[uA]");
    addParameter("forceCurrentHigh",
                 "SpecValue",
                 &forceCurrentHigh)
      .setDefault("0[uA]") 
      .setComment("force current for high level meas., e.g. 100[uA]");
    addParameter("maxPassLow",
                 "SpecValue",
                 &maxPassLow)
      .setDefault("0[V]")
      .setVisible(false)
      .setComment("max pass voltage for low level meas., e.g. 1[V]");
    addParameter("minPassLow",
                 "SpecValue",
                 &minPassLow)
      .setDefault("0[V]")
      .setVisible(false)
      .setComment("min pass voltage for low level meas., e.g. 5[V]");
    addParameter("maxPassHigh",
                 "SpecValue",
                 &maxPassHigh)
      .setDefault("0[V]")
      .setVisible(false)
      .setComment("max pass voltage for high level meas., e.g. 1[V]");
    addParameter("minPassHigh",
                 "SpecValue",
                 &minPassHigh)
      .setDefault("0[V]")
      .setVisible(false)
      .setComment("min pass voltage for high level meas., e.g. 5[V]");
    addParameter("settlingTimeLow",
                 "SpecValue",
                 &settlingTimeLow)
      .setDefault("0[ms]")
      .setComment("settling time for low level meas., e.g. 1[ms]");
    addParameter("settlingTimeHigh",
                 "SpecValue",
                 &settlingTimeHigh)
      .setDefault("0[ms]")
      .setComment("settling time for high level meas., e.g. 1[ms]");
    addParameter("spmuClampVoltageLow",
                 "SpecValue",
                 &spmuClampVoltageLow)
      .setDefault("0[V]")
      .setComment("clamp voltage for spmu meas. for low level, e.g. 1[V]");
    addParameter("spmuClampVoltageHigh",
                 "SpecValue",
                 &spmuClampVoltageHigh)
      .setDefault("0[V]")
      .setComment("clamp voltage for spmu meas. for high level, e.g. 5[V]");
    addParameter("vectorRange",
                 "string",
                 &vectorRange)
      .setComment("vector range to be scanned for low or high output.\n"
    	          "valid input format:\n"
                  "1) number\n"
                  "2) number1,number2\n"
                  "3) number1-number2");
    addParameter("testName",
                 "string",
                 &mTestName)
      .setDefault("("+sDefaultTestname_Low+","+sDefaultTestname_High+")")
      .setComment("(1)\"BOTH\" measurement need two test limits' names in pairs, "
         "pair.first is for low level measurement, no unit means 'V' \n"
         "pair.second is for high level measurement , no unit means 'V'\n"
         "(2)\"LOW\" or \"HIGH\" measurement at least need one test limit.\n"
         "if test table is used, the limit name is defined in table\n"
         "if predefined limit is used, the limit name must be as same as default.");
    addParameter("output",
                 "string",
                 &output)
      .setDefault("None")
      .setOptions("ReportUI:None")
      .setComment("Print message on UI report window if 'ReportUI'");

    addLimit(sDefaultTestname_Low);
    addLimit(sDefaultTestname_High);
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
    static OutputDcTest::TestParam param;
    static OutputDcTest::TestLimit testLimit;
    /*
     * For output dc test with programmeable load, all sites' result are stored in 'result' locally, but not in API.
     * here use 'static' direction to sustain these values of variables for all sites.
     */
    static OutputDcTest::TestResult result; 
  
      /*
       * Process all parameters needed in test. Actually this function is
       * called once under multisite because all input parameters should be
       * the same through all sites. The processed parameters are stored into
       * the static variable 'param' for later reference on all sites.
       */     
    ON_FIRST_INVOCATION_BEGIN();  
      OutputDcTest::processParameters(pinlist,
                                      measuredLevel,
                                      forceCurrentLow.getValueAsTargetUnit("uA"),
                                      forceCurrentHigh.getValueAsTargetUnit("uA"),
                                      minPassLow.getValueAsTargetUnit("V"),
                                      maxPassLow.getValueAsTargetUnit("V"),
                                      minPassHigh.getValueAsTargetUnit("V"),
                                      maxPassHigh.getValueAsTargetUnit("V"),
                                      mode,
                                      settlingTimeLow.getValueAsTargetUnit("ms"),
                                      settlingTimeHigh.getValueAsTargetUnit("ms"),
                                      spmuClampVoltageLow.getValueAsTargetUnit("mV"),
                                      spmuClampVoltageHigh.getValueAsTargetUnit("mV"),
                                      vectorRange,
                                      param);

      /*
       * Process the limit needed in test. Actually this function is
       * called once under multisite because all limits should be the
       * same through all sites. The processed limit are stored into the
       * static variable 'limit' for later reference on all sites.
       */
      OutputDcTest::processLimit(param,mTestName,testLimit);

    ON_FIRST_INVOCATION_END();
    
    /*
     * Execute measurement with the specified 'param' and store results 
     * into the 'result' judged with 'limit'. The multisite handling, i.e. 
     * ON_FIRST_INVOCATION block are executed inside this function.
     */
                                       
    if (param.mode == "ProgramLoad")
    {
      OutputDcTest::doMeasurementByProgramLoad(param,result);
    }
    else
    {
      OutputDcTest::doMeasurementByPMU(param,testLimit,result);
    }
  
    /*
     * Judge and datalog based on the 'result'. This function uses
     * testsuite name as test name, so if you'd like to use your own
     * test names for judgement and datalogging it's needed to modify
     * this funciton or create new one.
     */    
    OutputDcTest::judgeAndDatalog(param,testLimit,result,_results);
  
    /*
     * Output contents of the 'result' to Report Window if specified by
     * the "output" parameter.
     */
    OutputDcTest::reportToUI(param,testLimit,output,result);                                  
      
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
    if(parameterIdentifier == "mode")
    {
      //enable spmu clamp only for SPMU related mode.   	
      if (mode == "SPMU" || mode == "SPMUTerm")
      {
        getParameter("spmuClampVoltageLow").setEnabled(true);
        getParameter("spmuClampVoltageHigh").setEnabled(true);
      }
      else
      {
        getParameter("spmuClampVoltageLow").setEnabled(false);
        getParameter("spmuClampVoltageHigh").setEnabled(false); 
      }
      
      //enable settling time for non-progamableLoad mode.
      if (mode != "ProgramLoad")
      {
        getParameter("settlingTimeLow").setEnabled(true);
        getParameter("settlingTimeHigh").setEnabled(true);
      }
      else
      {
        getParameter("settlingTimeLow").setEnabled(false);
        getParameter("settlingTimeHigh").setEnabled(false); 
      } 
    }
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

string OutputDC::sDefaultTestname_Low = "OutputDC_LowLevel[V]";
string OutputDC::sDefaultTestname_High = "OutputDC_HighLevel[V]";

REGISTER_TESTMETHOD("DcTest.OutputDC", OutputDC);
