#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;

#define MAX_SITES 4
#define PINCOUNT 1
#define CAPTURE_SAMPLE 10000

class FREQ_MEASURE_HDSDI_RX_TMU: public testmethod::TestMethod {
protected:
    double mtreshold_a;
    int mPrescaler;
    int mSamples;
    int mDebug_Print;

	   virtual void initialize() {

	        addParameter("treshold_a", "double", &mtreshold_a,
	                testmethod::TM_PARAMETER_INPUT) .setDefault("0.0");
	        addParameter("Prescaler", "int", &mPrescaler,
	                testmethod::TM_PARAMETER_INPUT) .setDefault("1");
	        addParameter("Samples", "int", &mSamples,
	                testmethod::TM_PARAMETER_INPUT) .setDefault("1000");
	        addParameter("Debug_Print", "int", &mDebug_Print,
	                testmethod::TM_PARAMETER_INPUT) .setDefault("1");
	    }


	virtual void run() {

		TMU_TASK task1;

		FLEX_RELAY ac_relay;
		FLEX_RELAY util_relay;

		ON_FIRST_INVOCATION_BEGIN();
				DISCONNECT();

				ac_relay.pin("@").set("IDLE", "OFF");
				ac_relay.wait(1.5 ms );
				ac_relay.execute();

				ac_relay.pin("SDI_GRP").set("AC", "OFF");
				ac_relay.wait(1.5 ms );
				ac_relay.execute();

				string vector_lable = Primary.getLabel();
				if (mDebug_Print) {
					cout << "primary vector_lable = " << vector_lable << endl;
				}

				// i dont know how i can get multiport time spec value.
				double datarate = 4 * 1e-9;

				cout << "datarate : "<<datarate << endl;

				task1.pin("DCKOY").setEdgeSelect(TMU::RISE).setNumSamples(mSamples).setNumMeasurements(1).setNumShots(1);
				task1.pin("DCKOY").setDatarate(1.0/(datarate*2));
				task1.pin("DCKOY").setAThreshold(mtreshold_a);


				//task1.pin("SDIRP,SDIRM,SDITP,SDITM,MCKO,UCCDI").setEdgeSelect(
				//		TMU::RISE).setInterSampleDiscard(mInterSampleDiscard).setNumSamples(
				//		mSamples).setNumMeasurements(1).setPreScaler(mPrescaler);


				CONNECT();

				task1.setAsPrimary();

				task1.setup();

				//FLUSH();

				EXECUTE_TEST();

				//task1.disableTMU();

			ON_FIRST_INVOCATION_END();


		TMU_RESULTS tmuResult[PINCOUNT];

		task1.pin("DCKOY").uploadRawResults(tmuResult[0]);


		ARRAY_D value[PINCOUNT];

		tmuResult[0].getPinMeasuredResults("DCKOY", value[0], TMU::APP_RAW);


		if (mDebug_Print) {
			for (int i = 0; i < PINCOUNT; i++) {
				for (int j = 0; j < mSamples; j++) {

					printf("value%d %d: %.20f\n", i + 1, j, value[i][j]);

				}

				PUT_DEBUG("value", "value raw data", value[i]);

			}

		}

		//determine DUT data rate received


		cerr << setprecision(10);

		cerr << "DCKOY FREQUENCY (via measured value) = " <<
				((mPrescaler * 1) / (value[0][1] - value[0][0]))
				* 1e-6 << " MHz" << endl;



		task1.pin("DCKOY").uploadPeriodResults(tmuResult[0]);


		DOUBLE period[PINCOUNT];

		period[0] = tmuResult[0].getPeriodMean("DCKOY");

		if (mDebug_Print) {

			for (int i = 0; i < PINCOUNT; i++) {

				printf("period%d: %.20f\n", i + 1, period[i]);

			}

		}


		cerr << setprecision(10);

		cerr << "DCKOY FREQUENCY (via Period) = " << 1e-6 / period[0] << " MHz"
				<< endl;

		TEST("DCKOY", "DCKOY_FREQ", LIMIT(TM::GE, 70.0, TM::LE, 78.0), 1e-6
				/ period[0], TM::CONTINUE);

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
REGISTER_TESTMETHOD("FREQ_MEASURE_HDSDI_RX_TMU", FREQ_MEASURE_HDSDI_RX_TMU)
;
