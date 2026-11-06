#include "app/CliParser.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <iostream>
#include <optional>
#include <span>
#include <string_view>
#include <thread>

namespace {

/// Largest valid UDP port number.
constexpr unsigned kMaxPortValue = 65535;

/// Worker-thread count used when hardware concurrency is unknown.
constexpr unsigned kFallbackThreadCount = 2;

/// Parses a positive decimal number; nullopt if `text` is not one.
std::optional<unsigned> parse_positive(const std::string_view text) {
    unsigned value = 0;
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (error != std::errc{} || end != text.data() + text.size() || value == 0) {
        return std::nullopt;
    }
    return value;
}

/// `--port N`: 1..65535.
bool apply_port(const std::string_view value, CliOptions& opts) {
    const auto port = parse_positive(value);
    if (!port.has_value() || *port > kMaxPortValue) {
        return false;
    }
    opts.port = static_cast<uint16_t>(*port);
    return true;
}

/// `--threads N`: N > 0.
bool apply_threads(const std::string_view value, CliOptions& opts) {
    const auto threads = parse_positive(value);
    if (!threads.has_value()) {
        return false;
    }
    opts.threads = *threads;
    return true;
}

/// `--api v1|v2`.
bool apply_api(const std::string_view value, CliOptions& opts) {
    constexpr std::array<std::pair<std::string_view, ApiVersion>, 2> kVersions{{
        {"v1", ApiVersion::V1},
        {"v2", ApiVersion::V2},
    }};
    const auto version = std::ranges::find(kVersions, value, &std::pair<std::string_view, ApiVersion>::first);
    if (version == kVersions.end()) {
        return false;
    }
    opts.api = version->second;
    return true;
}

/// A `--flag value` option: applies a valid value to the options, returns false for an invalid one.
struct Option {
    std::string_view flag;

    bool (*apply)(std::string_view value, CliOptions& opts);
};

constexpr std::array<Option, 3> kOptions{{
    {"--port", apply_port},
    {"--threads", apply_threads},
    {"--api", apply_api},
}};

/// Command-line arguments without the program name (argc may be 0).
std::span<char* const> arguments_of(const int argc, char* argv[]) {
    if (argc <= 1) {
        return {};
    }
    return {argv + 1, static_cast<size_t>(argc - 1)};
}

/// Applies the option at the front of `args` (non-empty). Unknown flags, a flag
/// without a value and invalid values are reported on stderr and skipped.
/// Returns how many arguments were consumed: 2 for `--flag value`, otherwise 1.
size_t apply_next(const std::span<char* const> args, CliOptions& opts) {
    const std::string_view flag = args[0];
    const auto option = std::ranges::find(kOptions, flag, &Option::flag);
    if (option == kOptions.end() || args.size() < 2) {
        std::cerr << "Ignoring unknown argument: " << flag << '\n';
        return 1;
    }

    if (const std::string_view value = args[1]; !option->apply(value, opts)) {
        std::cerr << "Ignoring invalid value of " << flag << ": " << value << '\n';
    }
    return 2;
}

/// Resolves the default worker-thread count from the hardware.
unsigned default_thread_count() {
    const unsigned hw = std::thread::hardware_concurrency();
    return hw == 0 ? kFallbackThreadCount : hw;
}

}  // namespace

CliOptions CliParser::parse(const int argc, char* argv[]) const {
    CliOptions opts{.threads = default_thread_count()};

    for (auto rest = arguments_of(argc, argv); !rest.empty();) {
        rest = rest.subspan(apply_next(rest, opts));
    }
    return opts;
}
