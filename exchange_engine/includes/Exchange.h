#ifndef     EXCHANGE_H
#define     EXCHANGE_H
// CPP Standard Library


#include "CppStandard.h"

//Exchange Related
#include "WireFormat.h"
#include "MatchingEngine.h"
#include "Order.h"
#include "Constants.h"
#include "Utils.h"
#include <nlohmann/json.hpp>


namespace OUCH{
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AddOrder,
                                   timestamp,
                                   client_order_id,
                                   instrument_id,
                                   quantity,
                                   price,
                                   tif,
                                   side)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CancelOrder,
                                   timestamp,
                                   client_order_id,
                                   instrument_id)
}



int setup(int &argc, char **argv);

struct ExchangeConfig
{
    bool    contextRunning = false;
    bool    scenario_presence = false;
};



class   Exchange
{
    public :

    Exchange() = default;

    void    printOrder(const OUCH::AddOrder    &order)
    {
        std::cout << order.client_order_id << ", " << order.price << ", " << order.quantity << ", " << order.timestamp << std::endl;
    }


    // void    Parse()
    // {
    //     inline void    parse(RxDesc &desc, Msg &msg)
    //     {
    //     fillValueLe<uint32_t>(msg._id, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[0]));// static_cast<uint32_t>(*desc.addr) | static_cast<uint32_t>(*(desc.addr + static_cast<size_t>(dataSize[0]/4))) << CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 2)) << 2 * CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 3)) << 3 * CHAR_BIT;
    //     fillValueLe<uint32_t>(msg._instr, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[1]));// static_cast<uint32_t>(*desc.addr + 4) | static_cast<uint32_t>(*(desc.addr + 5)) <<  CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 6)) << 2 * CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 7)) << 3 * CHAR_BIT;
    //     fillValueLe<uint8_t>(msg._side, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[3])); //static_cast<uint8_t>(*(desc.addr + 12));
    //     fillValueLe<uint8_t>(msg._event_type, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[4]));
    //     fillValueLe<uint8_t>(msg._kind, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[5]));
    //     fillValueLe<uint32_t>(msg.price_, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[6]));
    //     fillValueLe<uint32_t>(msg.qty_, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[7]));
    //     }
    // }

    // void    pollNextOrder()
    // {
    //     Msg msg;
    //     if (rx_inet_RingBuffer_.tail != rx_inet_RingBuffer_.head)
    //     {
    //         Parse()
    //     }
    // }

    //USE CASES FUNCTIONS
    virtual int     run()
    {
        std::cout << "KSE - [Kinshasa Stock Exchange is open !]" << std::endl;

        while (true)
        {
            if (gate_match_engine_command_ring.tail != gate_match_engine_command_ring.head)
            {
                auto tail_tmp = gate_match_engine_command_ring.tail.load();
                size_t command_index = gate_match_engine_command_ring.tail.load(std::memory_order_acquire);
                std::visit([&](auto order){
                    using parser_type = std::decay_t<decltype(order)>;

                    if constexpr(std::is_same_v<parser_type, OUCH::AddOrder>)
                    {
                        Msg msg;
                        std::optional<Order> book_order;
                        msg.setId(order.client_order_id).setInstr(order.instrument_id).setPrice(order.price).setQty(order.quantity).setSide(order.side).setStatus(OrderStatus::Accepted);
                        book_order = matching_engine_.AmendOrderBook(msg);
                        std::cout << "Side = " << static_cast<uint8_t>(order.side) << std::endl;;
                        std::cout << "Exchange has received message : ClientOrderId(" << msg.id_ << "), Instrument(" << msg.instr_ << "), Side(" << (order.side == Side::Buy ? "Buy" : "Sell") << "), Price(" << msg.price_ << "), Quantity(" << msg.qty_ << ")\n";
                        if (((order_confirmation_command_ring.head + 1) & ExchangeInetConst::TX_RING_SIZE-1) != order_confirmation_command_ring.tail)
                        {
                            OrderConfirmation conf;
                            const auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now().time_since_epoch()).count();
                            if (book_order.has_value())
                            {
                                conf.timestamp = timestamp;
                                conf.exchangeOrderId = book_order->id_;
                                conf.clientOrderId = msg.id_;
                                conf.quantity = msg.qty_;
                                conf.price = msg.price_;
                                conf.state = msg.status_;

                                writeToExternalRing(order_confirmation_command_ring, conf);
                                // bool written = false;
                                // while (!written)
                                // {
                                //     if(((order_confirmation_command_ring.head + 1) & (ExchangeInetConst::TX_RING_SIZE - 1)) != order_confirmation_command_ring.tail)
                                //     {
                                //         auto head = order_confirmation_command_ring.head.load(std::memory_order_acquire);
                                //         order_confirmation_command_ring.command_content[head] = conf;
                                //         order_confirmation_command_ring.head.store((head+1)&(ExchangeInetConst::TX_RING_SIZE-1), std::memory_order_release);
                                //         written = true;
                                //     }
                                // }
                            //conf.
                            }
                        }
                    }
                }, gate_match_engine_command_ring.command_content[command_index]);
                gate_match_engine_command_ring.tail.store((tail_tmp+1)&(ExchangeInetConst::RX_RING_SIZE-1), std::memory_order_release);
                OrderBook::EventsRecord &record = matching_engine_.matchingUpdate(market_multicast_command_ring);
                while (record.buffering)
                {
                    OrderBook::EventsRecord &record = matching_engine_.matchingUpdate(market_multicast_command_ring);
                }
                if (record.event_tail != record.event_head)
                {
                    for (record.event_tail; record.event_tail != record.event_head; (record.event_tail+1)&(record.size-1))
                    {
                        writeToExternalRing(market_multicast_command_ring, record.events_ring[record.event_tail]);
                    }
                    //We send Event to UDP Multicast Session Gateway
                }
                //matchingUpdate DOes not work for the moment.
            }
        }
        //feedHandler();
        return 0;
    }

    void            EmitOrder()
    {

    }

    void            UpdateOrderBook()
    {

    }

    //BUSINESS FUNCTIONS

    RxRingBuffer<ExchangeInetConst::RX_RING_SIZE, public_OUCH_Messages>   &getCommandRing()
    {
        return gate_match_engine_command_ring;
    }

    RxRingBuffer<ExchangeInetConst::TX_RING_SIZE, OrderConfirmation>   &getOrderFeedbackRing()
    {
        return order_confirmation_command_ring;
    }

    protected :
    MatchingEngine        matching_engine_;
    ExchangeConfig        config_;

    RxRingBuffer<ExchangeInetConst::RX_RING_SIZE, public_OUCH_Messages>   gate_match_engine_command_ring;
    RxRingBuffer<ExchangeInetConst::TX_RING_SIZE, OrderConfirmation>      order_confirmation_command_ring;
    RxRingBuffer<ExchangeInetConst::TX_RING_SIZE, UDPMulticastEvent>      market_multicast_command_ring;
    //Change RX to TXRingBuffer for containers that aimed to send data outwards.
    int id_= 0;

    Msg                   msg_;
    //FeedHandler

};  

class   ExchangeScenario : public Exchange
{
    public :
    ExchangeScenario() = delete;
    ExchangeScenario(const std::string    &exchangeDataFile) : exchangeDataFile_(exchangeDataFile)
    {
    }


    int     run()
    {
        std::ifstream exchangeDataStream(exchangeDataFile_);
        if (!exchangeDataStream.is_open())
        {
            std::cerr << "Could not open Scenario File." << std::endl;
            return -1;
        }
        
        nlohmann::json parsed_json = nlohmann::json::parse(exchangeDataStream);

        const auto &orders = parsed_json["orders"];
        for (size_t i = 0; i < orders.size(); i++)
        {
            if (orders[i]["type"] == "NEW")
            {
                OUCH::AddOrder order = orders[i].get<OUCH::AddOrder>();
                
                //matching_engine_.AmendOrderBook(Msg());
                typedef struct alignas(64) Msg
                {
                    uint32_t                 _id;
                    uint32_t                 _instr;
                    uint32_t                 price_;
                    uint32_t                 qty_;
                    uint8_t                  _kind;
                    uint8_t                  _event_type;
                    uint8_t                  _side;
                }Msg;
            }
            if (orders[i]["type"] == "CANCEL")
            {
                OUCH::CancelOrder order = orders[i].get<OUCH::CancelOrder>();
            }
        }
        //printOrder(order);
        exchangeDataStream.close();
        //Filling OrderBook with Json Scenario File
        //for (auto &)
        Exchange::run();
        return 0;
    };

    private:
    std::string exchangeDataFile_;
};

class   ExchangeRandom : public Exchange
{
    public :    
    ExchangeRandom() = default;
    int     run()
    {
        
        Exchange::run();
        return 0;
    };
};

#endif