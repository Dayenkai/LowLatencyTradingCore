#ifndef MEMORY_POOL_HEADER
#define MEMORY_POOL_HEADER

#include "CppStandard.h"
#include "Constants.h"
#include "Order.h"
#include <cstdint>
#include <cstddef>
#include <atomic>
#include <variant>

template<typename T>
void byteEncoder(uint64_t value, std::vector<T>& dest);

void    charToBytes(char *content, std::vector<uint8_t> wireData);

typedef struct alignas(64) RxDesc
{
    uint32_t                len;
    uint16_t                flags;
    uint8_t                 padding;
    uint8_t                 addr;
}RxDesc;

enum class  OrderType : uint8_t
{
    AddOrder = 1
};

template <size_t capacity, typename... Types>
struct alignas(64) RxRingBuffer
{
    std::atomic<uint64_t>                           head = 0;
    std::atomic<uint64_t>                           tail = 0;
    std::array<std::variant<Types...>, capacity>    command_content;
};

typedef struct alignas(64) FeedPacketHeader
{
    uint32_t    seq = 0;
    uint32_t    msg_cnt;
    uint8_t     flags;
}FeedPacketHeader;

typedef struct alignas(64) Msg
{
    uint32_t                 id_;
    uint32_t                 instr_;
    uint32_t                 price_;
    uint32_t                 qty_;
    uint8_t                  kind_;
    uint8_t                  event_type_;
    Side                     side_;
    OrderStatus              status_;
    

    Msg        &setId(uint32_t id)
    {
        id_ = id;
        return *this;
    }

    Msg        &setInstr(uint32_t instr)
    {
        instr_ = instr;
        return *this;
    }

    Msg        &setPrice(uint32_t price)
    {
        price_ = price;
        return *this;
    }

    Msg        &setQty(uint32_t qty)
    {
        qty_ = qty;
        return *this;
    }

    Msg        &setKind(uint8_t kind)
    {
        kind_ = kind;
        return *this;
    }

    Msg        &setEventType(uint8_t event_type)
    {
        event_type_ = event_type;
        return *this;
    }

    Msg        &setSide(Side side)
    {
        side_ = side;
        return *this;
    }

    Msg        &setStatus(OrderStatus status)
    {
        status_ = status;
        return *this;
    }

}Msg;

typedef struct alignas(64) Order
{
    Order()
    {
        
    }
    Order(uint32_t qty, uint32_t price, Side side, uint16_t id, Order_Type type) : qty_(qty), price_(price), side_(side), id_(id), event_type_(type)
    {
    }

    Order&  setId(const uint16_t &id)
    {
        id_ = id;
        return *this;
    }

    Order&  setPrice(const uint32_t &price)
    {
        price_ = price;
        return *this;
    }

    Order&  setQty(const uint32_t &qty)
    {
        qty_ = qty;
        return *this;
    }

    Order&  setSide(const Side &side)
    {
        side_ = side;
        return *this;
    }

    Order&  setType(const Order_Type &event_type)
    {
        event_type_ = event_type;
        return *this;
    }

    Order&  setStatus(const OrderStatus &status)
    {
        status_ = status;
        return *this;
    }

    uint32_t        qty_;
    uint32_t        price_;
    uint16_t        id_;
    Side            side_;
    Order_Type      event_type_;
    OrderQtyKind    kind_;
    OrderStatus     status_;
}Order;


typedef struct alignas(64) MemoryPool
{
    public:
    FeedPacketHeader                                                       packetHeader;
    Msg                                                                    msg;
    uint8_t                                                                arena[exchange_engine::PACKET_NB][exchange_engine::BUFFER_SIZE];
}MemoryPool;

//USE A STD::MAP FOR THE MOMENT, AND THEN MEMORYPOOL ORDER IDS FOR MORE COMPLEX MEMORY MANAGMENT

#endif