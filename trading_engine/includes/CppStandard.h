#ifndef CPP_STANDARD_H
#define CPP_STANDARD_H

//INPUT-OUTPUT
#include <iostream>
#include <fstream>
#include <sstream>

//MEMORY MANAGMENT
#include <memory>

//STRING MANIPULATION
#include <cstring>
#include <cctype>

//BIT MANIPULATION
#include <bit>

//THREAD
#include <thread>
#include <stop_token>

//CONTAINERS
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <forward_list>
#include <utility>

//TIME
#include <chrono>
using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;

//OPTIONAL
#include <optional>

//EXPECTED
#include <expected>

//VARIANT
#include <variant>

//TYPE_TRAITS
#include <type_traits>

#endif