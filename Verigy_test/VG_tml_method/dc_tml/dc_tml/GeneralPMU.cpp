//for testmethod framework interfaces
#include "testmethod.hpp"
//for testmethod API interfaces
#include "mapi.hpp"

#include "GeneralPMUUtil.hpp"
/**
 *----------------------------------------------------------------------*
 * @testmethod class: GeneralPMU
 *
 * @Purpose: general PMU testing
 *
 *----------------------------------------------------------------------*
 * @Description:
 *   string pinlist:              {@ | pinlist}
 *     Name of pins to be tested
 *     Valid pins: all digital pins.
 *   string forceMode:            {VOLT | CURR}
 *     VOLT - VFIM
 *     CURR - IFVM
 *   testmethod::SpecValue forceValue             {mV | uA}
 *     Value of force voltage (for VFIM test), base unit is mV.
 *     Value of force current (for IFVM test), base unit is uA.
 *   testmethod::SpecValue spmuClamp              {uA | mV}
 *     Value of clamp current (VFIM test, SPMU only), base unit is uA.
 *     Value of clamp voltage (IFVM test, SPMU only), base unit is mV.
 *   string precharge             {ON | OFF}
 *     Enable precharging or not
 *   testmethod::SpecValue prechargeVoltage       {mV}
 *     Value of precharging voltage with unit, base unit is mV.
 *   testmethod::SpecValue settlingTime           {ms}
 *     Value of settling time with unit, base unit is ms.
 *   string testerState           {CONNECTED | DISCONNECTED | UNCHANGED}
 *     Tester state for measurement
 *   string termination           {ON | OFF}
 *     Select if the test is performed with or without termination
 *   string measureMode:          {PPMUpar | PPMUser | SPMUser}
 *     PPMUpar - PPMU parallel
 *     PPMUser - PPMU serial
 *     SPMUser - SPMU serial
 *   string relaySwitchMode:      {DEFAULT(BBM) | MBB | PARALLEL}
 *     In case of no termination:
 *     BBM - Break before Make: Open AC relay, then Close PMU relay 
 *     MBB - Make before break: Close PMU relay, then Open AC relay
 *     PARALLEL: Open AC relay and Close PMU relay in parallel
 *   testmethod::SpecValue ppmuClampLow           {V}
 *     Value of ppmu clamp low (PinScale, PPMUpar Only), base unit is V.
 *   testmethod::SpecValue ppmuClampHigh          {V}
 *     Value of ppmu clamp high (PinScale, PPMUpar Only), base unit is V.    
 *   string output:               {ReportUI | None}
 *     Print message or not. 
 *     ReportUI - for debug mode
 *     None - for production mode
 *
 * @Note:
 *   This testmethod provides two ppmu clamp parameters for PinScale
 * parallel test. If execute a parallel ppmu test on PinScale, clamps
 * would be set as input value. If input value is 0 (default), clamp 
 * value would be set as limit high/low +/- 0.5 V.
 *----------------------------------------------------------------------*
 */

class GeneralPMU: public testmethod::TestMethod
{
protected:
  double _results[4];
  string  pinlist;
  string  forceMode;
  testmethod::SpecValue  forceValue;
  testmethod::SpecValue  spmuClamp;
  string  precharge;
  testmethod::SpecValue  prechargeVoltage;
  testmethod::SpecValue  settlingTime;
  string  testerState;
  string  termination;
  string  measureMode;
  string  relaySwitchMode;
  testmethod::SpecValue  ppmuClampLow;
  testmethod::SpecValue  ppmuClampHigh;
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
                 &pinlist);
    addParameter("forceMode",
                 "string",
                 &forceMode)
      .setDefault("VOLT")
      .setOptions("VOLT:CURR")
      .setComment("VOLT: force voltage\nCURR: force current");
    addParameter("forceValue",
                 "SpecValue",
                 &forceValue)
      .setDefault("3800[mV]")
      .setComment("force value for current or voltage");
    addParameter("measureMode",
                 "string",
                 &measureMode)
      .setDefault("PPMUpar")
      .setOptions("PPMUpar:PPMUser:SPMUser");
    addParameter("spmuClamp",
                 "SpecValue",
                 &spmuClamp)
      .setDefault("0[mV]")
      .setComment("spmu clamp value for current or voltage");
    addParameter("precharge",
                 "string",
                 &precharge)
      .setDefault("OFF")
      .setOptions("ON:OFF");
    addParameter("prechargeVoltage",
                 "SpecValue",
                 &prechargeVoltage)
      .setDefault("0[mV]")
      .setComment("effective for PPMU Only");
    addParameter("relaySwitchMode",
                 "string",
                 &relaySwitchMode)
      .setDefault("DEFAULT(BBM)")
      .setOptions("DEFAULT(BBM):MBB:PARALLEL")
      .setComment("used for PPMU Only.\nonly 'PARALLEL' with unterminated works for PS400 pin.");
    addParameter("ppmuClampLow",
                 "SpecValue",
                 &ppmuClampLow)
      .setDefault("0[V]")
      .setComment("for PinScale and PPMUpar only");
    addParameter("ppmuClampHigh",
                 "SpecValue",
                 &ppmuClampHigh)
      .setDefault("0[V]")
      .setComment("for PinScale and PPMUpar only");
    addParameter("settlingTime",
                 "SpecValue",
                 &settlingTime)
      .setDefault("0[ms]")
      .setComment("settling time before measurement");
    addParameter("testerState",
                 "string",
                 &testerState)
      .setDefault("CONNECTED")
      .setOptions("CONNECTED:DISCONNECTED:UNCHANGED");
    addParameter("termination",
                 "string",
                 &termination)
      .setDefault("OFF")
      .setOptions("ON:OFF");    
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
    static GeneralPMUTest::GeneralPMUTestParam param;
    static GeneralPMUTest::GeneralPMUTestLimit limit;
    
    GeneralPMUTest::GeneralPMUTestResult result;
    
    ON_FIRST_INVOCATION_BEGIN();
      /*
       * Process all parameters needed in test. Actually this function is
       * called once under multisite because all input parameters should be
       * the same through all sites. The processed parameters are stored into
       * the static variable 'param' for later reference on all sites.
       */
      string fvTargetUnit = (forceMode=="VOLT"?"mV":"uA");
      string scTargetUnit = (forceMode=="VOLT"?"uA":"mV");
      GeneralPMUTest::processParameters(pinlist,
                                        forceMode,
                                        forceValue.getValueAsTargetUnit(fvTargetUnit),
                                        spmuClamp.getValueAsTargetUnit(scTargetUnit),
                                        precharge,
                                        prechargeVoltage.getValueAsTargetUnit("mV"),
                                        settlingTime.getValueAsTargetUnit("ms"),
                                        testerState,
                                        termination,
                                        measureMode,
                                        relaySwitchMode,
                                        ppmuClampLow.getValueAsTargetUnit("V"),
                                        ppmuClampHigh.getValueAsTargetUnit("V"),
                                        param);
      
      /*
       * Process the limit needed in test. Actually this function is
       * called once under multisite because all limits should be the 
       * same through all sites. The processed limit are stored into the 
       * static variable 'limit' for later reference on all sites.
       */
      GeneralPMUTest::processLimit(param,mTestName,(mTestName == sDefaultTestname),limit);
      
    ON_FIRST_INVOCATION_END();
     
    /*
     * Execute measurement with the specified 'param' and store results 
     * into the 'result' judged with 'limit'. The multisite handling, i.e. 
     * ON_FIRST_INVOCATION block are executed inside this function.
     */
    GeneralPMUTest::doMeasurement(param,limit,result);
    
    /*
     * Judge and datalog based on the 'result'. This function uses
     * testsuite name as test name, so if you'd like to use your own
     * test names for judgement and datalogging it's needed to modify
     * this funciton or create new one.
     */
    GeneralPMUTest::judgeAndDatalog(param,limit,result,_results);
    
    /*
     * Output contents of the 'result' to Report Window if specified by
     * the "output" parameter.
     */
    GeneralPMUTest::reportToUI(param,limit,result,output);
  
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
      bool toEnable = (measureMode != "SPMUser");
      getParameter("relaySwitchMode").setEnabled(toEnable);
      getParameter("ppmuClampLow").setEnabled(toEnable);
      getParameter("ppmuClampHigh").setEnabled(toEnable);
      getParameter("spmuClamp").setEnabled(!toEnable);   	  
    }
    else if(parameterIdentifier == "forceMode")
    {
      if(CommonUtil::trim(forceMode) == "VOLT")
      {     	  
        forceValue.setBaseUnit("mV");        
        spmuClamp.setBaseUnit("uA");
      }
      else
      {
        forceValue.setBaseUnit("uA");
        spmuClamp.setBaseUnit("mV");
      }
    }
    else if (parameterIdentifier == "precharge")
    {
      getParameter("prechargeVoltage").setEnabled(precharge == "ON");
    }
    return ;
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

string GeneralPMU::sDefaultTestname = "passLimit_uA_mV";

REGISTER_TESTMETHOD("DcTest.GeneralPMU", GeneralPMU);
