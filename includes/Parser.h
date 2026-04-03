#ifndef NIC_REPLAY
#define NIC_REPLAY

#include "CppStandard.h"
#include "MemoryPool.h"
#include "PersonnalLibrary.h"
#include "NicReplay.h"

struct  Parser
{
    template<typename T>
    static void byteEncoder(uint64_t value, std::vector<T>& dest)
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
    static void    charToBytes(char *content, std::vector<uint8_t> wireData)
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
};

#endif