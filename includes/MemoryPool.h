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
    uint8_t                  _kind;
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
    OrderQtyKind   _kind;
}Order;


typedef struct alignas(64) MemoryPool
{
    public:
    RxRingBuffer<RxDesc, RX_RING_SIZE>                    rxRingDesc;
    FeedPacketHeader                                      packetHeader;
    Msg                                                   msg;
    uint8_t                                               arena[PACKET_NB][BUFFER_SIZE];
}MemoryPool;

#endif