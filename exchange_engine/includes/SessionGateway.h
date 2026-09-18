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
#include "Constants.h"
#include "Order.h"
#include <span>

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
    
    int     ListenOrderEntryTCP(RxRingBuffer<ExchangeInetConst::RX_RING_SIZE, AddOrder> &order_entry_command_ring);
    int     SendOrderEntryFeedbackTCP();
    int     SendMarketDataUDP();
    int     SendOrderFeedbackTCP();


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
        std::byte                                             packet_buffer[26];
        char                                                  parsing_type = 'E';//THe current parsing type
        size_t                                                parsing_cursor = 0;//PLace where the filling of packet_buffer stops
        size_t                                                parsing_bound_len = 0;//According to parsing_type
        bool                                                  ignore_content = false;
        
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
        void    parsing(const std::span<std::byte>    &inbound_data)
        {
            switch (parsing_type)
            {
                case 'E':
                
                char order_type = static_cast<char>(packet_buffer[0]);
                if (order_type == 'A')
                {
                    parsing_type = 'A';
                    // if (parsing_pos >= 26)
                    // {
                        
                    // }
                    //If parsing doesn't match the format, ignore = true;
                }
                else
                {
                    ignore_content = true;
                }
                break;
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