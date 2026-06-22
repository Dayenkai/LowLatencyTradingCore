#ifndef _MATCHING_ENGINE
#define _MATCHING_ENGINE

#include "Constants.h"
#include "OrderBook.h"

class   MatchingEngine
{

    public:
    MatchingEngine() = default;

    void    handleOrder(const Msg &order)
    {
        switch(static_cast<uint32_t>(order._event_type))
        {
            case static_cast<uint32_t>(Order_Type::ADD) :
            book.addOrder(order);
            break;

            case static_cast<uint32_t>(Order_Type::UPDATE) :
            break;

            case static_cast<uint32_t>(Order_Type::CANCEL) :
            book.cancelOrder(order);
            break;

        }
        

        //Check Matching
        std::pair<uint32_t, uint32_t>   &best_ask_bid = book.topOfTheBook();       
        while (book.getAskOrders()[best_ask_bid.first-BASE_SELLING_TICK] != 0 && book.getBidOrders()[-1*(best_ask_bid.second)+BASE_BUYING_TICK] != 0 && best_ask_bid.second >= best_ask_bid.first)
        {
            std::cout << "MATCHING between ask[" << best_ask_bid.first << "," << book.getAskOrders()[best_ask_bid.first-BASE_SELLING_TICK] << "] and bid [" << best_ask_bid.second << "," << book.getBidOrders()[BASE_BUYING_TICK-best_ask_bid.second] << "]" << std::endl;
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