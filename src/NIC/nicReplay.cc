#include "../../includes/CppStandard.h"
#include "../../includes/PersonnalLibrary.h"
#include "../../includes/NicReplay.h"
#include "../../includes/MemoryPool.h"


int     NicReplay(std::vector<Channel>   &channels, std::vector<MemoryPool> &memoryPool)
{
    std::cout << "in [" << __func__ << "]" << std::endl;

    uint64_t            byteNb(0);

    for (uint64_t i = 0; i < channels.size(); i++)
    {
        uint64_t            packetCount(0);

        while (byteNb + static_cast<uint8_t>(BYTES_NB_PER_ENTRY) < channels[i]._channelContent.size())
        {
            while((((memoryPool[i].rxRingDesc.head + 1) & (RX_RING_SIZE - 1)) != memoryPool[i].rxRingDesc.tail))
            {
                int y = byteNb;
                memoryPool[i].rxRingDesc.head.load(std::memory_order_acquire);
                memcpy(memoryPool[i].arena[packetCount], reinterpret_cast<void*>(channels[i]._channelContent.data()), sizeof(uint32_t));
                memcpy(memoryPool[i].arena[packetCount], reinterpret_cast<void*>(channels[i]._channelContent.data() + sizeof(uint32_t)), sizeof(uint8_t));
                memcpy(memoryPool[i].arena[packetCount], reinterpret_cast<void*>(channels[i]._channelContent.data() + sizeof(uint32_t) + sizeof(uint8_t)), sizeof(uint8_t));
                memcpy(memoryPool[i].arena[packetCount], reinterpret_cast<void*>(channels[i]._channelContent.data() + sizeof(uint32_t) + 2 * sizeof(uint8_t)), sizeof(uint8_t));
                memcpy(memoryPool[i].arena[packetCount], reinterpret_cast<void*>(channels[i]._channelContent.data() + sizeof(uint32_t) + 3 * sizeof(uint8_t)), sizeof(uint32_t));
                memcpy(memoryPool[i].arena[packetCount], reinterpret_cast<void*>(channels[i]._channelContent.data() + 2 * sizeof(uint32_t) + 3 * sizeof(uint8_t)), sizeof(uint8_t));
                memcpy(memoryPool[i].arena[packetCount], reinterpret_cast<void*>(channels[i]._channelContent.data() + 3 * sizeof(uint32_t)), sizeof(uint8_t));
                memcpy(memoryPool[i].arena[packetCount], reinterpret_cast<void*>(channels[i]._channelContent.data() + 3 * sizeof(uint32_t) + sizeof(uint8_t)), sizeof(uint32_t));
                memcpy(memoryPool[i].arena[packetCount], reinterpret_cast<void*>(channels[i]._channelContent.data() + 4 * sizeof(uint32_t) + sizeof(uint8_t)), sizeof(uint32_t));
                memoryPool[i].rxRingDesc.data[packetCount & (RX_RING_SIZE - 1)].addr = memoryPool[i].arena[packetCount];
                memoryPool[i].rxRingDesc.data[packetCount & (RX_RING_SIZE - 1)].len  = static_cast<uint8_t>(BYTES_NB_PER_ENTRY);
                for (y; y < BYTES_NB_PER_ENTRY; y++)
                {
                    std::cout << (uint64_t)89
                }
                byteNb+=BYTES_NB_PER_ENTRY;
                ++packetCount;
                memoryPool[i].rxRingDesc.head.store((memoryPool[i].rxRingDesc.head + 1) & (RX_RING_SIZE - 1), std::memory_order_release);  
            }
        }
        
    }
    return 0;
}