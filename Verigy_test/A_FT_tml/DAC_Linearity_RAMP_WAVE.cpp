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

class DAC_Linearity_RAMP_WAVE: public testmethod::TestMethod {

protected:
	string DGT_pin;
	int debug;
	int test_site;
	double waittime;
	int TRIG_FLAG[4];
protected:
	virtual void initialize() {
		//Add your initialization code here
		//Note: Test Method API should not be used in this method!
		addParameter("DGT_pin", "PinString", &DGT_pin,
				testmethod::TM_PARAMETER_INPUT) .setDefault("DAC_MEAS") .setComment(
				"the dac output pin connected to the DGT");
		addParameter("debug", "int", &debug, testmethod::TM_PARAMETER_INPUT) .setDefault(
				"1") .setComment("control if the test is in debug mode");
		addParameter("waittime", "double", &waittime,
				testmethod::TM_PARAMETER_INPUT) .setComment(
				"relay control waittime and the unit is second");
	}

	/**
	 *This test is invoked per site.
	 */
	virtual void run() {
		FLEX_RELAY ac_relay;
		FLEX_RELAY relay;
		;

		int i, j;
		ARRAY_D ramp_unav_wf, ramp_av_wf, dnl_wf, inl_wf;
		LINEARtype linear_result;
		double tout;
		double ramp_min, ramp_max, diff;
		int ramp_min_index, ramp_max_index;
		double inl, dnl;
		int curr_site;
		int ramp_samples = 1024;
		int ramp_av = 5;
		//int ramp_wf_st_addr = 16-1; //repeat 16times - 1
		tout = 5.0;
		static INT tout_site_flag, mis_trg_site;

		ON_FIRST_INVOCATION_BEGIN();
				DISCONNECT();
				CONNECT();

				relay.util("CVBS_K4,COMP_K6,REXTD_K5").on();
				relay.wait(1.5 ms );
				relay.execute();

				ac_relay.pin("@").set("IDLE", "OFF");
				ac_relay.wait(1.5 ms );
				ac_relay.execute();

				ac_relay.pin(
						"DACTRI,RSTX,XI0,XI1,XI2,TP0,TP1,TP2,GPIO15,GPIO16,GPIO17,GPIO18,GPIO19,GPIO20,GPIO21,GPIO22,GPIO23,GPIO24,GPIO25,GPIO26,GPIO27").set(
						"AC", "OFF");
				ac_relay.wait(1.5 ms );
				ac_relay.execute();

				Routing.pin(DGT_pin).connect(TM::SINGLE);
				DGT(DGT_pin).enable();
				FLUSH();

				//	FUNCTIONAL_TEST(); //Need to Pre-Running
				EXECUTE_TEST();// START_TEST replaced, reason is read manual.
				WAIT_TEST_DONE(tout, &tout_site_flag, &mis_trg_site);
				//triggers (0) in a test or not (1) and returns the status to mis_trg_site variable.

				relay.util("CVBS_K4,COMP_K6,REXTD_K5").off();
				relay.wait(1.5 ms );
				relay.execute();

			ON_FIRST_INVOCATION_END();

		ramp_unav_wf = DGT(DGT_pin).getWaveform();
		ramp_av_wf.resize(ramp_samples);

		// captured all value check.
		// for(i = 0; i < ramp_samples*ramp_av; i++) {

		// 	printf("%.2f\n",ramp_unav_wf[i]);

		// }

		Routing.pin(DGT_pin).disconnect();
		DGT(DGT_pin).disable();
		FLUSH();

		for (i = 0; i < ramp_samples; i++) {
			ramp_av_wf[i] = 0.0;
		}

		for (i = 0; i < ramp_samples; i++) {
			for (j = 0; j < ramp_av; j++) {
				// is this need start adress adjust?
				//ramp_av_wf[i] = ramp_av_wf[i] + ramp_unav_wf[((j+1)*ramp_wf_st_addr) + i + j*ramp_samples];

				// no adjust start adress, just start 0.
				ramp_av_wf[i] = ramp_av_wf[i] + ramp_unav_wf[i + j
						* (ramp_samples*2)];
			}
		}

		for (i = 0; i < ramp_samples; i++) {
			ramp_av_wf[i] = ramp_av_wf[i] / (float) ramp_av;
		}

		PUT_DEBUG(DGT_pin, "Ramp unav", ramp_unav_wf);
		PUT_DEBUG(DGT_pin, "Ramp av", ramp_av_wf);

		DSP_LINEAR(ramp_av_wf, &linear_result, dnl_wf, inl_wf, MIN_RMS_LSB);
		DSP_MINMAX(ramp_unav_wf, &ramp_min, &ramp_max, &ramp_min_index,
				&ramp_max_index);

		// MIN MAX value check, index check
		//printf("MIN:%.2f\n",ramp_min);
		//printf("MAX:%.2f\n",ramp_max);
		//printf("MINindex:%d\n",ramp_min_index);
		//printf("MAXindex:%d\n",ramp_max_index);


		PUT_DEBUG(DGT_pin, "DNL WF", dnl_wf);
		PUT_DEBUG(DGT_pin, "INL WF", inl_wf);

		diff = fabs(ramp_max - ramp_min);
		if (diff < 0.3) {
			inl = 999.0;
			dnl = 999.0;
		}

		curr_site = CURRENT_SITE_NUMBER();
		if ((mis_trg_site >> (curr_site - 1)) & 1) {
			cout << "Trigger error on site" << curr_site << endl;
			TRIG_FLAG[CURRENT_SITE_NUMBER() - 1] = 1;
		} else {
			cout << "Triggered good on site" << curr_site << endl;
			TRIG_FLAG[CURRENT_SITE_NUMBER() - 1] = 0;
		}

		if (debug) {
			cout << "Site " << curr_site << " DAC DNL : "
					<< linear_result.damax << " LSB" << endl;
		}
		if (debug) {
			cout << "Site " << curr_site << " DAC INL : "
					<< linear_result.iamax << " LSB" << endl;
		}

		TEST("TRIGGER", "TRIGGER", LIMIT(TM::GE, 0.0, TM::LE, 0.0),
				TRIG_FLAG[CURRENT_SITE_NUMBER() - 1], TM::CONTINUE);
		TEST("DGT_pin", "DAC_DLE", LIMIT(TM::GE, -3.0, TM::LE, 3.0),
				linear_result.damax, TM::CONTINUE);
		TEST("DGT_pin", "DAC_ILE", LIMIT(TM::GE, -3.0, TM::LE, 3.0),
				linear_result.iamax, TM::CONTINUE);

		return;
	}

	virtual void postParameterChange(const string& parameterIdentifier) {

		return;
	}

	virtual const string getComment() const {
		string comment = "DAC RAMP Linearity Test.";
		return comment;
	}
};
REGISTER_TESTMETHOD("DAC_Linearity_RAMP_WAVE", DAC_Linearity_RAMP_WAVE)
;
