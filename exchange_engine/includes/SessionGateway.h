#ifndef  ORDER_ENTRY_GATEWAY_H
#define  ORDER_ENTRY_GATEWAY_H

#include "MemoryPool.h"

#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <atomic>
#include <thread>
#include <span>
#include <functional>
#include "Constants.h"
#include "Order.h"


namespace   TCPParsing
{
    enum class AddOrder : uint8_t
    {
        ClientOrderId = 0, InstrumentId = 4, Side = 6, Quantity = 7, Price = 11, TIF = 15
    };
    constexpr uint8_t addOrderFieldNb = 6;
    constexpr uint8_t addOrderLength = 16;
    inline std::array<size_t, addOrderFieldNb> addOrderFieldlen = {4,2,1,4,4,1};


    enum class ReplaceOrder : uint8_t
    {
        OriginalClientOrderId = 0, ReplacementClientOrderId = 4, TotalQuantity = 8, NewPrice = 12, TIF = 20
    };
    constexpr uint8_t replaceOrderFieldNb = 5;
    constexpr uint8_t replaceOrderLength = 17;
    inline std::array<size_t, replaceOrderFieldNb> replaceOrderFieldlen = {4,4,4,4,1};


    enum class CancelOrder : uint8_t
    {
        ClientOrderId = 0, IntendedTotalQuantity = 4
    };
    constexpr uint8_t cancelOrderFieldNb = 5;
    constexpr uint8_t cancelOrderLength = 8;
    inline std::array<size_t, cancelOrderFieldNb> CancelOrderFieldlen = {4,4};
}
class   Session
{
    uint64_t    client_id;
    uint64_t    msg_seq;
};

class   SessionGateway
{

    public:
    SessionGateway()
    {
        client_fd_ = -1;//Initializing Sequence Assignment
    }

    void    AssignSequence();
    
    int     ListenOrderEntryTCP(RxRingBuffer<ExchangeInetConst::RX_RING_SIZE, public_OUCH_Messages> &order_entry_command_ring);
    int     SendMarketDataUDP(RxRingBuffer<ExchangeInetConst::TX_RING_SIZE, UDPMulticastEvent> &market_multicast_events_ring);
    int     SendOrderFeedbackTCP(RxRingBuffer<ExchangeInetConst::TX_RING_SIZE, OrderConfirmation> &order_entry_feedback_ring);


    int     SendOrder();
    // bool    Parse(std::array<T, capacity>             data)
    // {
    //     inline void    parse(RxDesc &desc, Msg &msg)
    //     {
    //         fillValueLe<uint32_t>(msg._id, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[0]));// static_cast<uint32_t>(*desc.addr) | static_cast<uint32_t>(*(desc.addr + static_cast<size_t>(dataSize[0]/4))) << CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 2)) << 2 * CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 3)) << 3 * CHAR_BIT;
    //         fillValueLe<uint32_t>(msg._instr, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[1]));// static_cast<uint32_t>(*desc.addr + 4) | static_cast<uint32_t>(*(desc.addr + 5)) <<  CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 6)) << 2 * CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 7)) << 3 * CHAR_BIT;
    //         fillValueLe<uint8_t>(msg._side, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[3])); //static_cast<uint8_t>(*(desc.addr + 12));
    //         fillValueLe<uint8_t>(msg._event_type, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[4]));
    //         fillValueLe<uint8_t>(msg._kind, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[5]));
    //         fillValueLe<uint32_t>(msg.price_, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[6]));
    //         fillValueLe<uint32_t>(msg.qty_, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[7]));
    //     }
    // }

    struct  PacketParser
    {
        struct AddOrderParser
        {
            AddOrderParser()
            {
                newOrder.client_order_id = 0;
                newOrder.instrument_id = 0;
                newOrder.price = 0;
                newOrder.quantity = 0;
                newOrder.side = Side::Buy;
                newOrder.tif = 0;
                newOrder.timestamp = 0;

            }
            void    parseAddOrder(std::byte packet_buffer[26], std::span<std::byte>    &inbound_data, size_t &parsing_cursor, uint8_t &parsing_infos, RxRingBuffer<ExchangeInetConst::RX_RING_SIZE, public_OUCH_Messages> &order_entry_command_ring)//PLace where the filling of packet_buffer stops)
            {
                std::cout << "IN " << __func__ << std::endl;
                bool error;


                while ((parsing_infos & static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress)))
                {
                    switch(parsing_cursor)
                    {
                        case static_cast<uint8_t>(TCPParsing::AddOrder::ClientOrderId):
                        {
                            std::string rejection_reason;
                            if (inbound_data.size() < TCPParsing::addOrderFieldlen[0])
                            {
                                parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress));//Don't ignore next parsing iteration
                                parsing_infos |= (static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent));
                                parsing_cursor = 0;
                                std::cout << "[Error] : Too small of an order Instruction !\n";
                            }
                            else
                            {
                                auto field = inbound_data.subspan(0, TCPParsing::addOrderFieldlen[0]);
                                uint32_t    new_client_order_id = 0;

                                for (size_t  i = 0; i < TCPParsing::addOrderFieldlen[0]; i++)
                                {
                                    newOrder.client_order_id |= static_cast<uint32_t>(static_cast<unsigned char>(field[i])) << i * CHAR_BIT;
                                }
                                // if (std::find_if(field.begin(), field.end(), [](std::byte c){return !std::isdigit(static_cast<char>(c));}) != field.end())
                                // {
                                //     parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress));//Don't ignore next parsing iteration
                                //     parsing_infos |= (static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent));
                                //     parsing_cursor = 0;
                                //     std::cout << "[Error] : AddOrder contains non digit characters\n";
                                // }
                                
                                    std::cout << "ClientOrder_id : [" << newOrder.client_order_id << "]\n";
                                    parsing_cursor+= TCPParsing::addOrderFieldlen[0];
                                    inbound_data = inbound_data.subspan(TCPParsing::addOrderFieldlen[0]);
                                
                            }
                        }
                        break;

                        case static_cast<uint8_t>(TCPParsing::AddOrder::InstrumentId):
                        {
                            std::string rejection_reason;
                            if (inbound_data.size() < TCPParsing::addOrderFieldlen[1])
                            {
                                parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress));//Don't ignore next parsing iteration
                                parsing_infos |= (static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent));
                                parsing_cursor = 0;
                                std::cout << "[Error] : Too small of an InstrumentId !\n";
                            }
                            else
                            {
                                auto field = inbound_data.subspan(0, TCPParsing::addOrderFieldlen[1]);
                                for (size_t  i = 0; i < TCPParsing::addOrderFieldlen[1]; i++)
                                {
                                    newOrder.instrument_id |= (static_cast<uint16_t>(static_cast<unsigned char>(field[i])) << i * CHAR_BIT);
                                }

                                // if (std::find_if(field.begin(), field.end(), [](std::byte c){return !std::isdigit(static_cast<char>(c)) && !std::isalpha(static_cast<char>(c));}) != field.end())
                                // {
                                //     parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress));//Don't ignore next parsing iteration
                                //     parsing_infos |= (static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent));
                                //     parsing_cursor = 0;
                                //     std::cout << "[Error] : Instruemnt Id contains alphanumerical characters !\n";
                                // }
                                // else
                                // {          
                                    std::cout << "Instrument_id : [" << newOrder.instrument_id << "]\n";
                                    parsing_cursor+= TCPParsing::addOrderFieldlen[1];
                                    inbound_data = inbound_data.subspan(TCPParsing::addOrderFieldlen[1]);
                                //}
                            }
                        }
                        break;

                        case static_cast<uint8_t>(TCPParsing::AddOrder::Side):
                        {
                            std::string rejection_reason;
                            if (inbound_data.size() < TCPParsing::addOrderFieldlen[2])
                            {
                                parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress));//Don't ignore next parsing iteration
                                parsing_infos |= (static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent));
                                parsing_cursor = 0;
                                std::cout << "[Error] : Too small of Side instruction !\n";
                            }
                            else
                            {
                                auto field = inbound_data.subspan(0, TCPParsing::addOrderFieldlen[2]);
                                for (size_t  i = 0; i < TCPParsing::addOrderFieldlen[2]; i++)
                                {
                                    std::cout << "Side in the loop is " << static_cast<int>(field[i]) << "\n";
                                    newOrder.side = static_cast<Side>(field[i]) ;
                                }
                                if (newOrder.side != Side::Buy && 
                                    newOrder.side != Side::Sell)
                                //if (std::find_if(field.begin(), field.end(), [](std::byte c){return char(c)-'0'!=0 && char(c)-'0'!=1;}) != field.end())
                                {
                                    parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress));//Don't ignore next parsing iteration
                                    parsing_infos |= (static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent));
                                    parsing_cursor = 0;
                                    std::cout << "[Error] : Side(" << static_cast<int>(newOrder.side) << ") is neither Ask or Bid. No 0 or 1 detected.\n";
                                }
                                else
                                {  
                                    std::cout << "Side : [" << (newOrder.side == Side::Buy ? "Buy]" : "Ask]")  << "\n";
                                    parsing_cursor+= TCPParsing::addOrderFieldlen[2];
                                    inbound_data = inbound_data.subspan(TCPParsing::addOrderFieldlen[2]);
                                }
                            }
                        }
                        break;

                        

                        case static_cast<uint8_t>(TCPParsing::AddOrder::Quantity):
                        {
                            std::string rejection_reason;
                            if (inbound_data.size() < TCPParsing::addOrderFieldlen[3])
                            {
                                parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress));//Don't ignore next parsing iteration
                                parsing_infos |= (static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent));
                                parsing_cursor = 0;
                                std::cout << "[Error] : Too small of an quantity Instruction !\n";
                            }
                            else
                            {
                                auto field = inbound_data.subspan(0, TCPParsing::addOrderFieldlen[3]);
                                for (size_t  i = 0; i < TCPParsing::addOrderFieldlen[3]; i++)
                                {
                                    std::cerr << std::hex << std::to_integer<unsigned>(field[i]) << ' ';
                                    newOrder.quantity |= static_cast<uint32_t>(static_cast<unsigned char>(field[i])) << i * CHAR_BIT;
                                }
                                std::cout << "\n";
                                // if (std::find_if(field.begin(), field.end(), [](std::byte c){return !std::isdigit(static_cast<char>(c));}) != field.end())
                                // {
                                //     parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress));//Don't ignore next parsing iteration
                                //     parsing_infos |= (static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent));
                                //     parsing_cursor = 0;
                                //     std::cout << "[Error] : Quantity contains non digit characters.\n";
                                // }
                                // else
                                // {
                                    std::cout << "Quantity : [" << newOrder.quantity << "]\n";
                                    parsing_cursor+= TCPParsing::addOrderFieldlen[3];
                                    inbound_data = inbound_data.subspan(TCPParsing::addOrderFieldlen[3]);
                                //}
                            }
                        }
                        break;

                        case static_cast<uint8_t>(TCPParsing::AddOrder::Price):
                        {
                            std::string rejection_reason;
                            if (inbound_data.size() < TCPParsing::addOrderFieldlen[4])
                            {
                                parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress));//Don't ignore next parsing iteration
                                parsing_infos |= (static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent));
                                parsing_cursor = 0;
                                std::cout << "[Error] : Too small of a price Instruction.\n";
                            }
                            else
                            {
                                auto field = inbound_data.subspan(0, TCPParsing::addOrderFieldlen[4]);
                                for (size_t  i = 0; i < TCPParsing::addOrderFieldlen[4]; i++)
                                {
                                    std::cerr << std::hex << std::to_integer<unsigned>(field[i]) << ' ';
                                    newOrder.price |= static_cast<uint32_t>(static_cast<unsigned char>(field[i])) << i * CHAR_BIT;
                                }
                                std::cout << "\n";
                                // if (std::find_if(field.begin(), field.end(), [](std::byte c){return !std::isdigit(static_cast<char>(c));}) != field.end())
                                // {
                                //     parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress));//Don't ignore next parsing iteration
                                //     parsing_infos |= (static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent));
                                //     parsing_cursor = 0;
                                //     std::cout << "[Error] : Price contains non digit characters.\n";
                                // }
                                // else
                                // {
                                    std::cout << "Price : [" << newOrder.price << "]\n";
                                    parsing_cursor+= TCPParsing::addOrderFieldlen[4];
                                    inbound_data = inbound_data.subspan(TCPParsing::addOrderFieldlen[4]);
                                //}
                            }
                        }
                        break;

                        case static_cast<uint8_t>(TCPParsing::AddOrder::TIF):
                        {
                            auto i = order_entry_command_ring.head.load(std::memory_order_acquire);
                            bool written = false;
                            while(!written)
                            {
                                if ((((order_entry_command_ring.head + 1) & (exchange_engine::RX_RING_SIZE - 1)) != order_entry_command_ring.tail))
                                {
                                    std::cout << "New Order incoming !" << "\n";
                                    order_entry_command_ring.command_content[order_entry_command_ring.head] = newOrder;
                                    order_entry_command_ring.head.store((order_entry_command_ring.head+1) & (exchange_engine::RX_RING_SIZE - 1), std::memory_order_release);
                                    written = true;
                                }
                            }
                            parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress)) ;//Don't ignore next parsing iteration
                            inbound_data = inbound_data.subspan(static_cast<size_t>(TCPParsing::addOrderFieldlen[5]));
                            parsing_cursor = 0;
                        }

                        break;
                    }
                }
            }
            OUCH::AddOrder    newOrder;
            /* data */
        };
        struct CancelOrderParser
        {
            void    parseCancelOrder(std::byte packet_buffer[26], std::span<std::byte>    &inbound_data, size_t &parsing_cursor, uint8_t &parsing_infos,  RxRingBuffer<ExchangeInetConst::RX_RING_SIZE, public_OUCH_Messages> &order_entry_command_ring)//PLace where the filling of packet_buffer stops
            {

            }
            OUCH::CancelOrder    newOrder;
            /* data */
        };

        struct ReplaceOrderParser
        {
            void    parseReplaceOrder(std::byte packet_buffer[26], std::span<std::byte>    &inbound_data, size_t &parsing_cursor, uint8_t &parsing_infos, RxRingBuffer<ExchangeInetConst::RX_RING_SIZE, public_OUCH_Messages> &order_entry_command_ring)//PLace where the filling of packet_buffer stops)
            {

            }
            OUCH::ReplaceOrder    newOrder;
            //ReplaceOrder    newOrder;
            /* data */
        };

        using   ParserTypePackage = std::variant<AddOrderParser,ReplaceOrderParser,
                                    CancelOrderParser>;

        enum class  ParsingContext : uint8_t
        {
            InProgress = 0, IgnoreContent = 1
        };


        std::byte                                                   parser_buffer[26];
        //std::span<std::byte>                                      inbound_packet;

        size_t                                                      parsing_cursor = 0;//PLace where the filling of packet_buffer stops
        size_t                                                      parsing_bound_len = 0;//According to parsing_type
        uint8_t                                                     parsing_infos = 0;//First bit is for parsing currently processed, second is for ignoring_current_parsing_iteration. May reserve the 6 remaining bits for more information.
        ParserTypePackage                                           order_entry;
        



        //parsing bound is the limit of the max length of the parsing_type
        //the characters that we add is length of buffer & what remains empty(packet_buffer)
        //THe character that we also compare to ignore the invalidd type format is the same formula
        //While advancing through the input buffer... length of the type field of what remains in packet buffer(field according to interval belonging)
        // should be & to input buffer.

        // End of last buffer fill. Empty place or end.
        // LengthOfEmptySlots in packet_buffer = parsing_type_max_length - parsing_cursor.
        // The max fill that packet_buffer can have from input buffer is LengthOfEmptySlots & InputBufferlength.
        // And we advance each field through with parsing_cursor(next_field to be computed based on pos) 
        // = next_field_length & (input_buffer_length - second_cursor_input_buffer_pos);

        //To take then the content of the current input buffer to packet_buffer, we must do LengthOfEmptySlots & InputBufferLength-second_cursor)

        //The outter loop is then as long as there is one available byte to identify the parsing byte, and thatbyte disponiblesomething to fill

        
        void    parse(std::span<std::byte>    inbound_data, RxRingBuffer<ExchangeInetConst::RX_RING_SIZE, public_OUCH_Messages> &order_entry_command_ring)
        {
            std::cout << "In [" << __func__ << "]\n";
            parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent));//Reinitialize Parsing Errors


            while (!inbound_data.empty() && !(parsing_infos & static_cast<uint8_t>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent)))[[likely]] 
            {
                if (!(parsing_infos & (static_cast<uint8_t>(1) << static_cast<uint8_t>(ParsingContext::InProgress))))
                {
                    switch(inbound_data[0])
                    {
                        case static_cast<std::byte>('O'):
                        order_entry = AddOrderParser();
                        break;
                        case static_cast<std::byte>('U'):
                        order_entry = ReplaceOrderParser();
                        break;
                        case static_cast<std::byte>('X'):
                        order_entry = CancelOrderParser();
                        break;
                        default:
                         std::cout << "(" << static_cast<char>(inbound_data[0]) << ")<Error 42 - Unknown Instruction>, size of span is " << inbound_data.size()<<std::endl;
                         return;
                        break;
                    }
                    inbound_data = inbound_data.subspan(1);
                }
                std::visit([&](auto &parser)
                {
                    using   parserType = std::decay_t<decltype(parser)>;

                    if constexpr(std::is_same_v<parserType, AddOrderParser>){
                        parsing_infos |= static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress);//Parsing in progress
                        //parsing_infos |= 0 << static_cast<uint8_t>(ParsingContext::IgnoreContent);//Don't ignore next parsing iteration
                        parser.parseAddOrder(parser_buffer, inbound_data, parsing_cursor, parsing_infos, order_entry_command_ring);}

                    else if constexpr(std::is_same_v<parserType, ReplaceOrderParser>){
                        parser.parseReplaceOrder(parser_buffer, inbound_data, parsing_cursor, parsing_infos, order_entry_command_ring);}

                    else if constexpr(std::is_same_v<parserType, CancelOrderParser>){
                        parser.parseCancelOrder(parser_buffer, inbound_data, parsing_cursor, parsing_infos, order_entry_command_ring);}
                }, order_entry);
            }
        }
    };

    private:
    static  uint64_t                                      seq;
    std::atomic<int>                                      client_fd_;
    std::unordered_map<uint16_t,uint16_t>                 clients_infos;//Client_id<->tcp_socket_fd
    PacketParser                                          parser;
    
    
};

#endif