#include "OrderBook.h"
#include "CppStandard.h"

OrderBook::OrderBook()
{
    bid_orders.resize(exchange_engine::BAND_SIZE);
    ask_orders.resize(exchange_engine::BAND_SIZE);
    bitmapBids.resize(exchange_engine::BITMAP_SIZE);
    bitmapAsks.resize(exchange_engine::BITMAP_SIZE);

    //price_lvl_order_ids.resize(exchange_engine::BAND_SIZE);

    top_of_the_book_.best_ask = UINT32_MAX;
    top_of_the_book_.best_ask_qty = 0;
    top_of_the_book_.ask_out_of_band = false;

    top_of_the_book_.best_bid = 0;
    top_of_the_book_.best_bid_qty = 0;
    top_of_the_book_.bid_out_of_band = false;

    events_record.event_head = 0;
    events_record.event_tail = 0;

    id_incremental = 1;
}

std::optional<Order> OrderBook::addOrder(const Msg &order)
{
    if (order.price_ > 0 && order.qty_ > 0)
    {
        Order order;
        order.setId(id_incremental).setType(order.event_type_).setSide(order.side_).setPrice(order.price_).setQty(order.qty_).setStatus(order.status_);
        clients_orders[id_incremental] = order;
        //orders_ids[id_incremental] = order;
        ++id_incremental;

        uint32_t    standardized_price = normalizePrice(order.price_, order.side_);
        {
            auto it = lvl3_bid_orders[standardized_price].begin();
            if (it == lvl3_bid_orders[standardized_price].end())
            {
                lvl3_bid_orders[standardized_price].push_front(order.id_);  
            }
            else
            {
                while (std::next(it, 1) != lvl3_bid_orders[standardized_price].end())
                {
                    std::advance(it, 1);
                }
                lvl3_bid_orders[standardized_price].insert_after(it, order.id_);
            }
        }

        if (static_cast<Side>(order.side_) == Side::Buy)
        {
            if (order.price_ <= exchange_engine::BASE_BUYING_TICK)[[likely]]
            {
                bid_orders[standardized_price] += order.qty_;
                bitmapBids[standardized_price/64] |= 1ULL << standardized_price%64;

                if (order.price_ >= top_of_the_book_.best_bid)
                {                        
                    //If Orders cancelled, Find Next Best Bid Ask
                    
                    std::cout << "New Best Bid[" << order.price_ <<"," << bid_orders[standardized_price] << "]" <<  std::endl;
                    top_of_the_book_.best_bid = order.price_;//*-1+exchange_engine::BASE_BUYING_TICK
                    top_of_the_book_.best_bid_qty = bid_orders[standardized_price];
                    top_of_the_book_.bid_out_of_band = false;
                }
                return order;
            }
            out_of_band_bids[order.price_] += order.qty_;
            if (order.price_ >= top_of_the_book_.best_bid)
            {
                top_of_the_book_.best_bid = order.price_;
                top_of_the_book_.best_bid_qty = out_of_band_bids[order.price_];
                top_of_the_book_.bid_out_of_band = true;
            }
        }
        else
        {
            if (order.price_ >= exchange_engine::BASE_SELLING_TICK && order.price_ <= exchange_engine::BAND_SIZE-1)
            {
                ask_orders[standardized_price] += order.qty_;
                bitmapAsks[(standardized_price)/64] |= 1ULL << (standardized_price)%64;
                //If first Ask entering the OrderBook, or ask being inferior to the best Ask
                if (order.price_ <= top_of_the_book_.best_ask)
                {
                    std::cout << "New Best Ask[" << order.price_ <<"," << ask_orders[standardized_price] << "]" <<  std::endl;
                    top_of_the_book_.best_ask = order.price_;//+exchange_engine::BASE_SELLING_TICK
                    top_of_the_book_.best_ask_qty = ask_orders[standardized_price];
                    top_of_the_book_.ask_out_of_band = false;
                }
                return order;
            }

            out_of_band_asks[order.price_] += order.qty_;
            if (order.price_ <= top_of_the_book_.best_ask)
            {
                top_of_the_book_.best_ask = order.price_;//+exchange_engine::BASE_SELLING_TICK
                top_of_the_book_.best_ask_qty = out_of_band_asks[order.price_];
                top_of_the_book_.ask_out_of_band = true;
                
            }
        }
        return order;
    }
    return std::nullopt;
}

std::optional<Order>             OrderBook::cancelOrder(const Msg &order)
{
    return std::nullopt;
}

void    OrderBook::findTopOfTheBook(const Side& side, const std::vector<uint64_t>& bitmap, const uint64_t &&index)
{
    std::cout << "[in " << __func__ << "]" << std::endl;

    for (uint64_t i = index; i < bitmap.size(); i++)
    {
        if (bitmap[i] != 0)
        {
            uint64_t highestpos = __builtin_ctzll(bitmap[i]);
            std::cout << "New Top Of the Book found in normal range ! Best " << (side == Side::Sell ? "Ask" : "Bid") << " is " << (side == Side::Sell ? (i*64+highestpos) + exchange_engine::BASE_SELLING_TICK : exchange_engine::BASE_BUYING_TICK - (i*64+highestpos) )  << std::endl;
            if (side == Side::Sell)
            {
                if (!out_of_band_asks.empty() && out_of_band_asks.begin()->first < i*64+highestpos + exchange_engine::BASE_SELLING_TICK) [[unlikely]]
                {
                    top_of_the_book_.best_ask = out_of_band_asks.begin()->first;
                    top_of_the_book_.best_ask_qty = out_of_band_asks.begin()->second;
                    top_of_the_book_.ask_out_of_band = true;
                    return;
                }
                top_of_the_book_.best_ask = i*64+highestpos + exchange_engine::BASE_SELLING_TICK;
                top_of_the_book_.best_ask_qty = ask_orders[i*64+highestpos];
                top_of_the_book_.ask_out_of_band = false;
            }
            else
            {
                if (!out_of_band_bids.empty() && out_of_band_bids.begin()->first > exchange_engine::BASE_BUYING_TICK - i*64+highestpos) [[unlikely]]
                {
                    top_of_the_book_.best_bid = out_of_band_bids.begin()->first;
                    top_of_the_book_.best_bid_qty = out_of_band_bids.begin()->second;
                    top_of_the_book_.ask_out_of_band = true;
                    return;
                }
                top_of_the_book_.best_bid = exchange_engine::BASE_BUYING_TICK - i*64+highestpos;
                top_of_the_book_.best_bid_qty = bid_orders[i*64+highestpos];
                top_of_the_book_.ask_out_of_band = false;
            }
            return;
        }
    }
    if (side == Side::Sell)
    {
        if (!out_of_band_asks.empty())[[unlikely]]
        {
            top_of_the_book_.best_ask = out_of_band_asks.begin()->first;
            top_of_the_book_.best_bid_qty = out_of_band_asks.begin()->second;
            top_of_the_book_.ask_out_of_band = true;
            return;
        }
        top_of_the_book_.best_ask = UINT32_MAX;
        top_of_the_book_.best_ask_qty = 0;
        top_of_the_book_.ask_out_of_band = false;
    }
    else
    {
        if (!out_of_band_bids.empty())[[unlikely]]
        {
            top_of_the_book_.best_bid = out_of_band_bids.begin()->first;
            top_of_the_book_.best_bid_qty = out_of_band_bids.begin()->second;
            top_of_the_book_.ask_out_of_band = true;
            return;
        }
        top_of_the_book_.best_bid = 0;
        top_of_the_book_.best_bid_qty = 0;
        top_of_the_book_.bid_out_of_band = false;
    }
}

void    OrderBook::ListOrder(const Side &side) 
{
    auto listOrders = [&](std::vector<uint32_t>& lvlPrices)
    {
        auto bbt = static_cast<uint64_t>(exchange_engine::BASE_BUYING_TICK);
        auto bst = static_cast<uint64_t>(exchange_engine::BASE_SELLING_TICK);
        double orderprice = 0.0f;
        auto itOOBB = out_of_band_bids.begin();
        auto itOOBA = out_of_band_asks.begin();
        bool outOfBandPriority = false;

        uint32_t    qty;
        for (int i = 0; i < lvlPrices.size(); i++)
        {
            if (lvlPrices[i] > 0)
            {
                if (side == Side::Buy)
                {
                    if (itOOBB != out_of_band_bids.end() && itOOBB->first >= bbt - i) [[unlikely]]
                    {
                        orderprice = static_cast<double>((itOOBB->first) / (double)10000);
                        itOOBB++;
                        outOfBandPriority = true;
                        qty = itOOBB->second;
                    }
                    else
                    {
                        orderprice = static_cast<double>((bbt - i) / (double)10000);
                        qty = lvlPrices[i];
                    }
                }
                else
                {
                    if (itOOBA != out_of_band_bids.end() && itOOBA->first <= bst + i) [[unlikely]]
                    {
                        orderprice = static_cast<double>((itOOBA->first) / (double)10000);
                        itOOBA++;
                        outOfBandPriority = true;
                        qty = itOOBB->second;
                    }
                    else
                    {
                        qty = lvlPrices[i];
                        orderprice = static_cast<double>((bst + i) / (double)10000);
                    }
                }
                std::cout << "Lvl Order Price " << std::fixed << orderprice << " (index = " <<  (outOfBandPriority ? i-- : i) << ", Quantity : " << qty << std::endl; 
                outOfBandPriority = false;
            }
        }
        if (side == Side::Buy)
        {
            for (itOOBB; itOOBB != out_of_band_bids.end(); itOOBB++)
            {
                orderprice = static_cast<double>((itOOBB->first) / (double)10000);
                std::cout << "Lvl Order Price " << std::fixed << orderprice << ", Quantity : " << itOOBB->second << std::endl; 
            }

        }
        else
        {
            for (itOOBA; itOOBA != out_of_band_asks.end(); itOOBA++)
            {
                orderprice = static_cast<double>((itOOBA->first) / (double)10000);
                std::cout << "Lvl Order Price " << std::fixed << orderprice << ", Quantity : " << itOOBA->second << std::endl; 
            }
        }
    };
    if (side == Side::Buy)
    {
        std::cout << "Listing of Bid Orders :" << std::endl;
        listOrders(bid_orders);
    }
    else
    {
        std::cout << "Listing of Sell Orders :" << std::endl;
        listOrders(ask_orders);
    }
}

std::vector<uint32_t>   &OrderBook::getAskOrders()
{
    return ask_orders;
}

std::vector<uint32_t>   &OrderBook::getBidOrders()
{
    return bid_orders;
}

void                    OrderBook::matchingUpdate()
{
    bool                hasBeenOutOfBandAsks = true;
    bool                hasBeenOutOfBandBids = true;

    std::cout << "[in " << __func__ << "]" << std::endl;
    uint32_t minQty = std::min(top_of_the_book_.best_ask_qty, top_of_the_book_.best_bid_qty);

    top_of_the_book_.best_ask_qty -= minQty;
    top_of_the_book_.best_bid_qty -= minQty;

    //Update Client_orders

    if (top_of_the_book_.ask_out_of_band == false)
    {
        hasBeenOutOfBandAsks = false;
        ask_orders[top_of_the_book_.best_ask - exchange_engine::BASE_SELLING_TICK] -= minQty;
        if (top_of_the_book_.best_ask_qty == 0)
        {
            bitmapAsks[(top_of_the_book_.best_ask - exchange_engine::BASE_SELLING_TICK)/64] &= ~(1ULL << (top_of_the_book_.best_ask - exchange_engine::BASE_SELLING_TICK) % 64);
            findTopOfTheBook(Side::Sell, bitmapAsks, (top_of_the_book_.best_ask - exchange_engine::BASE_SELLING_TICK)/64);
        }
    }
    if (top_of_the_book_.bid_out_of_band == false)
    {
        hasBeenOutOfBandBids = false;
        bid_orders[exchange_engine::BASE_BUYING_TICK - top_of_the_book_.best_bid] -= minQty;
        if (top_of_the_book_.best_bid_qty == 0)
        {
            bitmapBids[(exchange_engine::BASE_BUYING_TICK - top_of_the_book_.best_bid)/64] &= ~(1ULL << (exchange_engine::BASE_BUYING_TICK - top_of_the_book_.best_bid) % 64);
            findTopOfTheBook(Side::Buy, bitmapBids, (exchange_engine::BASE_BUYING_TICK - top_of_the_book_.best_bid)/64);
        }
    }


    if (hasBeenOutOfBandAsks) [[unlikely]]
    {
        if (!out_of_band_asks.empty())
        {
            std::cout << " Matching ask update top of the book" << std::endl;
            auto it = out_of_band_asks.begin();
            it->second -= minQty;
            if (it->second == 0)
            {
                std::cout << "Erasing " << it->second  << "..." << std::endl;
                out_of_band_asks.erase(it);
            }
        }
        findTopOfTheBook(Side::Sell, bitmapAsks, 0);
        
    }
    if (hasBeenOutOfBandBids) [[unlikely]]
    {
        std::cout << " Matching bid update top of the book" << std::endl;
        if (!out_of_band_bids.empty())
        {
            auto it = out_of_band_bids.begin();
            it->second -= minQty;
            if (it->second == 0)
            {
                std::cout << "Erasing " << it->second  << "..." << std::endl;
                out_of_band_bids.erase(it);
            }
        }
        findTopOfTheBook(Side::Buy, bitmapBids, 0);
    }
    
    
    std::cout << "[in End of" << __func__ << "]" << std::endl;
}

OrderBook::TopOfTheBook&   OrderBook::topOfTheBook()
{
    return top_of_the_book_;
}

OrderBook::EventsRecord&  OrderBook::getEventsRecord()
{
    return events_record;
}


inline uint32_t   OrderBook::normalizeBidPrice(const uint32_t &price) const
{
    return exchange_engine::BASE_BUYING_TICK - price;
}

inline uint32_t   OrderBook::normalizeAskPrice(const uint32_t &price) const
{
    return price - exchange_engine::BASE_SELLING_TICK;
}

inline uint32_t   OrderBook::normalizePrice(const uint32_t &price, const Side &side) const
{
    return (side == Side::Buy ? exchange_engine::BASE_BUYING_TICK-price : price - exchange_engine::BASE_SELLING_TICK);
}

inline uint32_t   OrderBook::normalizePrice(const uint32_t &price, const uint32_t &price_reference, const Side &side) const
{
    return (side == Side::Buy ? price_reference-price : price - price_reference);
}

/*
NB : 
auto start = Clock::now();
        //         out_of_band_asks[order.price_] += order.qty_;
        //         auto end    = Clock::now();
        //         std::cout << "Update inside the selling vector takes : " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << std::endl;
*/

