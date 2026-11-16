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
        if(((ring.head + 1) & (ringSize - 1)) != ring.tail)
        {
            auto head = ring.head.load(std::memory_order_acquire);
            ring.command_content[head] = data;
            ring.head.store((head+1)&(ringSize - 1), std::memory_order_release);
            written = true;
        }
    }
}

#endif