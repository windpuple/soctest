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
class ADC_Distortion: public testmethod::TestMethod {
public:

protected:
	string AWG_pin;
	string capture_variable;
	int num_cycle_ADC;
	int num_harmonics_ADC;
	int debug;
	int test_site;
	double waittime;
	/**
	 *Initialize the parameter interface to the testflow.
	 *This method is called just once after a testsuite is created.
	 */
protected:
	virtual void initialize() {
		//Add your initialization code here
		//Note: Test Method API should not be used in this method!
		addParameter("AWG_pin", "PinString", &AWG_pin,
				testmethod::TM_PARAMETER_INPUT) .setDefault("ADC_SRC") .setComment(
				"the ADC input pin connected to the AWG");
		addParameter("capture_variable", "PinString", &capture_variable,
				testmethod::TM_PARAMETER_INPUT) .setDefault(
				"adc_9bit_capture_sine") .setComment(
				"the capture variable used for linearity test");
		addParameter("num_cycle_ADC", "int", &num_cycle_ADC,
				testmethod::TM_PARAMETER_INPUT) .setDefault("3") .setComment(
				"the cycle number of signal used to test ADC");
		addParameter("num_harmonics_ADC", "int", &num_harmonics_ADC,
				testmethod::TM_PARAMETER_INPUT) .setDefault("10") .setComment(
				"the number of calculated harmonics");

		addParameter("debug", "int", &debug, testmethod::TM_PARAMETER_INPUT) .setDefault(
				"1") .setComment("control if the test is in debug mode");
		addParameter("waittime", "double", &waittime,
				testmethod::TM_PARAMETER_INPUT) .setDefault("0.0") .setComment(
				"relay control waittime and the unit is second");
	}

	/**
	 *This test is invoked per site.
	 */
	virtual void run() {
		FLEX_RELAY relay;
		FLEX_RELAY ac_relay;

		// add your code
		THDtype thd_result;
		int i, j;
		double tout;
		int num_sine_unav_wf, num_sine_av_wf;
		ARRAY_I sine_unav_wf;
		ARRAY_D sine_av_wf, fft_wf;
		int curr_site;
		int num_samples = 4096;
		int num_av = 10;
#define ADC_resolution 12
#define ADC9bit_out_code 4096

		ON_FIRST_INVOCATION_BEGIN();
				DISCONNECT();
				CONNECT();

				relay.util("ADCI0_K1").on();
				relay.wait(waittime);
				relay.execute();

				ac_relay.pin("@").set("IDLE", "OFF");
				ac_relay.wait(1.5 ms );
				ac_relay.execute();

				ac_relay.pin(
						"ADCTRI,RSTX,XI0,TP0,TP1,TP2,VSI,HSI,UCCDI,RX0,TX0,GPIO0,GPIO1,GPIO2,GPIO3,GPIO4,GPIO5,GPIO6,GPIO7,GPIO8,GPIO9,GPIO10,GPIO11,GPIO12").set(
						"AC", "OFF");
				ac_relay.wait(1.5 ms );
				ac_relay.execute();

				/**Step1: Setup API: to activate analog module trigger line **/
				// for 6.5,4 using routing API instead of routing set up GUI
				Routing.pin(AWG_pin).connect(TM::SINGLE);
				AWG(AWG_pin).enable();

				DIGITAL_CAPTURE_TEST(1.0, TM::WAIT_DIGITAL);
				ABORT_TEST();

				/**Step3: Setup API: to deactivate analog module trigger line**/
				AWG(AWG_pin).disable();
				//Disconnet
				Routing.pin(AWG_pin).disconnect();
				FLUSH();

				relay.util("ADCI0_K1").off();
				relay.wait(waittime);
				relay.execute();
				relay.util("ADCI1_K2").off();
				relay.wait(waittime);
				relay.execute();
				relay.util("ADCI2_K3").off();
				relay.wait(waittime);
				relay.execute();

			ON_FIRST_INVOCATION_END();

		sine_unav_wf = VECTOR(capture_variable).getVectors();

		if (debug) {
			printf("adc_sine_size = %d\n", sine_unav_wf.size());
		}

		sine_av_wf.resize(num_samples);
		for (i = 0; i < num_samples; i++) {
			sine_av_wf[i] = 0.0;
		}

		for (i = 0; i < num_samples; i++) {
			for (j = 0; j < num_av; j++) {
				sine_av_wf[i] = sine_av_wf[i] + sine_unav_wf[i + j
						* num_samples];
			}
		}

		curr_site = CURRENT_SITE_NUMBER();
		for (i = 0; i < num_samples; i++) {
			sine_av_wf[i] = sine_av_wf[i] / (float) num_av;
		}

		PUT_DEBUG(AWG_pin, "SINE_UNAV", sine_unav_wf);
		PUT_DEBUG(AWG_pin, "SINE_AV", sine_av_wf);

		fft_wf.resize(num_samples / 2);

		DSP_SPECTRUM(sine_av_wf, fft_wf, DB, 0.0, RECT, 0);

		PUT_DEBUG(AWG_pin, "FFT_WF", fft_wf);

		DSP_THD(sine_av_wf, &thd_result, num_cycle_ADC, num_harmonics_ADC, DB,
				0);

		curr_site = CURRENT_SITE_NUMBER();

		if (debug) {
			cout << "Site " << curr_site << " ADC SNR : " << thd_result.snr
					<< "dB" << endl;
		}
		if (debug) {
			cout << "Site " << curr_site << " ADC THD : " << thd_result.thd
					<< "dB" << endl;
		}
		if (debug) {
			cout << "Site " << curr_site << " ADC SNDR : " << thd_result.snd
					<< "dB" << endl;
		}

		TEST("AWG_pin", "ADC_SNR", LIMIT(TM::GE, 45, TM::LE, 98),
				thd_result.snr, TM::CONTINUE);
		TEST("AWG_pin", "ADC_SNR", LIMIT(TM::GE, -98, TM::LE, -45),
				thd_result.thd, TM::CONTINUE);
		TEST("AWG_pin", "ADC_SNR", LIMIT(TM::GE, 45, TM::LE, 98),
				thd_result.snd, TM::CONTINUE);

		return;
	}

	virtual void postParameterChange(const string& parameterIdentifier) {
		return;
	}

	virtual const string getComment() const {
		string comment = "ADC RAMP Distortion Test.";
		return comment;
	}
};
REGISTER_TESTMETHOD("ADC_Distortion", ADC_Distortion)
;
