#ifndef     UTILS_H
#define     UTILS_H
// CPP Standard Library

#include "MemoryPool.h"

template<size_t ringSize, typename... T>
inline void    writeToExternalRing(RxRingBuffer<ringSize, T...>    &ring, auto data)
{
    bool written = false;
    while (!written)
    {
        if (std::is_same_v<std::decay_t<decltype(data)>, ITCH::AddOrder>)
        {
            std::cout << "Writting AddOrder Event ...\n";
        }
        
        if(((ring.head + 1) & (ringSize - 1)) != ring.tail)
        {
            std::cout << "Writting AddOrder Event...\n";
            auto head = ring.head.load(std::memory_order_acquire);
            ring.command_content[head] = data;
            ring.head.store((head+1)&(ringSize - 1), std::memory_order_release);
            written = true;
        }
    }
}

#endif