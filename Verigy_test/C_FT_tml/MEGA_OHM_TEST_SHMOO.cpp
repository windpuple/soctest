#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
#include <string>
#include <cstring>

using namespace std;

/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */

class MEGA_OHM_TEST_SHMOO: public testmethod::TestMethod {
protected:

	int mdebug;

#define D_MAX_ARRAY_SIZE 4096

protected:
	/**
	 *Initialize the parameter interface to the testflow.
	 *This method is called just once after a testsuite is created.
	 */
	virtual void initialize() {
		//Add your initialization code here
		//Note: Test Method API should not be used in this method!

		addParameter("debug", "int", &mdebug, testmethod::TM_PARAMETER_INPUT);

	}

	/**
	 *This test is invoked per site.
	 */

	// string split function

	string* StringSplit(string strTarget, string strTok) {
		int nCutPos;
		int nIndex = 0;
		string* strResult = new string[D_MAX_ARRAY_SIZE];

		while ((nCutPos = strTarget.find_first_of(strTok)) != strTarget.npos) {
			if (nCutPos > 0) {
				strResult[nIndex++] = strTarget.substr(0, nCutPos);
			}
			strTarget = strTarget.substr(nCutPos + 1);
		}

		if (strTarget.length() > 0) {
			strResult[nIndex++] = strTarget.substr(0, nCutPos);
		}

		return strResult;
	}

	virtual void run() {

		/* pinstring class cant export inner pin component. so this is useless MEGAOHM test.

		 string* MEGA_pin_ARRAY  = StringSplit(MEGA_pin,",");

		 cout <<  MEGA_pin.max_size() <<  endl;
		 cout <<  MEGA_pin.length() <<  endl;
		 cout <<  MEGA_pin.c_str() <<  endl;
		 cout <<  MEGA_pin.data() <<  endl;
		 cout <<  MEGA_pin.size() <<  endl;

		 for(int i = 0 ; i < 10; i++) {

		 cout << "MEGAOHMPIN : " << i << " : " << MEGA_pin_ARRAY[i] << endl;
		 }

		 */

		string MEGA_pin_ARRAY[] = {

		"snsr6sdout", "pll1lock", "pll0lock", "clkin", "dbgled4", "dbgled2",
				"mbdata4", "mbdata3", "mbdata0", "mbwren", "mbch2",
				"snsr3sdout", "snsr3scout", "snsr6resetn", "snsr6scout",
				"systemclksel", "dbgled5", "dbgled1", "mbdata7", "mbdata2",
				"mbclk", "mbreset", "mbch1", "snsr3resetn", "snsr3mclk",
				"snsr5sdout", "snsr6mclk", "taraio0/papll1lf",
				"taraio0/papll0lf", "dbgled3", "dbgled0", "mbdata5", "mbdata6",
				"mbdata1", "mbvsync", "mbch0", "snsr2sdout", "snsr2scout",
				"snsr5resetn", "snsr5scout", "snsr2resetn", "snsr2mclk",
				"snsr4sdout", "snsr5mclk", "snsr1sdout", "snsr1scout",
				"snsr4scout", "snsr4resetn", "snsr4mclk", "snsr1mclk",
				"snsr1resetn", "snsr6data1p", "snsr6data1n", "snsr1data0p",
				"snsr1data0n", "snsr6clkn", "snsr6clkp", "snsr6data0n",
				"snsr1data1p", "snsr1clkp", "snsr1clkn", "snsr5data1p",
				"snsr5data1n", "snsr6data0p", "snsr1data1n", "snsr2data0p",
				"snsr2data0n", "snsr5clkn", "snsr5clkp", "snsr5data0n",
				"snsr2data1p", "snsr2clkn", "snsr2clkp", "snsr4data1n",
				"snsr4data1p", "snsr5data0p", "snsr2data1n", "snsr3data0n",
				"snsr3data0p", "snsr4clkn", "snsr4clkp", "resetnlocalin",
				"snsr3clkn", "snsr3clkp", "snsr4data0n", "snsr4data0p",
				"pa50ldomipien", "resetnin", "ucomirq", "spicpol",
				"ucomspiclk", "ucomspinss", "snsr3data1n", "snsr3data1p",
				"pa50ldocoreen", "testmode", "chipid", "spicpna", "spi8b16b",
				"ucomspiin", "ucomspiout"
		//"vddldomipiout",
				//"vddldocoreout"

				};

		string MEGA_MIPI_PLL_IN[] = {

		"taraio0/papll1lf", "taraio0/papll0lf", "snsr6data1p", "snsr6data1n",
				"snsr1data0p", "snsr1data0n", "snsr6clkn", "snsr6clkp",
				"snsr6data0n", "snsr1data1p", "snsr1clkp", "snsr1clkn",
				"snsr5data1p", "snsr5data1n", "snsr6data0p", "snsr1data1n",
				"snsr2data0p", "snsr2data0n", "snsr5clkn", "snsr5clkp",
				"snsr5data0n", "snsr2data1p", "snsr2clkn", "snsr2clkp",
				"snsr4data1n", "snsr4data1p", "snsr5data0p", "snsr2data1n",
				"snsr3data0n", "snsr3data0p", "snsr4clkn", "snsr4clkp",
				"snsr3clkn", "snsr3clkp", "snsr4data0n", "snsr4data0p",
				"snsr3data1n", "snsr3data1p"

		//"vddldomipiout",
				//"vddldocoreout"

				};

		string MEGA_Pin_Single_Array;

		for (int i = 0; i < sizeof(MEGA_pin_ARRAY) / sizeof(string); i++) {

			if (i == 0) {

				MEGA_Pin_Single_Array = MEGA_Pin_Single_Array
						+ MEGA_pin_ARRAY[i] + ",";

			} else if (i == sizeof(MEGA_pin_ARRAY) / sizeof(string) - 1) {

				MEGA_Pin_Single_Array = MEGA_Pin_Single_Array
						+ MEGA_pin_ARRAY[i];

			} else {

				MEGA_Pin_Single_Array = MEGA_Pin_Single_Array
						+ MEGA_pin_ARRAY[i] + ",";

			}

			//cout << MEGA_Pin_Single_Array << endl;
		}

		string MEGA_MIPI_PLL_IN_Array;

		for (int i = 0; i < sizeof(MEGA_MIPI_PLL_IN) / sizeof(string); i++) {

			if (i == 0) {

				MEGA_MIPI_PLL_IN_Array = MEGA_MIPI_PLL_IN_Array
						+ MEGA_MIPI_PLL_IN[i] + ",";

			} else if (i == sizeof(MEGA_pin_ARRAY) / sizeof(string) - 1) {

				MEGA_MIPI_PLL_IN_Array = MEGA_MIPI_PLL_IN_Array
						+ MEGA_MIPI_PLL_IN[i];

			} else {

				MEGA_MIPI_PLL_IN_Array = MEGA_MIPI_PLL_IN_Array
						+ MEGA_MIPI_PLL_IN[i] + ",";

			}

			//cout << MEGA_Pin_Single_Array << endl;
		}

#define uA *1.0e-6
#define UA *1.0e-6
#define mA *1.0e-3
#define MA *1.0e-3
		FLEX_RELAY ac_relay;

		/* Very usefull c++ string array dimention count way
		 // int

		 const int iArr[] = {1,2,3,4,5};

		 iCnt = sizeof(iArr)/sizeof(int);

		 cout << "iArr dimension size = " << iCnt << endl;



		 // double

		 const double dArr[] = {12.25, 25.15, 45.3};

		 iCnt = sizeof(dArr)/sizeof(double);

		 cout << "dArr dimension size = " << iCnt << endl;



		 // char*

		 const char* szArr[] = {"apple","orange","banana"};

		 iCnt = sizeof(szArr)/sizeof(char*);

		 cout << "szArr dimension size = " << iCnt << endl;



		 // string

		 const string strArr[] = {"apple","orange","banana"};

		 iCnt = sizeof(strArr)/sizeof(string);

		 cout << "strArr dimension size = " << iCnt << endl;

		 */

		//Add your test code here.
		DOUBLE MEAGAOHM_VALUE_A[sizeof(MEGA_pin_ARRAY) / sizeof(string)];
		DOUBLE MEAGAOHM_VALUE_B[sizeof(MEGA_pin_ARRAY) / sizeof(string)];

		int site_num;

		PPMU_SETTING setting, setting2;
		PPMU_RELAY relay_on, relay_off;
		PPMU_MEASURE ppmuMeasure;
		PPMU_CLAMP clamp_on, clamp_off;
		TASK_LIST task1, task2;

		///// MEGAOHM TEST S2S_SHORT TEST /////

		for (double S2S_level = 0.3; S2S_level < 0.9; S2S_level = S2S_level
				+ 0.1) {

			ON_FIRST_INVOCATION_BEGIN();
					DISCONNECT();
					CONNECT();

					//// POWER DOWN ////
					///  LEVELSET USE ALL POWER 0V /////

					// INITIALIZE and ONLY CONTACT USE PIN ///
					ac_relay.pin("@").set("IDLE", "OFF");
					ac_relay.wait(1.5 ms );
					ac_relay.execute();

					ac_relay.pin(MEGA_Pin_Single_Array).set("AC", "OFF");
					ac_relay.wait(1.5 ms );
					ac_relay.execute();

					/// FORCE ALL OSPIN 0.3V /////
					LEVEL_SPEC spec1(1, 2);
					Primary.level(spec1);
					//Primary.getLevelSpec().change("MEGALEVEL", 0.3);
					Primary.getLevelSpec().change("MEGALEVEL", S2S_level);

					Sequencer.stopVector(1286).run(TM::NORMAL);
					// preFORCE 0.3V
					FLUSH();

					// FORCE 0V and CURRENT MEASURE
					// Setups for PPMU

					//Relay Setups

					//setting2.pin(MEGA_Pin_Single_Array).iRange(1 uA ).min(-1 uA ).max(
					//		1 uA ).vForce(0.3 V);
					setting2.pin(MEGA_Pin_Single_Array).iRange(1 uA ).min(-1 uA ).max(
							1 uA ).vForce(S2S_level V);

					relay_on.pin(MEGA_Pin_Single_Array).status("PPMU_ON");
					relay_off.pin(MEGA_Pin_Single_Array).status("AC_ON");

					task1.add(setting2).add(relay_on);

					for (int i = 0; i < sizeof(MEGA_pin_ARRAY) / sizeof(string); i++) {

						setting.pin(MEGA_pin_ARRAY[i]).iRange(1 uA ).min(-1 uA ).max(
								1 uA ).vForce(0.0 V);

						//relay_on.pin(MEGA_pin_ARRAY[i]).status("PPMU_ON");

						//relay_on.wait(1.3 ms );

						//relay_off.pin(MEGA_pin_ARRAY[i]).status("AC_ON");

						//MeasurEment Setups

						ppmuMeasure.pin(MEGA_pin_ARRAY[i]).execMode(TM::PVAL).numberOfSamples(
								1);

						//Hardware Specific Programing to avoid Hot Switching on the Current Force setup

						//clamp_on.pin(MEGA_pin_ARRAY[i]).status("CLAMP_ON").low(0.0 V).high(
						//		1.0 V);
						//clamp_off.pin(MEGA_pin_ARRAY[i]).status("CLAMP_OFF");

						//task1.add(setting).add(clamp_on).add(relay_on).add(
						//		ppmuMeasure).add(relay_off);

						//task1.add(setting).add(relay_on).add(ppmuMeasure).add(
						//		relay_off);

						if (i == sizeof(MEGA_pin_ARRAY) / sizeof(string) - 1) {

							task1.add(setting).add(ppmuMeasure).add(setting2).add(
									relay_off);

						} else {

							task1.add(setting).add(ppmuMeasure).add(setting2);

						}
						//Execute the Task list

						//FLUSH();

					}

					task1.execute();

				ON_FIRST_INVOCATION_END();

			for (int i = 0; i < sizeof(MEGA_pin_ARRAY) / sizeof(string); i++) {
				// Result upload and Datalog

				MEAGAOHM_VALUE_A[i] = ppmuMeasure.getValue(MEGA_pin_ARRAY[i]);
			}

			cout << "\n" << "site " << CURRENT_SITE_NUMBER() << " S2S LEVEL "
					<< S2S_level << " V S2S_SHORT MEGAOHM TEST \n" << endl;

			for (int i = 0; i < sizeof(MEGA_pin_ARRAY) / sizeof(string); i++) {

				site_num = CURRENT_SITE_NUMBER();
				///////////////////////////////////////////

				cout << "site " << CURRENT_SITE_NUMBER() << " S2S LEVEL "
						<< S2S_level << " V " << MEGA_pin_ARRAY[i]
						<< " S2S_SHORT MEGAOHM current : "
						<< MEAGAOHM_VALUE_A[i] << endl;

				//TEST(MEGA_pin_ARRAY[i], MEGA_pin_ARRAY[i], LIMIT(TM::GT, -0.06
				//		uA , TM::LT, 0.06 uA ), MEAGAOHM_VALUE_A[i],
				//		TM::CONTINUE);

			}

		}

		///// MEGAOHM TEST S2P_SHORT TEST /////

		for (double S2P_level = 0.1; S2P_level < 0.8; S2P_level = S2P_level
				+ 0.1) {

			ON_FIRST_INVOCATION_BEGIN();

					Sequencer.reset();
					FLUSH();

					/// FORCE ALL OSPIN 0.1V /////
					LEVEL_SPEC spec1(1, 2);
					Primary.level(spec1);
					//Primary.getLevelSpec().change("MEGALEVEL", 0.1);
					Primary.getLevelSpec().change("MEGALEVEL", S2P_level);

					// preFORCE 0.1V
					Sequencer.stopVector(1286).run(TM::NORMAL);
					FLUSH();

					// FORCE 0V and CURRENT MEASURE
					// Setups for PPMU


					setting.pin(MEGA_Pin_Single_Array).iRange(1 uA ).min(-1 uA ).max(
							1 uA ).vForce(S2P_level V);

					if (S2P_level >= 0.2 && S2P_level < 0.5) {

						setting.pin(MEGA_MIPI_PLL_IN_Array).iRange(100 uA ).min(
								-100 uA ).max(100 uA ).vForce(S2P_level V);

						if (S2P_level = 0.3) {

							setting.pin(MEGA_MIPI_PLL_IN_Array).iRange(10 uA ).min(
									-10 uA ).max(10 uA ).vForce(S2P_level V);

							setting.pin("taraio0/papll0lf,taraio0/papll1lf").iRange(100 uA ).min(
									-100 uA ).max(100 uA ).vForce(S2P_level V);
						}

						if (S2P_level = 0.4) {

							setting.pin("taraio0/papll0lf,taraio0/papll1lf").iRange(
									1 mA ).min(-1 mA ).max(1 mA ).vForce(
									S2P_level V);
						}

					} else if (S2P_level >= 0.5 && S2P_level < 0.7) {

						setting.pin(MEGA_MIPI_PLL_IN_Array).iRange(10 mA ).min(
								-10 mA ).max(10 mA ).vForce(S2P_level V);

					} else if (S2P_level >= 0.7 && S2P_level < 1.1) {

						setting.pin(MEGA_MIPI_PLL_IN_Array).iRange(10 mA ).min(
								-10 mA ).max(10 mA ).vForce(S2P_level V);

					}

					//Relay Setups

					// parallel measure

					relay_on.pin(MEGA_Pin_Single_Array).status("PPMU_ON");

					//relay_on.wait(1.3 ms );

					relay_off.pin(MEGA_Pin_Single_Array).status("AC_ON");

					//MeasurEment Setups

					ppmuMeasure.pin(MEGA_Pin_Single_Array).execMode(TM::PVAL).numberOfSamples(
							1);

					//Hardware Specific Programing to avoid Hot Switching on the Current Force setup

					//clamp_on.pin(MEGA_Pin_Single_Array).status("CLAMP_ON").low(0.0 V).high(
					//		1.0 V);
					//clamp_off.pin(MEGA_Pin_Single_Array).status("CLAMP_OFF");

					//task2.add(setting).add(clamp_on).add(relay_on).add(ppmuMeasure).add(
					//		relay_off);
					task2.add(setting).add(relay_on).add(ppmuMeasure).add(
							relay_off);

					//Execute the Task list

					//FLUSH();

					task2.execute();

					ac_relay.pin("@").set("AC", "OFF");
					ac_relay.wait(1.5 ms );
					ac_relay.execute();

				ON_FIRST_INVOCATION_END();

			for (int i = 0; i < sizeof(MEGA_pin_ARRAY) / sizeof(string); i++) {
				// Result upload and Datalog
				MEAGAOHM_VALUE_B[i] = ppmuMeasure.getValue(MEGA_pin_ARRAY[i]);
			}

			cout << "\n" << "site " << CURRENT_SITE_NUMBER() << " S2P LEVEL "
					<< S2P_level << " V S2P_SHORT MEGAOHM TEST \n" << endl;

			for (int i = 0; i < sizeof(MEGA_pin_ARRAY) / sizeof(string); i++) {

				site_num = CURRENT_SITE_NUMBER();
				///////////////////////////////////////////

				cout << "site " << CURRENT_SITE_NUMBER() << " S2P LEVEL "
						<< S2P_level << " V " << MEGA_pin_ARRAY[i]
						<< " S2P_SHORT MEGAOHM current : "
						<< MEAGAOHM_VALUE_B[i] << endl;

				//TEST(MEGA_pin_ARRAY[i], MEGA_pin_ARRAY[i], LIMIT(TM::GT,
				//		-0.1 uA , TM::LT, 0.1 uA ), MEAGAOHM_VALUE_B[i],
				//		TM::CONTINUE);

			}
		}

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
REGISTER_TESTMETHOD("MEGA_OHM_TEST_SHMOO", MEGA_OHM_TEST_SHMOO)
;
