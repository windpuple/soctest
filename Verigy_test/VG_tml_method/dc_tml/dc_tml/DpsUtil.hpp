#ifndef INCLUDED_DpsUtil
#define INCLUDED_DpsUtil

#include "TestSet_r.h"
#include "CommonUtil.hpp"


/*
*----------------------------------------------------------------------*
* DpsUtil:
*      This class do DPS measurement based on the DPS_TASK.
*      We use it to implement the operating current and standby current
*----------------------------------------------------------------------*
*/

class DpsUtil
{
public:
  /*These elements are defined for DPS status checking. */
  enum DpsStatus
  {
    CONSTANT_CURRENT  = 0x02,
    UNREGULATED       = 0x04,
    OVER_VOLTAGE      = 0x08,
    OVER_POWER_TEMP   = 0x30
  };
  static void currentMeasurement(
                const STRING&           dpsPins,
                const LIMIT&            limit,
                const TM::DCTEST_MODE&  testMode,  
                DPS_TASK&               dpsTask,
                const INT               samples,
                const DOUBLE            waitTime_ms = 0,
                const TM::TrigMode      trigMode = TM::INTERNAL);

  static void currentMeasurement(DPS_TASK&  dpsTask);
  
  static void currentGetResult(
                const STRING_VECTOR&    dpsPins,                              
                const TM::DCTEST_MODE&  mode,
                const DPS_TASK&         dpsTask,
                MeasurementResultContainer& measureResult,
                const STRING&           gangedMode = TURN_OFF);

  static void setDpsTask(
                const STRING&           dpsPins,
                const LIMIT&            limit,
                const INT&              samples,
                const STRING&           gangedMode,
                const TM::DCTEST_MODE&  testMode,
                DPS_TASK&               dpsTask,
                const DOUBLE            waitTime_ms = 0,
                const TM::TrigMode      trigMode = TM::INTERNAL);

  static void switchDpsStateToLOZ(STRING& fwCmd);

  static void restoreDpsStateFromLozToHIZ(STRING& fwCmd);

};

/* 
 *----------------------------------------------------------------------* 
 * Routine: DpsUtil::currentMeasurement
 *
 * Purpose: execute Current Measurement by using DPS_TASK
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline void DpsUtil::currentMeasurement(
                const STRING&          dpsPins,
                const LIMIT&           limit,
                const TM::DCTEST_MODE& testMode,  /*"PVAL","PPF" or "GPF"*/
                DPS_TASK&              dpsTask,
                const INT              samples,
                const DOUBLE           waitTime_ms,
                const TM::TrigMode     trigMode        /*default is TM::INTERNAL*/
                )
{
  double lowVal;
  double highVal;
  limit.getLow(&lowVal);
  limit.getHigh(&highVal);
  if (!limit.unit().empty() && limit.unit().find(";") == string::npos)
  {
    //LIMIT doesn't convert value if unit() is "uA", 
    //need to convert it here. 
    double factor_LimitUnitToA = SI_Value::getDiffValue(limit.unit(),"A");
    lowVal *= factor_LimitUnitToA; 
    highVal *= factor_LimitUnitToA; 
  }
 
  dpsTask.pin(dpsPins).min(lowVal).max(highVal);
  dpsTask.trigMode(trigMode);
  dpsTask.execMode(testMode);
  dpsTask.samples(samples);
  if (trigMode == TM::INTERNAL && waitTime_ms > 0.0)
  {
    dpsTask.wait(waitTime_ms ms);
  }
  dpsTask.execute();
}
 /*
 *----------------------------------------------------------------------*
 * Routine: DpsUtil::currentMeasurement
 *
 * Purpose: execute Current Measurement by using DPS_TASK
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */

inline void DpsUtil::currentMeasurement( DPS_TASK& dpsTask)   
{
  dpsTask.execute();
}

/*
 *----------------------------------------------------------------------*
 * Routine: DpsUtil::currentGetResult
 *
 * Purpose: get the result
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 *    can be done with ganged current.
 *----------------------------------------------------------------------*
 */
inline void DpsUtil::currentGetResult(
                const STRING_VECTOR&      dpsPins,                                        
                const TM::DCTEST_MODE&    mode,                                       
                const DPS_TASK&           dpsTask,
                MeasurementResultContainer&   measureResult,
                const STRING&             gangedMode
                )
{
  Boolean isPass;
  STRING_VECTOR::const_iterator it;
  double dVal = 0.0;

  if ( gangedMode == TURN_ON )
  { /* for gang mode case */
    for ( it = dpsPins.begin(); it != dpsPins.end(); ++it )
    {
      measureResult.setPinsValue((*it), dpsTask.getValue( *it ));
    }
  }
  else
  { /*for ungang mode*/
    switch ( mode )
    {
    case TM::PVAL:
      for ( it = dpsPins.begin(); it != dpsPins.end(); ++it )
      {
        dVal = dpsTask.getValue( *it );
        isPass = dpsTask.getPassFail( *it );
        measureResult.setPinsValue((*it), dVal);
        measureResult.setPinPassFail((*it),isPass);

      }
      break;
    case TM::PPF:
      for ( it = dpsPins.begin(); it != dpsPins.end(); ++it )
      {
        isPass = dpsTask.getPassFail( *it );
        measureResult.setPinPassFail((*it),isPass);
      }
      break;
    case TM::GPF:
      isPass = dpsTask.getPassFail();
      measureResult.setGlobalPassFail(isPass);
      break;
    default:
      throw Error("DpsUtil::currentGetResult",
                  "Unknown Test Mode");
    }  /*end of switch*/
  } /*end of if-else*/
}

 /*
 *----------------------------------------------------------------------*
 * Routine: DpsUtil::setDpsTask
 *
 * Purpose: set the DpsTask
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */

inline void DpsUtil::setDpsTask(                       
               const STRING&          dpsPins,
               const LIMIT&           limit,
               const INT&             samples,
               const STRING&          gangedMode,
               const TM::DCTEST_MODE& testMode,  /*"PVAL","PPF" or "GPF"*/
               DPS_TASK&              dpsTask,
               const DOUBLE           waitTime_ms,
               const TM::TrigMode     trigMode        /*default is TM::INTERNAL*/
               )
{
  double lowVal;
  double highVal;
  limit.getLow(&lowVal);
  limit.getHigh(&highVal);
  if (!limit.unit().empty() && limit.unit().find(";") == string::npos)
  {
    //LIMIT doesn't convert value if unit() is "uA", 
    //need to convert it here. 
    double factor_LimitUnitToA = SI_Value::getDiffValue(limit.unit(),"A");
    lowVal *= factor_LimitUnitToA;
    highVal *= factor_LimitUnitToA;
  }

  dpsTask.pin(dpsPins).min(lowVal).max(highVal);
  dpsTask.trigMode(trigMode);
  dpsTask.samples(samples);
  
  if (trigMode == TM::INTERNAL && waitTime_ms > 0.0)
  {
    dpsTask.wait(waitTime_ms ms);
  }

  if( gangedMode == TURN_ON )
  { /*In ganged mode, PPF&GPF is retrieved from PVAL too.*/
    dpsTask.execMode(TM::PVAL);
  }
  else
  {
    dpsTask.execMode(testMode);
  }
}


/*
 *----------------------------------------------------------------------*
 * Routine: DpsUtil::switchDpsStateToLOZ
 *
 * Purpose: conditionally change DPS to LOZ state
 *
 *----------------------------------------------------------------------*
 * Description:
 *   (1) check primary setting of ALL DPS pins.       
 *   (2) change the states of ALL 'HIZ' dps pins to 'LOZ' state. 
 * Note:
 *                 
 *----------------------------------------------------------------------*
 */
inline void DpsUtil::switchDpsStateToLOZ(STRING& strFwRestoreToHIZ)
{
  /*query DPS states of primary setting*/
  STRING strFwAnswer = "";
  FW_TASK("PSLV? PRM,(@)\n",strFwAnswer);

  STRING::size_type iCmdHeaderPos       = 0; /*the position of "PSLV"*/
  STRING::size_type iStatePos           = 0; /*the position of "HIZ" or "HIZ_R"*/
  STRING::size_type iCommaAfterStatePos = 0; /*the position of the comma after state*/
  STRING::size_type iPos                = 0; /*the other positions*/
  STRING strFwCmdToLOZ    = "";/*FW string to switch DPS to LOZ state*/

  iCmdHeaderPos = strFwAnswer.find("PSLV");
  while (iCmdHeaderPos != string::npos)
  {
    /*to identify "HIZ" and "HIZ_R"*/
    iStatePos = strFwAnswer.find("HIZ",iCmdHeaderPos);
    iPos   = strFwAnswer.find("(",iCmdHeaderPos);
    /*
     *******************************************************
     * The valid "HIZ" or "HIZ_R" must be before the "(".
     * and identify the special case:
     * "...,LOZ,,(Vcc,HIZ,Vee)\n".
     *******************************************************
     */
    if ( (iStatePos != string::npos) && (iPos > iStatePos) ) 
    {
      iCommaAfterStatePos = strFwAnswer.find(",",iStatePos);
      iPos = strFwAnswer.find(")",iPos+1);
      ++iPos; /*move to "\n"*/

      /*create FW command string of restoring dps state.*/
      strFwRestoreToHIZ += strFwAnswer.substr(iCmdHeaderPos,
                                              iPos-iCmdHeaderPos+1);

      /*create FW command string of switching dps to LOZ.*/
      strFwCmdToLOZ += strFwAnswer.substr(iCmdHeaderPos,
                                          iStatePos-iCmdHeaderPos);
      strFwCmdToLOZ += "LOZ";
      strFwCmdToLOZ += strFwAnswer.substr(iCommaAfterStatePos,
                                          iPos-iCommaAfterStatePos+1);
    }
    /*try to find the next command line*/
    iCmdHeaderPos = strFwAnswer.find("PSLV",iPos+1);
  }
  /*send command if any*/
  if ( !strFwCmdToLOZ.empty() )
  {
    FW_TASK(strFwCmdToLOZ);
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: DpsUtil::RestoreDpsStateFromLoz
 *
 * Purpose: restore DPS to HIZ state
 *
 *----------------------------------------------------------------------*
 * Description:
 *    Based on the existing command, and append updating setup command
 *   and then send FW command. 
 *    
 * Note:
 *                
 *----------------------------------------------------------------------*
 */
inline void DpsUtil::restoreDpsStateFromLozToHIZ(STRING& fwCmd)
{ 
  if ( !fwCmd.empty() ) 
  {
    /*append a string of update the DPS level setup*/
    fwCmd += " UPTD DPS,1\nUPTD LEV,1\n";
    FW_TASK(fwCmd);
  }
}
#endif
