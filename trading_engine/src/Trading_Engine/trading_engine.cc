// #ifndef TRADING_ENGINE_H
// #define TRADING_ENGINE_H

// #include "TradingEngine.h"

// int TradingEngine::run()
// {

// }

// class   TradingEngine
// {
//     public :

//     TradingEngine() = default;

//     //USE CASES FUNCTIONS
//     virtual int     run() = 0;

//     void            EmitOrder()
//     {

//     }

//     void            UpdateOrderBook()
//     {

//     }

//     //BUSINESS FUNCTIONS



//     protected :
//     OrderBook             order_book_;
//     FeedHandler           feed_handler_;

// };

// class   TradingEngineScenario : public TradingEngine
// {
//     public :
//     TradingEngine() = delete;
//     TradingEngine(int argc, char **argv)
//     {
//         std::ifstream                                channelList(argv[1]);
//         std::string                                  fileName("");
//         std::vector<Channel>                         channels;
//         std::vector<std::jthread>                    dataProcessorsVector;
//         uint64_t                                     thread_Idx(0);

        
//         if (!channelList.is_open())
//         {
//             std::cout << "Can't open Channel Lists File[" << CHANNELS_LIST_FILE << "]s : Subscriptions not found.";
//             return -1;
//         }
//         else
//         {
//             //Set up Market Data
//             while(std::getline(channelList, fileName))
//             {
//                 std::ifstream   channelStream(fileName);
//                 if (!channelStream.is_open())
//                 {
//                     std::cout << "No channel detected" << std::endl;
//                     break;
//                 }
//                 else
//                 {
//                     channelStream.seekg(0, channelStream.end);
//                     uint64_t    size = channelStream.tellg();
//                     channelStream.seekg(0, channelStream.beg);
                    
//                     char     content[size + 1];
//                     channelStream.read(content, size);
//                     content[size] = '\0';
//                     channels.emplace_back(Channel{fileName, content, thread_Idx});

//                     thread_Idx+=1;
//                 }
//                 channelStream.close();
//             }
//             channelList.close();
            

//             //Set up Market Data Pipeline

//                 /*Set Up Feed Handler*/
//             dataProcessorsVector.reserve(thread_Idx);
//             std::vector<MemoryPool>   memoryPoolvec(thread_Idx);
//             uint8_t                   size = thread_Idx;
//             std::vector<uint32_t>     coreIds{1,2,3};
//             for (thread_Idx = 0; thread_Idx < size; thread_Idx++)
//             {
//                 dataProcessorsVector.emplace_back(std::jthread(feedHandler, std::ref(memoryPoolvec[thread_Idx]), std::ref(coreIds[thread_Idx])));
//             }
//         }
//     }


//     int     run()
//     {
//         std::ifstream exchangeDataStream(exchangeDataFile_);
//         if (!exchangeDataStream.is_open())
//         {
//             std::cerr << "Could not open Scenario File." << std::endl;
//             return -1;
//         }
        
//         nlohmann::json parsed_json = nlohmann::json::parse(exchangeDataStream);

//         const auto &orders = parsed_json["orders"];
//         for (size_t i = 0; i < orders.size(); i++)
//         {
//             if (orders[i]["type"] == "NEW")
//             {
//                 AddOrder order = orders[i].get<AddOrder>();
//                 orderBook_.addOrder(Msg());
//                 typedef struct alignas(64) Msg
// {
//     uint32_t                 _id;
//     uint32_t                 _instr;
//     uint32_t                 price_;
//     uint32_t                 qty_;
//     uint8_t                  _kind;
//     uint8_t                  _event_type;
//     uint8_t                  _side;
// }Msg;
//             }
//             if (orders[i]["type"] == "DELETE")
//             {
//                 DeleteOrder order = orders[i].get<DeleteOrder>();
//             }
//             if (orders[i]["type"] == "CANCEL")
//             {
//                 CancelOrder order = orders[i].get<CancelOrder>();
//             }
//         }
//         //printOrder(order);
//         exchangeDataStream.close();
//         //Filling OrderBook with Json Scenario File
//         //for (auto &)
//         Exchange::run();
//         return 0;
//     };

//     private:
//     std::string exchangeDataFile_;
// };

// class   ExchangeRandom : public Exchange
// {
//     public :    
//     ExchangeRandom() = default;
//     ExchangeRandom(RxRingBuffer<RxDesc, ExchangeInetConst::TX_RING_SIZE> &tx_ring_buffer,
//                     RxRingBuffer<RxDesc, ExchangeInetConst::RX_RING_SIZE> &rx_ring_buffer) 
//                     : Exchange(tx_ring_buffer, rx_ring_buffer)
//     {
//     }
//     int     run()
//     {
//         Exchange::run();
//         return 0;
//     };
// };

// #endif