#pragma once

#include "app/IServer.hpp"

#include <memory>

/// Strategy that builds the fully-wired server for one specific API version.
///
/// One implementation per version (::ServerBuilder, v2::ServerBuilder). The
/// version-agnostic ServerFactory selects the right strategy at runtime.
struct IServerBuilder {
    virtual ~IServerBuilder() = default;

    /// Builds and returns the server. Caller owns the result.
    virtual std::unique_ptr<IServer> build() = 0;
};
