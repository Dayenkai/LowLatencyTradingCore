#ifndef ORDER_H
#define ORDER_H


struct   ClientOrder
{

    public:
    uint64_t    timestamp;
    uint64_t    instrument_id;
    uint32_t    client_order_id;
};

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
};

struct ExecuteOrder : public ClientOrder
{
    uint32_t    quantity;
    std::string type;
};

struct DeleteOrder : public ClientOrder
{
    uint32_t    quantity;
};

struct OrderConfirmation 
{
    uint64_t timestamp;
    uint64_t exchangeOrderId;
    uint32_t clientOrderId;
    uint32_t quantity;
    int32_t price;
    OrderState state;
};


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