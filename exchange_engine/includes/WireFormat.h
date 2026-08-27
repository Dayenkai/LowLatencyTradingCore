#ifndef WIRE_FORMAT
#define WIRE_FORMAT

#include <cstdint>

struct Packet
{
    uint64_t seq;
    uint64_t qty;
    uint64_t price;
    uint32_t id;
    uint8_t  type;
};

#endif