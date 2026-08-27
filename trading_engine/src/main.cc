#include "CppStandard.h"
#include "Engine.h"
#include "PersonnalLibrary.h"

int main()
{
    if (!TradingEngineSetUp())
    {
        log("Running Error. See the logs for more precision.");
    }
    else
    {
        log("Data Processed. Waiting response from Market server...");
    }
    return 0;
}