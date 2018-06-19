//for test method framework interfaces
#include "testmethod.hpp"
//for test method API interfaces
#include "mapi.hpp"

#include "DpsConnectivityUtil.hpp"
 /**
  *----------------------------------------------------------------------*
  * @test method class: DpsConnectivity
  *
  * @Purpose: DPS Connectivity testing
  *  (Force Sense Connectivity)
  *
  *----------------------------------------------------------------------*
  * @Description:
  *   string dpsPins:              {@ | pinlist}
  *     Name of dps pins to be tested
  *   string output:               {ReportUI | None}
  *     Print message or not. 
  *     ReportUI - for debug mode
  *     None - for production mode
  *
  * @Note:
  *
  *----------------------------------------------------------------------*
  */

class DpsConnectivity: public testmethod::TestMethod
{
protected:
  double _results[4];
  string  dpsPins;
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
    static DpsConnectivityTest::DpsConnectivityTestParam param;
    static DpsConnectivityTest::DpsConnectivityTestResult result;
    static DpsConnectivityTest::DpsConnectivityTestLimit limit;
   
    ON_FIRST_INVOCATION_BEGIN();
      /*
       * Process all parameters needed in test. Actually this function is
       * called once under multisite because all input parameters should be
       * the same through all sites. The processed parameters are stored into
       * the static variable 'param' for later reference on all sites.
       */   
      DpsConnectivityTest::processParameters(dpsPins,param);

      /*
       * Process the limit needed in test. Actually this function is
       * called once under multisite because all limits should be the 
       * same through all sites. The processed limit are stored into the 
       * static variable 'limit' for later reference on all sites.
       */
      DpsConnectivityTest::processLimit(mTestName,limit);

    ON_FIRST_INVOCATION_END();
   
    /*
     * Execute measurement with the specified 'param' and store results 
     * into the 'result'. The multisite handling, i.e. 
     * ON_FIRST_INVOCATION block are executed inside this function.
     */
    DpsConnectivityTest::doMeasurement(param,result);
   
    /*
     * Judge and datalog based on the 'result'. This function uses
     * testsuite name as test name, so if you'd like to use your own
     * test names for judgement and datalogging it's needed to modify
     * this funciton or create new one.
     */
    DpsConnectivityTest::judgeAndDatalog(param,result,limit,_results);
   
    /*
     * Output contents of the 'result' to Report Window if specified by
     * the "output" parameter.
     */
    DpsConnectivityTest::reportToUI(param,result,output);
  
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

string DpsConnectivity::sDefaultTestname = "DPS_ForceSense";

REGISTER_TESTMETHOD("DcTest.DpsConnectivity", DpsConnectivity);
