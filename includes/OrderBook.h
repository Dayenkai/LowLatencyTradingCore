#include "CppStandard.h"
#include "MemoryPool.h"
#include "PersonnalLibrary.h"

typedef class OrderBook
{
    public:
        OrderBook() : best_ask_bidIdx({BAND_SIZE-1,0})
        {
            bid_orders.resize(BAND_SIZE);
            ask_orders.resize(BAND_SIZE);
            bitmapBids.resize(BITMAP_SIZE);
            bitmapAsks.resize(BITMAP_SIZE);
            out_of_band_buy.reserve(BAND_SIZE);
            out_of_band_sell.reserve(BAND_SIZE);
        }

        std::pair<uint32_t, uint32_t>&   topOfTheBook()
        {
            return best_ask_bidIdx;
        }

        //Create a Function that returns PRICE LVL of ASK(LVL2)

        //Create a Function that returns PRICE LVL of BID(LVL2)


        //Create a Function that returns Details Order of ASK(LVL3)

        //Create a Function that returns Details Order of BID(LVL3)

        void                            addOrder(const Msg &order)
        {
            if (order._price > 0 && order._qty > 0)
            {
                if (static_cast<Side>(order._side) == Side::Buy)
                {
                    // auto start = Clock::now();
                
                    // auto end    = Clock::now();
                    // std::cout << "Update inside the Buying vector, with order price " << order._price << " with index " << BASE_BUYING_TICK - order._price <<  " takes : " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << std::endl; 
                    bid_orders[BASE_BUYING_TICK - order._price] += order._qty;
                    bitmapBids[(BASE_BUYING_TICK - order._price)/64] |= 1ULL << (BASE_BUYING_TICK - order._price)%64;

                    if (order._price > best_ask_bidIdx.second)
                    {                        
                        //If Orders cancelled, Find Next Best Bid Ask
                        
                        std::cout << "New Best Bid[" << order._price <<"," << bid_orders[BASE_BUYING_TICK - order._price] << "]" <<  std::endl;
                        best_ask_bidIdx.second = order._price;//*-1+BASE_BUYING_TICK
                    }
                    
                    
                }
                //     if (order._price <= BASE_BUYING_TICK)
                //     {
                //         //std::cout << "The index that we will add is " << BASE_BUYING_TICK - order._price;
                        
                //         // if (best_bidOrders_idx.first && order._price > bid[BASE_BUYING_TICK - best_bid_idx.second])
                //         // {
                //         //     best_bid_idx.first = true;
                //         //     //best_buy_idx.second = order._price;
                //         // }
                //     }
                //     // else
                //     // {
                //     //     auto start = Clock::now();
                //     //     out_of_band_buy[order._price] += order._qty;
                //     //     auto end    = Clock::now();
                //     //     std::cout << "Update inside the out_of_band buying vector takes : " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << std::endl; 
                //     //     // if (out_of_band_buy[order._price] > 0 && order._price > bid[BASE_BUYING_TICK - best_ask_idx])
                //     //     // {
                //     //     //     best_ask_idx.first = true;
                //     //     //     besttick;
                //     //     // }
                //     // }
                // }
                else
                {
                    ask_orders[order._price - BASE_SELLING_TICK] += order._qty;
                    bitmapAsks[(order._price - BASE_SELLING_TICK)/64] |= 1ULL << (order._price - BASE_SELLING_TICK)%64;
                    //If first Ask entering the OrderBook, or ask being inferior to the best Ask
                    if (order._price < best_ask_bidIdx.first)
                    {
                        std::cout << "New Best Ask[" << order._price <<"," << ask_orders[order._price - BASE_SELLING_TICK] << "]" <<  std::endl;
                        best_ask_bidIdx.first = order._price;//+BASE_SELLING_TICK
                    }
                }
                //     //std::cout << "THe order price is " << order._price << std::endl;
                //     //std::cout << "The index that we will add is " << order._price - BASE_SELLING_TICK << std::endl;
                //     uint32_t    band_limit = BASE_SELLING_TICK + BAND_SIZE;
                //     if (order._price >= BASE_SELLING_TICK && order._price <= band_limit)
                //     {
                //         auto start = Clock::now();
                //         
                //         auto end    = Clock::now();
                //         std::cout << "Update inside the selling vector takes : " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << std::endl;
                //     }
                //     else
                //     {
                //         auto start = Clock::now();
                //         out_of_band_sell[order._price] += order._qty;
                //         auto end    = Clock::now();
                //         std::cout << "Update inside the selling vector takes : " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << std::endl;
                //     }
                // }
            }
        }

        void                            cancelOrder(const Msg &order)
        {
            /*For Cancel Order
                    if (best_ask_bidIdx.second == BASE_BUYING_TICK - order._price && bid_orders[BASE_BUYING_TICK - order._price] <= 0)
                    {
                        findTopOfTheBook(Side::Buy, BASE_BUYING_TICK - order._price);
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
                    std::cout << "New Top Of the Book ! Best " << (side == Side::Sell ? "Ask" : "Bid") << " is " << (side == Side::Sell ? (i*64+highestpos) + BASE_SELLING_TICK : BASE_BUYING_TICK - (i*64+highestpos) )  << std::endl;
                    if (side == Side::Sell)
                    {
                        best_ask_bidIdx.first = i*64+highestpos + BASE_SELLING_TICK;
                    }
                    else
                    {
                        best_ask_bidIdx.second = BASE_BUYING_TICK - i*64+highestpos;
                    }
                    return;
                }
            }
            if (side == Side::Sell)
            {
                best_ask_bidIdx.first = bid_orders.size()-1;
            }
            else
            {
                best_ask_bidIdx.second = 0;
            }
        }

        void    ListOrder(const Side &side)
        {
            auto listOrders = [&](std::vector<uint32_t>& lvlPrices)
            {
                auto bbt = static_cast<uint64_t>(BASE_BUYING_TICK);
                auto bst = static_cast<uint64_t>(BASE_SELLING_TICK);
                double order_price = 0.0f;
                for (int i = 0; i < lvlPrices.size(); i++)
                {
                    if (lvlPrices[i] > 0)
                    {
                        if (side == Side::Buy)
                        {
                            order_price = static_cast<double>((bbt - i) / (double)10000);
                        }
                        else
                        {
                            order_price = static_cast<double>((bst + i) / (double)10000);
                        }
                        std::cout << "Lvl Order Price " << std::fixed << order_price << " (index = " <<  i << ", Quantity : " << lvlPrices[i] << std::endl; 
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
            std::cout << "[in " << __func__ << "]" << std::endl;
            uint32_t minQty = std::min(ask_orders[best_ask_bidIdx.first - BASE_SELLING_TICK], bid_orders[BASE_BUYING_TICK - best_ask_bidIdx.second]);
            ask_orders[best_ask_bidIdx.first - BASE_SELLING_TICK] -= minQty;
            bid_orders[BASE_BUYING_TICK - best_ask_bidIdx.second] -= minQty;
            

            if (ask_orders[best_ask_bidIdx.first - BASE_SELLING_TICK] == 0)
            {
                bitmapAsks[(best_ask_bidIdx.first - BASE_SELLING_TICK)/64] &= ~(1ULL << (best_ask_bidIdx.first - BASE_SELLING_TICK) % 64);
                findTopOfTheBook(Side::Sell, bitmapAsks, (best_ask_bidIdx.first - BASE_SELLING_TICK)/64);
            }
            if (bid_orders[BASE_BUYING_TICK - best_ask_bidIdx.second] == 0)
            {
                bitmapBids[(BASE_BUYING_TICK - best_ask_bidIdx.second)/64] &= ~(1ULL << (BASE_BUYING_TICK - best_ask_bidIdx.second) % 64);
                findTopOfTheBook(Side::Buy, bitmapBids, (BASE_BUYING_TICK - best_ask_bidIdx.second)/64);
            }
            std::cout << "[in End of" << __func__ << "]" << std::endl;
        }

    private:
    alignas(64) std::vector<uint32_t>                     bid_orders;
    alignas(64) std::vector<uint32_t>                     ask_orders;
    alignas(64) std::vector<uint64_t>                     bitmapBids;
    alignas(64) std::vector<uint64_t>                     bitmapAsks;
    alignas(64) std::unordered_map<uint32_t, uint32_t>    out_of_band_sell;
    alignas(64) std::unordered_map<uint32_t, uint32_t>    out_of_band_buy;
    alignas(64) std::pair<uint32_t, uint32_t>             best_ask_bidIdx;

}OrderBook;