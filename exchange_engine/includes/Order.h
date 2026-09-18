#ifndef ORDER_H
#define ORDER_H


struct   ClientOrder
{

    public:
    uint64_t    timestamp;
    uint32_t    client_order_id;
    uint32_t    instrument_id;
};

struct   AddOrder : public ClientOrder
{
    public:
    uint32_t    quantity;
    uint32_t    price;
    uint8_t     tif;
    Side        side;
};


#endif