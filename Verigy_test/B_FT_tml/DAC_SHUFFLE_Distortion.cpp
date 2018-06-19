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

class DAC_SHUFFLE_Distortion: public testmethod::TestMethod {

protected:
  string  DGT_pin;
  //string  SMC_Distortion_handleName;
  int num_cycle_DAC;
  int num_harmonics_DAC;
  int  debug;
  int test_site;
  double waittime;
  int TRIG_FLAG[4];
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
protected:
  virtual void initialize()
  {
    //Add your initialization code here
    //Note: Test Method API should not be used in this method!
    addParameter("DGT_pin",
                 "PinString",
                 &DGT_pin,
                 testmethod::TM_PARAMETER_INPUT)
      .setDefault("DAC_MEAS")
      .setComment("the dac output pin connected to the DGT");
    addParameter("num_cycle_DAC",
                 "int",
                 &num_cycle_DAC,
                 testmethod::TM_PARAMETER_INPUT)
      .setComment("the cycle number of signal used to test DAC");
    addParameter("num_harmonics_DAC",
                 "int",
                 &num_harmonics_DAC,
                 testmethod::TM_PARAMETER_INPUT)
      .setComment("dhe number of calculated harmonics");
    addParameter("debug",
                 "int",
                 &debug,
                 testmethod::TM_PARAMETER_INPUT)
      .setDefault("1")
      .setComment("control if the test is in debug mode");
    addParameter("waittime",
                 "double",
                 &waittime,
                 testmethod::TM_PARAMETER_INPUT)
      .setDefault("0.0")
      .setComment("relay control waittime and the unit is second");
  }

  virtual void run()
  {
	FLEX_RELAY relay;

    // add your code
    double thd, sndr;
    double signal,noise;
    int i,j;
    double tout;
    int sine_av_samples = 1024;
    int sine_av = 10;
    THDtype thd_result;

    ARRAY_D sine_unav_wf;
    ARRAY_D sine_av_wf;
    ARRAY_D fft_wf;
    ARRAY_D fft_volt_wf;
    tout = 5.0;

    FLEX_RELAY ac_relay;
    int curr_site;
    static INT tout_site_flag, mis_trg_site;

    ON_FIRST_INVOCATION_BEGIN();
		DISCONNECT();
		CONNECT();
		relay.util("K5_CVBS,K7_COMP_CAP,K6_REXTD_REGISTER").on();relay.wait(1.5 ms);relay.execute();

		ac_relay.pin("@").set("IDLE","OFF");
		ac_relay.wait(1.5 ms);
		ac_relay.execute();

		//ac_relay.pin("DACTRIG,RSTX,TP0,TP1,TP2,JMODE,JTRST,JTCK,JTMS,JTDI,GPIO20,GPIO31,GPIO21,GPIO22,GPIO23,GPIO24,GPIO25,GPIO26,GPIO27,GPIO28,GPIO29,GPIO30").set("AC","OFF");
		ac_relay.pin("DACTRIG,RSTX,TP0,TP1,TP2,GPIO20,GPIO31,GPIO21,GPIO22,GPIO23,GPIO24,GPIO25,GPIO26,GPIO27,GPIO28,GPIO29,GPIO30").set("AC","OFF");
		ac_relay.wait(1.5 ms);
		ac_relay.execute();

		Routing.pin(DGT_pin).connect(TM::SINGLE);
		DGT(DGT_pin).enable();
		FLUSH();

	//	FUNCTIONAL_TEST(); //Need to Pre-Running
		START_TEST();
		WAIT_TEST_DONE(tout,&tout_site_flag,&mis_trg_site);

		relay.util("K5_CVBS,K7_COMP_CAP,K6_REXTD_REGISTER").off();relay.wait(1.5 ms);relay.execute();

    ON_FIRST_INVOCATION_END();

    sine_unav_wf = DGT(DGT_pin).getWaveform();

    Routing.pin(DGT_pin).disconnect();
    DGT(DGT_pin).disable();
    FLUSH();

    sine_av_wf.resize(sine_av_samples);

    for(i = 0; i < sine_av_samples; i++)
    {
        sine_av_wf[i] = 0.0;
    }

    for(i = 0; i < sine_av_samples; i++)
    {
        for(j = 0; j < sine_av; j++)
        {
            sine_av_wf[i] = sine_av_wf[i] + sine_unav_wf[i + j*sine_av_samples];
        }
    }

    for(i = 0; i < sine_av_samples; i++)
    {
        sine_av_wf[i] = sine_av_wf[i]/sine_av;
    }

    fft_wf.resize(sine_av_samples/2);

    //ADD SHUFFLE
    ARRAY_D sine_av_wf_shuffle;
    DSP_SHUFFLE(sine_av_wf, sine_av_wf_shuffle,5);

    //DSP_SPECTRUM(sine_av_wf, fft_wf, DB, 0.0, RECT, 0);
    //DSP_SPECTRUM(sine_av_wf, fft_volt_wf, VOLT, 1.414, RECT, 0);
    DSP_SPECTRUM(sine_av_wf_shuffle, fft_wf, DB, 0.0, RECT, 0);
    DSP_SPECTRUM(sine_av_wf_shuffle, fft_volt_wf, VOLT, 1.414, RECT, 0);

    PUT_DEBUG(DGT_pin, "Sine unav", sine_unav_wf);
    PUT_DEBUG(DGT_pin, "Sine av", sine_av_wf);
    PUT_DEBUG(DGT_pin, "sine_av_wf_shuffle", sine_av_wf_shuffle);
    PUT_DEBUG(DGT_pin, "FFT wf", fft_wf);
    PUT_DEBUG(DGT_pin, "FFT_VOLT wf", fft_volt_wf);

#if 0
    double snr, signal_power, noise_power;
    DSP_SNR(sine_av_wf, &snr, &signal_power, &noise_power, 1024, num_cycle_DAC, num_harmonics_DAC, 1.414, NULL);
    cout << "Site " << CURRENT_SITE_NUMBER() << " snr : " << snr <<  "dB" << endl;
    cout << "Site " << CURRENT_SITE_NUMBER() << " signal_power : " << signal_power <<  "dB" << endl;
    cout << "Site " << CURRENT_SITE_NUMBER() << " noise_power : " << noise_power <<  "dB" << endl;
    cout << "Site " << CURRENT_SITE_NUMBER() << " signal_power-noise_power : " << signal_power-noise_power << endl;

    double snd, snd_signal_power, other_power;
    DSP_SND(sine_av_wf, &snd, &snd_signal_power, &other_power, 1024, num_cycle_DAC, 1.414, NULL);
    cout << "Site " << CURRENT_SITE_NUMBER() << " snd_signal_power : " << snd_signal_power <<  "dB" << endl;
    cout << "Site " << CURRENT_SITE_NUMBER() << " other_power : " << other_power <<  "dB" << endl;
    cout << "Site " << CURRENT_SITE_NUMBER() << " snd_signal_power-other_power : " << snd_signal_power-other_power << endl;
#endif

    DSP_THD(sine_av_wf_shuffle, &thd_result, num_cycle_DAC, num_harmonics_DAC, DB, 0);

    curr_site = CURRENT_SITE_NUMBER();
	if((mis_trg_site>>(curr_site -1))&1){
		cout <<"Trigger error on site"<< curr_site << endl;
		TRIG_FLAG[CURRENT_SITE_NUMBER()-1] = 1;
	}
	else{
		cout <<"Triggered good on site"<< curr_site << endl;
		TRIG_FLAG[CURRENT_SITE_NUMBER()-1] = 0;
	}

    if(debug){cout << "Site " << CURRENT_SITE_NUMBER() << " DAC SNR : " << thd_result.snr <<  "dB" << endl;}
    if(debug){cout << "Site " << CURRENT_SITE_NUMBER() << " DAC THD : " << thd_result.thd <<  "dB" << endl;}
    if(debug){cout << "Site " << CURRENT_SITE_NUMBER() << " DAC SNDR : " << thd_result.snd <<  "dB" << endl;}

    TEST("TRIGGER", "TRIGGER", LIMIT(TM::GE, 0.0, TM::LE, 0.0), TRIG_FLAG[CURRENT_SITE_NUMBER()-1],TM::CONTINUE);
    TEST(DGT_pin, "DAC_SNR", LIMIT(TM::GE, 40.0, TM::LE, 60.0), thd_result.snr,TM::CONTINUE);
    TEST(DGT_pin, "DAC_THD", LIMIT(TM::GE, -65.0, TM::LE, -40.0), thd_result.thd,TM::CONTINUE);
    TEST(DGT_pin, "DAC_SNDR", LIMIT(TM::GE, 40.0, TM::LE, 60.0), thd_result.snd,TM::CONTINUE);

    DISCONNECT();

    return;
  }

  virtual void postParameterChange(const string& parameterIdentifier)
  {
    return;
  }

  virtual const string getComment() const 
  {
    string comment = "DAC RAMP Distortion Test.";
    return comment;
  }
};
REGISTER_TESTMETHOD("DAC_SHUFFLE_Distortion", DAC_SHUFFLE_Distortion);
