#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"

#include <iostream>
#include <string>

using namespace std;

#define MAX_SITES 4
#define CAPTURE_SAMPLE 10000

class TON_ENZ_TMU_MEASURE: public testmethod::TestMethod {
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

		string TMUGROUP[2] = { "MUX0OUTB", "MUXEN" };

		VEC_LABEL_EDIT
				vector_label(Primary.getLabel(),
						"SENADDR6, SENADDR5, SENADDR4, SENADDR3, SENADDR2, SENADDR1, SENADDR0");

		/* another way vector write
		 VEC_LABEL_EDIT vector_label_SENADDR6(Primary.getLabel(), "SENADDR6");
		 VEC_LABEL_EDIT vector_label_SENADDR5(Primary.getLabel(), "SENADDR5");
		 VEC_LABEL_EDIT vector_label_SENADDR4(Primary.getLabel(), "SENADDR4");
		 VEC_LABEL_EDIT vector_label_SENADDR3(Primary.getLabel(), "SENADDR3");
		 VEC_LABEL_EDIT vector_label_SENADDR2(Primary.getLabel(), "SENADDR2");
		 VEC_LABEL_EDIT vector_label_SENADDR1(Primary.getLabel(), "SENADDR1");
		 VEC_LABEL_EDIT vector_label_SENADDR0(Primary.getLabel(), "SENADDR0");
		 */

		VECTOR_DATA vec_data0[99];
		VECTOR_DATA vec_data1[99];
		VECTOR_DATA vec_data2[99];
		VECTOR_DATA vec_data3[99];
		VECTOR_DATA vec_data4[99];
		VECTOR_DATA vec_data5[99];
		VECTOR_DATA vec_data6[99];

		/* another way vector write
		 vector_label_SENADDR6.downloadUserVectors(vec_data[0], 98);
		 vector_label_SENADDR5.downloadUserVectors(vec_data[1], 98);
		 vector_label_SENADDR4.downloadUserVectors(vec_data[2], 98);
		 vector_label_SENADDR3.downloadUserVectors(vec_data[3], 98);
		 vector_label_SENADDR2.downloadUserVectors(vec_data[4], 98);
		 vector_label_SENADDR1.downloadUserVectors(vec_data[5], 98);
		 vector_label_SENADDR0.downloadUserVectors(vec_data[6], 98);
		 */

		TMU_RESULTS tmuResult1[2], tmuResult2[2], tmuResult3[2];
		ARRAY_D value1[2], value2[2], value3[2];
		DOUBLE meanvalue0A[2], meanvalue0B[2], meanvalue0C[2];

		int k = 0;

		for (k = 0; k < 98; k++) {
			//for (k = 0; k < 2; k++) {

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
						cout << "primary vector_lable = " << vector_lable
								<< endl;
					}

					//FOR_EACH_SITE_BEGIN();

					string AddressCode = DecimalToBinaryString(k);

					AddressCode = AddressCode.substr(24);

					//cout << "Addresscode :" << AddressCode << endl;

					for (int j = 0; j < 99; j++) {

						if (j == 98) {

							vec_data0[j].phyWvfIndex = -1;
							vec_data0[j].vectorNum = -1;
							vec_data1[j].phyWvfIndex = -1;
							vec_data1[j].vectorNum = -1;
							vec_data2[j].phyWvfIndex = -1;
							vec_data2[j].vectorNum = -1;
							vec_data3[j].phyWvfIndex = -1;
							vec_data3[j].vectorNum = -1;
							vec_data4[j].phyWvfIndex = -1;
							vec_data4[j].vectorNum = -1;
							vec_data5[j].phyWvfIndex = -1;
							vec_data5[j].vectorNum = -1;
							vec_data6[j].phyWvfIndex = -1;
							vec_data6[j].vectorNum = -1;

							//cout << "Addresscode " << j << ":" << (int) (AddressCode.at(0)
							//		- '0') << (int) (AddressCode.at(1) - '0')
							//<< (int) (AddressCode.at(2) - '0')
							//<< (int) (AddressCode.at(3) - '0')
							//<< (int) (AddressCode.at(4) - '0')
							//<< (int) (AddressCode.at(5) - '0')
							//<< (int) (AddressCode.at(6) - '0') << endl;

							//cout << "vec_data" << j << "phyWvfIndex :"
							//<< vec_data0[j].phyWvfIndex << vec_data1[j].phyWvfIndex
							//<< vec_data2[j].phyWvfIndex << vec_data3[j].phyWvfIndex
							//<< vec_data4[j].phyWvfIndex << vec_data5[j].phyWvfIndex
							//<< vec_data6[j].phyWvfIndex << endl;

							//cout << "vec_data" << j << "vectorNum :"
							//<< vec_data0[j].vectorNum << vec_data1[j].vectorNum
							//<< vec_data2[j].vectorNum << vec_data3[j].vectorNum
							//<< vec_data4[j].vectorNum << vec_data5[j].vectorNum
							//<< vec_data6[j].vectorNum << endl;

						} else {

							vec_data0[j].phyWvfIndex = (int) (AddressCode.at(0)
									- '0');
							vec_data0[j].vectorNum = j;
							vec_data1[j].phyWvfIndex = (int) (AddressCode.at(1)
									- '0');
							vec_data1[j].vectorNum = j;
							vec_data2[j].phyWvfIndex = (int) (AddressCode.at(2)
									- '0');
							vec_data2[j].vectorNum = j;
							vec_data3[j].phyWvfIndex = (int) (AddressCode.at(3)
									- '0');
							vec_data3[j].vectorNum = j;
							vec_data4[j].phyWvfIndex = (int) (AddressCode.at(4)
									- '0');
							vec_data4[j].vectorNum = j;
							vec_data5[j].phyWvfIndex = (int) (AddressCode.at(5)
									- '0');
							vec_data5[j].vectorNum = j;
							vec_data6[j].phyWvfIndex = (int) (AddressCode.at(6)
									- '0');
							vec_data6[j].vectorNum = j;

							//cout << "Addresscode " << j << ":" << (int) (AddressCode.at(0)
							//		- '0') << (int) (AddressCode.at(1) - '0')
							//<< (int) (AddressCode.at(2) - '0')
							//<< (int) (AddressCode.at(3) - '0')
							//<< (int) (AddressCode.at(4) - '0')
							//<< (int) (AddressCode.at(5) - '0')
							//<< (int) (AddressCode.at(6) - '0') << endl;

							//cout << "vec_data" << j << "phyWvfIndex :"
							//<< vec_data0[j].phyWvfIndex << vec_data1[j].phyWvfIndex
							//<< vec_data2[j].phyWvfIndex << vec_data3[j].phyWvfIndex
							//<< vec_data4[j].phyWvfIndex << vec_data5[j].phyWvfIndex
							//<< vec_data6[j].phyWvfIndex << endl;

							//cout << "vec_data" << j << "vectorNum :"
							//<< vec_data0[j].vectorNum << vec_data1[j].vectorNum
							//<< vec_data2[j].vectorNum << vec_data3[j].vectorNum
							//<< vec_data4[j].vectorNum << vec_data5[j].vectorNum
							//<< vec_data6[j].vectorNum << endl;
						}
					}

					vector_label.downloadUserVectors(
							"SENADDR6, SENADDR5, SENADDR4, SENADDR3, SENADDR2, SENADDR1, SENADDR0",
							vec_data0, vec_data1, vec_data2, vec_data3,
							vec_data4, vec_data5, vec_data6);

					double datarate = Primary.getTimingSpec().getSpecValue(
							"per_3250") * 1e-9;

					//cout << "datarate " << datarate << endl;

					//task1.pin("TMUPIN").setDatarate(1/(datarate*2)).setEdgeSelect(TMU::FALL).setNumSamples(mSamples).setNumMeasurements(1).setNumShots(1);
					task1.pin("MUXEN").setEdgeSelect(TMU::FALL).setNumSamples(
							mSamples).setNumMeasurements(1).setNumShots(1);
					task1.pin("MUXEN").setDatarate(1.0 / (datarate * 5));
					task1.pin("MUXEN").setAThreshold(mtreshold_a);

					task1.pin("MUX0OUTB").setEdgeSelect(TMU::RISE).setNumSamples(
							mSamples).setNumMeasurements(1).setNumShots(1);
					task1.pin("MUX0OUTB").setDatarate(1.0 / (datarate * 5));
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

			int recount0 = mSamples;

			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < mSamples; j++) {

					double subvalue0 = value1[0][j] - value1[i][j];

					if (subvalue0 < 15 * 1e-9 || subvalue0 > 100 * 1e-9) {

						subvalue0 = 0.0;

						meanvalue0A[i] = meanvalue0A[i] + subvalue0;

						recount0 = recount0 - 1;
					} else {

						meanvalue0A[i] = meanvalue0A[i] + subvalue0;

					}

					//cout << "meanvalue0A[i]" <<i<<":"<<"value1[0][j] - value1[i][j]:"<<value1[0][j] - value1[i][j] << endl;

				}

				meanvalue0A[i] = meanvalue0A[i] / recount0;

				//cout << "meanvalue0A[i] :"<<i<<":"<< meanvalue0A[i]<<endl;

				recount0 = mSamples;

			}

			/////////////SECOND TEST

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
						cout << "primary vector_lable = " << vector_lable
								<< endl;
					}

					//FOR_EACH_SITE_BEGIN();

					string AddressCode = DecimalToBinaryString(k);

					AddressCode = AddressCode.substr(24);

					//cout << "Addresscode :" << AddressCode << endl;

					for (int j = 0; j < 99; j++) {

						if (j == 98) {

							vec_data0[j].phyWvfIndex = -1;
							vec_data0[j].vectorNum = -1;
							vec_data1[j].phyWvfIndex = -1;
							vec_data1[j].vectorNum = -1;
							vec_data2[j].phyWvfIndex = -1;
							vec_data2[j].vectorNum = -1;
							vec_data3[j].phyWvfIndex = -1;
							vec_data3[j].vectorNum = -1;
							vec_data4[j].phyWvfIndex = -1;
							vec_data4[j].vectorNum = -1;
							vec_data5[j].phyWvfIndex = -1;
							vec_data5[j].vectorNum = -1;
							vec_data6[j].phyWvfIndex = -1;
							vec_data6[j].vectorNum = -1;

							//cout << "Addresscode " << j << ":" << (int) (AddressCode.at(0)
							//		- '0') << (int) (AddressCode.at(1) - '0')
							//<< (int) (AddressCode.at(2) - '0')
							//<< (int) (AddressCode.at(3) - '0')
							//<< (int) (AddressCode.at(4) - '0')
							//<< (int) (AddressCode.at(5) - '0')
							//<< (int) (AddressCode.at(6) - '0') << endl;

							//cout << "vec_data" << j << "phyWvfIndex :"
							//<< vec_data0[j].phyWvfIndex << vec_data1[j].phyWvfIndex
							//<< vec_data2[j].phyWvfIndex << vec_data3[j].phyWvfIndex
							//<< vec_data4[j].phyWvfIndex << vec_data5[j].phyWvfIndex
							//<< vec_data6[j].phyWvfIndex << endl;

							//cout << "vec_data" << j << "vectorNum :"
							//<< vec_data0[j].vectorNum << vec_data1[j].vectorNum
							//<< vec_data2[j].vectorNum << vec_data3[j].vectorNum
							//<< vec_data4[j].vectorNum << vec_data5[j].vectorNum
							//<< vec_data6[j].vectorNum << endl;

						} else {

							vec_data0[j].phyWvfIndex = (int) (AddressCode.at(0)
									- '0');
							vec_data0[j].vectorNum = j;
							vec_data1[j].phyWvfIndex = (int) (AddressCode.at(1)
									- '0');
							vec_data1[j].vectorNum = j;
							vec_data2[j].phyWvfIndex = (int) (AddressCode.at(2)
									- '0');
							vec_data2[j].vectorNum = j;
							vec_data3[j].phyWvfIndex = (int) (AddressCode.at(3)
									- '0');
							vec_data3[j].vectorNum = j;
							vec_data4[j].phyWvfIndex = (int) (AddressCode.at(4)
									- '0');
							vec_data4[j].vectorNum = j;
							vec_data5[j].phyWvfIndex = (int) (AddressCode.at(5)
									- '0');
							vec_data5[j].vectorNum = j;
							vec_data6[j].phyWvfIndex = (int) (AddressCode.at(6)
									- '0');
							vec_data6[j].vectorNum = j;

							//cout << "Addresscode " << j << ":" << (int) (AddressCode.at(0)
							//		- '0') << (int) (AddressCode.at(1) - '0')
							//<< (int) (AddressCode.at(2) - '0')
							//<< (int) (AddressCode.at(3) - '0')
							//<< (int) (AddressCode.at(4) - '0')
							//<< (int) (AddressCode.at(5) - '0')
							//<< (int) (AddressCode.at(6) - '0') << endl;

							//cout << "vec_data" << j << "phyWvfIndex :"
							//<< vec_data0[j].phyWvfIndex << vec_data1[j].phyWvfIndex
							//<< vec_data2[j].phyWvfIndex << vec_data3[j].phyWvfIndex
							//<< vec_data4[j].phyWvfIndex << vec_data5[j].phyWvfIndex
							//<< vec_data6[j].phyWvfIndex << endl;

							//cout << "vec_data" << j << "vectorNum :"
							//<< vec_data0[j].vectorNum << vec_data1[j].vectorNum
							//<< vec_data2[j].vectorNum << vec_data3[j].vectorNum
							//<< vec_data4[j].vectorNum << vec_data5[j].vectorNum
							//<< vec_data6[j].vectorNum << endl;
						}
					}

					vector_label.downloadUserVectors(
							"SENADDR6, SENADDR5, SENADDR4, SENADDR3, SENADDR2, SENADDR1, SENADDR0",
							vec_data0, vec_data1, vec_data2, vec_data3,
							vec_data4, vec_data5, vec_data6);

					double datarate = Primary.getTimingSpec().getSpecValue(
							"per_3250") * 1e-9;

					//cout << "datarate " << datarate << endl;

					//task2.pin("TMUPIN").setDatarate(1/(datarate*2)).setEdgeSelect(TMU::FALL).setNumSamples(mSamples).setNumMeasurements(1).setNumShots(1);
					task2.pin("MUXEN").setEdgeSelect(TMU::FALL).setNumSamples(
							mSamples).setNumMeasurements(1).setNumShots(1);
					task2.pin("MUXEN").setDatarate(1.0 / (datarate * 5));
					task2.pin("MUXEN").setAThreshold(mtreshold_a);

					task2.pin("MUX0OUTB").setEdgeSelect(TMU::RISE).setNumSamples(
							mSamples).setNumMeasurements(1).setNumShots(1);
					task2.pin("MUX0OUTB").setDatarate(1.0 / (datarate * 5));
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

			recount0 = mSamples;

			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < mSamples; j++) {

					double subvalue0 = value2[0][j] - value2[i][j];

					if (subvalue0 < 15 * 1e-9 || subvalue0 > 100 * 1e-9) {

						subvalue0 = 0.0;

						meanvalue0B[i] = meanvalue0B[i] + subvalue0;

						recount0 = recount0 - 1;
					} else {

						meanvalue0B[i] = meanvalue0B[i] + subvalue0;

					}

					//cout << "meanvalue0B[i]" <<i<<":"<<"value2[0][j] - value2[i][j]:"<<value2[0][j] - value2[i][j] << endl;

				}

				meanvalue0B[i] = meanvalue0B[i] / recount0;

				//cout << "meanvalue0B[i] :"<<i<<":"<< meanvalue0B[i]<<endl;

				recount0 = mSamples;

			}


			/////THIRD test


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
						cout << "primary vector_lable = " << vector_lable
								<< endl;
					}

					//FOR_EACH_SITE_BEGIN();

					string AddressCode = DecimalToBinaryString(k);

					AddressCode = AddressCode.substr(24);

					//cout << "Addresscode :" << AddressCode << endl;

					for (int j = 0; j < 99; j++) {

						if (j == 98) {

							vec_data0[j].phyWvfIndex = -1;
							vec_data0[j].vectorNum = -1;
							vec_data1[j].phyWvfIndex = -1;
							vec_data1[j].vectorNum = -1;
							vec_data2[j].phyWvfIndex = -1;
							vec_data2[j].vectorNum = -1;
							vec_data3[j].phyWvfIndex = -1;
							vec_data3[j].vectorNum = -1;
							vec_data4[j].phyWvfIndex = -1;
							vec_data4[j].vectorNum = -1;
							vec_data5[j].phyWvfIndex = -1;
							vec_data5[j].vectorNum = -1;
							vec_data6[j].phyWvfIndex = -1;
							vec_data6[j].vectorNum = -1;

							//cout << "Addresscode " << j << ":" << (int) (AddressCode.at(0)
							//		- '0') << (int) (AddressCode.at(1) - '0')
							//<< (int) (AddressCode.at(2) - '0')
							//<< (int) (AddressCode.at(3) - '0')
							//<< (int) (AddressCode.at(4) - '0')
							//<< (int) (AddressCode.at(5) - '0')
							//<< (int) (AddressCode.at(6) - '0') << endl;

							//cout << "vec_data" << j << "phyWvfIndex :"
							//<< vec_data0[j].phyWvfIndex << vec_data1[j].phyWvfIndex
							//<< vec_data2[j].phyWvfIndex << vec_data3[j].phyWvfIndex
							//<< vec_data4[j].phyWvfIndex << vec_data5[j].phyWvfIndex
							//<< vec_data6[j].phyWvfIndex << endl;

							//cout << "vec_data" << j << "vectorNum :"
							//<< vec_data0[j].vectorNum << vec_data1[j].vectorNum
							//<< vec_data2[j].vectorNum << vec_data3[j].vectorNum
							//<< vec_data4[j].vectorNum << vec_data5[j].vectorNum
							//<< vec_data6[j].vectorNum << endl;

						} else {

							vec_data0[j].phyWvfIndex = (int) (AddressCode.at(0)
									- '0');
							vec_data0[j].vectorNum = j;
							vec_data1[j].phyWvfIndex = (int) (AddressCode.at(1)
									- '0');
							vec_data1[j].vectorNum = j;
							vec_data2[j].phyWvfIndex = (int) (AddressCode.at(2)
									- '0');
							vec_data2[j].vectorNum = j;
							vec_data3[j].phyWvfIndex = (int) (AddressCode.at(3)
									- '0');
							vec_data3[j].vectorNum = j;
							vec_data4[j].phyWvfIndex = (int) (AddressCode.at(4)
									- '0');
							vec_data4[j].vectorNum = j;
							vec_data5[j].phyWvfIndex = (int) (AddressCode.at(5)
									- '0');
							vec_data5[j].vectorNum = j;
							vec_data6[j].phyWvfIndex = (int) (AddressCode.at(6)
									- '0');
							vec_data6[j].vectorNum = j;

							//cout << "Addresscode " << j << ":" << (int) (AddressCode.at(0)
							//		- '0') << (int) (AddressCode.at(1) - '0')
							//<< (int) (AddressCode.at(2) - '0')
							//<< (int) (AddressCode.at(3) - '0')
							//<< (int) (AddressCode.at(4) - '0')
							//<< (int) (AddressCode.at(5) - '0')
							//<< (int) (AddressCode.at(6) - '0') << endl;

							//cout << "vec_data" << j << "phyWvfIndex :"
							//<< vec_data0[j].phyWvfIndex << vec_data1[j].phyWvfIndex
							//<< vec_data2[j].phyWvfIndex << vec_data3[j].phyWvfIndex
							//<< vec_data4[j].phyWvfIndex << vec_data5[j].phyWvfIndex
							//<< vec_data6[j].phyWvfIndex << endl;

							//cout << "vec_data" << j << "vectorNum :"
							//<< vec_data0[j].vectorNum << vec_data1[j].vectorNum
							//<< vec_data2[j].vectorNum << vec_data3[j].vectorNum
							//<< vec_data4[j].vectorNum << vec_data5[j].vectorNum
							//<< vec_data6[j].vectorNum << endl;
						}
					}

					vector_label.downloadUserVectors(
							"SENADDR6, SENADDR5, SENADDR4, SENADDR3, SENADDR2, SENADDR1, SENADDR0",
							vec_data0, vec_data1, vec_data2, vec_data3,
							vec_data4, vec_data5, vec_data6);

					double datarate = Primary.getTimingSpec().getSpecValue(
							"per_3250") * 1e-9;

					//cout << "datarate " << datarate << endl;

					//task3.pin("TMUPIN").setDatarate(1/(datarate*2)).setEdgeSelect(TMU::FALL).setNumSamples(mSamples).setNumMeasurements(1).setNumShots(1);
					task3.pin("MUXEN").setEdgeSelect(TMU::FALL).setNumSamples(
							mSamples).setNumMeasurements(1).setNumShots(1);
					task3.pin("MUXEN").setDatarate(1.0 / (datarate * 5));
					task3.pin("MUXEN").setAThreshold(mtreshold_a);

					task3.pin("MUX0OUTB").setEdgeSelect(TMU::RISE).setNumSamples(
							mSamples).setNumMeasurements(1).setNumShots(1);
					task3.pin("MUX0OUTB").setDatarate(1.0 / (datarate * 5));
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

			recount0 = mSamples;

			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < mSamples; j++) {

					double subvalue0 = value3[0][j] - value3[i][j];

					if (subvalue0 < 15 * 1e-9 || subvalue0 > 100 * 1e-9) {

						subvalue0 = 0.0;

						meanvalue0C[i] = meanvalue0C[i] + subvalue0;

						recount0 = recount0 - 1;
					} else {

						meanvalue0C[i] = meanvalue0C[i] + subvalue0;

					}

					//cout << "meanvalue0C[i]" <<i<<":"<<"value3[0][j] - value3[i][j]:"<<value3[0][j] - value3[i][j] << endl;

				}

				meanvalue0C[i] = meanvalue0C[i] / recount0;

				//cout << "meanvalue0C[i] :"<<i<<":"<< meanvalue0C[i]<<endl;

				recount0 = mSamples;

			}


			double meavalue0_final[2];

					for (int i = 0; i < 2; i++) {

						//cout << "meanvalue0A[" << i << "] : " << meanvalue0A[i] << endl;
						//cout << "meanvalue0B[" << i << "] : " << meanvalue0B[i] << endl;
						//cout << "meanvalue0C[" << i << "] : " << meanvalue0C[i] << endl;



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

			cout << " TON ENZ Code " << k
					<< " MUX0OUT value (via measured value) = "
					<< meavalue0_final[1] * 1e9 << " NS" << endl;

			TEST(TMUGROUP[0], "TON ENZ MUX0 Test", LIMIT(TM::GE, 0.0, TM::LE,
					10.0), meavalue0_final[1] * 1e9, TM::CONTINUE);

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
REGISTER_TESTMETHOD("TON_ENZ_TMU_MEASURE", TON_ENZ_TMU_MEASURE)
;
