#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

class   OrderEntryGateway
{

    public:
    OrderEntryGateway()
    {
        seq = 0;//Initializing Sequence Assignment
    }
    void    AssignSequence();

    int    Listen();

    int     SendOrder();
    // bool    Parse(std::array<T, capacity>             data)
    // {
    //     inline void    parse(RxDesc &desc, Msg &msg)
    //     {
    //         fillValueLe<uint32_t>(msg._id, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[0]));// static_cast<uint32_t>(*desc.addr) | static_cast<uint32_t>(*(desc.addr + static_cast<size_t>(dataSize[0]/4))) << CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 2)) << 2 * CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 3)) << 3 * CHAR_BIT;
    //         fillValueLe<uint32_t>(msg._instr, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[1]));// static_cast<uint32_t>(*desc.addr + 4) | static_cast<uint32_t>(*(desc.addr + 5)) <<  CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 6)) << 2 * CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 7)) << 3 * CHAR_BIT;
    //         fillValueLe<uint8_t>(msg._side, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[3])); //static_cast<uint8_t>(*(desc.addr + 12));
    //         fillValueLe<uint8_t>(msg._event_type, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[4]));
    //         fillValueLe<uint8_t>(msg._kind, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[5]));
    //         fillValueLe<uint32_t>(msg.price_, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[6]));
    //         fillValueLe<uint32_t>(msg.qty_, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[7]));
    //     }
    // }

    private:
    static  uint64_t seq;
};