#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;

#define MAX_SITES 4

class FREQ_MEASURE_PLL_SCKO_TIA: public testmethod::TestMethod {
protected:
  double  mThreshold;
  int     mSamples;
  double  mWaittime;
  int     mDebug_Print;
  double  SCKO_FREQ[MAX_SITES];
protected:

  virtual void initialize()
  {
		addParameter("Threshold",
	                 "double",
	                 &mThreshold,
	                 testmethod::TM_PARAMETER_INPUT)
	                 .setDefault("0.6");
		    addParameter("Waittime",
					 "double",
					 &mWaittime,
					 testmethod::TM_PARAMETER_INPUT)
		.setDefault("1.0");
	    addParameter("Samples",
	             "int",
	             &mSamples,
	            testmethod::TM_PARAMETER_INPUT)
	        .setDefault("1000");
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
	    DOUBLE time_s, freq_min, freq_max, stdev;

	    PPTIA_TASK task1, task2, task3;

		FLEX_RELAY ac_relay;
		FLEX_RELAY util_relay;
		ON_FIRST_INVOCATION_BEGIN();
			CONNECT();

			ac_relay.pin("@").set("IDLE","OFF");
			ac_relay.wait(1.5 ms);
			ac_relay.execute();

			ac_relay.pin("RSTX, XI0, XI1, TP0, TP1, TP2, HSI, JMODE, JTRST, JTCK, JTMS, JTDI, RX0, SCKO").set("AC","OFF");
			ac_relay.wait(1.5 ms);
			ac_relay.execute();

			string vector_lable = Primary.getLabel();
			if(mDebug_Print){cout<<"primary vector_lable = "<<vector_lable<<endl;}

			task1.pin("SCKO").eventASlope(TM::RISE).eventAThreshold(mThreshold V).eventACount(0);
			task1.pin("SCKO").eventBSlope(TM::RISE).eventBThreshold(mThreshold V).eventBCount(0);
			task1.pin("SCKO").armingMethod(TM::EVENT_A_FIRST);
			task1.pin("SCKO").numberOfSamples( mSamples );
			task1.pin("SCKO").resultType(TM::MEAN) ; //median error when sig is no capture
			task1.preAction(TM::SET_TIA_MODE).enableTimeoutFlag().timeout(0.2).execMode(TM::SEQ_PROG).execute(); //DIRECT default,TM::SEQ_PROG
			Sequencer.reset();

			FOR_EACH_SITE_BEGIN();
			  if(task1.getTimeoutFlag("SCKO")){ SCKO_FREQ[CURRENT_SITE_NUMBER()-1] = 9999.0; }
			  else{ time_s = task1.getValue("SCKO", TM::MEAN);    SCKO_FREQ[CURRENT_SITE_NUMBER()-1]= (1/(1e9 * time_s)*1000);
			  }

			  if(mDebug_Print){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" SCKO_FREQ: " <<SCKO_FREQ[CURRENT_SITE_NUMBER()-1]<<"MHz"<<endl;}

			FOR_EACH_SITE_END();

			DISCONNECT();

		ON_FIRST_INVOCATION_END();

        TEST("SCKO",   "SCKO_FREQ",  LIMIT(TM::GE, 174.0,  TM::LE, 184.0),  SCKO_FREQ[CURRENT_SITE_NUMBER()-1],  TM::CONTINUE);

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
REGISTER_TESTMETHOD("FREQ_MEASURE_PLL_SCKO_TIA", FREQ_MEASURE_PLL_SCKO_TIA);
