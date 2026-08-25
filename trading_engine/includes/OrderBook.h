#include "CppStandard.h"
#include "MemoryPool.h"
#include "PersonnalLibrary.h"

typedef class OrderBook
{
    public:
        OrderBook()
        {
            bid_orders.resize(BAND_SIZE);
            ask_orders.resize(BAND_SIZE);
            bitmapBids.resize(BITMAP_SIZE);
            bitmapAsks.resize(BITMAP_SIZE);

            //price_lvl_order_ids.resize(BAND_SIZE);

            top_of_the_book_.best_ask = UINT32_MAX;
            top_of_the_book_.best_ask_qty = 0;
            top_of_the_book_.ask_out_of_band = false;

            top_of_the_book_.best_bid = 0;
            top_of_the_book_.best_bid_qty = 0;
            top_of_the_book_.bid_out_of_band = false;
        }

        //Create a Function that returns PRICE LVL of ASK(LVL2)

        //Create a Function that returns PRICE LVL of BID(LVL2)


        //Create a Function that returns Details Order of ASK(LVL3)

        //Create a Function that returns Details Order of BID(LVL3)

        void                            addOrder(const Msg &order)
        {
            if (order.price_ > 0 && order.qty_ > 0)
            {
                if (static_cast<Side>(order._side) == Side::Buy)
                {
                    // auto start = Clock::now();
                
                    // auto end    = Clock::now();
                    // std::cout << "Update inside the Buying vector, with order price " << order.price_ << " with index " << BASE_BUYING_TICK - order.price_ <<  " takes : " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << std::endl;
                    if (order.price_ <= BASE_BUYING_TICK)
                    {
                        bid_orders[BASE_BUYING_TICK - order.price_] += order.qty_;
                        //[id] = qty
                        //PriceLevel      &price_lvl = price_lvl_order_ids[BASE_BUYING_TICK - order.price_];

                        //In case the nb of orders exceeds 1000 in a particularly price level, we use unfortunately an allocation.
                        // if (price_lvl.priority == static_cast<uint32_t>(ORDERS_NB_PER_LVL_PER_PAGE) - 1) [[unlikely]]
                        // {
                        //     price_lvl.order_ids.emplace_back(std::array<uint32_t, 1000>());
                        //     price_lvl.priority = 0;        
                        // }
                        // price_lvl.order_ids[price_lvl.order_ids.size()-1][price_lvl.priority] = price_lvl.priority;
                        // price_lvl.priority++;

                        bitmapBids[(BASE_BUYING_TICK - order.price_)/64] |= 1ULL << (BASE_BUYING_TICK - order.price_)%64;

                        if (order.price_ >= top_of_the_book_.best_bid)
                        {                        
                            //If Orders cancelled, Find Next Best Bid Ask
                            
                            std::cout << "New Best Bid[" << order.price_ <<"," << bid_orders[BASE_BUYING_TICK - order.price_] << "]" <<  std::endl;
                            top_of_the_book_.best_bid = order.price_;//*-1+BASE_BUYING_TICK
                            top_of_the_book_.best_bid_qty = bid_orders[BASE_BUYING_TICK - order.price_];
                            top_of_the_book_.bid_out_of_band = false;
                        }
                        return;
                    }
                    out_of_band_bids[order.price_] += order.qty_;
                    if (order.price_ >= top_of_the_book_.best_bid)
                    {
                        top_of_the_book_.best_bid = order.price_;
                        top_of_the_book_.best_bid_qty = out_of_band_bids[order.price_];
                        top_of_the_book_.bid_out_of_band = true;
                    }
                }
                //     if (order.price_ <= BASE_BUYING_TICK)
                //     {
                //         //std::cout << "The index that we will add is " << BASE_BUYING_TICK - order.price_;
                        
                //         // if (best_bidOrders_idx.first && order.price_ > bid[BASE_BUYING_TICK - best_bid_idx.second])
                //         // {
                //         //     best_bid_idx.first = true;
                //         //     //best_buy_idx.second = order.price_;
                //         // }
                //     }
                //     // else
                //     // {
                //     //     auto start = Clock::now();
                //     //     out_of_band_bids[order.price_] += order.qty_;
                //     //     auto end    = Clock::now();
                //     //     std::cout << "Update inside the out_of_band buying vector takes : " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << std::endl; 
                //     //     // if (out_of_band_bids[order.price_] > 0 && order.price_ > bid[BASE_BUYING_TICK - best_ask_idx])
                //     //     // {
                //     //     //     best_ask_idx.first = true;
                //     //     //     besttick;
                //     //     // }
                //     // }
                // }
                else
                {
                    if (order.price_ >= BASE_SELLING_TICK && order.price_ <= BAND_SIZE-1)
                    {
                        ask_orders[order.price_ - BASE_SELLING_TICK] += order.qty_;
                        bitmapAsks[(order.price_ - BASE_SELLING_TICK)/64] |= 1ULL << (order.price_ - BASE_SELLING_TICK)%64;
                        //If first Ask entering the OrderBook, or ask being inferior to the best Ask
                        if (order.price_ <= top_of_the_book_.best_ask)
                        {
                            std::cout << "New Best Ask[" << order.price_ <<"," << ask_orders[order.price_ - BASE_SELLING_TICK] << "]" <<  std::endl;
                            top_of_the_book_.best_ask = order.price_;//+BASE_SELLING_TICK
                            top_of_the_book_.best_ask_qty = ask_orders[order.price_ - BASE_SELLING_TICK];
                            top_of_the_book_.ask_out_of_band = false;
                        }
                        return;
                    }

                    out_of_band_asks[order.price_] += order.qty_;
                    if (order.price_ <= top_of_the_book_.best_ask)
                    {
                        top_of_the_book_.best_ask = order.price_;//+BASE_SELLING_TICK
                        top_of_the_book_.best_ask_qty = out_of_band_asks[order.price_];
                        top_of_the_book_.ask_out_of_band = true;
                        
                    }
                }
                //     //std::cout << "THe order price is " << order.price_ << std::endl;
                //     //std::cout << "The index that we will add is " << order.price_ - BASE_SELLING_TICK << std::endl;
                //     uint32_t    band_limit = BASE_SELLING_TICK + BAND_SIZE;
                //     if (order.price_ >= BASE_SELLING_TICK && order.price_ <= band_limit)
                //     {
                //         auto start = Clock::now();
                //         
                //         auto end    = Clock::now();
                //         std::cout << "Update inside the selling vector takes : " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << std::endl;
                //     }
                //     else
                //     {
                //         auto start = Clock::now();
                //         out_of_band_asks[order.price_] += order.qty_;
                //         auto end    = Clock::now();
                //         std::cout << "Update inside the selling vector takes : " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << std::endl;
                //     }
                // }
            }
        }

        void                            cancelOrder(const Msg &order)
        {
            /*For Cancel Order
                    if (top_of_the_book_.best_bid == BASE_BUYING_TICK - order.price_ && bid_orders[BASE_BUYING_TICK - order.price_] <= 0)
                    {
                        findTopOfTheBook(Side::Buy, BASE_BUYING_TICK - order.price_);
                    } 
                    */
        }

        void    findTopOfTheBook(const Side& side, const std::vector<uint64_t>& bitmap, const uint64_t &&index)
        {
            std::cout << "[in " << __func__ << "]" << std::endl;

            for (uint64_t i = index; i < bitmap.size(); i++)
            {
                if (bitmap[i] != 0)
                {
                    uint64_t highestpos = __builtin_ctzll(bitmap[i]);
                    std::cout << "New Top Of the Book found in normal range ! Best " << (side == Side::Sell ? "Ask" : "Bid") << " is " << (side == Side::Sell ? (i*64+highestpos) + BASE_SELLING_TICK : BASE_BUYING_TICK - (i*64+highestpos) )  << std::endl;
                    if (side == Side::Sell)
                    {
                        if (!out_of_band_asks.empty() && out_of_band_asks.begin()->first < i*64+highestpos + BASE_SELLING_TICK) [[unlikely]]
                        {
                            top_of_the_book_.best_ask = out_of_band_asks.begin()->first;
                            top_of_the_book_.best_ask_qty = out_of_band_asks.begin()->second;
                            top_of_the_book_.ask_out_of_band = true;
                            return;
                        }
                        top_of_the_book_.best_ask = i*64+highestpos + BASE_SELLING_TICK;
                        top_of_the_book_.best_ask_qty = ask_orders[i*64+highestpos];
                        top_of_the_book_.ask_out_of_band = false;
                    }
                    else
                    {
                        if (!out_of_band_bids.empty() && out_of_band_bids.begin()->first > BASE_BUYING_TICK - i*64+highestpos) [[unlikely]]
                        {
                            top_of_the_book_.best_bid = out_of_band_bids.begin()->first;
                            top_of_the_book_.best_bid_qty = out_of_band_bids.begin()->second;
                            top_of_the_book_.ask_out_of_band = true;
                            return;
                        }
                        top_of_the_book_.best_bid = BASE_BUYING_TICK - i*64+highestpos;
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

        void    ListOrder(const Side &side)
        {
            auto listOrders = [&](std::vector<uint32_t>& lvlPrices)
            {
                auto bbt = static_cast<uint64_t>(BASE_BUYING_TICK);
                auto bst = static_cast<uint64_t>(BASE_SELLING_TICK);
                double orderprice_ = 0.0f;
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
                                orderprice_ = static_cast<double>((itOOBB->first) / (double)10000);
                                itOOBB++;
                                outOfBandPriority = true;
                                qty = itOOBB->second;
                            }
                            else
                            {
                                orderprice_ = static_cast<double>((bbt - i) / (double)10000);
                                qty = lvlPrices[i];
                            }
                        }
                        else
                        {
                            if (itOOBA != out_of_band_bids.end() && itOOBA->first <= bst + i) [[unlikely]]
                            {
                                orderprice_ = static_cast<double>((itOOBA->first) / (double)10000);
                                itOOBA++;
                                outOfBandPriority = true;
                                qty = itOOBB->second;
                            }
                            else
                            {
                                qty = lvlPrices[i];
                                orderprice_ = static_cast<double>((bst + i) / (double)10000);
                            }
                        }
                        std::cout << "Lvl Order Price " << std::fixed << orderprice_ << " (index = " <<  (outOfBandPriority ? i-- : i) << ", Quantity : " << qty << std::endl; 
                        outOfBandPriority = false;
                    }
                }
                if (side == Side::Buy)
                {
                    for (itOOBB; itOOBB != out_of_band_bids.end(); itOOBB++)
                    {
                        orderprice_ = static_cast<double>((itOOBB->first) / (double)10000);
                        std::cout << "Lvl Order Price " << std::fixed << orderprice_ << ", Quantity : " << itOOBB->second << std::endl; 
                    }

                }
                else
                {
                    for (itOOBA; itOOBA != out_of_band_asks.end(); itOOBA++)
                    {
                        orderprice_ = static_cast<double>((itOOBA->first) / (double)10000);
                        std::cout << "Lvl Order Price " << std::fixed << orderprice_ << ", Quantity : " << itOOBA->second << std::endl; 
                    }
                }
            };
            if (side == Side::Buy)
            {
                log("Listing of Bid Orders :");
                listOrders(bid_orders);
            }
            else
            {
                log("Listing of Sell Orders :");
                listOrders(ask_orders);
            }
        }

        std::vector<uint32_t>   &getAskOrders()
        {
            return ask_orders;
        }

        std::vector<uint32_t>   &getBidOrders()
        {
            return bid_orders;
        }

        void                    matchingUpdate()
        {
            bool                hasBeenOutOfBandAsks = true;
            bool                hasBeenOutOfBandBids = true;

            std::cout << "[in " << __func__ << "]" << std::endl;
            uint32_t minQty = std::min(top_of_the_book_.best_ask_qty, top_of_the_book_.best_bid_qty);

            top_of_the_book_.best_ask_qty -= minQty;
            top_of_the_book_.best_bid_qty -= minQty;

            if (top_of_the_book_.ask_out_of_band == false)
            {
                hasBeenOutOfBandAsks = false;
                ask_orders[top_of_the_book_.best_ask - BASE_SELLING_TICK] -= minQty;
                if (top_of_the_book_.best_ask_qty == 0)
                {
                    bitmapAsks[(top_of_the_book_.best_ask - BASE_SELLING_TICK)/64] &= ~(1ULL << (top_of_the_book_.best_ask - BASE_SELLING_TICK) % 64);
                    findTopOfTheBook(Side::Sell, bitmapAsks, (top_of_the_book_.best_ask - BASE_SELLING_TICK)/64);
                }
            }
            if (top_of_the_book_.bid_out_of_band == false)
            {
                hasBeenOutOfBandBids = false;
                bid_orders[BASE_BUYING_TICK - top_of_the_book_.best_bid] -= minQty;
                if (top_of_the_book_.best_bid_qty == 0)
                {
                    bitmapBids[(BASE_BUYING_TICK - top_of_the_book_.best_bid)/64] &= ~(1ULL << (BASE_BUYING_TICK - top_of_the_book_.best_bid) % 64);
                    findTopOfTheBook(Side::Buy, bitmapBids, (BASE_BUYING_TICK - top_of_the_book_.best_bid)/64);
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

    struct  TopOfTheBook
    {
        uint32_t    best_ask;
        uint32_t    best_ask_qty;
        uint32_t    best_bid;
        uint32_t    best_bid_qty;
        bool        ask_out_of_band = false;
        bool        bid_out_of_band = false;
    };

    TopOfTheBook&   topOfTheBook()
    {
        return top_of_the_book_;
    }

    struct      PriceLevel
    {
        std::vector<std::array<uint32_t, 1000>>      order_ids = std::vector<std::array<uint32_t, 1000>>(1);
        uint32_t                                     priority = 0;
    };

    private:
    alignas(64) std::vector<uint32_t>                                           bid_orders;
    alignas(64) std::vector<uint32_t>                                           ask_orders;

    alignas(64) std::vector<PriceLevel>                                         price_lvl_order_ids;

    alignas(64) std::vector<uint64_t>                                           bitmapBids;
    alignas(64) std::vector<uint64_t>                                           bitmapAsks;

    alignas(64) std::map<uint32_t, uint32_t>                                    out_of_band_asks;
    alignas(64) std::map<uint32_t, uint32_t, std::greater<int>>                 out_of_band_bids;
    alignas(64) TopOfTheBook                                                    top_of_the_book_;

}OrderBook;