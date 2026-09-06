#ifndef  FEED_HANDLER_
#define  FEED_HANDLER_

#include "MemoryPool.h"
#include "CppStandard.h"
#include "NicReplay.h"
#include "MatchingEngine.h"



class   FeedHandler
{
    public:
    FeedHandler() = default;
    FeedHandler(uint32_t    &coreId) : coreId_(coreId)
    {
    }

    int            run(MemoryPool &pool, std::atomic<Msg>   &msg);
    inline void    parse(RxDesc &desc, Msg &msg);
    void           buildOrder(Msg &msg, Order &order);
    template<typename T>
    inline void    fillValueLe(T &dest, std::byte* val);

    private:
    uint32_t       coreId_ = 0;
};

#endif