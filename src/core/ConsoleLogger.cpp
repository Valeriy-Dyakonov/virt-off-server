#include "core/ConsoleLogger.hpp"

#include <iostream>

void ConsoleLogger::info(const std::string_view msg) {
    const std::scoped_lock lock(mutex_);
    std::cout << "[info] " << msg << std::endl;
}

void ConsoleLogger::warn(const std::string_view msg) {
    const std::scoped_lock lock(mutex_);
    std::cout << "[warn] " << msg << std::endl;
}

void ConsoleLogger::error(const std::string_view msg) {
    const std::scoped_lock lock(mutex_);
    std::cerr << "[error] " << msg << std::endl;
}
