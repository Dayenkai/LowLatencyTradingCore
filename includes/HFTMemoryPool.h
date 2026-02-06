#include "Constants.h"
#include <cstdint>

typedef struct RxDesc
{
    uint64_t    addr;
    uint32_t    len;
    uint32_t    flags;
}RxDesc;

typedef struct FeedPacketHeader
{
    uint64_t    seq;
    uint32_t    msg_cnt;
    uint32_t    flags;
}FeedPacketHeader;

typedef struct Msg
{
    uint16_t     id;
    uint8_t      type;
    uint16_t     price;
    uint16_t     qty;
    uint8_t      padding;
}Msg;


struct   MemoryPool
{
    public:

    private:
    RxDesc              rxDesc;
    alignas(64) int8_t  arena[PACKET_NUMBER][BUFFER_SIZE];
    FeedPacketHeader    packetHeader;
    Msg                 msg;
};