//for testmethod framework interfaces
#include "testmethod.hpp"
//for testmethod API interfaces
#include "mapi.hpp"

#include "DVMUtil.hpp"

 /**
  *----------------------------------------------------------------------*
  * @testmethod class: DVM
  *
  * @Purpose: Measure the voltage of pins.
  *
  *----------------------------------------------------------------------*
  * @Description:
  *   string pinlist:              {@ | pinlist}
  *     Name of pins to be tested.
  *   testmethod::SpecValue  settlingTime          {ms} 
  *     the settling time will be in addition to the settling time 
  *     calculated by the system. The base unit is ms.
  *   string measureMode           {PPMUpar | BoardADC}
  *     measurement via PPMU or BoardADC.
  *     default is PPMUpar.
  *   string output:               {ReportUI | None}
  *     Print message or not. 
  *     ReportUI - for debug mode
  *     None - for production mode
  *
  * @Note:
  *
  *----------------------------------------------------------------------*
  */


class DVM: public testmethod::TestMethod
{
protected:
  double _results[4];
  string  pinlist;
  string  measureMode;
  string  ACRelay;
  testmethod::SpecValue settlingTime;
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
      .setDefault("@")
      .setComment("name of pins need to be tested");
    addParameter("settlingTime",
                 "SpecValue",
                 &settlingTime)
      .setDefault("0[ms]")
      .setComment("extra settling time for the test,e.g. 1[ms]");
    addParameter("measureMode",
                 "string",
                 &measureMode)
      .setDefault("PPMUpar")
      .setOptions("PPMUpar:BoardADC")
      .setComment("do measurement via PPMU parallel or Board ADC.\n"
                  "'PPMUpar' doesn't work for PS400 pin, and use 'BoardADC' instead.");
    addParameter("testName",
                 "string",
                 &mTestName)
      .setDefault(sDefaultTestname)
      .setComment("test limit's name, default is \""+sDefaultTestname+"\"\n"
         "if test table is used, the limit name is defined in table\n"
         "if predefined limit is used, the limit name must be as same as default.");
    addParameter("ACRelay",
                 "string",
                 &ACRelay)
      .setDefault(TURN_ON)
      .setOptions("ON:OFF")
      .setVisible(false)
      .setComment("For BoardADC measure mode, AC relay can be controlled by user.");
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
    static DVMTest::DVMTestParam param;
    static DVMTest::DVMTestLimit limit;
    
    DVMTest::DVMTestResult result;
    
    ON_FIRST_INVOCATION_BEGIN();
      /*
       * Process all parameters needed in test. Actually this function is
       * called once under multisite because all input parameters should be
       * the same through all sites. The processed parameters are stored into
       * the static variable 'param' for later reference on all sites.
       */   
      DVMTest::processParameters(pinlist, 
                                 settlingTime.getValueAsTargetUnit("ms"),
                                 measureMode, 
                                 ACRelay, 
                                 param);
      
      /*
       * Process the limit needed in test. Actually this function is
       * called once under multisite because all limits should be the 
       * same through all sites. The processed limit are stored into the 
       * static variable 'limit' for later reference on all sites.
       */
      DVMTest::processLimit(mTestName, (mTestName == sDefaultTestname), limit);
    ON_FIRST_INVOCATION_END();
    
    /*
     * Execute measurement with the specified 'param' and store results 
     * into the 'result' judged with 'limit'. The multisite handling, i.e. 
     * ON_FIRST_INVOCATION block are executed inside this function.
     */
    DVMTest::doMeasurement(param,limit,result);
    
    /*
     * Judge and datalog based on the 'result'. This function uses
     * testsuite name as test name, so if you'd like to use your own
     * test names for judgement and datalogging it's needed to modify
     * this funciton or create new one.
     */
    DVMTest::judgeAndDatalog(param,limit,result,_results);
    
    /*
     * Output contents of the 'result' to Report Window if specified by
     * the "output" parameter.
     */
    DVMTest::reportToUI(param,limit,result,output);
    
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
    if ( parameterIdentifier == "measureMode" )
    {
      if ( measureMode == "BoardADC" )
      {
        getParameter("ACRelay").setVisible(true);
      }
      else  // default is "PPMUpar"
      {
        getParameter("ACRelay").setVisible(false);
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

string DVM::sDefaultTestname = "passVoltageLimit_mV";

REGISTER_TESTMETHOD("DcTest.DVM", DVM);
