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
class FUNC_MCU_TEST: public testmethod::TestMethod {
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
	FLEX_RELAY relay;

	DISCONNECT();
		CONNECT();

		relay.util("SFDQ_K7_12").on();
		relay.wait(1.5 ms );
		relay.execute();

		string vector_lable = Primary.getLabel();
		if(mDebug_Print){cout<<"primary vector_lable = "<<vector_lable<<endl;}

		FUNCTIONAL_TEST();

		relay.util("SFDQ_K7_12").off();
		relay.wait(1.5 ms );
		relay.execute();

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
REGISTER_TESTMETHOD("FUNC_MCU_TEST", FUNC_MCU_TEST);
