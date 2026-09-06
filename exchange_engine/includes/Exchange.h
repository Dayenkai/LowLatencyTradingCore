#ifndef     EXCHANGE_H
#define     EXCHANGE_H
// CPP Standard Library


#include "CppStandard.h"

//Exchange Related
#include "WireFormat.h"
#include "OrderBook.h"
#include "Constants.h"
#include <nlohmann/json.hpp>

struct AddOrder
{
    uint64_t            timestamp;
    uint64_t            order_id;
    uint32_t            price;
    uint32_t            quantity;
    std::string         side;
    std::string         type;
    uint8_t             kind;
};

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
                                   order_id,
                                   price,
                                   quantity,
                                   side,
                                   type,
                                   kind)

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

    Exchange(RxRingBuffer<RxDesc, 
                 ExchangeInetConst::TX_RING_SIZE> &tx_inet_ring_buffer, RxRingBuffer<RxDesc, 
                 ExchangeInetConst::TX_RING_SIZE> &rx_inet_ring_buffer) : tx_inet_RingBuffer_(tx_inet_ring_buffer), rx_inet_RingBuffer_(rx_inet_ring_buffer)
    {
    }

    void    printOrder(const AddOrder    &order)
    {
        std::cout << order.order_id << ", " << order.price << ", " << order.quantity << ", " << order.timestamp << std::endl;
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



    protected :
    OrderBook             orderBook_;
    ExchangeConfig        config_;

   
    RxRingBuffer<RxDesc, 
                 ExchangeInetConst::TX_RING_SIZE>   &tx_inet_RingBuffer_;
    RxRingBuffer<RxDesc, 
                 ExchangeInetConst::RX_RING_SIZE>   &rx_inet_RingBuffer_;

    Msg                   msg_;
    //FeedHandler

};

class   ExchangeScenario : public Exchange
{
    public :
    ExchangeScenario() = delete;
    ExchangeScenario(const std::string    &exchangeDataFile, RxRingBuffer<RxDesc, ExchangeInetConst::TX_RING_SIZE> &tx_ring_buffer,
                    RxRingBuffer<RxDesc, ExchangeInetConst::RX_RING_SIZE> &rx_ring_buffer) 
                    : Exchange(tx_ring_buffer, rx_ring_buffer), exchangeDataFile_(exchangeDataFile)
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
    ExchangeRandom(RxRingBuffer<RxDesc, ExchangeInetConst::TX_RING_SIZE> &tx_ring_buffer,
                    RxRingBuffer<RxDesc, ExchangeInetConst::RX_RING_SIZE> &rx_ring_buffer) 
                    : Exchange(tx_ring_buffer, rx_ring_buffer)
    {
    }
    int     run()
    {
        Exchange::run();
        return 0;
    };
};

#endif