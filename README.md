# LowLatencyTradingCore
Low-latency market data processing &amp; trading pipeline

Trading firms are important actors in financial markets, as their roles drastically improve the flow of market trades(Lower transaction costs, Higher Liquidity ect)
In that context, their performance stimulate a lot the exchanges. And one of type of trading actor, being the embodiment of the cross cutting-edge technology between IT and finance are HFT Firms.

In order to have an insight of what are the stakes that wraps HFT and their role, I constructed a personnal Market Data Ingestion with low-latency requirements, to simulate and understand the technological mechanisms, and the impacts they yield in Finance. It will be composed of a classical pipeline, from :
Data Receival -> Parsing -> Order Book Updates -> Strategy -> Execution.

It encompasses memory managment and principle reliying on modern CPU architecture :
- Data Layout
- Cache Efficiency
- Multithreading, Queues, Data Communication
- Correctness, Determinism
- Throughput

With the concrete formalization of results trhough :
- Benchmarking (P99, P50, Worst Cases. Real World Case Comparison)
_All of these cornerstones ensuring the right execution of the pipeline, from the receival to the application_


Documentation is also brought to enlighten with the project directions, such as the Design Choices, Explanations and Commentaries.
Additional Documentary will be added, about the advanced tools that enhances Firms like, attempting to make low-latency difference :
- FGPA
- DPDK
- Machine Learning / IA



