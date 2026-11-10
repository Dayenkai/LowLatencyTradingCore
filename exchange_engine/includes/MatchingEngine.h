#ifndef _MATCHING_ENGINE
#define _MATCHING_ENGINE

#include "Constants.h"
#include "OrderBook.h"

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
    OrderBook   book;
    //Can includes Parameters for the Matchnig Engine
};

#endif