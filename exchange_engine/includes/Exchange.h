#ifndef     EXCHANGE_H
#define     EXCHANGE_H
// CPP Standard Library


#include "CppStandard.h"

//Exchange Related
#include "WireFormat.h"
#include "OrderBook.h"
#include "Order.h"
#include "Constants.h"
#include <nlohmann/json.hpp>




struct ExecuteOrder
{
    uint64_t    timestamp;
    uint64_t    order_id;
    uint32_t    quantity;
    std::string type;
};

struct CancelOrder
{
    uint64_t    timestamp;
    uint64_t    order_id;
    uint32_t    quantity;
    std::string type;
};

struct DeleteOrder
{
    uint64_t    timestamp;
    uint64_t    order_id;
    std::string type;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AddOrder,
                                   timestamp,
                                   client_order_id,
                                   instrument_id,
                                   quantity,
                                   price,
                                   tif,
                                   side)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ExecuteOrder,
                                   timestamp,
                                   order_id,
                                   quantity,
                                   type)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CancelOrder,
                                   timestamp,
                                   order_id,
                                   quantity,
                                   type)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DeleteOrder,
                                   timestamp,
                                   order_id,
                                   type)



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

    void    printOrder(const AddOrder    &order)
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
        std::cout << "KSE - [Kinshasa Stock Exchange Running !]" << std::endl;

        while (1)
        {
            if (gate_match_engine_command_ring.tail != gate_match_engine_command_ring.head)
            {
                uint64_t command_nb = gate_match_engine_command_ring.tail.load(std::memory_order_acquire);
                // std::visit([](const auto &v){

                // });
                // switch(gate_match_engine_command_ring.command_content.[command_nb].type)
                // {
                //     case OrderType::AddOrder :
                //     const OrderPool    &order_pool = gate_match_engine_command_ring.data[command_nb].command_content;
                //     std::cout << "AddOrder : client_order_id(" << order_pool.add_order.client_order_id << "), instrument_id = (" << order_pool.add_order.instrument_id << "), price = (" << order_pool.add_order.price << "), quantity = (" << order_pool.add_order.quantity << "), ";
                //     break;
                // }
                gate_match_engine_command_ring.tail.store(command_nb+1, std::memory_order_release);
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

    RxRingBuffer<ExchangeInetConst::RX_RING_SIZE, AddOrder>   &getCommandRing()
    {
        return gate_match_engine_command_ring;
    }
    protected :
    OrderBook             orderBook_;
    ExchangeConfig        config_;

    RxRingBuffer<ExchangeInetConst::RX_RING_SIZE, AddOrder>   gate_match_engine_command_ring;
    RxRingBuffer<ExchangeInetConst::TX_RING_SIZE, AddOrder>   order_confirmation_command_ring;

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
                AddOrder order = orders[i].get<AddOrder>();
                
                orderBook_.addOrder(Msg());
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
            if (orders[i]["type"] == "DELETE")
            {
                DeleteOrder order = orders[i].get<DeleteOrder>();
            }
            if (orders[i]["type"] == "CANCEL")
            {
                CancelOrder order = orders[i].get<CancelOrder>();
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