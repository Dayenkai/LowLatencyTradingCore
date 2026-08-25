#ifndef NIC_REPLAY
#define NIC_REPLAY

#include "CppStandard.h"
#include "MemoryPool.h"
#include "PersonnalLibrary.h"
#include "NicReplay.h"
#include "Constants.h"

typedef struct Channel
{
    public:
    Channel(std::string channelFile, char* channelContent, uint64_t number) : _channelFile(channelFile), _number(number)
    {
        charToBytes(channelContent, _channelContent);
    }

    template<typename T>
    void byteEncoder(T& value, std::vector<std::byte>& dest)
    {
        for (size_t i = 0; i < sizeof(T); i++)
        {
            dest.emplace_back(static_cast<std::byte>(value >> i * CHAR_BIT));
            //std::cout << (uint64_t)_channelContent.back() << ";";
        }
    }

    //Packet Fixed Format
    template<typename T>
    void    charToBytes(char *content, std::vector<T>& wireData)
    {
        std::string         buffer(content);
        std::stringstream   bufferStream(buffer);
        std::string         line("");

        while (std::getline(bufferStream, line))
        {
            if (line.length() >= PACKET_ENTRY_SIZE)
            {
                uint32_t       seq   = std::stoi(line.substr(fieldPositions[0].first, fieldPositions[0].second));
                std::string    instr = line.substr(fieldPositions[1].first, fieldPositions[1].second);
                uint32_t       id    = std::stoi(line.substr(fieldPositions[2].first, fieldPositions[2].second));
                uint8_t        side  = std::stoi(line.substr(fieldPositions[3].first, fieldPositions[3].second));
                uint8_t        type  = std::stoi(line.substr(fieldPositions[4].first, fieldPositions[4].second));
                uint8_t        kind  = std::stoi(line.substr(fieldPositions[5].first, fieldPositions[5].second));
                uint32_t       price = std::stoi(line.substr(fieldPositions[6].first, fieldPositions[6].second));
                uint32_t       qty   = std::stoi(line.substr(fieldPositions[7].first, fieldPositions[7].second));

                //std::cout << "Seq = " << seq << ", instr = " << instr << ", id = " << id << ", side = " << (uint64_t)side << ", type = " << (uint64_t)type << ", kind = " << (uint64_t)kind << ", price = " << price << ", qty = " << qty << std::endl;
                byteEncoder(seq, wireData);
                byteEncoder(*(reinterpret_cast<uint32_t*>(instr.data())), wireData);
                byteEncoder(id, wireData);
                byteEncoder(side, wireData);
                byteEncoder(type, wireData);
                byteEncoder(kind, wireData);
                byteEncoder(price, wireData);
                byteEncoder(qty, wireData);
            }
        }
    }

    std::string            _channelFile;
    std::vector<std::byte> _channelContent;
    uint8_t                _number;
}Channel;

int     NicReplay(std::vector<Channel>  &channelFiles, std::vector<MemoryPool> &pool, uint32_t &coreId);


#endif