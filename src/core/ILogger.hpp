#pragma once

#include <string_view>

/// Sink for server diagnostics.
///
/// Thread-safety: REQUIRED — called from every io_context worker thread.
struct ILogger {
    virtual ~ILogger() = default;

    /// Routine events: connections, ownership changes.
    virtual void info(std::string_view msg) = 0;

    /// Dropped or suspicious input that the server survives.
    virtual void warn(std::string_view msg) = 0;

    /// Failures of the server itself.
    virtual void error(std::string_view msg) = 0;
};
