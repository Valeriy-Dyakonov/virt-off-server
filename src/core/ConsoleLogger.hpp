#pragma once

#include "core/ILogger.hpp"

#include <mutex>

/// ILogger writing "[info]"/"[warn]" lines to stdout and "[error]" lines to stderr.
///
/// Every line is flushed at once so the log survives a crash and reaches
/// Docker / journald without delay.
///
/// Thread-safety: yes — a mutex serialises the output.
class ConsoleLogger final : public ILogger {
public:
    void info(std::string_view msg) override;

    void warn(std::string_view msg) override;

    void error(std::string_view msg) override;

private:
    std::mutex mutex_;
};
