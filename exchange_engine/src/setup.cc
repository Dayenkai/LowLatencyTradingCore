#include "CppStandard.h"
#include "Exchange.h"

int setup(int &argc, char **argv)
{
    std::unique_ptr<Exchange>   exchange;
    if (argc == 1)
    {   
        exchange = std::make_unique<Exchange>(ExchangeRandom());
    }
    else
    {
        exchange = std::make_unique<Exchange>(ExchangeScenario(argv[1]));
    }
    exchange->run();
    return 0;
}