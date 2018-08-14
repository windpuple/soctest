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

class IS_OFF_CURRENT_PMU_MEASURE: public testmethod::TestMethod {
protected:
	double mWait_time;
	double SEN_LEVEL;
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
		addParameter("SEN_LEVEL", "double", &SEN_LEVEL,
				testmethod::TM_PARAMETER_INPUT) .setComment(
				"SENSOR INPUT LEVEL");
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
		DOUBLE iMUX0OUTA, iMUX1OUTA;

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
	    ac_relay.wait(1.5 ms);
	    ac_relay.execute();

        ac_relay.pin("SENADDR0,SENADDR1,SENADDR2,SENADDR3,SENADDR4,SENADDR5,SENADDR6,COMMMODE, BLEN, INPUTMODE, MUXEN, OSCON, MUX0OUTA, MUX1OUTA").set("AC","OFF");
        ac_relay.pin("SENGRP").set("AC","OFF");
        ac_relay.wait(1.5 ms);
	    ac_relay.execute();


		// Setups for PPMU

		setting.pin("MUX0OUTA, MUX1OUTA").iRange(10 uA).min(-10 uA).max(10 uA).vForce(0.0 V);

		//Relay Setups

		relay_on.pin("MUX0OUTA, MUX1OUTA").status("PPMU_ON");

		relay_on.wait(1.3 ms);

		relay_off.pin("MUX0OUTA, MUX1OUTA").status("AC_ON");

		//MeasurEment Setups

		ppmuMeasure.pin("MUX0OUTA, MUX1OUTA").execMode(TM::PVAL).numberOfSamples(100);

		//Hardware Specific Programing to avoid Hot Switching on the Current Force setup

		clamp_on.pin("MUX0OUTA, MUX1OUTA").status("CLAMP_ON").low(0.0 V).high(4.0 V);
		clamp_off.pin("MUX0OUTA, MUX1OUTA").status("CLAMP_OFF");

		task1.add(setting).add(relay_on).add(ppmuMeasure).add(relay_off).add(clamp_on);

		LEVEL_SPEC spec1(2, 1);

		Primary.level(spec1);

		Primary.getLevelSpec().change("SEN_LEVEL", SEN_LEVEL);

		FLUSH();

		//Execute the Task list
		//EXECUTE_TEST();

		Sequencer.stopCycle(98).run(TM::NORMAL);

		WAIT_TIME(mWait_time ms);

		task1.execute();

		Sequencer.abort();
		FLUSH();

	    ac_relay.pin("@").set("AC","OFF");
	    ac_relay.wait(1.5 ms);
	    ac_relay.execute();

		ON_FIRST_INVOCATION_END();

		// Result upload and Datalog
		iMUX0OUTA = ppmuMeasure.getValue("MUX0OUTA");
		iMUX1OUTA = ppmuMeasure.getValue("MUX1OUTA");

		cout << "site " << CURRENT_SITE_NUMBER() <<" IS OFF MUX0OUTA value  : " << iMUX0OUTA <<endl;
		cout << "site " << CURRENT_SITE_NUMBER() <<" IS OFF MUX1OUTA value  : " << iMUX1OUTA <<endl;

		TEST("VOLTAGE_MUX0OUTA", "VOLTAGE_MUX0OUTA", LIMIT(TM::GT, -1.0 uA ,
						TM::LT, 0.0 uA ), iMUX0OUTA, TM::CONTINUE);
		TEST("VOLTAGE_MUX1OUTA", "VOLTAGE_MUX1OUTA", LIMIT(TM::GT, -1.0 uA ,
								TM::LT, 0.0 uA ), iMUX1OUTA, TM::CONTINUE);


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
REGISTER_TESTMETHOD("IS_OFF_CURRENT_PMU_MEASURE", IS_OFF_CURRENT_PMU_MEASURE);
