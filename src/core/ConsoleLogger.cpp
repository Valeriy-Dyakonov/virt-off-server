#include "core/ConsoleLogger.hpp"

#include <iostream>

void ConsoleLogger::info(std::string_view msg) {
    const std::scoped_lock lock(mutex_);
    std::cout << "[info] " << msg << std::endl;
}

void ConsoleLogger::warn(std::string_view msg) {
    const std::scoped_lock lock(mutex_);
    std::cout << "[warn] " << msg << std::endl;
}

void ConsoleLogger::error(std::string_view msg) {
    const std::scoped_lock lock(mutex_);
    std::cerr << "[error] " << msg << std::endl;
}
