#include "testmethod.hpp"
#include "mapi.hpp"
using namespace std;

#define MAX_SITES 4

class FREQ_CAP: public testmethod::TestMethod {
protected:
  int     mSamples;
  double  mWaittime;
  int     mDebug_Print;
  int     mDebug_Freq_Print;
  double  mCaptureTime_ns;
  string  capture_variable;
  double  M_FREQ[MAX_SITES];
  ARRAY_I CAPTURE_ARRAY[MAX_SITES];
  ARRAY_I EACH_WAVE_ARRAY[MAX_SITES];

  double  MCKO_FREQ[MAX_SITES],DCKO_FREQ[MAX_SITES];
  double  SDIP_FREQ[MAX_SITES],SDIM_FREQ[MAX_SITES];
protected:

  virtual void initialize()
  {
	    addParameter("Waittime",
					 "double",
					 &mWaittime,
					 testmethod::TM_PARAMETER_INPUT)
		.setDefault("1.0");
	    addParameter("CaptureTime_ns",
					 "double",
					 &mCaptureTime_ns,
					 testmethod::TM_PARAMETER_INPUT);
	    addParameter("capture_variable",
	                 "PinString",
	                 &capture_variable,
	                 testmethod::TM_PARAMETER_INPUT);
		addParameter("Debug_Print",
					 "int",
					 &mDebug_Print,
					 testmethod::TM_PARAMETER_INPUT)
		.setDefault("1");
		addParameter("Debug_Freq_Print",
					 "int",
					 &mDebug_Freq_Print,
					 testmethod::TM_PARAMETER_INPUT)
		.setDefault("1");
  }
  void freq_capture(string PinName, int mask_value)
  {
	int i, mask;
	ARRAY_D spect ;
	int min_index , max_index ;
	double min , max , dR , interpol_index , frequency_MHz ;

	FOR_EACH_SITE_BEGIN();
	    CAPTURE_ARRAY[CURRENT_SITE_NUMBER()-1].init();
	    CAPTURE_ARRAY[CURRENT_SITE_NUMBER()-1] = 0;
	    CAPTURE_ARRAY[CURRENT_SITE_NUMBER()-1]=VECTOR(capture_variable).getVectors();
		if(mDebug_Freq_Print==1){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<< " CAPTURE_ARRAY="<< CAPTURE_ARRAY[CURRENT_SITE_NUMBER()-1] << endl;}
	FOR_EACH_SITE_END();

	//MSB  LSB
	//MCKO DCKO
	FOR_EACH_SITE_BEGIN();
	    mask=mask_value;
	    EACH_WAVE_ARRAY[CURRENT_SITE_NUMBER()-1].resize(CAPTURE_ARRAY[CURRENT_SITE_NUMBER()-1].size());
		for(i=0; i<CAPTURE_ARRAY[CURRENT_SITE_NUMBER()-1].size();i++)
		{
			if(mask & CAPTURE_ARRAY[CURRENT_SITE_NUMBER()-1][i]){
				EACH_WAVE_ARRAY[CURRENT_SITE_NUMBER()-1][i] = 1;
			}
			else{
				EACH_WAVE_ARRAY[CURRENT_SITE_NUMBER()-1][i] = 0;
			}
		}

		if(mDebug_Freq_Print==1){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<< "EACH_WAVE_ARRAY="<< EACH_WAVE_ARRAY[CURRENT_SITE_NUMBER()-1] << endl;}
	    PUT_DEBUG(PinName, "EACH_WAVE_ARRAY", EACH_WAVE_ARRAY[CURRENT_SITE_NUMBER()-1]);

		// Tabei-Ueda algorithm needs power spectrum (no log!) with Hanning filter applied
		DSP_SPECTRUM(EACH_WAVE_ARRAY[CURRENT_SITE_NUMBER()-1] ,spect , VOLT , 1.0 , HANNING , 0 ) ;
	    PUT_DEBUG(PinName, "M_SPEC", spect);

	    if(mDebug_Print==1){cout<<"mCaptureTime_ns= " <<mCaptureTime_ns<<endl;
	    }
        //cancel out the potential max bin at DC
		  if (spect.size() > 2){ spect[0] = 0; spect[1] = 0;}
		  else
		  { cerr<<"WARN: spectrum data is too small."<<endl; }

		  DSP_MINMAX  ( spect , &min , &max , &min_index , &max_index ) ;  // look for max bin

		  // interpolating equations
		  int max_index_plus_one = max_index + 1;
		  if(mDebug_Freq_Print==1){cout<<"SITE:" << CURRENT_SITE_NUMBER() <<"max_index:"  <<max_index<<endl;}

		  max_index_plus_one = (max_index_plus_one < spect.size()) ? max_index_plus_one : max_index_plus_one-1;

		  if ( ( max_index > 0 ) && ( spect[ max_index - 1 ] > spect[ max_index_plus_one ] ) )
		  {
		    dR = spect[ max_index - 1 ] / spect[ max_index ] ;
		    if(mDebug_Freq_Print==1){cout<<"SITE:" << CURRENT_SITE_NUMBER() <<"if_dR"  <<dR<<endl;}

		    interpol_index = max_index + ( 1.0 - 2.0 * dR ) / ( 1.0 + dR ) ;
		    if(mDebug_Freq_Print==1){cout<<"SITE:" << CURRENT_SITE_NUMBER() <<"if_interpol_index"  <<interpol_index<<endl;}
		  }
		  else
		  {
		    dR = spect[ max_index_plus_one ] / spect[ max_index ] ;
		    if(mDebug_Freq_Print==1){cout<<"SITE:" << CURRENT_SITE_NUMBER() <<"else_dR"  <<dR<<endl;}

		    interpol_index = max_index - ( 1.0 - 2.0 * dR ) / ( 1.0 + dR ) ;
		    if(mDebug_Freq_Print==1){cout<<"SITE:" << CURRENT_SITE_NUMBER() <<"else_interpol_index"  <<interpol_index<<endl;}
		  }

		  frequency_MHz = interpol_index / ((mCaptureTime_ns/1000) * EACH_WAVE_ARRAY[CURRENT_SITE_NUMBER()-1].size()); //sampling 4.125ns
		  M_FREQ[CURRENT_SITE_NUMBER()-1] = frequency_MHz;
		  if(mDebug_Freq_Print==1){cout<<"SITE:" << CURRENT_SITE_NUMBER() <<"frequency_MHz:"  <<frequency_MHz<<endl;}

	FOR_EACH_SITE_END();
  }
  virtual void run()
  {
        int i;
	    STRING enabled_sites;
      	FLEX_RELAY ac_relay;
		ARRAY_I MCKO_ARRAY[MAX_SITES],DCKO_ARRAY[MAX_SITES];
		ARRAY_I SDIP_ARRAY[MAX_SITES],SDIM_ARRAY[MAX_SITES];

		ON_FIRST_INVOCATION_BEGIN();
			DISCONNECT();
			CONNECT();

			ac_relay.pin("@").set("IDLE","OFF");
			ac_relay.wait(1.5 ms);
			ac_relay.execute();

			ac_relay.pin("JTDO,RSTX,XI0,XI1,TP0,TP1,TP2,HSI,JMODE,JTRST,JTCK,JTMS,JTDI,RX0,MCKO,DCKO,SDIP,SDIM").set("AC","OFF");
			ac_relay.wait(1.5 ms);
			ac_relay.execute();

			ac_relay.pin("RSET").set("AC","OFF");
			ac_relay.wait(1.5 ms);
			ac_relay.execute();

			string vector_lable = Primary.getLabel();
			if(mDebug_Print){cout<<"primary vector_lable = "<<vector_lable<<endl;}

			Primary.label(vector_lable);
			FLUSH(TM::APRM);
			DIGITAL_CAPTURE_TEST();
			WAIT_TEST_DONE(2.0);

			freq_capture("DCKO", 0x01); //DCKO
			FOR_EACH_SITE_BEGIN();
			   DCKO_ARRAY[CURRENT_SITE_NUMBER()-1].resize(CAPTURE_ARRAY[CURRENT_SITE_NUMBER()-1].size());
			   DCKO_ARRAY[CURRENT_SITE_NUMBER()-1] = EACH_WAVE_ARRAY[CURRENT_SITE_NUMBER()-1];
			   PUT_DEBUG("DCKO", "DCKO_CAP", DCKO_ARRAY[CURRENT_SITE_NUMBER()-1]);

			   DCKO_FREQ[CURRENT_SITE_NUMBER()-1] = M_FREQ[CURRENT_SITE_NUMBER()-1];
			   if(mDebug_Print==1){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<< "DCKO_FREQ="<< DCKO_FREQ[CURRENT_SITE_NUMBER()-1] << endl;}
			FOR_EACH_SITE_END();

			freq_capture("MCKO", 0x02); //MCKO
			FOR_EACH_SITE_BEGIN();
			   MCKO_ARRAY[CURRENT_SITE_NUMBER()-1].resize(CAPTURE_ARRAY[CURRENT_SITE_NUMBER()-1].size());
			   MCKO_ARRAY[CURRENT_SITE_NUMBER()-1] = EACH_WAVE_ARRAY[CURRENT_SITE_NUMBER()-1];
			   PUT_DEBUG("MCKO", "MCKO_CAP", MCKO_ARRAY[CURRENT_SITE_NUMBER()-1]);

			   MCKO_FREQ[CURRENT_SITE_NUMBER()-1] = M_FREQ[CURRENT_SITE_NUMBER()-1];
			   if(mDebug_Print==1){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<< "MCKO_FREQ="<< MCKO_FREQ[CURRENT_SITE_NUMBER()-1] << endl;}
			FOR_EACH_SITE_END();

			freq_capture("SDIM", 0x04); //SDIM
			FOR_EACH_SITE_BEGIN();
				SDIM_ARRAY[CURRENT_SITE_NUMBER()-1].resize(CAPTURE_ARRAY[CURRENT_SITE_NUMBER()-1].size());
				SDIM_ARRAY[CURRENT_SITE_NUMBER()-1] = EACH_WAVE_ARRAY[CURRENT_SITE_NUMBER()-1];
			   PUT_DEBUG("SDIM", "SDIM_CAP", SDIM_ARRAY[CURRENT_SITE_NUMBER()-1]);

			   SDIM_FREQ[CURRENT_SITE_NUMBER()-1] = M_FREQ[CURRENT_SITE_NUMBER()-1];
			   if(mDebug_Print==1){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<< "SDIM_FREQ="<< SDIM_FREQ[CURRENT_SITE_NUMBER()-1] << endl;}
			FOR_EACH_SITE_END();

			freq_capture("SDIP", 0x08); //SDIP
			FOR_EACH_SITE_BEGIN();
				SDIP_ARRAY[CURRENT_SITE_NUMBER()-1].resize(CAPTURE_ARRAY[CURRENT_SITE_NUMBER()-1].size());
				SDIP_ARRAY[CURRENT_SITE_NUMBER()-1] = EACH_WAVE_ARRAY[CURRENT_SITE_NUMBER()-1];
			   PUT_DEBUG("SDIP", "SDIP_CAP", SDIP_ARRAY[CURRENT_SITE_NUMBER()-1]);

			   SDIP_FREQ[CURRENT_SITE_NUMBER()-1] = M_FREQ[CURRENT_SITE_NUMBER()-1];
			   if(mDebug_Print==1){cout<<"SITE:"<<CURRENT_SITE_NUMBER()<< "SDIP_FREQ="<< SDIP_FREQ[CURRENT_SITE_NUMBER()-1] << endl;}
			FOR_EACH_SITE_END();

		ON_FIRST_INVOCATION_END();

		TEST("MCKO_FREQ", "MCKO_FREQ", LIMIT(TM::GE, 70.2,   TM::LE, 78.2), MCKO_FREQ[CURRENT_SITE_NUMBER()-1],TM::CONTINUE); //Target 74.2MHz
		TEST("DCKO_FREQ", "DCKO_FREQ", LIMIT(TM::GE, 70.2,   TM::LE, 78.2), DCKO_FREQ[CURRENT_SITE_NUMBER()-1],TM::CONTINUE); //Target 74.2MHz
        TEST("SDIM_FREQ", "SDIM_FREQ", LIMIT(TM::GE, 144.5,  TM::LE, 155.5),SDIM_FREQ[CURRENT_SITE_NUMBER()-1],  TM::CONTINUE); //148.5MHz
        TEST("SDIP_FREQ", "SDIP_FREQ", LIMIT(TM::GE, 144.5,  TM::LE, 155.5),SDIP_FREQ[CURRENT_SITE_NUMBER()-1],  TM::CONTINUE); //148.5MHz

		return;
  }


  virtual void postParameterChange(const string& parameterIdentifier)
  {
    return;
  }


  virtual const string getComment() const 
  {
    string comment = " please add your comment for this method.";
    return comment;
  }
};
REGISTER_TESTMETHOD("FREQ_CAP", FREQ_CAP);
