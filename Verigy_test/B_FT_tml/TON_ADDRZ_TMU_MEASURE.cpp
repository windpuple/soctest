#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"

#include <iostream>
#include <string>

using namespace std;

#define MAX_SITES 4
#define CAPTURE_SAMPLE 10000

class TON_ADDRZ_TMU_MEASURE: public testmethod::TestMethod {
protected:
	double mtreshold_a;
	double mtreshold_b;
	int mPrescaler;
	int mSamples;
	int mDebug_Print;

protected:

	virtual void initialize() {

		addParameter("treshold_a", "double", &mtreshold_a,
				testmethod::TM_PARAMETER_INPUT) .setDefault("0.0");
		addParameter("treshold_b", "double", &mtreshold_b,
				testmethod::TM_PARAMETER_INPUT) .setDefault("0.0");
		addParameter("Prescaler", "int", &mPrescaler,
				testmethod::TM_PARAMETER_INPUT) .setDefault("1");
		addParameter("Samples", "int", &mSamples,
				testmethod::TM_PARAMETER_INPUT) .setDefault("1000");
		addParameter("Debug_Print", "int", &mDebug_Print,
				testmethod::TM_PARAMETER_INPUT) .setDefault("1");
	}

	string DecimalToBinaryString(int a) {
		string binary = "";
		int mask = 1;
		for (int i = 0; i < 31; i++) {
			if ((mask & a) != 0)
				binary = "1" + binary;
			else
				binary = "0" + binary;
			mask <<= 1;
		}
		//cout << binary << endl;
		return binary;
	}

	virtual void run() {

		TMU_TASK task1, task2, task3;
		FLEX_RELAY ac_relay;
		FLEX_RELAY relay;

		string TMUGROUP[2] = { "MUX0OUTB", "SENADDR0" };

		TMU_RESULTS tmuResult1[2], tmuResult2[2], tmuResult3[2];
		ARRAY_D value1[2], value2[2], value3[2];
		DOUBLE meanvalue0A[98], meanvalue0B[98], meanvalue0C[98];

		ON_FIRST_INVOCATION_BEGIN();
				DISCONNECT();

				CONNECT();

				relay.util("MUX0_OUT_K5").on();
				relay.wait(1.5 ms );
				relay.execute();

				ac_relay.pin("@").set("IDLE", "OFF");
				ac_relay.wait(1.5 ms );
				ac_relay.execute();

				ac_relay.pin(
						"SENADDR0,SENADDR1,SENADDR2,SENADDR3,SENADDR4,SENADDR5,SENADDR6,COMMMODE, BLEN, INPUTMODE, MUXEN, OSCON, MUX0OUTB").set(
						"AC", "OFF");
				ac_relay.pin("SENGRP").set("AC", "OFF");
				ac_relay.wait(1.5 ms );
				ac_relay.execute();

				string vector_lable = Primary.getLabel();
				if (mDebug_Print) {
					cout << "primary vector_lable = " << vector_lable << endl;
				}

				//FOR_EACH_SITE_BEGIN();

				double datarate = Primary.getTimingSpec().getSpecValue(
						"per_3250") * 1e-9;

				//cout << "datarate " << datarate << endl;

				//task1.pin("TMUPIN").setDatarate(1/(datarate*2)).setEdgeSelect(TMU::FALL).setNumSamples(mSamples).setNumMeasurements(1).setNumShots(1);
				task1.pin("SENADDR0").setEdgeSelect(TMU::FALL).setNumSamples(
						mSamples).setNumMeasurements(1).setNumShots(1);
				task1.pin("SENADDR0").setDatarate(1.0 / (datarate * 2));
				task1.pin("SENADDR0").setAThreshold(mtreshold_a);

				task1.pin("MUX0OUTB").setEdgeSelect(TMU::RISE).setNumSamples(
						mSamples).setNumMeasurements(1).setNumShots(1);
				task1.pin("MUX0OUTB").setDatarate(1.0 / (datarate * 2));
				task1.pin("MUX0OUTB").setAThreshold(mtreshold_b);

				LEVEL_SPEC spec1(2, 1);

				Primary.level(spec1);

				Primary.getLevelSpec().change("SEN_LEVEL", 3.3);

				FLUSH();

				task1.setAsPrimary();

				task1.setup();

				FUNCTIONAL_TEST();

				//FOR_EACH_SITE_END();

				ac_relay.pin("@").set("AC", "OFF");
				ac_relay.wait(1.5 ms );
				ac_relay.execute();

				relay.util("MUX0_OUT_K5").off();
				relay.wait(1.5 ms );
				relay.execute();

			ON_FIRST_INVOCATION_END();

		for (int i = 0; i < 2; i++) {

			task1.pin(TMUGROUP[i]).uploadRawResults(tmuResult1[i]);

		}

		for (int i = 0; i < 2; i++) {

			meanvalue0A[i] = 0.0;

		}

		for (int i = 0; i < 2; i++) {

			tmuResult1[i].getPinMeasuredResults(TMUGROUP[i], value1[i],
					TMU::APP_RAW);

		}

		if (mDebug_Print) {
			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < mSamples; j++) {

					printf("value1%d %d: %.20f\n", i, j, value1[i][j]);

				}

				PUT_DEBUG("value1", "value1 raw data", value1[i]);

			}

		}

		for (int j = 0; j < mSamples; j++) {

			double subvalue0 = (value1[0][j] - value1[1][j]);

			if (subvalue0 < 15 * 1e-9 || subvalue0 > 100 * 1e-9) {

				subvalue0 = 0.0;

				meanvalue0A[j] = subvalue0;

			} else {

				meanvalue0A[j] = subvalue0;

			}

		}

		/// SECOND TEST


		ON_FIRST_INVOCATION_BEGIN();
				DISCONNECT();

				CONNECT();

				relay.util("MUX0_OUT_K5").on();
				relay.wait(1.5 ms );
				relay.execute();

				ac_relay.pin("@").set("IDLE", "OFF");
				ac_relay.wait(1.5 ms );
				ac_relay.execute();

				ac_relay.pin(
						"SENADDR0,SENADDR1,SENADDR2,SENADDR3,SENADDR4,SENADDR5,SENADDR6,COMMMODE, BLEN, INPUTMODE, MUXEN, OSCON, MUX0OUTB").set(
						"AC", "OFF");
				ac_relay.pin("SENGRP").set("AC", "OFF");
				ac_relay.wait(1.5 ms );
				ac_relay.execute();

				string vector_lable = Primary.getLabel();
				if (mDebug_Print) {
					cout << "primary vector_lable = " << vector_lable << endl;
				}

				//FOR_EACH_SITE_BEGIN();

				double datarate = Primary.getTimingSpec().getSpecValue(
						"per_3250") * 1e-9;

				//cout << "datarate " << datarate << endl;

				//task2.pin("TMUPIN").setDatarate(1/(datarate*2)).setEdgeSelect(TMU::FALL).setNumSamples(mSamples).setNumMeasurements(1).setNumShots(1);
				task2.pin("SENADDR0").setEdgeSelect(TMU::FALL).setNumSamples(
						mSamples).setNumMeasurements(1).setNumShots(1);
				task2.pin("SENADDR0").setDatarate(1.0 / (datarate * 2));
				task2.pin("SENADDR0").setAThreshold(mtreshold_a);

				task2.pin("MUX0OUTB").setEdgeSelect(TMU::RISE).setNumSamples(
						mSamples).setNumMeasurements(1).setNumShots(1);
				task2.pin("MUX0OUTB").setDatarate(1.0 / (datarate * 2));
				task2.pin("MUX0OUTB").setAThreshold(mtreshold_b);

				LEVEL_SPEC spec1(2, 1);

				Primary.level(spec1);

				Primary.getLevelSpec().change("SEN_LEVEL", 3.3);

				FLUSH();

				task2.setAsPrimary();

				task2.setup();

				FUNCTIONAL_TEST();

				//FOR_EACH_SITE_END();

				ac_relay.pin("@").set("AC", "OFF");
				ac_relay.wait(1.5 ms );
				ac_relay.execute();

				relay.util("MUX0_OUT_K5").off();
				relay.wait(1.5 ms );
				relay.execute();

			ON_FIRST_INVOCATION_END();

		for (int i = 0; i < 2; i++) {

			task2.pin(TMUGROUP[i]).uploadRawResults(tmuResult2[i]);

		}

		for (int i = 0; i < 2; i++) {

			meanvalue0B[i] = 0.0;

		}

		for (int i = 0; i < 2; i++) {

			tmuResult2[i].getPinMeasuredResults(TMUGROUP[i], value2[i],
					TMU::APP_RAW);

		}

		if (mDebug_Print) {
			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < mSamples; j++) {

					printf("value2%d %d: %.20f\n", i, j, value2[i][j]);

				}

				PUT_DEBUG("value2", "value2 raw data", value2[i]);

			}

		}

		for (int j = 0; j < mSamples; j++) {

			double subvalue0 = (value2[0][j] - value2[1][j]);

			if (subvalue0 < 15 * 1e-9 || subvalue0 > 100 * 1e-9) {

				subvalue0 = 0.0;

				meanvalue0B[j] = subvalue0;

			} else {

				meanvalue0B[j] = subvalue0;

			}

		}

		/// THIRD TEST


		ON_FIRST_INVOCATION_BEGIN();
				DISCONNECT();

				CONNECT();

				relay.util("MUX0_OUT_K5").on();
				relay.wait(1.5 ms );
				relay.execute();

				ac_relay.pin("@").set("IDLE", "OFF");
				ac_relay.wait(1.5 ms );
				ac_relay.execute();

				ac_relay.pin(
						"SENADDR0,SENADDR1,SENADDR2,SENADDR3,SENADDR4,SENADDR5,SENADDR6,COMMMODE, BLEN, INPUTMODE, MUXEN, OSCON, MUX0OUTB").set(
						"AC", "OFF");
				ac_relay.pin("SENGRP").set("AC", "OFF");
				ac_relay.wait(1.5 ms );
				ac_relay.execute();

				string vector_lable = Primary.getLabel();
				if (mDebug_Print) {
					cout << "primary vector_lable = " << vector_lable << endl;
				}

				//FOR_EACH_SITE_BEGIN();

				double datarate = Primary.getTimingSpec().getSpecValue(
						"per_3250") * 1e-9;

				//cout << "datarate " << datarate << endl;

				//task3.pin("TMUPIN").setDatarate(1/(datarate*2)).setEdgeSelect(TMU::FALL).setNumSamples(mSamples).setNumMeasurements(1).setNumShots(1);
				task3.pin("SENADDR0").setEdgeSelect(TMU::FALL).setNumSamples(
						mSamples).setNumMeasurements(1).setNumShots(1);
				task3.pin("SENADDR0").setDatarate(1.0 / (datarate * 2));
				task3.pin("SENADDR0").setAThreshold(mtreshold_a);

				task3.pin("MUX0OUTB").setEdgeSelect(TMU::RISE).setNumSamples(
						mSamples).setNumMeasurements(1).setNumShots(1);
				task3.pin("MUX0OUTB").setDatarate(1.0 / (datarate * 2));
				task3.pin("MUX0OUTB").setAThreshold(mtreshold_b);

				LEVEL_SPEC spec1(2, 1);

				Primary.level(spec1);

				Primary.getLevelSpec().change("SEN_LEVEL", 3.3);

				FLUSH();

				task3.setAsPrimary();

				task3.setup();

				FUNCTIONAL_TEST();

				//FOR_EACH_SITE_END();

				ac_relay.pin("@").set("AC", "OFF");
				ac_relay.wait(1.5 ms );
				ac_relay.execute();

				relay.util("MUX0_OUT_K5").off();
				relay.wait(1.5 ms );
				relay.execute();

			ON_FIRST_INVOCATION_END();

		for (int i = 0; i < 2; i++) {

			task3.pin(TMUGROUP[i]).uploadRawResults(tmuResult3[i]);

		}

		for (int i = 0; i < 2; i++) {

			meanvalue0C[i] = 0.0;

		}

		for (int i = 0; i < 2; i++) {

			tmuResult3[i].getPinMeasuredResults(TMUGROUP[i], value3[i],
					TMU::APP_RAW);

		}

		if (mDebug_Print) {
			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < mSamples; j++) {

					printf("value3%d %d: %.20f\n", i, j, value3[i][j]);

				}

				PUT_DEBUG("value3", "value3 raw data", value3[i]);

			}

		}

		for (int j = 0; j < mSamples; j++) {

			double subvalue0 = (value3[0][j] - value3[1][j]);

			if (subvalue0 < 15 * 1e-9 || subvalue0 > 100 * 1e-9) {

				subvalue0 = 0.0;

				meanvalue0C[j] = subvalue0;

			} else {

				meanvalue0C[j] = subvalue0;

			}

		}


		double meavalue0_final[98];

						for (int i = 0; i < mSamples; i++) {

							cout << "meanvalue0A[" << i << "] : " << meanvalue0A[i] << endl;
							cout << "meanvalue0B[" << i << "] : " << meanvalue0B[i] << endl;
							cout << "meanvalue0C[" << i << "] : " << meanvalue0C[i] << endl;



							if (meanvalue0A[i] > 0.0) {

								meavalue0_final[i] = meanvalue0A[i];

							} else if (meanvalue0B[i] > 0.0) {

								meavalue0_final[i] = meanvalue0B[i];

							} else if (meanvalue0C[i] > 0.0) {

								meavalue0_final[i] = meanvalue0C[i];

							} else {

								meavalue0_final[i] = 0.0;

							}


						}





		//determine DUT data rate received

		for (int j = 0; j < 98; j++) {
			//for (int j = 0; j < 10; j++) {

			cout << " TON ADDRZ Code " << j
					<< " MUX0OUT value (via measured value) = "
					<< meavalue0_final[j] * 1e9 << " NS" << endl;

			TEST(TMUGROUP[0], "TON ADDRZ MUX0 Test", LIMIT(TM::GE, 0.0, TM::LE,
					10.0), meavalue0_final[j] * 1e9, TM::CONTINUE);

		}

		return;
	}

	virtual void postParameterChange(const string& parameterIdentifier) {

		return;
	}

	virtual const string getComment() const {
		string comment = " please add your comment for this method.";
		return comment;
	}
};
REGISTER_TESTMETHOD("TON_ADDRZ_TMU_MEASURE", TON_ADDRZ_TMU_MEASURE)
;
