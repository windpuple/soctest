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

class SIDD: public testmethod::TestMethod {
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
    DOUBLE iVDDQ_DDR_SS,iVDDC,iVDDQ_BT,iVDDQ,iAVDD_LV33,iAVDD_SDI33,iAVDD_SDI,iAVDD_LVPLL;

	DPS_TASK dps_meas;
	int site_num;

        PPMU_SETTING ppmusetup;
        PPMU_RELAY   ppmu_on, ppmu_off;
        PPMU_MEASURE ppmu_measure;
        TASK_LIST    t_list;

	ON_FIRST_INVOCATION_BEGIN();

	    DISCONNECT();
		CONNECT();

	    dps_meas.pin("@"); //This function returns the reference to the DPS_PIN_TASK class object that represents the mea
	    dps_meas.execMode(TM::PVAL); //This member function selects the execution mode.

/*
VDDQ_DDR_SS [V]
VDDC [V]
VDDQ_BT [V]
VDDQ [V]
AVDD_LV33 [V]
AVDD_SDI33 [V]
AVDD_SDI [V]
AVDD_LVPLL [V]
*/
	    dps_meas.pin("VDDQ_DDR_SS").min(0.0 A).max(0.01 A);
	    dps_meas.pin("VDDC").min(0.0 A).max(0.1 A);
	    dps_meas.pin("VDDQ_BT").min(0.0 A).max(0.01 A);
	    dps_meas.pin("VDDQ").min(0.0 A).max(0.1 A);
	    dps_meas.pin("AVDD_LV33").min(0.0 A).max(0.01 A);
	    dps_meas.pin("AVDD_SDI33").min(0.0 A).max(0.01 A);
	    dps_meas.pin("AVDD_SDI").min(0.0 A).max(0.1 A);
	    dps_meas.pin("AVDD_LVPLL").min(0.0 A).max(0.01 A);

	    //dps_meas.trigMode(TM::INTERNAL);
	    //dps_meas.samples(100);

        dps_meas.trigMode(TM::INTERNAL);
        dps_meas.wait(0.05);
        dps_meas.samples(200);


        //ac_relay.pin("@").set("AC","OFF"); ac_relay.execute();
        //ac_relay.pin("CO0,CO1,CO2,CO3,CO4,CO5,CO6,CO7,CO8,CO9,YO0,YO1,YO2,YO3,YO4,YO5,YO6,YO7,YO8,YO9").set("IDLE","OFF");ac_relay.execute();

        ppmusetup.pin("RSTX").iRange(10.0 mA).min(0 mA).max(10.0 mA).vForce(1.0 V);
        ppmu_on.pin("RSTX").status("PPMU_ON");
        t_list.add(ppmusetup).add(ppmu_on);
        t_list.execute();

	    WAIT_TIME(mWait_time ms);

	    ppmusetup.pin("RSTX").iRange(10.0 mA).min(0 mA).max(10.0 mA).vForce(0.0 V);
	    ppmu_on.pin("RSTX").status("PPMU_ON");
	    t_list.add(ppmusetup).add(ppmu_on);
	    t_list.execute();

	    WAIT_TIME(mWait_time ms);
	    dps_meas.execute();


	    ppmusetup.pin("RSTX").iRange(1.0 mA).min(0 mA).max(1.0 mA).vForce(0.0 V);
	    ppmu_off.pin("RSTX").status("AC_ON");
	    t_list.add(ppmusetup).add(ppmu_off);
	    t_list.execute();

	ON_FIRST_INVOCATION_END();

	    iVDDQ_DDR_SS  = dps_meas.getValue("VDDQ_DDR_SS"); //retrun A
	    iVDDC         = dps_meas.getValue("VDDC");
	    iVDDQ_BT      = dps_meas.getValue("VDDQ_BT");
	    iVDDQ         = dps_meas.getValue("VDDQ");
	    iAVDD_LV33    = dps_meas.getValue("AVDD_LV33");
	    iAVDD_SDI33   = dps_meas.getValue("AVDD_SDI33");
	    iAVDD_SDI     = dps_meas.getValue("AVDD_SDI");
	    iAVDD_LVPLL   = dps_meas.getValue("AVDD_LVPLL");

	    site_num = CURRENT_SITE_NUMBER();
	    if(mdebug){cout<<"SITE:" << site_num << "@IDS_VDDQ_DDR_SS:"  <<iVDDQ_DDR_SS*1.0e3<<"mA"<<endl;}
	    if(mdebug){cout<<"SITE:" << site_num << "@IDS_VDDC:       "  <<iVDDC*1.0e3<<"mA"<<endl;}
	    if(mdebug){cout<<"SITE:" << site_num << "@IDS_VDDQ_BT:"      <<iVDDQ_BT*1.0e3<<"mA"<<endl;}
	    if(mdebug){cout<<"SITE:" << site_num << "@IDS_VDDQ:"         <<iVDDQ*1.0e3<<"mA"<<endl;}
	    if(mdebug){cout<<"SITE:" << site_num << "@IDS_AVDD_LV33:"    <<iAVDD_LV33*1.0e3<<"mA"<<endl;}
	    if(mdebug){cout<<"SITE:" << site_num << "@IDS_AVDD_SDI33:"   <<iAVDD_SDI33*1.0e3<<"mA"<<endl;}
	    if(mdebug){cout<<"SITE:" << site_num << "@IDS_AVDD_SDI:"     <<iAVDD_SDI*1.0e3<<"mA"<<endl;}
	    if(mdebug){cout<<"SITE:" << site_num << "@IDS_AVDD_LVPLL:"   <<iAVDD_LVPLL*1.0e3<<"mA"<<endl;}

	    TEST("IDS_VDDQ_DDR_SS",   "IDS_VDDQ_DDR_SS", LIMIT(TM::GE,  -0.1 MA, TM::LE,  10.0 MA),   iVDDQ_DDR_SS,   TM::CONTINUE);
	    TEST("IDS_VDDC",          "IDS_VDDC",        LIMIT(TM::GE,  5.0 MA, TM::LE, 30.0 MA),   iVDDC,          TM::CONTINUE);
	    TEST("IDS_VDDQ_BT",       "IDS_VDDQ_BT",     LIMIT(TM::GE,  -0.1 MA, TM::LE,  5.0 MA),   iVDDQ_BT,       TM::CONTINUE);
	    TEST("IDS_VDDQ",          "IDS_VDDQ",        LIMIT(TM::GE,  1.0 MA, TM::LE, 20.0 MA),   iVDDQ,          TM::CONTINUE);
	    TEST("IDS_AVDD_LV33",     "IDS_AVDD_LV33",   LIMIT(TM::GE,  -0.1 MA, TM::LE,  5.0 MA),   iAVDD_LV33,     TM::CONTINUE);
	    TEST("IDS_AVDD_SDI33",    "IDS_AVDD_SDI33",  LIMIT(TM::GE,  -0.1 MA, TM::LE,  5.0 MA),   iAVDD_SDI33,    TM::CONTINUE);
	    TEST("IDS_AVDD_SDI",      "IDS_AVDD_SDI",    LIMIT(TM::GE,  1.0 MA, TM::LE, 20.0 MA),   iAVDD_SDI,      TM::CONTINUE);
	    TEST("IDS_AVDD_LVPLL",    "IDS_AVDD_LVPLL",  LIMIT(TM::GE,  -0.1 MA, TM::LE,  5.0 MA),   iAVDD_LVPLL,    TM::CONTINUE);

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
REGISTER_TESTMETHOD("SIDD", SIDD);
