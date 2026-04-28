#include "../../includes/MemoryPool.h"
#include "../../includes/CppStandard.h"
#include "../../includes/FeedHandler.h"
#include "../../includes/NicReplay.h"
#include "../../includes/OrderBookManager.h"

void    buildOrder(Msg &msg, Order &order)
{
    order._qty = msg._qty;
    order._price = msg._price;
    order._id = msg._id;
    order._side = static_cast<Side>(msg._side);
    order._event_type = static_cast<Order_Type>(msg._event_type);

    std::cout << "order id = " << order._id << ", order qty = " << order._qty << ", and price = " << order._price << std::endl;
}

void    parse(RxDesc &desc, Msg &msg)
{
    msg._id            = static_cast<uint32_t>(*desc.addr) | static_cast<uint32_t>(*(desc.addr + 1)) << CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 2)) << 2 * CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 3)) << 3 * CHAR_BIT;
    msg._instr         = static_cast<uint32_t>(*desc.addr + 4) | static_cast<uint32_t>(*(desc.addr + 5)) <<  CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 6)) << 2 * CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 7)) << 3 * CHAR_BIT;
    msg._side          = static_cast<uint8_t>(*(desc.addr + 12));
    msg._event_type    = static_cast<uint8_t>(*(desc.addr + 13));
    msg._price         = static_cast<uint32_t>(*(desc.addr + 14)) | static_cast<uint32_t>(*(desc.addr + 15)) <<  CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 16)) << 2 * CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 17)) << 3 * CHAR_BIT;
    msg._qty           = static_cast<uint32_t>(*(desc.addr + 18)) | static_cast<uint16_t>(*(desc.addr + 19)) <<  CHAR_BIT | static_cast<uint16_t>(*(desc.addr + 20)) << 2 * CHAR_BIT | static_cast<uint16_t>(*(desc.addr + 21)) << 3 * CHAR_BIT;

    //std::cout << "order id = " << msg._id << ", order qty = " << msg._qty << ", and price = " << msg._price << std::endl;
}

int  feedHandler(MemoryPool &memory)
{
    TimePoint        start_time; 
    OrderBook        book;
    Msg              msg;
    int              orderNb(0);
    bool             started(false);

    
    while (true)
    {
        if (!started && !(((memory.rxRingDesc.tail) & (RX_RING_SIZE  - 1)) == memory.rxRingDesc.head ))
        {
            start_time = Clock::now();
            started = true;
        }
        if (!(((memory.rxRingDesc.tail) & (RX_RING_SIZE  - 1)) == memory.rxRingDesc.head ))
        {
            memory.rxRingDesc.tail.load(std::memory_order_acquire);
            if (memory.rxRingDesc.data[memory.rxRingDesc.tail].len != 0)
            {
                parse(memory.rxRingDesc.data[memory.rxRingDesc.tail], msg);
                //buildOrder(msg, marketOrder);
                //auto    start = Clock::now();
                book.addOrder(msg);
                //auto    end =   Clock::now();
                //std::cout << "Book Update : " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << " ns." << std::endl;
                
                ++orderNb;
                //std::cout << "ORDER NB" << orderNb << std::endl;
            }
            memory.rxRingDesc.tail.store((memory.rxRingDesc.tail + 1) & (RX_RING_SIZE - 1), std::memory_order_release); 
            if (orderNb == 10)
            {
                auto end = Clock::now();
                auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_time).count();
                //std::cout << "Data processing took " << ns << " ns\n" << std::endl;
            }
        }
    }
    return 0;
}