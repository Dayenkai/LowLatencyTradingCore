#include "../../includes/CppStandard.h"
#include "../../includes/PersonnalLibrary.h"
#include "../../includes/NicReplay.h"
#include "../../includes/MemoryPool.h"
#include "../../includes/Utils.h"



int     NicReplay(std::vector<Channel>   &channels, std::vector<MemoryPool> &memoryPool, uint32_t &coreId)
{
    pthread_t   current_thread = pthread_self();
    pinThreadToCore(current_thread, coreId);


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
                memcpy(memoryPool[i].arena[packetCount] + dataOffsets[0], reinterpret_cast<void*>(channels[i]._channelContent.data() + byteNb), dataSize[0]);//SequenceId
                memcpy(memoryPool[i].arena[packetCount] + dataOffsets[1], reinterpret_cast<void*>(channels[i]._channelContent.data() + byteNb + dataOffsets[1]), sizeof(uint8_t));//First Letter of MarketId
                memcpy(memoryPool[i].arena[packetCount] + dataOffsets[1] + sizeof(uint8_t), reinterpret_cast<void*>(channels[i]._channelContent.data() + byteNb + dataOffsets[1] + sizeof(uint8_t)), sizeof(uint8_t));//Second Letter of MarketId
                memcpy(memoryPool[i].arena[packetCount] + dataOffsets[1] + 2 * sizeof(uint8_t), reinterpret_cast<void*>(channels[i]._channelContent.data() + byteNb + dataOffsets[1] + 2 * sizeof(uint8_t)), sizeof(uint8_t));//Third Letter of MarketId
                memcpy(memoryPool[i].arena[packetCount] + dataOffsets[1] + 3 * sizeof(uint8_t), reinterpret_cast<void*>(channels[i]._channelContent.data() + byteNb + dataOffsets[1] + 3 * sizeof(uint8_t)), sizeof(uint8_t));//Fourth Letter of MarketId
                memcpy(memoryPool[i].arena[packetCount] + dataOffsets[2], reinterpret_cast<void*>(channels[i]._channelContent.data() + byteNb + dataOffsets[2]), dataSize[2]);//ClientId
                memcpy(memoryPool[i].arena[packetCount] + dataOffsets[3], reinterpret_cast<void*>(channels[i]._channelContent.data() + byteNb + dataOffsets[3]), dataSize[3]);//Side
                memcpy(memoryPool[i].arena[packetCount] + dataOffsets[4], reinterpret_cast<void*>(channels[i]._channelContent.data() + dataOffsets[4]), dataSize[4]);//Type
                memcpy(memoryPool[i].arena[packetCount] + dataOffsets[5], reinterpret_cast<void*>(channels[i]._channelContent.data() + dataOffsets[5]), dataSize[5]);//Order Kind
                memcpy(memoryPool[i].arena[packetCount] + dataOffsets[6], reinterpret_cast<void*>(channels[i]._channelContent.data() + byteNb + dataOffsets[6]), dataSize[6]);//Price
                memcpy(memoryPool[i].arena[packetCount] + dataOffsets[7], reinterpret_cast<void*>(channels[i]._channelContent.data() + byteNb + dataOffsets[7]), dataSize[7]);//Quantity
                memoryPool[i].rxRingDesc.data[packetCount & (RX_RING_SIZE - 1)].addr = memoryPool[i].arena[packetCount];
                memoryPool[i].rxRingDesc.data[packetCount & (RX_RING_SIZE - 1)].len  = static_cast<uint8_t>(BYTES_NB_PER_ENTRY);
                byteNb+=static_cast<uint64_t>(BYTES_NB_PER_ENTRY);
                ++packetCount;
                if (!(byteNb + static_cast<uint8_t>(BYTES_NB_PER_ENTRY) <= channels[i]._channelContent.size()))
                {
                    write = false;
                }
                memoryPool[i].rxRingDesc.head.store((memoryPool[i].rxRingDesc.head + 1) & (RX_RING_SIZE - 1), std::memory_order_release);
            }
        }
    }
    return 0;
}