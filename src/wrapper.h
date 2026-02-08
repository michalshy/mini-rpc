#pragma once

#include <tuple>
#include <type_traits>
namespace mini_rpc {

template<typename F>
struct function_traits;

template<typename R, typename C, typename... Args>
struct function_traits<R (C::*)(Args...) const> {
    using return_type = R;
    using args_tuple = std::tuple<std::decay_t<Args>...>;
};

template<typename F>
struct function_traits : function_traits<decltype(&F::operator())> {};

template<typename R, typename... Args>
struct function_traits<R (*)(Args...)> {
    using return_type = R;
    using args_tuple = std::tuple<std::decay_t<Args>...>;
};
} // namespace mini_rpc