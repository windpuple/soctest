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

class ADC_Linearity: public testmethod::TestMethod {
public:

protected:
	string AWG_pin;
	string capture_variable;
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
				"the adc input pin connected to the AWG");
		addParameter("capture_variable", "PinString", &capture_variable,
				testmethod::TM_PARAMETER_INPUT) .setDefault("adc_9bit_capture") .setComment(
				"the capture variable used for linearity test");
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

		ARRAY_I ramp_unav_wf;
		ARRAY_I histogram_wf;

		LINEARtype adc_lin;
		ARRAY_D adc_dnla, adc_inla;
		int miss_number, miss_codes;

		STRING sites;

#define ADC_resolution 12
#define ADC12bit_out_code 4096

		//INT miss_codes[(INT) pow(2,(DOUBLE)ADC_resolution)];

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

		ramp_unav_wf = VECTOR(capture_variable).getVectors();

/*
		 if (debug) {
		 for (int i = 0; i < 4096; i++) {
		 cout << "i=" << i << "ramp_unav_wf=" << ramp_unav_wf[i] << endl;
		 }
		 }
*/


		PUT_DEBUG(AWG_pin, "RAMP_CAPTURE", ramp_unav_wf);

		DSP_HISTOGRAM(ramp_unav_wf, histogram_wf, 1, ADC12bit_out_code - 2,
				ADC12bit_out_code);
		/*
		 if (debug) {
		 for (int i = 0; i < histogram_wf.size(); i++) {
		 if (0 == histogram_wf[i]) {
		 cout << "missing code=" << i << endl;
		 }

		 cout << "code=" << i << "  code value=" << histogram_wf[i]
		 << endl;
		 }
		 }
		 */

		DSP_ADC_HISTOGRAM(histogram_wf, &adc_lin, adc_dnla, adc_inla,
				&miss_number, &miss_codes, MIN_RMS, 0);

		PUT_DEBUG(AWG_pin, "HISTOGRAM", histogram_wf);
		PUT_DEBUG(AWG_pin, "DNL", adc_dnla);
		PUT_DEBUG(AWG_pin, "INL", adc_inla);

		//if(debug){cout << "Site " << CURRENT_SITE_NUMBER() << " miss_number : " << miss_number << endl;}
		//if(debug){cout << "Site " << CURRENT_SITE_NUMBER() << " miss_codes : " << miss_codes << endl;}
		if (debug) {
			cout << "Site " << CURRENT_SITE_NUMBER() << " ADC DNL : "
					<< adc_lin.damax << endl;
		}
		if (debug) {
			cout << "Site " << CURRENT_SITE_NUMBER() << " ADC INL : "
					<< adc_lin.iamax << endl;
		}

		TEST("AWG_pin", "ADC_DLE", LIMIT(TM::GE, -6.0, TM::LE, 6.0),
				adc_lin.damax, TM::CONTINUE);
		TEST("AWG_pin", "ADC_ILE", LIMIT(TM::GE, -6.0, TM::LE, 6.0),
				adc_lin.iamax, TM::CONTINUE);

		return;
	}

	virtual void postParameterChange(const string& parameterIdentifier) {
		return;
	}

	virtual const string getComment() const {
		string comment = "ADC RAMP Linearity Test.";
		return comment;
	}
};
REGISTER_TESTMETHOD("ADC_Linearity", ADC_Linearity)
;
