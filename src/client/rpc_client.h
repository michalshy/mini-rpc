#pragma once

#include "frame.h"
#include <cstddef>
#include <memory>
#include <vector>
namespace mini_rpc {

    class RpcClient
    {
    public:
        explicit RpcClient(std::string _endpoint)
            : framer(std::make_unique<Framer>(_endpoint))
        {}

        template<typename... Args>
        void send_raw(std::string method, Args&&... args) {
            // message has to be prepared and passed to framer
            std::vector<std::byte> test;
            test.push_back(static_cast<std::byte>(1));
            framer->send_message(test);
        }
    protected:
        std::unique_ptr<Framer> framer;
    };
}