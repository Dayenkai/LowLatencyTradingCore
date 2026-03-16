#include "../includes/CppStandard.h"
#include "../includes/Engine.h"
#include "../includes/PersonnalLibrary.h"

int main()
{
    if (!MarketDataEnvSetUp())
    {
        log("Running Error. See the logs for more precision.");
    }
    else
    {
        log("Data Processed. Waiting response from Market server...");
    }
    return 0;
}