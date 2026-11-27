#ifndef ORDER_ENTRY_GATEWAY_
#define ORDER_ENTRY_GATEWAY_

#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <atomic>
#include "CppStandard.h"
#include "Order.h"
#include "MemoryPool.h"


namespace   ITCHParsing
{
    

    enum class AddOrder : uint8_t
    {
        Stock_Locate = 0, Sequence = 4, TimeStamp = 12, ExchangeOrderId = 20, Side = 24, Quantity = 25, Stock_Symbol = 29, Price = 37
    };
    constexpr uint8_t addOrderFieldNb = 8;
    constexpr uint8_t addOrderLength = 37;
    inline std::array<size_t, addOrderFieldNb> addOrderFieldlen = {4,8,8,4,1,4,8,4};


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


    enum class MessageType : char
    {
        AddOrder = 'A', ReplaceOrder = 'U', CancelOrder = 'X', DeleteOrder = 'D', ExecuteOrder = 'E', ExecuteOrderWithPrice = 'C', AddOrderWithMPIDAttribution = 'F'
    };
}
class   Session
{
    uint64_t    client_id;
    uint64_t    msg_seq;
};

class   OrderEntryGateway
{

    public:
    OrderEntryGateway()
    {
        client_fd_ = -1;//Initializing Sequence Assignment
    }
    void    AssignSequence();

    int    ListenMarketDataUDP();
    int    ListenOrderFeedbackTCP();

    int     SendOrderTCP();
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

    template<typename T>
    static void byteEncoder(T& value, std::stringstream   &ss)
    {
        //Tracking Number Must be added in the udp message
        //A Add Order	Order Ref (8), Side (1), Shares (4), Stock Symbol (8), Price (4)

        for (size_t i = 0; i < sizeof(T); i++)
        {
            //ss << (static_cast<std::byte>(value >> i * CHAR_BIT));
        }
    }
    

    struct  PacketParser
    {
        struct AddOrderParser
        {
            AddOrderParser() : newOrder(0,0,std::array<char, trading_engine::STOCK_SYMBOL_LENGTH>{},0,0,0,0,Side::Buy,0)
            {
            }
            void    parseAddOrder(std::span<std::byte>    &inbound_data, size_t &parsing_cursor, uint8_t &parsing_infos, RxRingBuffer<ITCH::MarketEvent, trading_engine::RX_RING_SIZE> &udp_market_data_ring)//PLace where the filling of packet_buffer stops)
            {
                std::cout << "IN " << __func__ << std::endl;

                //ss << "A" << udp_event.getStockLocate() << udp_event.getTimestamp() << udp_event.getExchangeOrderId() << (udp_event.getSide() == Side::Buy ? "0":"1") << udp_event.getQuantity() << udp_event.getPrice() << "\n";
                
                while ((parsing_infos & static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress)))
                {
                    switch(parsing_cursor)
                    {
                        case static_cast<uint8_t>(ITCHParsing::AddOrder::Stock_Locate):
                        {
                            std::string rejection_reason;
                            if (inbound_data.size() < ITCHParsing::addOrderFieldlen[0])
                            {
                                parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress));//Don't ignore next parsing iteration
                                parsing_infos |= (static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent));
                                parsing_cursor = 0;
                                std::cout << "[Error] : Too small of a stock locate !\n";
                            }
                            else
                            {
                                auto field = inbound_data.subspan(0, ITCHParsing::addOrderFieldlen[0]);
                                auto    &stock_locate = newOrder.getStockLocate();

                                for (size_t  i = 0; i < ITCHParsing::addOrderFieldlen[0]; i++)
                                {
                                    stock_locate |= static_cast<uint32_t>(static_cast<unsigned char>(field[i])) << i * CHAR_BIT;
                                }
                                // if (std::find_if(field.begin(), field.end(), [](std::byte c){return !std::isdigit(static_cast<char>(c));}) != field.end())
                                // {
                                //     parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress));//Don't ignore next parsing iteration
                                //     parsing_infos |= (static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent));
                                //     parsing_cursor = 0;
                                //     std::cout << "[Error] : AddOrder contains non digit characters\n";
                                // }
                                
                                    std::cout << "Stock Locate : [" << stock_locate << "]\n";
                                    parsing_cursor+= ITCHParsing::addOrderFieldlen[0];
                                    inbound_data = inbound_data.subspan(ITCHParsing::addOrderFieldlen[0]);
                                
                            }
                        }
                        break;

                        case static_cast<uint8_t>(ITCHParsing::AddOrder::Sequence):
                        {
                            std::string rejection_reason;
                            if (inbound_data.size() < ITCHParsing::addOrderFieldlen[1])
                            {
                                parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress));//Don't ignore next parsing iteration
                                parsing_infos |= (static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent));
                                parsing_cursor = 0;
                                std::cout << "[Error] : Too small of a sequence !\n";
                            }
                            else
                            {
                                auto field = inbound_data.subspan(0, ITCHParsing::addOrderFieldlen[1]);
                                auto    &sequence = newOrder.getSequence();

                                for (size_t  i = 0; i < ITCHParsing::addOrderFieldlen[1]; i++)
                                {
                                    sequence |= static_cast<uint32_t>(static_cast<unsigned char>(field[i])) << i * CHAR_BIT;
                                }
                                // if (std::find_if(field.begin(), field.end(), [](std::byte c){return !std::isdigit(static_cast<char>(c));}) != field.end())
                                // {
                                //     parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress));//Don't ignore next parsing iteration
                                //     parsing_infos |= (static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent));
                                //     parsing_cursor = 0;
                                //     std::cout << "[Error] : AddOrder contains non digit characters\n";
                                // }
                                
                                    std::cout << "Sequence : [" << sequence << "]\n";
                                    parsing_cursor+= ITCHParsing::addOrderFieldlen[1];
                                    inbound_data = inbound_data.subspan(ITCHParsing::addOrderFieldlen[1]);
                                
                            }
                        }
                        break;

                        case static_cast<uint8_t>(ITCHParsing::AddOrder::TimeStamp):
                        {
                            std::string rejection_reason;
                            if (inbound_data.size() < ITCHParsing::addOrderFieldlen[2])
                            {
                                parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress));//Don't ignore next parsing iteration
                                parsing_infos |= (static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent));
                                parsing_cursor = 0;
                                std::cout << "[Error] : Too small of a Timestamp !\n";
                            }
                            else
                            {
                                auto field = inbound_data.subspan(0, ITCHParsing::addOrderFieldlen[2]);
                                auto    &timestamp = newOrder.getTimestamp();

                                for (size_t  i = 0; i < ITCHParsing::addOrderFieldlen[2]; i++)
                                {
                                    timestamp |= static_cast<uint32_t>(static_cast<unsigned char>(field[i])) << i * CHAR_BIT;
                                }

                                // if (std::find_if(field.begin(), field.end(), [](std::byte c){return !std::isdigit(static_cast<char>(c));}) != field.end())
                                // {
                                //     parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress));//Don't ignore next parsing iteration
                                //     parsing_infos |= (static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent));
                                //     parsing_cursor = 0;
                                //     std::cout << "[Error] : AddOrder contains non digit characters\n";
                                // }
                                
                                    std::cout << "Timestamp : [" << timestamp << "]\n";
                                    parsing_cursor+= ITCHParsing::addOrderFieldlen[2];
                                    inbound_data = inbound_data.subspan(ITCHParsing::addOrderFieldlen[2]);
                                
                            }
                        }
                        break;

                        case static_cast<uint8_t>(ITCHParsing::AddOrder::ExchangeOrderId):
                        {
                            std::string rejection_reason;
                            if (inbound_data.size() < ITCHParsing::addOrderFieldlen[3])
                            {
                                parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress));//Don't ignore next parsing iteration
                                parsing_infos |= (static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent));
                                parsing_cursor = 0;
                                std::cout << "[Error] : Too small of an Exchange Order Id !\n";
                            }
                            else
                            {
                                auto field = inbound_data.subspan(0, ITCHParsing::addOrderFieldlen[3]);
                                auto    &exchange_order_id = newOrder.getExchangeOrderId();
                                for (size_t  i = 0; i < ITCHParsing::addOrderFieldlen[3]; i++)
                                {
                                    exchange_order_id |= (static_cast<uint16_t>(static_cast<unsigned char>(field[i])) << i * CHAR_BIT);
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
                                    std::cout << "Exchange Order Id : [" << exchange_order_id << "]\n";
                                    parsing_cursor+= ITCHParsing::addOrderFieldlen[3];
                                    inbound_data = inbound_data.subspan(ITCHParsing::addOrderFieldlen[3]);
                                //}
                            }
                        }
                        break;

                        case static_cast<uint8_t>(ITCHParsing::AddOrder::Side):
                        {
                            std::string rejection_reason;
                            if (inbound_data.size() < ITCHParsing::addOrderFieldlen[4])
                            {
                                parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress));//Don't ignore next parsing iteration
                                parsing_infos |= (static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent));
                                parsing_cursor = 0;
                                std::cout << "[Error] : Too small of Side instruction !\n";
                            }
                            else
                            {
                                auto field = inbound_data.subspan(0, ITCHParsing::addOrderFieldlen[4]);
                                Side &side = newOrder.getSide();
                                for (size_t  i = 0; i < ITCHParsing::addOrderFieldlen[4]; i++)
                                {
                                    std::cout << "Side in the loop is " << static_cast<int>(field[i]) << "\n";
                                    side = static_cast<Side>(field[i]) ;
                                }
                                if (side != Side::Buy && side != Side::Sell)
                                //if (std::find_if(field.begin(), field.end(), [](std::byte c){return char(c)-'0'!=0 && char(c)-'0'!=1;}) != field.end())
                                {
                                    parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress));//Don't ignore next parsing iteration
                                    parsing_infos |= (static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent));
                                    parsing_cursor = 0;
                                    std::cout << "[Error] : Side(" << static_cast<int>(side) << ") is neither Ask or Bid. No 0 or 1 detected.\n";
                                }
                                else
                                {  
                                    std::cout << "Side : [" << (side == Side::Buy ? "Buy]" : "Ask]")  << "\n";
                                    parsing_cursor+= ITCHParsing::addOrderFieldlen[4];
                                    inbound_data = inbound_data.subspan(ITCHParsing::addOrderFieldlen[4]);
                                }
                            }
                        }
                        break;

                        

                        case static_cast<uint8_t>(ITCHParsing::AddOrder::Quantity):
                        {
                            std::string rejection_reason;
                            if (inbound_data.size() < ITCHParsing::addOrderFieldlen[5])
                            {
                                parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress));//Don't ignore next parsing iteration
                                parsing_infos |= (static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent));
                                parsing_cursor = 0;
                                std::cout << "[Error] : Too small of an Quantity Instruction !\n";
                            }
                            else
                            {
                                auto field = inbound_data.subspan(0, ITCHParsing::addOrderFieldlen[5]);
                                auto    &quantity = newOrder.getQuantity();
                                for (size_t  i = 0; i < ITCHParsing::addOrderFieldlen[5]; i++)
                                {
                                    std::cerr << std::hex << std::to_integer<unsigned>(field[i]) << ' ';
                                    quantity |= static_cast<uint32_t>(static_cast<unsigned char>(field[i])) << i * CHAR_BIT;
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
                                    std::cout << "Quantity : [" << quantity << "]\n";
                                    parsing_cursor+= ITCHParsing::addOrderFieldlen[5];
                                    inbound_data = inbound_data.subspan(ITCHParsing::addOrderFieldlen[5]);
                                //}
                            }
                        }
                        break;

                        case static_cast<uint8_t>(ITCHParsing::AddOrder::Stock_Symbol):
                        {
                            std::string rejection_reason;
                            if (inbound_data.size() < ITCHParsing::addOrderFieldlen[6])
                            {
                                parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress));//Don't ignore next parsing iteration
                                parsing_infos |= (static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent));
                                parsing_cursor = 0;
                                std::cout << "[Error] : Too small of a Stock Symbol !\n";
                            }
                            else
                            {
                                auto field = inbound_data.subspan(0, ITCHParsing::addOrderFieldlen[6]);
                                auto    &stock_symbol = newOrder.getStockSymbol();
                                for (size_t  i = 0; i < ITCHParsing::addOrderFieldlen[6]; i++)
                                {
                                    std::cerr << std::hex << std::to_integer<unsigned>(field[i]) << ' ';
                                    stock_symbol[i] = static_cast<char>(field[i]);
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
                                std::cout << "Stock_Symbol : [";
                                    for (size_t i = 0; i < stock_symbol.size(); i++)
                                    {
                                        std::cout << stock_symbol[i];
                                    }
                                    std::cout << "]\n";
                                    parsing_cursor+= ITCHParsing::addOrderFieldlen[6];
                                    inbound_data = inbound_data.subspan(ITCHParsing::addOrderFieldlen[6]);
                                //}
                            }
                        }
                        break;

                        case static_cast<uint8_t>(ITCHParsing::AddOrder::Price):
                        {
                            std::cout << "In Price !" << std::endl;
                            std::string rejection_reason;
                            if (inbound_data.size() < ITCHParsing::addOrderFieldlen[7])
                            {
                                parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::InProgress));//Don't ignore next parsing iteration
                                parsing_infos |= (static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent));
                                parsing_cursor = 0;
                                std::cout << "[Error] : Too small of a price Instruction.\n";
                            }
                            else
                            {
                                auto field = inbound_data.subspan(0, ITCHParsing::addOrderFieldlen[7]);
                                uint32_t    &price = newOrder.getPrice();
                                for (size_t  i = 0; i < ITCHParsing::addOrderFieldlen[7]; i++)
                                {
                                    std::cerr << std::hex << std::to_integer<unsigned>(field[i]) << ' ';
                                    price |= static_cast<uint32_t>(static_cast<unsigned char>(field[i])) << i * CHAR_BIT;
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
                                    std::cout << "Price : [" << price << "]\n";
                                    parsing_cursor+= ITCHParsing::addOrderFieldlen[7];
                                    inbound_data = inbound_data.subspan(ITCHParsing::addOrderFieldlen[7]);
                                //}
                            }
                        }
                        break;
                    }
                }
            }
            ITCH::AddOrder    newOrder;
            /* data */
        };
        struct CancelOrderParser
        {
            CancelOrderParser() : newOrder(0,0,std::array<char, trading_engine::STOCK_SYMBOL_LENGTH>{},0,0)
            {

            }
            // void    parseCancelOrder(std::byte packet_buffer[26], std::span<std::byte>    &inbound_data, size_t &parsing_cursor, uint8_t &parsing_infos,  RxRingBuffer<ITCH::MarketEvent, trading_engine::RX_RING_SIZE> &udp_market_data_ring)//PLace where the filling of packet_buffer stops
            // {

            // }
            ITCH::CancelOrder    newOrder;
            /* data */
        };

        struct ReplaceOrderParser
        {
            ReplaceOrderParser() : newOrder(0,0,std::array<char, trading_engine::STOCK_SYMBOL_LENGTH>{},0,0,0,0,0) 
            {

            }
            // void    parseReplaceOrder(std::byte packet_buffer[26], std::span<std::byte>    &inbound_data, size_t &parsing_cursor, uint8_t &parsing_infos, RxRingBuffer<ITCH::MarketEvent, trading_engine::RX_RING_SIZE> &udp_market_data_ring)//PLace where the filling of packet_buffer stops)
            // {

            // }
            ITCH::ReplaceOrder    newOrder;
            //ReplaceOrder    newOrder;
            /* data */
        };

        using   ParserTypePackage = std::variant<AddOrderParser,ReplaceOrderParser,
                                    CancelOrderParser>;

        enum class  ParsingContext : uint8_t
        {
            InProgress = 0, IgnoreContent = 1
        };

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

        
        void    parse(std::span<std::byte>    inbound_data, RxRingBuffer<ITCH::MarketEvent, trading_engine::RX_RING_SIZE> &udp_market_data_ring)
        {
            std::cout << "In [" << __func__ << "]\n";
            parsing_infos &= ~(static_cast<int>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent));//Reinitialize Parsing Errors


            while (!inbound_data.empty() && !(parsing_infos & static_cast<uint8_t>(1) << static_cast<uint8_t>(ParsingContext::IgnoreContent)))[[likely]] 
            {
                if (!(parsing_infos & (static_cast<uint8_t>(1) << static_cast<uint8_t>(ParsingContext::InProgress))))
                {
                    switch(inbound_data[0])
                    {
                        case static_cast<std::byte>(ITCHParsing::MessageType::AddOrder):
                        order_entry = AddOrderParser();
                        break;
                        case static_cast<std::byte>(ITCHParsing::MessageType::ReplaceOrder):
                        order_entry = ReplaceOrderParser();
                        break;
                        case static_cast<std::byte>(ITCHParsing::MessageType::CancelOrder):
                        order_entry = CancelOrderParser();
                        break;
                        case static_cast<std::byte>(ITCHParsing::MessageType::DeleteOrder):
                        order_entry = CancelOrderParser();
                        break;
                        case static_cast<std::byte>(ITCHParsing::MessageType::ExecuteOrder):
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
                        parser.parseAddOrder(inbound_data, parsing_cursor, parsing_infos, udp_market_data_ring);}

                    // else if constexpr(std::is_same_v<parserType, ReplaceOrderParser>){
                    //     parser.parseReplaceOrder(inbound_data, parsing_cursor, parsing_infos, udp_market_data_ring)}

                    // else if constexpr(std::is_same_v<parserType, CancelOrderParser>){
                    //     parser.parseCancelOrder(inbound_data, parsing_cursor, parsing_infos, udp_market_data_ring)}
                }, order_entry);
            }
        }
    };

    inline RxRingBuffer<OrderConfirmation, trading_engine::RX_RING_SIZE>   &getOrderFeedbackRing()
    {
        return tcp_order_entry_feedback_ring;
    }

    inline RxRingBuffer<ITCH::MarketEvent, trading_engine::RX_RING_SIZE>   &getUDPMktDataRing()
    {
        return udp_market_data_ring;
    }

    private:
    std::unordered_map<uint16_t,uint16_t>                 clients_infos;//Client_id<->tcp_socket_fd
    PacketParser                                          parser;





    private:
    alignas(64) RxRingBuffer<OrderConfirmation,      trading_engine::RX_RING_SIZE>   tcp_order_entry_feedback_ring;
    alignas(64) RxRingBuffer<ITCH::MarketEvent,      trading_engine::RX_RING_SIZE>   udp_market_data_ring;
    static  uint64_t              seq;
    std::atomic<int>              client_fd_;
};

#endif