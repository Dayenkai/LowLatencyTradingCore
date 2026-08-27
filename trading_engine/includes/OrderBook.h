#include "CppStandard.h"
#include "MemoryPool.h"
#include "PersonnalLibrary.h"


class OrderBook
{
    public:
        struct  TopOfTheBook
        {
            uint32_t    best_ask;
            uint32_t    best_ask_qty;
            uint32_t    best_bid;
            uint32_t    best_bid_qty;
            bool        ask_out_of_band = false;
            bool        bid_out_of_band = false;
        };
        struct      PriceLevel
        {
            std::vector<std::array<uint32_t, 1000>>      order_ids = std::vector<std::array<uint32_t, 1000>>(1);
            uint32_t                                     priority = 0;
        };

        OrderBook();
        void addOrder(const Msg &order);

        //Create a Function that returns PRICE LVL of ASK(LVL2)
        //Create a Function that returns PRICE LVL of BID(LVL2)
        //Create a Function that returns Details Order of ASK(LVL3)
        //Create a Function that returns Details Order of BID(LVL3)

        void                            cancelOrder(const Msg &order);
        std::vector<uint32_t>           &getAskOrders();
        std::vector<uint32_t>           &getBidOrders();
        void                            matchingUpdate();
        void                            findTopOfTheBook(const Side& side, const std::vector<uint64_t>& bitmap, const uint64_t &&index);
        void                            ListOrder(const Side &side);
        TopOfTheBook&                   topOfTheBook();

    private:
    alignas(64) std::vector<uint32_t>                                           bid_orders;
    alignas(64) std::vector<uint32_t>                                           ask_orders;

    alignas(64) std::vector<PriceLevel>                                         price_lvl_order_ids;

    alignas(64) std::vector<uint64_t>                                           bitmapBids;
    alignas(64) std::vector<uint64_t>                                           bitmapAsks;

    alignas(64) std::map<uint32_t, uint32_t>                                    out_of_band_asks;
    alignas(64) std::map<uint32_t, uint32_t, std::greater<int>>                 out_of_band_bids;
    alignas(64) TopOfTheBook                                                    top_of_the_book_;

};