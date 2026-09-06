#ifndef _MARKET_DATA_PUBLISHER_H
#define _MARKET_DATA_PUBLISHER_H

#include "MemoryPool.h"
#include "Constants.h"

void        MarketDataPublisher(RxRingBuffer<vector_golden_byte, ExchangeInetConst::TX_RING_SIZE>   &tx_inet_RingBuffer);

#endif