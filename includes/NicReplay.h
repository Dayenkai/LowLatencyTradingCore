#ifndef NIC_REPLAY
#define NIC_REPLAY

#include "CppStandard.h"
#include "MemoryPool.h"
#include "PersonnalLibrary.h"
#include "NicReplay.h"

typedef struct ChannelFile
{
    public:
    ChannelFile(std::string channelFile, char* channelContent, uint8_t number) : _channelFile(channelFile), _number(number), _channelContent(channelContent)
    {

    }
    std::string _channelFile;
    std::string _channelContent;
    uint8_t     _number;
}ChannelFile;

int     NicReplay(std::vector<ChannelFile>  &channelFiles, std::vector<MemoryPool> &pool);


#endif