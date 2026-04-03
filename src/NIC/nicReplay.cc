#include "../../includes/CppStandard.h"
#include "../../includes/PersonnalLibrary.h"
#include "../../includes/NicReplay.h"
#include "../../includes/MemoryPool.h"


template<typename T>
void byteEncoder(uint64_t value, std::vector<T>& dest)
{
    switch(sizeof(T))
    {
        case sizeof(uint8_t):
            dest.emplace_back(value & 0xFF);
        break;
        case sizeof(uint16_t):
        dest.emplace_back(value & 0xFF);
        dest.emplace_back(value & 0xFFFF);
        break;
        case sizeof(uint32_t):
        dest.emplace_back(value & 0xFF);
        dest.emplace_back(value & 0xFFFF);
        dest.emplace_back(value & 0xFFFFFF);
        dest.emplace_back(value & 0xFFFF);
        break;
        case sizeof(uint64_t):
        dest.emplace_back(value & 0xFF);
        dest.emplace_back(value & 0xFFFF);
        dest.emplace_back(value & 0xFFFFFF);
        dest.emplace_back(value & 0xFFFFFFFF);
        dest.emplace_back(value & 0xFFFFFFFFFF);
        dest.emplace_back(value & 0xFFFFFFFFFFFF);
        dest.emplace_back(value & 0xFFFFFFFFFFFFFF);
        dest.emplace_back(value & 0xFFFFFFFFFFFFFFFF);
        break;
    }
}

//Packet Fixed Format
void    charToBytes(char *content, std::vector<uint8_t> wireData)
{
    std::string     buffer(content);
    std::ifstream   streamBuffer(buffer);
    std::string     line("");

    buffer = "";
    buffer.reserve(BUFFER_SIZE);
    while (std::getline(streamBuffer, line))
    {
        if (line.length() > PACKET_LENGTH)
        {
            uint32_t       seq   = std::stoi(line.substr(0, 3));
            std::string    instr = line.substr(3, 4);
            uint32_t       id    = std::stoi(line.substr(7, 3));
            uint8_t        side  = std::stoi(line.substr(10, 1));
            uint32_t       price = std::stoi(line.substr(11, 8));
            uint32_t       qty   = std::stoi(line.substr(19, 8));

            byteEncoder(seq, wireData);
            byteEncoder(static_cast<uint32_t>(std::stoi(instr)), wireData);
            byteEncoder(id, wireData);
            byteEncoder(side, wireData);
            byteEncoder(price, wireData);
            byteEncoder(qty, wireData);
        }
    }
}

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