#include <string>

namespace mini_rpc {
    class client {
    public:
        explicit client(std::string _endpoint);

        template<typename... Args>
        void call(std::string method, Args... args) {
            
        }

        void debug_connect();
        constexpr void close();

    protected:
        std::string endpoint;
    };
}