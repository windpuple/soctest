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

class RON_BL_VOLTAGE_PMU_MEASURE: public testmethod::TestMethod {
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
		DOUBLE iBASELOOPA, iBASELOOPB;

		int site_num;

		PPMU_SETTING setting;
		PPMU_RELAY relay_on, relay_off;
		PPMU_MEASURE ppmuMeasure;
		PPMU_CLAMP clamp_on, clamp_off;
		TASK_LIST task1;

		ON_FIRST_INVOCATION_BEGIN();
		DISCONNECT();
		CONNECT();


	    ac_relay.pin("@").set("IDLE","OFF");
	    //ac_relay.wait(1.5 ms);
	    ac_relay.execute();

        ac_relay.pin("SENADDR0,SENADDR1,SENADDR2,SENADDR3,SENADDR4,SENADDR5,SENADDR6,COMMMODE, BLEN, INPUTMODE, MUXEN, OSCON, BASELOOPA,BASELOOPB").set("AC","OFF");
	    //ac_relay.wait(1.5 ms);
	    ac_relay.execute();


		// Setups for PPMU

		setting.pin("BASELOOPA,BASELOOPB").iRange(40 mA).min(-1 mV).max(1.0 V).iForce(0.0 A);

		//Relay Setups

		relay_on.pin("BASELOOPA,BASELOOPB").status("PPMU_ON");

		relay_on.wait(1.3 ms);

		relay_off.pin("BASELOOPA,BASELOOPB").status("AC_ON");

		//MeasurEment Setups

		ppmuMeasure.pin("BASELOOPA,BASELOOPB").execMode(TM::PVAL).numberOfSamples(1);

		//Hardware Specific Programing to avoid Hot Switching on the Current Force setup

		//clamp_on.pin("BASELOOPA,BASELOOPB").status("CLAMP_ON").low(0.0 V).high(4.0 V);
		//clamp_off.pin("BASELOOPA,BASELOOPB").status("CLAMP_OFF");

		task1.add(setting).add(relay_on).add(ppmuMeasure).add(relay_off);

		FLUSH();

		//Execute the Task list
		//EXECUTE_TEST();

		Sequencer.stopVector(98).run(TM::NORMAL);

		WAIT_TIME(mWait_time ms);

		task1.execute();

		Sequencer.abort();
		FLUSH();

	    ac_relay.pin("@").set("AC","OFF");
	    //ac_relay.wait(1.5 ms);
	    ac_relay.execute();

		ON_FIRST_INVOCATION_END();

		// Result upload and Datalog
		iBASELOOPA = ppmuMeasure.getValue("BASELOOPA");
		iBASELOOPB = ppmuMeasure.getValue("BASELOOPB");

		cout << "site " << CURRENT_SITE_NUMBER() <<" BASELOOPA value  : " << iBASELOOPA <<endl;
		cout << "site " << CURRENT_SITE_NUMBER() <<" BASELOOPB value  : " << iBASELOOPB <<endl;

		TEST("VOLTAGE_BASELOOPA", "VOLTAGE_BASELOOPA", LIMIT(TM::GT, 0.05 V ,
						TM::LT, 0.45 V ), iBASELOOPA, TM::CONTINUE);
		TEST("VOLTAGE_BASELOOPB", "VOLTAGE_BASELOOPB", LIMIT(TM::GT, 0.05 V ,
								TM::LT, 0.45 V ), iBASELOOPB, TM::CONTINUE);


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
REGISTER_TESTMETHOD("RON_BL_VOLTAGE_PMU_MEASURE", RON_BL_VOLTAGE_PMU_MEASURE);
