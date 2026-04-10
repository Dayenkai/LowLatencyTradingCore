#ifndef NIC_REPLAY
#define NIC_REPLAY

#include "CppStandard.h"
#include "MemoryPool.h"
#include "PersonnalLibrary.h"
#include "NicReplay.h"

typedef struct Channel
{
    public:
    Channel(std::string channelFile, char* channelContent, uint8_t number) : _channelFile(channelFile), _number(number)
    {
        charToBytes(channelContent, _channelContent);
    }

    template<typename T>
    void byteEncoder(T& value, std::vector<std::byte>& dest)
    {
        switch(sizeof(T))
        {
            case sizeof(uint8_t):
                dest.emplace_back(std::byte{*reinterpret_cast<uint8_t*>(&value)});
                std::cout << (uint64_t)_channelContent.back() << ";";

            break;
            case sizeof(uint16_t):
            dest.emplace_back(std::byte{static_cast<uint8_t>(value)});
            std::cout << (char)_channelContent.back() << ";";
            dest.emplace_back(std::byte{static_cast<uint8_t>(value << CHAR_BIT) });
            std::cout << (char)_channelContent.back() << ";";
            break;
            case sizeof(uint32_t):
            dest.emplace_back(std::byte{*reinterpret_cast<uint8_t*>(&value)});
            std::cout << (uint64_t)_channelContent.back() << ";";
            dest.emplace_back(std::byte{*(reinterpret_cast<uint32_t*>(&value)) >> CHAR_BIT});
            std::cout << (uint64_t)_channelContent.back() << ";";
            dest.emplace_back(std::byte{*(reinterpret_cast<uint32_t*>(&value)) >> 2 * CHAR_BIT});
            std::cout << (uint64_t)_channelContent.back() << ";";
            dest.emplace_back(std::byte{*(reinterpret_cast<uint32_t*>(&value)) >> 3 * CHAR_BIT});
            std::cout << (uint64_t)_channelContent.back() << ";";
            break;
            case sizeof(uint64_t):
            dest.emplace_back(std::byte{static_cast<uint8_t>(value)});
            dest.emplace_back(std::byte{static_cast<uint8_t>(value << CHAR_BIT)});
            dest.emplace_back(std::byte{static_cast<uint8_t>(value << 2 * CHAR_BIT)});
            dest.emplace_back(std::byte{static_cast<uint8_t>(value << 3 * CHAR_BIT)});
            dest.emplace_back(std::byte{static_cast<uint8_t>(value << 4 * CHAR_BIT)});
            dest.emplace_back(std::byte{static_cast<uint8_t>(value << 5 * CHAR_BIT)});
            dest.emplace_back(std::byte{static_cast<uint8_t>(value << 6 * CHAR_BIT)});
            dest.emplace_back(std::byte{static_cast<uint8_t>(value << 7 * CHAR_BIT)});
            break;
        }
    }

    //Packet Fixed Format
    template<typename T>
    void    charToBytes(char *content, std::vector<T>& wireData)
    {
        std::string         buffer(content);
        std::stringstream   streamBuffer(buffer);
        std::string         line("");

        while (std::getline(streamBuffer, line))
        {
            if (line.length() >= PACKET_ENTRY_SIZE)
            {
                uint32_t       seq   = std::stoi(line.substr(0, 9));
                std::string    instr = line.substr(9, 4);
                uint32_t       id    = std::stoi(line.substr(13, 9));
                uint8_t        side  = std::stoi(line.substr(22, 1));
                uint8_t        type  = std::stoi(line.substr(23, 1));
                uint32_t       price = std::stoi(line.substr(24, 9));
                uint32_t       qty   = std::stoi(line.substr(33, 9));

                std::cout << "Seq = " << seq << ", instr = " << instr << ", id = " << id << ", side = " << (uint64_t)side << ", type = " << (uint64_t)type << ", price = " << price << ", qty = " << qty << std::endl;
                byteEncoder(seq, wireData);
                byteEncoder(*(reinterpret_cast<uint32_t*>(instr.data())), wireData);
                byteEncoder(id, wireData);
                byteEncoder(side, wireData);
                byteEncoder(type, wireData);
                byteEncoder(price, wireData);
                byteEncoder(qty, wireData);
                std::cout << std::endl;
            }
        }
    }

    std::string            _channelFile;
    std::vector<std::byte> _channelContent;
    uint8_t                _number;
}Channel;

int     NicReplay(std::vector<Channel>  &channelFiles, std::vector<MemoryPool> &pool);


#endif