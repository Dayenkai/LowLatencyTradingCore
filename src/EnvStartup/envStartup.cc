#include "../../includes/CppStandard.h"
#include "../../includes/MemoryPool.h"
#include "../../includes/NicReplay.h"
#include "../../includes/Engine.h"
#include "../../includes/FeedHandler.h"
#include "../../includes/PersonnalLibrary.h"


// Set up Market Data Trading Core
int     TradingEngineSetUp()
{
    std::ifstream                                channelList(CHANNELS_LIST_FILE);
    std::string                                  fileName("");
    std::vector<Channel>                         channels;
    std::vector<std::jthread>                    dataProcessorsVector;
    uint64_t                                     thread_Idx(0);

    
    if (!channelList.is_open())
    {
        std::cout << "Can't open Channel Lists File[" << CHANNELS_LIST_FILE << "]s : Subscriptions not found.";
        return -1;
    }
    else
    {
        //Set up Market Data
        while(std::getline(channelList, fileName))
        {
            std::ifstream   channelStream(fileName);
            if (!channelStream.is_open())
            {
                std::cout << "No channel detected" << std::endl;
                break;
            }
            else
            {
                channelStream.seekg(0, channelStream.end);
                uint64_t    size = channelStream.tellg();
                channelStream.seekg(0, channelStream.beg);
                
                char     content[size + 1];
                channelStream.read(content, size);
                content[size] = '\0';
                std::cout << "CHANNEL DISPLAY :" << fileName << std::endl << content << std::endl;
                channels.emplace_back(Channel{fileName, content, thread_Idx});

                thread_Idx+=1;
            }
            channelStream.close();
        }
        channelList.close();
        

        //Set up Market Data Pipeline

            /*Set Up Feed Handler*/
        dataProcessorsVector.reserve(thread_Idx);
        std::vector<MemoryPool>   memoryPoolvec(thread_Idx);
        uint8_t                   size = thread_Idx;
        for (thread_Idx = 0; thread_Idx < size; thread_Idx++)
        {
            dataProcessorsVector.emplace_back(std::jthread(feedHandler, std::ref(memoryPoolvec[thread_Idx])));
        }
        
            /*Set up Nic Replay*/
        std::jthread    nicReplayThread(NicReplay, std::ref(channels), std::ref(memoryPoolvec));
        nicReplayThread.join();
        for (thread_Idx = 0; thread_Idx < size; thread_Idx++)
        {
            dataProcessorsVector[thread_Idx].join();
        }
    }
    return 0;
}