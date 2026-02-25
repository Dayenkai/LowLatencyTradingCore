#include "../../includes/CppStandard.h"
#include "../../includes/HFTMemoryPool.h"

int     Nic(MemoryPool &pool)
{
    std::string      channel_File("");
    std::ifstream    channelsListStream(CHANNELS_LIST_FILE);

    while(std::getline(channelsListStream, channel_File))
    {
        std::ifstream   channelStream(channel_File);
        if (!channelStream.is_open())
        {
            break;
        }
        else
        {
            channelStream.seekg(0, channelStream.end);
            uint64_t    contentLength = channelStream.tellg();
            channelStream.seekg(0, channelStream.beg);

            uint8_t     channelContent[contentLength];
            channelStream.read(reinterpret_cast<char*>(channelContent), contentLength);   
        }

        channelStream.close();
    }
    return 0;
}