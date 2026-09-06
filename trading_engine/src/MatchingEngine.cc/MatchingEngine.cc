#include "MatchingEngine.h"

class   MatchingEngine
{

    public:
    MatchingEngine() = delete;
    MatchingEngine(OrderBook    &order_book) : order_book_(order_book)
    {

    }

    void    handleOrder(const Msg &order)
    {

        //Check Even Type and process according to it.
        switch(static_cast<uint32_t>(order._event_type))
        {
            case static_cast<uint32_t>(Order_Type::ADD) :
            order_book_.addOrder(order);
            break;

            case static_cast<uint32_t>(Order_Type::UPDATE) :
            break;

            case static_cast<uint32_t>(Order_Type::CANCEL) :
            order_book_.cancelOrder(order);
            break;

        }
        

        //Check Matching Orders
        OrderBook::TopOfTheBook   &top_of_the_book = order_book_.topOfTheBook();
        std::cout << "Before matching condition : " << "Best Ask [" << top_of_the_book.best_ask << ", " << top_of_the_book.best_ask_qty << "]" << std::endl;
        std::cout << "Best Bid [" << top_of_the_book.best_bid << ", " << top_of_the_book.best_bid_qty << "]" << std::endl;  
        while (top_of_the_book.best_bid >= top_of_the_book.best_ask && top_of_the_book.best_ask_qty > 0 && top_of_the_book.best_bid_qty > 0)
        {
            std::cout << "MATCHING between ask[" << top_of_the_book.best_ask << "," << top_of_the_book.best_ask_qty << "] and bid [" << top_of_the_book.best_bid << "," << top_of_the_book.best_bid_qty << "]" << std::endl;
            order_book_.matchingUpdate();
        }
        
    }

    private:
    OrderBook   &order_book_;
    //Can includes Parameters for the Matchnig Engine
};