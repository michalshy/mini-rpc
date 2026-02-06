#include <string>

namespace mini_rpc {

    class server {
    public:
        explicit server(std::string _endpoint);

        constexpr void register_handler(std::string method);
    
        void run(); // blocking
        constexpr void stop(); // async safe  
    protected:
        std::string endpoint;
    };
}