#ifndef _MATCHING_ENGINE
#define _MATCHING_ENGINE

#include "Constants.h"
#include "OrderBook.h"
#include "Utils.h"

class   MatchingEngine
{

    public:
    MatchingEngine() = default;

    std::optional<Order>    AmendOrderBook(Msg &order)
    {
        std::optional<Order>    res;

        switch(static_cast<uint32_t>(order.event_type_))
        {
            case static_cast<uint32_t>(Order_Type::ADD) :
            res = book.addOrder(order);
            return res;

            case static_cast<uint32_t>(Order_Type::UPDATE) :
            break;
            
            case static_cast<uint32_t>(Order_Type::CANCEL) :
            res = book.cancelOrder(order);
            return res;
        }
        return std::nullopt;
    }

    OrderBook::EventsRecord    &matchingUpdate(RxRingBuffer<ExchangeInetConst::TX_RING_SIZE, UDPMulticastEvent> &udp_gateway_events_ring)
    {
        //Check Matching
        OrderBook::TopOfTheBook   &top_of_the_book = book.topOfTheBook();
        std::cout << "Before the while loop" << std::endl;
        //std::cout << "Before matching condition : " << "Best Ask [" << top_of_the_book.best_ask << ", " << top_of_the_book.best_ask_qty << "]" << std::endl;
        //std::cout << "Best Bid [" << top_of_the_book.best_bid << ", " << top_of_the_book.best_bid_qty << "]" << std::endl;  

        uint8_t &event_head = book.getEventsRecord().event_head;
        uint8_t &event_tail = book.getEventsRecord().event_tail;
        while (top_of_the_book.best_bid >= top_of_the_book.best_ask && top_of_the_book.best_ask_qty > 0 && top_of_the_book.best_bid_qty > 0)
        {
            std::cout << "MATCHING between ask[" << top_of_the_book.best_ask << "," << top_of_the_book.best_ask_qty << "] and bid [" << top_of_the_book.best_bid << "," << top_of_the_book.best_bid_qty << "]" << std::endl;
            book.matchingUpdate();
            if ((event_head < event_tail && (exchange_engine::MAX_EVENT_PER_TRANSM - event_tail) + event_head == exchange_engine::MAX_EVENT_PER_TRANSM - 1)
                || event_head - event_tail == exchange_engine::MAX_EVENT_PER_TRANSM - 1)//The event ring is full, we must send an immediate UDP multicast packets to all subscribed clients
            {
                for (event_tail; event_tail != event_head; event_tail = (event_tail+1)&(exchange_engine::MAX_EVENT_PER_TRANSM-1))
                {
                    std::cout << "WritingToExternalRing during Matchnig...\n";
                    writeToExternalRing(udp_gateway_events_ring, book.getEventsRecord().events_ring[event_tail]);
                }
            }
            //if array containing events exceeds max size, send events to ring buffer and return nullopt;
            //OrderBook contain an array of events to be sent back to Exchange to be sent directly to SessionGateWay UDP Multicast
        }
        return book.getEventsRecord();
        //return array of events to ring buffer.
    }

    OrderBook   &getOrderBook()
    {
        return book;
    }

    private:
    OrderBook   book;
    //Can includes Parameters for the Matchnig Engine
};

#endif