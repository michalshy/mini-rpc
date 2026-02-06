#include <string>

namespace mini_rpc {
    class client {
    public:
        explicit client(std::string _endpoint);

        void debug_connect();

        constexpr void close();

    protected:
        std::string endpoint;
    };
}