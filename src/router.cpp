#include "router.hpp"

#include <iostream>
#include <utility>

void Router::get(const std::string& path, Handler handler) {
    get_routes[path] = std::move(handler);
}

Response Router::route(const Request& request) const {
    if (request.method == "GET") {
        const auto it = get_routes.find(request.path);
        if (it != get_routes.end()) {
            try {
                return it->second(request);
            } catch (const std::exception& ex) {
                std::cerr << ex.what() << '\n';
            } catch (...) {
                std::cerr << "handler failed\n";
            }
            Response response;
            response.status = 500;
            response.body = "Internal Server Error\n";
            return response;
        }
    }

    Response response;
    response.status = 404;
    response.body = "Not Found\n";
    return response;
}
