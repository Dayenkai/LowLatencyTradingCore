#ifndef TRADING_ENGINE_H
#define TRADING_ENGINE_H

#include "FeedHandler.h"
#include "MatchingEngine.h"
#include "OrderBook.h"


int startTradingEngine(int argc, char **argv);

class   TradingEngine
{
    public :

    TradingEngine() = default;

    //USE CASES FUNCTIONS
    virtual int     run() = 0;

    void            SendOrder()
    {

    }

    void            UpdateOrderBook()
    {

    }

    //BUSINESS FUNCTIONS



    protected :
    OrderBook             order_book_;
    FeedHandler           feed_handler_;
    std::atomic<Msg>      msg_buffer_;
    MatchingEngine        matching_engine_;

};

class   TradingEngineReplay : public TradingEngine
{
    public :
    TradingEngineReplay(int &argc, char **argv)
    {
        std::ifstream                                channelList(argv[1]);
        std::string                                  fileName("");

        thread_Idx = 0;
        if (!channelList.is_open())
        {
            std::cout << "Can't open Channel Lists File[" << CHANNELS_LIST_FILE << "]s : Subscriptions not found.";
            return;
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
                    channels.emplace_back(Channel{fileName, content, thread_Idx});

                    thread_Idx+=1;
                }
                channelStream.close();
            }
            channelList.close();
        }
    }


    int     run()
    {
        if (!channels.empty()) [[likely]]
        {

            //std::jthread    order_gateThread(&OrderEntryGateway::Listen, order_entry_gateway.get());
            return 0;
        }


        //THIS PART IS RESERVED FOR SCENARIOS
        //Set up Market Data Pipeline
        /*Set Up Feed Handler*/
        dataProcessorsVector.reserve(thread_Idx);
        std::vector<MemoryPool>   memoryPoolvec(thread_Idx);
        uint8_t                   size = thread_Idx;
        std::vector<uint32_t>     coreIds{1,2,3};
        
        
        for (thread_Idx = 0; thread_Idx < size; thread_Idx++)
        {
            FeedHandler               feedHandler(coreIds[thread_Idx]);    
            dataProcessorsVector.emplace_back(std::jthread(&FeedHandler::run, feedHandler, std::ref(memoryPoolvec[thread_Idx]), std::ref(msg_buffer_)));
        }

        uint32_t        nicCoreId = 0;
        std::jthread    nicReplayThread(NicReplay, std::ref(channels), std::ref(memoryPoolvec), std::ref(nicCoreId));
        nicReplayThread.join();
        for (thread_Idx = 0; thread_Idx < size; thread_Idx++)
        {
            dataProcessorsVector[thread_Idx].join();
        }

        std::jthread    matching_engine_thread(&MatchingEngine::handleOrder, &matching_engine_, std::ref(msg_buffer_));
        matching_engine_thread.join();
    }

    private:
    std::vector<Channel>        channels;
    std::vector<std::jthread>   dataProcessorsVector;
    uint64_t                    thread_Idx;
};

class   TradingEngineLive : public TradingEngine
{
    public :    
    TradingEngineLive() = default;
    
    int     run()
    {
        //TradingEngine::run();
        return 0;
    };
};

#endif