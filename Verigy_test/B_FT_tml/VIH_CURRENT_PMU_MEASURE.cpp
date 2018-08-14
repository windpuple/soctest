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

class VIH_CURRENT_PMU_MEASURE: public testmethod::TestMethod {
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
		DOUBLE iMUX0OUTA;
		DOUBLE iMUX1OUTA;

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

		setting.pin("MUX0OUTA,MUX1OUTA").iRange(40 mA).min(-5 mA).max(5 mA).vForce(0.0 V);
	    //setting.pin("MUX0OUTA").iRange(10 mA).min(-0.1 V).max(3.6 V).iForce(0.0 A );

		//Relay Setups

		relay_on.pin("MUX0OUTA,MUX1OUTA").status("PPMU_ON");

		relay_on.wait(1.3 ms);

		relay_off.pin("MUX0OUTA,MUX1OUTA").status("AC_ON");

		//MeasurEment Setups

		ppmuMeasure.pin("MUX0OUTA,MUX1OUTA").execMode(TM::PVAL).numberOfSamples(100);

		//Hardware Specific Programing to avoid Hot Switching on the Current Force setup

		clamp_on.pin("MUX0OUTA,MUX1OUTA").status("CLAMP_ON").low(0.0 V).high(4.0 V);
		clamp_off.pin("MUX0OUTA,MUX1OUTA").status("CLAMP_OFF");

		task1.add(setting).add(relay_on).add(ppmuMeasure).add(relay_off).add(clamp_on);

		//Execute the Task list

		//sequncer run dont run flush so before run sequencer must declear FLUSH() for primary set reflect.

		// ON invocation block dont execute number of site. so when u make each site measured data bind some data value, u must
		// USE EACH_SITE_BEGIN API


		//FOR_EACH_SITE_BEGIN();

		LEVEL_SPEC spec1(2, 1);

		Primary.level(spec1);


		////////////////////////////////////////////////////////////
		Primary.getLevelSpec().change("SEN_LEVEL", 0.2);

		FLUSH();

		Sequencer.stopCycle(97).run();

		WAIT_TIME(mWait_time ms);

		task1.execute();


		//cout<<"sequencer status :" << Sequencer.getSequencerStatus() <<endl;
		cout<<"VIH MUX0 value  : " << iMUX0OUTA <<endl;
		//cout<<"VIH MUX1 value  : " << iMUX1OUTA <<endl;
		Sequencer.reset();

		//FOR_EACH_SITE_END();

	    ac_relay.pin("@").set("AC","OFF");
	    ac_relay.wait(1.5 ms);
	    ac_relay.execute();

		ON_FIRST_INVOCATION_END();

		// Result upload and Datalog
			iMUX0OUTA = ppmuMeasure.getValue("MUX0OUTA");
			iMUX1OUTA = ppmuMeasure.getValue("MUX1OUTA");


		site_num = CURRENT_SITE_NUMBER();
		///////////////////////////////////////////
		printf("SENPINS INPUT LEVEL : 200mV \n");
		printf("MUX0OUTA current ADDRESS VIH : %lf \n",iMUX0OUTA);

		TEST("VIH_CURRENT_MUX0OUTA", "VIH_CURRENT_MUX0OUTA", LIMIT(TM::GT, -4.5 MA ,
				TM::LT, -2.22 MA ), iMUX0OUTA, TM::CONTINUE);



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
REGISTER_TESTMETHOD("VIH_CURRENT_PMU_MEASURE", VIH_CURRENT_PMU_MEASURE);
