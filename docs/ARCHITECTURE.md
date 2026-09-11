                           ┌─────────────────────────┐
                           │       EXCHANGE          │
                           └───────────┬─────────────┘
                                       │
                Market Data            │ Order Entry / Responses
                UDP/multicast          │ TCP/native protocol
                     │                 │
                     ▼                 ▼
              ┌─────────────┐    ┌───────────────┐
              │ Feed Handler│    │ Order Gateway │
              │             │    │               │
              │ packet seq  │    │ session       │
              │ gap detect  │    │ encode/decode │
              │ decode      │    │ TCP state     │
              └──────┬──────┘    └───────┬───────┘
                     │                   │
                     ▼                   ▼
              ┌───────────────────────────────────┐
              │           TRADING CORE            │
              │                                   │
              │  Order Book                       │
              │      ↓                            │
              │  Features / Fair Value            │
              │      ↓                            │
              │  Strategy                         │
              │      ↓                            │
              │  Order Manager                    │
              │      ↓                            │
              │  Pre-trade Risk                   │
              │      ↓                            │
              │  Commit / Reserve                 │
              │      ↓                            │
              │  outbound order                   │
              │                                   │
              │  Position / Inventory             │
              │  PnL                              │
              │  Limits                           │
              └───────────────────────────────────┘




A more complex architecture would be this :


                         MARKET
      ┌────────────────────┴─────────────────────┐
      │                                          │
 MARKET DATA                                ORDER ENTRY
      │                                          │
      ▼                                          ▼
NIC / RX                                    NIC / TCP RX/TX
      │                                          │
Feed/session handler                       Session handler
      │                                          │
Packet sequencing                         OUCH/FIX/native
Gap detection                             sequence/session
Recovery                                  reconnect/recovery
      │                                          │
Binary decoder                                  │
      │                                          │
Normalizer                                      │
      │                                          │
      ▼                                          │
────────────────────── TRADING CORE ─────────────┤
                                                 │
 Instrument/reference state                     │
        │                                        │
 Order Book                                      │
        │                                        │
 Features / Signals                              │
        │                                        │
 Fair Value / Pricing                            │
        │                                        │
 Strategy                                        │
        │                                        │
 Order Manager  ◄────────────────────────────────┘
        │                ACK/FILL/CANCEL/REJECT
        │
 Pre-trade Risk
        │
 Position / Inventory
        │
 Order generation
        │
        ▼
 Order Gateway
        │
 Encoder
        │
 Throttle / venue rules
        │
 Session
        │
        ▼
      VENUE





Trading Architecture with threads :

TRADING SIDE

CPU 0
MarketDataReceiver / UDP RX
    ↓
decode market data
    ↓
ring

CPU 1
Order Feedback RX / TCP
    ↓
ACK / REJECT / FILL
    ↓
ring

CPU 2
FeedHandler
    ↓
Local OrderBook
    ↓
Strategy
    ↓
OrderIntent
    ↓
ring

CPU 3
OMS
    ↓
Risk
    ↓
OrderGateway TX
    ↓
TCP send()





Exchange Architecture with threads :
UDP market data
    ↓
CPU 0
    ↓ ring
CPU 2
FeedHandler → Book → Strategy
    ↓ ring
CPU 3
OMS → Risk → Gateway
    ↓ TCP
EXCHANGE