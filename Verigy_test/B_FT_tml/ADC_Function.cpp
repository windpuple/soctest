#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;
#define MAX_SITES 4

/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */
class ADC_Function: public testmethod::TestMethod {
protected:
  double  mWait_time;
  int   mDebug_Print;
  Boolean result1[MAX_SITES];
  Boolean result2[MAX_SITES];
  Boolean result3[MAX_SITES];
  Boolean result4[MAX_SITES];
  Boolean result5[MAX_SITES];
  Boolean result6[MAX_SITES];
  Boolean result7[MAX_SITES];
  Boolean result8[MAX_SITES];
protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
  virtual void initialize()
  {
    //Add your initialization code here
    //Note: Test Method API should not be used in this method!
    addParameter("wait_time",
                 "double",
                 &mWait_time,
                 testmethod::TM_PARAMETER_INPUT)
      .setDefault("0.01")
      .setComment("adc_func_wait");
    addParameter("Debug_Print",
                 "int",
                 &mDebug_Print,
                 testmethod::TM_PARAMETER_INPUT)
    .setDefault("0");

  }

  /**
   *This test is invoked per site.
   */
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
      //Add your test code here.

	    int i, site_num;
	    FLEX_RELAY ac_relay;

	    ON_FIRST_INVOCATION_BEGIN();  //For multisite testing
			DISCONNECT();
			CONNECT();

			//ZERO SCALE
			Functional_Test("adci0_func_000", 0.0);
			FOR_EACH_SITE_BEGIN();
				result1[CURRENT_SITE_NUMBER()-1] = GET_FUNCTIONAL_RESULT();
			FOR_EACH_SITE_END();

			Functional_Test("adci1_func_000", 0.0);
			FOR_EACH_SITE_BEGIN();
				result2[CURRENT_SITE_NUMBER()-1] = GET_FUNCTIONAL_RESULT();
			FOR_EACH_SITE_END();

			Functional_Test("adci2_func_000", 0.0);
			FOR_EACH_SITE_BEGIN();
				result3[CURRENT_SITE_NUMBER()-1] = GET_FUNCTIONAL_RESULT();
			FOR_EACH_SITE_END();

			Functional_Test("adci3_func_000", 0.0);
			FOR_EACH_SITE_BEGIN();
				result4[CURRENT_SITE_NUMBER()-1] = GET_FUNCTIONAL_RESULT();
			FOR_EACH_SITE_END();

			//FULL SCALE
			Functional_Test("adci0_func_1ff", 0.0);
			FOR_EACH_SITE_BEGIN();
				result5[CURRENT_SITE_NUMBER()-1] = GET_FUNCTIONAL_RESULT();
			FOR_EACH_SITE_END();

			Functional_Test("adci1_func_1ff", 0.0);
			FOR_EACH_SITE_BEGIN();
				result6[CURRENT_SITE_NUMBER()-1] = GET_FUNCTIONAL_RESULT();
			FOR_EACH_SITE_END();

			Functional_Test("adci2_func_1ff", 0.0);
			FOR_EACH_SITE_BEGIN();
				result7[CURRENT_SITE_NUMBER()-1] = GET_FUNCTIONAL_RESULT();
			FOR_EACH_SITE_END();

			Functional_Test("adci3_func_1ff", 0.0);
			FOR_EACH_SITE_BEGIN();
				result8[CURRENT_SITE_NUMBER()-1] = GET_FUNCTIONAL_RESULT();
			FOR_EACH_SITE_END();


	     ON_FIRST_INVOCATION_END();   //For multisite testing

	     TEST("ADCI0_000", "ADCI0_000", LIMIT(TM::GE, 1, TM::LE, 1),result1[CURRENT_SITE_NUMBER()-1],TM::CONTINUE);
	     TEST("ADCI1_000", "ADCI1_000", LIMIT(TM::GE, 1, TM::LE, 1),result2[CURRENT_SITE_NUMBER()-1],TM::CONTINUE);
	     TEST("ADCI2_000", "ADCI2_000", LIMIT(TM::GE, 1, TM::LE, 1),result3[CURRENT_SITE_NUMBER()-1],TM::CONTINUE);
	     TEST("ADCI3_000", "ADCI3_000", LIMIT(TM::GE, 1, TM::LE, 1),result4[CURRENT_SITE_NUMBER()-1],TM::CONTINUE);

	     TEST("ADCI0_1ff", "ADCI0_1ff", LIMIT(TM::GE, 1, TM::LE, 1),result5[CURRENT_SITE_NUMBER()-1],TM::CONTINUE);
	     TEST("ADCI1_1ff", "ADCI1_1ff", LIMIT(TM::GE, 1, TM::LE, 1),result6[CURRENT_SITE_NUMBER()-1],TM::CONTINUE);
	     TEST("ADCI2_1ff", "ADCI2_1ff", LIMIT(TM::GE, 1, TM::LE, 1),result7[CURRENT_SITE_NUMBER()-1],TM::CONTINUE);
	     TEST("ADCI3_1ff", "ADCI3_1ff", LIMIT(TM::GE, 1, TM::LE, 1),result8[CURRENT_SITE_NUMBER()-1],TM::CONTINUE);

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
REGISTER_TESTMETHOD("ADC_Function", ADC_Function);
