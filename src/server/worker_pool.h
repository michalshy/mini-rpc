#pragma once

#include "rpc_server.h"
#include "transport.h"
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace mini_rpc {

constexpr uint8_t POOL_SIZE = 8;

class WorkerPool {
public:
    explicit WorkerPool(RpcServer& _rpc);
    ~WorkerPool();

    void push_connection(std::unique_ptr<ITransport> _conn);
    void close();

protected:
    RpcServer& rpc;

    std::queue<std::unique_ptr<ITransport>> connections;
    // access
    std::mutex lock;
    std::condition_variable cvar;

    std::vector<std::thread> workers;

    std::atomic_bool stopped;

    void worker_loop();
};

} // namespace mini_rpc