#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;

#define MAX_SITES 4

class FREQ_MEASURE_PLL_HDSDI_TIA: public testmethod::TestMethod {
protected:
  double  mThreshold;
  int     mSamples;
  double  mWaittime;
  int     mDebug_Print;
  double  MCKO_FREQ[MAX_SITES];
  double  DCKO_FREQ[MAX_SITES];
  double  SDIP_FREQ[MAX_SITES];
  double  SDIM_FREQ[MAX_SITES];
  double  SCKO_FREQ[MAX_SITES];
  double  MCKO_FREQ_MIN[MAX_SITES];
  double  DCKO_FREQ_MIN[MAX_SITES];
  double  SDIP_FREQ_MIN[MAX_SITES];
  double  SDIM_FREQ_MIN[MAX_SITES];
  double  SCKO_FREQ_MIN[MAX_SITES];
  double  MCKO_FREQ_MAX[MAX_SITES];
  double  DCKO_FREQ_MAX[MAX_SITES];
  double  SDIP_FREQ_MAX[MAX_SITES];
  double  SDIM_FREQ_MAX[MAX_SITES];
  double  SCKO_FREQ_MAX[MAX_SITES];
  double  MCKO_FREQ_P2PJITTER[MAX_SITES];
  double  DCKO_FREQ_P2PJITTER[MAX_SITES];
  double  SDIP_FREQ_P2PJITTER[MAX_SITES];
  double  SDIM_FREQ_P2PJITTER[MAX_SITES];
  double  SCKO_FREQ_P2PJITTER[MAX_SITES];
  double  MCKO_FREQ_RMSJITTER[MAX_SITES];
  double  DCKO_FREQ_RMSJITTER[MAX_SITES];
  double  SDIP_FREQ_RMSJITTER[MAX_SITES];
  double  SDIM_FREQ_RMSJITTER[MAX_SITES];
  double  SCKO_FREQ_RMSJITTER[MAX_SITES];
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

			ac_relay.pin("JTDO,RSTX,XI0,XI1,TP0,TP1,TP2,HSI,JMODE,JTRST,JTCK,JTMS,JTDI,RX0,MCKO,DCKO,SDIP,SDIM,SCKO").set("AC","OFF");
			ac_relay.wait(1.5 ms);
			ac_relay.execute();

			ac_relay.pin("RSET").set("AC","OFF");
			ac_relay.wait(1.5 ms);
			ac_relay.execute();

			string vector_lable = Primary.getLabel();
			if(mDebug_Print){cout<<"primary vector_lable = "<<vector_lable<<endl;}

			task1.pin("MCKO,DCKO,SDIP,SDIM,SCKO").eventASlope(TM::RISE).eventAThreshold(mThreshold V).eventACount(0);
			task1.pin("MCKO,DCKO,SDIP,SDIM,SCKO").eventBSlope(TM::RISE).eventBThreshold(mThreshold V).eventBCount(0);
			task1.pin("MCKO,DCKO,SDIP,SDIM,SCKO").armingMethod(TM::EVENT_A_FIRST);
			task1.pin("MCKO,DCKO,SDIP,SDIM,SCKO").numberOfSamples( mSamples );
			task1.pin("MCKO,DCKO,SDIP,SDIM,SCKO").resultType(TM::MEAN|TM::MIN|TM::MAX|TM::STDEV) ; //median error when sig is no capture
			task1.preAction(TM::SET_TIA_MODE).enableTimeoutFlag().timeout(0.2).execMode(TM::SEQ_PROG).execute(); //DIRECT default,TM::SEQ_PROG
			Sequencer.reset();

			FOR_EACH_SITE_BEGIN();
			  if(task1.getTimeoutFlag("MCKO")){ MCKO_FREQ[CURRENT_SITE_NUMBER()-1] = 9999.0; }
			  else{ time_s = task1.getValue("MCKO", TM::MEAN);    MCKO_FREQ[CURRENT_SITE_NUMBER()-1]= (1/(1e9 * time_s)*1000);
//				    freq_min = task1.getValue("MCKO", TM::MIN);   MCKO_FREQ_MIN[CURRENT_SITE_NUMBER()-1] = freq_min;
//				    freq_max = task1.getValue("MCKO", TM::MAX);   MCKO_FREQ_MAX[CURRENT_SITE_NUMBER()-1] = freq_max;
//				    stdev    = task1.getValue("MCKO", TM::STDEV); MCKO_FREQ_RMSJITTER[CURRENT_SITE_NUMBER()-1] = stdev*1e9;
			  }

			  if(task1.getTimeoutFlag("DCKO")){ DCKO_FREQ[CURRENT_SITE_NUMBER()-1] = 9999.0; }
			  else{ time_s = task1.getValue("DCKO", TM::MEAN);    DCKO_FREQ[CURRENT_SITE_NUMBER()-1]= (1/(1e9 * time_s)*1000);
//					freq_min = task1.getValue("DCKO", TM::MIN);   DCKO_FREQ_MIN[CURRENT_SITE_NUMBER()-1] = freq_min;
//					freq_max = task1.getValue("DCKO", TM::MAX);   DCKO_FREQ_MAX[CURRENT_SITE_NUMBER()-1] = freq_max;
//					stdev    = task1.getValue("DCKO", TM::STDEV); DCKO_FREQ_RMSJITTER[CURRENT_SITE_NUMBER()-1] = stdev*1e9;
			  }

			  if(task1.getTimeoutFlag("SDIP")){ SDIP_FREQ[CURRENT_SITE_NUMBER()-1] = 9999.0; }
			  else{ time_s = task1.getValue("SDIP", TM::MEAN);    SDIP_FREQ[CURRENT_SITE_NUMBER()-1]= (1/(1e9 * time_s)*1000);
//					freq_min = task1.getValue("SDIP", TM::MIN);   SDIP_FREQ_MIN[CURRENT_SITE_NUMBER()-1] = freq_min;
//					freq_max = task1.getValue("SDIP", TM::MAX);   SDIP_FREQ_MAX[CURRENT_SITE_NUMBER()-1] = freq_max;
//					stdev    = task1.getValue("SDIP", TM::STDEV); SDIP_FREQ_RMSJITTER[CURRENT_SITE_NUMBER()-1] = stdev*1e9;
			  }

			  if(task1.getTimeoutFlag("SDIM")){ SDIM_FREQ[CURRENT_SITE_NUMBER()-1] = 9999.0; }
			  else{ time_s = task1.getValue("SDIM", TM::MEAN);    SDIM_FREQ[CURRENT_SITE_NUMBER()-1]= (1/(1e9 * time_s)*1000);
//					freq_min = task1.getValue("SDIM", TM::MIN);   SDIM_FREQ_MIN[CURRENT_SITE_NUMBER()-1] = freq_min;
//					freq_max = task1.getValue("SDIM", TM::MAX);   SDIM_FREQ_MAX[CURRENT_SITE_NUMBER()-1] = freq_max;
//					stdev    = task1.getValue("SDIM", TM::STDEV); SDIM_FREQ_RMSJITTER[CURRENT_SITE_NUMBER()-1] = stdev*1e9;
			  }
			  if(task1.getTimeoutFlag("SCKO")){ SCKO_FREQ[CURRENT_SITE_NUMBER()-1] = 9999.0; }
			  else{ time_s = task1.getValue("SCKO", TM::MEAN);    SCKO_FREQ[CURRENT_SITE_NUMBER()-1]= (1/(1e9 * time_s)*1000);
//					freq_min = task1.getValue("SCKO", TM::MIN);   SCKO_FREQ_MIN[CURRENT_SITE_NUMBER()-1] = freq_min;
//					freq_max = task1.getValue("SCKO", TM::MAX);   SCKO_FREQ_MAX[CURRENT_SITE_NUMBER()-1] = freq_max;
//					stdev    = task1.getValue("SCKO", TM::STDEV); SCKO_FREQ_RMSJITTER[CURRENT_SITE_NUMBER()-1] = stdev*1e9;
			  }

			  MCKO_FREQ_P2PJITTER[CURRENT_SITE_NUMBER()-1] = (MCKO_FREQ_MAX[CURRENT_SITE_NUMBER()-1]-MCKO_FREQ_MIN[CURRENT_SITE_NUMBER()-1])*1e9;
			  DCKO_FREQ_P2PJITTER[CURRENT_SITE_NUMBER()-1] = (DCKO_FREQ_MAX[CURRENT_SITE_NUMBER()-1]-DCKO_FREQ_MIN[CURRENT_SITE_NUMBER()-1])*1e9;
			  SDIP_FREQ_P2PJITTER[CURRENT_SITE_NUMBER()-1] = (SDIP_FREQ_MAX[CURRENT_SITE_NUMBER()-1]-SDIP_FREQ_MIN[CURRENT_SITE_NUMBER()-1])*1e9;
			  SDIM_FREQ_P2PJITTER[CURRENT_SITE_NUMBER()-1] = (SDIM_FREQ_MAX[CURRENT_SITE_NUMBER()-1]-SDIM_FREQ_MIN[CURRENT_SITE_NUMBER()-1])*1e9;
			  SCKO_FREQ_P2PJITTER[CURRENT_SITE_NUMBER()-1] = (SCKO_FREQ_MAX[CURRENT_SITE_NUMBER()-1]-SCKO_FREQ_MIN[CURRENT_SITE_NUMBER()-1])*1e9;

			  if(mDebug_Print){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" MCKO_FREQ: " <<MCKO_FREQ[CURRENT_SITE_NUMBER()-1]<<"MHz"<<endl;}
			  if(mDebug_Print){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" DCKO_FREQ: " <<DCKO_FREQ[CURRENT_SITE_NUMBER()-1]<<"MHz"<<endl;}
			  if(mDebug_Print){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" SDIP_FREQ: " <<SDIP_FREQ[CURRENT_SITE_NUMBER()-1]<<"MHz"<<endl;}
			  if(mDebug_Print){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" SDIM_FREQ: " <<SDIM_FREQ[CURRENT_SITE_NUMBER()-1]<<"MHz"<<endl;}
			  if(mDebug_Print){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" SCKO_FREQ: " <<SCKO_FREQ[CURRENT_SITE_NUMBER()-1]<<"MHz"<<endl;}

#if 0
			  if(mDebug_Print){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" MCKO_FREQ_P2PJITTER: " <<MCKO_FREQ_P2PJITTER[CURRENT_SITE_NUMBER()-1]<<"ns"<<endl;}
			  if(mDebug_Print){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" DCKO_FREQ_P2PJITTER: " <<DCKO_FREQ_P2PJITTER[CURRENT_SITE_NUMBER()-1]<<"ns"<<endl;}
			  if(mDebug_Print){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" SDIP_FREQ_P2PJITTER: " <<SDIP_FREQ_P2PJITTER[CURRENT_SITE_NUMBER()-1]<<"ns"<<endl;}
			  if(mDebug_Print){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" SDIM_FREQ_P2PJITTER: " <<SDIM_FREQ_P2PJITTER[CURRENT_SITE_NUMBER()-1]<<"ns"<<endl;}
			  if(mDebug_Print){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" SCKO_FREQ_P2PJITTER: " <<SCKO_FREQ_P2PJITTER[CURRENT_SITE_NUMBER()-1]<<"ns"<<endl;}

			  if(mDebug_Print){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" MCKO_FREQ_RMSJITTER: " <<MCKO_FREQ_RMSJITTER[CURRENT_SITE_NUMBER()-1]<<"ns"<<endl;}
			  if(mDebug_Print){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" DCKO_FREQ_RMSJITTER: " <<DCKO_FREQ_RMSJITTER[CURRENT_SITE_NUMBER()-1]<<"ns"<<endl;}
			  if(mDebug_Print){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" SDIP_FREQ_RMSJITTER: " <<SDIP_FREQ_RMSJITTER[CURRENT_SITE_NUMBER()-1]<<"ns"<<endl;}
			  if(mDebug_Print){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" SDIM_FREQ_RMSJITTER: " <<SDIM_FREQ_RMSJITTER[CURRENT_SITE_NUMBER()-1]<<"ns"<<endl;}
			  if(mDebug_Print){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" SCKO_FREQ_RMSJITTER: " <<SCKO_FREQ_RMSJITTER[CURRENT_SITE_NUMBER()-1]<<"ns"<<endl;}
#endif

			FOR_EACH_SITE_END();

			DISCONNECT();

		ON_FIRST_INVOCATION_END();

        TEST("MCKO_FREQ",   "MCKO_FREQ",  LIMIT(TM::GE, 86.2,  TM::LE, 94.2),  MCKO_FREQ[CURRENT_SITE_NUMBER()-1],  TM::CONTINUE);
        TEST("DCKO_FREQ",   "DCKO_FREQ",  LIMIT(TM::GE, 70.2,  TM::LE, 78.2),  DCKO_FREQ[CURRENT_SITE_NUMBER()-1],  TM::CONTINUE);
        TEST("SDIP_FREQ",   "SDIP_FREQ",  LIMIT(TM::GE, 144.5,  TM::LE, 155.5),  SDIP_FREQ[CURRENT_SITE_NUMBER()-1],  TM::CONTINUE);
        TEST("SDIM_FREQ",   "SDIM_FREQ",  LIMIT(TM::GE, 144.5,  TM::LE, 155.5),  SDIM_FREQ[CURRENT_SITE_NUMBER()-1],  TM::CONTINUE);
        TEST("SCKO_FREQ",   "SCKO_FREQ",  LIMIT(TM::GE, 176.0,  TM::LE, 184.0),  SCKO_FREQ[CURRENT_SITE_NUMBER()-1],  TM::CONTINUE);

#if 0
        TEST("MCKO_FREQ_P2PJITTER",   "MCKO_FREQ_P2PJITTER",  LIMIT(TM::GE, 0.0,  TM::LE, 10.0),  MCKO_FREQ_P2PJITTER[CURRENT_SITE_NUMBER()-1],  TM::CONTINUE);
        TEST("DCKO_FREQ_P2PJITTER",   "DCKO_FREQ_P2PJITTER",  LIMIT(TM::GE, 0.0,  TM::LE, 10.0),  DCKO_FREQ_P2PJITTER[CURRENT_SITE_NUMBER()-1],  TM::CONTINUE);
        TEST("SDIP_FREQ_P2PJITTER",   "SDIP_FREQ_P2PJITTER",  LIMIT(TM::GE, 0.0,  TM::LE, 10.0),  SDIP_FREQ_P2PJITTER[CURRENT_SITE_NUMBER()-1],  TM::CONTINUE);
        TEST("SDIM_FREQ_P2PJITTER",   "SDIM_FREQ_P2PJITTER",  LIMIT(TM::GE, 0.0,  TM::LE, 10.0),  SDIM_FREQ_P2PJITTER[CURRENT_SITE_NUMBER()-1],  TM::CONTINUE);

        TEST("MCKO_FREQ_RMSJITTER",   "MCKO_FREQ_RMSJITTER",  LIMIT(TM::GE, 0.0,  TM::LE, 5.0),  MCKO_FREQ_RMSJITTER[CURRENT_SITE_NUMBER()-1],  TM::CONTINUE);
        TEST("DCKO_FREQ_RMSJITTER",   "DCKO_FREQ_RMSJITTER",  LIMIT(TM::GE, 0.0,  TM::LE, 5.0),  DCKO_FREQ_RMSJITTER[CURRENT_SITE_NUMBER()-1],  TM::CONTINUE);
        TEST("SDIP_FREQ_RMSJITTER",   "SDIP_FREQ_RMSJITTER",  LIMIT(TM::GE, 0.0,  TM::LE, 5.0),  SDIP_FREQ_RMSJITTER[CURRENT_SITE_NUMBER()-1],  TM::CONTINUE);
        TEST("SDIM_FREQ_RMSJITTER",   "SDIM_FREQ_RMSJITTER",  LIMIT(TM::GE, 0.0,  TM::LE, 5.0),  SDIM_FREQ_RMSJITTER[CURRENT_SITE_NUMBER()-1],  TM::CONTINUE);
#endif

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
REGISTER_TESTMETHOD("FREQ_MEASURE_PLL_HDSDI_TIA", FREQ_MEASURE_PLL_HDSDI_TIA);
