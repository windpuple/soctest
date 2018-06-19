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

class ALPHA_TEST: public testmethod::TestMethod {

protected:
	string DGT_pin;
	string AWG_pin;
	//string  SMC_Distortion_handleName;
	int num_cycle_DAC_1;
	int num_cycle_DAC_2;
	int num_cycle_DAC_3;
	int num_cycle_DAC_4;
	int num_cycle_DAC_5;
	int num_cycle_DAC_6;
	int num_cycle_DAC_7;
	int num_cycle_DAC_8;
	int num_cycle_DAC_9;
	int num_cycle_DAC_10;

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

		addParameter("num_cycle_DAC_1", "int", &num_cycle_DAC_1,
				testmethod::TM_PARAMETER_INPUT) .setComment(
				"the first sinecycle number of signal used to test DAC");
		addParameter("num_cycle_DAC_2", "int", &num_cycle_DAC_2,
				testmethod::TM_PARAMETER_INPUT) .setComment(
				"the second sine cycle number of signal used t4o test DAC");
		addParameter("num_cycle_DAC_3", "int", &num_cycle_DAC_3,
				testmethod::TM_PARAMETER_INPUT) .setComment(
				"the second sine cycle number of signal used to test DAC");
		addParameter("num_cycle_DAC_4", "int", &num_cycle_DAC_4,
				testmethod::TM_PARAMETER_INPUT) .setComment(
				"the second sine cycle number of signal used to test DAC");
		addParameter("num_cycle_DAC_5", "int", &num_cycle_DAC_5,
				testmethod::TM_PARAMETER_INPUT) .setComment(
				"the second sine cycle number of signal used to test DAC");
		addParameter("num_cycle_DAC_6", "int", &num_cycle_DAC_6,
				testmethod::TM_PARAMETER_INPUT) .setComment(
				"the second sine cycle number of signal used to test DAC");
		addParameter("num_cycle_DAC_7", "int", &num_cycle_DAC_7,
				testmethod::TM_PARAMETER_INPUT) .setComment(
				"the second sine cycle number of signal used to test DAC");
		addParameter("num_cycle_DAC_8", "int", &num_cycle_DAC_8,
				testmethod::TM_PARAMETER_INPUT) .setComment(
				"the second sine cycle number of signal used to test DAC");
		addParameter("num_cycle_DAC_9", "int", &num_cycle_DAC_9,
				testmethod::TM_PARAMETER_INPUT) .setComment(
				"the second sine cycle number of signal used to test DAC");
		addParameter("num_cycle_DAC_10", "int", &num_cycle_DAC_10,
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
		THDtype thd_result[10];
		/*
		 ARRAY_D sine_unav_wf;
		 ARRAY_D sine_av_wf;
		 ARRAY_D fft_wf;
		 ARRAY_D fft_volt_wf;
		 tout = 5.0;

		 FLEX_RELAY ac_relay;
		 */

		string unaverage_wave[10] = { "1K_Sine unav", "10K_Sine unav",
				"95K_Sine unav", "950K_Sine unav", "2M_Sine unav", "9.2M_Sine unav",
				"29.2M_Sine unav", "39.2M_Sine unav", "50M_Sine unav" };
		string average_wave[10] = { "1K_Sine av", "10K_Sine av", "95K_Sine av",
				"950K_Sine av", "2M_Sine av", "9.2M_Sine av", "29.2M_Sine av",
				"39.2M_Sine av", "50M_Sine av" };
		string fwave_DB[10] = { "1K_F_wave_db", "10K_F_wave_db",
				"95K_F_wave_db", "950K_F_wave_db", "2M_F_wave_db", "9.2M_F_wave_db",
				"29.2M_F_wave_db", "39.2M_F_wave_db", "50M_F_wave_db" };
		string fwave_VT[10] = { "1K_F_wave_VT", "10K_F_wave_VT",
				"95K_F_wave_VT", "950K_F_wave_VT", "2M_F_wave_VT", "9.2M_F_wave_VT",
				"29.2M_F_wave_VT", "39.2M_F_wave_VT", "50M_F_wave_VT" };

		ARRAY_D sine_unav_wf[10];
		ARRAY_D sine_av_wf[10];
		ARRAY_D fft_wf[10];
		ARRAY_D fft_volt_wf[10];
		tout = 5.0;

		FLEX_RELAY ac_relay;

		for (k = 0; k < 9; k++) {
		//for (k = 0; k < 5; k++) {

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

					SET_SINE_SEN1_MUX0_1KHZ.AWG(AWG_pin).coreFunction(TM::LF).sequencerProgram(
							SEQ_SINE_SEN1_MUX0_1KHZ).vOffset(0.0).filter("1.5K").dutImpedance(
							"1000000");
					SET_SINE_SEN1_MUX0_1KHZ.AWG(AWG_pin).min(0.3).max(0.9);
					//SET_SINE_SEN1_MUX0_1KHZ.AWG(AWG_pin).dc(1.0).ac(3.0);


					///////////////////// DGT setting //////////////////////////

					// Definition of the sequencer program

					ANALOG_SEQUENCER SEQ_DGT_SEN1_MUX0_1KHZ(
							"SEQ_DGT_SEN1_MUX0_1KHZ");

					SEQ_DGT_SEN1_MUX0_1KHZ.add(TM::HALT, TM::POST, 30720, 3072,
							TM::ONE);

					// clock setting
					SET_SINE_SEN1_MUX0_1KHZ.DGT(DGT_pin).clockDomain(2).rule(
							TM::AUTOFS).frequency(1000000);

					// hardware settings

					SET_SINE_SEN1_MUX0_1KHZ.DGT(DGT_pin).coreFunction(TM::VHF).measurementMode(
							"BYPS").sequencerProgram(SEQ_DGT_SEN1_MUX0_1KHZ).filter(
							"15M").impedance("10K").vRange("2").vOffset(0.0).interDiscard(
							0).trigDelay(0);

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

					SET_SINE_SEN1_MUX0_10KHZ.AWG(AWG_pin).coreFunction(TM::LF).sequencerProgram(
							SEQ_SINE_SEN1_MUX0_10KHZ).vOffset(0.0).filter("30K").dutImpedance(
							"1000000");
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

					SET_SINE_SEN1_MUX0_10KHZ.DGT(DGT_pin).coreFunction(TM::VHF).measurementMode(
							"BYPS").sequencerProgram(SEQ_DGT_SEN1_MUX0_10KHZ).filter(
							"15M").impedance("10K").vRange("2").vOffset(0.0).interDiscard(
							0).trigDelay(0);

					//Analog.primary("SET_SINE_SEN1_MUX0_10KHZ");

					////////////////////95K SINE/////////////////////////////////////////////
					////////////////////AWG setting//////////////////////////////////////////

					// Definition of the Analog SET

					ANALOG_SET SET_SINE_SEN1_MUX0_95KHZ(
							"SET_SINE_SEN1_MUX0_95KHZ");

					// Definition of the waveform

					ANALOG_WAVEFORM sinewave_95KHZ("sinewave_95KHZ");

					sinewave_95KHZ.definition(TM::SINE).periods(29).phase(0).samples(
							3072).min(-1.25).max(1.25);

					// Sequencer Programs

					ANALOG_SEQUENCER SEQ_SINE_SEN1_MUX0_95KHZ(
							"SEQ_SINE_SEN1_MUX0_95KHZ");

					SEQ_SINE_SEN1_MUX0_95KHZ.add(TM::NOP, sinewave_95KHZ).add(
							TM::LOOPI, sinewave_95KHZ, 1, 0);

					// analog master clock setting

					SET_SINE_SEN1_MUX0_95KHZ.CLOCK_DOMAIN(2, TM::ANALOG).clock(
							TM::MCLK_AUTO);
					SET_SINE_SEN1_MUX0_95KHZ.AWG(AWG_pin).clockDomain(2).rule(
							TM::AUTOFS).frequency(10000000);

					// hardware settings

					SET_SINE_SEN1_MUX0_95KHZ.AWG(AWG_pin).coreFunction(TM::HF).sequencerProgram(
							SEQ_SINE_SEN1_MUX0_95KHZ).vOffset(0.0).filter(
							"1.5M").dutImpedance("1000000");
					SET_SINE_SEN1_MUX0_95KHZ.AWG(AWG_pin).min(0.3).max(0.9);
					//SET_SINE_SEN1_MUX0_1KHZ.AWG(AWG_pin).dc(1.0).ac(3.0);


					///////////////////// DGT setting //////////////////////////

					// Definition of the sequencer program

					ANALOG_SEQUENCER SEQ_DGT_SEN1_MUX0_95KHZ(
							"SEQ_DGT_SEN1_MUX0_95KHZ");

					SEQ_DGT_SEN1_MUX0_95KHZ.add(TM::HALT, TM::POST, 30720,
							3072, TM::ONE);

					// clock setting
					SET_SINE_SEN1_MUX0_95KHZ.DGT(DGT_pin).clockDomain(2).rule(
							TM::AUTOFS).frequency(10000000);

					// hardware settings

					SET_SINE_SEN1_MUX0_95KHZ.DGT(DGT_pin).coreFunction(TM::VHF).measurementMode(
							"BYPS").sequencerProgram(SEQ_DGT_SEN1_MUX0_95KHZ).filter(
							"15M").impedance("10K").vRange("2").vOffset(0.0).interDiscard(
							0).trigDelay(0);

					////////////////////950K SINE/////////////////////////////////////////////
					////////////////////AWG setting//////////////////////////////////////////

					// Definition of the Analog SET

					ANALOG_SET SET_SINE_SEN1_MUX0_950KHZ(
							"SET_SINE_SEN1_MUX0_950KHZ");

					// Definition of the waveform

					ANALOG_WAVEFORM sinewave_950KHZ("sinewave_950KHZ");

					sinewave_950KHZ.definition(TM::SINE).periods(29).phase(0).samples(
							3072).min(-1.25).max(1.25);

					// Sequencer Programs

					ANALOG_SEQUENCER SEQ_SINE_SEN1_MUX0_950KHZ(
							"SEQ_SINE_SEN1_MUX0_950KHZ");

					SEQ_SINE_SEN1_MUX0_950KHZ.add(TM::NOP, sinewave_950KHZ).add(
							TM::LOOPI, sinewave_950KHZ, 1, 0);

					// analog master clock setting

					SET_SINE_SEN1_MUX0_950KHZ.CLOCK_DOMAIN(2, TM::ANALOG).clock(
							TM::MCLK_AUTO);
					SET_SINE_SEN1_MUX0_950KHZ.AWG(AWG_pin).clockDomain(2).rule(
							TM::AUTOFS).frequency(100000000);

					// hardware settings

					SET_SINE_SEN1_MUX0_950KHZ.AWG(AWG_pin).coreFunction(TM::HF).sequencerProgram(
							SEQ_SINE_SEN1_MUX0_950KHZ).vOffset(0.0).filter(
							"15M").dutImpedance("1000000");
					SET_SINE_SEN1_MUX0_950KHZ.AWG(AWG_pin).min(0.3).max(0.9);
					//SET_SINE_SEN1_MUX0_1KHZ.AWG(AWG_pin).dc(1.0).ac(3.0);


					///////////////////// DGT setting //////////////////////////

					// Definition of the sequencer program

					ANALOG_SEQUENCER SEQ_DGT_SEN1_MUX0_950KHZ(
							"SEQ_DGT_SEN1_MUX0_950KHZ");

					SEQ_DGT_SEN1_MUX0_950KHZ.add(TM::HALT, TM::POST, 30720,
							3072, TM::ONE);

					// clock setting
					SET_SINE_SEN1_MUX0_950KHZ.DGT(DGT_pin).clockDomain(2).rule(
							TM::AUTOFS).frequency(100000000);

					// hardware settings

					SET_SINE_SEN1_MUX0_950KHZ.DGT(DGT_pin).coreFunction(TM::VHF).measurementMode(
							"BYPS").sequencerProgram(SEQ_DGT_SEN1_MUX0_950KHZ).filter(
							"15M").impedance("10K").vRange("2").vOffset(0.0).interDiscard(
							0).trigDelay(0);

					////////////////////2M SINE/////////////////////////////////////////////
					////////////////////AWG setting//////////////////////////////////////////

					// Definition of the Analog SET

					ANALOG_SET SET_SINE_SEN1_MUX0_2MHZ(
							"SET_SINE_SEN1_MUX0_2MHZ");

					// Definition of the waveform

					ANALOG_WAVEFORM sinewave_2MHZ("sinewave_2MHZ");

					sinewave_2MHZ.definition(TM::SINE).periods(61).phase(0).samples(
							3072).min(-1.25).max(1.25);

					// Sequencer Programs

					ANALOG_SEQUENCER SEQ_SINE_SEN1_MUX0_2MHZ(
							"SEQ_SINE_SEN1_MUX0_2MHZ");

					SEQ_SINE_SEN1_MUX0_2MHZ.add(TM::NOP, sinewave_2MHZ).add(
							TM::LOOPI, sinewave_2MHZ, 1, 0);

					// analog master clock setting

					SET_SINE_SEN1_MUX0_2MHZ.CLOCK_DOMAIN(2, TM::ANALOG).clock(
							TM::MCLK_AUTO);
					SET_SINE_SEN1_MUX0_2MHZ.AWG(AWG_pin).clockDomain(2).rule(
							TM::AUTOFS).frequency(100000000);

					// hardware settings

					SET_SINE_SEN1_MUX0_2MHZ.AWG(AWG_pin).coreFunction(TM::HF).sequencerProgram(
							SEQ_SINE_SEN1_MUX0_2MHZ).vOffset(0.0).filter(
							"15M").dutImpedance("1000000");
					SET_SINE_SEN1_MUX0_2MHZ.AWG(AWG_pin).min(0.3).max(0.9);
					SET_SINE_SEN1_MUX0_1KHZ.AWG(AWG_pin).dc(0.0).ac(6.0);


					///////////////////// DGT setting //////////////////////////

					// Definition of the sequencer program

					ANALOG_SEQUENCER SEQ_DGT_SEN1_MUX0_2MHZ(
							"SEQ_DGT_SEN1_MUX0_2MHZ");

					SEQ_DGT_SEN1_MUX0_2MHZ.add(TM::HALT, TM::POST, 30720,
							3072, TM::ONE);

					// clock setting
					SET_SINE_SEN1_MUX0_2MHZ.DGT(DGT_pin).clockDomain(2).rule(
							TM::AUTOFS).frequency(100000000);

					// hardware settings

					SET_SINE_SEN1_MUX0_2MHZ.DGT(DGT_pin).coreFunction(TM::VHF).measurementMode(
							"BYPS").sequencerProgram(SEQ_DGT_SEN1_MUX0_2MHZ).filter(
							"15M").impedance("10K").vRange("2").vOffset(0.0).interDiscard(
							0).trigDelay(0);

					////////////////////9.5M SINE/////////////////////////////////////////////
					////////////////////AWG setting//////////////////////////////////////////

					// Definition of the Analog SET

					ANALOG_SET SET_SINE_SEN1_MUX0_9_5MHZ(
							"SET_SINE_SEN1_MUX0_9_5MHZ");

					// Definition of the waveform

					ANALOG_WAVEFORM sinewave_9_5MHZ("sinewave_9_5MHZ");

					sinewave_9_5MHZ.definition(TM::SINE).periods(171).phase(0).samples(
							3072).min(-1.25).max(1.25);

					// Sequencer Programs

					ANALOG_SEQUENCER SEQ_SINE_SEN1_MUX0_9_5MHZ(
							"SEQ_SINE_SEN1_MUX0_9_5MHZ");

					SEQ_SINE_SEN1_MUX0_9_5MHZ.add(TM::NOP, sinewave_9_5MHZ).add(
							TM::LOOPI, sinewave_9_5MHZ, 1, 0);

					// analog master clock setting

					SET_SINE_SEN1_MUX0_9_5MHZ.CLOCK_DOMAIN(2, TM::ANALOG).clock(
							TM::MCLK_AUTO);
					SET_SINE_SEN1_MUX0_9_5MHZ.AWG(AWG_pin).clockDomain(2).rule(
							TM::AUTOFS).frequency(170000000);

					// hardware settings

					SET_SINE_SEN1_MUX0_9_5MHZ.AWG(AWG_pin).coreFunction(TM::VHF).sequencerProgram(
							SEQ_SINE_SEN1_MUX0_9_5MHZ).vOffset(0.0).filter(
							"58M").dutImpedance("1000000");
					SET_SINE_SEN1_MUX0_9_5MHZ.AWG(AWG_pin).min(0.3).max(0.9);
					SET_SINE_SEN1_MUX0_1KHZ.AWG(AWG_pin).dc(0.0).ac(0.6);

					///////////////////// DGT setting //////////////////////////

					// Definition of the sequencer program

					ANALOG_SEQUENCER SEQ_DGT_SEN1_MUX0_9_5MHZ(
							"SEQ_DGT_SEN1_MUX0_9_5MHZ");

					SEQ_DGT_SEN1_MUX0_9_5MHZ.add(TM::HALT, TM::POST, 30720,
							3072, TM::ONE);

					// clock setting
					SET_SINE_SEN1_MUX0_9_5MHZ.DGT(DGT_pin).clockDomain(2).rule(
							TM::AUTOFS).frequency(170000000);

					// hardware settings

					SET_SINE_SEN1_MUX0_9_5MHZ.DGT(DGT_pin).coreFunction(TM::VHF).measurementMode(
							"BYPS").sequencerProgram(SEQ_DGT_SEN1_MUX0_9_5MHZ).filter(
							"105M").impedance("10K").vRange("2").vOffset(1.0).interDiscard(
							0).trigDelay(0);

					////////////////////29.5M SINE/////////////////////////////////////////////
					////////////////////AWG setting//////////////////////////////////////////

					// Definition of the Analog SET

					ANALOG_SET SET_SINE_SEN1_MUX0_29_5MHZ(
							"SET_SINE_SEN1_MUX0_29_5MHZ");

					// Definition of the waveform

					ANALOG_WAVEFORM sinewave_29_5MHZ("sinewave_29_5MHZ");

					sinewave_29_5MHZ.definition(TM::SINE).periods(533).phase(0).samples(
							3072).min(-1.25).max(1.25);

					// Sequencer Programs

					ANALOG_SEQUENCER SEQ_SINE_SEN1_MUX0_29_5MHZ(
							"SEQ_SINE_SEN1_MUX0_29_5MHZ");

					SEQ_SINE_SEN1_MUX0_29_5MHZ.add(TM::NOP, sinewave_29_5MHZ).add(
							TM::LOOPI, sinewave_29_5MHZ, 1, 0);

					// analog master clock setting

					SET_SINE_SEN1_MUX0_29_5MHZ.CLOCK_DOMAIN(2, TM::ANALOG).clock(
							TM::MCLK_AUTO);
					SET_SINE_SEN1_MUX0_29_5MHZ.AWG(AWG_pin).clockDomain(2).rule(
							TM::AUTOFS).frequency(170000000);

					// hardware settings

					SET_SINE_SEN1_MUX0_29_5MHZ.AWG(AWG_pin).coreFunction(
							TM::VHF).sequencerProgram(
							SEQ_SINE_SEN1_MUX0_29_5MHZ).vOffset(0.0).filter(
							"58M").dutImpedance("1000000");
					SET_SINE_SEN1_MUX0_29_5MHZ.AWG(AWG_pin).min(0.3).max(0.9);
					SET_SINE_SEN1_MUX0_1KHZ.AWG(AWG_pin).dc(0.0).ac(0.6);

					///////////////////// DGT setting //////////////////////////

					// Definition of the sequencer program

					ANALOG_SEQUENCER SEQ_DGT_SEN1_MUX0_29_5MHZ(
							"SEQ_DGT_SEN1_MUX0_29_5MHZ");

					SEQ_DGT_SEN1_MUX0_29_5MHZ.add(TM::HALT, TM::POST, 30720,
							3072, TM::ONE);

					// clock setting
					SET_SINE_SEN1_MUX0_29_5MHZ.DGT(DGT_pin).clockDomain(2).rule(
							TM::AUTOFS).frequency(170000000);

					// hardware settings

					SET_SINE_SEN1_MUX0_29_5MHZ.DGT(DGT_pin).coreFunction(
							TM::VHF).measurementMode("BYPS").sequencerProgram(
							SEQ_DGT_SEN1_MUX0_29_5MHZ).filter("105M").impedance(
							"10K").vRange("2").vOffset(1.0).interDiscard(0).trigDelay(
							0);

					////////////////////39.5M SINE/////////////////////////////////////////////
					////////////////////AWG setting//////////////////////////////////////////

					// Definition of the Analog SET

					ANALOG_SET SET_SINE_SEN1_MUX0_39_5MHZ(
							"SET_SINE_SEN1_MUX0_39_5MHZ");

					// Definition of the waveform

					ANALOG_WAVEFORM sinewave_39_5MHZ("sinewave_39_5MHZ");

					sinewave_39_5MHZ.definition(TM::SINE).periods(713).phase(0).samples(
							3072).min(-1.25).max(1.25);

					// Sequencer Programs

					ANALOG_SEQUENCER SEQ_SINE_SEN1_MUX0_39_5MHZ(
							"SEQ_SINE_SEN1_MUX0_39_5MHZ");

					SEQ_SINE_SEN1_MUX0_39_5MHZ.add(TM::NOP, sinewave_39_5MHZ).add(
							TM::LOOPI, sinewave_39_5MHZ, 1, 0);

					// analog master clock setting

					SET_SINE_SEN1_MUX0_39_5MHZ.CLOCK_DOMAIN(2, TM::ANALOG).clock(
							TM::MCLK_AUTO);
					SET_SINE_SEN1_MUX0_39_5MHZ.AWG(AWG_pin).clockDomain(2).rule(
							TM::AUTOFS).frequency(170000000);

					// hardware settings

					SET_SINE_SEN1_MUX0_39_5MHZ.AWG(AWG_pin).coreFunction(
							TM::VHF).sequencerProgram(
							SEQ_SINE_SEN1_MUX0_39_5MHZ).vOffset(0.0).filter(
							"58M").dutImpedance("1000000");
					SET_SINE_SEN1_MUX0_39_5MHZ.AWG(AWG_pin).min(0.3).max(0.9);
					//SET_SINE_SEN1_MUX0_1KHZ.AWG(AWG_pin).dc(1.0).ac(3.0);


					///////////////////// DGT setting //////////////////////////

					// Definition of the sequencer program

					ANALOG_SEQUENCER SEQ_DGT_SEN1_MUX0_39_5MHZ(
							"SEQ_DGT_SEN1_MUX0_39_5MHZ");

					SEQ_DGT_SEN1_MUX0_39_5MHZ.add(TM::HALT, TM::POST, 30720,
							3072, TM::ONE);

					// clock setting
					SET_SINE_SEN1_MUX0_39_5MHZ.DGT(DGT_pin).clockDomain(2).rule(
							TM::AUTOFS).frequency(170000000);

					// hardware settings

					SET_SINE_SEN1_MUX0_39_5MHZ.DGT(DGT_pin).coreFunction(
							TM::VHF).measurementMode("BYPS").sequencerProgram(
							SEQ_DGT_SEN1_MUX0_39_5MHZ).filter("105M").impedance(
							"10K").vRange("2").vOffset(1.0).interDiscard(0).trigDelay(
							0);

					////////////////////50M SINE/////////////////////////////////////////////
					////////////////////AWG setting//////////////////////////////////////////

					// Definition of the Analog SET

					ANALOG_SET SET_SINE_SEN1_MUX0_50MHZ(
							"SET_SINE_SEN1_MUX0_50MHZ");

					// Definition of the waveform

					ANALOG_WAVEFORM sinewave_50MHZ("sinewave_50MHZ");

					sinewave_50MHZ.definition(TM::SINE).periods(903).phase(0).samples(
							3072).min(-1.25).max(1.25);

					// Sequencer Programs

					ANALOG_SEQUENCER SEQ_SINE_SEN1_MUX0_50MHZ(
							"SEQ_SINE_SEN1_MUX0_50MHZ");

					SEQ_SINE_SEN1_MUX0_50MHZ.add(TM::NOP, sinewave_50MHZ).add(
							TM::LOOPI, sinewave_50MHZ, 1, 0);

					// analog master clock setting

					SET_SINE_SEN1_MUX0_50MHZ.CLOCK_DOMAIN(2, TM::ANALOG).clock(
							TM::MCLK_AUTO);
					SET_SINE_SEN1_MUX0_50MHZ.AWG(AWG_pin).clockDomain(2).rule(
							TM::AUTOFS).frequency(170000000);

					// hardware settings

					SET_SINE_SEN1_MUX0_50MHZ.AWG(AWG_pin).coreFunction(TM::VHF).sequencerProgram(
							SEQ_SINE_SEN1_MUX0_50MHZ).vOffset(0.0).filter("58M").dutImpedance(
							"1000000");
					SET_SINE_SEN1_MUX0_50MHZ.AWG(AWG_pin).min(0.3).max(0.9);
					SET_SINE_SEN1_MUX0_1KHZ.AWG(AWG_pin).dc(0.0).ac(0.6);

					///////////////////// DGT setting //////////////////////////

					// Definition of the sequencer program

					ANALOG_SEQUENCER SEQ_DGT_SEN1_MUX0_50MHZ(
							"SEQ_DGT_SEN1_MUX0_50MHZ");

					SEQ_DGT_SEN1_MUX0_50MHZ.add(TM::HALT, TM::POST, 30720,
							3072, TM::ONE);

					// clock setting
					SET_SINE_SEN1_MUX0_50MHZ.DGT(DGT_pin).clockDomain(2).rule(
							TM::AUTOFS).frequency(170000000);

					// hardware settings

					SET_SINE_SEN1_MUX0_50MHZ.DGT(DGT_pin).coreFunction(TM::VHF).measurementMode(
							"BYPS").sequencerProgram(SEQ_DGT_SEN1_MUX0_50MHZ).filter(
							"105M").impedance("10K").vRange("2").vOffset(1.0).interDiscard(
							0).trigDelay(0);

					if (k == 0) {

						Analog.primary("SET_SINE_SEN1_MUX0_1KHZ");

					} else if (k == 1) {

						Analog.primary("SET_SINE_SEN1_MUX0_10KHZ");

					} else if (k == 2) {

						Analog.primary("SET_SINE_SEN1_MUX0_95KHZ");

					} else if (k == 3) {

						Analog.primary("SET_SINE_SEN1_MUX0_950KHZ");

					} else if (k == 4) {

						Analog.primary("SET_SINE_SEN1_MUX0_2MHZ");

					} else if (k == 5) {

						Analog.primary("SET_SINE_SEN1_MUX0_9_5MHZ");

					} else if (k == 6) {

						Analog.primary("SET_SINE_SEN1_MUX0_29_5MHZ");

					} else if (k == 7) {

						Analog.primary("SET_SINE_SEN1_MUX0_39_5MHZ");

					} else if (k == 8) {

						Analog.primary("SET_SINE_SEN1_MUX0_50MHZ");

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

			if (k == 0) {
				DSP_THD(sine_av_wf[k], &thd_result[k], num_cycle_DAC_1,
						num_harmonics_DAC, DB, 0);

				cout << "1K Sine Result" << endl;

				cout << average_wave[k] + " : F bin : "
						<< fft_volt_wf[k][num_cycle_DAC_1] << endl;

			} else if (k == 1) {

				DSP_THD(sine_av_wf[k], &thd_result[k], num_cycle_DAC_2,
						num_harmonics_DAC, DB, 0);

				cout << "10K Sine Result" << endl;

				cout << average_wave[k] + " : F bin : "
						<< fft_volt_wf[k][num_cycle_DAC_2] << endl;

			} else if (k == 2) {

				DSP_THD(sine_av_wf[k], &thd_result[k], num_cycle_DAC_3,
						num_harmonics_DAC, DB, 0);

				cout << "95K Sine Result" << endl;

				cout << average_wave[k] + " : F bin : "
						<< fft_volt_wf[k][num_cycle_DAC_3] << endl;

			} else if (k == 3) {

				DSP_THD(sine_av_wf[k], &thd_result[k], num_cycle_DAC_4,
						num_harmonics_DAC, DB, 0);

				cout << "950K Sine Result" << endl;

				cout << average_wave[k] + " : F bin : "
						<< fft_volt_wf[k][num_cycle_DAC_4] << endl;

			} else if (k == 4) {

				DSP_THD(sine_av_wf[k], &thd_result[k], num_cycle_DAC_5,
						num_harmonics_DAC, DB, 0);

				cout << "2M Sine Result" << endl;

				cout << average_wave[k] + " : F bin : "
						<< fft_volt_wf[k][num_cycle_DAC_5] << endl;

			} else if (k == 5) {

				DSP_THD(sine_av_wf[k], &thd_result[k], num_cycle_DAC_6,
						num_harmonics_DAC, DB, 0);

				cout << "9.5M Sine Result" << endl;

				cout << average_wave[k] + " : F bin : "
						<< fft_volt_wf[k][num_cycle_DAC_6] << endl;

			} else if (k == 6) {

				DSP_THD(sine_av_wf[k], &thd_result[k], num_cycle_DAC_7,
						num_harmonics_DAC, DB, 0);

				cout << "29.5M Sine Result" << endl;

				cout << average_wave[k] + " : F bin : "
						<< fft_volt_wf[k][num_cycle_DAC_7] << endl;

			} else if (k == 7) {

				DSP_THD(sine_av_wf[k], &thd_result[k], num_cycle_DAC_8,
						num_harmonics_DAC, DB, 0);

				cout << "39.5M Sine Result" << endl;

				cout << average_wave[k] + " : F bin : "
						<< fft_volt_wf[k][num_cycle_DAC_8] << endl;

			} else if (k == 8) {

				DSP_THD(sine_av_wf[k], &thd_result[k], num_cycle_DAC_9,
						num_harmonics_DAC, DB, 0);

				cout << "50M Sine Result" << endl;

				cout << average_wave[k] + " : F bin : "
						<< fft_volt_wf[k][num_cycle_DAC_9] << endl;

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

			/*
			 TEST("NOTRIG_FLAG", "NOTRIG_FLAG", LIMIT(TM::GE, 1.0, TM::LE, 1.0),
			 NOTRIG_FLAG, TM::CONTINUE);
			 TEST(DGT_pin, "DAC_SNR", LIMIT(TM::GE, 40.0, TM::LE, 60.0),
			 thd_result[k].snr, TM::CONTINUE);
			 TEST(DGT_pin, "DAC_THD", LIMIT(TM::GE, -65.0, TM::LE, -40.0),
			 thd_result[k].thd, TM::CONTINUE);
			 TEST(DGT_pin, "DAC_SNDR", LIMIT(TM::GE, 40.0, TM::LE, 60.0),
			 thd_result[k].snd, TM::CONTINUE);
			 */

		}

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
REGISTER_TESTMETHOD("ALPHA_TEST", ALPHA_TEST)
;
