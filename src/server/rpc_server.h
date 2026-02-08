#pragma once

#include "coder.h"
#include "transport_server.h"
#include "wrapper.h"

#include <functional>
#include <map>
#include <tuple>
#include <type_traits>

namespace mini_rpc {

using Result = uint8_t;

struct Handler {
    std::function<void(const buffer&, buffer&)> invoke;
};

// temporary returning void always, will change to some result
template<class F>
auto wrap_function(F&& func) {
    return Handler{// lets wrap this in lambda which conveniently returns nothing
                   [f = std::forward<F>(func)](const buffer& req, buffer& res) {
                       using traits = function_traits<std::decay_t<decltype(f)>>;
                       using ArgsTuple = typename traits::args_tuple;

                       int offset = 0;

                       ArgsTuple args = decode_args<ArgsTuple>(req.data(), offset);

                       if constexpr (std::is_void_v<typename traits::return_type>) {
                           std::apply(f, args);
                       } else {
                           auto result = std::apply(f, args);
                           encode_arg(res, result);
                       }
                   }};
}

class RpcServer {
public:
    RpcServer() = default;

    template<typename Func>
    void register_handler(std::string method, Func&& func) {
        handles.emplace(std::move(method), wrap_function(std::forward<Func>(func)));
    }

    Result handle_message(const buffer& buf);

protected:
    std::map<std::string, Handler> handles;
};
} // namespace mini_rpc