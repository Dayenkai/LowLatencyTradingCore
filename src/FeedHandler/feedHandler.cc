#include "../../includes/MemoryPool.h"
#include "../../includes/CppStandard.h"
#include "../../includes/FeedHandler.h"
#include "../../includes/NicReplay.h"
#include "../../includes/OrderBookManager.h"

void    parse(RxDesc &desc, Msg &msg)
{
    desc.addr;
}

int  feedHandler(MemoryPool &memory)
{
    FeedPacketHeader packetHeader;
    OrderBook        book;
    Order            marketOrder;
    Msg              msg;

    log("__IN FEED_HANDLER__ ! Yeah !");
    while (true)
    {
        if (!(((memory.rxRingDesc.tail) & (RX_RING_SIZE  - 1)) == memory.rxRingDesc.head ))
        {
            memory.rxRingDesc.tail.load(std::memory_order_acquire);
            std::cout << "Readind an entry !";
            
            parse(memory.rxRingDesc.data[memory.rxRingDesc.tail], msg);
            book.addOrder(marketOrder);
            for (size_t idx = 0; idx < memory.rxRingDesc.data[memory.rxRingDesc.tail].len; idx++)
            {
                std::cout << memory.rxRingDesc.data[memory.rxRingDesc.tail].addr[idx];
            }
            if (memory.rxRingDesc.data[memory.rxRingDesc.tail].len != 0)
            {
                std::cout << std::endl;
            }
            std::pair<uint32_t, uint32_t> best_bid_best_ask = book.topOfTheBook();
            std::cout << "best bid : " << best_bid_best_ask.first << ", best ask : " << best_bid_best_ask.second << std::endl;
            memory.rxRingDesc.tail.store((memory.rxRingDesc.tail + 1) & (RX_RING_SIZE - 1), std::memory_order_release);
        }
    }
    return 0;
}