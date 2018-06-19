#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;

#define MAX_SITES 4

class FREQ_MEASURE_TIA: public testmethod::TestMethod {
protected:
  double  mThreshold;
  int     mSamples;
  double  mWaittime;
  int     mDebug_Print;
  double  MCKO_FREQ[MAX_SITES];
  double  DCKO_FREQ[MAX_SITES];
  double  SDIP_FREQ[MAX_SITES];
  double  SDIM_FREQ[MAX_SITES];

protected:

  virtual void initialize()
  {
		addParameter("Threshold",
	                 "double",
	                 &mThreshold,
	                 testmethod::TM_PARAMETER_INPUT)
	                 .setDefault("1.5");
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
	    DOUBLE time_s;

	    PPTIA_TASK task1, task2, task3;

		FLEX_RELAY ac_relay;
		FLEX_RELAY util_relay;
		ON_FIRST_INVOCATION_BEGIN();
			CONNECT();

			ac_relay.pin("@").set("IDLE","OFF");
			ac_relay.wait(1.5 ms);
			ac_relay.execute();

			ac_relay.pin("RSTX,XI0,XI1,TP0,TP1,TP2,HSI,JMODE,JTRST,JTCK,JTMS,JTDI,RX0,MCKO,DCKO,SDIP,SDIM").set("AC","OFF");
			ac_relay.wait(1.5 ms);
			ac_relay.execute();

			ac_relay.pin("RSET").set("AC","OFF");
			ac_relay.wait(1.5 ms);
			ac_relay.execute();

			string vector_lable = Primary.getLabel();
			if(mDebug_Print){cout<<"primary vector_lable = "<<vector_lable<<endl;}
			Functional_Test(vector_lable, 0.0);

			task1.pin("MCKO,DCKO").eventASlope(TM::RISE).eventAThreshold(mThreshold V).eventACount(0) ;
			task1.pin("MCKO,DCKO").eventBSlope(TM::RISE).eventBThreshold(mThreshold V).eventBCount(mSamples) ;
			task1.pin("MCKO,DCKO").armingMethod(TM::EVENT_A_FIRST) ;
			task1.pin("MCKO,DCKO").resultType(TM::MEAN) ;

//			task1.pin("MCKO,DCKO,SDIP,SDIM").eventASlope(TM::RISE).eventAThreshold(mThreshold V).eventACount(0) ;
//			task1.pin("MCKO,DCKO,SDIP,SDIM").eventBSlope(TM::RISE).eventBThreshold(mThreshold V).eventBCount(mSamples) ;
//			task1.pin("MCKO,DCKO,SDIP,SDIM").armingMethod(TM::EVENT_A_FIRST) ;
//			task1.pin("MCKO,DCKO,SDIP,SDIM").resultType(TM::MEAN) ;
			Sequencer.reset();
			task1.preAction(TM::SET_TIA_MODE).enableTimeoutFlag().timeout(0.2).execMode(TM::SEQ_PROG).execute(); //DIRECT default,TM::SEQ_PROG

			FOR_EACH_SITE_BEGIN();
			  if(task1.getTimeoutFlag("MCKO")){ MCKO_FREQ[CURRENT_SITE_NUMBER()-1] = 9999.0; }
			  else{ time_s = task1.getValue("MCKO", TM::MEAN); MCKO_FREQ[CURRENT_SITE_NUMBER()-1]= (1e-6 * (mSamples+1) / time_s); }

			  if(task1.getTimeoutFlag("DCKO")){ DCKO_FREQ[CURRENT_SITE_NUMBER()-1] = 9999.0; }
			  else{ time_s = task1.getValue("DCKO", TM::MEAN); DCKO_FREQ[CURRENT_SITE_NUMBER()-1]= (1e-6 * (mSamples+1) / time_s); }

//			  if(task1.getTimeoutFlag("SDIP")){ SDIP_FREQ[CURRENT_SITE_NUMBER()-1] = 9999.0; }
//			  else{ time_s = task1.getValue("SDIP", TM::MEAN); SDIP_FREQ[CURRENT_SITE_NUMBER()-1]= (1e-6 * (mSamples+1) / time_s); }
//
//			  if(task1.getTimeoutFlag("SDIM")){ SDIM_FREQ[CURRENT_SITE_NUMBER()-1] = 9999.0; }
//			  else{ time_s = task1.getValue("SDIM", TM::MEAN); SDIM_FREQ[CURRENT_SITE_NUMBER()-1]= (1e-6 * (mSamples+1) / time_s); }

			  if(mDebug_Print){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" MCKO_FREQ: " <<MCKO_FREQ[CURRENT_SITE_NUMBER()-1]<<"MHz"<<endl;}
			  if(mDebug_Print){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" DCKO_FREQ: " <<DCKO_FREQ[CURRENT_SITE_NUMBER()-1]<<"MHz"<<endl;}
//			  if(mDebug_Print){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" SDIP_FREQ: " <<SDIP_FREQ[CURRENT_SITE_NUMBER()-1]<<"MHz"<<endl;}
//			  if(mDebug_Print){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" SDIM_FREQ: " <<SDIM_FREQ[CURRENT_SITE_NUMBER()-1]<<"MHz"<<endl;}

			FOR_EACH_SITE_END();

		ON_FIRST_INVOCATION_END();

        TEST("MCKO",   "MCKO_FREQ",  LIMIT(TM::GE, 70.2,  TM::LE, 78.2),  MCKO_FREQ[CURRENT_SITE_NUMBER()-1],  TM::CONTINUE);
        TEST("DCKO",   "DCKO_FREQ",  LIMIT(TM::GE, 70.2,  TM::LE, 78.2),  DCKO_FREQ[CURRENT_SITE_NUMBER()-1],  TM::CONTINUE);
      //  TEST("SDIP",   "SDIP_FREQ",  LIMIT(TM::GE, 72.2,  TM::LE, 76.2),  SDIP_FREQ[CURRENT_SITE_NUMBER()-1],  TM::CONTINUE);
      //  TEST("SDIM",   "SDIM_FREQ",  LIMIT(TM::GE, 72.2,  TM::LE, 76.2),  SDIM_FREQ[CURRENT_SITE_NUMBER()-1],  TM::CONTINUE);

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
REGISTER_TESTMETHOD("FREQ_MEASURE_TIA", FREQ_MEASURE_TIA);
