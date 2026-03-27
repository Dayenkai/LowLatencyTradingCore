#include "../../includes/HFTMemoryPool.h"
#include "../../includes/CppStandard.h"
#include "../../includes/FeedHandler.h"
#include "../../includes/PersonnalLibrary.h"
#include "../../includes/OrderBookManager.h"

void    parse(RxDesc &desc, FeedPacketHeader& packetHeader, Order& order, Msg &msg)
{
    
    uint32_t* p = (reinterpret_cast<uint32_t*>(desc.addr));
    std::cout << "seq = " << *p << std::endl;
    msg._instr = *(reinterpret_cast<uint32_t*>(desc.addr + sizeof(uint8_t)));
    order._id = *(reinterpret_cast<uint16_t*>(desc.addr + sizeof(uint8_t)*5));
    order._side = static_cast<Side>(reinterpret_cast<uint8_t>(*(desc.addr+sizeof(uint8_t)*9)));
    order._price = *(reinterpret_cast<uint32_t*>(desc.addr+sizeof(uint8_t)*10));
    order._qty = *(reinterpret_cast<uint32_t*>(desc.addr+sizeof(uint8_t)*18));
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
            
            parse(memory.rxRingDesc.data[memory.rxRingDesc.tail], packetHeader, marketOrder, msg);
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