#include "CppStandard.h"
#include "MemoryPool.h"
#include "Constants.h"


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

        struct      EventsRecord
        {
            EventsRecord() : event_head(0), event_tail(0), buffering(false), size(0)
            {
                
            }
            alignas(64) std::array<std::variant<UDPMulticastEvent>, exchange_engine::MAX_EVENT_PER_TRANSM>  events_ring;
            uint8_t event_head;
            uint8_t event_tail;
            uint8_t size;//We get to have a size because we don't want to rely on branching 
            //to get the accurate size of the current active elements in the events_ring, that could cause misprediction cases from CPU
            bool    buffering;
            /* data */
        };
        

        OrderBook();
        std::optional<Order> addOrder(const Msg &order);

        //Create a Function that returns PRICE LVL of ASK(LVL2)
        //Create a Function that returns PRICE LVL of BID(LVL2)
        //Create a Function that returns Details Order of ASK(LVL3)
        //Create a Function that returns Details Order of BID(LVL3)

        std::optional<Order>            cancelOrder(const Msg &order);
        std::vector<uint32_t>           &getAskOrders();
        std::vector<uint32_t>           &getBidOrders();
        void                            matchingUpdate();
        void                            findTopOfTheBook(const Side& side, const std::vector<uint64_t>& bitmap, const uint64_t &&index);
        void                            ListOrder(const Side &side);
        TopOfTheBook&                   topOfTheBook();
        EventsRecord&                   getEventsRecord();

        //Computational Purposes
        inline uint32_t                 normalizeBidPrice(const uint32_t &price) const;
        inline uint32_t                 normalizeAskPrice(const uint32_t &price) const;
        inline uint32_t                 normalizePrice(const uint32_t &price, const Side &side) const;
        inline uint32_t                 normalizePrice(const uint32_t &price, const uint32_t &price_reference, const Side &side) const;

    private:
    
    //LVL1 OrderBook
    alignas(64) TopOfTheBook                                                    top_of_the_book_;

    //LVL2 OrderBook
    alignas(64) std::vector<uint32_t>                                  bid_orders;
    alignas(64) std::vector<uint32_t>                                  ask_orders;

    alignas(64) std::vector<uint64_t>                                  bitmapBids;
    alignas(64) std::vector<uint64_t>                                  bitmapAsks;

    alignas(64) std::map<uint32_t, uint32_t>                           out_of_band_asks;
    alignas(64) std::map<uint32_t, uint32_t, std::greater<int>>        out_of_band_bids;

    

    //LVL3 OrderBook
    alignas(64) std::map<uint64_t, std::forward_list<uint32_t>, 
                        std::greater<uint64_t>>                        lvl3_bid_orders;
    alignas(64) std::map<uint64_t, std::forward_list<uint32_t>>        lvl3_ask_orders;

    //Client Orders - Authoritative Clients Orders
    alignas(64) std::unordered_map<uint64_t, Order>                    clients_orders;

    EventsRecord                                                       events_record;

    alignas(64) uint64_t                                               id_incremental;
    char                                                               stock[9] = "AAPL\0";
    uint16_t                                                           stock_locate_;


};