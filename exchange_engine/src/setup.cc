#include "CppStandard.h"
#include "Exchange.h"
#include "Constants.h"
#include "MarketDataPublisher.h"
#include "OrderEntryGateway.h"

int setup(int &argc, char **argv)
{
    RxRingBuffer<RxDesc, ExchangeInetConst::TX_RING_SIZE>   gate_match_engine_command_ring;
    RxRingBuffer<RxDesc, ExchangeInetConst::RX_RING_SIZE>   matching_engine_gate_resp_ring;
    RxRingBuffer<RxDesc, ExchangeInetConst::RX_RING_SIZE>   match_engine_mrkt_data_publish_command_ring;
    std::unique_ptr<Exchange>   exchange;
    std::unique_ptr<OrderEntryGateway>   order_entry_gateway;

    //TXRING and RX RING OrderEntryGateway, then thread with MatchingEngine HoldingOrderBook, then thread with MarketDataPublisher.
    //Trading_Side, Thread with NIC or MarketDataReceiver holding a NIC receiving packets, then thread with CoreTrading, local OrderBook,  Strategy, Risk and thread with OrderGateway with TX orders - RX ACK/REJECT/FILL
    if (argc == 1)
    {   
        exchange = std::make_unique<Exchange>(ExchangeRandom(gate_match_engine_command_ring, matching_engine_gate_resp_ring));
    }
    else
    {
        exchange = std::make_unique<Exchange>(ExchangeScenario(argv[1], gate_match_engine_command_ring, matching_engine_gate_resp_ring));
    }

    std::jthread    exchangeThread(&Exchange::run, exchange.get());
    std::jthread    order_gateThread(&OrderEntryGateway::Listen, order_entry_gateway.get());
    //std::jthread    marketDataPublisher(MarketDataPublisher, std::ref(tx_inet_RingBuffer));
    return 0;
}