#ifndef _MATCHING_ENGINE
#define _MATCHING_ENGINE

#include "Constants.h"
#include "OrderBook.h"

class   MatchingEngine
{

    public:
    MatchingEngine() = default;

    void    handleOrder(const Order &order)
    {
        book.addOrder(order);
        if (order._side == Side::Sell)
        {
            //if ()
        }
    }


    private:
    Orderbook   book;
    //Can includes Parameters for the Matchnig Engine
};








#endif