#ifndef _EVENT_
#define _EVENT_

#include "Constants.h"

class    Event
{

    public:
    MatchingEngine() = default;

    void    Event(std::variant<SystemEvent, OrderAddition, OrderExecution, OrderCancellation,
                     OrderDeletion, OrderReplacement> &event)
    {

        timestamp_creation_ = std::chrono::system_clock::now();
    }


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

    void    matchingUpdate()
    {
        //Check Matching
        OrderBook::TopOfTheBook   &top_of_the_book = book.topOfTheBook();
        std::cout << "Before matching condition : " << "Best Ask [" << top_of_the_book.best_ask << ", " << top_of_the_book.best_ask_qty << "]" << std::endl;
        std::cout << "Best Bid [" << top_of_the_book.best_bid << ", " << top_of_the_book.best_bid_qty << "]" << std::endl;
        while (top_of_the_book.best_bid >= top_of_the_book.best_ask && top_of_the_book.best_ask_qty > 0 && top_of_the_book.best_bid_qty > 0)
        {
            std::cout << "MATCHING between ask[" << top_of_the_book.best_ask << "," << top_of_the_book.best_ask_qty << "] and bid [" << top_of_the_book.best_bid << "," << top_of_the_book.best_bid_qty << "]" << std::endl;
            book.matchingUpdate();
        }
    }

    OrderBook   &getOrderBook()
    {
        return book;
    }

    private:
    std::chrono::time_point<std::chrono::system_clock>  timestamp_creation_;
    Status      status_;
    std::string reason_info_;

    auto    end;    = Clock::now();
    // Get the current time point from the system clock
    auto now = ;

    // Convert to time_t for human-readable format
    //std::time_t now_time = std::chrono::system_clock::to_time_t(now);
            //std::cout << "Parsing takes : " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << std::endl;

    using   EventType = std::variant<SystemEvent, OrderAddition, 
                                     OrderExecution, OrderCancellation,
                                     OrderDeletion, OrderReplacement>;

                                    //R	Stock Directory	InstrumentDefinition
/*A	Add Order	OrderAdded
E	Order Executed	OrderExecuted
X	Order Cancel	OrderReduced
D	Order Delete	OrderDeleted
U	Order Replace	OrderReplaced
S	System Event	SessionEvent*/
    //Can includes Parameters for the Matchnig Engine
};

#endif