#include "../../includes/CppStandard.h"
#include "../../includes/PersonnalLibrary.h"
#include "../../includes/NicReplay.h"
#include "../../includes/MemoryPool.h"


int     NicReplay(std::vector<Channel>   &channels, std::vector<MemoryPool> &memoryPool)
{
    std::cout << "in [" << __func__ << "]" << std::endl;

    for (uint64_t i = 0; i < channels.size(); i++)
    {
        uint64_t            packetCount(0);
        uint64_t            byteNb(0);
        bool                write(0);

        write = byteNb + static_cast<uint8_t>(BYTES_NB_PER_ENTRY) < channels[i]._channelContent.size();
        while (write)
        {
            while(write && (((memoryPool[i].rxRingDesc.head + 1) & (RX_RING_SIZE - 1)) != memoryPool[i].rxRingDesc.tail))
            {
               
                memoryPool[i].rxRingDesc.head.load(std::memory_order_acquire);
                int y = byteNb;
                memcpy(memoryPool[i].arena[packetCount], reinterpret_cast<void*>(channels[i]._channelContent.data() + byteNb), sizeof(uint32_t));
                memcpy(memoryPool[i].arena[packetCount] + sizeof(uint32_t), reinterpret_cast<void*>(channels[i]._channelContent.data() + byteNb + sizeof(uint32_t)), sizeof(uint8_t));
                memcpy(memoryPool[i].arena[packetCount] + sizeof(uint32_t) + sizeof(uint8_t), reinterpret_cast<void*>(channels[i]._channelContent.data() + byteNb + sizeof(uint32_t) + sizeof(uint8_t)), sizeof(uint8_t));
                memcpy(memoryPool[i].arena[packetCount] + sizeof(uint32_t) + 2 * sizeof(uint8_t), reinterpret_cast<void*>(channels[i]._channelContent.data() + byteNb + sizeof(uint32_t) + 2 * sizeof(uint8_t)), sizeof(uint8_t));
                memcpy(memoryPool[i].arena[packetCount] + sizeof(uint32_t) + 3 * sizeof(uint8_t), reinterpret_cast<void*>(channels[i]._channelContent.data() + byteNb + sizeof(uint32_t) + 3 * sizeof(uint8_t)), sizeof(uint32_t));
                memcpy(memoryPool[i].arena[packetCount] + 2 * sizeof(uint32_t) + 3 * sizeof(uint8_t), reinterpret_cast<void*>(channels[i]._channelContent.data() + byteNb + 2 * sizeof(uint32_t) + 3 * sizeof(uint8_t)), sizeof(uint8_t));
                memcpy(memoryPool[i].arena[packetCount] + 3 * sizeof(uint32_t), reinterpret_cast<void*>(channels[i]._channelContent.data() + byteNb + 3 * sizeof(uint32_t)), sizeof(uint8_t));
                memcpy(memoryPool[i].arena[packetCount] + 3 * sizeof(uint32_t) + sizeof(uint8_t), reinterpret_cast<void*>(channels[i]._channelContent.data() + byteNb + 3 * sizeof(uint32_t) + sizeof(uint8_t)), sizeof(uint32_t));
                memcpy(memoryPool[i].arena[packetCount] + 4 * sizeof(uint32_t) + sizeof(uint8_t), reinterpret_cast<void*>(channels[i]._channelContent.data() + byteNb + 4 * sizeof(uint32_t) + sizeof(uint8_t)), sizeof(uint32_t));
                memoryPool[i].rxRingDesc.data[packetCount & (RX_RING_SIZE - 1)].addr = memoryPool[i].arena[packetCount];
                memoryPool[i].rxRingDesc.data[packetCount & (RX_RING_SIZE - 1)].len  = static_cast<uint8_t>(BYTES_NB_PER_ENTRY);                
                byteNb+=static_cast<uint64_t>(BYTES_NB_PER_ENTRY);
                ++packetCount;
                if (!(byteNb + static_cast<uint8_t>(BYTES_NB_PER_ENTRY) < channels[i]._channelContent.size()))
                {
                    write = false;
                }
                memoryPool[i].rxRingDesc.head.store((memoryPool[i].rxRingDesc.head + 1) & (RX_RING_SIZE - 1), std::memory_order_release);
                
            }
        }
        
    }
    return 0;
}