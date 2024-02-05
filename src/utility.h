#pragma once

#include <format>
#include <iostream>

template<typename... Args>
inline void println(const std::format_string<Args...> fmt, Args&&... args) {
    std::cout << std::vformat(fmt.get(), std::make_format_args(args...)) << "\n";
}

template<typename... Args>
inline void print(const std::format_string<Args...> fmt, Args&&... args) {
    std::cout << std::vformat(fmt.get(), std::make_format_args(args...));
}
