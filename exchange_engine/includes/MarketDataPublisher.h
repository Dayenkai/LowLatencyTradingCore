#ifndef _MARKET_DATA_PUBLISHER_H
#define _MARKET_DATA_PUBLISHER_H

#include "MemoryPool.h"
#include "Constants.h"

void        MarketDataPublisher(RxRingBuffer<ExchangeInetConst::TX_RING_SIZE, public_ITCH_Messages>   &tx_inet_RingBuffer);

#endif