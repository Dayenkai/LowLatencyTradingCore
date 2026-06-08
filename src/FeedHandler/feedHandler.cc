#include "../../includes/MemoryPool.h"
#include "../../includes/CppStandard.h"
#include "../../includes/FeedHandler.h"
#include "../../includes/NicReplay.h"
#include "../../includes/MatchingEngine.h"

template<typename T>
inline void    fillValueLe(T &dest, std::byte* val)
{
    memcpy(&dest, val, sizeof(dest));

    if (std::endian::native == std::endian::big)
    {
        dest = std::byteswap(dest);
    }
}

void    buildOrder(Msg &msg, Order &order)
{
    order._qty = msg._qty;
    order._price = msg._price;
    order._id = msg._id;
    order._side = static_cast<Side>(msg._side);
    order._event_type = static_cast<Order_Type>(msg._event_type);
}

inline void    parse(RxDesc &desc, Msg &msg)
{
    fillValueLe<uint32_t>(msg._id, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[0]));// static_cast<uint32_t>(*desc.addr) | static_cast<uint32_t>(*(desc.addr + static_cast<size_t>(dataSize[0]/4))) << CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 2)) << 2 * CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 3)) << 3 * CHAR_BIT;
    fillValueLe<uint32_t>(msg._instr, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[1]));// static_cast<uint32_t>(*desc.addr + 4) | static_cast<uint32_t>(*(desc.addr + 5)) <<  CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 6)) << 2 * CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 7)) << 3 * CHAR_BIT;
    fillValueLe<uint8_t>(msg._side, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[3])); //static_cast<uint8_t>(*(desc.addr + 12));
    fillValueLe<uint8_t>(msg._event_type, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[4]));
    fillValueLe<uint8_t>(msg._kind, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[5]));
    fillValueLe<uint32_t>(msg._price, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[6]));
    fillValueLe<uint32_t>(msg._qty, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[7]));
}

int  feedHandler(MemoryPool &pool, uint32_t &coreId)
{
    pthread_t   current_thread = pthread_self();
    pinThreadToCore(current_thread, coreId);

    TimePoint        start_time;
    MatchingEngine   matchingEngine;
    Msg              msg;
    int              orderNb(0);
    bool             started(false);


    while (true)
    {
        if (!started && !(((pool.rxRingDesc.tail) & (RX_RING_SIZE  - 1)) == pool.rxRingDesc.head ))
        {
            start_time = Clock::now();
            started = true;
        }
        if (!(((pool.rxRingDesc.tail) & (RX_RING_SIZE  - 1)) == pool.rxRingDesc.head ))
        {
            pool.rxRingDesc.tail.load(std::memory_order_acquire);
            if (pool.rxRingDesc.data[pool.rxRingDesc.tail].len != 0)
            {
                auto start = Clock::now();
                parse(pool.rxRingDesc.data[pool.rxRingDesc.tail], msg);
                auto end    = Clock::now();
                //std::cout << "Parsing takes : " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << std::endl;
                //buildOrder(msg, marketOrder);
                //auto    start = Clock::now();
                //std::cout << "YEaah" << std::endl;
                //std::cout << "Order " << (static_cast<Side>(msg._side) == Side::Buy ? "Buy" : "Sell") << " price is : " << msg._price << " and qty is " << msg._qty << std::endl;
                matchingEngine.handleOrder(msg);
                //auto    end =   Clock::now();
                //std::cout << "Book Update : " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << " ns." << std::endl;
                ++orderNb;
                //std::cout << "ORDER NB" << orderNb << std::endl;
            }
            pool.rxRingDesc.tail.store((pool.rxRingDesc.tail + 1) & (RX_RING_SIZE - 1), std::memory_order_release); 
            if (orderNb == 10)
            {
                matchingEngine.getOrderBook().ListOrder(Side::Buy);
                matchingEngine.getOrderBook().ListOrder(Side::Sell);
                //std::cout << "Data processing took " << ns << " ns\n" << std::endl;
            }
        }
    }
    return 0;
}