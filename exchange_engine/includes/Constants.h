#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#include "CppStandard.h"

namespace ExchangeInetConst
{
    constexpr int TX_RING_SIZE  =       1024;
    constexpr int RX_RING_SIZE  =       1024;
}



using   vector_golden_byte = std::vector<std::byte>;


namespace   exchange_engine
{
    constexpr int  BUFFER_SIZE                = 4096;
    constexpr int  RX_RING_SIZE               = 1024;

    constexpr int  PACKET_ENTRY_SIZE          = 43;
    constexpr int  BYTES_NB_PER_ENTRY         = 23;
    constexpr int  PACKET_NB                  = 4096;

    constexpr int  BAND_SIZE                   = 10960000;       
    constexpr int  BITMAP_SIZE           =      171250;     
    constexpr int  BASE_SELLING_TICK       = 10000;    
    constexpr int  BASE_BUYING_TICK      = 3000000;      
}
//MEMORY LAYOUT


#include <array>

//ORDER_BOOK
enum class Order_Type : uint8_t
{
    ADD = 0, UPDATE = 1, CANCEL = 2
};

enum class Side : bool
{
    Buy = false, Sell = true
};

enum class OrderQtyKind : bool
{
    Limit = false, Market = true
};

enum class OrderKind : uint8_t
{
    GTC = 0, LimitIOC = 1, MarketOrder = 2, LimitFOK = 3, MarketFOK = 4
};

inline constexpr
//MarketDataFormat Columns {Pos,Length}
std::array<std::pair<uint8_t, uint8_t>, 8>    fieldPositions {{{0, 9},//SeqId 
                                               {9, 4},//MarketId 
                                               {13, 9},//ClientId 
                                               {22, 1},//Side 
                                               {23, 1},//Type 
                                               {24, 1},//OrderKind 
                                               {25, 9},//Price 
                                               {34, 9}}};//Quantity
//MarketDataFormat  Columns {Size}
inline constexpr
std::array<size_t, 8>                          dataSize{{{sizeof(uint32_t)},//SeqId 
                                               {sizeof(uint32_t)},//MarketId 
                                               {sizeof(uint32_t)},//ClientId 
                                               {sizeof(uint8_t)},//Side 
                                               {sizeof(uint8_t)},//Type 
                                               {sizeof(uint8_t)},//OrderKind 
                                               {sizeof(uint32_t)},//Price 
                                               {sizeof(uint32_t)}}};//Quantity

//MarketDataFormat  Columns {Offsets}             
inline constexpr
std::array<size_t, 8>                          dataOffsets{{{0},//SeqId 
                                               {dataSize[0]},//MarketId 
                                               {dataSize[1] + dataOffsets[1]},//ClientId 
                                               {dataSize[2] + dataOffsets[2]},//Side
                                               {dataSize[3] + dataOffsets[3]},//Type 
                                               {dataSize[4] + dataOffsets[4]},//OrderKind 
                                               {dataSize[5] + dataOffsets[5]},//Price 
                                               {dataSize[6] + dataOffsets[6]}}};//Quantity




//RESOURCES
#define CHANNELS_LIST_FILE          "../trading_engine/MarketData/channelsListFile.txt"


//CLIMITS
#include                            <climits>

static_assert(CHAR_BIT == 8);

#endif