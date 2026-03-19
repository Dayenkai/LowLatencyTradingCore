#include "../../includes/HFTMemoryPool.h"
#include "../../includes/CppStandard.h"
#include "../../includes/FeedHandler.h"
#include "../../includes/PersonnalLibrary.h"

int  feedHandler(MemoryPool &memory)
{
    log("__IN FEED_HANDLER__ ! Yeah !");
    while (true)
    {
        
        if (!(((memory.rxRingDesc.tail) & (RX_RING_SIZE  - 1)) == memory.rxRingDesc.head ))
        {
            //memory.rxRingDesc.tail.load(std::memory_order_acquire);
            memory.rxRingDesc.tail.load(std::memory_order_acquire);
            std::cout << "Readind an entry !";
            for (size_t idx = 0; idx < memory.rxRingDesc.data[memory.rxRingDesc.tail].len; idx++)
            {
                std::cout << memory.rxRingDesc.data[memory.rxRingDesc.tail].addr[idx];
            }
            if (memory.rxRingDesc.data[memory.rxRingDesc.tail].len != 0)
            {
                std::cout << std::endl;
            }
            memory.rxRingDesc.tail.store((memory.rxRingDesc.tail + 1) & (RX_RING_SIZE - 1), std::memory_order_release);
        }
    }
    return 0;
}