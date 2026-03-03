#include "../../includes/CppStandard.h"
#include <memory>
#include "../../includes/HFTMemoryPool.h"
#include "../../includes/Nic.h"
#include "../../includes/PersonnalLibrary.h"


int     MarketDataProcessor()
{
    std::string                                  channel_File("");
    uint8_t                                      index;
    std::vector<ChannelFile>                     channelFiles;
    std::ifstream                                channelsListStream(CHANNELS_LIST_FILE);
    std::unique_ptr<std::vector<ChannelFile>>    ptr = std::make_unique<std::vector<ChannelFile>>();
    std::vector<std::jthread>                    marketDataThreads;

    if (!channelsListStream.is_open())
    {
        std::cout << "Can't open Channel Lists File[" << CHANNELS_LIST_FILE << "]s subscription not found.";
        return -1;
    }
    else
    {
        while(std::getline(channelsListStream, channel_File))
        {
            std::ifstream   channelStream(channel_File);
            if (!channelStream.is_open())
            {
                std::cout << "No channel detected" << std::endl;
                break;
            }
            else
            {
                channelStream.seekg(0, channelStream.end);
                uint64_t    contentLength = channelStream.tellg();
                channelStream.seekg(0, channelStream.beg);
                
                char     channelContent[contentLength + 1];
                channelStream.read(channelContent, contentLength);
                channelContent[contentLength] = '\0';
                std::cout << "DISPLAY OF CHANNEL FILE " << channel_File << std::endl << channelContent << std::endl;
                
                channelFiles.emplace_back(ChannelFile{channel_File, channelContent, index});
                index++;
            }
            channelStream.close();
        }
        marketDataThreads.reserve(index);
        uint8_t size = index;
        for (index = 0; index < size; index++)
        {
            marketDataThreads.emplace_back(std::jthread(NicReplay, std::ref(channelFiles[index]._channelContent), std::ref(channelFiles[index]._number)));
        }

        for (index = 0; index < size; index++)
        {
            marketDataThreads[index].join();
        }
    }
    
    return 0;
}