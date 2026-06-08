#ifndef CONSTANTS_H
#define CONSTANTS_H

//MEMORY LAYOUT
#define BUFFER_SIZE                 4096
#define RX_RING_SIZE                1024

#define PACKET_ENTRY_SIZE           43
#define BYTES_NB_PER_ENTRY          23
#define PACKET_NB                   4096
#define PACKET_COLUMNS_NB           5

#include <array>

//ORDER_BOOK
enum class Order_Type : uint8_t
{
    ADD = 0, TRADE = 1, CANCEL = 2
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
                                               {sizeof(uint32_t)},//MarketId 
                                               {sizeof(uint32_t) + dataOffsets[1]},//ClientId 
                                               {sizeof(uint8_t) + dataOffsets[2]},//Side 
                                               {sizeof(uint8_t) + dataOffsets[3]},//Type 
                                               {sizeof(uint8_t) + dataOffsets[4]},//OrderKind 
                                               {sizeof(uint32_t) + dataOffsets[5]},//Price 
                                               {sizeof(uint32_t) + dataOffsets[6]}}};//Quantity

#define BAND_SIZE                   10960000
#define BITMAP_SIZE                 171250
#define BASE_SELLING_TICK           10000
#define BASE_BUYING_TICK            3000000


//RESOURCES
#define CHANNELS_LIST_FILE          "MarketData/channelsListFile.txt"


//CLIMITS
#include                            <climits>

static_assert(CHAR_BIT == 8);

#endif 