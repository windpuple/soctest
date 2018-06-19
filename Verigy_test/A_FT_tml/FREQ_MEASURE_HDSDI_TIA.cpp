#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;

#define MAX_SITES 4

class FREQ_MEASURE_HDSDI_TIA: public testmethod::TestMethod {
protected:
	double mThreshold;
	int mSamples;
	double mWaittime;
	int mDebug_Print;
	double SDIRP_FREQ[MAX_SITES];
	double SDIRM_FREQ[MAX_SITES];
	double SDITP_FREQ[MAX_SITES];
	double SDITM_FREQ[MAX_SITES];
	double MCKO_FREQ[MAX_SITES];
	double UCCDI_FREQ[MAX_SITES];

protected:

	virtual void initialize() {
		addParameter("Threshold", "double", &mThreshold,
				testmethod::TM_PARAMETER_INPUT) .setDefault("1.5");
		addParameter("Waittime", "double", &mWaittime,
				testmethod::TM_PARAMETER_INPUT) .setDefault("1.0");
		addParameter("Samples", "int", &mSamples,
				testmethod::TM_PARAMETER_INPUT) .setDefault("1000");
		addParameter("Debug_Print", "int", &mDebug_Print,
				testmethod::TM_PARAMETER_INPUT) .setDefault("1");
	}

	void Functional_Test(string lable, double waittime) {
		Primary.label(lable); //SetupAPI-Digital_Setting
		FUNCTIONAL_TEST();
		if (mDebug_Print == 1) {
			FOR_EACH_SITE_BEGIN();
if					(GET_FUNCTIONAL_RESULT())
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
				DISCONNECT();
				CONNECT();

				ac_relay.pin("@").set("IDLE","OFF");
				ac_relay.wait(1.5 ms);
				ac_relay.execute();

				ac_relay.pin("SDI_GRP").set("AC","OFF");
				ac_relay.wait(1.5 ms);
				ac_relay.execute();

				string vector_lable = Primary.getLabel();
				if(mDebug_Print) {cout<<"primary vector_lable = "<<vector_lable<<endl;}
				Functional_Test(vector_lable, 0.0);

				task1.pin("SDIRP,SDIRM,SDITP,SDITM,MCKO,UCCDI").eventASlope(TM::RISE).eventAThreshold(mThreshold V).eventACount(0);
				task1.pin("SDIRP,SDIRM,SDITP,SDITM,MCKO,UCCDI").eventBSlope(TM::RISE).eventBThreshold(mThreshold V).eventBCount(mSamples);
				task1.pin("SDIRP,SDIRM,SDITP,SDITM,MCKO,UCCDI").armingMethod(TM::EVENT_A_FIRST);
				task1.pin("SDIRP,SDIRM,SDITP,SDITM,MCKO,UCCDI").resultType(TM::MEAN);

				Sequencer.reset();
				task1.preAction(TM::SET_TIA_MODE).enableTimeoutFlag().timeout(0.2).execMode(TM::SEQ_PROG).execute(); //DIRECT default,TM::SEQ_PROG

				FOR_EACH_SITE_BEGIN();
				if(task1.getTimeoutFlag("SDIRP")) {SDIRP_FREQ[CURRENT_SITE_NUMBER()-1] = 9999.0;}
				else {time_s = task1.getValue("SDIRP", TM::MEAN); SDIRP_FREQ[CURRENT_SITE_NUMBER()-1]= (1e-6 * (mSamples+1) / time_s);}

				if(task1.getTimeoutFlag("SDIRM")) {SDIRM_FREQ[CURRENT_SITE_NUMBER()-1] = 9999.0;}
				else {time_s = task1.getValue("SDIRM", TM::MEAN); SDIRM_FREQ[CURRENT_SITE_NUMBER()-1]= (1e-6 * (mSamples+1) / time_s);}

				if(task1.getTimeoutFlag("SDITP")) {SDITP_FREQ[CURRENT_SITE_NUMBER()-1] = 9999.0;}
				else {time_s = task1.getValue("SDITP", TM::MEAN); SDITP_FREQ[CURRENT_SITE_NUMBER()-1]= (1e-6 * (mSamples+1) / time_s);}

				if(task1.getTimeoutFlag("SDITM")) {SDITM_FREQ[CURRENT_SITE_NUMBER()-1] = 9999.0;}
				else {time_s = task1.getValue("SDITM", TM::MEAN); SDITM_FREQ[CURRENT_SITE_NUMBER()-1]= (1e-6 * (mSamples+1) / time_s);}

				if(task1.getTimeoutFlag("MCKO")) {MCKO_FREQ[CURRENT_SITE_NUMBER()-1] = 9999.0;}
				else {time_s = task1.getValue("MCKO", TM::MEAN); MCKO_FREQ[CURRENT_SITE_NUMBER()-1]= (1e-6 * (mSamples+1) / time_s);}

				if(task1.getTimeoutFlag("UCCDI")) {UCCDI_FREQ[CURRENT_SITE_NUMBER()-1] = 9999.0;}
				else {time_s = task1.getValue("UCCDI", TM::MEAN); UCCDI_FREQ[CURRENT_SITE_NUMBER()-1]= (1e-6 * (mSamples+1) / time_s);}

				if(mDebug_Print) {cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" SDIRP_FREQ: " <<SDIRP_FREQ[CURRENT_SITE_NUMBER()-1]<<"MHz"<<endl;}
				if(mDebug_Print) {cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" SDIRM_FREQ: " <<SDIRM_FREQ[CURRENT_SITE_NUMBER()-1]<<"MHz"<<endl;}
				if(mDebug_Print) {cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" SDITP_FREQ: " <<SDITP_FREQ[CURRENT_SITE_NUMBER()-1]<<"MHz"<<endl;}
				if(mDebug_Print) {cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" SDITM_FREQ: " <<SDITM_FREQ[CURRENT_SITE_NUMBER()-1]<<"MHz"<<endl;}
				if(mDebug_Print) {cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" MCKO_FREQ: " <<MCKO_FREQ[CURRENT_SITE_NUMBER()-1]<<"MHz"<<endl;}
				if(mDebug_Print) {cout<<"SITE:"<<CURRENT_SITE_NUMBER()<<setw(10)<<" UCCDI_FREQ: " <<UCCDI_FREQ[CURRENT_SITE_NUMBER()-1]<<"MHz"<<endl;}

				FOR_EACH_SITE_END();

				ON_FIRST_INVOCATION_END();

				TEST("SDIRP", "SDIRP_FREQ", LIMIT(TM::GE, 70.2, TM::LE, 78.2), SDIRP_FREQ[CURRENT_SITE_NUMBER()-1], TM::CONTINUE);
				TEST("SDIRM", "SDIRM_FREQ", LIMIT(TM::GE, 70.2, TM::LE, 78.2), SDIRM_FREQ[CURRENT_SITE_NUMBER()-1], TM::CONTINUE);
				TEST("SDITP", "SDITP_FREQ", LIMIT(TM::GE, 70.2, TM::LE, 78.2), SDITP_FREQ[CURRENT_SITE_NUMBER()-1], TM::CONTINUE);
				TEST("SDITM", "SDITM_FREQ", LIMIT(TM::GE, 70.2, TM::LE, 78.2), SDITM_FREQ[CURRENT_SITE_NUMBER()-1], TM::CONTINUE);
				TEST("MCKO", "MCKO_FREQ", LIMIT(TM::GE, 70.2, TM::LE, 78.2), MCKO_FREQ[CURRENT_SITE_NUMBER()-1], TM::CONTINUE);
				TEST("UCCDI", "UCCDI_FREQ", LIMIT(TM::GE, 70.2, TM::LE, 78.2), UCCDI_FREQ[CURRENT_SITE_NUMBER()-1], TM::CONTINUE);

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
REGISTER_TESTMETHOD("FREQ_MEASURE_HDSDI_TIA", FREQ_MEASURE_HDSDI_TIA)
;
