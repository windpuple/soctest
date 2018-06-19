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

class DIDD: public testmethod::TestMethod {
protected:
  double  mWait_time;
  int   mdebug;
protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
  virtual void initialize()
  {
    //Add your initialization code here
    //Note: Test Method API should not be used in this method!
    addParameter("wait_time",
                 "double",
                 &mWait_time,
                 testmethod::TM_PARAMETER_INPUT)
      .setComment("wait for measurement");
    addParameter("debug",
                 "int",
                 &mdebug,
                 testmethod::TM_PARAMETER_INPUT);
  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
  {
	#define uA *1.0e-6
	#define UA *1.0e-6
	#define mA *1.0e-3
	#define MA *1.0e-3
	FLEX_RELAY ac_relay;

		//Add your test code here.
		DOUBLE iVCCD1, iVCCA1,iVCCD2, iVCCA2;

		DPS_TASK dps_meas1;
		DPS_TASK dps_meas2;

		int site_num;

		ON_FIRST_INVOCATION_BEGIN();

				DISCONNECT();
				CONNECT();

				Primary.label("PGM_MUX_0_SIDD");

				dps_meas1.pin("@"); //This function returns the reference to the DPS_PIN_TASK class object that represents the mea
				dps_meas1.execMode(TM::PVAL); //This member function selects the execution mode.

				dps_meas1.pin("VCCD").min(0.0 A).max(0.01 A);
				dps_meas1.pin("VCCA").min(0.0 A).max(0.1 A);

				dps_meas1.trigMode(TM::INTERNAL);
				dps_meas1.wait(0.05);
				dps_meas1.samples(200);

				ac_relay.pin("@").set("IDLE", "OFF");
				ac_relay.wait(1.5 ms );
				ac_relay.execute();

				ac_relay.pin(
						"SENADDR0,SENADDR1,SENADDR2,SENADDR3,SENADDR4,SENADDR5,SENADDR6,COMMMODE, BLEN, INPUTMODE, MUXEN, OSCON").set(
						"AC", "OFF");
				//ac_relay.pin("SENGRP").set("AC", "OFF");
				ac_relay.wait(1.5 ms );
				ac_relay.execute();

				FLUSH();

				cout << "LABEL : " << Primary.getLabel() << endl;

				Sequencer.stopCycle(97).run(); // flush then vector waiting test done.
				WAIT_TIME(mWait_time ms );

				dps_meas1.execute();

				//Sequencer.abort();

				Sequencer.reset();

				ac_relay.pin("@").set("AC", "OFF");
				ac_relay.wait(1.5 ms );
				ac_relay.execute();

			ON_FIRST_INVOCATION_END();

		iVCCD1 = dps_meas1.getValue("VCCD"); //retrun A
		iVCCA1 = dps_meas1.getValue("VCCA");

		site_num = CURRENT_SITE_NUMBER();
		if (mdebug) {
			cout << "SITE:" << site_num << "@IDS_VCCD:" << iVCCD1
					* 1.0e3  << "mA" << endl;
		}
		if (mdebug) {
			cout << "SITE:" << site_num << "@IDS_VCCA:" << iVCCA1 * 1.0e3
					<< "mA" << endl;
		}


	    ON_FIRST_INVOCATION_BEGIN();
	    CONNECT();

	    Primary.label("PGM_MUX_0_DIDD");

	    dps_meas2.pin("@"); //This function returns the reference to the DPS_PIN_TASK class object that represents the mea
	    dps_meas2.execMode(TM::PVAL); //This member function selects the execution mode.


	    dps_meas2.pin("VCCD").min(0.0 A).max(0.01 A);
	    dps_meas2.pin("VCCA").min(0.0 A).max(0.1 A);

	    dps_meas2.trigMode(TM::INTERNAL);
	    dps_meas2.samples(100);

		ac_relay.pin("@").set("IDLE", "OFF");
		ac_relay.wait(1.5 ms );
		ac_relay.execute();

		ac_relay.pin(
				"SENADDR0,SENADDR1,SENADDR2,SENADDR3,SENADDR4,SENADDR5,SENADDR6,COMMMODE, BLEN, INPUTMODE, MUXEN, OSCON").set(
				"AC", "OFF");
		ac_relay.pin("SENGRP").set("AC", "OFF");
		ac_relay.wait(1.5 ms );
		ac_relay.execute();

		FLUSH();

		cout << "LABEL : " << Primary.getLabel() << endl;

	    START_TEST(); // flush then vector end
	    //FUNCTIONAL_TEST(); // flush then vector waiting test done.
	    //EXECUTE_TEST();

		//Sequencer.reset();
		//Sequencer.run(TM::NORMAL);

		WAIT_TIME(mWait_time ms);

	    dps_meas2.execute();

	    Sequencer.abort();

		ac_relay.pin("@").set("AC", "OFF");
		ac_relay.wait(1.5 ms );
		ac_relay.execute();


	    ON_FIRST_INVOCATION_END();

	    iVCCD2  = dps_meas2.getValue("VCCD"); //retrun A
	    iVCCA2  = dps_meas2.getValue("VCCA");

	    site_num = CURRENT_SITE_NUMBER();
	    if(mdebug){cout<<"SITE:" << site_num << "@IDD_VCCD:"  <<iVCCD2*1.0e3<<"mA"<<endl;}
	    //if(mdebug){cout<<"SITE:" << site_num << "@IDD_VCCA:"  <<(iVCCA2-iVCCA1)*1.0e3<<"mA"<<endl;}

	    TEST("IDD_VCCD",   "IDD_VCCD", LIMIT(TM::GT,   0.0 MA, TM::LT,   0.015 MA),   iVCCD2,   TM::CONTINUE);
	    //TEST("IDD_VCCA",   "IDD_VCCA", LIMIT(TM::GT,  0.0 MA, TM::LT,  20.0 MA),     (iVCCA2-iVCCA1),   TM::CONTINUE);

	    return;
  }

  /**
   *This function will be invoked once the specified parameter's value is changed.
   *@param parameterIdentifier
   */
  virtual void postParameterChange(const string& parameterIdentifier)
  {
    //Add your code
    //Note: Test Method API should not be used in this method!
    return;
  }

  /**
   *This function will be invoked once the Select Test Method Dialog is opened.
   */
  virtual const string getComment() const 
  {
    string comment = " please add your comment for this method.";
    return comment;
  }
};
REGISTER_TESTMETHOD("DIDD", DIDD);
