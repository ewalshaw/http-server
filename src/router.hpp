#pragma once

#include "request.hpp"
#include "response.hpp"

#include <functional>
#include <string>
#include <unordered_map>

using Handler = std::function<Response(const Request&)>;

class Router {
public:
    void get(const std::string& path, Handler handler);
    Response route(const Request& request) const;

private:
    std::unordered_map<std::string, Handler> get_routes;
};
