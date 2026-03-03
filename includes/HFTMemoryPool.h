#ifndef MEMORY_POOL_HEADER
#define MEMORY_POOL_HEADER

#include "Constants.h"
#include <cstdint>
typedef struct RxDesc
{
    uint64_t    addr;
    uint32_t    len;
    uint32_t    flags;
}RxDesc;

typedef struct RxRingBuffer
{
    RxRingBuffer(){
        //rxDesc = 
    }
    uint64_t    *head;
    uint64_t    *tail;
    RxDesc      rxDesc[RX_RING_SIZE];
}rxRingBuffer;

typedef struct FeedPacketHeader
{
    uint64_t    seq;
    uint32_t    msg_cnt;
    uint32_t    flags;
}FeedPacketHeader;

typedef struct Msg
{
    uint16_t     id;
    uint16_t     price;
    uint16_t     qty;
    uint8_t      side;
    uint8_t      event_type;
}Msg;


typedef struct   MemoryPool
{
    public:

    private:
    RxRingBuffer        RxRingDesc;
    alignas(64) int8_t  arena[PACKET_NUMBER][BUFFER_SIZE];
    FeedPacketHeader    packetHeader;
    Msg                 msg;
}MemoryPool;

#endif