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
    uint16_t                 _id;
    uint16_t                 _price;
    uint16_t                 _qty;
    uint8_t                  _event_type;
    uint8_t                  _side;
    uint32_t                 _instr;
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
            buy_orders.reserve(BAND_SIZE);
            sell_orders.reserve(BAND_SIZE);
        }

        std::pair<uint32_t, uint32_t>   topOfTheBook()
        {
            return std::pair(best_ask_idx, best_bid_idx);
        }

        void                            addOrder(Order order)
        {
            uint32_t    tick = static_cast<uint32_t>(order._price * 100.00);
            if (order._price > 0 && order._qty > 0)
            {
                if (order._side == Side::Buy)
                {   
                    if (tick <= BASE_BUYING_TICK)
                    {
                        buy_orders[BASE_BUYING_TICK - tick] += order._qty;
                    }
                    else
                    {
                        out_of_band_buy[tick] += order._qty;
                    }
                }
                else
                {
                    uint32_t    band_limit = BASE_SELLING_TICK + BAND_SIZE;
                    if (tick >= BASE_SELLING_TICK && tick <= band_limit)
                    {
                        sell_orders[tick - BASE_SELLING_TICK] += order._qty;
                    }
                    else
                    {
                        out_of_band_sell[tick] += order._qty;
                    }
                }
            }
        }
    private:
    std::vector<uint32_t>                     buy_orders;
    std::vector<uint32_t>                     sell_orders;
    std::unordered_map<uint32_t, uint32_t>    out_of_band_sell;
    std::unordered_map<uint32_t, uint32_t>    out_of_band_buy;
    uint32_t                                  best_bid_idx;
    uint32_t                                  best_ask_idx;

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