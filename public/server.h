#include <string>
#include <map>
#include <utility>
#include <functional>
#include <vector>

namespace mini_rpc {

    using buffer = std::vector<std::byte>;

    struct Handler
    {
        std::function<void(const buffer&, buffer&)> invoke;
    };
    
    // temporary returning void always, will change to some result
    template<class F>
    auto wrap_function(F&& func) {
        return Handler {
            // lets wrap this in lambda which conveniently returns nothing
            [f = std::forward<F>(func)]
            (const buffer& req, buffer res) 
            {
                // here, serialization must happen but for now
                auto a = req[0];
                auto b = req[1];
                auto result = f(a, b);
                res.push_back(result);
            }
        };
    }

    class Server {
    public:
        explicit Server(std::string _endpoint);
        ~Server();

        template<typename Func>
        void register_handler(std::string method, Func&& func) {
            handles.emplace(
                std::move(method), 
                wrap_function(std::forward<Func>(func))
            );
        }
    
        void run(); // blocking
        constexpr void stop(); // async safe  
    protected:
        std::string endpoint;

        std::map<std::string, Handler> handles;
    };
}