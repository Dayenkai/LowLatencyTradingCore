cmake_minimum_required(VERSION 3.22)

project(LOW_LATENCY_SYSTEM_LAB)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(trading_engine trading_engine/src/main.cc trading_engine/src/EnvStartup/envStartup.cc trading_engine/src/NIC/nicReplay.cc trading_engine/src/PersonnalLibrary/OutputFunctions.cc trading_engine/src/FeedHandler/feedHandler.cc trading_engine/src/Utils/utils.cpp trading_engine/includes//Constants.h trading_engine/includes//CppStandard.h trading_engine/includes//Engine.h trading_engine/includes//FeedHandler.h trading_engine/includes//MatchingEngine.h trading_engine/includes//MemoryPool.h trading_engine/includes//NicReplay.h trading_engine/includes//OrderBook.h trading_engine/includes//Parser.h trading_engine/includes//PersonnalLibrary.h trading_engine/includes//Utils.h)

target_compile_features(trading_engine PRIVATE cxx_std_23)

target_compile_options(trading_engine PRIVATE
        -Wall
        -Wextra
        -Wpedantic)