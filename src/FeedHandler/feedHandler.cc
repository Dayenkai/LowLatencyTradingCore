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

    //std::cout << "order id = " << order._id << ", order qty = " << order._qty << ", and price = " << order._price << std::endl;
}

void    parse(RxDesc &desc, Msg &msg)
{
    msg._id            = static_cast<uint32_t>(*desc.addr) | static_cast<uint8_t>(*(desc.addr + 1)) | static_cast<uint8_t>(*(desc.addr + 2)) | static_cast<uint8_t>(*(desc.addr + 3));
    msg._instr         = static_cast<uint32_t>(*desc.addr + 4) | static_cast<uint8_t>(*(desc.addr + 5)) | static_cast<uint8_t>(*(desc.addr + 6)) | static_cast<uint8_t>(*(desc.addr + 7));
    msg._price         = static_cast<uint16_t>(*(desc.addr + 8)) | static_cast<uint16_t>(*(desc.addr + 9)) | static_cast<uint16_t>(*(desc.addr + 10)) | static_cast<uint16_t>(*(desc.addr + 11));
    msg._qty           = static_cast<uint16_t>(*(desc.addr + 12)) | static_cast<uint16_t>(*(desc.addr + 13)) | static_cast<uint16_t>(*(desc.addr + 14)) | static_cast<uint16_t>(*(desc.addr + 15));
    msg._event_type    = static_cast<uint8_t>(*(desc.addr + 16));
    msg._side          = static_cast<uint8_t>(*(desc.addr + 17));
}

int  feedHandler(MemoryPool &memory)
{
    TimePoint start_time; 
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
                book.addOrder(msg);
                ++orderNb;
                //std::cout << "ORDER NB" << orderNb << std::endl;
            }
            memory.rxRingDesc.tail.store((memory.rxRingDesc.tail + 1) & (RX_RING_SIZE - 1), std::memory_order_release); 
            if (orderNb == 10)
            {
                auto end = Clock::now();
                auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_time).count();
                std::cout << ns << " ns\n" << std::endl;
            }
        }
    }
    return 0;
}