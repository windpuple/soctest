#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;

#define MAX_SITES 4
#define PINCOUNT 4
#define CAPTURE_SAMPLE 10000

class FREQ_MEASURE_HDSDI_TMU: public testmethod::TestMethod {
protected:
	int mInterSampleDiscard;
	int mPrescaler;
	int mSamples;
	int mDebug_Print;


protected:

	virtual void initialize() {

		addParameter("InterSampleDiscard", "int", &mInterSampleDiscard,
				testmethod::TM_PARAMETER_INPUT) .setDefault("0");
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

				//FUNCTIONAL_TEST();
				EXECUTE_TEST();


				task1.pin("SDIRP,SDIRM,SDITP,SDITM,MCKO,UCCDI").setEdgeSelect(
						TMU::RISE).setInterSampleDiscard(mInterSampleDiscard).setNumSamples(
						mSamples).setNumMeasurements(1).setPreScaler(mPrescaler);


				CONNECT();

				task1.setAsPrimary();

				task1.setup();

				FUNCTIONAL_TEST();

			ON_FIRST_INVOCATION_END();


		TMU_RESULTS tmuResult[PINCOUNT];

		task1.pin("SDITP").uploadRawResults(tmuResult[0]);
		task1.pin("SDITM").uploadRawResults(tmuResult[1]);
		task1.pin("MCKO").uploadRawResults(tmuResult[2]);
		task1.pin("UCCDI").uploadRawResults(tmuResult[3]);


		ARRAY_D value[PINCOUNT];

		tmuResult[0].getPinMeasuredResults("SDITP", value[0], TMU::APP_RAW);
		tmuResult[1].getPinMeasuredResults("SDITM", value[1], TMU::APP_RAW);
		tmuResult[2].getPinMeasuredResults("MCKO", value[2], TMU::APP_RAW);
		tmuResult[3].getPinMeasuredResults("UCCDI", value[3], TMU::APP_RAW);

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

		cerr << "SDITP FREQUENCY (via measured value) = " << ((mPrescaler
				* (mInterSampleDiscard + 1)) / (value[0][1] - value[0][0]))
				* 1e-6 << " MHz" << endl;
		cerr << "SDITM FREQUENCY (via measured value) = " << ((mPrescaler
				* (mInterSampleDiscard + 1)) / (value[1][1] - value[1][0]))
				* 1e-6 << " MHz" << endl;
		cerr << "MCKO FREQUENCY (via measured value) = " << ((mPrescaler
				* (mInterSampleDiscard + 1)) / (value[2][1] - value[2][0]))
				* 1e-6 << " MHz" << endl;
		cerr << "UCCDI FREQUENCY (via measured value) = " << ((mPrescaler
				* (mInterSampleDiscard + 1)) / (value[3][1] - value[3][0]))
				* 1e-6 << " MHz" << endl;


		task1.pin("SDITP").uploadPeriodResults(tmuResult[0]);
		task1.pin("SDITM").uploadPeriodResults(tmuResult[1]);
		task1.pin("MCKO").uploadPeriodResults(tmuResult[2]);
		task1.pin("UCCDI").uploadPeriodResults(tmuResult[3]);


		DOUBLE period[PINCOUNT];

		period[0] = tmuResult[0].getPeriodMean("SDITP");
		period[1] = tmuResult[1].getPeriodMean("SDITM");
		period[2] = tmuResult[2].getPeriodMean("MCKO");
		period[3] = tmuResult[3].getPeriodMean("UCCDI");

		if (mDebug_Print) {

			for (int i = 0; i < PINCOUNT; i++) {

				printf("period%d: %.20f\n", i + 1, period[i]);

			}

		}


		cerr << setprecision(10);

		cerr << "SDITP FREQUENCY (via Period) = " << 1e-6 / period[0] << " MHz"
				<< endl;
		cerr << "SDITM FREQUENCY (via Period) = " << 1e-6 / period[1] << " MHz"
				<< endl;
		cerr << "MCKO FREQUENCY (via Period) = " << 1e-6 / period[2] << " MHz"
				<< endl;
		cerr << "UCCDI FREQUENCY (via Period) = " << 1e-6 / period[3] << " MHz"
				<< endl;

		TEST("SDITP", "SDITP_FREQ", LIMIT(TM::GE, 70.0, TM::LE, 78.0), 1e-6
				/ period[0], TM::CONTINUE);
		TEST("SDITM", "SDITM_FREQ", LIMIT(TM::GE, 70.0, TM::LE, 78.0), 1e-6
				/ period[1], TM::CONTINUE);
		TEST("MCKO", "MCKO_FREQ", LIMIT(TM::GE, 24.0, TM::LE, 30.0), 1e-6
				/ period[2], TM::CONTINUE);
		TEST("UCCDI", "UCCDI_FREQ", LIMIT(TM::GE, 32.0, TM::LE, 40.0), 1e-6
				/ period[3], TM::CONTINUE);

		//double unitInterval = mTarget_UI_ns * 1e3;

		double unitInterval[PINCOUNT];

		for (int i = 0; i < PINCOUNT; i++) {

			unitInterval[i] = period[i] * 1e12;

		}

		// find error of each time stamp

		vector<double> edgeError[4];

		for (int i = 0; i < PINCOUNT; i++) {

			for (int j = 1; j < mSamples; j++)

			{

				double edgediff = (value[i][j] - value[i][0]) * 1e12; //[ps]

				int N = (int) floor((edgediff + 0.5 * unitInterval[i])
						/ unitInterval[i]);

				double error = edgediff - N * unitInterval[i];

				edgeError[i].push_back(error);

			}

		}

		if (mDebug_Print) {

			PUT_DEBUG("SDITP", "Edge Error", ARRAY_D(&edgeError[0][0],
					edgeError[0].size()));
			PUT_DEBUG("SDITM", "Edge Error", ARRAY_D(&edgeError[1][0],
					edgeError[1].size()));
			PUT_DEBUG("MCKO", "Edge Error", ARRAY_D(&edgeError[2][0],
					edgeError[2].size()));
			PUT_DEBUG("UCCDI", "Edge Error", ARRAY_D(&edgeError[3][0],
					edgeError[3].size()));
		}

		// jitter calculation

		double mean[PINCOUNT], sigma[PINCOUNT];
		double min[PINCOUNT], max[PINCOUNT];
		int min_index[PINCOUNT], max_index[PINCOUNT];

		for (int i = 0; i < PINCOUNT; i++) {

			DSP_MEAN(&(edgeError[i][0]), &mean[i], &sigma[i],
					edgeError[i].size());

			DSP_MINMAX(&(edgeError[i][0]), &min[i], &max[i], &min_index[i],
					&max_index[i], edgeError[i].size());

		}

		if (mDebug_Print) {

			int nBins = 512;

			vector<int> histogram(nBins);

			DSP_HISTOGRAM(&(edgeError[0][0]), &(histogram[0]),
					edgeError[0].size(), min[0], max[0], nBins);
			PUT_DEBUG("SDITP", "Data Jitter Histogram", ARRAY_I(&histogram[0],
					histogram.size()));

			DSP_HISTOGRAM(&(edgeError[1][0]), &(histogram[0]),
					edgeError[1].size(), min[1], max[1], nBins);
			PUT_DEBUG("SDITM", "Data Jitter Histogram", ARRAY_I(&histogram[0],
					histogram.size()));

			DSP_HISTOGRAM(&(edgeError[2][0]), &(histogram[0]),
					edgeError[2].size(), min[2], max[2], nBins);
			PUT_DEBUG("MCKO", "Data Jitter Histogram", ARRAY_I(&histogram[0],
					histogram.size()));

			DSP_HISTOGRAM(&(edgeError[3][0]), &(histogram[0]),
					edgeError[3].size(), min[3], max[3], nBins);
			PUT_DEBUG("UCCDI", "Data Jitter Histogram", ARRAY_I(&histogram[0],
					histogram.size()));

		}

		cerr << setprecision(13);

		cerr << "SDITP" << " data rate  = " << unitInterval[0] / 1e3 << " ns"
				<< endl;
		cerr << "SDITP" << " rms jitter = " << sigma[0] << " ps" << endl;
		cerr << "SDITP" << " p-p jitter = " << max[0] - min[0] << " ps" << endl;

		cerr << "SDITM" << " data rate  = " << unitInterval[1] / 1e3 << " ns"
				<< endl;
		cerr << "SDITM" << " rms jitter = " << sigma[1] << " ps" << endl;
		cerr << "SDITM" << " p-p jitter = " << max[1] - min[1] << " ps" << endl;

		cerr << "MCKO" << " data rate  = " << unitInterval[2] / 1e3 << " ns"
				<< endl;
		cerr << "MCKO" << " rms jitter = " << sigma[2] << " ps" << endl;
		cerr << "MCKO" << " p-p jitter = " << max[2] - min[2] << " ps" << endl;

		cerr << "UCCDI" << " data rate  = " << unitInterval[3] / 1e3 << " ns"
				<< endl;
		cerr << "UCCDI" << " rms jitter = " << sigma[3] << " ps" << endl;
		cerr << "UCCDI" << " p-p jitter = " << max[3] - min[3] << " ps" << endl;

		TEST("SDITP", "SDITP_RMSJITTER", LIMIT(TM::GE, 0.0, TM::LE, 1000.0),sigma[0], TM::CONTINUE);
		TEST("SDITP", "SDITP_PPJITTER", LIMIT(TM::GE, 0.0, TM::LE, 1000.0),max[0] - min[0], TM::CONTINUE);

		TEST("SDITM", "SDITM_RMSJITTER", LIMIT(TM::GE, 0.0, TM::LE, 1000.0),sigma[1], TM::CONTINUE);
		TEST("SDITM", "SDITM_PPJITTER", LIMIT(TM::GE, 0.0, TM::LE, 1000.0),max[1] - min[1], TM::CONTINUE);

		TEST("MCKO", "MCKO_RMSJITTER", LIMIT(TM::GE, 0.0, TM::LE, 1000.0),sigma[2], TM::CONTINUE);
		TEST("MCKO", "MCKO_PPJITTER", LIMIT(TM::GE, 0.0, TM::LE, 1000.0),max[2] - min[2], TM::CONTINUE);

		TEST("UCCDI", "UCCDI_RMSJITTER", LIMIT(TM::GE, 0.0, TM::LE, 1000.0),sigma[3], TM::CONTINUE);
		TEST("UCCDI", "UCCDI_PPJITTER", LIMIT(TM::GE, 0.0, TM::LE, 1000.0),max[3] - min[3], TM::CONTINUE);

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
REGISTER_TESTMETHOD("FREQ_MEASURE_HDSDI_TMU", FREQ_MEASURE_HDSDI_TMU)
;
