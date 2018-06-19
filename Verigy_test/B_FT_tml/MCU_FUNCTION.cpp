#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;

/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */
class MCU_FUNCTION: public testmethod::TestMethod {
protected:
  double  mWaittime;
  int   mDebug_Print;
  int   mInitial_excute;
protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
  virtual void initialize()
  {
    //Add your initialization code here
    //Note: Test Method API should not be used in this method!
    addParameter("Waittime",
                 "double",
                 &mWaittime,
                 testmethod::TM_PARAMETER_INPUT);
	addParameter("Debug_Print",
				 "int",
				 &mDebug_Print,
				 testmethod::TM_PARAMETER_INPUT)
	.setDefault("1");
  }
  void Functional_Test(string lable,double waittime)
  {
      Primary.label(lable);  //SetupAPI-Digital_Setting
      FUNCTIONAL_TEST();
      if(mDebug_Print==1){
          FOR_EACH_SITE_BEGIN();
           if(GET_FUNCTIONAL_RESULT())
               cout <<"SITE:"<<CURRENT_SITE_NUMBER()<<" "<<lable<<" Test PASSED!!!" << endl;
           else
               cout <<"SITE:"<<CURRENT_SITE_NUMBER()<<" "<<lable<<" Test FAILED!!!" << endl;
          FOR_EACH_SITE_END();
      }
      WAIT_TIME(waittime ms);
  }

  virtual void run()
  {

	ON_FIRST_INVOCATION_BEGIN();

	FLEX_RELAY ac_relay;
	FLEX_RELAY util_relay;
        DISCONNECT();
		CONNECT();

		util_relay.util("K19K20_SFCSN_SFDQ012,K21K22_SFCKO_SFDQ3_VDDQ").on();util_relay.wait(1.5 ms);util_relay.execute();

	    ac_relay.pin("@").set("IDLE","OFF");
	    ac_relay.wait(1.5 ms);
	    ac_relay.execute();

        ac_relay.pin("EXTS, RSTX, RX0, JTMS, JTCK, JTRST, TP2, TP1, TP0, JTDI, JMODE, XI0, XI1").set("AC","OFF");
        ac_relay.pin("GPIOGRP").set("AC","OFF");
	    ac_relay.wait(1.5 ms);
	    ac_relay.execute();

		string vector_lable = Primary.getLabel();
		if(mDebug_Print){cout<<"primary vector_lable = "<<vector_lable<<endl;}

        Functional_Test(vector_lable, 0.0);

		util_relay.util("K19K20_SFCSN_SFDQ012,K21K22_SFCKO_SFDQ3_VDDQ").off();util_relay.wait(1.5 ms);util_relay.execute();

	    ac_relay.pin("@").set("AC","OFF");
	    ac_relay.wait(1.5 ms);
	    ac_relay.execute();

	ON_FIRST_INVOCATION_END();

    TEST(TM::CONTINUE);  //judge

    return;
  }


  virtual void postParameterChange(const string& parameterIdentifier)
  {
    return;
  }

  virtual const string getComment() const 
  {
    string comment = " please add your comment for this method.";
    return comment;
  }
};
REGISTER_TESTMETHOD("MCU_FUNCTION", MCU_FUNCTION);
