#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;

/*
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */

class DAC_Distortion_FS_DEBUG: public testmethod::TestMethod {

protected:
  string  DGT_pin;
  //string  SMC_Distortion_handleName;
  int num_cycle_DAC;
  int num_harmonics_DAC;
  int  debug;
  int test_site;
  double waittime;
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
protected:
  virtual void initialize()
  {
    //Add your initialization code here
    //Note: Test Method API should not be used in this method!
    addParameter("debug",
                 "int",
                 &debug,
                 testmethod::TM_PARAMETER_INPUT)
      .setDefault("1")
      .setComment("control if the test is in debug mode");
    addParameter("waittime",
                 "double",
                 &waittime,
                 testmethod::TM_PARAMETER_INPUT)
      .setDefault("0.0")
      .setComment("relay control waittime and the unit is second");
  }


  virtual void run()
  {
	FLEX_RELAY relay;

    int i,j;

    FLEX_RELAY ac_relay;

    ON_FIRST_INVOCATION_BEGIN();
		DISCONNECT();
		CONNECT();
		relay.util("K5_CVBS,K7_COMP_CAP,K6_REXTD_REGISTER").on();relay.wait(1.5 ms);relay.execute();

		ac_relay.pin("@").set("IDLE","OFF");
		ac_relay.wait(1.5 ms);
		ac_relay.execute();

		ac_relay.pin("DACTRIG,RSTX,TP0,TP1,TP2,JMODE,JTRST,JTCK,JTMS,JTDI,GPIO20,GPIO31,GPIO21,GPIO22,GPIO23,GPIO24,GPIO25,GPIO26,GPIO27,GPIO28,GPIO29,GPIO30").set("AC","OFF");
		ac_relay.wait(1.5 ms);
		ac_relay.execute();

		FUNCTIONAL_TEST(); //Need to Pre-Running


		relay.util("K5_CVBS,K7_COMP_CAP,K6_REXTD_REGISTER").off();relay.wait(1.5 ms);relay.execute();

    ON_FIRST_INVOCATION_END();


    DISCONNECT();

    return;
  }

  virtual void postParameterChange(const string& parameterIdentifier)
  {
    return;
  }

  virtual const string getComment() const 
  {
    string comment = "DAC RAMP Distortion Test.";
    return comment;
  }
};
REGISTER_TESTMETHOD("DAC_Distortion_FS_DEBUG", DAC_Distortion_FS_DEBUG);
