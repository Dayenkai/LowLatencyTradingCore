cmake_minimum_required(VERSION 3.22)

project(LOW_LATENCY_SYSTEM_LAB)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(trading_engine trading_engine/src/main.cc trading_engine/src/EnvStartup/envStartup.cc trading_engine/src/NIC/nicReplay.cc trading_engine/src/PersonnalLibrary/OutputFunctions.cc trading_engine/src/FeedHandler/feedHandler.cc trading_engine/src/Utils/utils.cpp)
add_executable(exchange_engine exchange_engine/src/main.cc exchange_engine/src/Exchange.cc)
add_executable(benchmark_tool benchmark/src/main.cc)

add_library(orderbook trading_engine/src/OrderBook/orderBook.cc)

target_include_directories(orderbook PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/trading_engine/includes)
target_include_directories(trading_engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/trading_engine/includes)
target_include_directories(exchange_engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/exchange_engine/includes PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/trading_engine/includes)
target_include_directories(benchmark_tool PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/includes)

target_link_libraries(trading_engine PRIVATE orderbook)
target_link_libraries(exchange_engine PRIVATE orderbook)

find_package(nlohmann_json QUIET)
if(nlohmann_json_FOUND)
    target_link_libraries(exchange_engine PRIVATE nlohmann_json::nlohmann_json)
else()
    message(STATUS "nlohmann_json not installed. Downloading and installing it, Please Wait...('dpkg -l | grep nlohmann' to ensure it has been installed afterward)")
    include(FetchContent)
    FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    )
    FetchContent_MakeAvailable(nlohmann_json)
    target_link_libraries(exchange_engine PRIVATE nlohmann_json::nlohmann_json)
endif()



target_compile_features(trading_engine PRIVATE cxx_std_23)

target_compile_options(trading_engine PRIVATE
        -Wall
        -Wextra
        -Wpedantic)