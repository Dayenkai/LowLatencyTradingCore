#include "TradingEngine.h"

int startTradingEngine(int argc, char **argv)
{
    std::unique_ptr<TradingEngine>   engine;

    if (argc > 1)
    {
        engine = std::make_unique<TradingEngineReplay>(argc, argv);
        log("Running Error. See the logs for more precision.");
    }
    else
    {
        engine = std::make_unique<TradingEngineLive>();
        log("Data Processed. Waiting response from Market server...");
    }

    return engine->run();
}