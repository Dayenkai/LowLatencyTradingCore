#include "../../includes/MemoryPool.h"
#include "../../includes/CppStandard.h"
#include "../../includes/FeedHandler.h"
#include "../../includes/NicReplay.h"
#include "../../includes/OrderBookManager.h"

void    buildOrder(Msg &msg, Order order)
{
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
    FeedPacketHeader packetHeader;
    OrderBook        book;
    Order            marketOrder;
    Msg              msg;
    Order            order;

    memory.rxRingDesc.tail;
    memory.rxRingDesc.head;
    // (memory.rxRingDesc.tail) & 1;
    log("__IN FEED_HANDLER__ ! Yeah !");
    while (true)
    {
        if (!(((memory.rxRingDesc.tail) & (RX_RING_SIZE  - 1)) == memory.rxRingDesc.head ))
        {
            memory.rxRingDesc.tail.load(std::memory_order_acquire);
            if (memory.rxRingDesc.data[memory.rxRingDesc.tail].len != 0)
            {
                std::cout << "Readind an entry : ";
                parse(memory.rxRingDesc.data[memory.rxRingDesc.tail], msg);
                buildOrder(msg, marketOrder);
                book.addOrder(marketOrder);
                for (size_t idx = 0; idx < memory.rxRingDesc.data[memory.rxRingDesc.tail].len; idx++)
                {
                    std::cout << (uint64_t)memory.rxRingDesc.data[memory.rxRingDesc.tail].addr[idx] << " ";
                }
                if (memory.rxRingDesc.data[memory.rxRingDesc.tail].len != 0)
                {
                    std::cout << std::endl;
                }
                std::pair<uint32_t, uint32_t> best_bid_best_ask = book.topOfTheBook();
                std::cout << "best bid : " << best_bid_best_ask.first << ", best ask : " << best_bid_best_ask.second << std::endl;
            }
            memory.rxRingDesc.tail.store((memory.rxRingDesc.tail + 1) & (RX_RING_SIZE - 1), std::memory_order_release);
        }
    }
    return 0;
}