//for testmethod framework interfaces
#include "testmethod.hpp"
//for testmethod API interfaces
#include "mapi.hpp"

#include "DpsStatusUtil.hpp"

 /**
  *----------------------------------------------------------------------*
  * @testmethod class: DpsStatus
  *
  * @Purpose: DPS Status testing
  *
  *----------------------------------------------------------------------*
  * @Description:
  *   string dpsPins:           {@ | pinlist}
  *     Name of dps pins to be tested
  *   string constantCurrent    { ON| OFF}
  *     check whether dps is in constant current state.
  *   string unregulated        { ON| OFF}
  *     check whether dps is in unregulated state.
  *   string overVoltage        { ON| OFF}
  *     check whether dps is in over voltage state.
  *   string overPowerTemp      { ON| OFF}
  *     check whether dps is in over power/over temperature state.
  *   string output:            {ReportUI | None}
  *     Print message or not. 
  *     ReportUI - for debug mode
  *     None - for production mode
  *
  * @Note:
  *
  *----------------------------------------------------------------------*
  */

class DpsStatus: public testmethod::TestMethod
{
protected:
  double _results[4];
  string  dpsPins;
  string  constantCurrent;
  string  unregulated;
  string  overVoltage;
  string  overPowerTemp;
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
      .setDefault("@")
      .setComment("dps pins need to be tested");
    addParameter("constantCurrent",
                 "string",
                 &constantCurrent)
      .setDefault("OFF")
      .setOptions("ON:OFF")
      .setComment("check constant current state");
    addParameter("unregulated",
                 "string",
                 &unregulated)
      .setDefault("OFF")
      .setOptions("ON:OFF")
      .setComment("check unregulated state");
    addParameter("overVoltage",
                 "string",
                 &overVoltage)
      .setDefault("OFF")
      .setOptions("ON:OFF")
      .setComment("check over voltage state");
    addParameter("overPowerTemp",
                 "string",
                 &overPowerTemp)
      .setDefault("OFF")
      .setOptions("ON:OFF")
      .setComment("check over power/ over temperature state");
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
    static DpsStatusTest::DpsStatusTestParam param;
    static DpsStatusTest::DpsStatusTestResult result;
    static DpsStatusTest::DpsStatusTestLimit limit;
   
    ON_FIRST_INVOCATION_BEGIN();
      /*
       * Process all parameters needed in test. Actually this function is
       * called once under multisite because all input parameters should be
       * the same through all sites. The processed parameters are stored into
       * the static variable 'param' for later reference on all sites.
       */   
      DpsStatusTest::processParameters(dpsPins,
                                       constantCurrent,
                                       unregulated,
                                       overVoltage,
                                       overPowerTemp,
                                       param);

      /*
       * Process the limit needed in test. Actually this function is
       * called once under multisite because all limits should be the 
       * same through all sites. The processed limit are stored into the 
       * static variable 'limit' for later reference on all sites.
       */
      DpsStatusTest::processLimit(mTestName,limit);
    ON_FIRST_INVOCATION_END();
     
    /*
     * Execute measurement with the specified 'param' and store results 
     * into the 'result'. The multisite handling, i.e. 
     * ON_FIRST_INVOCATION block are executed inside this function.
     */
    DpsStatusTest::doMeasurement(param,result);
   
    /*
     * Judge and datalog based on the 'result'. This function uses
     * testsuite name as test name, so if you'd like to use your own
     * test names for judgement and datalogging it's needed to modify
     * this funciton or create new one.
     */
    DpsStatusTest::judgeAndDatalog(param,result,limit,_results);
   
    /*
     * Output contents of the 'result' to Report Window if specified by
     * the "output" parameter.
     */
    DpsStatusTest::reportToUI(param,result,output);     
  
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

string DpsStatus::sDefaultTestname = "DPS_Status";

REGISTER_TESTMETHOD("DcTest.DpsStatus", DpsStatus);
