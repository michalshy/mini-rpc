#pragma once

#include "coder.h"
#include "result.h"
#include "transport_server.h"
#include "wrapper.h"

#include <functional>
#include <map>
#include <tuple>
#include <type_traits>

namespace mini_rpc {

struct Handler {
    std::function<buffer(const buffer&)> invoke;
};

// temporary returning void always, will change to some result
template<class F>
auto wrap_function(F&& func) {
    return Handler{// lets wrap this in lambda which conveniently returns nothing
                   [fun = std::forward<F>(func)](const buffer& req) -> buffer {
                       using traits = function_traits<std::decay_t<decltype(fun)>>;
                       using ArgsTuple = typename traits::args_tuple;

                       int offset = 0;
                       ArgsTuple args = decode_args<ArgsTuple>(req.data(), offset);

                       buffer res;

                       if constexpr (std::is_void_v<typename traits::return_type>) {
                           std::apply(fun, args);
                       } else {
                           auto result = std::apply(fun, args);
                           encode_arg(res, result);
                       }
                       return res;
                   }};
}

class RpcServer {
public:
    RpcServer() = default;

    template<typename Func>
    void register_handler(std::string method, Func&& func) {
        handles.emplace(std::move(method), wrap_function(std::forward<Func>(func)));
    }

    buffer handle_message(const buffer& message);

protected:
    std::map<std::string, Handler> handles;
};
} // namespace mini_rpc