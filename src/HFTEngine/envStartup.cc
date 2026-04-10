#include "../../includes/CppStandard.h"
#include "../../includes/MemoryPool.h"
#include "../../includes/NicReplay.h"
#include "../../includes/Engine.h"
#include "../../includes/FeedHandler.h"
#include "../../includes/PersonnalLibrary.h"

int     MarketDataEnvSetUp()
{
    std::string                                  channel_File("");
    uint64_t                                     thread_Idx(0);
    std::vector<Channel>                         channels;
    std::ifstream                                channelsListStream(CHANNELS_LIST_FILE);
    std::unique_ptr<std::string>                 ChannelContent;
    std::vector<std::jthread>                    dataProcessorsVector;

    if (!channelsListStream.is_open())
    {
        std::cout << "Can't open Channel Lists File[" << CHANNELS_LIST_FILE << "]s : Subscriptions not found.";
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
                
                char     channelContentBuffer[contentLength + 1];
                channelStream.read(channelContentBuffer, contentLength);
                channelContentBuffer[contentLength] = '\0';
                std::cout << "DISPLAY OF CHANNEL FILE " << channel_File << std::endl << channelContentBuffer << std::endl;
                
                channels.emplace_back(Channel{channel_File, channelContentBuffer, thread_Idx});
                thread_Idx+=1;
                std::cout << "The number of threads is " << thread_Idx << std::endl;
            }
            channelStream.close();
        }
        //channelsListStream.close();
        
        //system("read");
        dataProcessorsVector.reserve(thread_Idx);
        std::vector<MemoryPool>   memoryPoolvec(thread_Idx);
        uint8_t                   size = thread_Idx;
        for (thread_Idx = 0; thread_Idx < size; thread_Idx++)
        {
            dataProcessorsVector.emplace_back(std::jthread(feedHandler, std::ref(memoryPoolvec[thread_Idx])));
        }
        
        std::jthread    nicReplayThread(NicReplay, std::ref(channels), std::ref(memoryPoolvec));
        nicReplayThread.join();
        for (thread_Idx = 0; thread_Idx < size; thread_Idx++)
        {
            dataProcessorsVector[thread_Idx].join();
        }
    }

    return 0;
}