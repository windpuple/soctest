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
    DOUBLE iVDDC,iAVDDSDT,iVDDQDDR,iAVDDDAC,iAVDDLV33,iVDDQBT,iVDDQICIO,iVDDQ;

	    DPS_TASK dps_meas;
	    int site_num;

	    ON_FIRST_INVOCATION_BEGIN();
	    CONNECT();

	    dps_meas.pin("@"); //This function returns the reference to the DPS_PIN_TASK class object that represents the mea
	    dps_meas.execMode(TM::PVAL); //This member function selects the execution mode.


	    dps_meas.pin("VDDQDDR").min(0.0 A).max(1.0 A);
	    dps_meas.pin("VDDC").min(0.0 A).max(1.0 A);
	    dps_meas.pin("VDDQBT").min(0.0 A).max(1.0 A);
	    dps_meas.pin("VDDQ").min(0.0 A).max(1.0 A);
	    dps_meas.pin("AVDDLV33").min(0.0 A).max(1.0 A);
	    dps_meas.pin("AVDDSDT").min(0.0 A).max(1.0 A);
	    dps_meas.pin("AVDDDAC").min(0.0 A).max(0.01 A);
	    dps_meas.pin("VDDQICIO").min(0.0 A).max(1.0 A);

	    dps_meas.trigMode(TM::INTERNAL);
	    dps_meas.samples(1000);
        //ac_relay.pin("@").set("AC","OFF"); ac_relay.execute();
        //ac_relay.pin("CO0,CO1,CO2,CO3,CO4,CO5,CO6,CO7,CO8,CO9,YO0,YO1,YO2,YO3,YO4,YO5,YO6,YO7,YO8,YO9").set("IDLE","OFF");ac_relay.execute();

	    START_TEST(); // flush then vector end
	    //EXECUTE_TEST(); // flush then vector waiting test done.
	    WAIT_TIME(mWait_time ms);

	    dps_meas.execute();

	    Sequencer.abort();
	    FLUSH();

	    ON_FIRST_INVOCATION_END();

	    iVDDQDDR  = dps_meas.getValue("VDDQDDR"); //retrun A
	    iVDDC         = dps_meas.getValue("VDDC");
	    iVDDQBT      = dps_meas.getValue("VDDQBT");
	    iVDDQ         = dps_meas.getValue("VDDQ");
	    iAVDDLV33    = dps_meas.getValue("AVDDLV33");
	    iAVDDSDT   = dps_meas.getValue("AVDDSDT");
	    iAVDDDAC     = dps_meas.getValue("AVDDDAC");
	    iVDDQICIO   = dps_meas.getValue("VDDQICIO");

	    site_num = CURRENT_SITE_NUMBER();
	    if(mdebug){cout<<"SITE:" << site_num << "@DIDD_VDDQDDR:"  <<iVDDQDDR*1.0e3<<"mA"<<endl;}
	    if(mdebug){cout<<"SITE:" << site_num << "@DIDD_VDDC:       "  <<iVDDC*1.0e3<<"mA"<<endl;}
	    if(mdebug){cout<<"SITE:" << site_num << "@DIDD_VDDQBT:"      <<iVDDQBT*1.0e3<<"mA"<<endl;}
	    if(mdebug){cout<<"SITE:" << site_num << "@DIDD_VDDQ:"         <<iVDDQ*1.0e3<<"mA"<<endl;}
	    if(mdebug){cout<<"SITE:" << site_num << "@DIDD_AVDDLV33:"    <<iAVDDLV33*1.0e3<<"mA"<<endl;}
	    if(mdebug){cout<<"SITE:" << site_num << "@DIDD_AVDDSDT:"   <<iAVDDSDT*1.0e3<<"mA"<<endl;}
	    if(mdebug){cout<<"SITE:" << site_num << "@DIDD_AVDDDAC:"     <<iAVDDDAC*1.0e3<<"mA"<<endl;}
	    if(mdebug){cout<<"SITE:" << site_num << "@DIDD_VDDQICIO:"   <<iVDDQICIO*1.0e3<<"mA"<<endl;}

	    TEST("DIDD_VDDQDDR",   "DIDD_VDDQDDR", LIMIT(TM::GT,   5.0 MA, TM::LT,   40.0 MA),   iVDDQDDR,   TM::CONTINUE);
	    TEST("DIDD_VDDC",          "DIDD_VDDC",        LIMIT(TM::GT,  5.0 MA, TM::LT,  80.0 MA),   iVDDC,          TM::CONTINUE);
	    TEST("DIDD_VDDQBT",       "DIDD_VDDQBT",     LIMIT(TM::GT,   5.0 MA, TM::LT,   40.0 MA),   iVDDQBT,       TM::CONTINUE);
	    TEST("DIDD_VDDQ",          "DIDD_VDDQ",        LIMIT(TM::GT,  5.0 MA, TM::LT,   30.0 MA),   iVDDQ,          TM::CONTINUE);
	    TEST("DIDD_AVDDLV33",     "DIDD_AVDDLV33",   LIMIT(TM::GT,   5.0 MA, TM::LT,   50.0 MA),   iAVDDLV33,     TM::CONTINUE);
	    TEST("DIDD_AVDDSDT",    "DIDD_AVDDSDT",  LIMIT(TM::GT,  5.0 MA, TM::LT,    40.0 MA),   iAVDDSDT,    TM::CONTINUE);
	    TEST("DIDD_AVDDDAC",      "DIDD_AVDDDAC",    LIMIT(TM::GT,   0.0 MA, TM::LT,   5.0 MA),   iAVDDDAC,      TM::CONTINUE);
	    TEST("DIDD_VDDQICIO",    "DIDD_VDDQICIO",  LIMIT(TM::GT,     0.1 MA, TM::LT,   5.0 MA),   iVDDQICIO,    TM::CONTINUE);


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
