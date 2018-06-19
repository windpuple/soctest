#ifndef INCLUDED_PmuUtil
#define INCLUDED_PmuUtil

#include "DpsUtil.hpp" /*switch DPS state*/
#define SERIAL_TEST_WORKAROUND_FOR_PIN_GROUP  //workaround for group setup, need be removed once API is fixed!

class PmuUtil
{
public:

  static void pmuCurrentMeasurement(
                                 const STRING&     measureMode,
                                 const STRING&     pinList,
                                 const TM::DCTEST_MODE& testMode,
                                 const DOUBLE      forceVoltage,
                                 const LIMIT&      limit,
                                 const DOUBLE      spmuClampCurrent,
                                 const STRING&     preCharge,
                                 const DOUBLE      prechargeVoltage,
                                 const DOUBLE      settlingTime,
                                 const STRING&     relaySwitchMode,
                                 const STRING&     termination,
                                 PPMU_MEASURE&     measurePpmu,
                                 SPMU_TASK&        spmuTask );

  static void pmuVoltageMeasurement(
                                 const STRING&     measureMode,
                                 const STRING&     pinList,
                                 const TM::DCTEST_MODE& testMode,
                                 const DOUBLE      forceCurrent,
                                 const LIMIT&      limit,
                                 const DOUBLE      spmuClampVoltage,
                                 const STRING&     preCharge,
                                 const DOUBLE      prechargeVoltage,
                                 const DOUBLE      settlingTime,
                                 const STRING&     relaySwitchMode,
                                 const STRING&     termination,
                                 PPMU_MEASURE&     measurePpmu,
                                 SPMU_TASK&        spmuTask,
                                 const DOUBLE      ppmuClampLow,
                                 const DOUBLE      ppmuClampHigh );

  static void ppmuParallelCurrentMeasurement(
                                 const STRING&     pinList,
                                 const TM::DCTEST_MODE& testMode,
                                 const DOUBLE      forceVoltage,
                                 const LIMIT&      limit,
                                 const STRING&     preCharge,
                                 const DOUBLE      prechargeVoltage,
                                 const DOUBLE      settlingTime,
                                 const STRING&     relaySwitchMode,
                                 const STRING&     termination,
                                 PPMU_MEASURE&     measurePpmu);

  static void ppmuSerialCurrentMeasurement(
                                 const STRING&         pinList,
                                 const TM::DCTEST_MODE& testMode,
                                 const DOUBLE          forceVoltage,
                                 const LIMIT&          limit,
                                 const STRING&         preCharge,
                                 const DOUBLE          prechargeVoltage,
                                 const DOUBLE          settlingTime,
                                 const STRING&     relaySwitchMode,
                                 const STRING&     termination,
                                 PPMU_MEASURE&     measurePpmu);


  static void ppmuSerialCurrentMeasurement_vfim(
                                 const STRING&         pinList,
                                 const TM::DCTEST_MODE& testMode,
                                 const DOUBLE          forceVoltage,
                                 const LIMIT&          limit,
                                 const STRING&         preCharge,
                                 const DOUBLE          prechargeVoltage,
                                 const DOUBLE          settlingTime,
                                 const STRING&         termination, 
                                 PMU_VFIM&             pmuVfim );

  static void spmuSerialCurrentMeasurement(
                                 const STRING&         pinList,
                                 const TM::DCTEST_MODE& testMode,
                                 const DOUBLE          forceVoltage,
                                 const LIMIT&          limit,
                                 const STRING&         relaySwitchMode,
                                 const DOUBLE          spmuClampCurrent,
                                 const STRING&         preCharge, 
                                 const DOUBLE          prechargeVoltage, 
                                 const DOUBLE          settlingTime,
                                 const STRING&         termination,
                                 SPMU_TASK&            spmuTask );
                                 
  static void ppmuParallelVoltageMeasurement(
                                 const STRING&         pinList,
                                 const TM::DCTEST_MODE& testMode,
                                 const DOUBLE          forceCurrent,
                                 const LIMIT&          limit,
                                 const STRING&         preCharge,
                                 const DOUBLE          prechargeVoltage,
                                 const DOUBLE          settlingTime,
                                 const STRING&         relaySwitchMode,
                                 const STRING&         termination,
                                 const DOUBLE          ppmuClampLow,
                                 const DOUBLE          ppmuClampHigh,
                                 PPMU_MEASURE&         measurePpmu);

  static void ppmuSerialVoltageMeasurement(
                                 const STRING&         pinList,
                                 const TM::DCTEST_MODE& testMode,
                                 const DOUBLE          forceCurrent,
                                 const LIMIT&          limit,
                                 const STRING&         preCharge,
                                 const DOUBLE          prechargeVoltage,
                                 const DOUBLE          settlingTime,
                                 const STRING&         relaySwitchMode,
                                 const STRING&         termination,
                                 const DOUBLE          ppmuClampLow,
                                 const DOUBLE          ppmuClampHigh,
                                 PPMU_MEASURE&         measurePpmu);


  static void ppmuSerialVoltageMeasurement_ifvm(
                                 const STRING&         pinList,
                                 const TM::DCTEST_MODE& testMode,
                                 const DOUBLE          forceCurrent,
                                 const LIMIT&          limit,
                                 const STRING&         preCharge,
                                 const DOUBLE          prechargeVoltage,
                                 const DOUBLE          settlingTime,
                                 const STRING&         termination,
                                 PMU_IFVM&             pmuIfvm );
                                 
  static void spmuSerialVoltageMeasurement(
                                 const STRING&         pinList,
                                 const TM::DCTEST_MODE& testMode,
                                 const DOUBLE          forceCurrent,
                                 const LIMIT&          limit,
                                 const STRING&         relaySwitchMode,
                                 const DOUBLE          spmuClampVoltage,
                                 const STRING&         preCharge,
                                 const DOUBLE          prechargeVoltage,
                                 const DOUBLE          settlingTime,
                                 const STRING&         termination,
                                 SPMU_TASK&            spmuTask );
                                 
  static void pmuCurrentGetResult(
                                 const STRING&          measureMode,
                                 const STRING_VECTOR&   pinList,
                                 const TM::DCTEST_MODE& testMode,
                                 const PPMU_MEASURE&    measurePpmu,
                                 const SPMU_TASK&       spmuTask,
                                 MeasurementResultContainer &measureResult);

  static void pmuVoltageGetResult(
                                 const STRING&          measureMode,
                                 const STRING_VECTOR&   pinList,
                                 const TM::DCTEST_MODE& testMode,
                                 const PPMU_MEASURE&    measurePpmu,
                                 const SPMU_TASK&       spmuTask,
                                 MeasurementResultContainer &measureResult);

  static void ppmuParallelCurrentGetResult(
                                 const STRING_VECTOR&   pinList,
                                 const TM::DCTEST_MODE& testMode,
                                 const PPMU_MEASURE&    measurePpmu,
                                 MeasurementResultContainer &measureResult);

  static void ppmuSerialCurrentGetResult(
                                 const STRING_VECTOR&   pinList,
                                 const TM::DCTEST_MODE& testMode,
                                 const PPMU_MEASURE&    measurePpmu,
                                 MeasurementResultContainer &measureResult);


  static void ppmuSerialCurrentGetResult_vfim(
                                 const STRING_VECTOR&   pinList,
                                 const TM::DCTEST_MODE& testMode,
                                 const PMU_VFIM&        pmuVfim,
                                 MeasurementResultContainer &measureResult);

  static void spmuSerialCurrentGetResult(
                                 const STRING_VECTOR&    pinList,
                                 const TM::DCTEST_MODE&  testMode,
                                 const SPMU_TASK&        spmuTask,
                                 MeasurementResultContainer& measureResult);

  static void ppmuParallelVoltageGetResult(
                                 const STRING_VECTOR&     pinList,
                                 const TM::DCTEST_MODE&   testMode,
                                 const PPMU_MEASURE&      measurePpmu,
                                 MeasurementResultContainer& measureResult);

  static void ppmuSerialVoltageGetResult(
                                 const STRING_VECTOR&     pinList,
                                 const TM::DCTEST_MODE&   testMode,
                                 const PPMU_MEASURE&      measurePpmu,
                                 MeasurementResultContainer& measureResult);


  static void ppmuSerialVoltageGetResult_ifvm(
                                 const STRING_VECTOR&     pinList,
                                 const TM::DCTEST_MODE&   testMode,
                                 const PMU_IFVM&          pmuIfvm,
                                 MeasurementResultContainer& measureResult);

  static void spmuSerialVoltageGetResult(
                                 const STRING_VECTOR&     pinList,
                                 const TM::DCTEST_MODE&   testMode,
                                 const SPMU_TASK&         spmuTask,
                                 MeasurementResultContainer& measureResult);
                                    
  static void ppmuRelaySwitch(PPMU_RELAY&      relayPpmuOn,
                              PPMU_RELAY&      relayAcOff,
                              PPMU_RELAY&      relayPm,
                              const STRING&    pinList,
                              const DOUBLE     settlingTime,
                              const STRING&    termination,
                              const STRING&    relaySwitchMode,
                              TASK_LIST&       tasklist);

  static void ppmuRelayRestore(PPMU_RELAY&      relayPpmuOff,
                               PPMU_RELAY&      relayAcOn,
                               PPMU_RELAY&      relayAc,
                               const STRING&    pinList,
                               const STRING&    termination,
                               const STRING&    relaySwitchMode,
                               TASK_LIST&       tasklist);

  static void ppmuPreCharge(PPMU_SETTING&   settingPpmuPrecharge,
                            const STRING&   pinList,
                            const DOUBLE    prechargeVoltage,
                            TASK_LIST&      tasklist);

  static void boardADCVoltageMeasurement(const STRING& pinList,
                                 const TM::DCTEST_MODE& testMode,
                                 const DOUBLE  forceCurrent,
                                 const LIMIT&  limit,
                                 const STRING&  relaySwitchMode,
                                 const STRING&  preCharge,
                                 const DOUBLE  prechargeVoltage,
                                 const DOUBLE  settlingTime,
                                 const STRING&  termination,
                                 const bool isPinParallelExec,
                                 SPMU_TASK&  spmuTask );

  static void spmuVoltageGetResult(const STRING_VECTOR&     pinList,               
                                   const TM::DCTEST_MODE&   testMode,
                                   const SPMU_TASK&         spmuTask,
                                   MeasurementResultContainer& measureResult);

  static void ppmuMeasurementRelaySwitchChange_SinglePin(
                                  const STRING&   singlePinName,
                                  const DOUBLE    settlingTime,
                                  const STRING&   termination,
                                  const STRING&   relaySwitchMode);

  static void ppmuMeasurementRelaySwitchRestore_SinglePin(
                                  const STRING&   singlePinName,
                                  const STRING&   termination,
                                  const STRING&   relaySwitchMode);
};


/*
 *----------------------------------------------------------------------*
 * Routine: PmuUtil::pmuCurrentMeasurement
 *
 * Purpose: execute pmu Current Measurement
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::pmuCurrentMeasurement(
  const STRING&          measureMode,
  const STRING&          pinList,
  const TM::DCTEST_MODE& testMode,
  const DOUBLE           forceVoltage,
  const LIMIT&           limit,
  const DOUBLE           spmuClampCurrent,
  const STRING&          preCharge,
  const DOUBLE           prechargeVoltage,
  const DOUBLE           settlingTime,
  const STRING&          relaySwitchMode,
  const STRING&          termination,
  PPMU_MEASURE&          measurePpmu,
  SPMU_TASK&             spmuTask )
{
    
  if ( measureMode == "PPMUpar" )
  {
    ppmuParallelCurrentMeasurement(pinList,                                  
                                  testMode,
                                  forceVoltage,
                                  limit,
                                  preCharge,
                                  prechargeVoltage,
                                  settlingTime,
                                  relaySwitchMode,
                                  termination,
                                  measurePpmu);
  }
  else if ( measureMode == "PPMUser" )
  {
    ppmuSerialCurrentMeasurement(pinList,                               
                                 testMode,
                                 forceVoltage,
                                 limit,
                                 preCharge,
                                 prechargeVoltage,
                                 settlingTime,
                                 relaySwitchMode,
                                 termination,
                                 measurePpmu);
  }
  else if ( measureMode == "SPMUser" )
  {
    spmuSerialCurrentMeasurement(pinList,                               
                                 testMode,
                                 forceVoltage,
                                 limit,
                                 relaySwitchMode,
                                 spmuClampCurrent,
                                 preCharge,
                                 prechargeVoltage,
                                 settlingTime,
                                 termination,
                                 spmuTask );
  }
  else
  {
    throw Error("PmuUtil::pmuCurrentMeasurement",
                "Unknown measureMode.");
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: PmuUtil::pmuVoltageMeasurement
 *
 * Purpose: execute pmu Voltage Measurement
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::pmuVoltageMeasurement(
  const STRING&         measureMode,                                               
  const STRING&         pinList,
  const TM::DCTEST_MODE& testMode,
  const DOUBLE          forceCurrent,
  const LIMIT&          limit,
  const DOUBLE          spmuClampVoltage,
  const STRING&         preCharge,
  const DOUBLE          prechargeVoltage,
  const DOUBLE          settlingTime,
  const STRING&         relaySwitchMode,
  const STRING&         termination,
  PPMU_MEASURE&         measurePpmu,
  SPMU_TASK&            spmuTask,
  const DOUBLE          ppmuClampLow,
  const DOUBLE          ppmuClampHigh )
{
  if ( measureMode == "PPMUpar" )
  {
    ppmuParallelVoltageMeasurement(pinList,                                 
                                   testMode,
                                   forceCurrent,
                                   limit,
                                   preCharge,
                                   prechargeVoltage,
                                   settlingTime,
                                   relaySwitchMode,
                                   termination,
                                   ppmuClampLow,      
                                   ppmuClampHigh,
                                   measurePpmu);
  }
  else if ( measureMode == "PPMUser" )
  {
    ppmuSerialVoltageMeasurement(pinList,                                
                                 testMode,
                                 forceCurrent,
                                 limit,
                                 preCharge,
                                 prechargeVoltage,
                                 settlingTime,
                                 relaySwitchMode,
                                 termination,
                                 ppmuClampLow,      
                                 ppmuClampHigh,
                                 measurePpmu);
  }
  else if ( measureMode == "SPMUser" )
  {
    spmuSerialVoltageMeasurement(pinList,                                
                                 testMode,
                                 forceCurrent,
                                 limit,
                                 relaySwitchMode,
                                 spmuClampVoltage,
                                 preCharge,
                                 prechargeVoltage,
                                 settlingTime,
                                 termination,
                                 spmuTask );
  }
  else
  {
    throw Error("PmuUtil::pmuVoltageMeasurement",
                "Unknown measureMode");
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: PmuUtil::ppmuParallelCurrentMeasurement
 *
 * Purpose: execute ppmu Parallel Current Measurement
 *
 *----------------------------------------------------------------------*
 * Description:
 * workflow: 
 *   1) IF  precharge THEN 
 *        1.1 PPMU_SETTING for precharge purpose
 *        1.2 PPMU_RELAY switch change without extra settling time
 *        1.3 PPMU_SETTING for measurement with extra settling time 
 *      ELSE
 *        1.1 PPMU_SETTING for measurement without extra settling time
 *        1.2 PPMU_RELAY switch change with extra settling time
 *   2) PPMU_MEASURE measurement
 *   3) PPMU_RELAY switch restore
 *
 * Note:
 *   use Flex-DC
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::ppmuParallelCurrentMeasurement(
  const STRING&         pinList,
  const TM::DCTEST_MODE& testMode,
  const DOUBLE          forceVoltage,
  const LIMIT&          limit,
  const STRING&         preCharge,
  const DOUBLE          prechargeVoltage,
  const DOUBLE          settlingTime,
  const STRING&         relaySwitchMode,
  const STRING&         termination,
  PPMU_MEASURE&         measurePpmu)
{
  PPMU_SETTING settingPpmu;
  TASK_LIST    tasklist;
  DOUBLE currentRange = CommonUtil::getCurrentRange(limit);
  DOUBLE settling = settlingTime + 1; /* 1 PPMU settling time */

  /* 1. determine precharge */
  DOUBLE low, high; 

  limit.getLow(&low);
  limit.getHigh(&high);

  //PPMU_SETTING.min/max always use 'A' for current measurement
  if (!limit.unit().empty() && limit.unit().find(";") == string::npos)
  {
    //need to convert value if the specific default unit is used
    double factor = SI_Value::getDiffValue(limit.unit(),"A");
    low = low * factor;
    high = high * factor;
  }

  settingPpmu.pin(pinList)
             .iRange(currentRange)
             .min(low)
             .max(high)
             .vForce(forceVoltage mV);

  PPMU_SETTING settingPpmuPrecharge;

  if ( preCharge == TURN_ON )
  {
    PmuUtil::ppmuPreCharge(settingPpmuPrecharge, pinList,
                           prechargeVoltage,tasklist);
  }
  else
  {
    tasklist.add(settingPpmu);
  }

  /* 2.relay switch */

  PPMU_RELAY     relayPpmuOn;
  PPMU_RELAY     relayAcOff;
  PPMU_RELAY     relayPm;
  PmuUtil::ppmuRelaySwitch( relayPpmuOn,
                            relayAcOff,
                            relayPm,
                            pinList,
                            preCharge == TURN_ON?0:settling,
                            termination,
                            relaySwitchMode,
                            tasklist);

  /*  3. determine precharge */

  if ( preCharge == TURN_ON )
  {
    settingPpmu.wait(settling ms);
    tasklist.add(settingPpmu);
  }

  /*  4. determine measure */

  /*PVAL or PPF or GPF is defined by TestSuite Flag*/
  measurePpmu.pin(pinList).execMode( testMode );
  tasklist.add(measurePpmu);

  /* 5.relay restore */

  PPMU_RELAY     relayPpmuOff;
  PPMU_RELAY     relayAcOn;
  PPMU_RELAY     relayAc;
  PmuUtil::ppmuRelayRestore( relayPpmuOff,
                             relayAcOn,
                             relayAc,
                             pinList,
                             termination,
                             relaySwitchMode,
                             tasklist);


  /*6.task list execute  */
  tasklist.execute();

}

/*
 *----------------------------------------------------------------------*
 * Routine: PmuUtil::ppmuSerialCurrentMeasurement
 *
 * Purpose: execute ppmu Serial Current Measurement by PPMU_MEASURE
 *
 *----------------------------------------------------------------------*
 * Description:
 * workflow:
 *   1) IF  precharge THEN
 *        PPMU_SETTING for precharge purpose
 *      END-IF
 *   FOR each single pin DO
 *     2) IF  precharge THEN
 *          2.1 PPMU_RELAY switch change without extra settling time 
 *          2.2 PPMU_SETTING for measurement with extra settling time
 *        ELSE     
 *          2.1 PPMU_SETTING for measurement without extra settling time
 *          2.2 PPMU_RELAY switch change with extra settling time       
 *        END-IF
 *     3) PPMU_MEASURE do measurement
 *     4) PPMU_RELAY restore relay status
 *   END-FOR
 *
 * Note:
 *   use Flex-DC API
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::ppmuSerialCurrentMeasurement(const STRING&         pinList,             
                                      const TM::DCTEST_MODE& testMode,
                                      const DOUBLE          forceVoltage,
                                      const LIMIT&          limit,
                                      const STRING&         preCharge,
                                      const DOUBLE          prechargeVoltage,
                                      const DOUBLE          settlingTime,
                                      const STRING&         relaySwitchMode,
                                      const STRING&         termination,
                                      PPMU_MEASURE&         measurePpmu)
{  
  /* 1. PPMU_SETTING for precharge purpose */  
  if ( preCharge == TURN_ON )
  {
    TASK_LIST tasklist;
    PPMU_SETTING settingPpmuPrecharge;
    PmuUtil::ppmuPreCharge(settingPpmuPrecharge,
                           pinList,
                           prechargeVoltage,
                           tasklist);
    tasklist.execute();
  }

  /* FOR each single pin DO */
  DOUBLE currentRange = CommonUtil::getCurrentRange(limit);    
  DOUBLE low, high;
  limit.getLow(&low);
  limit.getHigh(&high);
  if (!limit.unit().empty() && limit.unit().find(";") == string::npos)
  {      
    double factor = SI_Value::getDiffValue(limit.unit(),"A");
    low = low * factor;
    high = high * factor;
  }
  DOUBLE settling = settlingTime + 1;

  const STRING_VECTOR pinVec = 
    PinUtility.expandDigitalPinNamesFromPinList(pinList,TM::ALL_DIGITAL);

#ifdef SERIAL_TEST_WORKAROUND_FOR_PIN_GROUP
  for (STRING_VECTOR::const_iterator pinIt = pinVec.begin();
       pinIt != pinVec.end();
       ++pinIt)
  {
    measurePpmu.pin(*pinIt).execMode(testMode);
  }
#else
  measurePpmu.pin(pinList).execMode(testMode);
#endif

  for (STRING_VECTOR::const_iterator pinIt = pinVec.begin();
       pinIt != pinVec.end();
       ++pinIt)
  {
    PPMU_SETTING settingPpmu;
    settingPpmu.pin((*pinIt))
               .iRange(currentRange)
               .min(low)
               .max(high)
               .vForce(forceVoltage mV);

    /* 2 */
    TASK_LIST tasklist;
    if ( preCharge == TURN_ON )    
    {
      ppmuMeasurementRelaySwitchChange_SinglePin(*pinIt,
                                                 0,
                                                 termination,
                                                 relaySwitchMode);

      TASK_LIST tasklist;
      settingPpmu.wait(settling ms);
      tasklist.add(settingPpmu);
      tasklist.execute();
    }
    else
    {      
      tasklist.add(settingPpmu);
      tasklist.execute();

      ppmuMeasurementRelaySwitchChange_SinglePin(*pinIt,
                                                 settling,
                                                 termination,
                                                 relaySwitchMode);
    }

    /* 3. measure */    
    measurePpmu.execute(*pinIt);

    /* 4. relay restore */
    ppmuMeasurementRelaySwitchRestore_SinglePin(*pinIt,
                                                termination,
                                                relaySwitchMode);
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: PmuUtil::ppmuSerialCurrentMeasurement_vfim
 *
 * Purpose: execute ppmu Serial Current Measurement
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *   use PMU_VFIM
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::ppmuSerialCurrentMeasurement_vfim(
  const STRING&         pinList,
  const TM::DCTEST_MODE& testMode,
  const DOUBLE          forceVoltage,
  const LIMIT&          limit,
  const STRING&         preCharge,
  const DOUBLE          prechargeVoltage,
  const DOUBLE          settlingTime,
  const STRING&         termination,
  PMU_VFIM&             pmuVfim )
{
  DOUBLE currentRange = CommonUtil::getCurrentRange(limit);
  if ( preCharge == TURN_ON )
  {
    pmuVfim.preCharge(prechargeVoltage mV);
  }

  DOUBLE low, high;

  limit.getLow(&low);
  limit.getHigh(&high);

  //PMU_VFIM.min_current/max_current always use 'A' for current measurement
  if (!limit.unit().empty() && limit.unit().find(";") == string::npos)
  {
    //need to convert value if the specific default unit is used
    double factor = SI_Value::getDiffValue(limit.unit(),"A");
    low = low * factor;
    high = high * factor;
  } 

  pmuVfim.vForce(forceVoltage mV)
         .min_current(low)
         .max_current(high)
         .settling(settlingTime ms)
         .iRange(currentRange)
         .mode(termination==TURN_OFF?"NS":"TS")
         /* PVAL or PPF or GPF is defined by TestSuite Flag */
         .execute( testMode) ;         
}

/*
 *----------------------------------------------------------------------*
 * Routine: PmuUtil::spmuSerialCurrentMeasurement
 *
 * Purpose: execute spmu Serial Current Measurement
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *   use SPMU_TASK
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::spmuSerialCurrentMeasurement(
  const STRING&  pinList,
  const TM::DCTEST_MODE& testMode,
  const DOUBLE          forceVoltage,
  const LIMIT&          limit,
  const STRING&         relaySwitchMode,
  const DOUBLE          spmuClampCurrent,
  const STRING&         preCharge,
  const DOUBLE          prechargeVoltage,
  const DOUBLE          settlingTime,
  const STRING&         termination, 
  SPMU_TASK&            spmuTask )
{

  DOUBLE low, high;

  limit.getLow(&low);
  limit.getHigh(&high);

  //SPMU_TASK.min/max always use 'A' for current measurement
  if (!limit.unit().empty() && limit.unit().find(";") == string::npos)
  {
    //need to convert value if the specific default unit is used
    double factor = SI_Value::getDiffValue(limit.unit(),"A");
    low = low * factor;
    high = high * factor;
  }

  STRING relayMode;

  if ( termination == TURN_ON )
  {
    relayMode = "TERM";
  }
  else if ( termination == TURN_OFF )
  {
    if ( (relaySwitchMode == "DEFAULT(BBM)") || (relaySwitchMode == "PARALLEL") )
    {
      relayMode = "NTBBM";
    }
    else if ( relaySwitchMode == "MBB" )
    {
      relayMode = "NTMBB";
    }
    else
    {
      throw Error("PmuUtil::spmuSerialCurrentMeasurement",
                  "InValid relaySwitchMode");
    }
  }
  else
  {
    throw Error("PmuUtil::spmuSerialCurrentMeasurement",
                "Unknown termination mode");
  }

  spmuTask.pin(pinList)
          .max(high).min(low)
          .mode("VFIM")
          .vForce(forceVoltage mV)
          .clamp(spmuClampCurrent uA)
          .execMode("SER")
          .relay(relayMode) 
          .settling(settlingTime ms);

  /* PVAL or PPF or GPF is defined by TestSuite Flag */
  spmuTask.execMode(testMode);
  spmuTask.execute();

}
/*
 *----------------------------------------------------------------------*
 * Routine: PmuUtil::ppmuParallelVoltageMeasurement
 *
 * Purpose: execute ppmu Parallel Voltage Measurement
 *
 *----------------------------------------------------------------------*
 * Description:
 * workflow:
 * <pin-scale>:
 *   1) PPMU_SETTING setup
 *   2) PPMU_CLAMP ON 
 *   3) PPMU_RELAY switch change
 *   4) PPMU_CLAMP OFF
 *   5) PPMU_MEASURE measurement
 *   6) PPMU_RELAY switch restore
 *
 * <single-density>  
 *   1) IF  precharge THEN 
 *        1.1 PPMU_SETTING for precharge purpose
 *        1.2 PPMU_RELAY switch change without extra settling time
 *        1.3 PPMU_SETTING for measurement with extra settling time
 *      ELSE
 *        1.1 PPMU_SETTING for measurement without extra settling time
 *        1.2 PPMU_RELAY switch change with extra settling time
 *   2) PPMU_MEASURE measurement
 *   3) PPMU_RELAY switch restore 
 *
 * Note:
 *   use Flex-DC API
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::ppmuParallelVoltageMeasurement(
  const STRING&         pinList,
  const TM::DCTEST_MODE& testMode,
  const DOUBLE          forceCurrent,
  const LIMIT&          limit,
  const STRING&         preCharge,
  const DOUBLE          prechargeVoltage,
  const DOUBLE          settlingTime,
  const STRING&         relaySwitchMode,
  const STRING&         termination,
  const DOUBLE          ppmuClampLow,
  const DOUBLE          ppmuClampHigh,
  PPMU_MEASURE&         measurePpmu)
{
  PPMU_SETTING   settingPpmu;
  TASK_LIST   tasklist;
  DOUBLE currentRange = fabs(forceCurrent);
  DOUBLE settling = settlingTime + 1; /* 1 PPMU settling time */
  
  PPMU_CLAMP ppmuClampOn;
  PPMU_CLAMP ppmuClampOff;
  

  /*  1. set the clamp (PinScale) */
  DOUBLE low, high, clampLow, clampHigh;

  limit.getLow(&low);
  limit.getHigh(&high);

  //PPMU_SETTING.min/max always use 'V' for voltage measurement
  if (!limit.unit().empty() && limit.unit().find(";") == string::npos)
  {
    //need to convert value if the specific default unit is used
    double factor = SI_Value::getDiffValue(limit.unit(),"V");
    low = low * factor;
    high = high * factor;
  }

  settingPpmu.pin(pinList)
             .iRange(currentRange uA)
             .min(low)
             .max(high)
             .iForce(forceCurrent uA);

  tasklist.add(settingPpmu);
    
  clampLow = ppmuClampLow;
  clampHigh = ppmuClampHigh;
    
  /* if input clamp is 0 (default), then set them as limit high/low */
  if ( fabs(clampLow) < 1e-38)
  {
    clampLow = low;
  }
  if ( fabs(clampHigh) < 1e-38)
  {
    clampHigh = high;
  }
  ppmuClampOn.pin(pinList).status("CLAMP_ON").low(clampLow V).high(clampHigh V);  
  ppmuClampOn.wait(1 ms);
  tasklist.add(ppmuClampOn);
  
  /*  2.relay switch */

  PPMU_RELAY     relayPpmuOn;
  PPMU_RELAY     relayAcOff;
  PPMU_RELAY     relayPm;
  PmuUtil::ppmuRelaySwitch (relayPpmuOn,relayAcOff,relayPm,pinList,
                            0,termination,relaySwitchMode,tasklist);

  /* 3. switch clamp off */
  ppmuClampOff.pin(pinList).status("CLAMP_OFF");
  ppmuClampOff.wait(settling ms);
  tasklist.add(ppmuClampOff);
  
  /*  4. determine measure */
  /* PVAL or PPF or GPF is defined by TestSuite Flag */
  measurePpmu.pin(pinList).execMode(  testMode );
  tasklist.add(measurePpmu);

  /* 5.relay restore */

  PPMU_RELAY     relayPpmuOff;
  PPMU_RELAY     relayAcOn;
  PPMU_RELAY     relayAc;
  PmuUtil::ppmuRelayRestore(relayPpmuOff,
                            relayAcOn,
                            relayAc,
                            pinList,
                            termination,
                            relaySwitchMode,
                            tasklist);

  /*  6.task list execute */
  tasklist.execute();
}

/*
 *----------------------------------------------------------------------*
 * Routine: PmuUtil::ppmuSerialVoltageMeasurement
 *
 * Purpose: execute ppmu serial pin Voltage Measurement
 *
 *----------------------------------------------------------------------*
 * Description:
 * workflow: 
 * <pin-scale>:
 *   1) PPMU_SETTING setup
 *   2) PPMU_CLAMP ON 
 *   FOR each pin DO
 *      3) PPMU_RELAY switch change
 *      4) PPMU_CLAMP OFF
 *      5) PPMU_MEASURE measurement
 *      6) PPMU_RELAY switch restore
 *   END-FOR
 *   
 * <single-density>  
 *   IF  precharge THEN 
 *     1) PPMU_SETTING for precharge purpose
 *     2) PPMU_SETTING for measurement with extra settling time
 *     FOR each pin DO
 *       3) PPMU_RELAY switch change without extra settling time           
 *       4) PPMU_MEASURE do measurement            
 *       5) PPMU_RELAY switch restore           
 *     END-FOR
 *   ELSE 
 *     1) PPMU_SETTING for measurement without extra settling time
 *     FOR each pin DO
 *       2) PPMU_RELAY switch change with extra settling time
 *       3) PPMU_MEASURE do measurement
 *       4) PPMU_RELAY switch restore
 *     END-FOR
 *
 * Note:
 *   use Flex-DC
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::ppmuSerialVoltageMeasurement(const STRING&    pinList,
                                      const TM::DCTEST_MODE& testMode,
                                      const DOUBLE     forceCurrent,
                                      const LIMIT&     limit,
                                      const STRING&    preCharge,
                                      const DOUBLE     prechargeVoltage,
                                      const DOUBLE     settlingTime,
                                      const STRING&    relaySwitchMode,
                                      const STRING&    termination,
                                      const DOUBLE     ppmuClampLow,
                                      const DOUBLE     ppmuClampHigh,
                                      PPMU_MEASURE&    measurePpmu)
{
  PPMU_SETTING   settingPpmu;
  TASK_LIST   tasklist;
  DOUBLE currentRange = fabs(forceCurrent);
  DOUBLE settling = settlingTime + 1; /* 1 PPMU settling time */
  
  /*  1. determine precharge (P/CE) or set the clamp (PinScale) */
  DOUBLE low, high, clampLow, clampHigh;
  limit.getLow(&low);
  limit.getHigh(&high);

  //PPMU_SETTING.min/max always use 'V' for voltage measurement
  if (!limit.unit().empty() && limit.unit().find(";") == string::npos)
  {
    //need to convert value if the specific default unit is used
    double factor = SI_Value::getDiffValue(limit.unit(),"V");
    low = low * factor;
    high = high * factor;
  }

  settingPpmu.pin(pinList)
             .iRange(currentRange uA)
             .min(low)
             .max(high)
             .iForce(forceCurrent uA);

  PPMU_CLAMP ppmuClampOn;

  tasklist.add(settingPpmu);
  clampLow = ppmuClampLow;
  clampHigh = ppmuClampHigh;

  /* if input clamp is 0 (default), then set them as limit high/low */
  if ( fabs(clampLow) < 1e-38)
  {
    clampLow = low;
  }
  if ( fabs(clampHigh) < 1e-38)
  {
    clampHigh = high;
  }
  ppmuClampOn.pin(pinList).status("CLAMP_ON").low(clampLow V).high(clampHigh V);
  ppmuClampOn.wait(1 ms);
  tasklist.add(ppmuClampOn);    
  
  /*execute setup condition for all specified pins*/
  tasklist.execute();
 
  const STRING_VECTOR pinVec = 
    PinUtility.expandDigitalPinNamesFromPinList(pinList,TM::ALL_DIGITAL);

#ifdef SERIAL_TEST_WORKAROUND_FOR_PIN_GROUP
  for (STRING_VECTOR::const_iterator pinIt = pinVec.begin();
       pinIt != pinVec.end();
       ++pinIt)
  {
    measurePpmu.pin(*pinIt).execMode(testMode);
  }
#else
  measurePpmu.pin(pinList).execMode(testMode);
#endif

  /* turn off clamp before execution */
  PPMU_CLAMP ppmuClampOff;
  ppmuClampOff.pin(pinList).status("CLAMP_OFF");
  ppmuClampOff.wait(settling ms);

  TASK_LIST clampTaskOnly;
  clampTaskOnly.add(ppmuClampOff);
  clampTaskOnly.execute();

  /* switch relay and measure for each pin */
  for (STRING_VECTOR::const_iterator pinIt = pinVec.begin();
       pinIt != pinVec.end();
       ++pinIt)
  {
    /* switch relay for single pin */
    ppmuMeasurementRelaySwitchChange_SinglePin(*pinIt,
                                               preCharge == TURN_ON?0:settling,
                                               termination,
                                               relaySwitchMode);

    /* do measurement for single pin */
    measurePpmu.execute(*pinIt);

    /* restore relay for single pin */
    ppmuMeasurementRelaySwitchRestore_SinglePin(*pinIt,
                                                termination,
                                                relaySwitchMode);
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: PmuUtil::ppmuSerialVoltageMeasurement_ifvm
 *
 * Purpose: execute ppmu Serial Voltage Measurement
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *   use PMU_IFVM
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::ppmuSerialVoltageMeasurement_ifvm(
  const STRING&         pinList,                                                     
  const TM::DCTEST_MODE& testMode,
  const DOUBLE          forceCurrent,
  const LIMIT&          limit,
  const STRING&         preCharge,
  const DOUBLE          prechargeVoltage,
  const DOUBLE          settlingTime,
  const STRING&         termination,
  PMU_IFVM&             pmuIfvm )

{
  DOUBLE currentRange = fabs(forceCurrent);
  if ( preCharge == TURN_ON )
  {
    pmuIfvm.preCharge(prechargeVoltage mV);
  }

  DOUBLE low, high;

  limit.getLow(&low);
  limit.getHigh(&high);
  //PMU_IFVM.min_voltage/max_voltage always use 'V' for voltage measurement
  if (!limit.unit().empty() && limit.unit().find(";") == string::npos)
  {
    //need to convert value if the specific default unit is used
    double factor = SI_Value::getDiffValue(limit.unit(),"V");
    low = low * factor;
    high = high * factor;
  }

  pmuIfvm.iForce(forceCurrent uA)
         .min_voltage(low)
         .max_voltage(high)
         .settling(settlingTime ms)
         .iRange(currentRange uA)
         .mode(termination==TURN_OFF?"NS":"TS") 
         /* PVAL or PPF or GPF is defined by TestSuite Flag */
         .execute( (CommonUtil::getMode()) );         

}
/*
 *----------------------------------------------------------------------*
 * Routine: PmuUtil::spmuSerialVoltageMeasurement
 *
 * Purpose: execute spmu Serial Voltage Measurement
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *   use SPMU_TASK for all pins in serial measurement mode
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::spmuSerialVoltageMeasurement(
  const STRING&         pinList,
  const TM::DCTEST_MODE& testMode,
  const DOUBLE          forceCurrent,
  const LIMIT&          limit,
  const STRING&         relaySwitchMode,
  const DOUBLE          spmuClampVoltage,
  const STRING&         preCharge,        
  const DOUBLE          prechargeVoltage,
  const DOUBLE          settlingTime,
  const STRING&         termination,  
  SPMU_TASK&            spmuTask )

{
  DOUBLE low, high;

  limit.getLow(&low);
  limit.getHigh(&high);
  //SPMU_TASK.min/max always use 'V' for voltage measurement
  if (!limit.unit().empty() && limit.unit().find(";") == string::npos)
  {
    //need to convert value if the specific default unit is used
    double factor = SI_Value::getDiffValue(limit.unit(),"V");
    low = low * factor;
    high = high * factor;
  }

  STRING relayMode;

  if ( termination == TURN_ON )
  {
    relayMode = "TERM";
  }
  else if ( termination == TURN_OFF )
  {
    if ( (relaySwitchMode == "DEFAULT(BBM)") || (relaySwitchMode == "PARALLEL") )
    {
      relayMode = "NTBBM";
    }
    else if ( relaySwitchMode == "MBB" )
    {
      relayMode = "NTMBB";
    }
    else
    {
      throw Error("PmuUtil::spmuSerialVoltageMeasurement",
                  "InValid relaySwitchMode");
    }
  }
  else
  {
    throw Error("PmuUtil::spmuSerialVoltageMeasurement",
                "Unknown termination mode");
  }

  spmuTask.pin(pinList)
          .max(high)
          .min(low)
          .mode("IFVM")
          .iForce(forceCurrent uA)
          .clamp(spmuClampVoltage mV)
          .execMode("SER")
          .relay( relayMode) 
          .settling(settlingTime ms);

  /* PVAL or PPF or GPF is defined by TestSuite Flag */
  spmuTask.execMode(testMode);
  spmuTask.execute();
}

/*
 *----------------------------------------------------------------------*
 * Routine: PmuUtil::boardADCVoltageMeasurement
 *
 * Purpose: execute Voltage Measurement via Board ADC
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *   use SPMU_TASK in 'VM_HIZ' mode
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::boardADCVoltageMeasurement(
  const STRING& pinList,
  const TM::DCTEST_MODE& testMode,
  const DOUBLE  forceCurrent,
  const LIMIT&  limit,
  const STRING&  relaySwitchMode,
  const STRING&  preCharge,
  const DOUBLE  prechargeVoltage,
  const DOUBLE  settlingTime,
  const STRING&  termination,
  const bool isPinParallelExec, 
  SPMU_TASK&  spmuTask)
{
  DOUBLE low, high;

  limit.getLow(&low);
  limit.getHigh(&high);
  //SPMU_TASK.min/max always use 'V' for voltage measurement
  if (!limit.unit().empty() && limit.unit().find(";") == string::npos)
  {
    //need to convert value if the specific default unit is used
    double factor = SI_Value::getDiffValue(limit.unit(),"V");
    low = low * factor;
    high = high * factor;
  }

  STRING relayMode;

  if ( termination == TURN_ON )
  {
    relayMode = "TERM";
  }
  else if ( termination == TURN_OFF )
  {
    if ((relaySwitchMode == "DEFAULT(BBM)") || (relaySwitchMode == "PARALLEL"))
    {
      relayMode = "NTBBM";
    }
    else if ( relaySwitchMode == "MBB" )
    {
      relayMode = "NTMBB";
    }
    else
    {
      throw Error("PmuUtil::boardADCVoltageMeasurement",
                  "InValid relaySwitchMode");
    }
  }
  else
  {
    throw Error("PmuUtil::boardADCVoltageMeasurement",
                "Unknown termination mode");
  }

  string pinSerParaExec = isPinParallelExec?"PAR":"SER";
  spmuTask.pin(pinList)
          .max(high)
          .min(low)
          .mode("VM_HIZ") 
          .iForce(forceCurrent uA)
          .execMode(pinSerParaExec)
          .relay(relayMode) 
          .settling(settlingTime ms);

  /* PVAL or PPF or GPF is defined by TestSuite Flag */
  spmuTask.execMode(testMode);
  spmuTask.execute();
}

/*
 *----------------------------------------------------------------------*
 * Routine: PmuUtil::pmuCurrentGetResult
 *
 * Purpose: Get pmu current Result
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::pmuCurrentGetResult(
  const STRING&          measureMode,
  const STRING_VECTOR&   pinList, 
  const TM::DCTEST_MODE& testMode,        
  const PPMU_MEASURE&    measurePpmu,
  const SPMU_TASK&       spmuTask,
  MeasurementResultContainer &measureResult)
{
  if ( measureMode == "PPMUpar" )
  {
    ppmuParallelCurrentGetResult(
                                pinList,
                                testMode,
                                measurePpmu,
                                measureResult
                                );
  }
  else if ( measureMode == "PPMUser" )
  {
    ppmuSerialCurrentGetResult(
                              pinList,
                              testMode,
                              measurePpmu,
                              measureResult
                               );
  }
  else if ( measureMode == "SPMUser" )
  {
    spmuSerialCurrentGetResult(
                              pinList,
                              testMode,
                              spmuTask,
                              measureResult
                              );
  }
  else
  {
    throw Error ("PmuUtil::pmuCurrentGetResult",
                 "Unkown measureMode");
  }

}

/*
 *----------------------------------------------------------------------------------*
 * Routine: PmuUtil::ppmuParallelCurrentGetResult
 *
 * Purpose: Get ppmu Parallel Current Result 
 *          and fill it into MeasurementResultContainer
 *
 *----------------------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *
 *----------------------------------------------------------------------------------*
 */
inline void 
PmuUtil::ppmuParallelCurrentGetResult(
  const STRING_VECTOR&   pinList,                                                     
  const TM::DCTEST_MODE& testMode,
  const PPMU_MEASURE&    measurePpmu, 
  MeasurementResultContainer &measureResult)                                                    
{
  STRING_VECTOR::size_type  j = 0;
  STRING pinListStr = "";
  Boolean isPass = TRUE;

  switch ( testMode )
  {
  case TM::PVAL:
    for ( j = 0; j < pinList.size(); ++j )
    {
      isPass = measurePpmu.getPassFail(pinList[j]);
      DOUBLE dMeasValue = (measurePpmu.getValue(pinList[j])); /* [A] */
      measureResult.setPinPassFail(pinList[j],isPass);
      measureResult.setPinsValue(pinList[j],dMeasValue); 

    }
    break;

  case TM::PPF:
    for ( j = 0; j < pinList.size(); ++j )
    {
      isPass = measurePpmu.getPassFail(pinList[j]);
      measureResult.setPinPassFail(pinList[j],isPass);

    }
    break;

  case TM::GPF:
    isPass = measurePpmu.getPassFail();
    measureResult.setGlobalPassFail(isPass);


    break;

  default:
    throw Error("PmuUtil::ppmuParallelCurrentGetResult",
                "Unknown Measure Mode");
  }

}

/*
 *----------------------------------------------------------------------------------*
 * Routine: PmuUtil::ppmuSerialCurrentGetResult
 *
 * Purpose: Get ppmu Serial Current Result and fill it into MeasurementResultContainer
 *
 *----------------------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *   invoke PmuUtil::ppmuParallelCurrentGetResult()
 *----------------------------------------------------------------------------------*
 */
inline void 
PmuUtil::ppmuSerialCurrentGetResult(
  const STRING_VECTOR&   pinList,
  const TM::DCTEST_MODE& testMode,
  const PPMU_MEASURE&    measurePpmu, 
  MeasurementResultContainer &measureResult)
{
  if (testMode != TM::GPF && testMode != TM::PPF && testMode != TM::PVAL)
  {
    throw Error("PmuUtil::ppmuSerialCurrentGetResult","Unknown Measure Mode"); 
  }
  
  ppmuParallelCurrentGetResult(pinList,testMode,measurePpmu,measureResult);
}

/*
 *----------------------------------------------------------------------*
 * Routine: PmuUtil::ppmuSerialCurrentGetResult_vfim
 *
 * Purpose: Get ppmu Serial Current Result
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::ppmuSerialCurrentGetResult_vfim(
  const STRING_VECTOR&   pinList,
  const TM::DCTEST_MODE& testMode,
  const PMU_VFIM&        pmuVfim,
  MeasurementResultContainer &measureResult)
{
  STRING_VECTOR::size_type j = 0;
  STRING pinListStr = "";
  Boolean isPass = TRUE;

  switch ( testMode )
  {
  case TM::PVAL:
    for ( j = 0; j < pinList.size(); ++j )
    {
      DOUBLE dMeasValue = pmuVfim.getValue(pinList[j]); /* [uA]*/
      dMeasValue = dMeasValue * 1e-6; /* uA -> A*/
      isPass = pmuVfim.getPassFail(pinList[j]);
      measureResult.setPinsValue(pinList[j], dMeasValue); 
      measureResult.setPinPassFail(pinList[j], isPass);

    }
    break;

  case TM::PPF:
    for ( j = 0; j < pinList.size(); ++j )
    {
      isPass = pmuVfim.getPassFail(pinList[j]);
      measureResult.setPinPassFail(pinList[j],isPass);
    }
    break;

  case TM::GPF:

    isPass = pmuVfim.getPassFail();
    measureResult.setGlobalPassFail(isPass);

    break;

  default:
    throw Error("PmuUtil::ppmuSerialCurrentGetResult_vfim",
                "Unknown Measure Mode");
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: PmuUtil::spmuSerialCurrentGetResult
 *
 * Purpose: Get spmu Serial Current Result
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 * 
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::spmuSerialCurrentGetResult(
  const STRING_VECTOR&    pinList,                                                    
  const TM::DCTEST_MODE&  testMode,
  const SPMU_TASK&        spmuTask,
  MeasurementResultContainer &measureResult)
                                                    
{
  STRING_VECTOR::size_type j = 0;
  Boolean isPass = TRUE;

  switch ( testMode )
  {
  case TM::PVAL:
    for ( j = 0; j < pinList.size(); ++j )
    {
      DOUBLE dMeasValue = spmuTask.getValue(pinList[j]);
      isPass = spmuTask.getPassFail(pinList[j]);
      measureResult.setPinsValue(pinList[j], dMeasValue);
      measureResult.setPinPassFail(pinList[j],isPass);       
    }
    break;

  case TM::PPF:
    for ( j = 0; j < pinList.size(); ++j )
    {
      isPass = spmuTask.getPassFail(pinList[j]);
      measureResult.setPinPassFail(pinList[j], isPass);
    }
    break;

  case TM::GPF:
    isPass = spmuTask.getPassFail();
    measureResult.setGlobalPassFail(isPass);
    break;

  default:
    throw Error("PmuUtil::spmuSerialCurrentGetResult",
                "Unknown Measure Mode");
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: PmuUtil::pmuVoltageGetResult
 *
 * Purpose: Get pmu voltage Result for PPMU,SPMU
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::pmuVoltageGetResult(
  const STRING&          measureMode,                                             
  const STRING_VECTOR&   pinList,
  const TM::DCTEST_MODE& testMode,
  const PPMU_MEASURE&    measurePpmu,
  const SPMU_TASK&       spmuTask,
  MeasurementResultContainer &measureResult)
                                             
{
  if ( measureMode == "PPMUpar" )
  {
    ppmuParallelVoltageGetResult(
                                pinList,
                                testMode,
                                measurePpmu,
                                measureResult
                                );
  }
  else if ( measureMode == "PPMUser" )
  {
    ppmuSerialVoltageGetResult(
                              pinList,
                              testMode,
                              measurePpmu,
                              measureResult
                              );
  }
  else if ( measureMode == "SPMUser" )
  {
    spmuSerialVoltageGetResult(
                              pinList,
                              testMode,
                              spmuTask,
                              measureResult
                              );
  }
  else
  {
    throw Error ("PmuUtil::pmuVoltageGetResult",
                 "Unkown measureMode");
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: PmuUtil::ppmuParallelVoltageGetResult
 *
 * Purpose: ppmu Parallel Voltage Get Result
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::ppmuParallelVoltageGetResult(
  const STRING_VECTOR&     pinList,
  const TM::DCTEST_MODE&   testMode,
  const PPMU_MEASURE&      measurePpmu,
  MeasurementResultContainer& measureResult)
                                                      
{
  STRING_VECTOR::size_type j = 0;
  Boolean isPass = TRUE;

  switch ( testMode )
  {
  case TM::PVAL:
    for ( j = 0; j< pinList.size(); ++j )
    {
      DOUBLE dMeasValue = (measurePpmu.getValue(pinList[j])); /*[V]*/
      isPass = measurePpmu.getPassFail(pinList[j]);
      measureResult.setPinsValue(pinList[j], dMeasValue); 
      measureResult.setPinPassFail(pinList[j], isPass);

    }
    break;

  case TM::PPF:
    for ( j = 0; j < pinList.size(); ++j )
    {
      isPass = measurePpmu.getPassFail(pinList[j]);
      measureResult.setPinPassFail(pinList[j], isPass);
    }
    break;

  case TM::GPF:
    isPass = measurePpmu.getPassFail();
    measureResult.setGlobalPassFail(isPass);
    break;

  default:
    throw Error("PmuUtil::ppmuParallelVoltageGetResult",
                "Unknown Test Mode");
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: PmuUtil::ppmuSerialVoltageGetResult
 *
 * Purpose: ppmu Serial Voltage Get Result
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *   invoke PmuUtil::ppmuParallelVoltageGetResult()
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::ppmuSerialVoltageGetResult(
  const STRING_VECTOR&     pinList,
  const TM::DCTEST_MODE&   testMode,
  const PPMU_MEASURE&      measurePpmu,
  MeasurementResultContainer& measureResult)
                                                      
{
  if (testMode != TM::GPF && testMode != TM::PPF && testMode != TM::PVAL)
  {
    throw Error("PmuUtil::ppmuSerialVoltageGetResult","Unknown Measure Mode"); 
   }
   
  ppmuParallelVoltageGetResult(pinList,testMode,measurePpmu,measureResult);
}

/*
 *----------------------------------------------------------------------*
 * Routine: ppmuSerialVoltageGetResult_ifvm
 *
 * Purpose: Get ppmu Serial Voltage Result
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::ppmuSerialVoltageGetResult_ifvm(
  const STRING_VECTOR&     pinList,
  const TM::DCTEST_MODE&   testMode,
  const PMU_IFVM&          pmuIfvm,
  MeasurementResultContainer& measureResult)
{
  STRING_VECTOR::size_type j = 0;
  Boolean isPass = TRUE;

  switch ( testMode )
  {
  case TM::PVAL:
    for ( j = 0; j < pinList.size(); ++j )
    {
      DOUBLE dMeasValue = pmuIfvm.getValue(pinList[j]); /* [mV] */
      dMeasValue = dMeasValue * 1e-3; /* mV -> V*/
      isPass = pmuIfvm.getPassFail(pinList[j]);
      measureResult.setPinsValue(pinList[j], dMeasValue);
      measureResult.setPinPassFail(pinList[j],isPass);
    }
    break;

  case TM::PPF:
    for ( j = 0; j < pinList.size(); ++j )
    {
      measureResult.setPinPassFail(pinList[j],pmuIfvm.getPassFail(pinList[j]));
    }
    break;

  case TM::GPF:
    measureResult.setGlobalPassFail(pmuIfvm.getPassFail());
    break;

  default:
    throw Error("PmuUtil::ppmuSerialVoltageGetResult_ifvm",
                "Unknown Test Mode");
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: PmuUtil::spmuVoltageGetResult
 *
 * Purpose: Get spmu Voltage Result
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *   
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::spmuVoltageGetResult(
  const STRING_VECTOR&     pinList,               
  const TM::DCTEST_MODE&   testMode,
  const SPMU_TASK&         spmuTask,
  MeasurementResultContainer& measureResult)
{
  STRING_VECTOR::size_type j = 0;
  Boolean isPass = TRUE;

  switch ( testMode )
  {
  case TM::PVAL:
    for ( j = 0; j < pinList.size(); ++j )
    {
      DOUBLE dMeasValue = spmuTask.getValue(pinList[j]);
      isPass = spmuTask.getPassFail(pinList[j]);
      measureResult.setPinsValue(pinList[j],dMeasValue);
      measureResult.setPinPassFail(pinList[j], isPass);
    }
    break;

  case TM::PPF:
    for ( j = 0; j < pinList.size(); ++j )
    {
      isPass = spmuTask.getPassFail(pinList[j]);
      measureResult.setPinPassFail(pinList[j], isPass);      
    }
    break;

  case TM::GPF:
    isPass = spmuTask.getPassFail();
    measureResult.setGlobalPassFail(isPass); 
    break;

  default:
    throw Error("PmuUtil::spmuSerialVoltageGetResult",
                "Unknown Measure Mode");
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: PmuUtil::spmuSerialVoltageGetResult
 *
 * Purpose: Get spmu Serial Voltage Result
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::spmuSerialVoltageGetResult(
  const STRING_VECTOR&     pinList,                                                    
  const TM::DCTEST_MODE&   testMode,
  const SPMU_TASK&         spmuTask,
  MeasurementResultContainer& measureResult)
{
  spmuVoltageGetResult(pinList,testMode,spmuTask,measureResult);
}

/*
 *----------------------------------------------------------------------*
 * Routine: ppmuRelaySwitch
 *
 * Purpose: use Flex-DC API to handle relay swith
 *
 *----------------------------------------------------------------------*
 * Description:
 *   
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::ppmuRelaySwitch (
  PPMU_RELAY&     relayPpmuOn,                                          
  PPMU_RELAY&     relayAcOff,
  PPMU_RELAY&     relayPm,
  const STRING&   pinList,
  const DOUBLE    settlingTime,
  const STRING&   termination,
  const STRING&   relaySwitchMode,
  TASK_LIST&      tasklist)
{
  relayAcOff.pin(pinList).status("OAC_OFF");
  relayPpmuOn.pin(pinList).status("OPM_ON");

  if ( termination == TURN_OFF )
  {
    if ( relaySwitchMode == "DEFAULT(BBM)" )
    {
      relayAcOff.wait(0.3 ms);
      relayPpmuOn.wait((settlingTime + 0.3) ms);
      tasklist.add(relayAcOff)
              .add(relayPpmuOn);

    }
    else if ( relaySwitchMode == "MBB" )
    {
      relayPpmuOn.wait(0.3 ms);
      relayAcOff.wait((settlingTime + 0.3) ms);
      tasklist.add(relayPpmuOn)
              .add(relayAcOff);

    }
    else if ( relaySwitchMode == "PARALLEL" )
    {
      relayPm.pin(pinList).status("PPMU_ON");
      relayPm.wait((settlingTime+0.3) ms);
      tasklist.add(relayPm);

    }
    else
    {
      throw Error("PmuUtil::ppmuRelaySwitch",
                  "InValid relaySwitchMode");
    }
  }
  else if ( termination == TURN_ON )
  {
    relayPpmuOn.wait((settlingTime + 0.3) ms);
    tasklist.add(relayPpmuOn);
  }
  else
  {
    throw Error ("PmuUtil::ppmuRelaySwitch",
                 "Unknown termination Status");
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: PmuUtil::ppmuRelayRestore
 *
 * Purpose: use Flex-DC API to restore relay
 *
 *----------------------------------------------------------------------*
 * Description:
 *   
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::ppmuRelayRestore(
  PPMU_RELAY&     relayPpmuOff,                                         
  PPMU_RELAY&     relayAcOn,
  PPMU_RELAY&     relayAc,
  const STRING&   pinList,
  const STRING&   termination,
  const STRING&   relaySwitchMode,
  TASK_LIST&      tasklist)
{
  relayPpmuOff.pin(pinList).status("OPM_OFF");
  relayPpmuOff.wait(0.3 ms);
  relayAcOn.pin(pinList).status("OAC_ON");
  relayAcOn.wait(0.3 ms);
  if ( termination == TURN_OFF )
  {

    if ( relaySwitchMode == "DEFAULT(BBM)" )
    {
      tasklist.add(relayPpmuOff).add(relayAcOn);
    }
    else if ( relaySwitchMode == "MBB" )
    {
      tasklist.add(relayAcOn).add(relayPpmuOff);
    }
    else if ( relaySwitchMode == "PARALLEL" )
    {
      relayAc.pin(pinList).status("AC_ON");
      relayAc.wait(0.3 ms);
      tasklist.add(relayAc);
    }
    else
    {
      throw Error ("PmuUtil::ppmuRelayRestore",
                   "Unknown Switch Mode");
    }
  }
  else if ( termination == TURN_ON )
  {
    tasklist.add(relayPpmuOff);
  }
  else
  {
    throw Error ("PmuUtil::ppmuRelayRestore",
                 "Unknown Termination Status");
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: PmuUtil::ppmuPreCharge
 *
 * Purpose: use PPMU_SETTING API to set ppmu voltage preCharge
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::ppmuPreCharge(
  PPMU_SETTING&   settingPpmuPrecharge,
  const STRING&   pinList,                                       
  const DOUBLE    prechargeVoltage,
  TASK_LIST&      tasklist)
{
  /* min and max have no influence on precharge */
  settingPpmuPrecharge.pin(pinList)
                      .iRange(1 uA)
                      .min(-1 uA)
                      .max(1 uA)
                      .vForce(prechargeVoltage mV);
  settingPpmuPrecharge.wait(1 ms);
  tasklist.add(settingPpmuPrecharge);
}

/*
 *----------------------------------------------------------------------*
 * Routine: PmuUtil::ppmuMeasurementRelaySwitchChange_SinglePin
 *
 * Purpose: using FLEX_RELAY to switch PMU and AC relay.
 *
 *----------------------------------------------------------------------*
 * Description:
 *   support relay swith mode: DEFAULT(BBM)|MBB|PARALLEL
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::ppmuMeasurementRelaySwitchChange_SinglePin(
  const STRING&   singlePinName,
  const DOUBLE    settlingTime,
  const STRING&   termination,
  const STRING&   relaySwitchMode)
{
  TASK_LIST relayList;
  FLEX_RELAY step1;
  FLEX_RELAY step2;

  if ( termination == TURN_OFF )
  {
    if ( relaySwitchMode == "DEFAULT(BBM)" )
    {
      /*open AC -> close PMU*/
      step1.pin(singlePinName).set("IDLE");
      step1.wait(0.3 ms);
      step2.pin(singlePinName).set("PPMU");
      step2.wait((settlingTime + 0.3) ms);      

      relayList.add(step1).add(step2);
    }
    else if ( relaySwitchMode == "MBB" )
    {
      /*close PMU/AC -> open AC*/
      step1.pin(singlePinName).set("ACPM");
      step1.wait(0.3 ms);
      step2.pin(singlePinName).set("PPMU");
      step2.wait((settlingTime + 0.3) ms);

      relayList.add(step1).add(step2);
    }
    else if ( relaySwitchMode == "PARALLEL" )
    {
      /*close PMU/open AC*/
      step1.pin(singlePinName).set("PPMU");
      step1.wait((settlingTime + 0.3) ms);
      relayList.add(step1);
    }
    else
    {
      throw Error("PmuUtil::ppmuMeasurementRelaySwitchChange_SinglePin",
                  "InValid relaySwitchMode");
    }
  }
  else if ( termination == TURN_ON )
  {
    /*close PMU/AC*/
    step1.pin(singlePinName).set("ACPM");
    step1.wait((settlingTime + 0.3) ms);
    relayList.add(step1);    
  }
  else
  {
    throw Error ("PmuUtil::ppmuMeasurementRelaySwitchChange_SinglePin",
                 "Unknown termination Status");
  }
  /*make it happen*/
  relayList.execute();
}

/*
 *----------------------------------------------------------------------*
 * Routine: PmuUtil::ppmuMeasurementRelaySwitchRestore_SinglePin 
 *
 * Purpose: using FLEX_RELAY to restore PMU and AC relay.
 *
 *----------------------------------------------------------------------*
 * Description:
 *   support relay swithc mode: DEFAULT(BBM)|MBB|PARALLEL
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline void 
PmuUtil::ppmuMeasurementRelaySwitchRestore_SinglePin(
  const STRING&   singlePinName,
  const STRING&   termination,
  const STRING&   relaySwitchMode)
{
  TASK_LIST relayList;
  FLEX_RELAY step1;
  FLEX_RELAY step2;
  
  if ( termination == TURN_OFF )
  {
    if ( relaySwitchMode == "DEFAULT(BBM)" )
    {
      /*open PPMU -> close AC*/
      step1.pin(singlePinName).set("IDLE");
      step1.wait(0.3 ms);
      step2.pin(singlePinName).set("AC");
      step2.wait(0.3 ms);

      relayList.add(step1).add(step2);
    }
    else if ( relaySwitchMode == "MBB" )
    {
      /*close AC -> open PPMU*/
      step1.pin(singlePinName).set("ACPM");
      step1.wait(0.3 ms);
      step2.pin(singlePinName).set("AC");
      step2.wait(0.3 ms);

      relayList.add(step1).add(step2);
    }
    else if ( relaySwitchMode == "PARALLEL" )
    {
      /*close AC/open PMU*/
      step1.pin(singlePinName).set("AC");
      step1.wait(0.3 ms);
      relayList.add(step1);
    }
    else
    {
      throw Error("PmuUtil::ppmuMeasurementRelaySwitchRestore_SinglePin",
                  "InValid relaySwitchMode");
    }
  }
  else if ( termination == TURN_ON )
  {
    /*close AC/open PMU*/
    step1.pin(singlePinName).set("AC");
    step1.wait(0.3 ms);
    relayList.add(step1);
  }
  else
  {
    throw Error ("PmuUtil::ppmuMeasurementRelaySwitchRestore_SinglePin",
                 "Unknown termination Status");
  }
  /*make it happen*/
  relayList.execute();
}

#endif
