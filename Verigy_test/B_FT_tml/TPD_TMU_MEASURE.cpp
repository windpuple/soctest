#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"

#include <iostream>
#include <string>

using namespace std;

#define MAX_SITES 4
#define CAPTURE_SAMPLE 10000

class TPD_TMU_MEASURE: public testmethod::TestMethod {
protected:
	double mtreshold_a;
	double mtreshold_b;
	double SEN_LEVEL;
	int mPrescaler;
	int mSamples;
	int mDebug_Print;

protected:

	virtual void initialize() {

		addParameter("treshold_a", "double", &mtreshold_a,
				testmethod::TM_PARAMETER_INPUT) .setDefault("0.0");
		addParameter("treshold_b", "double", &mtreshold_b,
				testmethod::TM_PARAMETER_INPUT) .setDefault("0.0");
		addParameter("SEN_LEVEL", "double", &SEN_LEVEL,
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

		TMU_TASK task1, task2, task3, task4;

		FLEX_RELAY ac_relay;
		FLEX_RELAY relay;

		string TMUGROUP[100] = { "MUX0OUTC", "MUX1OUTB", "SEN00", "SEN01",
				"SEN02", "SEN03", "SEN04", "SEN05", "SEN06", "SEN07", "SEN08",
				"SEN09", "SEN10", "SEN11", "SEN12", "SEN13", "SEN14", "SEN15",
				"SEN16", "SEN17", "SEN18", "SEN19", "SEN20", "SEN21", "SEN22",
				"SEN23", "SEN24", "SEN25", "SEN26", "SEN27", "SEN28", "SEN29",
				"SEN30", "SEN31", "SEN32", "SEN33", "SEN34", "SEN35", "SEN36",
				"SEN37", "SEN38", "SEN39", "SEN40", "SEN41", "SEN42", "SEN43",
				"SEN44", "SEN45", "SEN46", "SEN47", "SEN48", "SEN49", "SEN50",
				"SEN51", "SEN52", "SEN53", "SEN54", "SEN55", "SEN56", "SEN57",
				"SEN58", "SEN59", "SEN60", "SEN61", "SEN62", "SEN63", "SEN64",
				"SEN65", "SEN66", "SEN67", "SEN68", "SEN69", "SEN70", "SEN71",
				"SEN72", "SEN73", "SEN74", "SEN75", "SEN76", "SEN77", "SEN78",
				"SEN79", "SEN80", "SEN81", "SEN82", "SEN83", "SEN84", "SEN85",
				"SEN86", "SEN87", "SEN88", "SEN89", "SEN90", "SEN91", "SEN92",
				"SEN93", "SEN94", "SEN95", "SEN96", "SEN97" };

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

		TMU_RESULTS tmuResult1[100], tmuResult2[100], tmuResult3[100],
				tmuResult4[100];
		ARRAY_D value1[100], value2[100], value3[100], value4[100];
		DOUBLE meanvalue0A[100], meanvalue0B[100], meanvalue0C[100],
				meanvalue0D[100];
		DOUBLE meanvalue1A[100], meanvalue1B[100], meanvalue1C[100],
				meanvalue1D[100];

		int k = 0;

		//for (k = 0; k < 97; k++) {
			for (k = 0; k < 1; k++) {

			ON_FIRST_INVOCATION_BEGIN();
					DISCONNECT();

					CONNECT();

					relay.util("MUX0_OUT_K6,MUX1_OUT_K11").on();
					relay.wait(1.5 ms );
					relay.execute();

					ac_relay.pin("@").set("IDLE", "OFF");
					ac_relay.wait(1.5 ms );
					ac_relay.execute();

					ac_relay.pin(
							"SENADDR0,SENADDR1,SENADDR2,SENADDR3,SENADDR4,SENADDR5,SENADDR6,COMMMODE, BLEN, INPUTMODE, MUXEN, OSCON, MUX0OUTC, MUX1OUTB").set(
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

					LEVEL_SPEC spec1(2, 1);

					Primary.level(spec1);

					////////////////////////////////////////////////////////////
					Primary.getLevelSpec().change("SEN_LEVEL", SEN_LEVEL);

					//task1.pin("TMUPIN").setDatarate(1/(datarate*2)).setEdgeSelect(TMU::FALL).setNumSamples(mSamples).setNumMeasurements(1).setNumShots(1);
					task1.pin("TMU_MUX_EXP").setEdgeSelect(TMU::FALL).setNumSamples(
							mSamples).setNumMeasurements(1).setNumShots(1);
					task1.pin("TMU_MUX_EXP").setDatarate(1.0 / (datarate * 2));
					task1.pin("TMU_MUX_EXP").setAThreshold(mtreshold_a);

					task1.pin("MUX0OUTC,MUX1OUTB").setEdgeSelect(TMU::FALL).setNumSamples(
							mSamples).setNumMeasurements(1).setNumShots(1);
					task1.pin("MUX0OUTC,MUX1OUTB").setDatarate(1.0 / (datarate
							* 2));
					task1.pin("MUX0OUTC,MUX1OUTB").setAThreshold(mtreshold_b);

					FLUSH();

					task1.setAsPrimary();

					task1.setup();

					FUNCTIONAL_TEST();

					ac_relay.pin("@").set("AC", "OFF");
					ac_relay.wait(1.5 ms );
					ac_relay.execute();

					relay.util("MUX0_OUT_K6,MUX1_OUT_K11").off();
					relay.wait(1.5 ms );
					relay.execute();

					// second tmu measure

					DISCONNECT();
					CONNECT();

					relay.util("MUX0_OUT_K6,MUX1_OUT_K11").on();
					relay.wait(1.5 ms );
					relay.execute();

					ac_relay.pin("@").set("IDLE", "OFF");
					ac_relay.wait(1.5 ms );
					ac_relay.execute();

					ac_relay.pin(
							"SENADDR0,SENADDR1,SENADDR2,SENADDR3,SENADDR4,SENADDR5,SENADDR6,COMMMODE, BLEN, INPUTMODE, MUXEN, OSCON, MUX0OUTC, MUX1OUTB").set(
							"AC", "OFF");
					ac_relay.pin("SENGRP").set("AC", "OFF");
					ac_relay.wait(1.5 ms );
					ac_relay.execute();

					task2.pin("TMU_MUX_EXP").setEdgeSelect(TMU::FALL).setNumSamples(
							mSamples).setNumMeasurements(1).setNumShots(1);
					task2.pin("TMU_MUX_EXP").setDatarate(1.0 / (datarate * 2));
					task2.pin("TMU_MUX_EXP").setAThreshold(mtreshold_a);

					task2.pin("MUX0OUTC,MUX1OUTB").setEdgeSelect(TMU::FALL).setNumSamples(
							mSamples).setNumMeasurements(1).setNumShots(1);
					task2.pin("MUX0OUTC,MUX1OUTB").setDatarate(1.0 / (datarate
							* 2));
					task2.pin("MUX0OUTC,MUX1OUTB").setAThreshold(mtreshold_b);

					FLUSH();

					task2.setAsPrimary();

					task2.setup();

					FUNCTIONAL_TEST();

					//FOR_EACH_SITE_END();

					ac_relay.pin("@").set("AC", "OFF");
					ac_relay.wait(1.5 ms );
					ac_relay.execute();

					relay.util("MUX0_OUT_K6,MUX1_OUT_K11").off();
					relay.wait(1.5 ms );
					relay.execute();

					// third tmu measure

					DISCONNECT();
					CONNECT();

					relay.util("MUX0_OUT_K6,MUX1_OUT_K11").on();
					relay.wait(1.5 ms );
					relay.execute();

					ac_relay.pin("@").set("IDLE", "OFF");
					ac_relay.wait(1.5 ms );
					ac_relay.execute();

					ac_relay.pin(
							"SENADDR0,SENADDR1,SENADDR2,SENADDR3,SENADDR4,SENADDR5,SENADDR6,COMMMODE, BLEN, INPUTMODE, MUXEN, OSCON, MUX0OUTC, MUX1OUTB").set(
							"AC", "OFF");
					ac_relay.pin("SENGRP").set("AC", "OFF");
					ac_relay.wait(1.5 ms );
					ac_relay.execute();

					task3.pin("TMU_MUX_EXP").setEdgeSelect(TMU::FALL).setNumSamples(
							mSamples).setNumMeasurements(1).setNumShots(1);
					task3.pin("TMU_MUX_EXP").setDatarate(1.0 / (datarate * 2));
					task3.pin("TMU_MUX_EXP").setAThreshold(mtreshold_a);

					task3.pin("MUX0OUTC,MUX1OUTB").setEdgeSelect(TMU::FALL).setNumSamples(
							mSamples).setNumMeasurements(1).setNumShots(1);
					task3.pin("MUX0OUTC,MUX1OUTB").setDatarate(1.0 / (datarate
							* 2));
					task3.pin("MUX0OUTC,MUX1OUTB").setAThreshold(mtreshold_b);

					FLUSH();

					task3.setAsPrimary();

					task3.setup();

					FUNCTIONAL_TEST();

					//FOR_EACH_SITE_END();

					ac_relay.pin("@").set("AC", "OFF");
					ac_relay.wait(1.5 ms );
					ac_relay.execute();

					relay.util("MUX0_OUT_K6,MUX1_OUT_K11").off();
					relay.wait(1.5 ms );
					relay.execute();

				ON_FIRST_INVOCATION_END();

			for (int i = 0; i < 100; i++) {

				task1.pin(TMUGROUP[i]).uploadRawResults(tmuResult1[i]);

			}

			for (int i = 0; i < 100; i++) {

				meanvalue0A[i] = 0.0;
				meanvalue1A[i] = 0.0;
			}

			for (int i = 0; i < 100; i++) {

				tmuResult1[i].getPinMeasuredResults(TMUGROUP[i], value1[i],
						TMU::APP_RAW);

			}

			if (mDebug_Print) {
				for (int i = 0; i < 100; i++) {
					for (int j = 0; j < mSamples; j++) {

						printf("value1%d %d: %.20f \n", i, j, value1[i][j]);

					}

					PUT_DEBUG("value1", "value1 raw data", value1[i]);

				}

			}

			///// first data sub sequence
			int recount0 = mSamples;
			int recount1 = mSamples;

			for (int i = 0; i < 100; i++) {
				for (int j = 0; j < mSamples; j++) {

					double subvalue0 = value1[0][j] - value1[i][j];
					double subvalue1 = value1[1][j] - value1[i][j];

					if (subvalue0 < 15 * 1e-9 || subvalue0 > 100 * 1e-9) {

						subvalue0 = 0.0;

						meanvalue0A[i] = meanvalue0A[i] + subvalue0;

						recount0 = recount0 - 1;
					} else {

						meanvalue0A[i] = meanvalue0A[i] + subvalue0;

					}

					if (subvalue1 < 15 * 1e-9 || subvalue1 > 100 * 1e-9) {

						subvalue1 = 0.0;
						meanvalue1A[i] = meanvalue1A[i] + subvalue1;

						recount1 = recount1 - 1;

					} else {

						meanvalue1A[i] = meanvalue1A[i] + subvalue1;

					}

					//cout << "meanvalue0A[i]" <<i<<":"<<"value1[0][j] - value1[i][j]:"<<value1[0][j] - value1[i][j] << endl;
					//cout << "meanvalue1A[i]" <<i<<":"<<"value1[1][j] - value1[i][j]:"<<value1[1][j] - value1[i][j] << endl;
				}

				meanvalue0A[i] = meanvalue0A[i] / recount0;
				meanvalue1A[i] = meanvalue1A[i] / recount1;

				//cout << "meanvalue0A[i] :"<<i<<":"<< meanvalue0A[i]<<endl;
				//cout << "meanvalue1A[i] :"<<i<<":"<< meanvalue1A[i]<<endl;

				recount0 = mSamples;
				recount1 = mSamples;
			}

			//////////////// SECOND TEST/////////////////////

			ON_FIRST_INVOCATION_BEGIN();
					DISCONNECT();

					CONNECT();

					relay.util("MUX0_OUT_K6,MUX1_OUT_K11").on();
					relay.wait(1.5 ms );
					relay.execute();

					ac_relay.pin("@").set("IDLE", "OFF");
					ac_relay.wait(1.5 ms );
					ac_relay.execute();

					ac_relay.pin(
							"SENADDR0,SENADDR1,SENADDR2,SENADDR3,SENADDR4,SENADDR5,SENADDR6,COMMMODE, BLEN, INPUTMODE, MUXEN, OSCON, MUX0OUTC, MUX1OUTB").set(
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

					LEVEL_SPEC spec1(2, 1);

					Primary.level(spec1);

					////////////////////////////////////////////////////////////
					Primary.getLevelSpec().change("SEN_LEVEL", SEN_LEVEL);

					task2.pin("TMU_MUX_EXP").setEdgeSelect(TMU::FALL).setNumSamples(
							mSamples).setNumMeasurements(1).setNumShots(1);
					task2.pin("TMU_MUX_EXP").setDatarate(1.0 / (datarate * 2));
					task2.pin("TMU_MUX_EXP").setAThreshold(mtreshold_a);

					task2.pin("MUX0OUTC,MUX1OUTB").setEdgeSelect(TMU::FALL).setNumSamples(
							mSamples).setNumMeasurements(1).setNumShots(1);
					task2.pin("MUX0OUTC,MUX1OUTB").setDatarate(1.0 / (datarate
							* 2));
					task2.pin("MUX0OUTC,MUX1OUTB").setAThreshold(mtreshold_b);

					FLUSH();

					task2.setAsPrimary();

					task2.setup();

					FUNCTIONAL_TEST();

					//FOR_EACH_SITE_END();

					ac_relay.pin("@").set("AC", "OFF");
					ac_relay.wait(1.5 ms );
					ac_relay.execute();

					relay.util("MUX0_OUT_K6,MUX1_OUT_K11").off();
					relay.wait(1.5 ms );
					relay.execute();

				ON_FIRST_INVOCATION_END();

			for (int i = 0; i < 100; i++) {

				task2.pin(TMUGROUP[i]).uploadRawResults(tmuResult2[i]);

			}

			for (int i = 0; i < 100; i++) {

				meanvalue0B[i] = 0.0;
				meanvalue1B[i] = 0.0;

			}

			for (int i = 0; i < 100; i++) {

				tmuResult2[i].getPinMeasuredResults(TMUGROUP[i], value2[i],
						TMU::APP_RAW);

			}

			if (mDebug_Print) {
				for (int i = 0; i < 100; i++) {
					for (int j = 0; j < mSamples; j++) {

						printf("value2%d %d: %.20f\n", i, j, value2[i][j]);

					}

					PUT_DEBUG("value2", "value2 raw data", value2[i]);

				}

			}

			///// second data sub sequence
			recount0 = mSamples;
			recount1 = mSamples;

			for (int i = 0; i < 100; i++) {
				for (int j = 0; j < mSamples; j++) {

					double subvalue0 = value2[0][j] - value2[i][j];
					double subvalue1 = value2[1][j] - value2[i][j];

					if (subvalue0 < 15 * 1e-9 || subvalue0 > 100 * 1e-9) {

						subvalue0 = 0.0;

						meanvalue0B[i] = meanvalue0B[i] + subvalue0;

						recount0 = recount0 - 1;
					} else {

						meanvalue0B[i] = meanvalue0B[i] + subvalue0;

					}

					if (subvalue1 < 15 * 1e-9 || subvalue1 > 100 * 1e-9) {

						subvalue1 = 0.0;
						meanvalue1B[i] = meanvalue1B[i] + subvalue1;

						recount1 = recount1 - 1;

					} else {

						meanvalue1B[i] = meanvalue1B[i] + subvalue1;

					}

					//cout << "meanvalue0B[i]" <<i<<":"<<"value2[0][j] - value2[i][j]:"<<value2[0][j] - value2[i][j] << endl;
					//cout << "meanvalue1B[i]" <<i<<":"<<"value2[1][j] - value2[i][j]:"<<value2[1][j] - value2[i][j] << endl;
				}

				meanvalue0B[i] = meanvalue0B[i] / recount0;
				meanvalue1B[i] = meanvalue1B[i] / recount1;

				//cout << "meanvalue0B[i] :"<<i<<":"<< meanvalue0B[i]<<endl;
				//cout << "meanvalue1B[i] :"<<i<<":"<< meanvalue1B[i]<<endl;

				recount0 = mSamples;
				recount1 = mSamples;
			}

			////////////// THIRD TEST ////////////////////
			ON_FIRST_INVOCATION_BEGIN();
					DISCONNECT();

					CONNECT();

					relay.util("MUX0_OUT_K6,MUX1_OUT_K11").on();
					relay.wait(1.5 ms );
					relay.execute();

					ac_relay.pin("@").set("IDLE", "OFF");
					ac_relay.wait(1.5 ms );
					ac_relay.execute();

					ac_relay.pin(
							"SENADDR0,SENADDR1,SENADDR2,SENADDR3,SENADDR4,SENADDR5,SENADDR6,COMMMODE, BLEN, INPUTMODE, MUXEN, OSCON, MUX0OUTC, MUX1OUTB").set(
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

					LEVEL_SPEC spec1(2, 1);

					Primary.level(spec1);

					////////////////////////////////////////////////////////////
					Primary.getLevelSpec().change("SEN_LEVEL", SEN_LEVEL);

					task3.pin("TMU_MUX_EXP").setEdgeSelect(TMU::FALL).setNumSamples(
							mSamples).setNumMeasurements(1).setNumShots(1);
					task3.pin("TMU_MUX_EXP").setDatarate(1.0 / (datarate * 2));
					task3.pin("TMU_MUX_EXP").setAThreshold(mtreshold_a);

					task3.pin("MUX0OUTC,MUX1OUTB").setEdgeSelect(TMU::FALL).setNumSamples(
							mSamples).setNumMeasurements(1).setNumShots(1);
					task3.pin("MUX0OUTC,MUX1OUTB").setDatarate(1.0 / (datarate
							* 2));
					task3.pin("MUX0OUTC,MUX1OUTB").setAThreshold(mtreshold_b);

					FLUSH();

					task3.setAsPrimary();

					task3.setup();

					FUNCTIONAL_TEST();

					//FOR_EACH_SITE_END();

					ac_relay.pin("@").set("AC", "OFF");
					ac_relay.wait(1.5 ms );
					ac_relay.execute();

					relay.util("MUX0_OUT_K6,MUX1_OUT_K11").off();
					relay.wait(1.5 ms );
					relay.execute();

				ON_FIRST_INVOCATION_END();

			for (int i = 0; i < 100; i++) {

				task3.pin(TMUGROUP[i]).uploadRawResults(tmuResult3[i]);

			}

			for (int i = 0; i < 100; i++) {

				meanvalue0C[i] = 0.0;
				meanvalue1C[i] = 0.0;

			}

			for (int i = 0; i < 100; i++) {

				tmuResult3[i].getPinMeasuredResults(TMUGROUP[i], value3[i],
						TMU::APP_RAW);

			}

			if (mDebug_Print) {
				for (int i = 0; i < 100; i++) {
					for (int j = 0; j < mSamples; j++) {

						printf("value3%d %d: %.20f\n", i, j, value3[i][j]);

					}

					PUT_DEBUG("value3", "value2 raw data", value3[i]);
				}

			}

			///// first data sub sequence

			///// third data sub sequence
			recount0 = mSamples;
			recount1 = mSamples;

			for (int i = 0; i < 100; i++) {
				for (int j = 0; j < mSamples; j++) {

					double subvalue0 = value3[0][j] - value3[i][j];
					double subvalue1 = value3[1][j] - value3[i][j];

					if (subvalue0 < 15 * 1e-9 || subvalue0 > 100 * 1e-9) {

						subvalue0 = 0.0;

						meanvalue0C[i] = meanvalue0C[i] + subvalue0;

						recount0 = recount0 - 1;
					} else {

						meanvalue0C[i] = meanvalue0C[i] + subvalue0;

					}

					if (subvalue1 < 15 * 1e-9 || subvalue1 > 100 * 1e-9) {

						subvalue1 = 0.0;
						meanvalue1C[i] = meanvalue1C[i] + subvalue1;

						recount1 = recount1 - 1;

					} else {

						meanvalue1C[i] = meanvalue1C[i] + subvalue1;

					}

					//cout << "meanvalue0B[i]" <<i<<":"<<"value2[0][j] - value2[i][j]:"<<value2[0][j] - value2[i][j] << endl;
					//cout << "meanvalue1B[i]" <<i<<":"<<"value2[1][j] - value2[i][j]:"<<value2[1][j] - value2[i][j] << endl;
					//cout << "meanvalue2B[i]" <<i<<":"<<"value3[1][j] - value2[i][j]:"<<value3[1][j] - value3[i][j] << endl;
				}

				meanvalue0C[i] = meanvalue0C[i] / recount0;
				meanvalue1C[i] = meanvalue1C[i] / recount1;

				//cout << "meanvalue0B[i] :"<<i<<":"<< meanvalue0B[i]<<endl;
				//cout << "meanvalue1B[i] :"<<i<<":"<< meanvalue1B[i]<<endl;

				recount0 = mSamples;
				recount1 = mSamples;

			}

			////////////// FOURTH TEST ////////////////////
			ON_FIRST_INVOCATION_BEGIN();
					DISCONNECT();

					CONNECT();

					relay.util("MUX0_OUT_K6,MUX1_OUT_K11").on();
					relay.wait(1.5 ms );
					relay.execute();

					ac_relay.pin("@").set("IDLE", "OFF");
					ac_relay.wait(1.5 ms );
					ac_relay.execute();

					ac_relay.pin(
							"SENADDR0,SENADDR1,SENADDR2,SENADDR3,SENADDR4,SENADDR5,SENADDR6,COMMMODE, BLEN, INPUTMODE, MUXEN, OSCON, MUX0OUTC, MUX1OUTB").set(
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

					LEVEL_SPEC spec1(2, 1);

					Primary.level(spec1);

					////////////////////////////////////////////////////////////
					Primary.getLevelSpec().change("SEN_LEVEL", SEN_LEVEL);

					task4.pin("TMU_MUX_EXP").setEdgeSelect(TMU::FALL).setNumSamples(
							mSamples).setNumMeasurements(1).setNumShots(1);
					task4.pin("TMU_MUX_EXP").setDatarate(1.0 / (datarate * 2));
					task4.pin("TMU_MUX_EXP").setAThreshold(mtreshold_a);

					task4.pin("MUX0OUTC,MUX1OUTB").setEdgeSelect(TMU::FALL).setNumSamples(
							mSamples).setNumMeasurements(1).setNumShots(1);
					task4.pin("MUX0OUTC,MUX1OUTB").setDatarate(1.0 / (datarate
							* 2));
					task4.pin("MUX0OUTC,MUX1OUTB").setAThreshold(mtreshold_b);

					FLUSH();

					task4.setAsPrimary();

					task4.setup();

					FUNCTIONAL_TEST();

					//FOR_EACH_SITE_END();

					ac_relay.pin("@").set("AC", "OFF");
					ac_relay.wait(1.5 ms );
					ac_relay.execute();

					relay.util("MUX0_OUT_K6,MUX1_OUT_K11").off();
					relay.wait(1.5 ms );
					relay.execute();

				ON_FIRST_INVOCATION_END();

			for (int i = 0; i < 100; i++) {

				task4.pin(TMUGROUP[i]).uploadRawResults(tmuResult4[i]);

			}

			for (int i = 0; i < 100; i++) {

				meanvalue0D[i] = 0.0;
				meanvalue1D[i] = 0.0;

			}

			for (int i = 0; i < 100; i++) {

				tmuResult4[i].getPinMeasuredResults(TMUGROUP[i], value4[i],
						TMU::APP_RAW);

			}

			if (mDebug_Print) {
				for (int i = 0; i < 100; i++) {
					for (int j = 0; j < mSamples; j++) {

						printf("value4%d %d: %.20f\n", i, j, value4[i][j]);

					}

					PUT_DEBUG("value4", "value4 raw data", value4[i]);
				}

			}

			///// first data sub sequence

			///// third data sub sequence
			recount0 = mSamples;
			recount1 = mSamples;

			for (int i = 0; i < 100; i++) {
				for (int j = 0; j < mSamples; j++) {

					double subvalue0 = value4[0][j] - value4[i][j];
					double subvalue1 = value4[1][j] - value4[i][j];

					if (subvalue0 < 15 * 1e-9 || subvalue0 > 100 * 1e-9) {

						subvalue0 = 0.0;

						meanvalue0D[i] = meanvalue0D[i] + subvalue0;

						recount0 = recount0 - 1;
					} else {

						meanvalue0D[i] = meanvalue0D[i] + subvalue0;

					}

					if (subvalue1 < 15 * 1e-9 || subvalue1 > 100 * 1e-9) {

						subvalue1 = 0.0;
						meanvalue1D[i] = meanvalue1D[i] + subvalue1;

						recount1 = recount1 - 1;

					} else {

						meanvalue1D[i] = meanvalue1D[i] + subvalue1;

					}

					//cout << "meanvalue0B[i]" <<i<<":"<<"value2[0][j] - value2[i][j]:"<<value2[0][j] - value2[i][j] << endl;
					//cout << "meanvalue1B[i]" <<i<<":"<<"value2[1][j] - value2[i][j]:"<<value2[1][j] - value2[i][j] << endl;
					//cout << "meanvalue2B[i]" <<i<<":"<<"value3[1][j] - value2[i][j]:"<<value3[1][j] - value3[i][j] << endl;
				}

				meanvalue0D[i] = meanvalue0D[i] / recount0;
				meanvalue1D[i] = meanvalue1D[i] / recount1;

				//cout << "meanvalue0B[i] :"<<i<<":"<< meanvalue0B[i]<<endl;
				//cout << "meanvalue1B[i] :"<<i<<":"<< meanvalue1B[i]<<endl;

				recount0 = mSamples;
				recount1 = mSamples;

			}

			double meavalue0_final[100];
			double meavalue1_final[100];

			for (int i = 0; i < 100; i++) {

				//cout << "meanvalue0A[" << i << "] : " << meanvalue0A[i] << endl;
				//cout << "meanvalue1A[" << i << "] : " << meanvalue1A[i] << endl;

				//cout << "meanvalue0B[" << i << "] : " << meanvalue0B[i] << endl;
				//cout << "meanvalue1B[" << i << "] : " << meanvalue1B[i] << endl;

				//cout << "meanvalue0C[" << i << "] : " << meanvalue0C[i] << endl;
				//cout << "meanvalue1C[" << i << "] : " << meanvalue1C[i] << endl;

				//cout << "meanvalue0D[" << i << "] : " << meanvalue0D[i] << endl;
				//cout << "meanvalue1D[" << i << "] : " << meanvalue1D[i] << endl;

				if (meanvalue0A[i] > 0.0) {

					meavalue0_final[i] = meanvalue0A[i];

				} else if (meanvalue0B[i] > 0.0) {

					meavalue0_final[i] = meanvalue0B[i];

				} else if (meanvalue0C[i] > 0.0) {

					meavalue0_final[i] = meanvalue0C[i];

				} else if (meanvalue0D[i] > 0.0) {

					meavalue0_final[i] = meanvalue0D[i];

				} else {

					meavalue0_final[i] = 0.0;

				}

				if (meanvalue1A[i] > 0.0) {

					meavalue1_final[i] = meanvalue1A[i];

				} else if (meanvalue1B[i] > 0.0) {

					meavalue1_final[i] = meanvalue1B[i];

				} else if (meanvalue1C[i] > 0.0) {

					meavalue1_final[i] = meanvalue1C[i];

				} else if (meanvalue1D[i] > 0.0) {

					meavalue1_final[i] = meanvalue1D[i];

				} else {

					meavalue1_final[i] = 0.0;

				}

			}

			//determine DUT data rate received

			cout << "site " << CURRENT_SITE_NUMBER() << " SEN" << k << " Tpd Code " << k
					<< " MUX0OUT value (via measured value) = "
					<< meavalue0_final[k + 2] * 1e9 << " NS" << endl;
			cout << "site " << CURRENT_SITE_NUMBER() << " SEN" << k << " Tpd Code " << k
					<< " MUX1OUT value (via measured value) = "
					<< meavalue1_final[k + 2] * 1e9 << " NS" << endl;

			TEST(TMUGROUP[k + 2], "Tpd MUX0 Test", LIMIT(TM::GE, 47.0, TM::LE,
					55.0), meavalue0_final[k + 2] * 1e9, TM::CONTINUE);
			TEST(TMUGROUP[k + 2], "Tpd MUX1 Test", LIMIT(TM::GE, 47.0, TM::LE,
					55.0), meavalue1_final[k + 2] * 1e9, TM::CONTINUE);
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
REGISTER_TESTMETHOD("TPD_TMU_MEASURE", TPD_TMU_MEASURE)
;
