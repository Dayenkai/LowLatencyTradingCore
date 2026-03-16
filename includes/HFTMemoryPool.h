#ifndef MEMORY_POOL_HEADER
#define MEMORY_POOL_HEADER

#include "CppStandard.h"
#include "Constants.h"
#include <cstdint>
#include <cstddef>
#include <atomic>
typedef struct RxDesc
{
    alignas(64) uint8_t    *addr;
    uint32_t                len;
    uint16_t                flags;
    uint8_t                 padding;
}RxDesc;

template<typename T, size_t capacity>
struct RxRingBuffer
{
    alignas(64) std::atomic<uint64_t>   head = 0;
    std::atomic<uint64_t>               tail = 0;
    std::array<T, capacity>             data;
};

typedef struct FeedPacketHeader
{
    uint64_t    seq;
    uint32_t    msg_cnt;
    uint32_t    flags;
}FeedPacketHeader;

typedef struct Msg
{
    alignas(64) uint16_t     id;
    uint8_t                  event_type;
    uint8_t                  side;
    uint16_t                 price;
    uint16_t                 qty;
}Msg;


typedef struct   MemoryPool
{
    public:    
    alignas(64) RxRingBuffer<RxDesc, RX_RING_SIZE>        rxRingDesc;
    alignas(64) uint8_t                                   arena[PACKET_NB][BUFFER_SIZE];
    alignas(64) FeedPacketHeader                          packetHeader;
    alignas(64) Msg                                       msg;
}MemoryPool;

#endif