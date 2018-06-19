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

class SINE_SET_GENERATE: public testmethod::TestMethod {

protected:

	/**
	 *Initialize the parameter interface to the testflow.
	 *This method is called just once after a testsuite is created.
	 */
protected:
	virtual void initialize() {

	}

	virtual void run() {


		ON_FIRST_INVOCATION_BEGIN();

				// Definition of the Analog SET

				ANALOG_SET SINE_SEN1_MUX0_1KHZ ("SINE_SEN1_MUX0_1KHZ");

				// Definition of the waveform

				ANALOG_WAVEFORM sinewave_1KHZ ("sinewave_1KHZ");

				sinewave_1KHZ.definition(TM::SINE).periods(3).phase(0).samples(3072).min(-0.5).max(0.5);

				// Sequencer Programs

				ANALOG_SEQUENCER SEQ_SINE_SEN1_MUX0_1KHZ ("SEQ_SINE_SEN1_MUX0_1KHZ");

				SEQ_SINE_SEN1_MUX0_1KHZ.add(TM::NOP,sinewave_1KHZ).add(TM::LOOPI,sinewave_1KHZ,1,0);

				// analog master clock setting

				SINE_SEN1_MUX0_1KHZ.CLOCK_DOMAIN(2,TM::ANALOG).clock(TM::MCLK_AUTO);

				// clock settings

				SINE_SEN1_MUX0_1KHZ.AWG("ADC_AWG").clockDomain(2).rule(TM::AUTOFS).frequency(1e6);

				// hardware settings

				SINE_SEN1_MUX0_1KHZ.AWG("ADC_AWG").sequencerProgram(SEQ_SINE_SEN1_MUX0_1KHZ).coreFunction(TM::LF).filter("1.5K").dutImpedance("1000000");

			ON_FIRST_INVOCATION_END();

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
REGISTER_TESTMETHOD("SINE_SET_GENERATE", SINE_SET_GENERATE)
;
