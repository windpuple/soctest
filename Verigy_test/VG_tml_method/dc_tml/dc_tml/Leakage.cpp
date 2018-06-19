//for testmethod framework interfaces
#include "testmethod.hpp"
//for testmethod API interfaces
#include "mapi.hpp"

#include "LeakageUtil.hpp"
/**
 *----------------------------------------------------------------------*
 * @testmethod class: Leakage
 *
 * @Purpose: leakage testing
 *
 *----------------------------------------------------------------------*
 * @Description:
 *   string pinlist:              {@ | pinlist}
 *     Name of pins to be tested
 *     Valid pins: all digital pins.
 *   string measure:              {LOW | HIGH | BOTH}
 *     Measure side
 *   testmethod::SpecValue forceVoltageLow:       {mV}
 *     Value of force voltage (for low level test) with unit. The base
 *     unit is mV.
 *   testmethod::SpecValue forceVoltageHigh:      {mV}
 *     Value of force voltage (for high level test) with unit. The base
 *     unit is mV.
 *   testmethod::SpecValue spmuClampCurrentLow:   {uA}
 *     Value of clamp current (low level test, SPMU only) with unit. The
 *     base unit is uA.
 *   testmethod::SpecValue spmuClampCurrentHigh:  {uA}
 *     Value of clamp current (high level test, SPMU only) with unit. The
 *     base unit is uA.
 *   string ppmuPreCharge:        {ON | OFF}
 *     Enable precharging or not, only PPMU
 *   testmethod::SpecValue prechargeVoltageLow:   {mV}
 *     Value of precharging voltage (low level test, only PPMU) with unit.
 *     The base unit is mV.
 *   testmethod::SpecValue prechargeVoltageHigh:  {mV}
 *     Value of precharging voltage (high level test, only PPMU) with unit.
 *     The base unit is mV.
 *   testmethod::SpecValue settlingTimeLow:       {ms}
 *     Value of settling time (low level test) with unit. The base unit
 *     is ms.
 *   testmethod::SpecValue settlingTimeHigh:      {ms}
 *     Value of settling time (high level test) with unit. The base unit
 *     is ms.
 *   string preFunction:          {NO | ALL | ToStopVEC | ToStopCYC}
 *     Select mode of executing the functional pre-test
 *   int stopCycVecLow:
 *     Units of stop cycle/vector (low level test)
 *   int stopCycVecHigh:
 *     Units of stop cycle/vector (high level test)
 *   string measureMode:          {PPMUpar | PPMUser | SPMUser}
 *     PPMUpar - PPMU parallel
 *     PPMUser - PPMU serial
 *     SPMUser - SPMU serial
 *   string relaySwitchMode:      {DEFAULT(BBM) | MBB | PARALLEL}
 *     BBM - Break before Make 
 *     MBB - Make before break
 *   string output:               {ReportUI | None}
 *     Print message or not. 
 *     ReportUI - for debug mode
 *     None - for production mode
 *   
 * @Note:
 *
 *----------------------------------------------------------------------*
 */

class Leakage: public testmethod::TestMethod
{
protected:
  double _results[4];
  string  pinlist;
  string  measure;
  testmethod::SpecValue  forceVoltageLow;
  testmethod::SpecValue  forceVoltageHigh;
  testmethod::SpecValue  spmuClampCurrentLow;
  testmethod::SpecValue  spmuClampCurrentHigh;
  string  ppmuPreCharge;
  testmethod::SpecValue  prechargeVoltageLow;
  testmethod::SpecValue  prechargeVoltageHigh;
  testmethod::SpecValue  settlingTimeLow;
  testmethod::SpecValue  settlingTimeHigh;
  string  preFunction;
  int  stopCycVecLow;
  int  stopCycVecHigh;
  string  measureMode;
  string  relaySwitchMode;
  string  output;
  string  mTestName;
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
      .setDefault("@");
    addParameter("measure",
                 "string",
                 &measure)
      .setDefault("BOTH")
      .setOptions("LOW:HIGH:BOTH")
      .setComment("LOW: low level test\n"
                  "HIGH: high level test\n"
                  "BOTH: low and high level test");
    addParameter("measureMode",
                 "string",
                 &measureMode)
      .setDefault("PPMUpar")
      .setOptions("PPMUpar:PPMUser:SPMUser");
    addParameter("relaySwitchMode",
                 "string",
                 &relaySwitchMode)
      .setDefault("DEFAULT(BBM)")
      .setOptions("DEFAULT(BBM):MBB:PARALLEL")
      .setComment("used for PPMU Only.\nonly 'PARALLEL' works for PS400 pin.");    
    addParameter("forceVoltageLow",
                 "SpecValue",
                 &forceVoltageLow)
      .setDefault("400[mV]")
      .setComment("force voltage for low measurement,e.g. 400[mV]");
    addParameter("forceVoltageHigh",
                 "SpecValue",
                 &forceVoltageHigh)
      .setDefault("3800[mV]")
      .setComment("force voltage for high measurement, e.g. 3800[mV]");
    addParameter("spmuClampCurrentLow",
                 "SpecValue",
                 &spmuClampCurrentLow)
      .setDefault("0[uA]")
      .setComment("SPMU current clamp for low measurement, e.g. 0[uA]");
    addParameter("spmuClampCurrentHigh",
                 "SpecValue",
                 &spmuClampCurrentHigh)
      .setDefault("0[uA]")
      .setComment("SPMU current clamp for high measurement, e.g. 10[uA]");
    addParameter("ppmuPreCharge",
                 "string",
                 &ppmuPreCharge)
      .setDefault("ON")
      .setOptions("ON:OFF");
    addParameter("prechargeVoltageLow",
                 "SpecValue",
                 &prechargeVoltageLow)
      .setDefault("0[mV]")
      .setComment("precharge PPMU for low measurement, e.g. 0[mV]");
    addParameter("prechargeVoltageHigh",
                 "SpecValue",
                 &prechargeVoltageHigh)
      .setDefault("0[mV]")
      .setComment("precharge PPMU for high measurement, e.g. 0[mV]");
    addParameter("settlingTimeLow",
                 "SpecValue",
                 &settlingTimeLow)
      .setDefault("0[ms]")
      .setComment("settling time for low measurement, e.g. 1[ms]");
    addParameter("settlingTimeHigh",
                 "SpecValue",
                 &settlingTimeHigh)
      .setDefault("0[ms]")
      .setComment("settling time for hgih measurement, e.g. 1[ms]");
    addParameter("preFunction",
                 "string",
                 &preFunction)
      .setDefault("NO")
      .setOptions("NO:ALL:ToStopVEC:ToStopCYC");
    addParameter("stopCycVecLow",
                 "int",
                 &stopCycVecLow);
    addParameter("stopCycVecHigh",
                 "int",
                 &stopCycVecHigh);
    addParameter("testName",
                 "string",
                 &mTestName)
      .setDefault("("+sDefaultTestname_Low+","+sDefaultTestname_High+")")
      .setComment("(1)\"BOTH\" measurement need two test limits' names in pairs, "
         "pair.first is for low level measurement, no unit means 'uA' \n"
         "pair.second is for high level measurement , no unit means 'uA'\n"
         "(2)\"LOW\" or \"HIGH\" measurement at least need one test limit.\n"
         "if test table is used, the limit name is defined in table\n"
         "if predefined limit is used, the limit name must be as same as default.");

    addParameter("output",
                 "string",
                 &output)
      .setDefault("None")
      .setOptions("ReportUI:None");
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
    static LeakageTest::LeakageTestParam param;
    static LeakageTest::LeakageTestLimit limit;
    
    LeakageTest::LeakageTestResult result;
     
    ON_FIRST_INVOCATION_BEGIN();
     /*
      * Process all parameters needed in test. Actually this function is
      * called once under multisite because all input parameters should be
      * the same through all sites. The processed parameters are stored into
      * the static variable 'param' for later reference on all sites.
      */    
      LeakageTest::processParameters(pinlist,
                                     measure,
                                     forceVoltageLow.getValueAsTargetUnit("mV"),
                                     forceVoltageHigh.getValueAsTargetUnit("mV"),
                                     spmuClampCurrentLow.getValueAsTargetUnit("uA"),
                                     spmuClampCurrentHigh.getValueAsTargetUnit("uA"),
                                     ppmuPreCharge,
                                     prechargeVoltageLow.getValueAsTargetUnit("mV"),
                                     prechargeVoltageHigh.getValueAsTargetUnit("mV"),
                                     settlingTimeLow.getValueAsTargetUnit("ms"),
                                     settlingTimeHigh.getValueAsTargetUnit("ms"),
                                     preFunction,
                                     stopCycVecLow,
                                     stopCycVecHigh,
                                     measureMode,
                                     relaySwitchMode,
                                     param);
                                     
      /*
       * Process the limit needed in test. Actually this function is
       * called once under multisite because all limits should be the 
       * same through all sites. The processed limit are stored into the 
       * static variable 'limit' for later reference on all sites.
       */
      LeakageTest::processLimit(param,mTestName,limit);
      
    ON_FIRST_INVOCATION_END();
    /*
     * Execute measurement with the specified 'param' and store results 
     * into the 'result' judged with 'limit'. The multisite handling, i.e. 
     * ON_FIRST_INVOCATION block are executed inside this function.
     */
    LeakageTest::doMeasurement(param,limit,result);
    
    /*
     * Judge and datalog based on the 'result'. This function uses
     * testsuite name as test name, so if you'd like to use your own
     * test names for judgement and datalogging it's needed to modify
     * this funciton or create new one.
     */
    LeakageTest::judgeAndDatalog(param,limit,result,_results);
    
    /*
     * Output contents of the 'result' to Report Window if specified by
     * the "output" parameter.
     */
    LeakageTest::reportToUI(param,limit,result,output);
    
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
    if (parameterIdentifier == "measureMode")
    {
      //exclusively enable PMU and SPMU related parameters.
      bool toEnable = (measureMode != "SPMUser");
      getParameter("ppmuPreCharge").setEnabled(toEnable);	  
      getParameter("prechargeVoltageLow").setEnabled(toEnable);
      getParameter("prechargeVoltageHigh").setEnabled(toEnable);
      getParameter("relaySwitchMode").setEnabled(toEnable);	  
      getParameter("spmuClampCurrentLow").setEnabled(!toEnable);
      getParameter("spmuClampCurrentHigh").setEnabled(!toEnable);	  
    }
    else if (parameterIdentifier == "ppmuPreCharge")
    {
      bool toEnable = (ppmuPreCharge == "ON");
      getParameter("prechargeVoltageLow").setEnabled(toEnable);
      getParameter("prechargeVoltageHigh").setEnabled(toEnable);
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
    
string Leakage::sDefaultTestname_Low = "passCurrentLow_uA"; 
string Leakage::sDefaultTestname_High = "passCurrentHigh_uA"; 

REGISTER_TESTMETHOD("DcTest.Leakage", Leakage);
