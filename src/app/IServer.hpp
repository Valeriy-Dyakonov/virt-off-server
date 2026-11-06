#pragma once

/// A runnable server of one API version.
///
/// Thread-safety: Start and Stop are called from the main thread only.
struct IServer {
    virtual ~IServer() = default;

    /// Binds the socket and registers handlers. Does not block.
    virtual void Start() = 0;

    /// Closes the socket; pending asynchronous operations are cancelled.
    virtual void Stop() noexcept = 0;
};
