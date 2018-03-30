#include <box.h>
#include <elem.h>
#include <mtbox.h>
#include <parallel.h>
#include <scanbox.h>
#include <toolbox.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include "base_const.h"
#include "base_bin_incl.h"
#include "/test2/JJB_LIB/PM.h"


/* #define  DCP_OPTION */
/* #define  TD2_OPTION */
   #define  TD3_OPTION 

   #define  LF_TEST  
/* #define  HF_TEST */

/* #define  FCAL_SKIP  */

static int pll_check_done = 0;
/*********  JJB DBBUG TOOL  *********/
static int one_time = 0;
/*********  JJB DBBUG TOOL  *********/

/*********  JJB HISTOGRAM  *********/
static int    NUM_DUT=0;
static int    HITEM,     HCODE;
static int    SITE_INFO;
static double M_DXX[8];
static char   TESTER_NAME[20], PGM_NAME[50], LOT_NUMBER[30], BD_ID[25];
static char   buffer[1024],  buffer1[100];
static char   YEAR[10], MONTH[5], DAY[5];
static char   START_DATE[100], START_TIME[100];
static int    HH, MM, SS;
static int    N_SITES;
static int    PS_SITE[8];;
/*********  JJB HISTOGRAM  *********/

#define  PROGRAM_REV   "0.0"
#define  CODE_NAME     "JJBSOC"
#define  DEVICE_REV    "A"
#define  PROD_DIR      "/test3/customer/eyenix/prod_data"

#ifdef TD2_OPTION
    #include "td_drvr.h"
    #include "td_elem.h"
    WAVE_RESULT_TYPE *wave;
    BOOLEAN_t gpib_enabled = TRUE;

    PINLIST  *td1,        *td2,        *td3,        *td4,        *td5,        *td6,        *td7,        *td8;
    PINLIST  *TD_PINS;
#endif TD2_OPTION

#ifdef TD3_OPTION
    #include <GPmux.h>
    #include <TD3_drvr.h>

    PINLIST *TD_PINS;
    extern TD3_RESULT_TYPE TD3_data;
    void TD3_data_init() {
        TD3_data.measure1 = 0.0;
        TD3_data.measure2 = 0.0;
        TD3_data.measure3 = 0.0;
        TD3_data.measure4 = 0.0;
        TD3_data.measure5 = 0.0;
        TD3_data.measure6 = 0.0;
        TD3_data.measure7 = 0.0;
        TD3_data.measure8 = 0.0;
    }
PINLIST  *td1,        *td2,        *td3,        *td4,        *td5,        *td6,        *td7,        *td8;
PINLIST  *TD_PINS;
#endif TD3_OPTION

#define  S   *1.0        
#define  MS  *1.0e-3
#define  US  *1.0e-6
#define  NS  *1.0e-9
#define  pS  *1.0e-12
#define  A   *1.0
#define  MA  *1.0e-3
#define  UA  *1.0e-6
#define  NA  *1.0e-9
#define  V   *1.0
#define  MV  *1.0e-3
#define  UV  *1.0e-6
#define  NV  *1.0e-9
#define  HZ  *1.0
#define  KHZ *1.0e3
#define  MHZ *1.0e6
#define  Hz  *1.0
#define  KHz *1.0e3
#define  MHz *1.0e6

FILE  *logfile;
void initialize_tester();
void shutdown_device();

static char PKG_OPT[10];
static int FTTEST;
static void CREATE_PINS();
static void CREATE_PINS_SINGLE();
static void CREATE_PINLIST();
static char LOT_NUM[50],PARTS_QUANTITY[50],TEST_TEMP[50],OPERATOR_NAME[50];
static char OPERATOR_SHIFT[50],cmd[256];
static int  status,statusb,test_option, number_of_active_sites;

static int    fbin;
static double period;
static double TS1,   TS2,   TE1,   TE2,   STB1;
static double vdd1,  vdd2,  vdd3,  vdd4;
static double vref,  vref1, vref2;
static double vil1,  vil2,  vil3,  vil4;
static double vih1,  vih2,  vih3,  vih4;
static double vol1,  vol2,  vol3,  vol4;
static double voh1,  voh2,  voh3,  voh4;
static double DX[4], DXX,   DMIN,  DMAX;
static double PX;
static double VDDIO_TYP,   VDDIO_MIN,   VDDIO_MAX,   VDDIO_VAL;
static double VDDCO_TYP,   VDDCO_MIN,   VDDCO_MAX,   VDDCO_VAL;
static double VDPLL_TYP,   VDPLL_MIN,   VDPLL_MAX,   VDPLL_VAL;
static double VDDAC_TYP,   VDDAC_MIN,   VDDAC_MAX,   VDDAC_VAL;
static double VDDT_TYP,    VDDT_MIN,    VDDT_MAX,    VDDT_VAL;
static double pll_vth0,   pll_vth1,   pll_vth2,   pll_vth3, pll_vth;


/******** for Analog ***********/
#define  num_acps 1
#define  NUM_ACP_CHANS 4

#define  SIZE_1K    1024
#define  SIZE_2K    2048
#define  SIZE_4K    4096
#define  SIZE_16K   16384
#define  SIZE_32K   32768
#define  SIZE_128K  131072
/*
WAVE_RESULT_TYPE *wave;
BOOLEAN_t gpib_enabled = TRUE;
*/
ERRORTYPE_t    *DspStatus;
WaveHdl_t       sine_wf_unav;
WaveHdl_t       sine_wf_av;
WaveHdl_t       top;
WaveHdl_t       bottom;
WaveHdl_t       sine_plus,sine_neg;

WaveHdl_t       ramp_id_wf1;
WaveHdl_t       ramp_id_wf2[4];
WaveHdl_t       dac_inl_wf[NUM_ACP_CHANS];
WaveHdl_t       dac_dnl_wf[NUM_ACP_CHANS];
WaveHdl_t       wave_wf[4], sine_wave[4];

/*******************************************/
/*                                         */
/* DAC output Spec                         */
/*                                         */
/*******************************************/

double dac_lev[16] = { 0.00, 0.60, 1.87, 2.26,
					   2.50, 2.60, 2.64, 2.67,
					   2.69, 2.66, 2.64, 2.57,
					   2.49, 2.32, 1.87, 0.48 };

static double   mcode_worst, dnl_worst, inl_worst, fs_error;
static double   zero_level, fs_level, Vlsb, freq, period1;

static double   thd, snr, sdr, rms_noise, sndr, magfund, dcoffset;
static double   snr_lf, thd_lf, sndr_lf, sfdr_lf, ampfund, offset_v;
static int      num_har, missing_codes, histo_num_av;
static double   lsb;
static double   dac_fs[4];
static double   dac_DLE[4],  dac_ILE[4];
static double   dac_SNR[4],  dac_THD[4],  dac_SNDR[4],  dac_SFDR[4];

/* WAVEFORM DEFINITION */
int   ADC_TRI_HF,  ADC_SINE_HF;
/******** for Analog ***********/


PINLIST    *ALLPIN,     *ALLIN,      *ALLIO,      *ALLOUT,     *LEAKIO, *LEAKIN,  *OSPIN;
PINLIST    *CONTPIN,    *LFACP,      *HFACP,      *DCP_PIN,    *LFACPP,     *LFACPN,     *HFACPP,     *HFACPN;

PINLIST    *XI,         *XO,         *RSTN,       *DI0,        *DI1,        *DI2,        *DI3,        *DI4,        *DI5,        *DI6;
PINLIST    *DI7,        *DI8,        *DI9,        *DSL,        *H1,         *H2,         *RG,         *TP0,        *TP1,        *TP2;
PINLIST    *V2,         *V1,         *SG1,        *V3,         *SG3,        *V4,         *SUB,        *ESYN,       *COXL,       *GPIO17;
PINLIST    *GPIO16,     *GPIO15,     *GPIO14,     *GPIO13,     *RX,         *TX,         *AP,         *AM,         *COMP,       *REXT;
PINLIST    *GPIO12,     *GPIO11,     *GPIO10,     *GPIO9,      *GPIO8,      *GPIO7,      *GPIO6,      *GPIO5,      *GPIO4,      *GPIO3;
PINLIST    *GPIO2,      *GPIO1,      *GPIO0,      *PBLK,       *CLPOB,      *SHP,        *SHD,        *ADCK;

PINLIST    *VDDALL,     *VDDIO,      *VDDCO,      *VDPLL,      *VDDAC;
PINLIST    *VDDTRLY;

PINLIST    *LOADPIN, *ATPGPIN, *GPIOPIN, *DIPIN, *VPIN, *TPPIN, *AMAP, *DACOUT, *FLASHPIN, *IILHPIN;
PINLIST    *FUNCLOAD, *FUNCGRP0, *FUNCGRP2;


PARALLEL_PINLIST pmu_struct;
PARALLEL_PINLIST par_allpin;
PARALLEL_PINLIST par_allin;
PARALLEL_PINLIST par_allio;
PARALLEL_PINLIST par_allout;
PARALLEL_PINLIST par_leakin;
PARALLEL_PINLIST par_ospin;
PARALLEL_PINLIST par_iilhpin;

/*********  JJB HISTOGRAM  *********/
void GET_START_TIME()
{
    sprintf(buffer, get_date());
    sscanf(buffer, "%s %s %s", MONTH, DAY, YEAR);
    if     (!strcmp(MONTH, "Jan")) { sprintf(MONTH, "01"); }
    else if(!strcmp(MONTH, "Feb")) { sprintf(MONTH, "02"); }
    else if(!strcmp(MONTH, "Mar")) { sprintf(MONTH, "03"); }
    else if(!strcmp(MONTH, "Apr")) { sprintf(MONTH, "04"); }
    else if(!strcmp(MONTH, "May")) { sprintf(MONTH, "05"); }
    else if(!strcmp(MONTH, "Jun")) { sprintf(MONTH, "06"); }
    else if(!strcmp(MONTH, "Jul")) { sprintf(MONTH, "07"); }
    else if(!strcmp(MONTH, "Aug")) { sprintf(MONTH, "08"); }
    else if(!strcmp(MONTH, "Sep")) { sprintf(MONTH, "09"); }
    else if(!strcmp(MONTH, "Oct")) { sprintf(MONTH, "10"); }
    else if(!strcmp(MONTH, "Nov")) { sprintf(MONTH, "11"); }
    else if(!strcmp(MONTH, "Dec")) { sprintf(MONTH, "12"); }
    if     (!strcmp(DAY, "1")) { sprintf(DAY, "01"); }
    else if(!strcmp(DAY, "2")) { sprintf(DAY, "02"); }
    else if(!strcmp(DAY, "3")) { sprintf(DAY, "03"); }
    else if(!strcmp(DAY, "4")) { sprintf(DAY, "04"); }
    else if(!strcmp(DAY, "5")) { sprintf(DAY, "05"); }
    else if(!strcmp(DAY, "6")) { sprintf(DAY, "06"); }
    else if(!strcmp(DAY, "7")) { sprintf(DAY, "07"); }
    else if(!strcmp(DAY, "8")) { sprintf(DAY, "08"); }
    else if(!strcmp(DAY, "9")) { sprintf(DAY, "09"); }
    sprintf(START_DATE, "%s%s%02s", YEAR, MONTH, DAY);
    
    sprintf(buffer, get_time());
    sscanf(buffer, "%d:%d:%d", &HH, &MM, &SS);
    sprintf(START_TIME, "%02d%02d%02d", HH, MM, SS);
}

void INIT_LOT_START()
{
    char tmp[30], ans[10];

    INIT_HISTOGRAM_ARRAY();

    debug_printf("\nPlease write the TestBoard NUMBER : ");
    gets(BD_ID);
    debug_printf("Board ID %s\n", BD_ID);

    if(!strcmp(BD_ID, "eng")){
        strcpy(LOT_NUM, "eng");
        vista_debug_command("pdoff");
    }
    else {
        debug_printf("\n ============================================== ");
        debug_printf("\n  bar-code reader input [Y/N] ? ");
        gets(ans);

        if((ans[0]=='y')||(ans[0]=='Y')){
            bd_lot_num_checkR2(PROD_DIR, BD_ID, LOT_NUM);
        }
        else {
            debug_printf("\nPlease write the LOT NUMBER : ");
            gets(LOT_NUM);
            debug_printf("Lot_number %s\n", LOT_NUM);

            bd_lot_num_check(BD_ID, LOT_NUM);

            debug_printf("\nPlease write the Input Quantity : ");
            gets(tmp);
            debug_printf("\nInput Quantity: %s\n", tmp);
            set_test_input(atoi(tmp));

        }
    }

    set_board_id(BD_ID);
    set_lot_num(LOT_NUM);

    GET_START_TIME();
    vista_debug_command("pdon");
    production_display(1);

    if((LOT_NUM[strlen(LOT_NUM)-2] == '-') &&
       ((LOT_NUM[strlen(LOT_NUM)-1] == 'q') || (LOT_NUM[strlen(LOT_NUM)-1] == 'Q'))){
           printf("qa\n");
           FTTEST = 3;
   }
   else{
        printf("ft\n");
        FTTEST = 0;
   }

}


/*********  JJB HISTOGRAM  *********/

void MULTI_DATA_REARRANGE(double *X_result, double *m_result )
{
    int i,j;
    j=0;
    for(i=0; i<N_SITES; i++) { m_result[i] = 9999.99; }
    for(i=0; i<N_SITES; i++)
    {
        if(active_test_sites() & SITE_N(i+1)) { m_result[i]=X_result[j++]; }
    }
}



/*******************************************************/
/*            LEVEL  SET                               */
/*******************************************************/

void UTIL_POWER()
{
    set_voltage_supply(PS5, 5.0, 1.0, NODELAY, 0.0);
}

void POWER_DOWN()
{
    vi8_voltage_setup (VDDALL,   0.0, VI8_4V_RANGE, VI8_200MA, 200.0e-3, -200.0e-3);
    vi8_relays        (VDDALL, VI8_CLOSE, VI8_LOCAL_KELVIN_RELAY |VI8_FORCE_RELAY|VI8_SENSE_RELAY );
    set_voltage_supply(PS6, 0.0, 1.0, NODELAY, 0.0); 

    if(active_test_sites() & SITE1) { set_voltage_supply(PS1, 0.0, 1.0, NODELAY, 0.0);  }
    if(active_test_sites() & SITE2) { set_voltage_supply(PS2, 0.0, 1.0, NODELAY, 0.0);  }
    if(active_test_sites() & SITE3) { set_voltage_supply(PS3, 0.0, 1.0, NODELAY, 0.0);  }
    if(active_test_sites() & SITE4) { set_voltage_supply(PS4, 0.0, 1.0, NODELAY, 0.0);  }

}

void POWER_OFF()
{
int i;

    open_io_relays(ALLPIN);
    prim_wait(0.01);
    if(active_test_sites() & SITE1) { set_voltage_supply(PS1, 0.0, 1.0, NODELAY, 0.0);  }
    if(active_test_sites() & SITE2) { set_voltage_supply(PS2, 0.0, 1.0, NODELAY, 0.0);  }
    if(active_test_sites() & SITE3) { set_voltage_supply(PS3, 0.0, 1.0, NODELAY, 0.0);  }
    if(active_test_sites() & SITE4) { set_voltage_supply(PS4, 0.0, 1.0, NODELAY, 0.0);  }

    for(i=0; i<N_SITES; i++) {
        if(active_test_sites() & SITE_N(i+1)) { PARALLEL_CALL_BY_SITE(VDDTRLY, SITE_N(i+1), open_fixture_relays(VDDTRLY, 0.0)); }
    }
    vi8_voltage_setup (VDDALL,      0.0, VI8_4V_RANGE, VI8_20MA, 10.0e-3, -10.0e-3);
    vi8_relays        (VDDALL, VI8_OPEN, VI8_LOCAL_KELVIN_RELAY | VI8_FORCE_RELAY | VI8_SENSE_RELAY );

}

void POWER_DUT()
{
int i;

    vi8_voltage_setup (VDDIO,  VDDIO_VAL, VI8_4V_RANGE, VI8_200MA, 200.0e-3, -200.0e-3);
    vi8_voltage_setup (VDDCO,  VDDCO_VAL, VI8_4V_RANGE, VI8_200MA, 200.0e-3, -200.0e-3);
    vi8_voltage_setup (VDPLL,  VDPLL_VAL, VI8_4V_RANGE, VI8_200MA, 200.0e-3, -200.0e-3);
    vi8_voltage_setup (VDDAC,  VDDAC_VAL, VI8_4V_RANGE, VI8_200MA, 200.0e-3, -200.0e-3);

    vi8_relays        (VDDALL, VI8_CLOSE, VI8_LOCAL_KELVIN_RELAY |VI8_FORCE_RELAY|VI8_SENSE_RELAY );

    if(active_test_sites() & SITE1) { set_voltage_supply(PS1, VDDT_VAL, 1.0, NODELAY, 0.0);  }
    if(active_test_sites() & SITE2) { set_voltage_supply(PS2, VDDT_VAL, 1.0, NODELAY, 0.0);  }
    if(active_test_sites() & SITE3) { set_voltage_supply(PS3, VDDT_VAL, 1.0, NODELAY, 0.0);  }
    if(active_test_sites() & SITE4) { set_voltage_supply(PS4, VDDT_VAL, 1.0, NODELAY, 0.0);  }

    for(i=0; i<N_SITES; i++) {
        if(active_test_sites() & SITE_N(i+1)) { PARALLEL_CALL_BY_SITE(VDDTRLY, SITE_N(i+1), close_fixture_relays(VDDTRLY, 0.0)); }
    }

}

void OSP_LEVEL()
{
    SET_LEVEL(OSPIN, 0, 0.0, 0.0, 0.2, 0.8, -1.0, -1.0, -1.0);
    set_active_loads(OSPIN, PULLUP_PER_PIN,  500.0 UA, 0.5,  2.0);
}

void OSN_LEVEL()
{
    SET_LEVEL(OSPIN, 0, 0.0, 0.0, -0.8, -0.2, -1.0, -1.0, -1.0);
    set_active_loads(OSPIN, PULLDOWN_PER_PIN, -500.0 UA, 2.0, -2.0);
}

void ACTIVE_LOAD()
{
double iolhd, iolh1, iolh2, iolh4, iolh8, iolh16, iolh20;

    vref = VDDIO_VAL/2.0;

    iolhd = 100.0 UA; iolh1 = 1.0 MA; iolh4 = 4.0 MA, iolh8 = 8.0 MA; iolh16 = 16.0 MA; iolh20 = 20.0 MA;

    set_active_loads(ALLPIN,   PULL_BOTH_PER_PIN,   iolhd, 0.5, vref,   iolhd, 2.0, vref);
}
/***
    VDDIO_VAL = 3.30; VDDCO_VAL = 1.80; VDPLL_VAL = 1.80; VDDAC_VAL = 3.30; VDDT_VAL = 3.30;
***/
void GROSS_LEVEL(int PRT)
{
    POWER_DUT();
                
    vil1 = 0.00 * VDDIO_VAL;   vih1 = 1.00 * VDDIO_VAL;
    vol1 = 0.45 * VDDIO_VAL;   voh1 = 0.50 * VDDIO_VAL;
 
    SET_LEVEL(ALLPIN, 0, vil1, vih1, vol1, voh1, 100.0 UA, 100.0 UA, VDDIO_VAL/2);
    ACTIVE_LOAD();

    if(spec("logger") && PRT) {
        debug_printf("\n< GROSS LEVEL >\n");
        debug_printf("VDDIO = %5.2fV, VDDCO = %5.2fV, VDPLL = %5.2fV, VDDAC = %5.2fV\n", 
                      VDDIO_VAL, VDDCO_VAL, VDPLL_VAL, VDDAC_VAL);
        debug_printf("Normal VIL/VIH = %5.2f/%5.2f  VOL/VOH = %5.2f/%5.2f\n", vil1, vih1, vol1, voh1);
        debug_printf("Active Load Enabled\n");
    }
}


void F_PWR_DOWN(int failed_sites )
{
int i;

    if(failed_sites & SITE1) { set_voltage_supply(PS1, 0.0, 1.0, NODELAY, 0.0);  }
    if(failed_sites & SITE2) { set_voltage_supply(PS2, 0.0, 1.0, NODELAY, 0.0);  }
    if(failed_sites & SITE3) { set_voltage_supply(PS3, 0.0, 1.0, NODELAY, 0.0);  }
    if(failed_sites & SITE4) { set_voltage_supply(PS4, 0.0, 1.0, NODELAY, 0.0);  }

    for(i=0; i<N_SITES; i++) {
        if(failed_sites & SITE_N(i+1)) {
            PARALLEL_CALL_BY_SITE(ALLPIN, SITE_N(i+1), disconnect_dut(ALLPIN));
            PARALLEL_CALL_BY_SITE(ALLPIN, SITE_N(i+1), open_pmu_relays(ALLPIN));

            PARALLEL_CALL_BY_SITE(VDDALL, SITE_N(i+1), vi8_voltage_setup(VDDALL, 0.0, VI8_4V_RANGE, VI8_20MA, 10.0e-3, -10.0e-3));
            PARALLEL_CALL_BY_SITE(VDDALL, SITE_N(i+1), vi8_relays       (VDDALL, VI8_OPEN, VI8_LOCAL_KELVIN_RELAY | VI8_FORCE_RELAY| VI8_SENSE_RELAY  ));
            PARALLEL_CALL_BY_SITE(VDDTRLY, SITE_N(i+1), open_fixture_relays(VDDTRLY, 0.0));
        }
    }
}


/*******************************************************/
/*            TIMING SET                               */
/*******************************************************/

void OS_TIME(double c_time)
{
    STB1= c_time * 0.9;

    set_cycle_length(0, c_time, NULL);

    SET_TIME(OSPIN, 0, DNRZ, 0.0, 0.0, DNRI, 0.0, 0.0, TRISTATE, STB1, NOCHANGE);
}

void FUNC_TIME(double c_time)
{
double edg1, edg2, edg3, edg4;
double dly1, dly2, dly3, dly4;
double stb1, stb2, stb3, stb4, stb5, stb6, stb7;
/*
    edg1 = c_time * 0.05; 
    edg2 = c_time * 0.50;
    dly1 = c_time * 0.00;
    dly2 = c_time * 0.125;
    dly3 = c_time * 0.975;
    stb1 = c_time * 0.625;
    stb2 = c_time * 1.125;
    stb3 = c_time * 0.675;
    stb4 = c_time * 1.50;
    stb5 = c_time * 0.750;
    stb6 = c_time * 0.80;
*/
/*
    edg1 = c_time * 0.00;
    edg2 = c_time * 0.50;
    dly1 = c_time * 0.00;
    dly2 = c_time * 0.125;
    dly3 = c_time * 0.975;
    stb1 = c_time * 0.625;
    stb2 = c_time * 0.70;
    stb3 = c_time * 1.15;
    stb4 = c_time * 0.90;
    stb5 = c_time * 0.350;
    stb6 = c_time * 0.40;
    stb7 = c_time * 0.20;
*/


    edg1 = c_time * 0.00;
    edg2 = c_time * 0.50;
    dly1 = c_time * 0.00;
    dly2 = c_time * 0.125;
    dly3 = c_time * 0.975;
    stb1 = c_time * 0.80;
    stb2 = c_time * 0.40;
    stb3 = c_time * 0.20;



    set_cycle_length(0, c_time, NULL);
/*  40ns
	SET_TIME(ALLPIN, 0,     DNRZ, dly1, dly1, DNRI,  0.0,  0.0, MASK,   stb1, NOCHANGE);
    SET_TIME(XI,    0,        RZ, edg1, edg2, DNRI, -1.0, -1.0, MASK,   -1.0, NOCHANGE);
    SET_TIME(RSTN,  0,      DNRZ, dly2, dly2, DNRI, -1.0, -1.0, MASK,   -1.0, NOCHANGE);
    SET_TIME(FUNCGRP2,  0,  DNRZ, dly3, dly3, DNRI, -1.0, -1.0, MASK,   -1.0, NOCHANGE);
    SET_TIME(FUNCGRP0,  0,  DNRZ, dly1, dly1, DNRI, -1.0, -1.0, NORMAL, stb1, NOCHANGE);
    SET_TIME(AM,  0,  DNRZ, dly1, dly1, DNRI, -1.0, -1.0, MASK, stb1, NOCHANGE);
    SET_TIME(AP,  0,  DNRZ, dly1, dly1, DNRI, -1.0, -1.0, MASK, stb1, NOCHANGE);
    SET_TIME(CLPOB,  0,  DNRZ, dly1, dly1, DNRI, -1.0, -1.0, NORMAL, stb2, NOCHANGE);
    SET_TIME(PBLK,  0,   DNRZ, dly1, dly1, DNRI, -1.0, -1.0, NORMAL, stb2, NOCHANGE);
    SET_TIME(SHD,  0,   DNRZ, dly1, dly1, DNRI, -1.0, -1.0, NORMAL, stb5, NOCHANGE);
    SET_TIME(H2,  0,   DNRZ, dly1, dly1, DNRI, -1.0, -1.0, NORMAL, stb4, NOCHANGE);
    SET_TIME(RG,  0,   DNRZ, dly1, dly1, DNRI, -1.0, -1.0, NORMAL, stb4, NOCHANGE);
    SET_TIME(SHP,  0,   DNRZ, dly1, dly1, DNRI, -1.0, -1.0, NORMAL, stb6, NOCHANGE);
    SET_TIME(H1,  0,   DNRZ, dly1, dly1, DNRI, -1.0, -1.0, NORMAL, stb5, NOCHANGE);
    SET_TIME(XO,  0,   DNRZ, dly1, dly1, DNRI, -1.0, -1.0, NORMAL, stb5, NOCHANGE);
*/
/*
    SET_TIME(ALLPIN, 0,     DNRZ, dly1, dly1, DNRI,  0.0,  0.0, MASK,   stb1, NOCHANGE);
    SET_TIME(XI,    0,        RZ, edg1, edg2, DNRI, -1.0, -1.0, MASK,   -1.0, NOCHANGE);
    SET_TIME(RSTN,  0,      DNRZ, dly2, dly2, DNRI, -1.0, -1.0, MASK,   -1.0, NOCHANGE);
    SET_TIME(FUNCGRP2,  0,  DNRZ, dly3, dly3, DNRI, -1.0, -1.0, MASK,   -1.0, NOCHANGE);
    SET_TIME(FUNCGRP0,  0,  DNRZ, dly1, dly1, DNRI, -1.0, -1.0, NORMAL, stb1, NOCHANGE);
    SET_TIME(AM,  0,  DNRZ, dly1, dly1, DNRI, -1.0, -1.0, MASK, stb1, NOCHANGE);
    SET_TIME(AP,  0,  DNRZ, dly1, dly1, DNRI, -1.0, -1.0, MASK, stb1, NOCHANGE);
    SET_TIME(CLPOB,  0,  DNRZ, dly1, dly1, DNRI, -1.0, -1.0, NORMAL, stb2, NOCHANGE);
    SET_TIME(PBLK,  0,   DNRZ, dly1, dly1, DNRI, -1.0, -1.0, NORMAL, stb2, NOCHANGE);
    SET_TIME(SHD,  0,   DNRZ, dly1, dly1, DNRI, -1.0, -1.0, NORMAL, stb4, NOCHANGE);
    SET_TIME(H2,  0,   DNRZ, dly1, dly1, DNRI, -1.0, -1.0, NORMAL, stb4, NOCHANGE);
    SET_TIME(RG,  0,   DNRZ, dly1, dly1, DNRI, -1.0, -1.0, NORMAL, stb3, NOCHANGE);
    SET_TIME(SHP,  0,   DNRZ, dly1, dly1, DNRI, -1.0, -1.0, NORMAL, stb6, NOCHANGE);
    SET_TIME(H1,  0,   DNRZ, dly1, dly1, DNRI, -1.0, -1.0, NORMAL, stb5, NOCHANGE);
    SET_TIME(XO,  0,   DNRZ, dly1, dly1, DNRI, -1.0, -1.0, NORMAL, stb2, NOCHANGE);
    SET_TIME(ADCK,  0,   DNRZ, dly1, dly1, DNRI, -1.0, -1.0, NORMAL, stb7, NOCHANGE);
*/


    SET_TIME(ALLPIN, 0,     DNRZ, dly1, dly1, DNRI,  0.0,  0.0, MASK,   stb1, NOCHANGE);
    SET_TIME(XI,    0,        RZ, edg1, edg2, DNRI, -1.0, -1.0, MASK,   -1.0, NOCHANGE);
    SET_TIME(RSTN,  0,      DNRZ, dly2, dly2, DNRI, -1.0, -1.0, MASK,   -1.0, NOCHANGE);
    SET_TIME(FUNCGRP2,  0,  DNRZ, dly3, dly3, DNRI, -1.0, -1.0, MASK,   -1.0, NOCHANGE);
    SET_TIME(FUNCGRP0,  0,  DNRZ, dly1, dly1, DNRI, -1.0, -1.0, NORMAL, stb1, NOCHANGE);
    SET_TIME(AM,  0,  DNRZ, dly1, dly1, DNRI, -1.0, -1.0, MASK, stb1, NOCHANGE);
    SET_TIME(AP,  0,  DNRZ, dly1, dly1, DNRI, -1.0, -1.0, MASK, stb1, NOCHANGE);
    SET_TIME(H1,  0,   DNRZ, dly1, dly1, DNRI, -1.0, -1.0, NORMAL, stb2, NOCHANGE);
    SET_TIME(ADCK,  0,   DNRZ, dly1, dly1, DNRI, -1.0, -1.0, NORMAL, stb3, NOCHANGE);




}

void SIDD_TIME(double c_time, int PRT)
{
double edg1, edg2, edg3, edg4;
double dly1, dly2, dly3, dly4;
double stb1, stb2, stb3, stb4;

    edg1 = c_time * 0.00; 
    edg2 = c_time * 0.50;  
    dly1 = c_time * 0.00;  
    dly2 = c_time * 0.50;  
    stb1 = c_time * 0.85;  
    stb2 = c_time * 0.285;  

    set_cycle_length(0, c_time, NULL);
    SET_TIME(ALLPIN, 0, DNRZ, dly1, dly1, DNRI,  0.0,  0.0, NORMAL,  stb1, NOCHANGE);
    SET_TIME(GPIO17, 0, DNRZ, dly1, dly1, DNRI,  0.0,  0.0, NORMAL,  stb2, NOCHANGE);
    SET_TIME(XI,    0,   RZ, edg1, edg2, DNRI, -1.0, -1.0, NORMAL,  -1.0, NOCHANGE);

    if(spec("logger") && PRT) {
        debug_printf("\n< FUNC TIME >\n");
        debug_printf("Period = %7.2fNS\n",           c_time * 1.0e9);
        debug_printf("   RZ  = %7.2fNS, %7.2fNS\n",  edg1 * 1.0e9, edg2 * 1.0e9);
        debug_printf(" DNRZ  = %7.2fNS\n",           dly1 * 1.0e9);
        debug_printf(" STB1  = %7.2fNS\n",           stb1 * 1.0e9);
    }
}

void DIDD_TIME(double c_time, int PRT)
{
double edg1, edg2, edg3, edg4;
double dly1, dly2, dly3, dly4;
double stb1, stb2, stb3, stb4;

    edg1 = c_time * 0.00;
    edg2 = c_time * 0.50;
    dly1 = c_time * 0.00;
    dly2 = c_time * 0.50;
    stb1 = c_time * 0.85;
    stb2 = c_time * 0.27;

    set_cycle_length(0, c_time, NULL);
    SET_TIME(ALLPIN, 0, DNRZ, dly1, dly1, DNRI,  0.0,  0.0, NORMAL,  stb1, NOCHANGE);
    SET_TIME(AMAP,   0, DNRZ, dly1, dly1, DNRI,  0.0,  0.0, NORMAL,  stb2, NOCHANGE);
    SET_TIME(XI,    0,   RZ, edg1, edg2, DNRI, -1.0, -1.0, NORMAL,  -1.0, NOCHANGE);

    if(spec("logger") && PRT) {
        debug_printf("\n< FUNC TIME >\n");
        debug_printf("Period = %7.2fNS\n",           c_time * 1.0e9);
        debug_printf("   RZ  = %7.2fNS, %7.2fNS\n",  edg1 * 1.0e9, edg2 * 1.0e9);
        debug_printf(" DNRZ  = %7.2fNS\n",           dly1 * 1.0e9);
        debug_printf(" STB1  = %7.2fNS\n",           stb1 * 1.0e9);
    }
}

void DAC_TIME(double c_time, int PRT)
{
double edg1, edg2, edg3, edg4;
double dly1, dly2, dly3, dly4;
double stb1, stb2, stb3, stb4;

    edg1 = c_time * 0.00;
    edg2 = c_time * 0.50;
    dly1 = c_time * 0.00;
    dly2 = c_time * 0.50;
    stb1 = c_time * 0.85;
    stb2 = c_time * 0.27;

    set_cycle_length(0, c_time, NULL);
    SET_TIME(ALLPIN, 0, DNRZ, dly1, dly1, DNRI,  0.0,  0.0, NORMAL,  stb1, NOCHANGE);
    SET_TIME(GPIO17,   0, DNRZ, dly1, dly1, DNRI,  0.0,  0.0, NORMAL,  stb2, NOCHANGE);
    SET_TIME(XI,    0,   RZ, edg1, edg2, DNRI, -1.0, -1.0, NORMAL,  -1.0, NOCHANGE);
    SET_TIME(COMP,  0,   F1,  0.0,  0.0,  FON,  0.0,  0.0,   MASK,   0.0, NOCHANGE);

    if(spec("logger") && PRT) {
        debug_printf("\n< FUNC TIME >\n");
        debug_printf("Period = %7.2fNS\n",           c_time * 1.0e9);
        debug_printf("   RZ  = %7.2fNS, %7.2fNS\n",  edg1 * 1.0e9, edg2 * 1.0e9);
        debug_printf(" DNRZ  = %7.2fNS\n",           dly1 * 1.0e9);
        debug_printf(" STB1  = %7.2fNS\n",           stb1 * 1.0e9);
    }
}



void BIST_TIME(double c_time, int PRT)
{
double edg1, edg2, edg3, edg4;
double dly1, dly2, dly3, dly4;
double stb1, stb2, stb3, stb4;

    edg1 = c_time * 0.00; 
    edg2 = c_time * 0.50;
    dly1 = c_time * 0.00;
    dly2 = c_time * 0.50;
    stb1 = c_time * 0.90;
    stb2 = c_time * 0.15; 

    set_cycle_length(0, c_time, NULL);
    SET_TIME(ALLPIN, 0, DNRZ, dly1, dly1, DNRI,  0.0,  0.0, MASK,  stb1, NOCHANGE);
    SET_TIME(SUB, 0, DNRZ, dly1, dly1, DNRI,  0.0,  0.0, NORMAL,  stb2, NOCHANGE);
    SET_TIME(XI,     0,   RZ, edg1, edg2, DNRI, -1.0, -1.0, NORMAL,  -1.0, NOCHANGE);

    if(spec("logger") && PRT) {
        debug_printf("\n< FUNC TIME >\n");
        debug_printf("Period = %7.2fNS\n",           c_time * 1.0e9);
        debug_printf("   RZ  = %7.2fNS, %7.2fNS\n",  edg1 * 1.0e9, edg2 * 1.0e9);
        debug_printf(" DNRZ  = %7.2fNS\n",           dly1 * 1.0e9);
        debug_printf(" STB1  = %7.2fNS\n",           stb1 * 1.0e9);
    }
}

void FLASH_TIME(double c_time, int PRT)
{
double edg1, edg2, edg3, edg4;
double dly1, dly2, dly3, dly4;
double stb1, stb2, stb3, stb4;

    edg1 = c_time * 0.00;
    edg2 = c_time * 0.50;
    dly1 = c_time * 0.00;
    dly2 = c_time * 0.50;
    stb1 = c_time * 0.85;
    stb2 = c_time * 0.27;

    set_cycle_length(0, c_time, NULL);
    SET_TIME(ALLPIN, 0, DNRZ, dly1, dly1, DNRI,  0.0,  0.0, MASK,  stb1, NOCHANGE);
    SET_TIME(XI,     0,   RZ, edg1, edg2, DNRI, -1.0, -1.0, NORMAL,  -1.0, NOCHANGE);
    SET_TIME(GPIO7,  0, DNRZ, dly1, dly1, DNRI,  0.0,  0.0, NORMAL,  stb2, NOCHANGE);

    set_cycle_length(1, c_time, NULL);
    SET_TIME(ALLPIN, 1, DNRZ, dly1, dly1, DNRI,  0.0,  0.0, MASK,  stb1, NOCHANGE);
    SET_TIME(XI,     1,   RZ, edg1, edg2, DNRI, -1.0, -1.0, NORMAL,  -1.0, NOCHANGE);
    SET_TIME(GPIO7,  1, DNRZ, dly1, dly1, DNRI,  0.0,  0.0, NORMAL,  stb2, NOCHANGE);


    if(spec("logger") && PRT) {
        debug_printf("\n< FUNC TIME >\n");
        debug_printf("Period = %7.2fNS\n",           c_time * 1.0e9);
        debug_printf("   RZ  = %7.2fNS, %7.2fNS\n",  edg1 * 1.0e9, edg2 * 1.0e9);
        debug_printf(" DNRZ  = %7.2fNS\n",           dly1 * 1.0e9);
        debug_printf(" STB1  = %7.2fNS\n",           stb1 * 1.0e9);
    }
}



void ATPG_TIME(double c_time, int PRT)
{
double edg1, edg2, edg3, edg4;
double dly1, dly2, dly3, dly4;
double stb1, stb2, stb3, stb4;

    edg1 = c_time * 0.45;  /*  0ns : 100ns * 0.45 */
    edg2 = c_time * 0.55;  /* 50ns : 100ns * 0.55 */
    dly1 = c_time * 0.00;  /* 10ns : 100ns * 0.00 */
    dly2 = c_time * 0.50;  /* 10ns : 100ns * 0.50 */
    stb1 = c_time * 0.40;  /* 90ns : 100ns * 0.85 */

    set_cycle_length(0, c_time, NULL);
    SET_TIME(ALLPIN, 0, DNRZ, dly1, dly1, DNRI,  0.0,  0.0, NORMAL,  stb1, NOCHANGE);
    SET_TIME(XI,    0,   RZ, edg1, edg2, DNRI, -1.0, -1.0, NORMAL,  -1.0, NOCHANGE);
    SET_TIME(RSTN,    0,   R1, edg2, edg1, DNRI, -1.0, -1.0, NORMAL,  -1.0, NOCHANGE);

    if(spec("logger") && PRT) {
        debug_printf("\n< FUNC TIME >\n");
        debug_printf("Period = %7.2fNS\n",           c_time * 1.0e9);
        debug_printf("   RZ  = %7.2fNS, %7.2fNS\n",  edg1 * 1.0e9, edg2 * 1.0e9);
        debug_printf(" DNRZ  = %7.2fNS\n",           dly1 * 1.0e9);
        debug_printf(" STB1  = %7.2fNS\n",           stb1 * 1.0e9);
    }
}

int TEST_RESULTS(char testname[20], PINLIST *pins, double *var_array, double lo_limit, double hi_limit, int units, char bin[20])
{
int status, i;
    status = variable_test (pins, var_array, lo_limit, hi_limit, units, TEST_SINGLE_RESULT);
    if((status & PASS_FAIL_MASK) == FAIL){
        F_PWR_DOWN(status>>8);
        number_of_active_sites = deactivate_failed_sites(status>>8);

        set_bin (bin_name(bin),status>>8);
        if (number_of_active_sites == 0)
        return 1;
    }
     return 0;
}


void FREQ_SHMOO(char *label, double TCX_MIN, double TCX_MAX, double TSTEP, double tVDD_MIN, double tVDD_MAX, double VSTEP )
{
double tVDD, tVDD_STEP, vdd_test, vdd_test2;
double TCX, TCX_STEP, T1, T3, Tset, debug_vdd;
unsigned int COM1, COM2, T2, debug_freq, over_flg, data_flg, pause_flg, timing;

    data_flg = datalog_flag(DEFAULT_MODE, NULL); datalog_flag(LOG_OFF, NULL);
    over_flg = override_flag(DEFAULT_MODE, NULL); override_flag(0x000000, NULL);
    pause_flg = pause_flag(DEFAULT_MODE, NULL); pause_flag(0x000000, NULL);


    TCX_STEP  = (  TCX_MAX -  TCX_MIN ) / ( TSTEP + 1 );
    tVDD_STEP = ( tVDD_MAX - tVDD_MIN ) / ( VSTEP + 1 );

    debug_printf("\n\n");
    debug_printf("************************************************************\n");
    debug_printf("** FREQUENCY SHMOO PLOT  @VDDio : %4.2fV   @VDDco : %4.2fV  **\n", VDDIO_VAL, VDDCO_VAL);
    debug_printf("************************************************************\n");
    debug_printf("** Pattern : %-30s               **\n", label);
    debug_printf("************************************************************\n");

    debug_printf("VDDIO(V)  1....+....+....+....4\n");

    for(TCX= TCX_MAX; TCX>=TCX_MIN; TCX-=TCX_STEP) {

             if(!strcmp(label, "func_415"))     { timing =  1; }
        else { debug_printf("\nYou have wrong information or code.\n"); }

        switch(timing) {
           case   1: { FUNC_TIME(TCX);   break; }
           default : { debug_printf("\nYou have wrong Timing.\n"); break; }
        }
        if(TCX<10.001 NS) { TCX = 10.0001 NS; }

        debug_printf("%7.2fNS ",TCX*1.0e9);
        for(tVDD=tVDD_MIN; tVDD<=tVDD_MAX; tVDD+=tVDD_STEP) {
    
	        VDDIO_VAL = tVDD; VDDCO_VAL = 1.80; VDPLL_VAL = 1.80; VDDAC_VAL = 3.30; VDDT_VAL = 3.30;

            GROSS_LEVEL(0);

            JJB_SFUNC(ALLPIN,  get_patref_start(label), get_patref_stop(label));
        }
        debug_printf("\n");
    }
    debug_printf("VDDIO(V)  1....+....+....+....4\n");

    datalog_flag(data_flg, NULL); override_flag(over_flg, NULL); pause_flag(pause_flg, NULL);
}

void VDD_SHMOO(char *label, char *testpin, double tVDDi_MIN, double tVDDi_MAX, double tVDDi_STEP, double tVDDc_MIN, double tVDDc_MAX, double tVDDc_STEP )
{
unsigned int over_flg, data_flg, pause_flg, timing;
double TCX, TCX_MIN, TCX_MAX, TCX_STEP, T1, T3, Tset, debug_vdd;
double tVDDIO, tVDDCO, VDDIO_STEP, VDDCO_STEP;

    data_flg = datalog_flag(DEFAULT_MODE, NULL); datalog_flag(LOG_OFF, NULL);
    over_flg = override_flag(DEFAULT_MODE, NULL); override_flag(0x000000, NULL);
    pause_flg = pause_flag(DEFAULT_MODE, NULL); pause_flag(0x000000, NULL);

    VDDIO_STEP = ( tVDDi_MAX - tVDDi_MIN) / ( tVDDi_STEP + 1 );
    VDDCO_STEP = ( tVDDc_MAX - tVDDc_MIN) / ( tVDDc_STEP + 1 );

    debug_printf("\n\n\n\n\n");
    debug_printf("******************************************************************\n");
    debug_printf("** VDDIO vs VDDC SHMOO ( @%4s Pattern )  @TestPin : %7s **\n", label, testpin);
    debug_printf("******************************************************************\n\n");
    debug_printf("**  [VDDio vs VDDcore] MARGIN SHMOO   **\n");
    debug_printf(" VDDI (V)1....+....2....+....3 [ VDDCO ]\n");


    for(tVDDIO = tVDDi_MAX; tVDDIO>=tVDDi_MIN-0.01; tVDDIO-=VDDIO_STEP) {

        VDDIO_VAL = tVDDIO; VDDCO_VAL = 1.80; VDPLL_VAL = 1.80; VDDAC_VAL = 3.30; VDDT_VAL = 3.30;

        GROSS_LEVEL(0);
        debug_printf("%7.2f V ",tVDDIO);

        for(tVDDCO=tVDDc_MIN; tVDDCO<=tVDDc_MAX+0.01; tVDDCO+=VDDCO_STEP) {

        VDDIO_VAL = tVDDIO; VDDCO_VAL = tVDDCO; VDPLL_VAL = 1.80; VDDAC_VAL = 3.30; VDDT_VAL = 3.30;

        GROSS_LEVEL(0);

        status=func_test(FUNCLOAD, get_patref_start(label), get_patref_stop(label));

       if((status & PASS_FAIL_MASK) == FAIL){ debug_printf("."); }
       else                                 { debug_printf("P"); }

}
        debug_printf("\n");
    }
    debug_printf(" VDDI (V)1....+....2....+....3 [ VDDCO ]\n");

    datalog_flag(data_flg, NULL);
    override_flag(over_flg, NULL);
    pause_flag(pause_flg, NULL);
}



int check_dsp (DspStatus)
ERRORTYPE_t *DspStatus;
{
    static ET_STRINGS_t *etsPtr;

    if( DspStatus == DSP_NOERROR) { return NOERROR; }
    else {
        if( DspStatus == DSP_NOT_DONE) {
            debug_printf ("DSP function in test %s did not", test_name(NULL));
            return 3;
       }
       else {
            etsPtr = dsp_error_msg (*DspStatus);

            debug_printf ("DSP function in test %s error  ", test_name(NULL));
            debug_printf ("SEVERITY %s  MSG NUMBER %s\n", etsPtr->ets_severity, etsPtr->ets_msg_number);

            if( DspStatus->et_severity == FATAL) {
                debug_printf ("DSP FATAL Error detected.\n");
                return FATAL;
            }
            if( DspStatus->et_severity == WARN) {
                debug_printf ("DSP WARNING Error detected.\n");
                return WARN;
            }
        }
    }
}

my_end_func(acpdcp_pins, cp_timeout)  /* void delete */
PINLIST  *acpdcp_pins;
double    cp_timeout;
{
    load_pinlist(acpdcp_pins, UNUSED);
    dsp_event_wait(acpdcp_pins,cp_timeout);
    set_func_mode(STOP_ON_FIRST);
    func_advance(NULL);
    func_stop();
    /* superclock_run(DISABLE_RUN); */
    superclock_control(acpdcp_pins, SC_DISABLE_RUN);
    cp_capture_enable(FALSE); /* Disable capture for reading data */
    acp_channel_enable(FALSE);
}


/*+++++++++++++++++++++++ INITIALIZE_TESTER +++++++++++++++++++++++++++++*/
ScanChainPinMap *map1;
void initialize_tester()
{
int  start_addr;            /* Vector Location */
int  link_number = 0;       /* SCAN Vector Location */
int  loadsw;
char answer[10]; 

    set_site_count(4);
    set_operational(1, TRUE);
    set_operational(2, TRUE);
    set_operational(3, TRUE);
    set_operational(4, TRUE);
    N_SITES = 4;
    CREATE_PINS(); 
    CREATE_PINLIST();

/****************************************************************/
/***       soft   hard   bin              category    pass or ***/
/***       bin    bin    description      display     fail    ***/
/****************************************************************/
/************  Soft Bin vs Character Mapping  *******************
    11111111112222222222333333  44444444555555555566666666
	01234567890123456789012345  23456789012345678901234567
	------------------------------------------------------
    ABCDEFGHIJKLMNOPQRSTUVWXYZ  abcdefghijklmnopqrstuvwxyz
*****************************************************************/
    make_bin(  1,    1,     "PASS",         " .",    PASS);

    make_bin(  2,    2,     "F_OS",         " 2",    FAIL);
    make_bin(  3,    5,     "F_IIL",        " 3",    FAIL);
    make_bin(  4,    5,     "F_IIH",        " 4",    FAIL);

    make_bin(  9,    5,     "F_IDD",        " 9",    FAIL);

    make_bin( 11,    3,     "F_BIST_GRS",       " B",    FAIL);
    make_bin( 12,    3,     "F_FLASH_GRS",       " C",    FAIL);
    make_bin( 13,    3,     "F_FUNC_GRS",       " D",    FAIL);

    make_bin( 31,    5,     "F_DAC",       " C",    FAIL);
    make_bin( 35,    5,     "F_PLL",       " C",    FAIL);
    make_bin( 99,    6,     "UNTESTED",     "99",    FAIL);
 
    INIT_LOT_START();

#ifdef FCAL_SKIP
RE_FOCUSE_CAL:
    debug_printf("\nIs focused calibration desired? (y/n) ");
    gets(answer);
    if((answer[0] == 'y') || (answer[0] == 'Y')) {
        set_focused_cal_levels(TDRPIN, 0.0, 2.5, 1.25, 1.25);
        debug_printf("\nDo you want the previous cal data? (y/n) ");
        gets(answer);
        if((answer[0] == 'y') || (answer[0] == 'Y')) {
            if(load_focused_cal_correction("b1r00") == ERROR_ABORT) {
                debug_printf("Is DUT removed from fixture? (y/n) ");
                gets(answer);
                if((answer[0] == 'y') || (answer[0] == 'Y') ) {
                    make_fixture_delay_file(TDRPIN, -1.0e-9, 5.0e-9, "b1r00");
                    if(load_focused_cal_correction("b1r00") == ERROR_ABORT) {
                        goto RE_FOCUSE_CAL;
                    }
                }
            }
            debug_printf("End of focused calibration routine. \n");
        }
        else {
            system("rm /vtsys/rel_current/sys_cor/b1r00.fd1\n");
            make_fixture_delay_file(TDRPIN, -1.0e-9, 5.0e-9, "b1r00");
            if(load_focused_cal_correction("b1r00") == ERROR_ABORT) {
                debug_printf("Is DUT removed from fixture? (y/n) ");
                gets(answer);
                if((answer[0] == 'y') || (answer[0] == 'Y') ) {
                    make_fixture_delay_file(TDRPIN, -1.0e-9, 5.0e-9, "b1r00");
                    if(load_focused_cal_correction("b1r00") == ERROR_ABORT) {
                        goto RE_FOCUSE_CAL;
                    }
                }
            }
            debug_printf("End of focused calibration routine. \n");
        }
    }
#endif FCAL_SKIP

/************************************************************************/
/****                     Load Pattern File                          ****/
/************************************************************************/
                 
    start_addr = 16;          
    debug_printf("\n\nDo You Want To Load Patterns? (Y/N): ");
    gets(answer);

    if((answer[0] == 'y') || (answer[0] == 'Y')) { loadsw = TRUE; }
    else                                         { loadsw = FALSE; }
    debug_printf("\nLoading Pattern ......\n\n" );

    start_addr = PatLoad("contact",           "OSPIN",      start_addr, loadsw);
    start_addr = PatLoad("flash_max",         "FLASHPIN",      start_addr, loadsw);
	start_addr = PatLoad("bist",              "LOADPIN",      start_addr, loadsw);
    start_addr = PatLoad("quartet",           "LOADPIN",      start_addr, loadsw);
	start_addr = PatLoad("sidd",              "LOADPIN",      start_addr, loadsw);
	start_addr = PatLoad("didd",              "LOADPIN",      start_addr, loadsw);

    start_addr = PatLoad("dac_080902",        "LOADPIN",      start_addr, loadsw);
    start_addr = PatLoad("pll",               "LOADPIN",      start_addr, loadsw);
    start_addr = PatLoad("function_415",      "FUNCLOAD",     start_addr, loadsw);


if(loadsw == 1) { 

 sprintf(cmd, "ficm write X funcgrp0  %d", LABEL_START(func_415));  vista_debug_command(cmd);
/*
 sprintf(cmd, "ficm write X H2  %d %d", LABEL_START(func_415), LABEL_START(func_415)+160455  );  vista_debug_command(cmd);
 sprintf(cmd, "ficm write X RG  %d %d", LABEL_START(func_415), LABEL_START(func_415)+160455  );  vista_debug_command(cmd);
 sprintf(cmd, "ficm write X SHD  %d %d", LABEL_START(func_415), LABEL_START(func_415)+160455  );  vista_debug_command(cmd);
*/

 sprintf(cmd, "ficm write X FUNCGRP0  %d %d", LABEL_START(func_415), LABEL_START(func_415)+160455  );  vista_debug_command(cmd);

	vista_debug_command("ficm write X GPIO7_		756253");
	vista_debug_command("ficm write X GPIO7_		756826");
	vista_debug_command("ficm write X GPIO7_		757170");
	vista_debug_command("ficm write X GPIO7_		757743");
	vista_debug_command("ficm write X GPIO7_		758087");
	vista_debug_command("ficm write X GPIO7_		758316");
	vista_debug_command("ficm write X GPIO7_		758660");
	vista_debug_command("ficm write X GPIO7_		759004");
	vista_debug_command("ficm write X GPIO7_		762557");
	vista_debug_command("ficm write X GPIO7_		762786");
	vista_debug_command("ficm write X GPIO7_		765766");
	vista_debug_command("ficm write X GPIO7_		765995");
	vista_debug_command("ficm write X GPIO7_		767600");
	vista_debug_command("ficm write X GPIO7_		768975");
	vista_debug_command("ficm write X GPIO7_		770580");
	vista_debug_command("ficm write X GPIO7_		770809");
	vista_debug_command("ficm write X GPIO7_		774018");
	vista_debug_command("ficm write X GPIO7_		775623");
	vista_debug_command("ficm write X GPIO7_		775852");
	vista_debug_command("ficm write X GPIO7_		778832");
	vista_debug_command("ficm write X GPIO7_		780666");
	vista_debug_command("ficm write X GPIO7_		782041");
	vista_debug_command("ficm write X GPIO7_		783875");
	vista_debug_command("ficm write X GPIO7_		786855");
	vista_debug_command("ficm write X GPIO7_		787084");
	vista_debug_command("ficm write X GPIO7_		1038836");
	vista_debug_command("ficm write X GPIO7_		1039180");
	vista_debug_command("ficm write X GPIO7_		1039409");
	vista_debug_command("ficm write X GPIO7_		1039753");
	vista_debug_command("ficm write X GPIO7_		1040097");
	vista_debug_command("ficm write X GPIO7_		1040326");
	vista_debug_command("ficm write X GPIO7_		1040670");
	vista_debug_command("ficm write X GPIO7_		1041014");
	vista_debug_command("ficm write X GPIO7_		1041243");
	vista_debug_command("ficm write X GPIO7_		1041472");
	vista_debug_command("ficm write X GPIO7_		1041587");
	vista_debug_command("ficm write X GPIO7_		1043879");
	vista_debug_command("ficm write X GPIO7_		1045713");
	vista_debug_command("ficm write X GPIO7_		1048693");
	vista_debug_command("ficm write X GPIO7_		1048922");
	vista_debug_command("ficm write X GPIO7_		1051902");
	vista_debug_command("ficm write X GPIO7_		1053736");
	vista_debug_command("ficm write X GPIO7_		1055111");
	vista_debug_command("ficm write X GPIO7_		1056945");
	vista_debug_command("ficm write X GPIO7_		1058779");
	vista_debug_command("ficm write X GPIO7_		1060154");
	vista_debug_command("ficm write X GPIO7_		1061759");
	vista_debug_command("ficm write X GPIO7_		1062217");
	vista_debug_command("ficm write X GPIO7_		1064968");
	vista_debug_command("ficm write X GPIO7_		1065197");
	vista_debug_command("ficm write X GPIO7_		1066802");
	vista_debug_command("ficm write X GPIO7_		1068177");
	vista_debug_command("ficm write X GPIO7_		1070011");

	vista_debug_command("ficm write X SUB_		1078527");

}

    cal_contin_active_loads(AL_LOAD_FROM_FILE);


#ifdef TD2_OPTION
   if(wave_initialize() == FALSE){
      debug_printf( "Unable to initialize DTS2070\n" );
      exit();
      }
   else gpib_enabled = TRUE;
#endif TD2_OPTION

#ifdef TD3_OPTION
    GPmux_reset(TD_PINS , "TD_PINS");
    if( TD3_initialize() == ERROR_FOUND ) {
        printf("ERROR! Can't initialize the TD3 instrument\n");
        abort();
    }
    TD3_reset();
    TD3_timeout(1);
    TD3_data_init();
#endif TD3_OPTION

    hbiu_operator_msg("READY");
    supply_null_current_offset(PS1); 
    supply_null_current_offset(PS2);  
    supply_null_current_offset(PS3); 
    supply_null_current_offset(PS4);  
    supply_null_current_offset(PS5);  
    supply_null_current_offset(PS6);  
    supply_null_current_offset(PS7);  
	unset_power_supply(PS1,TRUE);
	unset_power_supply(PS2,TRUE);
	unset_power_supply(PS3,TRUE);
	unset_power_supply(PS4,TRUE);
	unset_power_supply(PS5,TRUE);
	unset_power_supply(PS6,TRUE);
	unset_power_supply(PS7,TRUE);

    set_func_stop_mode(ENABLE_EXTENDED_SPF_MEMORY | ENABLE_REPEAT | LT1001_PAT
                | ENABLE_STOP_ON_FUNCTIONAL_BIT | ENABLE_STOP_ON_ERROR,NULL);

    make_spec("logger",          0);
    make_spec("char",            0);
    make_spec("func_char",            0);

    make_spec("dactest",         1);
    make_spec("iil_wait",     0.05);
    make_spec("iih_wait",     0.02);
    make_spec("start_wait",    0.1);
    make_spec("pwr_wait",     0.02);
    make_spec("OS_PMU",          0);
    make_spec("os_repeat",       1);
    make_spec("os_count",       10);

	make_spec("bd_chk",          0);
    make_spec("psh_wait",     0.05);
    make_spec("psh_size",       50);
    make_spec("sidd_wait",    0.10);
    make_spec("didd_wait",    0.10);
    make_spec("sidd_size",     200);
    make_spec("didd_size",     200);

    make_spec("plllog0",         0);
    make_spec("plllog",          0);
    make_spec("pllopt",          0);
    make_spec("pll_wait1",    0.10);
    make_spec("pll_wait2",    0.01);
    make_spec("td2_sample",   1000);
    make_spec("td3_sample",   1000);
    make_spec("pll_vth",      1.00);
    make_spec("pll_vth2",     0.90);
    make_spec("pll_vth3",     0.90);
    make_spec("pll_char",        0);
    make_spec("pchar_step",      0);

    make_spec("log_analog",      0);
    make_spec("dac_wait0",    0.05);
    make_spec("dac_wait1",    0.01);
    make_spec("dac_debug",       0);
    make_spec("dac_dbg_start",   0);
    make_spec("dac_dbg_stop",   64);
    make_spec("dac_char",        0);

    make_spec("dacr_ave",        4);
    make_spec("hfampr",        1.0);
    make_spec("hfoffr",        0.7);
    make_spec("hfcycr",     1.0e-6);
    make_spec("hfskewr",    3.7e-6);
    make_spec("hfcpr",           0);

    make_spec("dacs_ave",       10);
    make_spec("hfamps",        1.0);
    make_spec("hfoffs",        0.7);
    make_spec("hfcycs",    18.5e-9);
    make_spec("hfskews",    9.0e-6);
    make_spec("hfcps",           0);

    make_spec("rampoff",         2);
    make_spec("lfampr",        6.0);
    make_spec("lfoffr",        0.0);

    make_spec("lfamps",        7.0);
    make_spec("lfoffs",        0.0);
    make_spec("lfcycs",  63665.0e-9);
    make_spec("lfskews",  314.68e-6);
    make_spec("lfcps",           0);

    make_spec("flash_time",   35.0e-9);
    make_spec("atpg_time",   100.0e-9);
    make_spec("bist_time",   35.0e-9);
    make_spec("func_time",   80.0e-9);
    make_spec("dac_time",   35.0e-9);
    make_spec("comp_vth",   1.62);

    make_spec("leak_wait",   0.005);

    PS_SITE[0] = 0;
    PS_SITE[1] = 0;
    PS_SITE[2] = 0;
    PS_SITE[3] = 0;
    PS_SITE[4] = 0;
    PS_SITE[5] = 0;
    PS_SITE[6] = 0;



/**********  JJB DEBUG TOOL  *********/
    if(!one_time) { INIT_USER_DEBUG_CMDS(); user_production_display(); one_time = 1; }
/**********  JJB DEBUG TOOL  *********/

/**********  JJB HISTOGRAM  *********/
    INIT_HISTOGRAM_ARRAY();
/**********  JJB HISTOGRAM  *********/

    vista_debug_command("f capt sw");
    vista_debug_command("f log sig pin max 0 ");
    vista_debug_command("log off eot");
    vista_debug_command("over off");
    vista_debug_command("pause off");

/**********  JJB CONTACT  **********/
    make_spec("contact_check",  0);
    vista_debug_command("alias ct_on spec contact_check 1");
    vista_debug_command("alias ct_off spec contact_check 0");
/**********  JJB CONTACT  **********/

    UTIL_POWER();
}

/**********  JJB HISTOGRAM  *********/
void  INIT_HISTO_VALUE()
{
    NUM_DUT = 0;
}
/**********  JJB HISTOGRAM  *********/


void summary() {
char file_dir[100];
char file_name[100];

    jjb_gadong(START_DATE, START_TIME);

	sprintf(file_dir, "%s", "/test3/customer/eyenix/prod_data"); 

    sprintf(buffer,  get_tester_name());
    sprintf(buffer,  get_tester_name());

    sprintf(TESTER_NAME,  "%s", "\0");
    sprintf(LOT_NUMBER,   "%s", "\0");

    sprintf(buffer,  get_tester_name()); sscanf(buffer,  "%s",  TESTER_NAME);
    sprintf(buffer,  get_lot_number());  sscanf(buffer,  "%s",  LOT_NUMBER);

    sprintf(buffer, get_program_name()); sscanf(buffer, "./%s", PGM_NAME);
	sprintf(file_name, "%s_%s_%s_%s_%s_%s", 
                  CODE_NAME, PGM_NAME, LOT_NUMBER, TESTER_NAME, START_DATE, START_TIME);

/**********  JJB HISTOGRAM  *********/
/*
    vista_debug_command("script on /imsidata/hist");
	PRINT_HISTOGRAM_DATA(CODE_NAME); 
    vista_debug_command("script off");
	sprintf(cmd, "mv /imsidata/hist.log  %s/%s.hist\n", file_dir, file_name);
    system(cmd);
*/
	PRINT_BACKUP_DATA("/imsidata", CODE_NAME, START_DATE, START_TIME);
	sprintf(cmd, "mv /imsidata/%s.back  %s/%s.back\n", file_name, file_dir, file_name);
    system(cmd);
/**********  JJB HISTOGRAM  *********/
    vista_debug_command("pdoff");
    vista_debug_command("script on /imsidata/summary");
    jjb_summary(CODE_NAME);
    vista_debug_command("script off");

/*	sprintf(cmd, "lpr -P192_1 /imsidata/summary.log\n");
    system(cmd);  */

	sprintf(cmd, "mv /imsidata/summary.log  %s/%s.sum\n", file_dir, file_name);
    system(cmd);

    debug_printf("\n");
    debug_printf("***********   Do you want the New LOT NUMBER ***********\n");
    debug_printf("If yes, it will clear the Bin Counters & Histogram Array\n");
    debug_printf("Answer? ( Y/N ) ");
    gets(cmd);
    if((cmd[0] == 'y') || (cmd[0] == 'Y')) { INIT_LOT_START(); }
/*
	system("ftpexample");
*/
}

/**********  JJB CONTACT  **********/
void CONTACT_CHECK()
{
    POWER_DOWN();
    contact_check(ALLPIN, &par_allpin);
}
/**********  JJB CONTACT  **********/
 
int OS_TEST()
{
int s,kk;
double  pshco[4], pshio[4], kkk[4];

    test_name("OS");  fbin = 2;

    POWER_DOWN();
    OS_TIME(10.0 US);
    OSN_LEVEL();
    close_io_relays(OSPIN);

    test_name("OSN");
    if(JJB_IFUNCM(OSPIN, LABEL_START(contact), LABEL_STOP(contact), fbin)) { return 1; }
    if( spec("OS_PMU") ){
        set_active_loads(OSPIN, DISABLED );
        set_force_edges(OSPIN,    F0, 0, NOCHANGE, NOCHANGE );
        set_inhibit_edges(OSPIN, FON, 0, NOCHANGE, NOCHANGE );
      
        set_pmu_measure_v(OSPIN, -500.0e-6, -2.5, 3.0, 1.0 MA );
 
		if(JJB_IPMUTEST(par_ospin, -0.9, -0.2, fbin)) { return 1; }
        unset_pmu_mode(OSPIN);

        set_force_edges(OSPIN,   DNRZ, 0, NOCHANGE, NOCHANGE );
        set_inhibit_edges(OSPIN, DNRI, 0, NOCHANGE, NOCHANGE );
    }

if(spec("os_repeat")) {
    OSN_LEVEL(); /* active load mode apply 04.10*/

	for(kk = 0 ; kk<=spec("os_count") ; kk++) {
	if(JJB_IFUNCM(OSPIN, LABEL_START(contact), LABEL_STOP(contact), fbin)) { return 1; }
	}

}

/*
    OSP_LEVEL();
    test_name("OSP");
    if(JJB_IFUNCM(ALLPIN, LABEL_START(contact), LABEL_STOP(contact), fbin)) { return 1; }
    if( spec("OS_PMU") ){
        set_active_loads(ALLPIN, DISABLED );
        set_force_edges(ALLPIN,    F0, 0, NOCHANGE, NOCHANGE );
        set_inhibit_edges(ALLPIN, FON, 0, NOCHANGE, NOCHANGE );
      
        set_pmu_measure_v(ALLPIN,  500.0e-6, -2.5, 3.0, 1.0 MA );
		if(JJB_IPMUTEST(par_allpin,  0.2,  0.9, fbin)) { return 1; }
        unset_pmu_mode(ALLPIN);

        set_force_edges(ALLPIN,   DNRZ, 0, NOCHANGE, NOCHANGE );
        set_inhibit_edges(ALLPIN, DNRI, 0, NOCHANGE, NOCHANGE );
    }
*/
    open_io_relays(OSPIN);
/*
    test_name("PSH");
    vi8_voltage_setup(VDDALL, 0.0, VI8_4V_RANGE, VI8_20UA, 20.0e-6, -20.0e-6);
    vi8_relays       (VDDALL, VI8_CLOSE, VI8_LOCAL_KELVIN_RELAY | VI8_FORCE_RELAY | VI8_SENSE_RELAY );

    vi8_current_setup (VDDIO,    -200.0 UA, VI8_2MA, 2.0,-2.0);
    prim_wait(spec("psh_wait"));
    vi8_measure_voltage(VDDIO, kkk, VI8_4V_RANGE, spec("psh_size"), 20.4US); JJB_DATA_REARRANGE(kkk, pshio);
    vi8_voltage_setup (VDDIO,  0.0, VI8_4V_RANGE, VI8_20UA, 20.0e-6, -20.0e-6);

    vi8_current_setup (VDDCO,    -200.0 UA, VI8_2MA, 2.0,-2.0);
    prim_wait(spec("psh_wait"));
    vi8_measure_voltage(VDDCO, kkk, VI8_4V_RANGE, spec("psh_size"), 20.4US); JJB_DATA_REARRANGE(kkk, pshco);
    vi8_voltage_setup (VDDCO,  0.0, VI8_4V_RANGE, VI8_20UA, 20.0e-6, -20.0e-6);

    if(JJB_IRESULT("PSH_VDDIO", VDDCO, pshio, -0.8, -0.05,  V_UNIT,  7)) { return 1; }
    if(JJB_IRESULT("PSH_VDDCO", VDDCO, pshco, -0.8, -0.05,  V_UNIT,  7)) { return 1; }
*/

    close_io_relays(OSPIN);

    return 0;
}

int IIL_TEST()
{
int s;
double vreg[4];

    test_name("IIL");  fbin = 3;

    VDDIO_VAL = 3.30; VDDCO_VAL = 1.80; VDPLL_VAL = 1.80; VDDAC_VAL = 3.30; VDDT_VAL = 3.30;
    POWER_DUT();

    set_force_levels(ALLPIN,  LEVELS_PER_PIN, 0.0 V, 3.3 V);

    set_force_edges(ALLPIN,    F1, 0, NOCHANGE, NOCHANGE );
    set_inhibit_edges(ALLPIN, FON, 0, NOCHANGE, NOCHANGE );
	open_io_relays(ALLOUT);
	open_io_relays(ALLIO);

    set_pmu_measure_i(IILHPIN, MC10UA, 0.0);
	if(JJB_IPMUTEST(par_iilhpin,  -2.0 UA  - (1.0 UA*FTTEST),  2.0 UA + (1.0 UA *FTTEST), fbin)) { return 1; }
    unset_pmu_mode(IILHPIN);

    open_io_relays(DSL);
    set_pmu_measure_i(DSL, MC10UA, 0.0);
    pmu_measure(DSL, vreg, TRUE);
    MULTI_DATA_REARRANGE(vreg, M_DXX);
    unset_pmu_mode(DSL);
    close_io_relays(DSL);
    GATHERING_HISTOGRAM_DATA(HITEM++, "IILDSL",   "UA",  -2.00 - (1.0*FTTEST),     2.0 + (1.0*FTTEST),   M_DXX,  NUM_DUT, SITE_INFO);

    if(TEST_RESULTS("IILDSL", DSL, M_DXX, -2.0 UA  - (1.0 UA *FTTEST), 2.0 UA + (1.0 UA *FTTEST), A_UNIT, "F_IIL")) { return 1; }

    open_io_relays(TP0);
    set_pmu_measure_i(TP0, MC10UA, 0.0);
    pmu_measure(TP0, vreg, TRUE);
    MULTI_DATA_REARRANGE(vreg, M_DXX);
    unset_pmu_mode(TP0);
    close_io_relays(TP0);
    GATHERING_HISTOGRAM_DATA(HITEM++, "IILTP0",   "UA",  -2.00 - (1.0*FTTEST),     2.0 + (1.0*FTTEST),   M_DXX,  NUM_DUT, SITE_INFO);

    if(TEST_RESULTS("IILTP0", TP0, M_DXX, -2.0 UA - (1.0 UA *FTTEST), 2.0 UA + (1.0 UA *FTTEST), A_UNIT, "F_IIL")) { return 1; }


    open_io_relays(TP1);
    set_pmu_measure_i(TP1, MC10UA, 0.0);
    pmu_measure(TP1, vreg, TRUE);
    MULTI_DATA_REARRANGE(vreg, M_DXX);
    unset_pmu_mode(TP1);
    close_io_relays(TP1);
    GATHERING_HISTOGRAM_DATA(HITEM++, "IILTP1",   "UA",  -2.00 - (1.0*FTTEST),     2.0 + (1.0*FTTEST),   M_DXX,  NUM_DUT, SITE_INFO);
    
    if(TEST_RESULTS("IILTP1", TP1, M_DXX, -2.0 UA  - (1.0 UA *FTTEST), 2.0 UA + (1.0 UA*FTTEST), A_UNIT, "F_IIL")) { return 1; }

	close_io_relays(ALLPIN);
    return 0;
}

int IIH_TEST()
{
int s;
double vreg[4];
    test_name("IIH");  fbin = 4;

    VDDIO_VAL = 3.30; VDDCO_VAL = 1.80; VDPLL_VAL = 1.80; VDDAC_VAL = 3.30; VDDT_VAL = 3.30;
    POWER_DUT();

    set_force_levels(ALLPIN,  LEVELS_PER_PIN, 0.0 V, 3.3 V);

    set_force_edges(ALLPIN,    F0, 0, NOCHANGE, NOCHANGE );
    set_inhibit_edges(ALLPIN, FON, 0, NOCHANGE, NOCHANGE );
	open_io_relays(ALLOUT);
	open_io_relays(ALLIO);

    set_pmu_measure_i(IILHPIN, MC10UA, 3.30 V);
	if(JJB_IPMUTEST(par_iilhpin,  -2.0 UA - (1.0 UA*FTTEST),  2.0 UA + (1.0 UA*FTTEST), fbin)) { return 1; }
    unset_pmu_mode(IILHPIN);

    open_io_relays(DSL);
    set_pmu_measure_i(DSL, MC10UA, 3.3);
    pmu_measure(DSL, vreg, TRUE);
    MULTI_DATA_REARRANGE(vreg, M_DXX);
    unset_pmu_mode(DSL);
    close_io_relays(DSL);
    GATHERING_HISTOGRAM_DATA(HITEM++, "IIHDSL",   "UA",  -2.00 - (1.0*FTTEST),     2.0 + (1.0*FTTEST),   M_DXX,  NUM_DUT, SITE_INFO);

    if(TEST_RESULTS("IIHDSL", DSL, M_DXX, -2.0 UA - (1.0 UA *FTTEST), 2.0 UA + (1.0 UA*FTTEST), A_UNIT, "F_IIL")) { return 1; }

    open_io_relays(TP0);
    set_pmu_measure_i(TP0, MC10UA, 3.3);
    pmu_measure(TP0, vreg, TRUE);
    MULTI_DATA_REARRANGE(vreg, M_DXX);
    unset_pmu_mode(TP0);
    close_io_relays(TP0);
    GATHERING_HISTOGRAM_DATA(HITEM++, "IIHTP0",   "UA",  -2.00 - (1.0*FTTEST),     2.0 + (1.0*FTTEST),   M_DXX,  NUM_DUT, SITE_INFO);

    if(TEST_RESULTS("IIHTP0", TP0, M_DXX, -2.0 UA - (1.0 UA *FTTEST), 2.0 UA + (1.0 UA*FTTEST), A_UNIT, "F_IIL")) { return 1; }


    open_io_relays(TP1);
    set_pmu_measure_i(TP1, MC10UA, 3.3);
    pmu_measure(TP1, vreg, TRUE);
    MULTI_DATA_REARRANGE(vreg, M_DXX);
    unset_pmu_mode(TP1);
    close_io_relays(TP1);
    GATHERING_HISTOGRAM_DATA(HITEM++, "IIHTP1",   "UA",  -2.00 - (1.0*FTTEST),     2.0 + (1.0*FTTEST),   M_DXX,  NUM_DUT, SITE_INFO);
    
    if(TEST_RESULTS("IIHTP1", TP1, M_DXX, -2.0 UA - (1.0 UA *FTTEST), 2.0 UA + (1.0 UA*FTTEST), A_UNIT, "F_IIL")) { return 1; }

	close_io_relays(ALLPIN);
    return 0;
}





int SIDD_TEST()
{
int    i,j, RETEST;
double SIDD1, SIDD2, SIDD3, SIDD4;
double idsio[4],   idsco[4],  idspll[4],  idsdac[4],  idsvddt[4],  kkk[4];

    test_name("SIDD"); fbin = 8;

    VDDIO_VAL = 3.63; VDDCO_VAL = 1.98; VDPLL_VAL = 1.98; VDDAC_VAL = 3.63; VDDT_VAL = 5.50;

    GROSS_LEVEL(0);
	close_io_relays(ALLPIN);

    vi8_voltage_setup(VDDIO,  VDDIO_VAL, VI8_4V_RANGE, VI8_20MA,   20.0e-3,  -20.0e-3);
    vi8_voltage_setup(VDDCO,  VDDCO_VAL, VI8_4V_RANGE, VI8_20MA,   20.0e-3,  -20.0e-3);
    vi8_voltage_setup(VDPLL,  VDPLL_VAL, VI8_4V_RANGE, VI8_20MA,   20.0e-3,  -20.0e-3);
    vi8_voltage_setup(VDDAC,  VDDAC_VAL, VI8_4V_RANGE, VI8_200MA, 200.0e-3, -200.0e-3);




    SIDD_TIME(35.0 NS, 0);
    if(JJB_IFUNCM(LOADPIN, LABEL_START(sidd), LABEL_STOP(sidd), fbin)) { return 1; }
/*
	set_pmu_measure_i(VDDTRLY, MC25MA, VDDT_VAL);
    close_pmu_relays(VDDTRLY);
    open_fixture_relays(VDDTRLY, 0.0); 
*/
    set_active_loads(ALLPIN, DISABLED );
    open_io_relays(ALLOUT);
    open_io_relays(COMP);
    open_io_relays(REXT);
	func_seq(LABEL_START(sidd), LABEL_STOP(sidd));
    prim_wait(spec("sidd_wait"));

    vi8_measure_current(VDDIO, kkk, spec("sidd_size"), 20.4 US); JJB_DATA_REARRANGE(kkk, idsio);
    vi8_measure_current(VDDCO, kkk, spec("sidd_size"), 20.4 US); JJB_DATA_REARRANGE(kkk, idsco);
    vi8_measure_current(VDPLL, kkk, spec("sidd_size"), 20.4 US); JJB_DATA_REARRANGE(kkk, idspll);
    vi8_measure_current(VDDAC, kkk, spec("sidd_size"), 20.4 US); JJB_DATA_REARRANGE(kkk, idsdac);
/*
	pmu_measure(VDDTRLY,  kkk,  FALSE);                          JJB_DATA_REARRANGE(kkk, idsvddt);

    open_pmu_relays(VDDTRLY);
    unset_pmu_mode(VDDTRLY);
    close_fixture_relays(VDDTRLY, 0.0); 
*/
	close_io_relays(ALLPIN);

    GATHERING_HISTOGRAM_DATA(HITEM++, "IDSIO",   "mA",  -0.10,     0.5,   idsio,  NUM_DUT, SITE_INFO);
    GATHERING_HISTOGRAM_DATA(HITEM++, "IDSCO",   "uA", -200.0,   200.0,   idsco,  NUM_DUT, SITE_INFO);
    GATHERING_HISTOGRAM_DATA(HITEM++, "IDSPLL",  "uA", -100.0,   200.0,  idspll,  NUM_DUT, SITE_INFO);
    GATHERING_HISTOGRAM_DATA(HITEM++, "IDSDAC",  "uA", -200.0,   700.0,  idsdac,  NUM_DUT, SITE_INFO);

    if(JJB_IRESULT("IDSIO",    VDDIO,    idsio,   -0.10 MA,    0.5 MA,  A_UNIT, fbin)) { return 1; }
    if(JJB_IRESULT("IDSCO",    VDDCO,    idsco, -200.00 UA,  200.0 UA,  A_UNIT, fbin)) { return 1; }
    if(JJB_IRESULT("IDSPLL",   VDPLL,   idspll,  -100.0 UA,  200.0 UA,  A_UNIT, fbin)) { return 1; }
    if(JJB_IRESULT("IDSDAC",   VDDAC,   idsdac,  -200.0 UA,  700.0 UA,  A_UNIT, fbin)) { return 1; }

    return 0;
}

int DIDD_TEST()
{
int    k,i,j;
double DIDD1, DIDD2, DIDD3, DIDD4;
double iddio[4],   iddco[4],  iddpll[4],  idddac[4],   iddvddt[4],   kkk[4];

    test_name("DIDD"); fbin = 9;

    VDDIO_VAL = 3.63; VDDCO_VAL = 1.98; VDPLL_VAL = 1.98; VDDAC_VAL = 3.63; VDDT_VAL = 3.30;

    GROSS_LEVEL(0);
    DIDD_TIME(35 NS, 0);
/*  if(JJB_IFUNCM(LOADPIN, LABEL_START(ft), LABEL_STOP(ft), fbin)) { return 1; } */

    set_active_loads(ALLPIN, DISABLED );
    open_io_relays(ALLOUT);
    open_io_relays(COMP);
    open_io_relays(REXT);

    set_func_mode(LOOP_ON_FUNCTIONAL);
    func_start(LABEL_START(didd), LABEL_STOP(didd)); 

    prim_wait(spec("didd_wait"));

    vi8_measure_current(VDDIO, kkk, spec("didd_size"), 20.4 US); JJB_DATA_REARRANGE(kkk,  iddio);
    vi8_measure_current(VDDCO, kkk, spec("didd_size"), 20.4 US); JJB_DATA_REARRANGE(kkk,  iddco);
    vi8_measure_current(VDPLL, kkk, spec("didd_size"), 20.4 US); JJB_DATA_REARRANGE(kkk, iddpll);
    vi8_measure_current(VDDAC, kkk, spec("didd_size"), 20.4 US); JJB_DATA_REARRANGE(kkk, idddac);

    set_func_mode(STOP_ON_FIRST);
    func_advance(NULL);
    func_stop();

    close_io_relays(ALLPIN);

    GATHERING_HISTOGRAM_DATA(HITEM++, " IDDIO",  "mA",    5.0,   20.0,  iddio,  NUM_DUT, SITE_INFO);
    GATHERING_HISTOGRAM_DATA(HITEM++, " IDDCO",  "mA",   50.0,  90.0,  iddco,  NUM_DUT, SITE_INFO);
    GATHERING_HISTOGRAM_DATA(HITEM++, "IDDPLL",  "mA",    4.0,    7.0, iddpll, NUM_DUT, SITE_INFO);
    GATHERING_HISTOGRAM_DATA(HITEM++, "IDDDAC",  "mA",    15.0,   25.0, idddac, NUM_DUT, SITE_INFO); 

    if(JJB_IRESULT("IDDIO",  VDDIO,   iddio,    5.0 MA,   20.0 MA,  A_UNIT, fbin)) { return 1; }
    if(JJB_IRESULT("IDDCO",  VDDCO,   iddco,   50.0 MA,  90.0 MA,  A_UNIT, fbin)) { return 1; }
    if(JJB_IRESULT("IDDPLL", VDPLL,  iddpll,    4.0 MA,    7.0 MA,  A_UNIT, fbin)) { return 1; }
    if(JJB_IRESULT("IDDDAC", VDDAC,  idddac,    15.0 MA,    25.0 MA,  A_UNIT, fbin)) { return 1; }

    return 0;
}
int SCAN_TEST()
{
    test_name("GRS");
    GROSS_LEVEL(1);
    open_io_relays(COMP);
    open_io_relays(REXT);

    ATPG_TIME(spec("atpg_time"), 1);  test_name("atpg");   fbin = 10;
    if(JJB_IFUNCM(ATPGPIN, LABEL_START(atpg_0805160), LABEL_STOP(atpg_0805160), fbin)) { return 1; }
#ifdef SKIP
    if(spec("char"))  {
        FREQ_SHMOO ("atpg", 10.0 NS, 50.0 NS, 20, 1.0 V, 3.0 V, 20);
        ATPG_TIME(100.0 NS, 0);
        LEVEL_SHMOO("atpg",  1.0 V,  3.0 V, 20, 0.0 V, 2.0 V, 20 );
    }
#endif SKIP

    return 0;

}

int GRS_TEST()
{
    test_name("GRS");
    GROSS_LEVEL(1);
    open_io_relays(COMP);
    open_io_relays(REXT);
/*
    ATPG_TIME(spec("atpg_time"), 1);  test_name("atpg");   fbin = 10;
    if(JJB_IFUNCM(ATPGPIN, LABEL_START(atpg), LABEL_STOP(atpg), fbin)) { return 1; }
*/
#ifdef SKIP
	if(spec("char"))  { 
        FREQ_SHMOO ("atpg", 10.0 NS, 50.0 NS, 20, 1.0 V, 3.0 V, 20); 
        ATPG_TIME(100.0 NS, 0);
        LEVEL_SHMOO("atpg",  1.0 V,  3.0 V, 20, 0.0 V, 2.0 V, 20 ); 
    }
#endif SKIP

    BIST_TIME(spec("bist_time"), 1);  test_name("bist");   fbin = 11;
    if(JJB_IFUNCM(LOADPIN, LABEL_START(bist), LABEL_STOP(bist), fbin)) { return 1; }
#ifdef SKIP
    if(spec("char"))  {
        FREQ_SHMOO ("bist", 10.0 NS, 50.0 NS, 20, 1.0 V, 3.0 V, 20);
        ATPG_TIME(100.0 NS, 0);
        LEVEL_SHMOO("bist",  1.0 V,  3.0 V, 20, 0.0 V, 2.0 V, 20 );
    }
#endif SKIP


    FUNC_TIME(spec("func_time"));  test_name("func");   fbin = 13;
    if(JJB_IFUNCM(FUNCLOAD, LABEL_START(func_415), LABEL_STOP(func_415), fbin)) { return 1; }

    if(spec("char") || spec("func_char"))  {
        FREQ_SHMOO ("func_415", 10.0 NS, 110.0 NS, 20, 1.0 V, 4.0 V, 20);
        FUNC_TIME(spec("func_time")); 
        VDDIO_VAL = 3.30; VDDCO_VAL = 1.80; VDPLL_VAL = 1.80; VDDAC_VAL = 3.30; VDDT_VAL = 3.30;
        GROSS_LEVEL(1);
		VDD_SHMOO("func_415", "FUNCLOAD", 1.0 , 4.0, 20, 1.0 V, 3.0 V, 20);
        VDDIO_VAL = 3.30; VDDCO_VAL = 1.80; VDPLL_VAL = 1.80; VDDAC_VAL = 3.30; VDDT_VAL = 3.30;
        GROSS_LEVEL(1);
    }

    FLASH_TIME(spec("flash_time"), 1);  test_name("flash");   fbin = 12;
    if(JJB_IFUNCM(FLASHPIN, LABEL_START(flash), LABEL_STOP(flash), fbin)) { return 1; }
#ifdef SKIP
    if(spec("char"))  {
        FREQ_SHMOO ("flash", 10.0 NS, 50.0 NS, 20, 1.0 V, 3.0 V, 20);
        ATPG_TIME(100.0 NS, 0);
        LEVEL_SHMOO("flash",  1.0 V,  3.0 V, 20, 0.0 V, 2.0 V, 20 );
    }
#endif SKIP

    return 0;
}

#ifdef DCP_OPTION

int PLL_CHECK (int pllopt)
{
int    i,  ii, kk, count, RETEST2;
int    status = 0, status1;
double pll_freq[4], rms_jitt[4], max_jitt[4];
double temp_rj[4],  temp_mj[4],  ave_period[4], min_period[4], max_period[4];
double duration,       value_f[4];

for(ii=0; ii<N_SITES; ii++) {
        pll_freq[ii] = 0.0;    pll_freq[ii] = 0.0;
}

    if(pllopt == 0) { test_name("PLL_CHECK");      DIDD_TIME(35.0 NS, 1);      }

	GROSS_LEVEL(1);
	close_io_relays(ALLPIN);
    open_io_relays(COMP);
    open_io_relays(REXT);
    set_active_loads(ALLPIN, PULLDOWN_PER_PIN, -1.0 MA, 2.0,  0.0);

    open_io_relays(ADCK);

    duration = 8000 * 35.0 NS;

	pll_vth0 = pll_vth1 = pll_vth2 = pll_vth3 = spec("pll_vth");

    for(ii=0; ii<N_SITES; ii++) {
        if(active_test_sites() & SITE_N(ii+1)) {


                PARALLEL_CALL_BY_SITE(ADCK, SITE_N(ii+1), close_fixture_relays(ADCK, 0.0)); 

                for(i=0; i<100; i++) {
                    freq_count(DCP_PIN, DCP_DISCONNECT_FROM_PMU, duration, spec("pll_vth")+i*0.020,
                                        "pll", value_f, 2);
                    if(spec("plllog0")) { debug_printf("PLLCHECK%d_%d vth =%5.2fV, freq =%6.2fMHz\n", 
                                          ii+1, kk, spec("pll_vth")+i*0.010, value_f[0]*1.0e-6); }

                    if(ii == 0) { if((value_f[0]>10.00 MHZ) && (value_f[0]<18.00 MHZ)) { 
                                   pll_vth0 = spec("pll_vth")+i*0.010 + 0.0; break;  }
                    } 
                    else if(ii == 1) { if((value_f[0]>26.00 MHZ) && (value_f[0]<30.00 MHZ)) { 
                                   pll_vth1 = spec("pll_vth")+i*0.010 + 0.0; break;  }
                    } 
                    else  if(ii ==2) { if((value_f[0]>26.00 MHZ) && (value_f[0]<30.00 MHZ)) { 
                                   pll_vth2 = spec("pll_vth")+i*0.010 + 0.0; break;  }
                    } 
                    else  if(ii ==3) { if((value_f[0]>26.00 MHZ) && (value_f[0]<30.00 MHZ)) { 
                                   pll_vth3 = spec("pll_vth")+i*0.010 + 0.0; break;  }
                    } 
                }
                pll_freq[ii] = value_f[0];

                PARALLEL_CALL_BY_SITE(ADCK, SITE_N(ii+1), open_fixture_relays(ADCK, 0.0)); 

        }
    }
    close_io_relays(ADCK);

    pll_check_done = 1;
    return 0;
}

int PLL_TEST(int pllopt)
{
int    i,  ii, kk, count, RETEST2;
int    status = 0, status1;
double pll_freq[4], rms_jitt[4], max_jitt[4];
double temp_rj[4],  temp_mj[4],  ave_period[4], min_period[4], max_period[4];
double duration,       value_f[4];

for(ii=0; ii<N_SITES; ii++) {
        pll_freq[ii] = 0.0;    pll_freq[ii] = 0.0;
}

    if(pllopt == 0) { test_name("PLL");      DIDD_TIME(35.0 NS, 1);      }

	GROSS_LEVEL(1);
	close_io_relays(ALLPIN);
    open_io_relays(COMP);
    open_io_relays(REXT);
    set_active_loads(ALLPIN, PULLDOWN_PER_PIN, -1.0 MA, 2.0,  0.0);

    open_io_relays(ADCK);

    test_name("PLL_TEST");  fbin = 35;
    duration = 8000 * 35.0 NS;


    for(ii=0; ii<N_SITES; ii++) {
        if(active_test_sites() & SITE_N(ii+1)) {


                PARALLEL_CALL_BY_SITE(ADCK, SITE_N(ii+1), close_fixture_relays(ADCK, 0.0)); 

                for(i=0; i<20; i++) {
                    if(ii==0) { freq_count(DCP_PIN, DCP_DISCONNECT_FROM_PMU, duration, pll_vth0+i*0.002, "pll", value_f, 2);  pll_vth = pll_vth0+i*0.002; }
                    if(ii==1) { freq_count(DCP_PIN, DCP_DISCONNECT_FROM_PMU, duration, pll_vth1+i*0.002, "pll", value_f, 2);  pll_vth = pll_vth1+i*0.002; }
                    if(ii==2) { freq_count(DCP_PIN, DCP_DISCONNECT_FROM_PMU, duration, pll_vth2+i*0.002, "pll", value_f, 2);  pll_vth = pll_vth2+i*0.002; }
                    if(ii==3) { freq_count(DCP_PIN, DCP_DISCONNECT_FROM_PMU, duration, pll_vth3+i*0.002, "pll", value_f, 2);  pll_vth = pll_vth3+i*0.002; }

                    if(spec("plllog")) { debug_printf("SITE%d_%d vth =%5.3fV, freq =%6.2fMHz\n", 
                                         ii+1, kk, pll_vth, value_f[0]*1.0e-6); }
                    if((value_f[0]>10.00 MHZ) && (value_f[0]<18.00 MHZ)) break;
                }
                pll_freq[ii] = value_f[0];

                PARALLEL_CALL_BY_SITE(ADCK, SITE_N(ii+1), open_fixture_relays(ADCK, 0.0)); 

        }
    }
    close_io_relays(ADCK);

if(pllopt == 0) {
    GATHERING_HISTOGRAM_DATA(HITEM++, "FREQ00",   "MHz",   10.00,  18.00, pll_freq, NUM_DUT, SITE_INFO);

    if(JJB_IRESULT("ADCK",   ADCK, pll_freq,   10.00 MHz,   18.00 MHz,  HZ_UNIT, fbin)) { return 1; }
}

    return 0;
}
#endif DCP_OPTION

#ifdef TD2_OPTION
int PLL_TEST2(int pllopt)
{
int    ii, kk, count, RETEST2;
int    status = 0, status1;
double pll_freq[4], rms_jitt[4], max_jitt[4];
double temp_rj[4],  temp_mj[4],  ave_period[4], min_period[4], max_period[4];


for(ii=0; ii<N_SITES; ii++) {
        pll_freq[ii] = 0.0;    pll_freq[ii] = 0.0;
        rms_jitt[ii] = -1.0e6; rms_jitt[ii] = -1.0e6;
        max_jitt[ii] = -1.0e6; max_jitt[ii] = -1.0e6;
}

    if(pllopt == 0) { test_name("PLL");      DIDD_TIME(35.0 NS, 1);      }

	GROSS_LEVEL(1);

    open_io_relays(ADCK);

    test_name("PLL_TEST");  fbin = 35;

    wave_setup_sample_size(spec("td2_sample"));
    wave_setup_arm(AUTO_ARM, STOP_SEQ, NO_ARM, NO_ARM, 0.0, 0.0, POS, POS, 1, 2);
/*  wave_setup_period(CHANNEL_1, NO_PERCENT, spec("pll_vth2"), spec("pll_vth2")); */

    for(ii=0; ii<N_SITES; ii++) {
        if(active_test_sites() & SITE_N(ii+1)) {

                PARALLEL_CALL_BY_SITE(ADCK, SITE_N(ii+1), close_fixture_relays(ADCK, 0.0)); 

                connect_matrix(TD_PINS, 1, 0, 0, 0);

                if(pllopt == 0) { func_start(LABEL_START(pll),     LABEL_STOP(pll)); }

                prim_wait(spec("pll_wait1"));
RETEST2 = 0;
RE_MEASURE2:
                if( RETEST2 > 5 ) { goto EXIT2; }

                wave_setup_period(CHANNEL_1, NO_PERCENT, spec("pll_vth2")+0.01*RETEST2, spec("pll_vth2")+0.01*RETEST2);
                wave = wave_measure_all();
                prim_wait(spec("pll_wait2")); 

                ave_period[ii] = (double)(wave -> measure1);
                temp_rj[ii]    = (double)(wave -> measure2);
                min_period[ii] = (double)(wave -> measure3);
                max_period[ii] = (double)(wave -> measure4);
    
                temp_mj[ii] = ((max_period[ii]*1.0e+12)-(min_period[ii]*1.0e+12))*1.0e-12;
    
                pll_freq[ii] = 1/ave_period[ii];
        
           	    rms_jitt[ii] = temp_rj[ii];
                max_jitt[ii] = temp_mj[ii];
    
                func_advance(NULL);
                func_stop();
        


if(pllopt == 0) {
    if( (pll_freq[ii] < 11.7 MHz) || (pll_freq[ii] > 18.3 MHz) ) { RETEST2++; goto RE_MEASURE2; }
}
if(pllopt == 1) {
    if( (pll_freq[ii] < 12.3 MHz) || (pll_freq[ii] > 17.3 MHz) ) { RETEST2++; goto RE_MEASURE2; }
}
if(pllopt == 2) {
    if( (pll_freq[ii] < 13.3 MHz) || (pll_freq[ii] > 16.3 MHz) ) { RETEST2++; goto RE_MEASURE2; }
}

EXIT2:


                connect_matrix(TD_PINS, 0, 0, 0, 0);
                PARALLEL_CALL_BY_SITE(ADCK, SITE_N(ii+1), open_fixture_relays(ADCK, 0.0)); 

        }
    }
    close_io_relays(ADCK);

    GATHERING_HISTOGRAM_DATA(HITEM++, "FREQ00",   "MHz",   10.00,  18.00, pll_freq, NUM_DUT, SITE_INFO);
    GATHERING_HISTOGRAM_DATA(HITEM++, "JITTER00",  "pS", -300.00, 300.00, rms_jitt, NUM_DUT, SITE_INFO);

    if(JJB_IRESULT("FREQ00",   ADCK, pll_freq,   10.00 MHz,   18.00 MHz,  HZ_UNIT, fbin)) { return 1; }
    if(JJB_IRESULT("JITTER00", ADCK, rms_jitt, -300.00  pS,  300.00  pS,   S_UNIT, fbin)) { return 1; }

    return 0;
}
#endif TD2_OPTION

#ifdef TD3_OPTION
void PLL_CHAR3(int SITEN)
{
int          s,          size,       ii,         kk;
double       vpd_st,     vpd_sp,     vpd_dt;
double       vpa_st,     vpa_sp,     vpa_dt;
double       idspa[2],   idspd[2],   iddpa[2],   iddpd[2],  kkk[2];
double       ave_period, min_period, max_period;
double       temp_rj,    temp_mj,    pll_freq,   rms_jitt,  max_jitt;

unsigned int over_flg;
unsigned int data_flg;
unsigned int pause_flg;

    data_flg = datalog_flag(DEFAULT_MODE, NULL); datalog_flag(LOG_OFF, NULL);
    over_flg = override_flag(DEFAULT_MODE, NULL); override_flag(0x000000, NULL);
    pause_flg = pause_flag(DEFAULT_MODE, NULL); pause_flag(0x000000, NULL);

    vpd_st = 1.0;  vpd_sp = 1.4;
    vpa_st = 1.0;  vpa_sp = 1.4;
    vpd_dt = ( vpd_sp - vpd_st ) / spec("pchar_step");
    vpa_dt = ( vpa_sp - vpa_st ) / spec("pchar_step");

    if(!spec("pchar_step")) { vpd_st = 1.2; vpa_st = 1.2; vpd_dt = 0.0; vpa_dt = 0.0; }

        connect_GPmux(TD_PINS, OPEN, OPEN, PIN1, OPEN);
        connect_GPmux_input(TD_PINS, NORM, PIN1, NULL, NULL);

    test_name("PLL_CHAR");

    debug_printf("\nPattern : %s\n", "pll");
    debug_printf(" VPLLD, VPLLA : %9s, %9s, %9s, %9s, %8s , %11s, %11s, %8s , %11s, %11s\n", 
                 "IDSPD", "IDSPA", "IDDPD", "IDDPA", "FREQ1", "Jitter1_rms", "Jitter1_max", "FREQ2", "Jitter2_rms", "Jitter2_max");

    for(ii=0; ii<=spec("pchar_step"); ii++) {
        for(kk=0; kk<=spec("pchar_step"); kk++) {
            TD3_data_init();
            pll_freq = 0.0;
            rms_jitt = 999.0;
            max_jitt = 999.0;
        
            VDPLL_VAL = vpd_st+vpd_dt*ii; 
            GROSS_LEVEL(0);
            vi8_voltage_setup (VDPLL,   vpd_st+vpd_dt*ii, VI8_4V_RANGE, VI8_2MA, 2.0e-3, -2.0e-3);
            set_active_loads(ALLPIN, DISABLED );
            open_io_relays(ALLOUT);

            debug_printf("%5.2fV,%5.2fV : ", vpd_st+vpd_dt*ii, vpa_st+vpa_dt*kk);
        
            func_start(LABEL_START(pll), LABEL_STOP(pll));
            func_advance(NULL);
            prim_wait(spec("sidd_wait"));

            vi8_measure_current(VDPLL,  kkk, spec("sidd_size"), 20.4 US); JJB_DATA_REARRANGE(kkk, idspd);

            PARALLEL_EXECUTE(debug_printf("%8.3fuA,", idspd[parallel_idx]*1.0e6));

            vi8_voltage_setup (VDPLL,   vpd_st+vpd_dt*ii, VI8_4V_RANGE, VI8_20MA, 20.0e-3, -20.0e-3);

            func_start(LABEL_START(pll), LABEL_STOP(pll));
            prim_wait(spec("didd_wait"));
        
            vi8_measure_current(VDPLL,  kkk, spec("sidd_size"), 20.4 US); JJB_DATA_REARRANGE(kkk, iddpd);

            PARALLEL_EXECUTE(debug_printf("%8.3fmA,", iddpd[parallel_idx]*1.0e3));

            func_advance(NULL);
            func_stop();

            VDPLL_VAL = vpd_st+vpd_dt*ii;
            GROSS_LEVEL(0);
            close_io_relays(ALLOUT);
            open_io_relays(ADCK);
            close_fixture_relays(ADCK, 0.0);

            func_start(LABEL_START(pll), LABEL_STOP(pll));
            prim_wait(spec("pll_wait1"));

            TD3_setup_period(CHANNEL_1, NO_PERCENT,spec("pll_vth3"), spec("pll_vth3"));
            TD3_measure_all(CHANNEL_1);
            prim_wait(spec("pll_wait2"));

            ave_period = TD3_data.measure1;
            temp_rj    = TD3_data.measure2;
            min_period = TD3_data.measure3;
            max_period = TD3_data.measure4;

            temp_mj = ((max_period*1.0e+12)-(min_period*1.0e+12))*1.0e-12;

            pll_freq = 1/ave_period;
            rms_jitt = temp_rj;
            max_jitt = temp_mj;

            PARALLEL_EXECUTE(debug_printf("%7.3fMHz,", pll_freq*1.0e-6));
            PARALLEL_EXECUTE(debug_printf("%9.3fpS,",  rms_jitt*1.0e12));
            PARALLEL_EXECUTE(debug_printf("%10.3fpS,", max_jitt*1.0e12));

            TD3_setup_period(CHANNEL_2, NO_PERCENT,spec("pll_vth3"), spec("pll_vth3"));
            TD3_measure_all(CHANNEL_2);
            
            ave_period = TD3_data.measure1;
            temp_rj    = TD3_data.measure2;
            min_period = TD3_data.measure3;
            max_period = TD3_data.measure4;

            temp_mj = ((max_period*1.0e+12)-(min_period*1.0e+12))*1.0e-12;

            pll_freq = 1/ave_period;
            rms_jitt = temp_rj;
            max_jitt = temp_mj;

            PARALLEL_EXECUTE(debug_printf("%8.3fMHz,", pll_freq*1.0e-6));
            PARALLEL_EXECUTE(debug_printf("%9.3fpS,",  rms_jitt*1.0e12));
            PARALLEL_EXECUTE(debug_printf("%10.3fpS", max_jitt*1.0e12));

            debug_printf("\n");
            close_io_relays(ADCK);
            open_fixture_relays(ADCK, 0.0);
        }
    }
    connect_GPmux(TD_PINS, OPEN, OPEN, OPEN, OPEN);

    datalog_flag(data_flg, NULL);
    override_flag(over_flg, NULL);
    pause_flag(pause_flg, NULL);
}

int PLL_TEST3(int pllopt)
{
int    ii, kk, count, RETEST3;
int    status = 0, status1;
double pll_freq[4], rms_jitt[4], max_jitt[4];
double temp_rj[4],  temp_mj[4],  ave_period[4], min_period[4], max_period[4];

for(ii=0; ii<N_SITES; ii++) {
        pll_freq[ii] = 0.0;    pll_freq[ii] = 0.0;
        rms_jitt[ii] = -1.0e6; rms_jitt[ii] = -1.0e6;
        max_jitt[ii] = -1.0e6; max_jitt[ii] = -1.0e6;
}

    if(pllopt == 0) { test_name("PLL");      DIDD_TIME(35.0 NS, 1);      }

	GROSS_LEVEL(1);

    open_io_relays(ADCK);

    test_name("PLL_TEST");  fbin = 35;

    GPmux_buffer(TD_PINS, spec("pll_vth3"), spec("pll_vth3"), TERM, TERM);

    TD3_sample_size(spec("td3_sample"));
    TD3_setup_arm(AUTO_ARM, STOP_SEQ, NO_ARM, NO_ARM, 0.0, 0.0, POS, POS, 1, 2);

    for(ii=0; ii<N_SITES; ii++) {
        if(active_test_sites() & SITE_N(ii+1)) {

            connect_GPmux(TD_PINS, OPEN, OPEN, PIN1, OPEN);
            connect_GPmux_input(TD_PINS, NORM, PIN1, NULL, NULL); 


                if(pllopt == 0) { func_start(LABEL_START(pll),     LABEL_STOP(pll)); }

                PARALLEL_CALL_BY_SITE(ADCK, SITE_N(ii+1), close_fixture_relays(ADCK, 0.0));
                prim_wait(spec("pll_wait1"));
RETEST3 = 0;
RE_MEASURE3:
                test_name("pllmeas");
        
                if( RETEST3 > 5 ) { goto EXIT3; }

                TD3_data_init();
    
                TD3_setup_period(CHANNEL_1, NO_PERCENT,spec("pll_vth3")+0.01*RETEST3, spec("pll_vth3")+0.01*RETEST3);
                TD3_measure_all(CHANNEL_1);
                prim_wait(spec("pll_wait2"));
    
                ave_period[ii] = TD3_data.measure1;
                temp_rj[ii]    = TD3_data.measure2;
                min_period[ii] = TD3_data.measure3;
                max_period[ii] = TD3_data.measure4;
    
                temp_mj[ii] = ((max_period[ii]*1.0e+12)-(min_period[ii]*1.0e+12))*1.0e-12;
    
                pll_freq[ii] = 1/ave_period[ii];
        
           	    rms_jitt[ii] = temp_rj[ii];
                max_jitt[ii] = temp_mj[ii];
    
                func_advance(NULL);
                func_stop();
        
                if(spec("plllog")) { 
                    debug_printf("SITE%d_%d, Vth = %5.3fV, freq=%8.3fMHz, jitter=%8.3fPS,%8.3fPS\n", 
                                  ii, kk, spec("pll_vth3")+0.01*RETEST3,
                                  pll_freq[ii]*1.0e-6, rms_jitt[ii]*1.0e12, max_jitt[ii]*1.0e12); 
                }
                if( (pll_freq[ii] < 10.7 MHz) || (pll_freq[ii] > 18.3 MHz) ) { RETEST3++; goto RE_MEASURE3; }
if(pllopt == 0) { 
    if( (pll_freq[ii] < 11.7 MHz) || (pll_freq[ii] > 18.3 MHz) ) { RETEST3++; goto RE_MEASURE3; }
}
if(pllopt == 1) { 
    if( (pll_freq[ii] < 12.3 MHz) || (pll_freq[ii] > 17.3 MHz) ) { RETEST3++; goto RE_MEASURE3; }
}
if(pllopt == 2) { 
    if( (pll_freq[ii] < 13.3 MHz) || (pll_freq[ii] > 16.3 MHz) ) { RETEST3++; goto RE_MEASURE3; }
}

EXIT3:

                PARALLEL_CALL_BY_SITE(ADCK, SITE_N(ii+1), open_fixture_relays(ADCK, 0.0)); 
            connect_GPmux(TD_PINS, OPEN, OPEN, OPEN, OPEN);

		    if(spec("pll_char")) { PLL_CHAR3(ii); }

        }
    }
    close_io_relays(ADCK);

    GATHERING_HISTOGRAM_DATA(HITEM++, "FREQ00",   "MHz",   10.00,  18.00, pll_freq, NUM_DUT, SITE_INFO);
    GATHERING_HISTOGRAM_DATA(HITEM++, "JITTER00",  "pS", -300.00, 300.00, rms_jitt, NUM_DUT, SITE_INFO);

    if(JJB_IRESULT("FREQ00",   ADCK, pll_freq,   10.00 MHz,   18.00 MHz,  HZ_UNIT, fbin)) { return 1; }
    if(JJB_IRESULT("JITTER00", ADCK, rms_jitt, -300.00  pS,  300.00  pS,   S_UNIT, fbin)) { return 1; }

    return 0;
}
#endif TD3_OPTION

#ifdef HF_TEST
void DAC_Linearity_HF(PINLIST *plist, double acp_rng, double acp_off_v, int num_samples, int cp_offset, int ave, int num_bits)
{
double lsb;
double dnl_minval[2], dnl_maxval[2], inl_minval[2], inl_maxval[2];
double MAX_DNL[2], MAX_INL[2];
int    min_loc[2], max_loc[2];
int i, num_codes;
double fs[2],zero[2],Vlsb[2],fs_level[2],zero_level[2];

WaveHdl_t  ramp_wf_unav[2];
WaveHdl_t  ramp_wf_av[2];
WaveHdl_t  inl_wf[2];
WaveHdl_t  dnl_wf[2];
WaveHdl_t  dnl_temp1_wf[2];
WaveHdl_t  dnl_temp2_wf[2];
WaveHdl_t  imsi_wf[2];
 
/* num_codes = (int) power(2.0, (double) num_bits); */
    num_codes = 1024;
    lsb = 2.0*acp_rng/4095;
    lsb = lsb*1.49; /* cal factor 50ohm/75ohm */
 
    load_pinlist(plist, UNUSED);

    check_dsp(dsp_free_waveforms (2, ramp_wf_unav[0], ramp_wf_unav[1]));
 	check_dsp(dsp_free_waveforms (2, ramp_wf_av[0], ramp_wf_av[1]));
	check_dsp(dsp_free_waveforms (2, dnl_wf[0], dnl_wf[1]));
 	check_dsp(dsp_free_waveforms (2, inl_wf[0], inl_wf[1]));
 	check_dsp(dsp_free_waveforms (2, dnl_temp1_wf[0], dnl_temp1_wf[1]));
 	check_dsp(dsp_free_waveforms (2, dnl_temp2_wf[0], dnl_temp2_wf[1]));
	check_dsp(dsp_free_waveforms (2, imsi_wf[0], imsi_wf[1]));

    check_dsp(dsp_deinterleave_cp_to_wf(plist, 0, num_samples/num_active_sites(), num_active_sites(), 
                                                RRECT, "ramp_wf_unav", &ramp_wf_unav[0] ));
 
    for (i = 0; i < num_active_sites(); i++) {
        check_dsp(dsp_create_wf(plist, num_samples/(ave*num_active_sites()), 
                                       RRECT, "ramp_wf_av", &ramp_wf_av[i]));
        check_dsp(dsp_create_wf(plist, num_codes-2, RRECT, "dnl_wf", &dnl_wf[i]));
        check_dsp(dsp_create_wf(plist, num_codes-2, RRECT, "inl_wf", &inl_wf[i]));

	    check_dsp(dsp_average_wf(plist,ramp_wf_unav[i],ave,ramp_wf_av[i]));
       	check_dsp(dsp_wc_sub (plist, ramp_wf_av[i], 2048, ramp_wf_av[i]));
        check_dsp(dsp_zone_wf(plist, ramp_wf_av[i], 0, num_codes-1, "imsi_wf", &imsi_wf[i]));
        check_dsp(dsp_min_max_meas(plist, imsi_wf[i], VALUE, &zero[i], &fs[i], &min_loc[i], &max_loc[i]));
        	
        zero_level[i]=zero[i]*lsb;
	    fs_level[i]=fs[i]*lsb;
	    Vlsb[i]= (fs_level[i]-zero_level[i])/(num_codes - 1.0);

	    if(spec("log_analog")) { 
    		printf("Actual LSB\n");
    		printf("zero_level[%d] = %9.6f, fs_level[%d] = %9.6f, lsb = %9.6f, Vlsb[i] = %9.6f\n",
    			              i, zero[i]*lsb, i, fs[i]*lsb, lsb, Vlsb[i]); 
	    }
	
	    check_dsp(dsp_wc_mult(plist, imsi_wf[i], lsb, imsi_wf[i]));         

        /* DNL Calculating */
        check_dsp(dsp_zone_wf(plist, imsi_wf[i], 2, num_codes-1, "dnl_temp1_wf", &dnl_temp1_wf[i]));
        check_dsp(dsp_zone_wf(plist, imsi_wf[i], 1, num_codes-2, "dnl_temp2_wf", &dnl_temp2_wf[i]));
        check_dsp(dsp_wf_sub(plist, dnl_temp1_wf[i], dnl_temp2_wf[i], dnl_wf[i]));
        check_dsp(dsp_wc_add(plist, dnl_wf[i], -Vlsb[i], dnl_wf[i]));
	    check_dsp(dsp_wc_mult(plist, dnl_wf[i], 1/Vlsb[i], dnl_wf[i]));
	
	    check_dsp(dsp_min_max_meas(plist, dnl_wf[i], VALUE, &dnl_minval[i], &dnl_maxval[i], NULL, NULL));
        MAX_DNL[i] = fabs(dnl_maxval[i])>fabs(dnl_minval[i])?(dnl_maxval[i]):(dnl_minval[i]);
  
        /* Convert to INL by integration */
	    check_dsp(dsp_wf_integr_t (plist, dnl_wf[i], inl_wf[i]));
        
	    check_dsp(dsp_min_max_meas(plist, inl_wf[i], VALUE, &inl_minval[i], &inl_maxval[i], NULL, NULL));
        MAX_INL[i] = fabs(inl_maxval[i])>fabs(inl_minval[i])?(inl_maxval[i]):(inl_minval[i]);

 	    if(spec("log_analog")) { printf("DNL[%d] = %9.6f, INL[%d] = %9.6f\n", i, i, MAX_DNL[i], MAX_INL[i]); }
    }
	JJB_DATA_REARRANGE(MAX_DNL, dac_DLE);
	JJB_DATA_REARRANGE(MAX_INL, dac_ILE);
	
  	check_dsp(dsp_free_waveforms (2, dnl_wf[0], dnl_wf[1])); 	     
 	check_dsp(dsp_free_waveforms (2, inl_wf[0], inl_wf[1]));  	     
 	check_dsp(dsp_free_waveforms (2, dnl_temp1_wf[0], dnl_temp1_wf[1])); 
 	check_dsp(dsp_free_waveforms (2, dnl_temp2_wf[0], dnl_temp2_wf[1])); 
	check_dsp(dsp_free_waveforms (2, imsi_wf[0], imsi_wf[1]));	     
	check_dsp(dsp_free_waveforms (2, ramp_wf_av[0], ramp_wf_av[1]));     
	check_dsp(dsp_free_waveforms (2, ramp_wf_unav[0], ramp_wf_unav[1])); 
}

void SNDR_SFDR_MEASURE_HF(PINLIST *plist, double acp_rng, double acp_off_v, int num_samples, int cp_offset, int ave, int num_bits)
{
double     lsb;
int        i, num_codes;
double     SFDR[2], SNDR[2], THD[2], SNR[2],sfdr_max[2], mag[2];
WaveHdl_t  sfdr_crect[2];
WaveHdl_t  sfdr_polar[2];
WaveHdl_t  sine_wf_unav[2];
WaveHdl_t  sine_wf_av[2];
 
 /* num_codes = (int) power(2.0, (double) num_bits); */
    num_codes = 1024;
    lsb = 2.0*acp_rng/4095;
    lsb = lsb*1.49; /* cal factor 50ohm/75ohm */
 
    load_pinlist(plist, UNUSED);

    check_dsp(dsp_free_waveforms (2, sine_wf_unav[0], sine_wf_unav[1]));
    check_dsp(dsp_free_waveforms (2, sine_wf_av[0], sine_wf_av[1]));
    check_dsp(dsp_free_waveforms (2, sfdr_crect[0], sfdr_crect[1]));
    check_dsp(dsp_free_waveforms (2, sfdr_polar[0], sfdr_polar[1]));

    check_dsp(dsp_deinterleave_cp_to_wf(plist, 0, num_samples*ave/num_active_sites(),
    num_active_sites(), RRECT, "sine_wf_unav", &sine_wf_unav[0] ));    
  
    for (i = 0; i < num_active_sites(); i++) {
        check_dsp(dsp_create_wf(plist, num_samples/num_active_sites(), RRECT, "sine_wf_av", &sine_wf_av[i]));
 
	    check_dsp(dsp_average_wf(plist,sine_wf_unav[i],ave,sine_wf_av[i]));
       	check_dsp(dsp_wc_sub (plist, sine_wf_av[i], 2048, sine_wf_av[i]));
        check_dsp(dsp_wc_mult(plist, sine_wf_av[i], lsb, sine_wf_av[i]));
        
	    check_dsp(dsp_ratios_meas(plist,
                               sine_wf_av[i],
                               5,
                               NULL,
                               &SNR[i],
                               &THD[i],
                               &SNDR[i],
                               NULL,
                               NULL));

     	test_name("SFDR");
        check_dsp(dsp_create_wf(plist, 513, CRECT, "sfdr_crect", &sfdr_crect[i]));
        check_dsp(dsp_create_wf(plist, 513, POLAR, "sfdr_polar", &sfdr_polar[i]));
        check_dsp(dsp_fft(plist, sine_wf_av[i], sfdr_crect[i]));

        check_dsp(dsp_polar(plist, sfdr_crect[i], sfdr_polar[i]));
        check_dsp(dsp_read_num(plist, sfdr_polar[i], MAG, 19, &mag[i]));
        check_dsp(dsp_write_num(plist, 0.0, MAG, 19, sfdr_polar[i]));
        check_dsp(dsp_write_num(plist, 0.0, MAG,  0, sfdr_polar[i]));
        check_dsp(dsp_min_max_meas(plist,sfdr_polar[i],MAG,NULL,&sfdr_max[i],NULL,NULL));
          
        SFDR[i] = 20*log10(mag[i] / sfdr_max[i]);

	    if(spec("log_analog")) { printf("SNR[%d]  = %9.6f\n", i, SNR[i]); } 
	    if(spec("log_analog")) { printf("THD[%d]  = %9.6f\n", i, THD[i]); } 
	    if(spec("log_analog")) { printf("SNDR[%d] = %9.6f\n", i, SNDR[i]); } 
	    if(spec("log_analog")) { printf("SFDR[%d] = %9.6f\n", i, SFDR[i]); } 
    }							      
	
	JJB_DATA_REARRANGE(SNR,  dac_SNR); 	
	JJB_DATA_REARRANGE(THD,  dac_THD); 
	JJB_DATA_REARRANGE(SNDR, dac_SNDR); 
	JJB_DATA_REARRANGE(SFDR, dac_SFDR); 

	check_dsp(dsp_free_waveforms (2, sfdr_polar[0], sfdr_polar[1]));
	check_dsp(dsp_free_waveforms (2, sfdr_crect[0], sfdr_crect[1]));
    check_dsp(dsp_free_waveforms (2, sine_wf_av[0], sine_wf_av[1]));     
	check_dsp(dsp_free_waveforms (2, sine_wf_unav[0], sine_wf_unav[1])); 
}

int HF_DAC_TEST()
{
int fbin, i, j, retest11, retest12;
int dac_repeat, mmm, ii, jj, iii;
int d00, d01, d02, d03, d04, d05, d06, d07, d08, d09, d10, d11;
int k00, k01, k02, k03, k04, k05, k06, k07, k08, k09, k10, k11;
char KK0[4], KK1[4], KK2[4];

double vdd_step, vdd_start, vdd_stop, vdd_delta;
double kkk[4];
double dac_IDD33[4],dac_IDD25[4], dac_IDDA[4],  dac_IREF[4],  idd_result,   dac_VALUE[4];
double mxx_DLE[10], mxx_ILE[10], mxx_THD[10], mxx_SNDR[10], mxx_SFDR[10], mxx_IDD[10];

    dac_DLE[0]=0.0;   dac_DLE[1]=0.0;   dac_DLE[2]=0.0;   dac_DLE[3]=0.0;
    dac_ILE[0]=0.0;   dac_ILE[1]=0.0;   dac_ILE[2]=0.0;   dac_ILE[3]=0.0;
    dac_THD[0]=0.0;   dac_THD[1]=0.0;   dac_THD[2]=0.0;   dac_THD[3]=0.0;
    dac_SNR[0]=0.0;   dac_SNR[1]=0.0;   dac_SNR[2]=0.0;   dac_SNR[3]=0.0;
    dac_SNDR[0]=0.0;  dac_SNDR[1]=0.0;  dac_SNDR[2]=0.0;  dac_SNDR[3]=0.0;
    dac_SFDR[0]=0.0;  dac_SFDR[1]=0.0;  dac_SFDR[2]=0.0;  dac_SFDR[3]=0.0;

    test_name("dac_test"); fbin = 31;

    close_io_relays(ALLPIN);

if(spec("dac_char")) { vdd_step = 11; vdd_start = 2.8; vdd_stop = 3.8; vdd_delta = 0.10; }
else                 { vdd_step = 1;  vdd_start = 3.3; vdd_stop = 3.3; vdd_delta = 0.00; }

    for(ii=1; ii<=vdd_step; ii++) {
    
        VDACI_VAL = vdd_start+vdd_delta*ii;
    	GROSS_LEVEL(0); 
    
        test_name("dac_tri");
    
        close_io_relays(HFACP);
    
        close_fixture_relays(DACOUT, 0.0);
    
        test_name("dac_tri");
        DAC_TIME(spec("hfcycr"), 0);
    	func_seq(LABEL_START(dac), LABEL_STOP(dac));
        prim_wait(spec("dac_wait0"));

        setup_acp(HFACP,
                  spec("hfcycr"),
                  spec("hfskewr"),
                  spec("hfoffr"),
                  spec("hfampr"),
                  ACP_20MHZ_BW,
                  TRIG_VECTOR1,
                  TRIG_START);
    
        setup_cp(element(HFACP[0]), spec("hfcpr")+2048*spec("dacr_ave")*num_active_sites(), 0, 
                                    TRIG_PROCESSOR_WRITE, TRIG_SAMPLE_COUNTER);
    
        prim_wait(spec("dac_wait1"));
        func_start(LABEL_START(dac_tri), LABEL_STOP(dac_tri));
        my_end_func(HFACP, 20.0);
    
        DAC_Linearity_HF(element(HFACP[0]), spec("hfampr"), spec("hfoffr"), 
                                            2048*num_active_sites()*spec("dacr_ave"), 
                                            spec("hfcpr"),  spec("dacr_ave"), 10);
    
    
        if(spec("log_analog")) {
            PARALLEL_EXECUTE(debug_printf("DNL[%d] = %8.3f LSB\n", parallel_idx, dac_DLE[parallel_idx]));
            PARALLEL_EXECUTE(debug_printf("INL[%d] = %8.3f LSB\n", parallel_idx, dac_ILE[parallel_idx]));
        }
    
        test_name("dac_sine");
    
        DAC_TIME(spec("hfcycs"), 0);
        setup_acp(HFACP,
                  spec("hfcycs")*1024+spec("hfcycs"),
                  spec("hfskews"),
                  spec("hfoffs"),
                  spec("hfamps"),
                  ACP_20MHZ_BW,
                  TRIG_VECTOR1,
                  TRIG_START);
    
        setup_cp(element(HFACP[0]), spec("hfcps")+1024*spec("dacs_ave")*num_active_sites(), 0, 
                                    TRIG_PROCESSOR_WRITE, TRIG_SAMPLE_COUNTER);
    
        prim_wait(spec("dac_wait1"));
        func_start(LABEL_START(dac_sine), LABEL_STOP(dac_sine));
        my_end_func(HFACP, 20.0);
    
        SNDR_SFDR_MEASURE_HF(HFACP, spec("hfamps"), spec("hfoffs"), 1024*num_active_sites(), 
                                    spec("hfcps"), spec("dacs_ave"), 10);

        if(spec("log_analog")) {
            PARALLEL_EXECUTE(debug_printf("SNR[%d]  = %8.3f LSB\n", parallel_idx, dac_SNR[parallel_idx]));
            PARALLEL_EXECUTE(debug_printf("THD[%d]  = %8.3f LSB\n", parallel_idx, dac_THD[parallel_idx]));
            PARALLEL_EXECUTE(debug_printf("SNDR[%d] = %8.3f LSB\n", parallel_idx, dac_SNDR[parallel_idx]));
            PARALLEL_EXECUTE(debug_printf("SFDR[%d] = %8.3f LSB\n", parallel_idx, dac_SFDR[parallel_idx]));
        } 

        if( (ii==1) && spec("dac_char") ) { 
            debug_printf("\n\n <<<  DAC Characterization   >>>\n");
    			    debug_printf("   VDD  :      DNL  ,      INL  ,     SNR  ,     THD  ,    SNDR  ,    SFDR\n");         }
        if( spec("dac_char") ) { 
    	     PARALLEL_EXECUTE(debug_printf(" %5.3fV : %7.2fLSB, %7.2fLSB, %7.2fdB, %7.2fdB, %7.2fdB, %7.2fdB\n", 
    		                  vdd_start+vdd_delta*ii, dac_DLE[i], dac_ILE[i], dac_SNR[i], dac_THD[i], dac_SNDR[i], dac_SFDR[i]));
        }

        open_fixture_relays(DACOUT, 0.0);
        open_io_relays(HFACP);
    }


    GATHERING_HISTOGRAM_DATA(HITEM++,    "DAC_DLE",  "LSB",  -6.0,   6.0,     dac_DLE, NUM_DUT, SITE_INFO);
    GATHERING_HISTOGRAM_DATA(HITEM++,    "DAC_ILE",  "LSB",  -6.0,   6.0,     dac_ILE, NUM_DUT, SITE_INFO);
    GATHERING_HISTOGRAM_DATA(HITEM++,    "DAC_SNR",   "dB",  40.0,  80.0,     dac_SNR, NUM_DUT, SITE_INFO);
    GATHERING_HISTOGRAM_DATA(HITEM++,    "DAC_THD",   "dB",  40.0,  80.0,     dac_THD, NUM_DUT, SITE_INFO);
    GATHERING_HISTOGRAM_DATA(HITEM++,   "DAC_SNDR",   "dB",  40.0,  80.0,    dac_SNDR, NUM_DUT, SITE_INFO);
    GATHERING_HISTOGRAM_DATA(HITEM++,   "DAC_SFDR",   "dB",  40.0,  80.0,    dac_SFDR, NUM_DUT, SITE_INFO);

    if(JJB_IRESULT("DACDLE",      DACOUT,   dac_DLE,      -6.0,       6.0,   LSB_UNIT, fbin)) { return 1; }  /* 1 LSB */
    if(JJB_IRESULT("DACILE",      DACOUT,   dac_ILE,      -6.0,       6.0,   LSB_UNIT, fbin)) { return 1; }  /* 4 LSB */
    if(JJB_IRESULT("DACSNR",      DACOUT,   dac_SNR,      40.0,      80.0,    DB_UNIT, fbin)) { return 1; }
    if(JJB_IRESULT("DACTHD",      DACOUT,   dac_THD,      40.0,      80.0,    DB_UNIT, fbin)) { return 1; }
    if(JJB_IRESULT("DACSNDR",     DACOUT,  dac_SNDR,      40.0,      80.0,    DB_UNIT, fbin)) { return 1; }
    if(JJB_IRESULT("DACSFDR",     DACOUT,  dac_SFDR,      40.0,      80.0,    DB_UNIT, fbin)) { return 1; }

    open_io_relays(HFACP);
    close_io_relays(ALLPIN);
 
    return 0;
}
#endif HF_TEST

#ifdef LF_TEST
void DAC_Linearity_LF(PINLIST *plist, double amplitude, double off_v, int size, int cp_offset, int average, int bit_num)
{
int    i, iii;
int    min_loc, max_loc;
int    code_size=1024;
int    d00, d01, d02, d03, d04, d05, d06, d07, d08, d09, d10, d11;
int    k00, k01, k02, k03, k04, k05, k06, k07, k08, k09, k10, k11;
char   KK0[4], KK1[4], KK2[4];
double dnl_minval, dnl_maxval, inl_minval, inl_maxval;
double fs, zero, fs1, zero1, fs2, zero2, xyratio, lsb, dac_value;

ERRORTYPE_t    *DspStatus;
WaveHdl_t       ramp_unav_wf;
WaveHdl_t       ramp_wf_av;
WaveHdl_t       ramp_wf1;
WaveHdl_t       inl_wf;
WaveHdl_t       dnl_wf;
WaveHdl_t       inl_temp_wf;
WaveHdl_t       dnl_temp1_wf;
WaveHdl_t       dnl_temp2_wf;
WaveHdl_t       ramp_temp_wf;
WaveHdl_t       imsi_wf;
WaveHdl_t       imsi1_wf;
WaveHdl_t       imsi2_wf;

    load_pinlist(plist, UNUSED); 

    check_dsp(dsp_free_waveforms(6,imsi_wf,ramp_temp_wf,inl_wf,dnl_wf,dnl_temp1_wf,dnl_temp2_wf));
    check_dsp(dsp_free_waveforms(2,ramp_unav_wf,ramp_wf_av));

if(spec("log_analog")) { 
    printf("size = %d\n", size); 
}            
    check_dsp(dsp_deinterleave_cp_to_wf(plist, cp_offset, size, 1, RRECT, "ramp_unav_wf", &ramp_unav_wf));
    check_dsp(dsp_create_wf(plist, code_size*2, RRECT, "ramp_wf_av", &ramp_wf_av));
    check_dsp(dsp_average_wf(plist,ramp_unav_wf, average, ramp_wf_av));
    check_dsp(dsp_wc_sub (plist, ramp_wf_av, 524288, ramp_wf_av));
    check_dsp(dsp_zone_wf(plist, ramp_wf_av, 0, code_size-1, "imsi_wf", &imsi_wf));
    check_dsp(dsp_min_max_meas(plist, imsi_wf, VALUE, &zero, &fs, &min_loc, &max_loc));

    lfacp_lsb_voltage_info(plist,&lsb);
    zero_level=zero*lsb;

    fs_level=fs*lsb;
    Vlsb= (fs_level-zero_level)/(code_size - 1.0);

if(spec("log_analog")) { 
    printf("Actual LSB\n");
    printf("zero = %9.6f, fs = %9.6f, lsb = %14.9f, Vlsb = %14.9f\n", zero*lsb, fs*lsb, lsb, Vlsb); 
}
    check_dsp(dsp_create_wf(plist, 1022-spec("rampoff")*2, RRECT, "dnl_wf", &dnl_wf));
    check_dsp(dsp_create_wf(plist, 1022-spec("rampoff")*2, RRECT, "inl_wf", &inl_wf));

    check_dsp(dsp_wc_mult(plist, imsi_wf, lsb, imsi_wf));
/************* YHS DEBUG ***************/
if(spec("dac_debug")) {
for(iii = spec("dac_dbg_start"); iii < spec("dac_dbg_stop"); iii++) {
    check_dsp(dsp_read_num(plist, imsi_wf, VALUE, iii, &dac_value));

    d09 = iii/512;   k09 = iii%512;
    d08 = k09/256;   k08 = k09%256;
    d07 = k08/128;   k07 = k08%128;
    d06 = k07/64;    k06 = k07%64;
    d05 = k06/32;    k05 = k06%32;
    d04 = k05/16;    k04 = k05%16;
    d03 = k04/8;     k03 = k04%8;
    d02 = k03/4;     k02 = k03%4;
    d01 = k02/2;     k01 = k02%2;
    d00 = k01/1;     k00 = k01%1;

    sprintf(KK2, "00%d%d",   d09, d08);
    sprintf(KK1, "%d%d%d%d", d07, d06, d05, d03);
    sprintf(KK0, "%d%d%d%d", d03, d02, d01, d00);

	debug_printf("DACIN(%4d) : %s,%s,%s -> %7.3f mV\n", iii, KK2, KK1, KK0, dac_value*1.0e3);
}
}
/************* YHS DEBUG ***************/

/* DNL Calculating */
    check_dsp(dsp_zone_wf(plist, imsi_wf, 1+spec("rampoff"), code_size-1-spec("rampoff"), "dnl_temp1_wf", &dnl_temp1_wf));
    check_dsp(dsp_zone_wf(plist, imsi_wf, 0+spec("rampoff"), code_size-2-spec("rampoff"), "dnl_temp2_wf", &dnl_temp2_wf));
    check_dsp(dsp_wf_sub(plist,  dnl_temp1_wf, dnl_temp2_wf, dnl_wf));
    check_dsp(dsp_wc_add(plist,  dnl_wf, -Vlsb, dnl_wf));
    check_dsp(dsp_wc_mult(plist, dnl_wf, 1/Vlsb, dnl_wf));

/*  check_dsp(dsp_write_num(plist, 0.0, VALUE, 4095, dnl_wf)); */

    check_dsp(dsp_min_max_meas(plist, dnl_wf, VALUE, &dnl_minval, &dnl_maxval, NULL, NULL));
    dnl_worst = fabs(dnl_maxval)>fabs(dnl_minval)?(dnl_maxval):(dnl_minval);

if(spec("log_analog")) { printf("DNL = %9.6f\n", dnl_worst); }
 
/* Convert to INL by integration */
    check_dsp(dsp_wf_integr_t (plist, dnl_wf, inl_wf));
    check_dsp(dsp_min_max_meas(plist, inl_wf, VALUE, &inl_minval, &inl_maxval, NULL, NULL));
    inl_worst = fabs(inl_maxval)>fabs(inl_minval)?(inl_maxval):(inl_minval);

if(spec("log_analog")) { printf("INL = %9.6f\n", inl_worst); }

    check_dsp(dsp_free_waveforms(6,imsi_wf,ramp_temp_wf,inl_wf,dnl_wf,dnl_temp1_wf,dnl_temp2_wf));
    check_dsp(dsp_free_waveforms(2,ramp_unav_wf,ramp_wf_av));
}

void SNDR_SFDR_MEASURE_LF(plist,lfacp_cycle,offset,sample_num,average, dac_out)
PINLIST *plist;
int      offset, sample_num, average;
double   lfacp_cycle;
double   dac_out[16];
{
double lsb,Thd,Snr,Sndr,sfdr,rms_noise,gain_level,offset_level, freq;
double sfdr_max, mag;
int    min_loc, max_loc;
double fs, zero, fs1, zero1, fs2, zero2, xyratio, dac_value;
double    pls_params[11], tlevel[] = {0.2,0.5,0.8};
int    ii;

ERRORTYPE_t     *DspStatus;
WaveHdl_t temp_wf1= NULL;
WaveHdl_t sfdr_crect = NULL;
WaveHdl_t sfdr_polar = NULL;


	fs = 0.0;
    zero = 0.0;
    min_loc = 0;
	max_loc = 0;
    pls_params[10] = 0.0;
    period1 = 0.0;

    load_pinlist(plist, UNUSED);

    check_dsp(dsp_free_waveforms(2, temp_wf1, sine_wf_av));

    check_dsp(dsp_deinterleave_cp_to_wf(plist,offset,sample_num,1,RRECT,"temp_wf1",&temp_wf1));

    lfacp_lsb_voltage_info(plist,&lsb);

    check_dsp(dsp_wc_sub (plist, temp_wf1, 524288, temp_wf1));
    check_dsp(dsp_wc_mult(plist, temp_wf1, lsb, temp_wf1));
    check_dsp(dsp_wf_set_x_scale(plist, temp_wf1, lfacp_cycle));

    check_dsp(dsp_create_wf(plist, sample_num/(average/2), RRECT, "sine_wf_av", &sine_wf_av));
    check_dsp(dsp_average_wf(plist, temp_wf1, average/2, sine_wf_av));
    check_dsp(dsp_min_max_meas(plist, sine_wf_av, VALUE, &zero, &fs, &min_loc, &max_loc));
/*
	for(ii=0;ii<16;ii++){
		check_dsp(dsp_read_num(plist, sine_wf_av, VALUE, ii, &dac_out[ii]));
	}
*/

    check_dsp(dsp_pls_freq(plist, sine_wf_av, VALUE, 0.0, (1.8 * spec("lfcycs") * 16), 1, &freq));
/*
    check_dsp(dsp_pls_meas(plist, sine_wf_av, RRECT, 0.0, (2.0 * spec("lfcycs") * 16), DSP_MEAN, 1,tlevel, &pls_params));
    period1 = pls_params[10];
*/
    period1 =1/freq; 

    fs_level=fs;
    zero_level=zero;
/*
	debug_printf("FSLEVEL = %6.2f V \n", fs_level);
	debug_printf("ZSLEVEL = %6.2f V \n", zero_level);
	debug_printf("freq = %6.2f mS \n", freq);
	debug_printf("T = %6.4f mS \n", period1*1000);
*/
    check_dsp(dsp_free_waveforms(2, temp_wf1,sine_wf_av));
}

int LF_DAC_TEST_AP()
{
int fbin, i, j, retest11, retest12;
int dac_repeat, mmm, ii, jj, iii;
int d00, d01, d02, d03, d04, d05, d06, d07, d08, d09, d10, d11;
int k00, k01, k02, k03, k04, k05, k06, k07, k08, k09, k10, k11;
char KK0[4], KK1[4], KK2[4];

double vdd_step, vdd_start, vdd_stop, vdd_delta;
double kkk[4];
double amp[4], time[4];
double dac_IDD33[4],dac_IDD25[4], dac_IDDA[4],  dac_IREF[4],  idd_result,   dac_VALUE[4];
double mxx_DLE[10], mxx_ILE[10], mxx_THD[10], mxx_SNDR[10], mxx_SFDR[10], mxx_IDD[10];
double dac_out[16][4], dac_out1[4][16];

    dac_DLE[0]=0.0;   dac_DLE[1]=0.0;   dac_DLE[2]=0.0;   dac_DLE[3]=0.0;
    dac_ILE[0]=0.0;   dac_ILE[1]=0.0;   dac_ILE[2]=0.0;   dac_ILE[3]=0.0;
    dac_THD[0]=0.0;   dac_THD[1]=0.0;   dac_THD[2]=0.0;   dac_THD[3]=0.0;
    dac_SNR[0]=0.0;   dac_SNR[1]=0.0;   dac_SNR[2]=0.0;   dac_SNR[3]=0.0;
    dac_SNDR[0]=0.0;  dac_SNDR[1]=0.0;  dac_SNDR[2]=0.0;  dac_SNDR[3]=0.0;
    dac_SFDR[0]=0.0;  dac_SFDR[1]=0.0;  dac_SFDR[2]=0.0;  dac_SFDR[3]=0.0;

    test_name("dac_test"); fbin = 31;

    close_io_relays(ALLPIN);

if(spec("dac_char")) { vdd_step = 11; vdd_start = 2.8; vdd_stop = 3.8; vdd_delta = 0.10; }
else                 { vdd_step = 1;  vdd_start = 3.3; vdd_stop = 3.3; vdd_delta = 0.00; }

for(ii=1; ii<=vdd_step; ii++) {

    VDDAC_VAL = vdd_start+vdd_delta*ii;
	GROSS_LEVEL(0); 
    SET_LEVEL(COMP, 0, spec("comp_vth"), spec("comp_vth"), -1.0, -1.0, -1.0, -1.0, -1.0);


    for(i=0; i<N_SITES; i++){
        if(active_test_sites() & SITE_N(i+1)){

            PARALLEL_CALL_BY_SITE(AP, SITE_N(i+1), close_fixture_relays(AP, 0.0));
            PARALLEL_CALL_BY_SITE(LFACP,  SITE_N(i+1), close_io_relays(LFACP));

            lfi_enabled();

            DAC_TIME(spec("dac_time"), 0);
            func_seq(LABEL_START(dac), LABEL_STOP(dac));
            prim_wait(spec("dac_wait0"));
            lfacp_input_setup(LFACP,
                              spec("lfoffs"),
                              spec("lfamps"),
                              LFACP_INPUT_10M_OHM,
                              LF_DISCONNECT,
                              LF_DISCONNECT,   /* LF_LOWPASS_100KHZ,   */
                              INPUT_SINGLE_ENDED_POS);

            lfacp_timing_setup(LFACP,
                               LF_16_BITS,
                               spec("lfcycs"), 
                               spec("lfskews"), 
                               TRIG_VECTOR1,
                               TRIG_START,
                               QUAL_ALWAYS_TRUE,
                               UNUSED);

            setup_cp(element(LFACP[0]), 16*spec("dacs_ave"), 0, TRIG_PROCESSOR_WRITE, TRIG_SAMPLE_COUNTER);
            prim_wait(spec("dac_wait1"));

            set_func_mode(LOOP_ON_FUNCTIONAL);
            func_start(LABEL_START(dac), LABEL_STOP(dac));
            my_end_func(LFACP, 1.0);

            SNDR_SFDR_MEASURE_LF(element(LFACP[0]), spec("lfcycs"), spec("lfcps"), 16*spec("dacs_ave"), spec("dacs_ave"), dac_out1[i]);
            lfi_disabled();

			amp[i] = fs_level - zero_level;
			time[i] = period1;
/*
            debug_printf("FULL SCALE = %6.2f V \n", fs_level);
*/
            PARALLEL_CALL_BY_SITE(LFACP,  SITE_N(i+1), open_io_relays(LFACP));
            PARALLEL_CALL_BY_SITE(AP, SITE_N(i+1), open_fixture_relays(AP, 0.0));

            if( (ii==1) && spec("dac_char") ) { 
                debug_printf("\n\n <<<  DAC Characterization   >>>\n");
			    debug_printf("   VDD  :      DNL  ,      INL  ,     SNR  ,     THD  ,    SNDR  ,    SFDR\n"); }
            if( spec("dac_char") ) { 
	            PARALLEL_EXECUTE(debug_printf(" %5.3fV : %7.2fLSB, %7.2fLSB, %7.2fdB, %7.2fdB, %7.2fdB, %7.2fdB\n", 
		                             vdd_start+vdd_delta*ii, dac_DLE[i], dac_ILE[i], dac_SNR[i], dac_THD[i], dac_SNDR[i], dac_SFDR[i]));
            }
        }
    }
}
    open_io_relays(LFACP);
    close_io_relays(ALLPIN);

    GATHERING_HISTOGRAM_DATA(HITEM++,   "FSCALE",   "V",  2.0,  3.0, amp, NUM_DUT, SITE_INFO);
    GATHERING_HISTOGRAM_DATA(HITEM++,   "CYCLE",   "MS",  0.5,  1.5, time, NUM_DUT, SITE_INFO);

    if(JJB_IRESULT("AP_FSCALE",     DACOUT,   amp,       2.0,  3.0,    V_UNIT, fbin)) { return 1; }
    if(JJB_IRESULT("AP_CYCLE",      DACOUT,   time,   0.5e-3,   1.5e-3,    S_UNIT, fbin)) { return 1; }

/*
	for(ii=0;ii<16;ii++){
		for(i=0;i<4;i++){
			dac_out[ii][i] = dac_out1[i][ii];
		}
	}

	if(JJB_IRESULT("DAC_LEV1", VDDAC, dac_out[0],  dac_lev[0]-0.05, dac_lev[0]+0.05, V_UNIT, fbin)) { return 1; }
	for(ii=1;ii<16;ii++){
   		if(JJB_IRESULT("DAC_LEV1", VDDAC, dac_out[ii],  dac_lev[ii]*0.95, dac_lev[ii]*1.05, V_UNIT, fbin)) { return 1; }
	}
*/
    return 0;
}


int LF_DAC_TEST_AM()
{
int fbin, i, j, retest11, retest12;
int dac_repeat, mmm, ii, jj, iii;
int d00, d01, d02, d03, d04, d05, d06, d07, d08, d09, d10, d11;
int k00, k01, k02, k03, k04, k05, k06, k07, k08, k09, k10, k11;
char KK0[4], KK1[4], KK2[4];

double vdd_step, vdd_start, vdd_stop, vdd_delta;
double kkk[4];
double amp[4], time[4];
double dac_IDD33[4],dac_IDD25[4], dac_IDDA[4],  dac_IREF[4],  idd_result,   dac_VALUE[4];
double mxx_DLE[10], mxx_ILE[10], mxx_THD[10], mxx_SNDR[10], mxx_SFDR[10], mxx_IDD[10];
double dac_out[16][4], dac_out1[4][16];

    dac_DLE[0]=0.0;   dac_DLE[1]=0.0;   dac_DLE[2]=0.0;   dac_DLE[3]=0.0;
    dac_ILE[0]=0.0;   dac_ILE[1]=0.0;   dac_ILE[2]=0.0;   dac_ILE[3]=0.0;
    dac_THD[0]=0.0;   dac_THD[1]=0.0;   dac_THD[2]=0.0;   dac_THD[3]=0.0;
    dac_SNR[0]=0.0;   dac_SNR[1]=0.0;   dac_SNR[2]=0.0;   dac_SNR[3]=0.0;
    dac_SNDR[0]=0.0;  dac_SNDR[1]=0.0;  dac_SNDR[2]=0.0;  dac_SNDR[3]=0.0;
    dac_SFDR[0]=0.0;  dac_SFDR[1]=0.0;  dac_SFDR[2]=0.0;  dac_SFDR[3]=0.0;

    test_name("dac_test"); fbin = 31;

    close_io_relays(ALLPIN);

if(spec("dac_char")) { vdd_step = 11; vdd_start = 2.8; vdd_stop = 3.8; vdd_delta = 0.10; }
else                 { vdd_step = 1;  vdd_start = 3.3; vdd_stop = 3.3; vdd_delta = 0.00; }

for(ii=1; ii<=vdd_step; ii++) {

    VDDAC_VAL = vdd_start+vdd_delta*ii;
	GROSS_LEVEL(0); 
    SET_LEVEL(COMP, 0, spec("comp_vth"), spec("comp_vth"), -1.0, -1.0, -1.0, -1.0, -1.0);

    for(i=0; i<N_SITES; i++){
        if(active_test_sites() & SITE_N(i+1)){

            PARALLEL_CALL_BY_SITE(AM, SITE_N(i+1), close_fixture_relays(AM, 0.0));
            PARALLEL_CALL_BY_SITE(LFACP,  SITE_N(i+1), close_io_relays(LFACP));

            lfi_enabled();

            DAC_TIME(spec("dac_time"), 0);
            func_seq(LABEL_START(dac), LABEL_STOP(dac));
            prim_wait(spec("dac_wait0"));
            lfacp_input_setup(LFACP,
                              spec("lfoffs"),
                              spec("lfamps"),
                              LFACP_INPUT_10M_OHM,
                              LF_DISCONNECT,
                              LF_DISCONNECT,   /* LF_LOWPASS_100KHZ,   */
                              INPUT_SINGLE_ENDED_NEG);

            lfacp_timing_setup(LFACP,
                               LF_16_BITS,
                               spec("lfcycs"), 
                               spec("lfskews"), 
                               TRIG_VECTOR1,
                               TRIG_START,
                               QUAL_ALWAYS_TRUE,
                               UNUSED);

            setup_cp(element(LFACP[0]), 16*spec("dacs_ave"), 0, TRIG_PROCESSOR_WRITE, TRIG_SAMPLE_COUNTER);
            prim_wait(spec("dac_wait1"));

            set_func_mode(LOOP_ON_FUNCTIONAL);
            func_start(LABEL_START(dac), LABEL_STOP(dac));
            my_end_func(LFACP, 1.0);

            SNDR_SFDR_MEASURE_LF(element(LFACP[0]), spec("lfcycs"), spec("lfcps"), 16*spec("dacs_ave"), spec("dacs_ave"), dac_out1[i]);
            lfi_disabled();

			amp[i] = fs_level - zero_level;
			time[i] = period1;
/*
            debug_printf("FULL SCALE = %6.2f V \n", fs_level);
*/
            PARALLEL_CALL_BY_SITE(LFACP,  SITE_N(i+1), open_io_relays(LFACP));
            PARALLEL_CALL_BY_SITE(AM, SITE_N(i+1), open_fixture_relays(AM, 0.0));

            if( (ii==1) && spec("dac_char") ) { 
                debug_printf("\n\n <<<  DAC Characterization   >>>\n");
			    debug_printf("   VDD  :      DNL  ,      INL  ,     SNR  ,     THD  ,    SNDR  ,    SFDR\n"); }
            if( spec("dac_char") ) { 
	            PARALLEL_EXECUTE(debug_printf(" %5.3fV : %7.2fLSB, %7.2fLSB, %7.2fdB, %7.2fdB, %7.2fdB, %7.2fdB\n", 
		                             vdd_start+vdd_delta*ii, dac_DLE[i], dac_ILE[i], dac_SNR[i], dac_THD[i], dac_SNDR[i], dac_SFDR[i]));
            }
        }
    }
}
    open_io_relays(LFACP);
    close_io_relays(ALLPIN);

    GATHERING_HISTOGRAM_DATA(HITEM++,   "FSCALE",   "V",  2.0,  3.0, amp, NUM_DUT, SITE_INFO);
    GATHERING_HISTOGRAM_DATA(HITEM++,   "CYCLE",   "MS",  0.5,  1.5, time, NUM_DUT, SITE_INFO);

    if(JJB_IRESULT("AM_FSCALE",     DACOUT,   amp,       2.0,      3.0,    V_UNIT, fbin)) { return 1; }
    if(JJB_IRESULT("AM_CYCLE",      DACOUT,   time,   0.5e-3,   1.5e-3,    S_UNIT, fbin)) { return 1; }

/*
	for(ii=0;ii<16;ii++){
		for(i=0;i<4;i++){
			dac_out[ii][i] = dac_out1[i][ii];
		}
	}

	if(JJB_IRESULT("DAC_LEV1", VDDAC, dac_out[0],  dac_lev[0]-0.05, dac_lev[0]+0.05, V_UNIT, fbin)) { return 1; }
	for(ii=1;ii<16;ii++){
   		if(JJB_IRESULT("DAC_LEV1", VDDAC, dac_out[ii],  dac_lev[ii]*0.95, dac_lev[ii]*1.05, V_UNIT, fbin)) { return 1; }
	}
*/
    return 0;
}

#endif LF_TEST

/*+++++++++++++++++++++++ TEST_DEVICE +++++++++++++++++++++++++++++*/
void test_device()
{
char YEAR[10],MONTH[5],DAY[5];
int  HH,MM,SS;

/***** VDD RANGE *****
 1.2V =  1.08 ~ 1.32
 1.8V =  1.62 ~ 1.98
 2.8V =  2.52 ~ 3.08
 3.3V =  2.97 ~ 3.63
**********************/
/**********  JJB HISTOGRAM  **********/
HITEM = 0;
SITE_INFO = active_test_sites();
GATHERING_SITE(NUM_DUT, SITE_INFO);
/**********  JJB HISTOGRAM  *********/

    /**********  JJB CONTACT  **********/
    if(spec("contact_check")){
        CONTACT_CHECK();
        return;
    }
    /**********  JJB CONTACT  **********/

ex_time("start");

    prim_wait(spec("start_wait"));
    test_name( "START" );
    set_bin(bin_name("UNTESTED"), active_test_sites());
    hbiu_operator_msg("TESTING");

    close_io_relays(ALLPIN);
    vista_debug_command("cform 1..448 mask");


    if(OS_TEST())    { hbiu_operator_msg("OS FAIL");     goto finish; } ex_time("OS");

    if(IIL_TEST())   { hbiu_operator_msg("IIL_FAIL");    goto finish; } ex_time("F_IIL");
    if(IIH_TEST())   { hbiu_operator_msg("IIH_FAIL");    goto finish; } ex_time("F_IIH");

    if(DIDD_TEST())  { hbiu_operator_msg("IDD_FAIL");    goto finish; } ex_time("F_IDD");

/*** VDDIO : 3.3 => 1.8V : 2007.10.15 requested by WY PARK ***/
    VDDIO_VAL = 3.30; VDDCO_VAL = 1.80; VDPLL_VAL = 1.80; VDDAC_VAL = 3.30; VDDT_VAL = 3.30;
    if(GRS_TEST())   { hbiu_operator_msg("GRS_FAIL");    goto finish; } ex_time("F_GRS");

    VDDIO_VAL = 3.30; VDDCO_VAL = 1.80; VDPLL_VAL = 1.80; VDDAC_VAL = 3.30; VDDT_VAL = 3.30;

#ifdef DCP_OPTION
    if(!pll_check_done) { PLL_CHECK(0); }

    if(PLL_TEST (0))  { hbiu_operator_msg("PLL_FAIL");    goto finish; } ex_time("F_PLL");
#endif DCP_OPTION


#ifdef TD2_OPTION
    if(PLL_TEST2(0))  { hbiu_operator_msg("PLL_FAIL");    goto finish; } ex_time("F_PLL");
#endif TD2_OPTION
#ifdef TD3_OPTION
    if(PLL_TEST3(0))  { hbiu_operator_msg("PLL_FAIL");    goto finish; } ex_time("F_PLL");
#endif TD3_OPTION


#ifdef LF_TEST
	if(LF_DAC_TEST_AP()){ hbiu_operator_msg("DAC_FAIL");    goto finish; } ex_time("F_DAC");
	if(LF_DAC_TEST_AM()){ hbiu_operator_msg("DAC_FAIL");    goto finish; } ex_time("F_DAC");
#endif LF_TEST
#ifdef HF_TEST
    if(HF_DAC_TEST()){ hbiu_operator_msg("DAC_FAIL");    goto finish; } ex_time("F_DAC");
#endif HF_TEST

/***********************************************************************/
/*  good_dut */
    set_bin(bin_name("PASS"), active_test_sites());
    hbiu_operator_msg("ALL PASS");

/*  If someone sets the over fail switch the part will be binned to a untested bin */
    if(override_flag(DEFAULT_MODE , NULL) != 0) {
        hbiu_operator_msg("UNTESTED");
        log_printf("\n\n******** WARNING OVERRIDE CONDITION SET ******\n");
        log_printf("         BINING PART to NOT_TESTED Bin \n Over ride flag is\n");
        debugger_command("over");log_printf("\n");
        set_bin(bin_name("UNTESTED"), active_test_sites());
    }

finish:
    update_bin_counters();
    shutdown_device();

ex_time("test_end");
ex_time("print");


} /* end_test_device */

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
 SHUTDOWN_DEVICE
 *+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void shutdown_device()
{
int  i;
char f_name[100];

    test_name("shutdown");
    activate_sites();    /* DUAL SITES */

#ifdef TD2_OPTION
connect_matrix(TD_PINS, 0, 0, 0, 0);
#endif TD2_OPTION

#ifdef TD3_OPTION
    connect_GPmux(TD_PINS, OPEN, OPEN, OPEN, OPEN);
    open_io_relays(TD_PINS);
#endif TD3_OPTION

    set_func_mode(STOP_ON_FIRST);
    func_advance(NULL);
    func_stop();

    set_force_levels(ALLPIN,LEVELS_PER_PIN,0.0,0.0);
    set_active_loads(ALLPIN,DISABLED);
    SET_TIME(ALLPIN,   0, DNRZ, -1.0,-1.0, DNRI,-1.0,-1.0,   MASK,  0.0, NOCHANGE);
    open_io_relays(ALLPIN);

    POWER_OFF();

    unset_pmu_mode(ALLPIN);
    set_pmu_measure_i(ALLPIN, MC1UA, 0.0);
    unset_pmu_mode(ALLPIN);

/**********  JJB HISTOGRAM  **********/
    GATHERING_BIN_DATA(NUM_DUT, SITE_INFO, 0, CODE_NAME, BD_ID);  /* appended 2006.1.5. */
    for(i=1; i<9; i++) { if(SITE_INFO & SITE_N(i)) { NUM_DUT++; } }
    production_display(0);
/**********  JJB HISTOGRAM  *********/


}

/*************************************************************/
int PatLoad(
    char pattern_name[448],
    char pin_load[448],
    int stad, int load)
{
    char full_pattern_name[448];
    char buff[448];
    int     vec_cnt;


    sprintf(buff,"/jjblnx6/customer/eyenix/vector/jjbsoc/pattern/%s",pattern_name);                   
    load_patrefs(buff, stad); 

    if (load == FALSE) {
        /*** Read .patref file ***/
        stad += get_ppm_vector_quantity(buff);
    }
    else {
        /*** Load Pattern ***/
        apply_program_defaults(full_pattern_name, buff,".pat");
        vec_cnt = pat_load(full_pattern_name, stad, pin_load, ALL_SITES);
        log_printf("Pattern Loaded: %30s (%10d   %10d)\n",
                full_pattern_name, stad, stad +vec_cnt -1);

        stad += vec_cnt;
    }        

    return stad;
}

void CREATE_PINS()
{
make_pin("XI_",              "b",      54,  239,   68,  140 );
make_pin("XO_",              "b",      52,  242,   66,  142 );
make_pin("RSTN_",            "b",      50,  244,   71,  144 );
make_pin("DI0_",             "b",      51,  246,   69,  137 );
make_pin("DI1_",             "b",      49,  248,   67,  139 );
make_pin("DI2_",             "b",      64,  241,   65,  141 );
make_pin("DI3_",             "b",      62,  243,   80,  143 );
make_pin("DI4_",             "b",      60,  245,   78,  146 );
make_pin("DI5_",             "b",      58,  247,   76,  148 );
make_pin("DI6_",             "b",      63,  250,   74,  150 );

make_pin("DI7_",             "b",      61,  252,   79,  152 );
make_pin("DI8_",             "b",      59,  254,   77,  145 );
make_pin("DI9_",             "b",      57,  256,   75,  147 );
make_pin("DSL_",             "b",     264,  249,   73,  149 );
make_pin("H1_",              "b",     262,  200,   88,  151 );
make_pin("H2_",              "b",     260,  198,   86,  154 );
make_pin("RG_",              "b",     258,  196,   84,  156 );
make_pin("TP0_",             "b",     263,  194,   87,  158 );
make_pin("TP1_",             "b",     261,  199,   85,  160 );
make_pin("TP2_",             "b",     259,  197,   83,  153 );

make_pin("V2_",              "b",     257,  195,   81,  155 );
make_pin("V1_",              "b",     272,  193,   96,  157 );
make_pin("SG1_",             "b",     270,  204,   94,  159 );
make_pin("V3_",              "b",     268,  206,   92,  164 );
make_pin("SG3_",             "b",     266,  208,   90,  166 );
make_pin("V4_",              "b",     271,  201,   95,  168 );
make_pin("SUB_",             "b",     269,  203,   93,  161 );
make_pin("ESYN_",            "b",     267,  205,   91,  163 );
make_pin("COXL_",            "b",      20,  207,   98,  165 );
make_pin("GPIO17_",          "b",      23,  210,  100,  167 );

make_pin("GPIO16_",          "b",      21,  212,  102,  170 );
make_pin("GPIO15_",          "b",      19,  214,  112,  172 );
make_pin("GPIO14_",          "b",      17,  216,  110,  174 );
make_pin("GPIO13_",          "b",      32,  209,  108,  176 );
make_pin("RX_",              "b",      30,  211,  106,  169 );
make_pin("TX_",              "b",      28,  213,  111,  171 );
make_pin("AP_",              "b",      22,  251,  306,  186 );
make_pin("AM_",              "b",      24,  253,  314,  177 );
make_pin("COMP_",            "b",      29,  222,  105,  180 );
make_pin("REXT_",            "b",      40,  228,  308,  184 );

make_pin("GPIO12_",          "b",      26,  215,  109,  173 );
make_pin("GPIO11_",          "b",      31,  218,  107,  175 );
make_pin("GPIO10_",          "b",      27,  220,  312,  178 );
make_pin("GPIO9_",           "b",      25,  226,  310,  182 );
make_pin("GPIO8_",           "b",      38,  230,  311,  179 );
make_pin("GPIO7_",           "b",      36,  232,  309,  181 );
make_pin("GPIO6_",           "b",      34,  225,  307,  183 );
make_pin("GPIO5_",           "b",      39,  227,  305,  188 );
make_pin("GPIO4_",           "b",      37,  229,  320,  190 );
make_pin("GPIO3_",           "b",      35,  231,  318,  192 );

make_pin("GPIO2_",           "b",      44,  234,  316,  185 );
make_pin("GPIO1_",           "b",      42,  236,  319,  187 );
make_pin("GPIO0_",           "b",      47,  238,  317,  189 );
make_pin("PBLK_",            "b",      45,  240,  315,  191 );
make_pin("CLPOB_",           "b",      43,  233,  313,  133 );
make_pin("SHP_",             "b",      41,  235,   72,  135 );
make_pin("SHD_",             "b",      56,  237,   70,  138 );
make_pin("ADCK_",            "b",      18,  255,   10,    2 );

make_pin("VDDIO_",           "p",     377,  381,  369,  373 );
make_pin("VDDCO_",           "p",     378,  382,  370,  374 );
make_pin("VDPLL_",           "p",     379,  383,  371,  375 );
make_pin("VDDAC_",           "p",     380,  384,  372,  376 );

#ifdef LF_TEST
make_pin("LFACPP_",          "o",     465,  465,  469,  471 );   /* LFACP PIN */
make_pin("LFACPN_",          "o",     466,  468,  470,  472 );   /* LFACP PIN */
#endif LF_TEST
#ifdef HF_TEST
make_pin("HFACPP_",          "o",     473,  475,  477,  479 );   /* HFACP PIN */
make_pin("HFACPN_",          "o",     474,  476,  478,  480 );   /* HFACP PIN */
#endif HF_TEST

#ifdef TD2_OPTION
make_pin("td1_",             "o",     497,  497,  497,  497 );
make_pin("td2_",             "o",     498,  498,  498,  498 );
make_pin("td3_",             "o",     499,  499,  499,  499 );
make_pin("td4_",             "o",     500,  500,  500,  500 );
make_pin("td5_",             "o",     501,  501,  501,  501 );
make_pin("td6_",             "o",     502,  502,  502,  502 );
make_pin("td7_",             "o",     503,  503,  503,  503 );
make_pin("td8_",             "o",     504,  504,  504,  504 );
#endif TD2_OPTION

#ifdef TD3_OPTION
make_pin("td1_",             "o",     497,  497,  497,  497 );
make_pin("td2_",             "o",     498,  498,  498,  498 );
make_pin("td3_",             "o",     499,  499,  499,  499 );
make_pin("td4_",             "o",     500,  500,  500,  500 );
make_pin("td5_",             "o",     501,  501,  501,  501 );
make_pin("td6_",             "o",     502,  502,  502,  502 );
make_pin("td7_",             "o",     503,  503,  503,  503 );
make_pin("td8_",             "o",     504,  504,  504,  504 );
#endif TD3_OPTION

#ifdef DCP_OPTION
make_pin("dcp1_",            "o",     481,  483,  485,  487 );
#endif DCP_OPTION

make_pin("VDDTRLY_",         "i",     265,  202,   89,  162 );

}


void CREATE_PINLIST()
{
MAKE_PINLIST(XI,             "XI_");
MAKE_PINLIST(XO,             "XO_");
MAKE_PINLIST(RSTN,           "RSTN_");
MAKE_PINLIST(DI0,            "DI0_");
MAKE_PINLIST(DI1,            "DI1_");
MAKE_PINLIST(DI2,            "DI2_");
MAKE_PINLIST(DI3,            "DI3_");
MAKE_PINLIST(DI4,            "DI4_");
MAKE_PINLIST(DI5,            "DI5_");
MAKE_PINLIST(DI6,            "DI6_");

MAKE_PINLIST(DI7,            "DI7_");
MAKE_PINLIST(DI8,            "DI8_");
MAKE_PINLIST(DI9,            "DI9_");
MAKE_PINLIST(DSL,            "DSL_");
MAKE_PINLIST(H1,             "H1_");
MAKE_PINLIST(H2,             "H2_");
MAKE_PINLIST(RG,             "RG_");
MAKE_PINLIST(TP0,            "TP0_");
MAKE_PINLIST(TP1,            "TP1_");
MAKE_PINLIST(TP2,            "TP2_");

MAKE_PINLIST(V2,             "V2_");
MAKE_PINLIST(V1,             "V1_");
MAKE_PINLIST(SG1,            "SG1_");
MAKE_PINLIST(V3,             "V3_");
MAKE_PINLIST(SG3,            "SG3_");
MAKE_PINLIST(V4,             "V4_");
MAKE_PINLIST(SUB,            "SUB_");
MAKE_PINLIST(ESYN,           "ESYN_");
MAKE_PINLIST(COXL,           "COXL_");
MAKE_PINLIST(GPIO17,         "GPIO17_");

MAKE_PINLIST(GPIO16,         "GPIO16_");
MAKE_PINLIST(GPIO15,         "GPIO15_");
MAKE_PINLIST(GPIO14,         "GPIO14_");
MAKE_PINLIST(GPIO13,         "GPIO13_");
MAKE_PINLIST(RX,             "RX_");
MAKE_PINLIST(TX,             "TX_");
MAKE_PINLIST(AP,             "AP_");
MAKE_PINLIST(AM,             "AM_");
MAKE_PINLIST(COMP,           "COMP_");
MAKE_PINLIST(REXT,           "REXT_");

MAKE_PINLIST(GPIO12,         "GPIO12_");
MAKE_PINLIST(GPIO11,         "GPIO11_");
MAKE_PINLIST(GPIO10,         "GPIO10_");
MAKE_PINLIST(GPIO9,          "GPIO9_");
MAKE_PINLIST(GPIO8,          "GPIO8_");
MAKE_PINLIST(GPIO7,          "GPIO7_");
MAKE_PINLIST(GPIO6,          "GPIO6_");
MAKE_PINLIST(GPIO5,          "GPIO5_");
MAKE_PINLIST(GPIO4,          "GPIO4_");
MAKE_PINLIST(GPIO3,          "GPIO3_");

MAKE_PINLIST(GPIO2,          "GPIO2_");
MAKE_PINLIST(GPIO1,          "GPIO1_");
MAKE_PINLIST(GPIO0,          "GPIO0_");
MAKE_PINLIST(PBLK,           "PBLK_");
MAKE_PINLIST(CLPOB,          "CLPOB_");
MAKE_PINLIST(SHP,            "SHP_");
MAKE_PINLIST(SHD,            "SHD_");
MAKE_PINLIST(ADCK,           "ADCK_");


MAKE_PINLIST(ALLPIN,
"XI          XO          RSTN        DI0         DI1         DI2         DI3         DI4         DI5         DI6 \
 DI7         DI8         DI9         DSL         H1          H2          RG          TP0         TP1         TP2 \
 V2          V1          SG1         V3          SG3         V4          SUB         ESYN        COXL        GPIO17 \
 GPIO16      GPIO15      GPIO14      GPIO13      RX          TX          AP          AM          COMP        REXT \
 GPIO12      GPIO11      GPIO10      GPIO9       GPIO8       GPIO7       GPIO6       GPIO5       GPIO4       GPIO3 \
 GPIO2       GPIO1       GPIO0       PBLK        CLPOB       SHP         SHD         ADCK");

MAKE_PINLIST(OSPIN,
"XI          XO          RSTN        DI0         DI1         DI2         DI3         DI4         DI5         DI6 \
 DI7         DI8         DI9         DSL         H1          H2          RG          TP0         TP1         TP2 \
 V2          V1          SG1         V3          SG3         V4          SUB         ESYN        COXL        GPIO17 \
 GPIO16      GPIO15      GPIO14      GPIO13      RX          TX          AP          AM\
 GPIO12      GPIO11      GPIO10      GPIO9       GPIO8       GPIO7       GPIO6       GPIO5       GPIO4       GPIO3 \
 GPIO2       GPIO1       GPIO0       PBLK        CLPOB       SHP         SHD         ADCK");



MAKE_PINLIST(ALLIN,
"XI          RSTN        DI0         DI1         DI2         DI3         DI4         DI5         DI6         DI7 \
 DI8         DI9         DSL         TP0         TP1         TP2         ESYN        COXL        GPIO10      GPIO9 \
 GPIO8");

MAKE_PINLIST(IILHPIN,
"XI          RSTN        DI0         DI1         DI2         DI3         DI4         DI5         DI6         DI7 \
 DI8         DI9         TP2         ESYN        COXL");

MAKE_PINLIST(LEAKIN,
"XI          RSTN        DI0         DI1         DI2         DI3         DI4         DI5         DI6         DI7 \
 DI8         DI9         DSL         TP0         TP1         TP2         ESYN        COXL");

MAKE_PINLIST(ALLIO,
"GPIO17      GPIO16      GPIO15      GPIO14      GPIO13      RX          TX          REXT        GPIO12      GPIO11 \
 GPIO6       GPIO5       GPIO4       GPIO3       GPIO2       GPIO1       GPIO0");

MAKE_PINLIST(ALLOUT,
"XO          H1          H2          RG          V2          V1          SG1         V3          SG3         V4 \
 SUB         AP          AM          COMP        GPIO7       PBLK        CLPOB       SHP         SHD         ADCK");

MAKE_PINLIST(VDDIO,          "VDDIO_");
MAKE_PINLIST(VDDCO,          "VDDCO_");
MAKE_PINLIST(VDPLL,          "VDPLL_");
MAKE_PINLIST(VDDAC,          "VDDAC_");
MAKE_PINLIST(VDDALL,         "VDDIO,     VDDCO,     VDPLL,     VDDAC");

#ifdef LF_TEST
MAKE_PINLIST(LFACPP,         "LFACPP_");
MAKE_PINLIST(LFACPN,         "LFACPN_");
MAKE_PINLIST(LFACP,          "LFACPP,    LFACPN");
#endif LF_TEST
#ifdef HF_TEST
MAKE_PINLIST(HFACPP,         "HFACPP_");
MAKE_PINLIST(HFACPN,         "HFACPN_");
MAKE_PINLIST(HFACP,          "HFACPP,    HFACPN");
#endif HF_TEST


#ifdef TD2_OPTION
MAKE_PINLIST(td1,            "td1_");
MAKE_PINLIST(td2,            "td2_");
MAKE_PINLIST(td3,            "td3_");
MAKE_PINLIST(td4,            "td4_");
MAKE_PINLIST(td5,            "td5_");
MAKE_PINLIST(td6,            "td6_");
MAKE_PINLIST(td7,            "td7_");
MAKE_PINLIST(td8,            "td8_");
MAKE_PINLIST(TD_PINS,        "td1        td2        td3        td4        td5        td6        td7        td8");
#endif TD2_OPTION
#ifdef TD3_OPTION
MAKE_PINLIST(td1,            "td1_");
MAKE_PINLIST(td2,            "td2_");
MAKE_PINLIST(td3,            "td3_");
MAKE_PINLIST(td4,            "td4_");
MAKE_PINLIST(td5,            "td5_");
MAKE_PINLIST(td6,            "td6_");
MAKE_PINLIST(td7,            "td7_");
MAKE_PINLIST(td8,            "td8_");
MAKE_PINLIST(TD_PINS,        "td1        td2        td3        td4        td5        td6        td7        td8");
#endif TD3_OPTION

#ifdef DCP_OPTION
MAKE_PINLIST(DCP_PIN,        "dcp1_");
#endif DCP_OPTION

MAKE_PINLIST(VDDTRLY,        "VDDTRLY_");

MAKE_PINLIST(LOADPIN,        "ADCK, AM, AP, CLPOB, COXL, DI0, DI1, DI2, DI3, DI4, DI5, DI6,\
                              DI7, DI8, DI9, DSL, ESYN, GPIO0, GPIO1, GPIO2, GPIO3, GPIO4, GPIO5,\
                              GPIO6, GPIO7, GPIO8, GPIO9, GPIO10, GPIO11, GPIO12, GPIO13, GPIO14,\
                              GPIO15, GPIO16, GPIO17, H1, H2, PBLK, RG, RX, SHD, SHP, TP0, TP1, TP2,\
                              TX, XI, XO, RSTN, SG1, SG3, SUB, V1, V2, V3, V4");


MAKE_PINLIST(FLASHPIN, "ADCK CLPOB COXL DI0 DI1 DI2 DI3 DI4 DI5 DI6 DI7 DI8 DI9\
                        DSL ESYN GPIO0 GPIO1 GPIO2 GPIO3 GPIO4 GPIO5 GPIO6 GPIO7\
                        GPIO8 GPIO9 GPIO10 GPIO11 GPIO12 GPIO13 GPIO14 GPIO15\
                        GPIO16 GPIO17 H1 H2 PBLK RG RX SHD SHP TP0 TP1 TP2 TX\
                        XI XO RSTN SG1 SG3 SUB V1 V2 V3 V4");


MAKE_PINLIST(ATPGPIN,        "RSTN, XI, XO, DI0, DI1, DI2, DI3, DI4, DI5, DI6,\
                              DI7, DI8, DI9, DSL, H1, H2, RG, SHP, SHD, ADCK, V1, V2,\
                              V3, V4, SG1, SG3, SUB, CLPOB, PBLK, AM, AP, REXT, COMP,\
                              RX, TX, GPIO0, GPIO1, GPIO2, GPIO3, GPIO4, GPIO5, GPIO6,\
                              GPIO7, GPIO8, GPIO9, GPIO10, GPIO11, GPIO12, GPIO13, GPIO14,\
                              GPIO15, GPIO16, GPIO17, TP0, TP1, TP2, COXL, ESYN");

MAKE_PINLIST(GPIOPIN,        "GPIO0, GPIO1, GPIO2, GPIO3, GPIO4, GPIO5, GPIO6,\
                              GPIO7, GPIO8, GPIO9, GPIO10, GPIO11, GPIO12, GPIO13, GPIO14,\
                              GPIO15, GPIO16, GPIO17");

MAKE_PINLIST(DIPIN,          "DI0, DI1, DI2, DI3, DI4, DI5, DI6,\
                              DI7, DI8, DI9");

MAKE_PINLIST(VPIN,          "V1, V2, V3, V4");

MAKE_PINLIST(TPPIN,          "TP0, TP1, TP2");

MAKE_PINLIST(AMAP,           "AM, AP");
MAKE_PINLIST(DACOUT,         "AM, AP");

MAKE_PINLIST(FUNCLOAD,
"ADCK, AM, AP, CLPOB, COXL, DI0, DI1, DI2, DI3, DI4, DI5, DI6,\
DI7, DI8, DI9, DSL, ESYN, GPIO0, GPIO1, GPIO2, GPIO3, GPIO4, GPIO5,\
GPIO6, GPIO7, GPIO8, GPIO9, GPIO10, GPIO11, GPIO12, GPIO13, GPIO14,\
GPIO15, GPIO16, GPIO17, H1, H2, PBLK, RG, RX, SHD, SHP, TP0, TP1, TP2,\
TX, XI, XO, RSTN");

MAKE_PINLIST(FUNCGRP0,
"ADCK, AM, AP, CLPOB, GPIO7, H1, H2, PBLK, RG, SHD, SHP, TX, XO");

MAKE_PINLIST(FUNCGRP2,
"DI0, DI1, DI2, DI3, DI4, DI5, DI6, DI7, DI8, DI9");


create_parallel_pinlist(ALLPIN,   &par_allpin);
create_parallel_pinlist(ALLIN,    &par_allin);
create_parallel_pinlist(ALLIO,    &par_allio);
create_parallel_pinlist(ALLOUT,   &par_allout);
create_parallel_pinlist(LEAKIN,   &par_leakin);
create_parallel_pinlist(OSPIN,   &par_ospin);
create_parallel_pinlist(IILHPIN,   &par_iilhpin);
}
