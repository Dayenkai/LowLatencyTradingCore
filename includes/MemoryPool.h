#ifndef MEMORY_POOL_HEADER
#define MEMORY_POOL_HEADER

#include "CppStandard.h"
#include "Constants.h"
#include <cstdint>
#include <cstddef>
#include <atomic>

template<typename T>
void byteEncoder(uint64_t value, std::vector<T>& dest);

void    charToBytes(char *content, std::vector<uint8_t> wireData);

typedef struct alignas(64) RxDesc
{
    uint32_t                len;
    uint16_t                flags;
    uint8_t                 padding;
    uint8_t                 *addr;
}RxDesc;

template<typename T, size_t capacity>
struct RxRingBuffer
{
    std::atomic<uint64_t>               head = 0;
    std::atomic<uint64_t>               tail = 0;
    std::array<T, capacity>             data;
};

typedef struct alignas(64) FeedPacketHeader
{
    uint32_t    seq = 0;
    uint32_t    msg_cnt;
    uint8_t     flags;
}FeedPacketHeader;

typedef struct alignas(64) Msg
{
    uint32_t                 _id;
    uint32_t                 _instr;
    uint32_t                 _price;
    uint32_t                 _qty;
    uint8_t                  _event_type;
    uint8_t                  _side;
}Msg;

typedef struct alignas(64) Order
{
    Order()
    {
        
    }
    Order(uint32_t qty, uint32_t price, Side side, uint16_t id, Order_Type type) : _qty(qty), _price(price), _side(side), _id(id), _event_type(type)
    {
    }
    uint32_t        _qty;
    uint32_t        _price;
    uint16_t        _id;
    Side            _side;
    Order_Type     _event_type;
}Order;

typedef class OrderBook
{
    public:
        OrderBook()
        {
            buy_orders.resize(BAND_SIZE);
            sell_orders.resize(BAND_SIZE);
            out_of_band_buy.reserve(BAND_SIZE);
            out_of_band_sell.reserve(BAND_SIZE);
        }

        std::pair<uint32_t, uint32_t>   topOfTheBook()
        {
            return std::pair(best_ask_idx.second, best_bid_idx.second);
        }

        void                            addOrder(Msg order)
        {
            
            Side side = static_cast<Side>(order._side);
            if (order._price > 0 && order._qty > 0)
            {
                if (side == Side::Buy)
                {   
                    if (order._price <= BASE_BUYING_TICK)
                    {
                        buy_orders[BASE_BUYING_TICK - order._price] += order._qty;
                        if (best_bid_idx.first && buy_orders[BASE_BUYING_TICK - best_bid_idx.second] > 0 && order._price > buy_orders[BASE_BUYING_TICK - best_bid_idx.second])
                        {
                            best_bid_idx.first = true;
                            //best_buy_idx.second = order._price;
                        }
                    }
                    else
                    {
                        out_of_band_buy[order._price] += order._qty;
                        // if (out_of_band_buy[order._price] > 0 && order._price > buy_orders[BASE_BUYING_TICK - best_ask_idx])
                        // {
                        //     best_ask_idx.first = true;
                        //     besttick;
                        // }
                    }
                }
                else
                {
                    uint32_t    band_limit = BASE_SELLING_TICK + BAND_SIZE;
                    if (order._price >= BASE_SELLING_TICK && order._price <= band_limit)
                    {
                        sell_orders[order._price - BASE_SELLING_TICK] += order._qty;
                    }
                    else
                    {
                        out_of_band_sell[order._price] += order._qty;
                    }
                }
            }
        }
    private:
    alignas(64)std::vector<uint32_t>                     buy_orders;
    std::vector<uint32_t>                     sell_orders;
    std::unordered_map<uint32_t, uint32_t>    out_of_band_sell;
    std::unordered_map<uint32_t, uint32_t>    out_of_band_buy;
    std::pair<bool,uint32_t>                  best_bid_idx{false,0};
    std::pair<bool,uint32_t>                  best_ask_idx{false,0};

}OrderBook;


typedef struct alignas(64) MemoryPool
{
    public:
    RxRingBuffer<RxDesc, RX_RING_SIZE>                    rxRingDesc;
    FeedPacketHeader                                      packetHeader;
    Msg                                                   msg;
    uint8_t                                               arena[PACKET_NB][BUFFER_SIZE];
}MemoryPool;

#endif