//for testmethod framework interfaces
#include "testmethod.hpp"
//for testmethod API interfaces
#include "mapi.hpp"

#include "StandbyCurrentUtil.hpp"

/**
 *----------------------------------------------------------------------*
 * @testmethod class: StandbyCurrent()
 *
 * @Purpose: standby current
 *
 *----------------------------------------------------------------------*
 * @Description: test method program for standby current 
 *              by utilizing DPS_TASK API
 *  
 * @Input Parameters:
 *   string  dpsPins      : {@ | DPS pin}  
 *     DPS pin(s).
 *   int  samples              
 *     The samples for DPS  measurement. default is 16. 
 *     This must be positve integer.
 *   testmethod::SpecValue  settlingTime     : {ms}
 *     specifies a wait time which is applied right before the 
 *     measurement(after any necessary range-switching and/or
 *     unganging of channels). 
 *     base unit is ms.
 *   string  termination  : {ON | OFF}
 *     "ON"  for measurement with termination
 *     "OFF" for no termination
 *   string  output       : {NONE | ReportUI} 
 *     Print message or not.
 *     ReportUI - for debug mode
 *     None - for production mode
 *
 * @Note:
 * 
 *----------------------------------------------------------------------*
 */

class StandbyCurrent: public testmethod::TestMethod
{
protected:
  double _results[4];
  string  dpsPins;
  int  samples;
  testmethod::SpecValue  settlingTime;
  string  termination;
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
      .setDefault("@");
    addParameter("samples",
                 "int",
                 &samples)
      .setDefault("16")
      .setComment("samples to dps measurement");
    addParameter("settlingTime",
                 "SpecValue",
                 &settlingTime)
      .setDefault("0[ms]")
      .setComment("extra settling time, e.g. 1[ms]");
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
    static StandbyCurrentTest::StandbyCurrentTestParam param;
    static StandbyCurrentTest::StandbyCurrentTestLimit limit;
    
    StandbyCurrentTest::StandbyCurrentTestResult result;
    
    ON_FIRST_INVOCATION_BEGIN(); 
      /*
       * Process all parameters needed in test. Actually this function is
       * called once under multisite because all input parameters should be
       * the same through all sites. The processed parameters are stored into
       * the static variable 'param' for later reference on all sites.
       */   
      StandbyCurrentTest::processParameters(dpsPins,
                                            samples,
                                            settlingTime.getValueAsTargetUnit("ms"),
                                            termination,
                                            param);
      
      /*
       * Process the limit needed in test. Actually this function is
       * called once under multisite because all limits should be the 
       * same through all sites. The processed limit are stored into the 
       * static variable 'limit' for later reference on all sites.
       */
      StandbyCurrentTest::processLimit(mTestName, (mTestName == sDefaultTestname), limit);
    ON_FIRST_INVOCATION_END();
   
    /*
     * Execute measurement with the specified 'param' and store results 
     * into the 'result' judged with 'limit'. The multisite handling, i.e. 
     * ON_FIRST_INVOCATION block are executed inside this function.
     */
    StandbyCurrentTest::doMeasurement(param,limit,result);
   
    /*
     * Judge and datalog based on the 'result'. This function uses
     * testsuite name as test name, so if you'd like to use your own
     * test names for judgement and datalogging it's needed to modify
     * this funciton or create new one.
     */
    StandbyCurrentTest::judgeAndDatalog(param,limit,result,_results);
   
    /*
     * Output contents of the 'result' to Report Window if specified by
     * the "output" parameter.
     */
    StandbyCurrentTest::reportToUI(param,limit,result,output);
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

string StandbyCurrent::sDefaultTestname = "passCurrLimit_uA";

REGISTER_TESTMETHOD("DcTest.StandbyCurrent", StandbyCurrent);
