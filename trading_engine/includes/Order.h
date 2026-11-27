#ifndef ORDER_H
#define ORDER_H

#include "CppStandard.h"
#include "Constants.h"

namespace OUCH{

struct   ClientOrder
{
    public:
    uint64_t    timestamp;
    uint64_t    instrument_id;
    uint32_t    client_order_id;
};

//OUCH Client OrderEntry / Client -> Exchange
struct   AddOrder : public ClientOrder
{

    AddOrder()
    {
        quantity = 0;
        price = 0;
        tif = 0;
        side = static_cast<Side>(0);
    }
    public:
    uint32_t    quantity;
    uint32_t    price;
    uint8_t     tif;
    Side        side;
};

struct CancelOrder : public ClientOrder
{
    uint32_t    quantity;
    uint32_t    exchange_order_id;
};

struct ReplaceOrder : public ClientOrder
{
    uint32_t    quantity;
    uint32_t    exchange_order_id;
    uint32_t    price;
};


//OUCH OrderFeedback / Exchange -> Client

struct  OrderAccepted : public ClientOrder
{
    uint32_t    exchange_order_id;
};

struct  OrderRejected : public ClientOrder
{
    uint64_t    timestamp;
    std::string reason;
};

struct  OrderReplaced : public ClientOrder
{
    uint32_t    old_exchange_order_id;
    uint32_t    new_exchange_order_id;
    uint32_t    quantity;
    uint32_t    price;

};

struct  OrderCancelled : public ClientOrder
{
    uint64_t    timestamp;
    uint32_t    exchange_order_id;
};

struct OrderExecuted : public ClientOrder
{
    uint32_t    quantity;
    uint32_t    exchange_order_id;
    uint32_t    price;
};

struct OrderDeleted : public ClientOrder
{
    uint32_t    quantity;
    uint32_t    exchange_order_id;
};

using   OrderEntryEvent = std::variant<OUCH::AddOrder, OUCH::OrderReplaced, OUCH::OrderCancelled, OUCH::OrderDeleted, OUCH::OrderRejected, OUCH::OrderExecuted>;

namespace FEEDBACK
{
    using   OrderEntryFdbckEvent = std::variant<OUCH::OrderAccepted, OUCH::OrderReplaced, OUCH::OrderCancelled, OUCH::OrderDeleted, OUCH::OrderRejected, OUCH::OrderExecuted>;
}
}





enum class OrderStatus : uint8_t
{
    Accepted = 0, Rejected = 1, Deleted = 2
};
struct OrderConfirmation 
{
    uint64_t timestamp;
    uint64_t exchangeOrderId;
    uint32_t clientOrderId;
    uint32_t quantity;
    int32_t price;
    OrderStatus state;
};




namespace ITCH
{
    struct  ITCHMessage
    {
        ITCHMessage() = delete;

        ITCHMessage(uint64_t new_timestamp, uint32_t new_stock_locate, std::array<char, trading_engine::STOCK_SYMBOL_LENGTH> stock_symbol,  uint32_t new_exchange_order_id, uint64_t sequence)
        : creation_timestamp_(new_timestamp), exchange_order_id_(new_exchange_order_id), stock_locate_(new_stock_locate), sequence_(sequence), stock_symbol_(stock_symbol)
        {}

        inline uint64_t &getTimestamp()
        {
            return creation_timestamp_;
        }

        inline uint32_t &getExchangeOrderId()
        {
            return exchange_order_id_;
        }

        inline uint16_t &getStockLocate()
        {
            return stock_locate_;
        }

        inline uint64_t &getSequence()
        {
            return sequence_;
        }

        inline std::array<char, trading_engine::STOCK_SYMBOL_LENGTH> &getStockSymbol()
        {
            return stock_symbol_;
        }

        protected:
        uint64_t    creation_timestamp_;
        uint32_t    exchange_order_id_;
        uint16_t    stock_locate_;
        uint64_t    sequence_;
        std::array<char, trading_engine::STOCK_SYMBOL_LENGTH>       stock_symbol_;
        
    };
//E Message

struct   AddOrder : public ITCHMessage
{
    public:
    AddOrder() = delete;
    AddOrder(uint64_t timestamp, uint32_t stock_locate, std::array<char, trading_engine::STOCK_SYMBOL_LENGTH> stock_symbol, uint32_t exchange_order_id, uint64_t sequence, uint32_t price, uint32_t quantity, Side side, uint8_t tif)
    :   ITCHMessage(timestamp, stock_locate, stock_symbol, exchange_order_id, sequence), price_(price), quantity_(quantity), tif_(tif), side_(side) 

    {
    }

    inline uint32_t &getPrice()
    {
        return price_;
    }

    inline uint32_t &getQuantity()
    {
        return quantity_;
    }

    inline uint8_t &getTif()
    {
        return tif_;
    }

    inline Side &getSide()
    {
        return side_;
    }

    private:
    uint32_t    price_;
    uint32_t    quantity_;
    uint8_t     tif_;
    Side        side_;
};


//'D' OUCH Message Type
struct   DeleteOrder : public ITCHMessage
{
};

//'X' OUCH Message Type
struct   CancelOrder : public ITCHMessage
{
    CancelOrder() = delete;
    CancelOrder(uint64_t timestamp, uint32_t stock_locate, std::array<char, trading_engine::STOCK_SYMBOL_LENGTH> stock_symbol, uint32_t exchange_order_id, uint64_t sequence, uint32_t quantity = 0)
    :   ITCHMessage(timestamp, stock_locate, stock_symbol, exchange_order_id, sequence), quantity_(quantity)

    {
    }

    inline const uint32_t &getQuantity()
    {
        return quantity_;
    }

    private:
    uint32_t    quantity_;
};

//'U' OUCH Message Type
struct   ReplaceOrder : public ITCHMessage
{
    ReplaceOrder() = delete;
    ReplaceOrder(uint64_t timestamp, uint32_t stock_locate, std::array<char, trading_engine::STOCK_SYMBOL_LENGTH> stock_symbol, uint32_t old_exchange_order_id, uint64_t sequence, uint32_t new_exchange_order_id, uint32_t price = 0, uint32_t quantity = 0)
    : ITCHMessage(timestamp, stock_locate, stock_symbol, old_exchange_order_id, sequence), new_exchange_order_id_(new_exchange_order_id), quantity_(quantity), price_(price)
    {
    }

    inline const uint32_t &getNewExchangeOrderId()
    {
        return new_exchange_order_id_;
    }

    inline const uint32_t &getPrice()
    {
        return price_;
    }

    inline uint32_t &getQuantity()
    {
        return quantity_;
    }

    private:
    uint32_t    new_exchange_order_id_;
    uint32_t    quantity_;
    uint32_t    price_;
};


struct MatchedOrder : public ITCHMessage
{
    /*
    uint32_t    quantity;
    uint32_t    price;
    uint32_t    match_id;
    */
};

//'C' OUCH message type
struct ExecuteOrderWithPrice : public ITCHMessage
{
    /*
    uint32_t    quantity;
    uint32_t    price;
    uint32_t    match_id;*/
};

//'P' OUCH message type
struct ExecuteHiddenOrder : public ITCHMessage
{
    /*
    uint32_t    quantity;
    uint32_t    price;
    uint32_t    match_id;*/
};

//'B' OUCH Broken Trade
struct BrokenTrade : public ITCHMessage
{
    //uint32_t    match_id;
};

//Non Cross Trade
struct NonCrossTrade : public ITCHMessage
{
    /*
    uint32_t    quantity;
    uint32_t    price;
    uint32_t    match_id;*/
};

//Auction Report Message
struct AuctionReport
{
    /*
    uint32_t    quantity;
    uint32_t    price;*/
};

//#define UDPMulticastEvent std::monostate, ITCH::AddOrder, ITCH::OrderReplaced, ITCH::OrderCancelled, ITCH::OrderDeleted, ITCH::OrderExecuted, ITCH::OrderExecutedWithPrice


using MarketEvent =  std::variant<std::monostate, ITCH::AddOrder, ITCH::ReplaceOrder, ITCH::CancelOrder, 
                           ITCH::DeleteOrder, ITCH::MatchedOrder, ITCH::ExecuteOrderWithPrice>;

// std::expected<std::variant<OrderAdded>, std::array<char,100>>  makeITCH(std::variant<OrderAdded>    &)
// {

// }

}

#endif