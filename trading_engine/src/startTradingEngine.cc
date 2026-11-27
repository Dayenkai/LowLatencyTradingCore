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

     std::jthread    listen_mkt_data(&OrderEntryGateway::ListenMarketDataUDP, &engine->getOrderEntryGateway());
        //Listening to TCP Order Feedback
    std::jthread    listen_feedback_order(&OrderEntryGateway::ListenOrderFeedbackTCP, &engine->getOrderEntryGateway());

        //Sending TCP Orders
    std::jthread    order_entry_sender(&OrderEntryGateway::SendOrderTCP, &engine->getOrderEntryGateway());

    return engine->run();
}