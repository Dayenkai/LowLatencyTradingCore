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

#define public_OUCH_Feedback_Messages OUCH::OrderAdded, OUCH::OrderReplaced, OUCH::OrderCancelled, OUCH::OrderDeleted, OUCH::OrderRejected, OUCH::OrderExecuted
#define public_OUCH_Messages OUCH::AddOrder, OUCH::ReplaceOrder, OUCH::CancelOrder
#define UDPMulticastEvent ITCH::OrderAdded, ITCH::OrderReplaced, ITCH::OrderCancelled, ITCH::OrderDeleted, ITCH::OrderExecuted, ITCH::OrderExecutedWithPrice
}






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
    struct  ClientOrder
    {
        uint64_t    timestamp;
        uint32_t    exchange_order_id;
    };
//E Message

struct   OrderAdded : public ClientOrder
{
    public:
    uint32_t    quantity;
    uint32_t    price;
    uint8_t     tif;
    Side        side;
};


//'D' OUCH Message Type
struct   OrderDeleted : public ClientOrder
{
};

//'X' OUCH Message Type
struct   OrderCancelled : public ClientOrder
{
    uint32_t    quantity;
};

//'U' OUCH Message Type
struct   OrderReplaced : public ClientOrder
{
    uint32_t    old_exchange_order_id;
    uint32_t    new_exchange_order_id;
    uint32_t    quantity;
    uint32_t    price;
};

//'E' OUCH message Type
struct OrderExecuted : public ClientOrder
{
    uint32_t    quantity;
    uint32_t    price;
    uint32_t    match_id;
};

//'C' OUCH message type
struct OrderExecutedWithPrice : public ClientOrder
{
    uint32_t    quantity;
    uint32_t    price;
    uint32_t    match_id;
};

//'P' OUCH message type
struct HiddenOrderExecuted : public ClientOrder
{
    uint32_t    quantity;
    uint32_t    price;
    uint32_t    match_id;
};

//'B' OUCH Broken Trade
struct BrokenTrade : public ClientOrder
{
    uint32_t    match_id;
};

//Non Cross Trade
struct NonCrossTrade : public ClientOrder
{
    uint32_t    quantity;
    uint32_t    price;
    uint32_t    match_id;
};

//Auction Report Message
struct AuctionReport
{
    uint32_t    quantity;
    uint32_t    price;
};

#define public_ITCH_Messages ITCH::OrderAdded, ITCH::OrderReplaced, ITCH::OrderCancelled, ITCH::OrderDeleted, ITCH::OrderExecuted, ITCH::OrderExecutedWithPrice

}

// struct CancelOrder : public ClientOrder
// {
//     /* data */
// };

// struct DeleteOrder : public ClientOrder
// {
//     public:
//     uint32_t    quantity;
//     uint32_t    price;
//     /* data */
// };

#endif