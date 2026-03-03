#ifndef     PERSONNAL_LIBRARY
#define     PERSONNAL_LIBRARY

#include    "CppStandard.h"    

void        log(const std::string &info);

typedef struct ChannelFile
{
    ChannelFile(std::string channelFile, char* channelContent, uint8_t number) : _channelFile(channelFile), _number(number), _channelContent(channelContent){}
    std::string _channelFile;
    std::string _channelContent;
    uint8_t     _number;
}ChannelFile;




#endif