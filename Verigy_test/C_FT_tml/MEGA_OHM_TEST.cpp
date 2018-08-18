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

class MEGA_OHM_TEST: public testmethod::TestMethod {
protected:
	double mWait_time;
	string MEGA_pin;
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
		addParameter("MEGA_pin", "PinString", &MEGA_pin,
				testmethod::TM_PARAMETER_INPUT) .setDefault("ALLIO") .setComment(
				"the dac output pin connected to the DGT");

		addParameter("wait_time", "double", &mWait_time,
				testmethod::TM_PARAMETER_INPUT) .setComment(
				"wait for measurement");
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

		// add your pins

		};

		//

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

		PPMU_SETTING setting;
		PPMU_RELAY relay_on, relay_off;
		PPMU_MEASURE ppmuMeasure;
		PPMU_CLAMP clamp_on, clamp_off;
		TASK_LIST task1, task2;

		///// MEGAOHM TEST S2S_SHORT TEST /////

		ON_FIRST_INVOCATION_BEGIN();
				DISCONNECT();
				CONNECT();

				//// POWER DOWN ////
				///  LEVELSET USE ALL POWER 0V /////

				cout << "A position" << endl;

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
				Primary.getLevelSpec().change("MEGALEVEL", 0.3);

				// preFORCE 0.3V
				FLUSH();

				// FORCE 0V and CURRENT MEASURE
				// Setups for PPMU

				cout << "B position" << endl;

				//Relay Setups

				cout << sizeof(MEGA_pin_ARRAY) / sizeof(string) << endl;

				for (int i = 0; i < sizeof(MEGA_pin_ARRAY) / sizeof(string); i++) {

					cout << "C-1-1 position" << endl;

					setting.pin(MEGA_pin_ARRAY[i]).iRange(1 uA ).min(-1 uA ).max(
							1 uA ).vForce(0.0 V);

					cout << "C-1 position" << endl;

					relay_on.pin(MEGA_pin_ARRAY[i]).status("PPMU_ON");

					relay_on.wait(1.3 ms );

					cout << "C-2 position" << endl;

					relay_off.pin(MEGA_pin_ARRAY[i]).status("AC_ON");

					//MeasurEment Setups

					cout << "C-3 position" << endl;

					ppmuMeasure.pin(MEGA_pin_ARRAY[i]).execMode(TM::PVAL).numberOfSamples(
							100);

					//Hardware Specific Programing to avoid Hot Switching on the Current Force setup

					cout << "C-4 position" << endl;

					clamp_on.pin(MEGA_pin_ARRAY[i]).status("CLAMP_ON").low(0.0 V).high(
							1.0 V);
					clamp_off.pin(MEGA_pin_ARRAY[i]).status("CLAMP_OFF");

					cout << "C-5 position" << endl;

					task1.add(setting).add(clamp_on).add(relay_on).add(
							ppmuMeasure).add(relay_off);

					//Execute the Task list

					FLUSH();

				}

				task1.execute();

			ON_FIRST_INVOCATION_END();

		cout << "D position" << endl;

		for (int i = 0; i < sizeof(MEGA_pin_ARRAY) / sizeof(string); i++) {
			// Result upload and Datalog

			MEAGAOHM_VALUE_A[i] = ppmuMeasure.getValue(MEGA_pin_ARRAY[i]);
		}

		for (int i = 0; i < sizeof(MEGA_pin_ARRAY) / sizeof(string); i++) {

			site_num = CURRENT_SITE_NUMBER();
			///////////////////////////////////////////

			cout << "site " << CURRENT_SITE_NUMBER() << " "
					<< MEGA_pin_ARRAY[i] << " MEGAOHM current : "
					<< MEAGAOHM_VALUE_A[i] << endl;

			TEST(MEGA_pin_ARRAY[i], MEGA_pin_ARRAY[i], LIMIT(TM::GT, -0.06 uA ,
					TM::LT, 0.06 uA ), MEAGAOHM_VALUE_A[i], TM::CONTINUE);

		}

		///// MEGAOHM TEST S2P_SHORT TEST /////

		cout << "E position" << endl;

		ON_FIRST_INVOCATION_BEGIN();

				/// FORCE ALL OSPIN 0.1V /////
				LEVEL_SPEC spec1(1, 2);
				Primary.level(spec1);
				Primary.getLevelSpec().change("MEGALEVEL", 0.1);

				// preFORCE 0.1V
				FLUSH();

				// FORCE 0V and CURRENT MEASURE
				// Setups for PPMU

				setting.pin(MEGA_Pin_Single_Array).iRange(1 uA ).min(-1 uA ).max(
						1 uA ).vForce(0.1 V);

				//Relay Setups

				// parallel measure

				relay_on.pin(MEGA_Pin_Single_Array).status("PPMU_ON");

				relay_on.wait(1.3 ms );

				relay_off.pin(MEGA_Pin_Single_Array).status("AC_ON");

				//MeasurEment Setups

				ppmuMeasure.pin(MEGA_Pin_Single_Array).execMode(TM::PVAL).numberOfSamples(
						100);

				//Hardware Specific Programing to avoid Hot Switching on the Current Force setup

				clamp_on.pin(MEGA_Pin_Single_Array).status("CLAMP_ON").low(0.0 V).high(
						1.0 V);
				clamp_off.pin(MEGA_Pin_Single_Array).status("CLAMP_OFF");

				task2.add(setting).add(clamp_on).add(relay_on).add(ppmuMeasure).add(
						relay_off);

				//Execute the Task list

				FLUSH();

				task2.execute();

				ac_relay.pin("@").set("AC", "OFF");
				ac_relay.wait(1.5 ms );
				ac_relay.execute();

			ON_FIRST_INVOCATION_END();

		cout << "F position" << endl;

		for (int i = 0; i < sizeof(MEGA_pin_ARRAY) / sizeof(string); i++) {
			// Result upload and Datalog
			MEAGAOHM_VALUE_B[i] = ppmuMeasure.getValue(MEGA_pin_ARRAY[i]);
		}

		for (int i = 0; i < sizeof(MEGA_pin_ARRAY) / sizeof(string); i++) {

			site_num = CURRENT_SITE_NUMBER();
			///////////////////////////////////////////

			cout << "site " << CURRENT_SITE_NUMBER() << " "
					<< MEGA_pin_ARRAY[i] << " MEGAOHM current : "
					<< MEAGAOHM_VALUE_B[i] << endl;

			TEST(MEGA_pin_ARRAY[i], MEGA_pin_ARRAY[i], LIMIT(TM::GT, -0.1 uA ,
					TM::LT, 0.1 uA ), MEAGAOHM_VALUE_B[i], TM::CONTINUE);

		}

		cout << "G position" << endl;

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
REGISTER_TESTMETHOD("MEGA_OHM_TEST", MEGA_OHM_TEST)
;
