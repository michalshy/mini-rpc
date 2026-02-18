#pragma once

#include <string>
#include <string_view>
#include <utility>

#ifdef MINI_RPC_WIN
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

namespace mini_rpc {

// Parse IPv4 endpoint in format "host:port"
// Returns pair of (host, port)
inline std::pair<std::string, std::string> parse_ipv4_endpoint(std::string_view endpoint) {
    auto colon = endpoint.rfind(':');
    std::string host(endpoint.substr(0, colon));
    std::string port(endpoint.substr(colon + 1));
    return {host, port};
}

#ifdef MINI_RPC_WIN
// Helper to create sockaddr_in from host:port
inline sockaddr_in create_sockaddr_in(std::string_view endpoint) {
    auto [host, port] = parse_ipv4_endpoint(endpoint);
    
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<u_short>(std::stoi(port)));
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);
    
    return addr;
}
#endif

} // namespace mini_rpc
