#include "../../includes/CppStandard.h"
#include "../../includes/PersonnalLibrary.h"
#include "../../includes/NicReplay.h"

int     NicReplay(std::vector<ChannelFile>   &channelContent, std::vector<MemoryPool> &memoryPool)
{
    //std::cout << "File processed by Nic with content : " << channelContent << std::endl;

    std::string         token("");
    uint64_t            packetCount(0);
    std::string         line;

    for (uint64_t i = 0; i < channelContent.size(); i++)
    {
        std::stringstream   channel_Content_Stream(channelContent[i]._channelContent);

        while(std::getline(channel_Content_Stream, line) && ((memoryPool[i].rxRingDesc.head + 1) & RX_RING_SIZE - 1) != memoryPool[i].rxRingDesc.tail)
        {
            memoryPool[i].rxRingDesc.head.load(std::memory_order_acquire);
            std::cout << "Writing an entry !!!" << std::endl;
            memcpy(memoryPool[i].arena[packetCount], line.c_str(), line.length());
            memoryPool[i].rxRingDesc.data[packetCount & (RX_RING_SIZE - 1)].addr = memoryPool[i].arena[packetCount];
            memoryPool[i].rxRingDesc.data[packetCount & (RX_RING_SIZE - 1)].len  = line.length();
            memoryPool[i].rxRingDesc.head++;
            memoryPool[i].rxRingDesc.head.load(std::memory_order_release);
        }
    }
    // std::stringstream   channel_Content_Stream(channelContent);
    // while(std::getline(channel_Content_Stream, line))
    // {
    //     uint8_t     dataIndex(0);
    //     std::vector<std::string>    tokens;

    //     tokens.reserve(PACKET_COLUMNS_NB);
    //     while (std::getline(std::stringstream(line), token, ' '))
    //     {
    //         if (dataIndex < PACKET_COLUMNS_NB)
    //         {
    //             //pool.arena[packetCount][dataIndex] = 
    //         }    
    //     }
    // }
    return 0;
}