#include "../../includes/HFTMemoryPool.h"
#include "../../includes/CppStandard.h"
#include "../../includes/FeedHandler.h"
#include "../../includes/PersonnalLibrary.h"

int  feedHandler(MemoryPool &memory)
{
    log("__IN FEED_HANDLER__ ! Yeah !");
    while (true)
    {
        if (!((memory.rxRingDesc.tail + 1) % RX_RING_SIZE == memory.rxRingDesc.head))
        {
            memory.rxRingDesc.tail.load(std::memory_order_acquire);
            memory.rxRingDesc.tail =  (memory.rxRingDesc.tail + 1) % (RX_RING_SIZE - 1);
            std::cout << "Readind an entry !";
            memory.rxRingDesc.tail.load(std::memory_order_release);
        }
    }
    return 0;
}