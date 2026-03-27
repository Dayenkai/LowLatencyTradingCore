#include "../../includes/CppStandard.h"
#include "../../includes/PersonnalLibrary.h"
#include "../../includes/NicReplay.h"

int     NicReplay(std::vector<ChannelFile>   &channelContent, std::vector<MemoryPool> &memoryPool)
{
    std::cout << "in [" << __func__ << "]" << std::endl;

    std::string         token("");
    std::string         line;

    for (uint64_t i = 0; i < channelContent.size(); i++)
    {
        std::stringstream   channel_Content_Stream(channelContent[i]._channelContent);
        uint64_t            packetCount(0);
   
        while(std::getline(channel_Content_Stream, line) && ((memoryPool[i].rxRingDesc.head + 1) & (RX_RING_SIZE - 1)) != memoryPool[i].rxRingDesc.tail)
        {
            memoryPool[i].rxRingDesc.head.load(std::memory_order_acquire);
            memcpy(memoryPool[i].arena[packetCount], line.c_str(), line.length());
            memoryPool[i].rxRingDesc.data[packetCount & (RX_RING_SIZE - 1)].addr = memoryPool[i].arena[packetCount];
            memoryPool[i].rxRingDesc.data[packetCount & (RX_RING_SIZE - 1)].len  = line.length();
            ++packetCount;
            memoryPool[i].rxRingDesc.head.store((memoryPool[i].rxRingDesc.head + 1) & (RX_RING_SIZE - 1), std::memory_order_release);
        }
    }
    return 0;
}