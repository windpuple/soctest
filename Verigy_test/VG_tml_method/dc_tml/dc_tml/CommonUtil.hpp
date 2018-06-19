#ifndef INCLUDED_CommonUtil
#define INCLUDED_CommonUtil

#include <string.h>
#include <stdlib.h>
#include <iomanip>
#include <float.h>
#include <sstream>
using namespace std;

#include "TestSet_r.h"
#include "testmethod.hpp"

const static STRING TURN_ON = "ON";
const static STRING TURN_OFF = "OFF";
const static INT OUTPUT_WIDTH = 10;
const static DOUBLE MAX_DOUBLE_LOCAL = DBL_MAX;
const static DOUBLE MIN_DOUBLE_LOCAL = -DBL_MAX;
const static std::string _TML_VERSION = "version: tml_7.1.4_2.1.6";
const static double MPR_LOG_PF_TEST_PASS = 0;
const static double MPR_LOG_PF_TEST_FAIL = 1;

typedef struct
{
  DOUBLE value;
  bool hasValue;
}DoubleWithFlag;

struct SpecSearchResultData
{
    TM::SPEC_RESULT specResult;
    Boolean isTestPassed;
    DOUBLE passVal;
    DOUBLE failVal;
    void init()
    {
        specResult = TM::AllPass;
        isTestPassed = false;
        passVal = 0.0;
        failVal = 0.0;
    }
    SpecSearchResultData()
    {
       init();
    }
};

class SI_Value : public testmethod::SpecValue
{
private:
  testmethod::SpecValue* mpSIValue;

public:
  SI_Value()
  {
    mpSIValue = new testmethod::SpecValue();
  }

  SI_Value(const double value, const std::string& unit)
  {
    mpSIValue = new testmethod::SpecValue();
    mpSIValue->setBaseUnit(unit);
    std::ostringstream valueunit;
    valueunit << value << "[" << unit << "]";
    mpSIValue->inputValueUnit(valueunit.str());
  }

  ~SI_Value()
  {
    delete mpSIValue;
    mpSIValue = 0;
  }

  void setBaseUnit(const std::string& unit)
  {
    mpSIValue->setBaseUnit(unit);
  }

  const double getValue(const std::string& unit)
  {
    return mpSIValue->getValueAsTargetUnit(unit);
  }
  //diff value = 1[origUni]/1[targetUnit]
  static double getDiffValue(const std::string& origUnit, const std::string& targetUnit)
  {
    if (targetUnit.empty())
    {
      return 1.0;
    }
    testmethod::SpecValue siValue;
    siValue.setBaseUnit(origUnit);
    siValue.inputValueUnit("1["+origUnit+"]");
    return siValue.getValueAsTargetUnit(targetUnit);
  }
};       
/*
 *---------------------------------------------------------------------* 
 *MeasurementResultContainer: 
 *  This class is used to hold and output 
 *  global or per pin result: pass/fail, values. 
 *---------------------------------------------------------------------*  
 *  Notes:
 *  If the results of all sites are cached in Testmethod, not in 
 *  TestmethodAPI, this class can store results for each specific site. 
 *   
 *  Typical use case is to directly use firmware commands to test and get 
 *  result for all sites in Testmethod, then use this class to store and
 *  retrieve result for all sites.
 *---------------------------------------------------------------------* 
 */
class MeasurementResultContainer
{
public:
  MeasurementResultContainer();
  ~MeasurementResultContainer();
  void setGlobalPassFail(Boolean isPass,INT siteNumber = 0 );
  void setPinPassFail(const STRING& pin,Boolean isPass,INT siteNumber = 0);
  void setPinsValue(const STRING& pins,DOUBLE value,INT siteNumber = 0);
  Boolean getGlobalPassFail(INT nSite = 0)const;
  Boolean getPinPassFail(const STRING& pin,INT siteNumber = 0)const;
  DOUBLE getPinsValue(const STRING& pins,INT siteNumber = 0)const;
  DOUBLE getMinValue(INT siteNumber = 0)const;
  DOUBLE getMaxValue(INT siteNumber = 0)const;
  void init();
private:
  
  /*
   *********************************************
   * Record value for pin(s) per site, e.g.
   *   mMultiSitePinsValue[1] = {"I/O1",2.50}
   *   mMultiSitePinsValue[1] = {"I/O0",1.30}
   *   mMultiSitePinsValue[2] = {"I/O1",2.52}
   *   mMultiSitePinsValue[2] = {"I/O0",1.33}
   ********************************************* 
   */
  map< INT,map< STRING,DOUBLE > > mMultiSitePinsValue;

  /*
   *********************************************
   * Record pass/fail for each pin per site, e.g.
   *   TRUE: pass; FALSE: fail.
   *   mMultiSitePinPassFail[1] = {"Vcc",FALSE}
   *   mMultiSitePinPassFail[1] = {"Vee",FALSE}
   *   mMultiSitePinPassFail[2] = {"Vcc",TRUE}
   *   mMultiSitePinPassFail[2] = {"Vee",FALSE}
   ********************************************* 
   */
  map< INT,map< STRING,Boolean > > mMultiSitePinPassFail;
  /*
   ********************************************* 
   * Record global pass/fail per site, e.g.
   *   TRUE: pass; FALSE: fail.
   *   mMultiSiteGlobalPassFail[1] = TRUE
   *   mMultiSiteGlobalPassFail[2] = FALSE
   ********************************************* 
   */
  map< INT, Boolean > mMultiSiteGlobalPassFail;
 
};


/*
 *----------------------------------------------------------------------*
 * Routine: MeasurementResultContainer::MeasurementResultContainer
 *
 * Purpose: constructor for MeasurementResultContainer
 *
 *----------------------------------------------------------------------*
 * Description:
 *   
 * Note:
 *   
 *----------------------------------------------------------------------*
 */
inline MeasurementResultContainer::MeasurementResultContainer()
{
  init();
}
 /*
 *----------------------------------------------------------------------*
 * Routine: MeasurementResultContainer::MeasurementResultContainer
 *
 * Purpose: destructor for MeasurementResultContainer
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline MeasurementResultContainer::~MeasurementResultContainer()
{
}
/*
 *----------------------------------------------------------------------*
 * Routine: MeasurementResultContainer::init
 *
 * Purpose: initialize member data
 *
 *----------------------------------------------------------------------*
 * Description:
 *   
 * Note:
 *   
 *----------------------------------------------------------------------*
 */
inline void MeasurementResultContainer::init()
{
  mMultiSitePinsValue.clear();
  mMultiSitePinPassFail.clear();
  mMultiSiteGlobalPassFail.clear();
}

 /*
 *----------------------------------------------------------------------*
 * Routine: MeasurementResultContainer::setGlobalPassFail
 *
 * Purpose: record global result
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *  
 *
 *----------------------------------------------------------------------*
 */
inline  void MeasurementResultContainer::setGlobalPassFail(
                                       Boolean isPass,
                                       INT siteNumber)
{
  mMultiSiteGlobalPassFail[siteNumber] = isPass;
}
/*
 *----------------------------------------------------------------------*
 * Routine: MeasurementResultContainer::setPinPassFail
 *
 * Purpose: record  result for the specified pin.
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline  void MeasurementResultContainer::setPinPassFail(
                                        const STRING& pin, 
                                        Boolean isPass,
                                        INT siteNumber)
{ 
  mMultiSitePinPassFail[siteNumber][pin]= isPass;
}
/*
 *----------------------------------------------------------------------*
 * Routine: MeasurementResultContainer::setPinsValue
 *
 * Purpose: record  value for the specified pin(s).
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline  void MeasurementResultContainer::setPinsValue(
                                        const STRING& pins, 
                                        DOUBLE value,
                                        INT siteNumber)
{ 
  mMultiSitePinsValue[siteNumber][pins]= value;

}

 /*
 *----------------------------------------------------------------------*
 * Routine: MeasurementResultContainer::getGlobalPassFail
 *
 * Purpose: get global result
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */

inline Boolean MeasurementResultContainer::getGlobalPassFail(INT siteNumber)const
{ 
  map< INT,Boolean >::const_iterator it = 
                      mMultiSiteGlobalPassFail.find(siteNumber);
  if ( it != mMultiSiteGlobalPassFail.end() )
  {
    return it->second;
  }
  else
  {
    throw Error("MeasurementResultContainer::getGlobalPassFail",
                "No global result for this site!");
  }
}
 /*
 *----------------------------------------------------------------------*
 * Routine: MeasurementResultContainer::getPinPassFail
 *
 * Purpose: get the resulf of specified pin
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */

inline Boolean MeasurementResultContainer::getPinPassFail(
                                              const STRING& pin,
                                              INT siteNumber)const
{
  map< INT,map<STRING,Boolean> >::const_iterator it = 
                                  mMultiSitePinPassFail.find(siteNumber);

  /* get the result of the specified site*/
  if ( it != mMultiSitePinPassFail.end() )
  { 
    const map< STRING,Boolean >& tmpPinResult = it->second;
    map< STRING,Boolean >::const_iterator itPinResult = tmpPinResult.find(pin);

    /* get the result of the specified pin */
    if ( itPinResult != tmpPinResult.end() )
    {
      return itPinResult->second;
    }
    else
    {
      throw Error("MeasurementResultContainer::getPinPassFail",
                  "No result for this pin: "+pin+".");
    }
  }
  else
  {
    throw Error("MeasurementResultContainer::getPinPassFail",
                "No per pin result for this site!");
  }
}
 /*
 *----------------------------------------------------------------------*
 * Routine: MeasurementResultContainer::getPinsValue
 *
 * Purpose: get the resulf of specified pin(s)
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */

inline DOUBLE MeasurementResultContainer::getPinsValue(
                                              const STRING& pins,
                                              INT siteNumber)const
{
  map< INT,map<STRING,DOUBLE> >::const_iterator it = 
                                  mMultiSitePinsValue.find(siteNumber);

  /* get the result of the specified site*/
  if ( it != mMultiSitePinsValue.end() )
  { 
    const map< STRING,DOUBLE >& tmpPinResult = it->second;
    map< STRING,DOUBLE >::const_iterator itPinResult = tmpPinResult.find(pins);

    /* get the result of the specified pins */
    if ( itPinResult != tmpPinResult.end() )
    {
      return itPinResult->second;
    }
    else
    {
      throw Error("MeasurementResultContainer::getPinsValue",
                  "No result for this pin(s): "+pins+".");
    }
  }
  else
  {
    throw Error("MeasurementResultContainer::getPinsValue",
                "No result for this site!");
  }
}

 /*
 *----------------------------------------------------------------------*
 * Routine: MeasurementResultContainer::getMinValue
 *
 * Purpose: get minimum value among pins for the specified site
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline DOUBLE MeasurementResultContainer::getMinValue(INT siteNumber)const
{
  map< INT,map<STRING,DOUBLE> >::const_iterator it = 
                                  mMultiSitePinsValue.find(siteNumber);
  DOUBLE minVal = DBL_MAX;
  /* get the result of the specified site*/
  if ( it != mMultiSitePinsValue.end() )
  { 
    const map< STRING,DOUBLE >& tmpPinResult = it->second;
    for (map< STRING,DOUBLE >::const_iterator itPinResult = tmpPinResult.begin();
         itPinResult != tmpPinResult.end();
         ++itPinResult)
    {
      minVal = MIN(minVal,itPinResult->second);   
    }
  }  
  else
  {
    throw Error("MeasurementResultContainer::getMinValue",
                "No result for this site!");
  }
  return minVal;
}
 /*
 *----------------------------------------------------------------------*
 * Routine: MeasurementResultContainer::getMaxValue
 *
 * Purpose: get maximum value among pins for the specified site
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline DOUBLE MeasurementResultContainer::getMaxValue(INT siteNumber)const
{
  map< INT,map<STRING,DOUBLE> >::const_iterator it = 
                                  mMultiSitePinsValue.find(siteNumber);
  DOUBLE maxVal = -DBL_MAX;
  /* get the result of the specified site*/
  if ( it != mMultiSitePinsValue.end() )
  { 
    const map< STRING,DOUBLE >& tmpPinResult = it->second;
    for (map< STRING,DOUBLE >::const_iterator itPinResult = tmpPinResult.begin();
         itPinResult != tmpPinResult.end();
         ++itPinResult)
    {
      maxVal = MAX(maxVal,itPinResult->second);   
    }
  }
  else
  {
    throw Error("MeasurementResultContainer::getMaxValue",
                "No result for this site!");
  }
  return maxVal;
}

/*
 *---------------------------------------------------------------------* 
 * CommonUtil: 
 *  This class provides common fucntions. 
 *               
 *---------------------------------------------------------------------* 
 */

class CommonUtil
{
public:
  struct SmarTestVersion
  {
    int _major;
    int _minor;
    int _quality;
    SmarTestVersion(int _majorIn, int _minorIn, int _qualityIn)
      :_major(_majorIn),_minor(_minorIn),_quality(_qualityIn)
    {

    }
    void dumpVersion()const
    {
      cout<<"---------------"<<endl;
      cout<< "SmarTest version: "<<_major<<"."<<_minor<<"."<<_quality<<endl;
      cout<<"---------------"<<endl;
    } 

    void reset()
    {
      _major = _minor = _quality = 0;
    }

    bool operator >= (const SmarTestVersion& rhd)const
    {
      return ((this->_major - rhd._major)*10000 + 
              (this->_minor - rhd._minor)*100 +
              (this->_quality - rhd._quality) >= 0);
    }

    bool operator < (const SmarTestVersion& rhd)const
    {
      return !(*this >= rhd); 
    }
  };

  static const SmarTestVersion& getCurrentSmarTestVersion();

  static LIMIT getLimit(const STRING& limitName, const STRING& unit);
  
  static DOUBLE getCurrentRange(const LIMIT& limit);
  
  static STRING getTerminatedPins();
  
  static TM::DCTEST_MODE getMode();
  
  static void disconnectPins(const STRING& pinlist);
  
  static void connectPins(const STRING& pinlist);
  
  static void validateAndConvertStrToInt(
                          const STRING& strNum, 
                          ARRAY_I& stopNumber);
  
  static void conditionalConnect(const STRING& connectState);

  static Boolean isToSetSequencerOff(
                const STRING& connectState,
                const STRING& measureMode);

  static void clampSimulation(
                const string& measureMode,
                const double spmuClampVoltage_V, 
                const double limitHighEnd_V, 
                double& forceCurrent_uA);
  
  static void printTestSuiteName(
                const STRING& output,
                const STRING& preMessage = "", 
                const STRING& postMessage = "");
        
  static INT  splitStr (
                const STRING &origStr, 
                const CHAR sepCh,
                STRING_VECTOR &array, 
                const Boolean bSupportBracket = TRUE);
  
  static void datalogToWindow(
                const STRING&         output,
                const STRING&         pin,
                const STRING&         standardUnitOfValue,
                const DOUBLE          measuredValue,
                const STRING&         specificLimitUnit,
                const LIMIT&          limit);

  static void datalogToWindow(
                const STRING&         output,
                const STRING&         pin,
                const Boolean         bPass);

  static void preprocessLimitForContinuityTest(
                 LIMIT &limit,
                 const DOUBLE forceCurrent);

  static void programmableLoadContinuityTest(
                 const STRING_VECTOR&    vecPins,
                 const STRING&           polarity,
                 const LIMIT&            limit,
                 const TM::DCTEST_MODE&  mode,
                 MeasurementResultContainer& continRes,
                 DOUBLE                  forceCurrent);

  static void evaluateMeasurementResult(
                 const STRING_VECTOR&          pins,
                 const TM::DCTEST_MODE &       mode,
                 const MeasurementResultContainer& measResult,
                 const STRING&                 output);
                 
  static DOUBLE string2Double(const STRING& input, const STRING& name);
  static LONG string2Long(const STRING& input,const STRING& name);
  static STRING trim(const STRING& str);
  static void getUnburstLabels(const STRING& strLabel, STRING_VECTOR& unburstLabels);

  static const STRING double2String(DOUBLE number);
  
  static INT splitPortDataAndPortName(
                 const STRING& input, 
                 STRING& portData, 
                 STRING& portName,
                 Boolean throwErrorForNoPortName);
 
 static const STRING convertVectorNumToCycleNum(const STRING& strOfVecNum); 
 static void parseAndRecordPRLTResult(
                 const STRING&           fwAnswer,
                 MeasurementResultContainer& continResult);  
  
 static TM::SPEC_TYPE convertStringToSearchType(const STRING& input);

 static TM::SPEC_METHOD convertStringToMethod(const STRING& input);               

 static const std::string getUnitNameOfLimit(const LIMIT& limit);

private:

  static void printValue(
                 DOUBLE        val, 
                 const STRING& unit, 
                 const Boolean       bFormat);
  
  static void parseAndRecordContinuityResult(
                 const INT                     pinsSize,
                 const TM::DCTEST_MODE&  mode,
                 const STRING&           fwAnswer,
                 MeasurementResultContainer& continResult);

  static void parseAndRecordContinuityResultForPPF(
                 const INT                     pinsSize,
                 const STRING&           fwAnswer,
                 MeasurementResultContainer& continResult);
};

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::getCurrentSmarTestVersion
 *
 * Purpose: query current smartest version (#.#.#)
 *
 *----------------------------------------------------------------------*
 * Description:
 *    retieve current SmarTest version with "major.minor.quality"
 * Note: 
 *
 *----------------------------------------------------------------------*
 */
inline const CommonUtil::SmarTestVersion& 
CommonUtil::getCurrentSmarTestVersion()
{
  static bool isDone = false;
  static SmarTestVersion sLocalSMT(0,0,0);
  if (!isDone)
  {
    std::string answer;
    if (FW_PASSED == FW_TASK("*IDN?",answer))
    {
      //answer could be: "FirmWare s/w rev. 6.3.6 "
      std::string::size_type posStart = answer.find("rev.");
      posStart = answer.find_first_not_of(" \n",posStart+4);
      std::string::size_type posEnd = answer.find_first_of("_, \n",posStart);
      //to get "x.x.x" or "x.x.x.x" string
      std::string numberString = answer.substr(posStart,posEnd-posStart);
      
      std::istringstream ansParser(numberString);
      char dot = '.';

      ansParser >> sLocalSMT._major 
                >> dot 
                >> sLocalSMT._minor 
                >> dot 
                >> sLocalSMT._quality;

      if (!ansParser.fail())
      {
        isDone = true;
      }
      else
      {
        sLocalSMT.reset();
        std::cout << "Error: parse SmarTest Version failed. "<< std::endl;
      }
    }
    else
    {
      std::cout << "Error: query SmarTest Version failed. "<< std::endl;
    }
  }
  return sLocalSMT;
}
/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::getLimit
 *
 * Purpose: copy Limit from TM-Gui into local limit
 *
 *----------------------------------------------------------------------*
 * Description:
 *    throws an ERROR, if limits are NA,
 *    otherwise returns a limit
 *
 * Note: The limit always uses "A" or "V" as its unit.
 *
 *----------------------------------------------------------------------*
 */
inline LIMIT CommonUtil::getLimit(const STRING& limitName, 
                                  const STRING& unit)
{
  LIMIT limit;

  TM::COMPARE lowCmp,highCmp;
  DOUBLE lowVal, highVal;

  GET_LIMIT(limitName.c_str(), lowCmp, lowVal, highCmp, highVal);

  if ( lowCmp == TM::NA || highCmp == TM::NA )
  {
    throw Error("CommonUtil::getLimit",
                limitName + " is not specified");
  }

  if (unit == "uA")
  {
    limit.low(lowCmp, lowVal * 1e-6);
    limit.high(highCmp, highVal * 1e-6);
    limit.unit("A"); 
  }
  else if (unit == "mV")
  {
    limit.low(lowCmp, lowVal * 1e-3);
    limit.high(highCmp, highVal * 1e-3);
    limit.unit("V"); 
  }
  else if (unit == "MHz")
  {
    limit.low(lowCmp, lowVal);
    limit.high(highCmp, highVal);
    limit.unit("MHz"); 
  }
  else
  {
    throw Error("CommonUtil::getLimit ", "unknown unit");
  }

  return limit; 
}
/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::preprocessLimitForContinuityTest
 *
 * Purpose: change the limit according to the forceCurrent.
 *
 *----------------------------------------------------------------------*
 * Description:
 *    Update the limit value according to the polarity of force current.
 *
 * Note: 
 *
 *----------------------------------------------------------------------*
 */
inline void CommonUtil::preprocessLimitForContinuityTest(
                             LIMIT &limit,
                             const DOUBLE forceCurrent)
{
  TM::COMPARE lowCmp,highCmp;
  DOUBLE dLowVal = 0.0, dHighVal = 0.0;
  DOUBLE dUpdatedLow = 0.0, dUpdatedHigh = 0.0;
  limit.get(lowCmp,dLowVal,highCmp,dHighVal);

  if (forceCurrent < 0.0) 
  {  /* negative */
    dUpdatedLow  = MIN(-fabs(dLowVal),-fabs(dHighVal));
    dUpdatedHigh = MAX(-fabs(dLowVal),-fabs(dHighVal));
  }
  else
  {  /* positive */
    dUpdatedLow  = MIN(fabs(dLowVal),fabs(dHighVal));
    dUpdatedHigh = MAX(fabs(dLowVal),fabs(dHighVal));
  }
  /* update the limit */
  limit.high(highCmp,dUpdatedHigh);
  limit.low(lowCmp,dUpdatedLow);
}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::getCurrentRange
 *
 * Purpose: determine current range from limit [A]
 *
 *----------------------------------------------------------------------*
 * Description:
 *    otherwise current range
 * Note:
 *  the unit must be in [A]
 *----------------------------------------------------------------------*
 */
inline DOUBLE  CommonUtil::getCurrentRange(const LIMIT& limit)
{
  DOUBLE low, high;

  limit.getLow(&low);
  limit.getHigh(&high);

  double factor_To_A = 1;
  if (!limit.unit().empty() && limit.unit().find(";") == string::npos)
  {
    factor_To_A = SI_Value::getDiffValue(limit.unit(),"A");
  } 
  return (MAX (fabs(low),fabs(high)) * factor_To_A);
}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::validateAndConvertStrToInt
 *
 * Purpose: validate input string of number and convert it into integer.
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Usage:
 * input strNum : "1,2,3"
 * output stopNumber[]: 1,2,3
 *
 *----------------------------------------------------------------------*
*/
inline void CommonUtil::validateAndConvertStrToInt(
                              const STRING& strNum, 
                              ARRAY_I& stopNumber)  
{
  if ( strNum.empty() )
  {
    return;
  }

  STRING_VECTOR vecNum;
  STRING_VECTOR::const_iterator it;
  const CHAR* pChar;
  INT index = 0;
  STRING strError;
 
  CommonUtil::splitStr(strNum,',',vecNum);
  stopNumber.resize( vecNum.size() );

  /*check each string of number, and conver it into integer*/ 
  for ( it = vecNum.begin(); it != vecNum.end(); ++it,++index ) 
  {
    pChar = it->c_str();
    while ( (*pChar != '\0') || (*it == "")  )
    {
      if ( !isdigit(*pChar++) )
      {
        strError = "The number of vector or cyclc : \"" +
                   (*it) + "\" is NOT valid!";
        throw Error("CommonUtil::validateAndConvertStrToInt", 
                    strError);      
      }
    }
    stopNumber[index] = atoi(it->c_str());
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::getTerminatedPins
 *
 * Purpose: Create a pinlist of terminated pins
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline STRING CommonUtil::getTerminatedPins()
{
  STRING fwAnswer = "";
  STRING terminatedPins = "";
  INT i = 0;
  INT j = 0;

  /* get answer */
  FW_TASK ( "TERM? PRM,(@)\n", fwAnswer );

  /* parse answer */
  i = fwAnswer.find ("TERM");
  while ( i != static_cast<INT>(string::npos) )
  {
     i = fwAnswer.find (',', i + 5);
     i += 1;

     switch ( fwAnswer[i] )
     {
       case 'T':
       case 'A':
       case 'C':
         i = fwAnswer.find ('(', i);
         i += 1;
         j = fwAnswer.find (')', i);

         if ( terminatedPins.length() == 0 )
         {
           terminatedPins = fwAnswer.substr(i,j-i);
         }
         else
         {
           terminatedPins += ',' + fwAnswer.substr(i,j-i);
         }
         break;

       default: /* Just skip other types of answer */
         break;
     }

     i = fwAnswer.find ("TERM", i);
  }

  return terminatedPins;
}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::getMode
 *
 * Purpose: define to use PVAL|PPF|GPF according test suite flag
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline TM::DCTEST_MODE CommonUtil::getMode()
{
  STRING testSuiteName = "";

  INT valueOnPass;
  INT perPinOnPass;
  INT valueOnFail;
  INT perPinOnFail;

  GET_TESTSUITE_NAME(testSuiteName);
  GET_TESTSUITE_FLAG(testSuiteName, "value_on_pass",   &valueOnPass);
  GET_TESTSUITE_FLAG(testSuiteName, "value_on_fail",   &valueOnFail);
  GET_TESTSUITE_FLAG(testSuiteName, "per_pin_on_pass", &perPinOnPass);
  GET_TESTSUITE_FLAG(testSuiteName, "per_pin_on_fail", &perPinOnFail);

  if ( 1 == valueOnPass || 1 == valueOnFail )
  {
    return TM::PVAL;
  }
  else if ( 1 == perPinOnPass || 1 == perPinOnFail )
  {
    return TM::PPF;
  }

  return TM::GPF;
}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::disconnectPins
 *
 * Purpose: disconnect pins specified in pinlist
 *
 *----------------------------------------------------------------------*
 * Description:
 *   only disconnect pins for enabled sites
 * Note:
 *   please refer to RLYC command
 *----------------------------------------------------------------------*
 */

inline void CommonUtil::disconnectPins(const STRING& pinlist)
{

  if ( pinlist.length() != 0 )
  {
    STRING disco;
    STRING enabledSites;
    GET_ENABLED_SITES(enabledSites);
    disco = "PSFC \"" + enabledSites + "\"\n";
    disco += "RLYC IDLE,OFF,(" + pinlist + ")\n PSFC ALL\n";

    FW_TASK (disco);

  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::connectPins
 *
 * Purpose: connect pins specified in pinlist
 *
 *----------------------------------------------------------------------*
 * Description:
 *   only connect pins for enabled sites
 * Note:
 *   please refer to RLYC command
 *----------------------------------------------------------------------*
 */
inline void CommonUtil::connectPins(const STRING& pinlist)
{
  if ( pinlist.length() != 0)
  {
    STRING con;
    STRING enabledSites;
    GET_ENABLED_SITES(enabledSites);
    con = "PSFC \"" + enabledSites + "\"\n";
    con += "RLYC AC,OFF,(" + pinlist + ")\n PSFC ALL\n";
 
    FW_TASK (con);
  }
}
/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::isToSetSequencerOff
 *
 * Purpose:  according to conditions
 *
 *----------------------------------------------------------------------*
 * Description:
 *   @output: needSetSequencerOff
 *      to eliminate side-effect of sequencer's state 
 *      after PPMU-parallel(FlexDC) measurement. we should do:
 *      if connectState==DISCONNECT, 
 *          must set: needSetSequencerOff = TRUE
 *      if connectState==UNCHANGED and the state of sequencer is OFF, 
 *          must set: needSetSequencerOff = TRUE
 *      otherwise needSetSequencerOff = FALSE.
 *   The result is TRUE, that means to set sequencer to OFF state.
 *
 * Note:
 *   
 *----------------------------------------------------------------------*
 */

inline Boolean CommonUtil::isToSetSequencerOff(
                                    const STRING& connectState,
                                    const STRING& measureMode)
{
  Boolean needSetSequencerOff = FALSE;

  if (measureMode == "PPMUpar")
  {
    if (connectState == "DISCONNECTED")
    {
      needSetSequencerOff = TRUE;
    }
    else if (connectState == "UNCHANGED")
    {
      STRING fwAnswer;
      FW_TASK("SQST?\n",fwAnswer);
      if (fwAnswer.find("OFF",5) != string::npos)
      {
        needSetSequencerOff = TRUE;
      }
    }
  }

  return needSetSequencerOff;
}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::conditionalConnect
 *
 * Purpose: set connection according to conditions
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *   
 *----------------------------------------------------------------------*
 */
inline void CommonUtil::conditionalConnect(const STRING& connectState)
{
  if (connectState == "CONNECTED")
  {
    CONNECT();
  } 
  else if (connectState == "DISCONNECTED")
  {
    DISCONNECT();
  }
  else if (connectState != "UNCHANGED")
  {
    throw Error("CommonUtil::conditionalConnect", 
                "Wrong Connect State.");
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::clampSimulation
 *
 * Purpose: a patch for compatible with current soc lib
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *   The logic is in "General PMU specification.doc"
 *----------------------------------------------------------------------*
 */
inline void CommonUtil::clampSimulation(const string& measureMode,
                                        const double spmuClampVoltage_V, 
                                        const double limitHighEnd_V, 
                                        double& forceCurrent_uA)
{
  if (fabs(spmuClampVoltage_V) < 1e-9) 
  {
      return;
  }

  if ( (measureMode == "PPMUpar") || (measureMode == "PPMUser") )
  {
    if ( (spmuClampVoltage_V < limitHighEnd_V) && (forceCurrent_uA > 0) )
    {
        forceCurrent_uA = - forceCurrent_uA;
    }
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::printTestSuiteName
 *
 * Purpose: print test suite's name with other given messages
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline void CommonUtil::printTestSuiteName(const STRING& output, 
                                                const STRING& preMessage, 
                                                const STRING& postMessage)
{
  if ( output != "ReportUI" ) 
  {
    return;
  }
  STRING testSuiteName;
  GET_TESTSUITE_NAME(testSuiteName);

  cout << preMessage << testSuiteName << postMessage;
  cout << " Site: " << CURRENT_SITE_NUMBER() << endl;
}


/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::splitStr
 *
 * Purpose: split a string into an array by serperation CHAR sepCh
 *----------------------------------------------------------------------*
 * Usage:
 *  (1)splitStr("1,2,3",',',array)
 *     output array will be ["1","2","3"]
 *  (1)splitStr("1,2,3,(4,5)" , ',' , array)
 *     output array will be ["1","2","3","(4,5)"]
 *  (2)splitStr("1,2,3,(4,5)" , ',' , array, FALSE)
 *     output array will be ["1","2","3","(4","5)"]
 *
 * Note:
 *  (1) for "Vee,Vcc,", it will be splitted into ["Vee",Vcc"], not
 *      ["Vee","Vcc",""]
 *----------------------------------------------------------------------*
 */
inline INT CommonUtil::splitStr (
                             const STRING&   origStr, 
                             const CHAR            sepCh,
                             STRING_VECTOR&  array, 
                             const Boolean         bSupportBracket)
{
  Boolean bIsWithinBracket = FALSE;
  STRING word;
  const CHAR* str = origStr.c_str();

  for ( INT i = 0 ; *(str+i) != 0 ; ++i )
  {
    CHAR ch = str[i];

    /* seperator CHAR '\n' */
    if ( sepCh == '\n' )
    {
      if ( ch == sepCh )
      {
        array.push_back(word);
        word = "";
      }
      else
      {
        word += ch;
      }
      continue;
    }

    /* for other seperator char, like ',', '"',' ", etc.*/
    if( ch == sepCh )
    {
      if ( bIsWithinBracket && bSupportBracket )
      {
        word += ch;
      }
      else
      {
        array.push_back(word);
        word = "";
      }
    }
    else if ( ch == '(' )
    {
      bIsWithinBracket = TRUE;
    }
    else if ( ch == ')' )
    {
      bIsWithinBracket = TRUE;
    }
    else if ( ch == ' ' ) 
    {
      if ( i == 4)
      {
        /* this space is after the FW command name, eg. "DFPN " */
        array.push_back(word);
        word = "";
      }
    }
    else 
    {
      word += ch;
    }
  }

  /* origStr is like "abc,def", we pick the rest one */
  if ( word.size() )
  {
    array.push_back(word);
  }

  return 0;
}


/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::printValue
 *
 * Purpose: print result value
 *
 *----------------------------------------------------------------------*
 *
 * Note:
 *  
 *----------------------------------------------------------------------*
 */
inline void CommonUtil::printValue(DOUBLE val, 
                                   const STRING& unit, 
                                   const Boolean bFormat)
{
  if ( bFormat )
  {
    CHAR buf[80];                       /* "80" is enough for real-world value! */
    snprintf(buf, 80, "%.3f", val);    /* ".3f" is enough for precision */
    STRING str(buf);
    str += (" " + unit);
    sprintf(buf, "%14s", str.c_str());  /* "14" is proper for format */
    cout << buf << '\t';
  }
  else
  {
    cout << '\t' << val << " " << unit;
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::datalogToWindow
 *
 * Purpose: print datalog To report Window
 *
 *----------------------------------------------------------------------*
 * Description:
 *   this prototype only for TM::PVAL
 * 
 *   output: string name of output
 *   standardUnitOfValue: the unit for measured value, e.g. A,V,S,Hz
 *   theMeasuredValue: the meausred value which is standardised in 
 *                     the unit of "standardUnitOfValue"
 *   specificDefaultLimitUnit: default unit for limit 
 *   limit: the target limit
 * Note:
 *   
 *----------------------------------------------------------------------*
 */
inline void CommonUtil::datalogToWindow(
                            const STRING& output,
                            const STRING& pin,
                            const STRING& standardUnitOfValue,
                            const DOUBLE  theMeasuredValue,
                            const STRING& specificDefaultLimitUnit,
                            const LIMIT&  limit)
{
  if ( output != "ReportUI" ) 
  {
    return;
  }
  double measuredValue = theMeasuredValue;
  //if default unit is used, need scale the value before judgement
  if (!specificDefaultLimitUnit.empty()) 
  {
    measuredValue *= SI_Value::getDiffValue(standardUnitOfValue,specificDefaultLimitUnit); 
  }

  STRING PFjudge = limit.pass(measuredValue)?"P":"F";
  STRING opl_str, oph_str;

  DOUBLE lowVal,highVal;
  TM::COMPARE lowCmp, highCmp;
  limit.get(lowCmp, lowVal, highCmp, highVal);

  switch (lowCmp)
  {
    case TM::GT:
      opl_str = "< ";
      break;

    case TM::GE:
      opl_str = "<=";
      break;

    case TM::NA:
      opl_str = "";
      break;

    default:
      throw Error("ClassPmuUtil::datalogToWindow",
                  "Unknown Compare Mode of opl");

  }
  switch (highCmp)
  {
    case TM::LT:
      oph_str = "< ";
      break;

    case TM::LE:
      oph_str = "<=";
      break;

    case TM::NA:
      oph_str = "";
      break;

    default:
      throw Error("ClassPmuUtil::datalogToWindow",
                  "Unknown Compare Mode of oph");

  }

  CHAR buf[pin.length()+1];
  sprintf(buf, "%-16s", pin.c_str());
  cout << buf << '\t';

  string limitUnitName;
  double factor = 1;
  if (specificDefaultLimitUnit.empty())
  {
    limitUnitName = getUnitNameOfLimit(limit);
    if (string::npos != limit.unit().find(";")) 
    {
      //need to re-scale the lower and upper value of limit
      //because the current limit values have been converted to standard unit
      factor = SI_Value::getDiffValue(standardUnitOfValue,limitUnitName);
    }
  }
  else
  {
    limitUnitName = specificDefaultLimitUnit;
  }

  /* value of lower limit */
  if ( opl_str != "" )
  {
    cout << "min:";
    printValue(lowVal * factor, limitUnitName, FALSE);
    cout << '\t' << opl_str << '\t';
  }

  /* measured value */
  printValue(measuredValue * factor, limitUnitName, TRUE);
  cout << '\t';

  /* value of higher limit */
  if ( oph_str != "" )
  {
    cout << oph_str << '\t' << "max:";
    printValue(highVal * factor, limitUnitName, FALSE);
    cout << '\t';
  }

  /* Pass or Fail Info */
  cout << PFjudge <<endl;
}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::datalogToWindow
 *
 * Purpose: print datalog To report Window
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:
 *   this protoype only for TM::PPF
 *----------------------------------------------------------------------*
 */
inline void CommonUtil::datalogToWindow(
                  const STRING& output,
                  const STRING& pin,
                  const Boolean bPass)
{
  if ( output != "ReportUI" ) 
  {
    return;
  }
  
  CHAR buf[pin.length()+1];
  sprintf(buf, "%-16s", pin.c_str());
  cout << buf << "\t";
  cout << (bPass? "P": "F") <<endl;
}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::programmableLoadContinuityTest
 *
 * Purpose: do continuity by using programmable load (active load). 
 *
 *----------------------------------------------------------------------*
 * Description:
 *        (1) call FW_TASK to execute the test.
 *        (2) parse the result string and record results.
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline void CommonUtil::programmableLoadContinuityTest(
                       const STRING_VECTOR&    vecPins,
                       const STRING&           polarity,
                       const LIMIT&            limit,
                       const TM::DCTEST_MODE&  mode,
                       MeasurementResultContainer& continRes,
                       DOUBLE                  dForceCurrent)
{
  DOUBLE dLowVal = 0.0,dHighVal = 0.0;
  limit.getLow(&dLowVal);
  limit.getHigh(&dHighVal);
  /*
   ************************************
   * DFCT FW command's internal units:
   * current [uA] 
   * voltage [mV]
   ************************************
   */
  dLowVal   *= 1e+3; /* convert from V to mV */
  dHighVal  *= 1e+3; /* convert from V to mV */

  /*
   *********************************
   * set commutation voltage,
   * 1000mV is the minimum value. 
   *********************************
   */
  DOUBLE dCommutVolt = (dForceCurrent < 0.0)?(-1000.0 + dLowVal):(1000.0 + dHighVal); 

  /*
   ***************************************
   * create and send fw command string
   * of defining continuity test.
   ***************************************
   */
  STRING strFwCmd;
  /* precondition FW command string */
  strFwCmd = "SQGB ACQF,0\n";

  /* create DFCT FW command string */
  strFwCmd += "DFCT ";
  CHAR fwCmdStringValue[20] = "\0";

  /* force current */
  snprintf(fwCmdStringValue,20,"%.9f,",dForceCurrent);
  strFwCmd += fwCmdStringValue; 

  /* commutation voltage */
  snprintf(fwCmdStringValue,20,"%.9f,",dCommutVolt);
  strFwCmd += fwCmdStringValue; 

  /* pass min. voltage */
  snprintf(fwCmdStringValue,20,"%.9f,",dLowVal);
  strFwCmd += fwCmdStringValue; 

  /* pass max. voltage */
  snprintf(fwCmdStringValue,20,"%.9f,",dHighVal);
  strFwCmd += fwCmdStringValue; 

  /* polarity */
  strFwCmd += polarity + ","; 

  /* pin list */
  STRING strPins = PinUtility.createPinListFromPinNames(vecPins);
  strFwCmd += "(" + strPins +")\n"; 

  /*
   *************************************** 
   * append query commands for 
   * all active sites.
   ***************************************
   */
  switch (mode) 
  {
    case TM::PPF:
    case TM::PVAL: /* consider PVAL as PPF for 'CTST?' */
      FOR_EACH_SITE_BEGIN();
        /* set query focus site */
        strFwCmd += "PQFC ";
        sprintf(fwCmdStringValue,"%d",CURRENT_SITE_NUMBER());
        strFwCmd += fwCmdStringValue;
        strFwCmd += "\n";
        /* CTST? command */
        strFwCmd += "CTST? PPF\n";
      FOR_EACH_SITE_END();
      break;
    case TM::GPF:
      strFwCmd += "CTST? GPF\nPRLT? ALL\n";
      break;
    default:
      throw Error("CommonUtil::programmableLoadContinuityTest",
                  "Unknown execute mode.");
      break; 
  }

  /* send FW */ 
  STRING strFwAnswer = "";
  FW_TASK(strFwCmd,strFwAnswer);

  /*
   ***************************************
   * Parse and record the result
   ***************************************
   */
  parseAndRecordContinuityResult(vecPins.size(),mode,strFwAnswer,continRes);

}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::parseAndRecordContinuityResult
 *
 * Purpose: parse and record continuity test results. 
 *
 *----------------------------------------------------------------------*
 * Description:
 *      support multi-site.
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline void CommonUtil::parseAndRecordContinuityResult(
                 const INT                     pinsSize,
                 const TM::DCTEST_MODE&  mode,
                 const STRING&           fwAnswer,
                 MeasurementResultContainer& continResult)
{
  switch (mode) 
  {
    case TM::PPF:
    case TM::PVAL: 
      parseAndRecordContinuityResultForPPF(pinsSize,fwAnswer,continResult);
      break;
    case TM::GPF:
      parseAndRecordPRLTResult(fwAnswer,continResult);
      break;
    default:
      throw Error("CommonUtil::parseAndRecordContinuityResult",
                  "Unknown execute mode.");
      break; 
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::parseAndRecordContinuityResultPPF
 *
 * Purpose: parse and record continuity test results for PPF mode
 *
 *----------------------------------------------------------------------*
 * Description:
 *      support multi-site.
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline void CommonUtil::parseAndRecordContinuityResultForPPF(
                 const INT                     pinsSize,
                 const STRING&           fwAnswer,
                 MeasurementResultContainer& continResult)
{
 /*
  ************************************
  * Analyze the query result
  * examples of the answer string:
  * (1) CTST F,(I/O0,I/O1,I/O2)\n
  * (2) CTST P,(I/O0)\n CTST F,(I/O1,I/O2)\n
  ************************************
  */
  
  INT  iPosOne = 0;
  INT  iPosTwo = 0;
  iPosOne = fwAnswer.find( "CTST" );
 
  /* retrieve and record results for each site */
  FOR_EACH_SITE_BEGIN();    
    STRING resToken = "";
    INT nSite = CURRENT_SITE_NUMBER();
    Boolean isPass = TRUE;       /*define for per pin result*/
    Boolean bGlobalResult = TRUE; /*define for global result*/
    INT nPins = pinsSize;         /*to identify results for each site*/

    while ( (iPosOne != static_cast<INT>(string::npos)) && (nPins > 0) ) 
    {
      /*retrieve the result of checking states*/
      iPosOne = fwAnswer.find( ',',iPosOne+5 );
      resToken = fwAnswer[iPosOne-1];
      /*get pass/fail*/
      isPass = ( (resToken == "P") || (resToken == "p") )?TRUE:FALSE;
      /*update GPF result*/
      if ( (!isPass) )
      {
        bGlobalResult = FALSE; 
      }
      iPosOne = fwAnswer.find( '(',iPosOne+1 );
      ++iPosOne;
      iPosTwo = fwAnswer.find( ')',iPosOne+1 );

      /*retrieve the pin name*/
      resToken = fwAnswer.substr( iPosOne,iPosTwo-iPosOne );
      /*Parse this substring and record the results*/
      STRING_VECTOR arrayOfPinNames;
      arrayOfPinNames.clear();
      CommonUtil::splitStr( resToken,',',arrayOfPinNames );
      for( STRING_VECTOR::const_iterator it = arrayOfPinNames.begin(); 
           it != arrayOfPinNames.end(); ++it )
      {
        continResult.setPinPassFail( *it,isPass,nSite );
        --nPins;
      }
      /* try to get the next line starting with 'CTST' */
      iPosOne = fwAnswer.find( "CTST",iPosTwo+1 );
    }
    /* set global result for the site */
    continResult.setGlobalPassFail(bGlobalResult,nSite);
  FOR_EACH_SITE_END();
}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::parseAndRecordPRLTResult
 *
 * Purpose: parse and record test results by PRLT
 *
 *----------------------------------------------------------------------*
 * Description:
 *      support multi-site.
 *
 * Note:
 *
 *----------------------------------------------------------------------*
 */
inline void CommonUtil::parseAndRecordPRLTResult(
                 const STRING&           fwAnswer,
                 MeasurementResultContainer& measResult)
{
 /*
  ************************************
  * Analyze the query result
  * examples of the answer string:
  * (1) PRLT ALL, "PF0"
  ************************************
  */
  
  INT  iPosOne = 0;
  iPosOne = fwAnswer.find( "PRLT" );
  iPosOne = fwAnswer.find( '"',iPosOne+5 );
 
  /* retrieve and record results for each site */
  FOR_EACH_SITE_BEGIN();    
    STRING resToken = "";
    INT nSite = CURRENT_SITE_NUMBER();
    Boolean bGlobalResult = TRUE; /*define for global result*/

    /*retrieve the result of checking states*/
    resToken = fwAnswer[iPosOne+nSite];
    /*get pass/fail*/
    bGlobalResult = ( (resToken == "P") || (resToken == "p") )?TRUE:FALSE;
    /* set global result for the site */
    measResult.setGlobalPassFail(bGlobalResult,nSite);
  FOR_EACH_SITE_END();
}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::evaluateMeasurementResult
 *
 * Purpose: evaluate the result and do the datalog
 *
 *----------------------------------------------------------------------*
 * Description:
 *   It is used for :
 *           checking dps status,
 *           dps connectivity, 
 *           continuity(in active load mode).
 * Note:
 *    This function consider PVAL as PPF, because these kind of tests
 *    don't return any values.
 *----------------------------------------------------------------------*
 */
inline void CommonUtil::evaluateMeasurementResult(
                      const STRING_VECTOR&          pins,
                      const TM::DCTEST_MODE&        mode,
                      const MeasurementResultContainer& measResult,
                      const STRING&                 output)
{
  STRING allPins ;
  STRING_VECTOR::const_iterator it;
  Boolean isPass = TRUE;
  STRING testsuiteName;
  GET_TESTSUITE_NAME(testsuiteName);
  INT nSiteNumber = CURRENT_SITE_NUMBER();

  switch(mode)
  {
    case TM::PVAL:  /* same as PPF */
    case TM::PPF:
      for ( it = pins.begin(); it != pins.end(); ++it )
      {
        isPass = measResult.getPinPassFail( *it,nSiteNumber );
        TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(
             *it,
             testsuiteName,
             isPass?TM::Pass : TM::Fail,
             0.0);
        CommonUtil::datalogToWindow( output, *it, isPass );
      }
      break;

    case TM::GPF:
      allPins = PinUtility.createPinListFromPinNames(pins);
      isPass = measResult.getGlobalPassFail(nSiteNumber);
      TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(
          allPins, 
          testsuiteName, 
          isPass?TM::Pass : TM::Fail, 
          0.0);
      break;

    default:
      throw Error("CommonUtil::evaluateMeasurementResult",
                  "Unknown Output Mode!");
      break;
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::string2Double
 *
 * Purpose: convert a string to double, if the string content is not a 
 *          double,throw an error                                                               
 *
 *----------------------------------------------------------------------*
 * Description:                                          
 *
 * Parameters:  1. STRING input :                                                                                     
 *                 string need to convert to double                                                                   
 *              2. STRING name :                                                                                      
 *                 parameters name, need for output error message                                                                              
 *
 * Return Value: output double value 
 *----------------------------------------------------------------------*
 */
inline DOUBLE CommonUtil::string2Double(const STRING& input, const STRING& name)
{
  DOUBLE result;
  char* pEnd;
  result = strtod(input.c_str(), &pEnd);
    
  if((result == 0 && atof(input.c_str())!= 0)
     || pEnd[0] != '\0')
  {
    STRING api = "Convert parameter["+name+"] :"+ input + " to DOUBLE fail";
    STRING msg = "it is not a double!";
    throw Error(api.c_str(),msg.c_str(),api.c_str());     
  }
   
  return result; 
}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::string2Long
 *
 * Purpose: convert a string to long int, if the string content is not a 
 *          long int,throw an error                                                               
 *
 *----------------------------------------------------------------------*
 * Description:                                          
 *
 * Parameters:  1. STRING input :                                                                                     
 *                 string need to convert to long int                                                                   
 *              2. STRING name :                                                                                      
 *                 parameters name, need for output error message                                                                              
 *
 * Return Value: output long int value 
 *----------------------------------------------------------------------*
 */
inline LONG CommonUtil::string2Long(const STRING& input,const STRING& name)
{
  LONG result;
  char* pEnd;
  result = strtol(input.c_str(),&pEnd,10);

  if((result == 0 && atol(input.c_str()) != 0) ||
     (pEnd[0] != '\0' && pEnd[0] != '\n'))
  {
    STRING api = "Convert parameter["+name+"] :"+input + " to LONG fail";
    STRING msg = "it is not a long int!";
    throw Error(api.c_str(),msg.c_str(),api.c_str());
  }
  
  return result;
}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::double2String
 *
 * Purpose: convert a double to string                                                          
 *
 *----------------------------------------------------------------------*
 * Description:                                          
 *
 * Note:                                                                            
 *   
 *----------------------------------------------------------------------*
 */
inline const STRING CommonUtil::double2String(DOUBLE number)
{
  char outStr[41] = "\0"; /*the buffer len is enough to contain numbers*/
  if ( snprintf(outStr,40,"%.15g",number) < 0) /*precision is enough*/
  {
    throw Error("CommonUtil::double2String",
                "error happened when convert!",
                "CommonUtil::double2String");
  }

  return outStr;
}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::trim
 *
 * Purpose: get rid of head and tail space of input
 *
 *----------------------------------------------------------------------*
 * Description:                                          
 *
 * Note:                                 
 *
 * Return Value: new string 
 *----------------------------------------------------------------------*
 */
inline STRING CommonUtil::trim(const STRING& str)
{
  int iStart = 0;
  int iStop = 0;
  int i;

  for(i=0;i<static_cast<int>(str.length());i++)
  {
    if(str[i] != ' ')
    {
      iStart = i;
      break;
    }
  }

  for(i=str.length()-1;i>=0;i--)
  {
    if(str[i] != ' ')
    {
      iStop = i+1;
      break;
    }
  }

  return str.substr(iStart,iStop-iStart);
}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::getUnburstLabels
 *
 * Purpose: get the labels contained in the input BRST label
 *
 *----------------------------------------------------------------------*
 * Description:
 *    Use "SQLB?" to check if the input label is a BRST label,
 *    if it is, then use "SQPG?" to query the labels contained in the
 *    BRST label, then return the labels.
 * 
 *    @param  strLabel: the label name you want to unburst.
 *    @return the labels contained in the input BRST label OR an empty vector
 *   
 *----------------------------------------------------------------------*
 */
inline void CommonUtil::getUnburstLabels(const STRING& strLabel,STRING_VECTOR& unburstLabels)
{
  STRING::size_type iPosBegin = 0,iPosEnd = 0;
  STRING strSQLBQuery = STRING("SQLB? ") + "\"" + strLabel + "\",BRST\n";
  STRING strSQLBResult = "";
  STRING strSQPGQuery = "SQPG? ";
  STRING strSQPGResult = "";
    
  /* SQLB? "strLabel",BRST */
  FW_TASK(strSQLBQuery,strSQLBResult);
  if( !strSQLBResult.empty())
  {
    iPosBegin = strSQLBResult.find(",BRST,");
    iPosBegin = iPosBegin + 6;
    iPosEnd = strSQLBResult.find(',',iPosBegin);
    STRING strStartCmdNum = strSQLBResult.substr(iPosBegin,iPosEnd-iPosBegin);
    iPosBegin = iPosEnd + 1;
    iPosEnd = strSQLBResult.find(',',iPosBegin);
    STRING strStopCmdNum = strSQLBResult.substr(iPosBegin,iPosEnd-iPosBegin);    
    strSQPGQuery = strSQPGQuery + "\"" + strStartCmdNum + "," + strStopCmdNum + "\",(@)\n";
    /* SQPG? "strStartCmdNum,strStopCmdNum",(@) */
    FW_TASK(strSQPGQuery,strSQPGResult);
    if(!strSQPGResult.empty())
    {
      istringstream sttForAnaylsis(strSQPGResult);
      STRING strSingleLine = "";
      /* Get label names from the result of "SQPG?" line by line */
      while(getline(sttForAnaylsis,strSingleLine,'\n'))
      {
        iPosBegin = strSingleLine.find('"');
        if( iPosBegin != string::npos)
        {
          iPosBegin = iPosBegin + 1;
          iPosEnd = strSingleLine.find('"',iPosBegin);
          unburstLabels.push_back(strSingleLine.substr(iPosBegin,iPosEnd - iPosBegin));
        }               
      }
    }
  }
}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::convertVectorNumToCycleNum
 *
 * Purpose: convert cycle number to cycle number in string type
 *
 *----------------------------------------------------------------------*
 * Description:
 *   Use GETC fw cmd to do conversion.
 * Parameters:
 *   strOfVecNum: the vector number in the type of string
 * Output: 
 *   the string of cycle number from fw answer.
 * Note: 
 *   
 *----------------------------------------------------------------------*
 */
inline const STRING 
CommonUtil::convertVectorNumToCycleNum(const STRING& strOfVecNum)
{
  STRING answer;
  /*send cmd*/
  FW_TASK("GETC? "+strOfVecNum+"\n",answer);
  
  /*
   *answer(e.g.): "GETC xxx\n"
   *5 is the start pos of the cycle number in fw answer.
   *minus 1 to exclude '\n' char! 
   */
  return answer.substr(5,answer.size()-5-1);

}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::splitPortDataAndPortName
 *
 * Purpose: split port data and port name from input string.
 *
 *----------------------------------------------------------------------*
 * Description:
 *   Typical Usage(e.g.): 
 *        input   : "123@port_1"
 *        portData: "123"
 *        portName: "port_1"
 *   Invalid input(e.g):
 *       (1)"@port_x","nnn@","nnn@illegal_portname"
 *       (2)"nnn" (if throwErrorForNoPortName is true)   
 * Parameters:
 *   1. input: the original input string need to be split  
 *   2. portData: to be filled with the data info at that port.
 *   3. portName: to be filled with the specific port name
 *   4. throwErrorForNoPortName: 
 *             issue error if input doesn't contain port.
 *             this flag is true by default.    
 * Output: 
 *   status
 *   0:   succeed
 *   -1:  failed.
 * Note: 
 *   
 *----------------------------------------------------------------------*
 */
inline int CommonUtil::splitPortDataAndPortName(
                 const STRING& input, 
                 STRING& portData, 
                 STRING& portName,
                 Boolean throwErrorForNoPortName)
{
  int status = -1;
  portData = "";
  portName = "";
  
  if ( !input.empty() )
  {
    /*position of port symbol*/ 
    STRING::size_type pos = input.find_first_of('@',0); 
    if ( pos != STRING::npos )
    { 
      if ( pos > 0) /* 0 means a testflow variable */    
      {
        /*get all chars before '@'*/
        portData = input.substr(0,pos);
        /*get all chars after '@'*/
        portName = CommonUtil::trim(input.substr(pos+1));
        if ( portName.length() > 0 )
        { 
            status = 0; 
        }
      }
    }
    else /*no '@'* found*/
    {  
      if ( !throwErrorForNoPortName )
      { 
        status = 0;/*sucess case*/  
        portData = input;
        portName = "";
      } 
    }
  }
  
  /*check the status and error handling*/
  if ( status != 0 )
  {
    STRING funcName = "CommonUtil::splitPortDataAndPortName";
    STRING errorMsg;
    if ( input.empty() )
    {
      errorMsg = "Input string is empty!";  
    }
    else
    {
      errorMsg = "Invalid port of input string: " + input;
    }
        
    throw Error(funcName,errorMsg,funcName);            
  }   
  
  return status;
}
/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::convertStringToSearchType
 *
 * Purpose: convert a string to spec_type
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:        SPEC_SEARCH can't support input string in type() 
 *              directly, but SEARCH_FUNC_TASK can support it                                                                                                           
 *
 * Parameters:  STRING input:  (timing/level only) 
 *
 * Return Value: output SPEC_TYPE
 *
 *----------------------------------------------------------------------*
 */
inline TM::SPEC_TYPE 
CommonUtil::convertStringToSearchType(const STRING& input) 
{
  if(input == "timing" || input == "TIM") return TM::TIM;
  if(input == "level" || input == "LEV") return TM::LEV;
  
  STRING api = "Spec_search: convertStringToSearchType : ";
  STRING msg = "SearchType must be timing | level ! " + input + "is illegal!";
  throw Error(api.c_str(),msg.c_str(),api.c_str()); 
}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::convertStringToMethod
 *
 * Purpose: convert a string to spec_method
 *
 *----------------------------------------------------------------------*
 * Description:
 *
 * Note:        SPEC_SEARCH can't support input string in method() directly, 
 *              but SEARCH_FUNC_TASK can support it
 *
 * Parameters:  STRING input:  (linear |binary |lin/bin only) 
 *
 * Return Value: output SPEC_METHOD
 *
 *----------------------------------------------------------------------*
 */
inline TM::SPEC_METHOD 
CommonUtil::convertStringToMethod(const STRING& input) 
{
  if(input == "linear") return TM::Linear;
  if(input == "binary") return TM::Binary;
  if(input == "lin/bin") return TM::LinBin;
  
  STRING api = "Spec_search: convertStringToMethod : ";
  STRING msg = "SearchType must be linear | binary | lin/bin ! " 
               + input + "is illegal!";
  throw Error(api.c_str(),msg.c_str(),api.c_str()); 
}

/*
 *----------------------------------------------------------------------*
 * Routine: CommonUtil::getUnitNameOfLimit
 *
 * Purpose: get the single unit name from LIMIT object
 *
 *----------------------------------------------------------------------*
 * Description:
 *  the LIMIT::unit content varies in several formats, according to 
 *  user manual:
 *    a) "V"           --single unit name
 *    b) "ms;-4;ms;0"  --lower and higher units
 *    c) empty         
 * Note: 
 *      
 *
 * Parameters:  LIMIT& object
 *
 * Return Value: string of single unit name
 *
 *----------------------------------------------------------------------*
 */
inline const std::string 
CommonUtil::getUnitNameOfLimit(const LIMIT& limit)
{
  std::string unitStr = limit.unit();
  std::string::size_type unitSep = unitStr.find(";");
  if (std::string::npos != unitSep)
  {
    unitStr = unitStr.substr(0,unitSep);
  }
  return unitStr;
}

/**
 * try to get all required test names from test table
 * record valid test name and invalid test name
 */
class TesttableLimitHelper
{
  private:
    std::vector<std::string> mTestnameNotInTable;
    std::vector<std::string> mTestnameInTable;
    std::string mTestsuiteName;
    bool mIsLimitCsvFileLoad;
  public:
    explicit TesttableLimitHelper(const string& testsuitename)
      :mTestsuiteName(testsuitename),mIsLimitCsvFileLoad(false)
    {
      TestTable* pLimitTable = TestTable::getDefaultInstance();
      pLimitTable->readCsvFile();
      mIsLimitCsvFileLoad = pLimitTable->isTmLimitsCsvFile();

      mTestnameNotInTable.clear();
      mTestnameInTable.clear();
    }

    ~TesttableLimitHelper()
    {
    }

    const std::vector<std::string>& getInvalidTestnames()
    {
      return mTestnameNotInTable;
    }

    const std::vector<std::string>& getValidTestnames()
    {
      return mTestnameInTable;
    }

    //return if all test names are valid in CSV file
    const bool isAllInTable()
    {
      return (mIsLimitCsvFileLoad && 
              !mTestnameInTable.empty() && 
              mTestnameNotInTable.empty());
    }

    const bool isLimitCsvFileLoad()
    {
      return mIsLimitCsvFileLoad;     
    }

    /**
     * for the current testsuite, the method try to get the limit object from test table
     * by the specified test name (i.e. name for limit).
     * @param: 
     *    testname: the specific test name defined for the testsuite
     *    toLimit: the target limit object defined in test table
     * @note:
     *   1) if the test name is defined in test table, 
     *      the target limit object is got from table; 
     *      otherwise, the limit object is untouched as it is.
     *   2) exceptional case:
     *      a) if the csv file is not loaded, exception is thrown.
     * 
     *      b) if there are multiple limits needed for this testsuite,
     *      and only if some testname are valid but others are NOT valid,
     *      one exception is thrown out!  
     */
    void getLimit(const std::string& testname, LIMIT& toLimit)throw(Error)
    {
      if (!mIsLimitCsvFileLoad)
      {
        throw Error("TesttableLimitHelper",
          "no limit CSV file is loaded!",
          "TesttableLimitHelper");
      }

      try
      {
        V93kLimits::TMLimits::LimitInfo& limitInfo =
          V93kLimits::tmLimits.getLimit(mTestsuiteName,testname);

        toLimit = limitInfo.TEST_API_LIMIT;
        mTestnameInTable.push_back(testname);
      }
      catch (Error& e)
      {
        mTestnameNotInTable.push_back(testname);
      }

      //confiliction: some limits are valid but others are not
      if (!mTestnameInTable.empty() && !mTestnameNotInTable.empty())
      {
        throw Error("TesttableLimitHelper",
          "[testsuite: \""+mTestsuiteName+"\"]: test name \""+testname
          +"\" is not found in test table!");
      }
    } 
}; //class TesttableLimitHelper

#endif
