//for testmethod framework interfaces
#include "testmethod.hpp"
//for testmethod API interfaces
#include "mapi.hpp"

#include "ProductionIddqUtil.hpp"

/**
 *----------------------------------------------------------------------*
 * @testmethod class: ProductionIddq()
 *
 * @Purpose: production Iddq current
 *----------------------------------------------------------------------*
 * @Description: test method program for production Iddq current 
 *              by utilizing DPS_TASK API
 *
 * @Input Parameters:
 *   string  dpsPins         : {@ | DPS pin}  
 *     DPS pin(s).
 *   string  disconnectPins  : {@ | Digital Pins} 
 *     any pins need to be disconnected(AC relays opened) 
 *     from tester before the measurement.
 *   testmethod::SpecValue  settlingTime     : {ms} 
 *     the time will be in addition to the settling time 
 *     calculated by the system. base unit is ms.
 *     after the settling time, the current is measured.
 *   string  stopMode        : {ToStopVec | ToStopCyc}  
 *     the sequencer to stop at a specific vector or cycle.  
 *   string  strStopVecCycNum:
 *     the specified number where the sequencer has to stop.
 *     if specify more than one number, use commas to separated them.
 *   int  samples              
 *     The samples for DPS measurement, default is 16. 
 *     This must be positve integer.
 *   string  checkFunctional : {ON | OFF} 
 *     the functional test shall contribute to 
 *     the overall test result, 
 *     OFF is for not.
 *   string  gangedMode      : {ON | OFF}
 *     the sum of all Iddq currents of dpsPins will be given back, 
 *     OFF is for each pin's Iddq current.         
 *   string  output          : {NONE | ReportUI} 
 *     Print message or not.
 *     ReportUI - for debug mode
 *     None - for production mode
 * 
 * @Note:
 * 
 *----------------------------------------------------------------------*
 */

class ProductionIddq: public testmethod::TestMethod
{
protected:
  double _results[4];
  string  dpsPins;
  string  disconnectPins;
  testmethod::SpecValue  settlingTime;
  string  stopMode;
  string  strStopVecCycNum;
  int  samples;
  string  checkFunctional;
  string  gangedMode;
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
    addParameter("dpsPins",
                 "PinString",
                 &dpsPins)
      .setDefault("Vee")
      .setComment("dps pins for measurement");
    addParameter("disconnectPins",
                 "PinString",
                 &disconnectPins)
      .setComment("pins disconnected before measurement.");
    addParameter("settlingTime",
                 "SpecValue",
                 &settlingTime)
      .setDefault("0[ms]")
      .setComment("extra settling time, e.g. 1[ms]");
    addParameter("stopMode",
                 "string",
                 &stopMode)
      .setDefault("ToStopVEC")
      .setOptions("ToStopVEC:ToStopCYC")
      .setComment("stop sequencer at vector or cycle");
    addParameter("strStopVecCycNum",
                 "string",
                 &strStopVecCycNum)
      .setComment("the number of vector or cycle \n"
                  "When parameter stopMode select ToStopCYC, support multi-port. \n"
                  "For single port pattern, the format of input value is like:12,15 \n"
                  "For multi-port pattern, the format is like 12,15@portName.\n"
                  "When select ToStopVEC, don't support multi-port \n"
                  "the format of input value like: 12,15");
    addParameter("samples",
                 "int",
                 &samples)
      .setDefault("16")
      .setComment("The number of measurement to be averaged");
    addParameter("checkFunctional",
                 "string",
                 &checkFunctional)
      .setDefault("ON")
      .setOptions("ON:OFF")
      .setComment("check functional result");
    addParameter("gangedMode",
                 "string",
                 &gangedMode)
      .setDefault("OFF")
      .setOptions("ON:OFF")
      .setComment("sum of all Iddq currents of dps pins");
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
      .setOptions("ReportUI:None")
      .setComment("print information on UI window");

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
    static ProductionIddqTest::ProductionIddqTestParam param;
    static ProductionIddqTest::ProductionIddqTestLimit limit;
   
    ProductionIddqTest::ProductionIddqTestResult result;
     
    ON_FIRST_INVOCATION_BEGIN(); 
      /*
       * Process all parameters needed in test. Actually this function is
       * called once under multisite because all input parameters should be
       * the same through all sites. The processed parameters are stored into
       * the static variable 'param' for later reference on all sites.
       */   
      ProductionIddqTest::processParameters(dpsPins,
                                            disconnectPins,
                                            settlingTime.getValueAsTargetUnit("ms"),
                                            stopMode,
                                            strStopVecCycNum,
                                            samples,
                                            checkFunctional,
                                            gangedMode,
                                            param);
      /*
       * Process the limit needed in test. Actually this function is
       * called once under multisite because all limits should be the 
       * same through all sites. The processed limit are stored into the 
       * static variable 'limit' for later reference on all sites.
       */
      ProductionIddqTest::processLimit(mTestName, (mTestName==sDefaultTestname), limit);
    ON_FIRST_INVOCATION_END();
    
     /*
     * Execute measurement with the specified 'param' and store results 
     * into the 'result' judged with 'limit'. The multisite handling, i.e. 
     * ON_FIRST_INVOCATION block are executed inside this function.
     */
    ProductionIddqTest::doMeasurement(param,limit,result);
    /*
     * Judge and datalog based on the 'result'. This function uses
     * testsuite name as test name, so if you'd like to use your own
     * test names for judgement and datalogging it's needed to modify
     * this funciton or create new one.
     */
    ProductionIddqTest::judgeAndDatalog(param,limit,result,_results);
    /*
     * Output contents of the 'result' to Report Window if specified by
     * the "output" parameter.
     */
    ProductionIddqTest::reportToUI(param,limit,result,output);
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

string ProductionIddq::sDefaultTestname = "passCurrLimit_uA";

REGISTER_TESTMETHOD("DcTest.ProductionIddq", ProductionIddq);
