#include "FeedHandler.h"
#include "Utils.h"

template<typename T>
inline void    FeedHandler::fillValueLe(T &dest, std::byte* val)
{
    memcpy(&dest, val, sizeof(dest));

    if (std::endian::native == std::endian::big)
    {
        dest = std::byteswap(dest);
    }
}

void    FeedHandler::buildOrder(Msg &msg, Order &order)
{
    order.qty_ = msg.qty_;
    order.price_ = msg.price_;
    order._id = msg._id;
    order._side = static_cast<Side>(msg._side);
    order._event_type = static_cast<Order_Type>(msg._event_type);
}

inline void    FeedHandler::parse(RxDesc &desc, Msg &msg)
{
    fillValueLe<uint32_t>(msg._id, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[0]));// static_cast<uint32_t>(*desc.addr) | static_cast<uint32_t>(*(desc.addr + static_cast<size_t>(dataSize[0]/4))) << CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 2)) << 2 * CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 3)) << 3 * CHAR_BIT;
    fillValueLe<uint32_t>(msg._instr, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[1]));// static_cast<uint32_t>(*desc.addr + 4) | static_cast<uint32_t>(*(desc.addr + 5)) <<  CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 6)) << 2 * CHAR_BIT | static_cast<uint32_t>(*(desc.addr + 7)) << 3 * CHAR_BIT;
    fillValueLe<uint8_t>(msg._side, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[3])); //static_cast<uint8_t>(*(desc.addr + 12));
    fillValueLe<uint8_t>(msg._event_type, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[4]));
    fillValueLe<uint8_t>(msg._kind, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[5]));
    fillValueLe<uint32_t>(msg.price_, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[6]));
    fillValueLe<uint32_t>(msg.qty_, reinterpret_cast<std::byte*>(desc.addr + dataOffsets[7]));
}

int  FeedHandler::run(MemoryPool &pool, std::atomic<Msg> &parsed_msg)
{
    pthread_t   current_thread = pthread_self();
    pinThreadToCore(current_thread, coreId_);

    TimePoint        start_time;
    MatchingEngine   matchingEngine;
    int              orderNb(0);
    bool             started(false);


    
    if (pool.rxRingDesc.tail != pool.rxRingDesc.head)
    {
        if (!started)
        {
            start_time = Clock::now();
            started = true;
        }
        pool.rxRingDesc.tail.load(std::memory_order_acquire);
        if (pool.rxRingDesc.data[pool.rxRingDesc.tail].len != 0)
        {
            Msg new_msg = parsed_msg.load();
            auto start = Clock::now();
            parse(pool.rxRingDesc.data[pool.rxRingDesc.tail], new_msg);
            parsed_msg.store(new_msg);
            auto end    = Clock::now();
            //std::cout << "Parsing takes : " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << std::endl;
            //buildOrder(msg, marketOrder);
            //auto    start = Clock::now();
            //std::cout << "YEaah" << std::endl;
            //std::cout << "Order " << (static_cast<Side>(msg._side) == Side::Buy ? "Buy" : "Sell") << " price is : " << msg.price_ << " and qty is " << msg.qty_ << std::endl;
            
            //matchingEngine.handleOrder(parsed_msg);
            
            //auto    end =   Clock::now();
            //std::cout << "Book Update : " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << " ns." << std::endl;
            ++orderNb;
            //std::cout << "ORDER NB" << orderNb << std::endl;
        }
        pool.rxRingDesc.tail.store((pool.rxRingDesc.tail + 1) & (trading_engine::RX_RING_SIZE - 1), std::memory_order_release); 
        if (orderNb == 13)
        {
            matchingEngine.getOrderBook().ListOrder(Side::Buy);
            matchingEngine.getOrderBook().ListOrder(Side::Sell);
            OrderBook::TopOfTheBook &top_of_the_book = matchingEngine.getOrderBook().topOfTheBook();
            std::cout << "The top of the book is : " << std::endl << "Ask : [" << top_of_the_book.best_ask << ", " << top_of_the_book.best_ask_qty << "]" << std::endl;
            std::cout << "Bid : [" << top_of_the_book.best_bid << ", " << top_of_the_book.best_bid_qty << "]" << std::endl;
        }
    }
    return 0;
}