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

class SIDD: public testmethod::TestMethod {
protected:
	double mWait_time;
	int mdebug;
protected:
	/**
	 *Initialize the parameter interface to the testflow.
	 *This method is called just once after a testsuite is created.
	 */
	virtual void initialize() {
		//Add your initialization code here
		//Note: Test Method API should not be used in this method!
		addParameter("wait_time", "double", &mWait_time,
				testmethod::TM_PARAMETER_INPUT) .setComment(
				"wait for measurement");
		addParameter("debug", "int", &mdebug, testmethod::TM_PARAMETER_INPUT);
	}

	/**
	 *This test is invoked per site.
	 */
	virtual void run() {
#define uA *1.0e-6
#define UA *1.0e-6
#define mA *1.0e-3
#define MA *1.0e-3
		FLEX_RELAY ac_relay;

		//Add your test code here.
		DOUBLE iVCCD, iVCCA;

		DPS_TASK dps_meas;

		int site_num;

		ON_FIRST_INVOCATION_BEGIN();

				DISCONNECT();
				CONNECT();

				dps_meas.pin("@"); //This function returns the reference to the DPS_PIN_TASK class object that represents the mea
				dps_meas.execMode(TM::PVAL); //This member function selects the execution mode.

				dps_meas.pin("VCCD").min(0.0 A).max(0.01 A);
				dps_meas.pin("VCCA").min(0.0 A).max(0.1 A);

				dps_meas.trigMode(TM::INTERNAL);
				dps_meas.wait(0.05);
				dps_meas.samples(200);

				ac_relay.pin("@").set("IDLE", "OFF");
				ac_relay.wait(1.5 ms );
				ac_relay.execute();

				ac_relay.pin(
						"SENADDR0,SENADDR1,SENADDR2,SENADDR3,SENADDR4,SENADDR5,SENADDR6,COMMMODE, BLEN, INPUTMODE, MUXEN, OSCON").set(
						"AC", "OFF");
				//ac_relay.pin("SENGRP").set("AC", "OFF");
				ac_relay.wait(1.5 ms );
				ac_relay.execute();

				FLUSH();

				Sequencer.stopCycle(97).run(); // flush then vector waiting test done.
				WAIT_TIME(mWait_time ms );

				dps_meas.execute();

				//Sequencer.abort();

				Sequencer.reset();

				ac_relay.pin("@").set("AC", "OFF");
				ac_relay.wait(1.5 ms );
				ac_relay.execute();

			ON_FIRST_INVOCATION_END();

		iVCCD = dps_meas.getValue("VCCD"); //retrun A
		iVCCA = dps_meas.getValue("VCCA");

		site_num = CURRENT_SITE_NUMBER();
		if (mdebug) {
			cout << "SITE:" << site_num << "@VCCD:" << iVCCD
					* 1.0e3  << "mA" << endl;
		}
		if (mdebug) {
			cout << "SITE:" << site_num << "@VCCA:" << iVCCA * 1.0e3
					<< "mA" << endl;
		}

		TEST("IDS_VCCD", "IDS_VCCD", LIMIT(TM::GE, -0.1 MA ,
				TM::LE, 10.0 MA ), iVCCD, TM::CONTINUE);
		TEST("IDS_VCCA", "IDS_VCCA", LIMIT(TM::GE, 5.0 MA , TM::LE, 30.0 MA ),
				iVCCA , TM::CONTINUE);

		return;
	}

	/**
	 *This function will be invoked once the specified parameter's value is changed.
	 *@param parameterIdentifier
	 */
	virtual void postParameterChange(const string& parameterIdentifier) {
		//Add your code
		//Note: Test Method API should not be used in this method!
		return;
	}

	/**
	 *This function will be invoked once the Select Test Method Dialog is opened.
	 */
	virtual const string getComment() const {
		string comment = " please add your comment for this method.";
		return comment;
	}
};
REGISTER_TESTMETHOD("SIDD", SIDD)
;
