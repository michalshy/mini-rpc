#include "worker_pool.h"
#include "frame/frame.h"
#include "rpc_server.h"
#include "transport.h"
#include <memory>
#include <mutex>
#include <thread>

namespace mini_rpc {

WorkerPool::WorkerPool(RpcServer& _rpc) : rpc(_rpc) {
    for (int i = 0; i < POOL_SIZE; i++) {
        workers.emplace_back(&WorkerPool::worker_loop, this);
    }
}

WorkerPool::~WorkerPool() {
    for (auto& worker : workers) {
        worker.join();
    }
}

void WorkerPool::push_connection(std::unique_ptr<ITransport> _con) {
    std::lock_guard<std::mutex> guard(lock);
    connections.push(std::move(_con));
    cvar.notify_one();
}

void WorkerPool::close() {
    stopped = true;
    cvar.notify_all();
}

void WorkerPool::worker_loop() {
    while (true) {
        std::unique_lock<std::mutex> guard(lock);
        cvar.wait(guard, [this]() { return !connections.empty() || stopped; });
        if (stopped)
            break;

        std::unique_ptr<ITransport> conn = std::move(connections.front());
        connections.pop();

        Framer framer(std::move(conn));

        while (true) {
            auto msg = framer.recv_message();
            if (!msg) {
                break; // client disconnected cleanly
            }

            buffer response = rpc.handle_message(*msg);
            framer.send_message(response);
        }
    }
}

} // namespace mini_rpc