#include "../../includes/MemoryPool.h"
#include "../../includes/CppStandard.h"
#include "../../includes/FeedHandler.h"
#include "../../includes/NicReplay.h"
#include "../../includes/MatchingEngine.h"

void    buildOrder(Msg &msg, Order &order)
{
    order._qty = msg._qty;
    order._price = msg._price;
    order._id = msg._id;
    order._side = static_cast<Side>(msg._side);
    order._event_type = static_cast<Order_Type>(msg._event_type);
}

void    parse(RxDesc &desc, Msg &msg)
{
    msg._id            = static_cast<uint32_t>(*desc.addr) | static_cast<uint32_t>(*(desc.addr + 1)) << CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 2)) << 2 * CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 3)) << 3 * CHAR_BIT;
    msg._instr         = static_cast<uint32_t>(*desc.addr + 4) | static_cast<uint32_t>(*(desc.addr + 5)) <<  CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 6)) << 2 * CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 7)) << 3 * CHAR_BIT;
    msg._side          = static_cast<uint8_t>(*(desc.addr + 12));
    msg._event_type    = static_cast<uint8_t>(*(desc.addr + 13));
    msg._price         = static_cast<uint32_t>(*(desc.addr + 14)) | static_cast<uint32_t>(*(desc.addr + 15)) <<  CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 16)) << 2 * CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 17)) << 3 * CHAR_BIT;
    msg._qty           = static_cast<uint32_t>(*(desc.addr + 18)) | static_cast<uint16_t>(*(desc.addr + 19)) <<  CHAR_BIT | static_cast<uint16_t>(*(desc.addr + 20)) << 2 * CHAR_BIT | static_cast<uint16_t>(*(desc.addr + 21)) << 3 * CHAR_BIT;
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