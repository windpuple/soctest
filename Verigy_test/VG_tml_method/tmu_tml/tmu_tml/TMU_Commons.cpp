//for testmethod framework interfaces
#include "testmethod.hpp"
//for testmethod API interfaces
#include "mapi.hpp"

#include "TMU_Commons.hpp"


TMU_TML_RESULT_BASE::TMU_TML_RESULT_BASE()
: funcTestResult(TMU::NOT_EXECUTED)
{
	noResultsForSite = FALSE;
	noPinsHasResults = FALSE;
 	errorStr[0] = '\0';
}

TMU_TML_RESULT_BASE::~TMU_TML_RESULT_BASE()
{
	pinlist.clear();
	pinvector.clear();
	missingpins.clear();
}

// put pins without results into   missing pins vector.
void getMissingPins(STRING_VECTOR& missingpins, STRING_VECTOR& measuredpins, STRING& pinlist)
{
	Boolean found = false;
	char delimiter = ',';
	STRING tmpstr = "";
	STRING::const_iterator iter;
	for(iter = pinlist.begin(); iter != pinlist.end();iter++)
	{
		if((*iter != delimiter) && (*iter != ' '))
			tmpstr += *iter;
		else if(*iter == delimiter)
		{
			tmpstr += "\0";
			for(size_t i = 0; i < measuredpins.size();i++)
			{
				found = false;
				if(measuredpins[i] == tmpstr)
				{
					found = true;
					break;
				}
			}
			if(!found)
			{
				missingpins.push_back(tmpstr);
			}
			tmpstr = "";
		}
	}
	// at least the rest
	if(tmpstr != "")
	{
		tmpstr += "\0";
		for(size_t i = 0; i < measuredpins.size();i++)
		{
			found = false;
			if(measuredpins[i] == tmpstr)
			{
				found = true;
				break;
			}
		}
		if(!found)
		{
			missingpins.push_back(tmpstr);
		}
	}
}

// log pins as failed
void logMissingPins(STRING_VECTOR& missingpins)
{
	if(missingpins.empty())
	{
		return;
	}
	for(size_t i = 0; i < missingpins.size();i++)
	{
		TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(missingpins[i],"",TM::Fail,0);
	}
}

// print pins wich has no results
void printMissingPins(STRING_VECTOR& missingpins)
{
	if(missingpins.empty())
	{
		return;
	}
	for(size_t i = 0; i < missingpins.size();i++)
	{
		printf("ERROR: no results for pin %s\n",missingpins[i].c_str());
	}
}
