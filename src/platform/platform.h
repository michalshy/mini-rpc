#pragma once

#include <mutex>

#ifdef MINI_RPC_WIN
#include <WinSock2.h>
#endif

namespace mini_rpc {

// Platform specific inits - currently only win
inline void platform_init() {
    static std::once_flag flag;
    std::call_once(flag, [] {
#ifdef MINI_RPC_WIN
        WSADATA wsa;
        WSAStartup(MAKEWORD(2, 2), &wsa);
        std::atexit([] { WSACleanup(); });
#endif
    });
}
} // namespace mini_rpc