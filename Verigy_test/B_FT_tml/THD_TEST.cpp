#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;

/*
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */

class THD_TEST: public testmethod::TestMethod {

protected:
	string DGT_pin;
	string AWG_pin;
	//string  SMC_Distortion_handleName;
	int first_num_cycle_DAC;
	int second_num_cycle_DAC;
	int num_harmonics_DAC;
	int debug;
	int test_site;
	double waittime;
	int NOTRIG_FLAG;
	int TRIG_FLAG[4];

	/**
	 *Initialize the parameter interface to the testflow.
	 *This method is called just once after a testsuite is created.
	 */
protected:
	virtual void initialize() {
		//Add your initialization code here
		//Note: Test Method API should not be used in this method!
		addParameter("DGT_pin", "PinString", &DGT_pin,
				testmethod::TM_PARAMETER_INPUT) .setDefault("DAC_OUT") .setComment(
				"the dac output pin connected to the DGT");
		addParameter("AWG_pin", "PinString", &AWG_pin,
				testmethod::TM_PARAMETER_INPUT) .setDefault("ADC_AWG") .setComment(
				"the dac output pin connected to the DGT");

		addParameter("first_num_cycle_DAC", "int", &first_num_cycle_DAC,
				testmethod::TM_PARAMETER_INPUT) .setComment(
				"the first sinecycle number of signal used to test DAC");
		addParameter("second_num_cycle_DAC", "int", &second_num_cycle_DAC,
				testmethod::TM_PARAMETER_INPUT) .setComment(
				"the second sine cycle number of signal used to test DAC");


		addParameter("num_harmonics_DAC", "int", &num_harmonics_DAC,
				testmethod::TM_PARAMETER_INPUT) .setComment(
				"dhe number of calculated harmonics");
		addParameter("debug", "int", &debug, testmethod::TM_PARAMETER_INPUT) .setDefault(
				"1") .setComment("control if the test is in debug mode");
		addParameter("waittime", "double", &waittime,
				testmethod::TM_PARAMETER_INPUT) .setDefault("0.0") .setComment(
				"relay control waittime and the unit is second");
	}

	virtual void run() {
		FLEX_RELAY relay;

		// add your code
		double thd, sndr;
		double signal, noise;
		int i, j, k;
		double tout;
		int sine_av_samples = 3072;
		int sine_av = 5;
		int curr_site;
		static INT tout_site_flag, mis_trg_site;
		THDtype thd_result[2];
/*
		ARRAY_D sine_unav_wf;
		ARRAY_D sine_av_wf;
		ARRAY_D fft_wf;
		ARRAY_D fft_volt_wf;
		tout = 5.0;

		FLEX_RELAY ac_relay;
*/


		 string unaverage_wave[2] = {"1K_Sine unav", "10K_Sine unav"};
		 string average_wave[2] = {"1K_Sine av", "10K_Sine av"};
		 string fwave_DB[2] = {"1K_FFT_wf", "10K_FFT_wf"};
		 string fwave_VT[2] = {"1K_FFT_VOLT_wf", "10K_FFT_VOLT_wf"};

		 ARRAY_D sine_unav_wf[2];
		 ARRAY_D sine_av_wf[2];
		 ARRAY_D fft_wf[2];
		 ARRAY_D fft_volt_wf[2];
		 tout = 5.0;

		 FLEX_RELAY ac_relay;


		 for (k = 0; k < 2; k++) {


		 ON_FIRST_INVOCATION_BEGIN();
		 DISCONNECT();
		 CONNECT();
		 relay.util("MUX0_OUT_K2,MUX0_OUT_K3,SEN01_K1").on();
		 //relay.util("SEN01_K1").on();
		 relay.wait(1.5 ms );
		 relay.execute();

		 ac_relay.pin("@").set("IDLE", "OFF");
		 ac_relay.wait(1.5 ms );
		 ac_relay.execute();

		 ac_relay.pin(
		 "SENADDR0,SENADDR1,SENADDR2,SENADDR3,SENADDR4,SENADDR5,SENADDR6,COMMMODE, BLEN, INPUTMODE, MUXEN, OSCON, ADCTRI, DACTRI").set(
		 "AC", "OFF");
		 //ac_relay.pin("SENGRP").set("AC", "OFF");
		 ac_relay.wait(1.5 ms );
		 ac_relay.execute();


		 ////////////////////AWG setting//////////////////////////////////////////

		 // Definition of the Analog SET

		 ANALOG_SET SET_SINE_SEN1_MUX0_1KHZ(
		 "SET_SINE_SEN1_MUX0_1KHZ");

		 // Definition of the waveform

		 ANALOG_WAVEFORM sinewave_1KHZ("sinewave_1KHZ");

		 sinewave_1KHZ.definition(TM::SINE).periods(3).phase(0).samples(
		 3072).min(-3.0).max(3.0);

		 // Sequencer Programs

		 ANALOG_SEQUENCER SEQ_SINE_SEN1_MUX0_1KHZ(
		 "SEQ_SINE_SEN1_MUX0_1KHZ");

		 SEQ_SINE_SEN1_MUX0_1KHZ.add(TM::NOP, sinewave_1KHZ).add(
		 TM::LOOPI, sinewave_1KHZ, 1, 0);

		 // analog master clock setting

		 SET_SINE_SEN1_MUX0_1KHZ.CLOCK_DOMAIN(2, TM::ANALOG).clock(
		 TM::MCLK_AUTO);
		 SET_SINE_SEN1_MUX0_1KHZ.AWG(AWG_pin).clockDomain(2).rule(
		 TM::AUTOFS).frequency(1000000);

		 // hardware settings

		 SET_SINE_SEN1_MUX0_1KHZ.AWG(AWG_pin).coreFunction(
		 TM::LF).sequencerProgram(
		 SEQ_SINE_SEN1_MUX0_1KHZ).vOffset(0.0).filter(
		 "1.5K").dutImpedance("1000000");
		 SET_SINE_SEN1_MUX0_1KHZ.AWG(AWG_pin).min(0.3).max(0.9);
		 //SET_SINE_SEN1_MUX0_1KHZ.AWG(AWG_pin).dc(1.0).ac(3.0);


		 ///////////////////// DGT setting //////////////////////////

		 // Definition of the sequencer program

		 ANALOG_SEQUENCER SEQ_DGT_SEN1_MUX0_1KHZ(
		 "SEQ_DGT_SEN1_MUX0_1KHZ");

		 SEQ_DGT_SEN1_MUX0_1KHZ.add(TM::HALT, TM::POST, 30720,
		 3072, TM::ONE);

		 // clock setting
		 SET_SINE_SEN1_MUX0_1KHZ.DGT(DGT_pin).clockDomain(2).rule(
		 TM::AUTOFS).frequency(1000000);

		 // hardware settings

		 SET_SINE_SEN1_MUX0_1KHZ.DGT(DGT_pin).coreFunction(
		 TM::VHF).measurementMode("BYPS").sequencerProgram(
		 SEQ_DGT_SEN1_MUX0_1KHZ).filter("15M").impedance(
		 "10K").vRange("2").vOffset(0.0).interDiscard(0).trigDelay(
		 0);

		// Analog.primary("SET_SINE_SEN1_MUX0_1KHZ");


		 ////////////////////AWG setting//////////////////////////////////////////

		 // Definition of the Analog SET

		 ANALOG_SET SET_SINE_SEN1_MUX0_10KHZ(
		 "SET_SINE_SEN1_MUX0_10KHZ");

		 // Definition of the waveform

		 ANALOG_WAVEFORM sinewave_10KHZ("sinewave_10KHZ");

		 sinewave_10KHZ.definition(TM::SINE).periods(27).phase(0).samples(
		 3072).min(-3.0).max(3.0);

		 // Sequencer Programs

		 ANALOG_SEQUENCER SEQ_SINE_SEN1_MUX0_10KHZ(
		 "SEQ_SINE_SEN1_MUX0_10KHZ");

		 SEQ_SINE_SEN1_MUX0_10KHZ.add(TM::NOP, sinewave_10KHZ).add(
		 TM::LOOPI, sinewave_10KHZ, 1, 0);

		 // analog master clock setting

		 SET_SINE_SEN1_MUX0_10KHZ.CLOCK_DOMAIN(2, TM::ANALOG).clock(
		 TM::MCLK_AUTO);
		 SET_SINE_SEN1_MUX0_10KHZ.AWG(AWG_pin).clockDomain(2).rule(
		 TM::AUTOFS).frequency(1000000);

		 // hardware settings

		 SET_SINE_SEN1_MUX0_10KHZ.AWG(AWG_pin).coreFunction(
		 TM::LF).sequencerProgram(
		 SEQ_SINE_SEN1_MUX0_10KHZ).vOffset(0.0).filter(
		 "30K").dutImpedance("1000000");
		 SET_SINE_SEN1_MUX0_10KHZ.AWG(AWG_pin).min(0.3).max(0.9);
		 //SET_SINE_SEN1_MUX0_1KHZ.AWG(AWG_pin).dc(1.0).ac(3.0);



		 ///////////////////// DGT setting //////////////////////////

		 // Definition of the sequencer program

		 ANALOG_SEQUENCER SEQ_DGT_SEN1_MUX0_10KHZ(
		 "SEQ_DGT_SEN1_MUX0_10KHZ");

		 SEQ_DGT_SEN1_MUX0_10KHZ.add(TM::HALT, TM::POST, 30720,
		 3072, TM::ONE);

		 // clock setting
		 SET_SINE_SEN1_MUX0_10KHZ.DGT(DGT_pin).clockDomain(2).rule(
		 TM::AUTOFS).frequency(1000000);

		 // hardware settings

		 SET_SINE_SEN1_MUX0_10KHZ.DGT(DGT_pin).coreFunction(
		 TM::VHF).measurementMode("BYPS").sequencerProgram(
		 SEQ_DGT_SEN1_MUX0_10KHZ).filter("15M").impedance(
		 "10K").vRange("2").vOffset(0.0).interDiscard(0).trigDelay(
		 0);


		 //Analog.primary("SET_SINE_SEN1_MUX0_10KHZ");


		 if(k == 0) {

			 Analog.primary("SET_SINE_SEN1_MUX0_1KHZ");

		 } else if(k == 1) {

			 Analog.primary("SET_SINE_SEN1_MUX0_10KHZ");

		 }


		 Routing.pin(AWG_pin).connect(TM::SINGLE);
		 AWG(AWG_pin).enable();

		 Routing.pin(DGT_pin).connect(TM::SINGLE);
		 DGT(DGT_pin).enable();
		 FLUSH();


		 //FUNCTIONAL_TEST(); //Need to Pre-Running
		 EXECUTE_TEST();


		 WAIT_TEST_DONE(tout);


		 relay.util("MUX0_OUT_K2,MUX0_OUT_K3,SEN01_K1").off();
		 relay.wait(1.5 ms );
		 relay.execute();


		 WAIT_TEST_DONE(tout, &tout_site_flag, &mis_trg_site);
		 //triggers (0) in a test or not (1) and returns the status to mis_trg_site variable.

		 //SET_SINE_SEN1_MUX0_1KHZ.remove();

		 ON_FIRST_INVOCATION_END();


		 sine_unav_wf[k] = DGT(DGT_pin).getWaveform();

		 DGT(DGT_pin).disable();
		 Routing.pin(DGT_pin).disconnect();
		 AWG(AWG_pin).disable();
		 Routing.pin(AWG_pin).disconnect();
		 FLUSH();

		 sine_av_wf[k].resize(sine_av_samples);

		 for (i = 0; i < sine_av_samples; i++) {
		 sine_av_wf[k][i] = 0.0;
		 }

		 for (i = 0; i < sine_av_samples; i++) {
		 for (j = 0; j < sine_av; j++) {
		 sine_av_wf[k][i] = sine_av_wf[k][i] + sine_unav_wf[k][i + j
		 * sine_av_samples];
		 }
		 }

		 for (i = 0; i < sine_av_samples; i++) {
		 sine_av_wf[k][i] = sine_av_wf[k][i] / sine_av;
		 }

		 fft_wf[0].resize(sine_av_samples / 2);
		 DSP_SPECTRUM(sine_av_wf[k], fft_wf[k], DB, 0.0, RECT, 0);
		 DSP_SPECTRUM(sine_av_wf[k], fft_volt_wf[k], VOLT, 1.414, RECT, 0);

		 PUT_DEBUG(DGT_pin, unaverage_wave[k], sine_unav_wf[k]);
		 PUT_DEBUG(DGT_pin, average_wave[k], sine_av_wf[k]);
		 PUT_DEBUG(DGT_pin, fwave_DB[k], fft_wf[k]);
		 PUT_DEBUG(DGT_pin, fwave_VT[k], fft_volt_wf[k]);

		 #if 0
		 double snr, signal_power, noise_power;
		 DSP_SNR(sine_av_wf[k], &snr, &signal_power, &noise_power, 1024, num_cycle_DAC, num_harmonics_DAC, 1.414, NULL);
		 cout << "Site " << CURRENT_SITE_NUMBER() << " snr : " << snr << "dB" << endl;
		 cout << "Site " << CURRENT_SITE_NUMBER() << " signal_power : " << signal_power << "dB" << endl;
		 cout << "Site " << CURRENT_SITE_NUMBER() << " noise_power : " << noise_power << "dB" << endl;
		 cout << "Site " << CURRENT_SITE_NUMBER() << " signal_power-noise_power : " << signal_power-noise_power << endl;

		 double snd, snd_signal_power, other_power;
		 DSP_SND(sine_av_wf[k], &snd, &snd_signal_power, &other_power, 1024, num_cycle_DAC, 1.414, NULL);
		 cout << "Site " << CURRENT_SITE_NUMBER() << " snd_signal_power : " << snd_signal_power << "dB" << endl;
		 cout << "Site " << CURRENT_SITE_NUMBER() << " other_power : " << other_power << "dB" << endl;
		 cout << "Site " << CURRENT_SITE_NUMBER() << " snd_signal_power-other_power : " << snd_signal_power-other_power << endl;
		 #endif

		 if(k == 0){
		 DSP_THD(sine_av_wf[k], &thd_result[k], first_num_cycle_DAC,
		 num_harmonics_DAC, DB, 0);

		 cout << "1K Sine Result" << endl;


		 }
		 else if(k == 1) {

			 DSP_THD(sine_av_wf[k], &thd_result[k], second_num_cycle_DAC,
			 num_harmonics_DAC, DB, 0);

			 cout << "10K Sine Result" << endl;

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
		 cout << "Site " << CURRENT_SITE_NUMBER() << " DAC SNR : "
		 << thd_result[k].snr << "dB" << endl;
		 }
		 if (debug) {
		 cout << "Site " << CURRENT_SITE_NUMBER() << " DAC THD : "
		 << thd_result[k].thd << "dB" << endl;
		 }
		 if (debug) {
		 cout << "Site " << CURRENT_SITE_NUMBER() << " DAC SNDR : "
		 << thd_result[k].snd << "dB" << endl;
		 }

		 TEST("NOTRIG_FLAG", "NOTRIG_FLAG", LIMIT(TM::GE, 1.0, TM::LE, 1.0),
		 NOTRIG_FLAG, TM::CONTINUE);
		 TEST(DGT_pin, "DAC_SNR", LIMIT(TM::GE, 40.0, TM::LE, 60.0),
		 thd_result[k].snr, TM::CONTINUE);
		 TEST(DGT_pin, "DAC_THD", LIMIT(TM::GE, -65.0, TM::LE, -40.0),
		 thd_result[k].thd, TM::CONTINUE);
		 TEST(DGT_pin, "DAC_SNDR", LIMIT(TM::GE, 40.0, TM::LE, 60.0),
		 thd_result[k].snd, TM::CONTINUE);
		 }

/*
		ON_FIRST_INVOCATION_BEGIN();

				relay.util("MUX0_OUT_K2,MUX0_OUT_K3,SEN01_K1").on();
				//relay.util("SEN01_K1").on();
				relay.wait(1.5 ms );
				relay.execute();

				ac_relay.pin("@").set("IDLE", "OFF");
				ac_relay.wait(1.5 ms );
				ac_relay.execute();

				ac_relay.pin(
						"SENADDR0,SENADDR1,SENADDR2,SENADDR3,SENADDR4,SENADDR5,SENADDR6,COMMMODE, BLEN, INPUTMODE, MUXEN, OSCON, ADCTRI, DACTRI").set(
						"AC", "OFF");
				//ac_relay.pin("SENGRP").set("AC", "OFF");
				ac_relay.wait(1.5 ms );
				ac_relay.execute();

				////////////////////AWG setting//////////////////////////////////////////

				// Definition of the Analog SET

				ANALOG_SET SET_SINE_SEN1_MUX0_10KHZ("SET_SINE_SEN1_MUX0_10KHZ");

				// Definition of the waveform

				ANALOG_WAVEFORM sinewave_10KHZ("sinewave_10KHZ");

				sinewave_10KHZ.definition(TM::SINE).periods(27).phase(0).samples(
						3072).min(-3.0).max(3.0);

				// Sequencer Programs

				ANALOG_SEQUENCER SEQ_SINE_SEN1_MUX0_10KHZ(
						"SEQ_SINE_SEN1_MUX0_1KHZ");

				SEQ_SINE_SEN1_MUX0_10KHZ.add(TM::NOP, sinewave_10KHZ).add(
						TM::LOOPI, sinewave_10KHZ, 1, 0);

				// analog master clock setting

				SET_SINE_SEN1_MUX0_10KHZ.CLOCK_DOMAIN(4, TM::ANALOG).clock(
						TM::MCLK_AUTO);
				SET_SINE_SEN1_MUX0_10KHZ.AWG(AWG_pin).clockDomain(4).rule(
						TM::AUTOFS).frequency(1000000);

				// hardware settings

				SET_SINE_SEN1_MUX0_10KHZ.AWG(AWG_pin).coreFunction(TM::LF).sequencerProgram(
						SEQ_SINE_SEN1_MUX0_10KHZ).vOffset(0.0).filter("1.5K").dutImpedance(
						"1000000");
				SET_SINE_SEN1_MUX0_10KHZ.AWG(AWG_pin).min(0.0).max(0.6);
				//SET_SINE_SEN1_MUX0_1KHZ.AWG(AWG_pin).dc(1.0).ac(3.0);


				///////////////////// DGT setting //////////////////////////

				// Definition of the sequencer program

				ANALOG_SEQUENCER SEQ_DGT_SEN1_MUX0_10KHZ(
						"SEQ_DGT_SEN1_MUX0_10KHZ");

				SEQ_DGT_SEN1_MUX0_10KHZ.add(TM::HALT, TM::POST, 30720, 3072,
						TM::ONE);

				// clock setting
				SET_SINE_SEN1_MUX0_10KHZ.DGT(DGT_pin).clockDomain(4).rule(
						TM::AUTOFS).frequency(1000000);

				// hardware settings

				SET_SINE_SEN1_MUX0_10KHZ.DGT(DGT_pin).coreFunction(TM::VHF).measurementMode(
						"BYPS").sequencerProgram(SEQ_DGT_SEN1_MUX0_10KHZ).filter(
						"15M").impedance("10K").vRange("2").vOffset(0.0).interDiscard(
						0).trigDelay(0);

				Analog.primary("SET_SINE_SEN1_MUX0_10KHZ");

				Routing.pin(AWG_pin).connect(TM::SINGLE);
				AWG(AWG_pin).enable();

				Routing.pin(DGT_pin).connect(TM::SINGLE);
				DGT(DGT_pin).enable();
				//FLUSH();

				//	FUNCTIONAL_TEST(); //Need to Pre-Running
				EXECUTE_TEST();
				WAIT_TEST_DONE(tout);

				relay.util("MUX0_OUT_K2,MUX0_OUT_K3,SEN01_K1").off();
				relay.wait(1.5 ms );
				relay.execute();

				WAIT_TEST_DONE(tout, &tout_site_flag, &mis_trg_site);
				//triggers (0) in a test or not (1) and returns the status to mis_trg_site variable.

				//SET_SINE_SEN1_MUX0_1KHZ.remove();

			ON_FIRST_INVOCATION_END();

		sine_unav_wf = DGT(DGT_pin).getWaveform();

		DGT(DGT_pin).disable();
		Routing.pin(DGT_pin).disconnect();
		AWG(AWG_pin).disable();
		Routing.pin(AWG_pin).disconnect();
		FLUSH();

		sine_av_wf.resize(sine_av_samples);

		for (i = 0; i < sine_av_samples; i++) {
			sine_av_wf[i] = 0.0;
		}

		for (i = 0; i < sine_av_samples; i++) {
			for (j = 0; j < sine_av; j++) {
				sine_av_wf[i] = sine_av_wf[i] + sine_unav_wf[i + j
						* sine_av_samples];
			}
		}

		for (i = 0; i < sine_av_samples; i++) {
			sine_av_wf[i] = sine_av_wf[i] / sine_av;
		}

		fft_wf.resize(sine_av_samples / 2);
		DSP_SPECTRUM(sine_av_wf, fft_wf, DB, 0.0, RECT, 0);
		DSP_SPECTRUM(sine_av_wf, fft_volt_wf, VOLT, 1.414, RECT, 0);

		PUT_DEBUG(DGT_pin, "Sine unav", sine_unav_wf);
		PUT_DEBUG(DGT_pin, "Sine av", sine_av_wf);
		PUT_DEBUG(DGT_pin, "FFT wf", fft_wf);
		PUT_DEBUG(DGT_pin, "FFT_VOLT wf", fft_volt_wf);

#if 0
		double snr, signal_power, noise_power;
		DSP_SNR(sine_av_wf, &snr, &signal_power, &noise_power, 1024, num_cycle_DAC, num_harmonics_DAC, 1.414, NULL);
		cout << "Site " << CURRENT_SITE_NUMBER() << " snr : " << snr << "dB" << endl;
		cout << "Site " << CURRENT_SITE_NUMBER() << " signal_power : " << signal_power << "dB" << endl;
		cout << "Site " << CURRENT_SITE_NUMBER() << " noise_power : " << noise_power << "dB" << endl;
		cout << "Site " << CURRENT_SITE_NUMBER() << " signal_power-noise_power : " << signal_power-noise_power << endl;

		double snd, snd_signal_power, other_power;
		DSP_SND(sine_av_wf, &snd, &snd_signal_power, &other_power, 1024, num_cycle_DAC, 1.414, NULL);
		cout << "Site " << CURRENT_SITE_NUMBER() << " snd_signal_power : " << snd_signal_power << "dB" << endl;
		cout << "Site " << CURRENT_SITE_NUMBER() << " other_power : " << other_power << "dB" << endl;
		cout << "Site " << CURRENT_SITE_NUMBER() << " snd_signal_power-other_power : " << snd_signal_power-other_power << endl;
#endif

		DSP_THD(sine_av_wf, &thd_result, num_cycle_DAC, num_harmonics_DAC, DB,
				0);

		curr_site = CURRENT_SITE_NUMBER();
		if ((mis_trg_site >> (curr_site - 1)) & 1) {
			cout << "Trigger error on site" << curr_site << endl;
			TRIG_FLAG[CURRENT_SITE_NUMBER() - 1] = 1;
		} else {
			cout << "Triggered good on site" << curr_site << endl;
			TRIG_FLAG[CURRENT_SITE_NUMBER() - 1] = 0;
		}

		if (debug) {
			cout << "Site " << CURRENT_SITE_NUMBER() << " 10K_DAC SNR : "
					<< thd_result.snr << "dB" << endl;
		}
		if (debug) {
			cout << "Site " << CURRENT_SITE_NUMBER() << " 10K_DAC THD : "
					<< thd_result.thd << "dB" << endl;
		}
		if (debug) {
			cout << "Site " << CURRENT_SITE_NUMBER() << " 10K_DAC SNDR : "
					<< thd_result.snd << "dB" << endl;
		}

		TEST("NOTRIG_FLAG", "NOTRIG_FLAG", LIMIT(TM::GE, 1.0, TM::LE, 1.0),
				NOTRIG_FLAG, TM::CONTINUE);
		TEST(DGT_pin, "10K_DAC_SNR", LIMIT(TM::GE, 40.0, TM::LE, 60.0),
				thd_result.snr, TM::CONTINUE);
		TEST(DGT_pin, "10K_DAC_THD", LIMIT(TM::GE, -65.0, TM::LE, -40.0),
				thd_result.thd, TM::CONTINUE);
		TEST(DGT_pin, "10K_DAC_SNDR", LIMIT(TM::GE, 40.0, TM::LE, 60.0),
				thd_result.snd, TM::CONTINUE);
*/

		DISCONNECT();

		return;
	}

	virtual void postParameterChange(const string& parameterIdentifier) {
		return;
	}

	virtual const string getComment() const {
		string comment = "DAC RAMP Distortion Test.";
		return comment;
	}
};
REGISTER_TESTMETHOD("THD_TEST", THD_TEST)
;
