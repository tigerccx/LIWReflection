#pragma once
#include <chrono>

#define TICK(name) \
auto __##name##_name = #name; \
auto __##name##_start = std::chrono::system_clock::now();

#define TOCK(name) \
auto __##name##_end = std::chrono::system_clock::now(); \
std::cout << #name << " " << double((std::chrono::duration_cast<std::chrono::microseconds>(__##name##_end - __##name##_start)).count()) << "us" << std::endl; 