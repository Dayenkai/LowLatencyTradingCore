#ifndef CONSTANTS_H
#define CONSTANTS_H

//MEMORY LAYOUT
#define BUFFER_SIZE                 4096
#define RX_RING_SIZE                1024

#define PACKET_ENTRY_SIZE           42
#define BYTES_NB_PER_ENTRY          22
#define PACKET_NB                   4096
#define PACKET_COLUMNS_NB           5

//ORDER_BOOK
enum class Order_Type : uint8_t
{
    ADD_ORDER = 0, TRADE = 1, CANCEL = 2
};

enum class Side : bool
{
    Buy = true, Sell = false
};

#define BAND_SIZE                   4096
#define BASE_SELLING_TICK           10000
#define BASE_BUYING_TICK            9000


//RESOURCES                
#define CHANNELS_LIST_FILE          "resources/channelsListFile.txt"


//CLIMITS
#include                            <climits>

static_assert(CHAR_BIT == 8);

#endif 