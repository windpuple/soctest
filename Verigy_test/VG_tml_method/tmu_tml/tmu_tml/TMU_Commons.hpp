#ifndef TMU_COMMONS_HPP_
#define TMU_COMMONS_HPP_

#define  _TML_VERSION "versioin: tml_7.1.0_2.1.0"

 /*
   *----------------------------------------------------------------------*
   * @class: TMU_TML_RESULT_BASE
   *
   * @Purpose: Container base class to store tmu measurement results information
   *
   *----------------------------------------------------------------------*
   */

class TMU_TML_RESULT_BASE
{
public:
	Boolean noResultsForSite;
	Boolean noPinsHasResults;
    TMU::STATUS_TYPE funcTestResult;
    STRING pinlist;
    char errorStr[512];
    STRING_VECTOR pinvector;
	STRING_VECTOR missingpins;
	TMU_TML_RESULT_BASE();
	~TMU_TML_RESULT_BASE();
	Boolean hasFailed() { return (noResultsForSite || noPinsHasResults);}
};


// put pins without results into   missing pins vector.
void getMissingPins(STRING_VECTOR& missingpins, STRING_VECTOR& measuredpins, STRING& pinlist);

// log pins as failed
void logMissingPins(STRING_VECTOR& missingpins);

// print pins wich has no results
void printMissingPins(STRING_VECTOR& missingpins);

#endif

