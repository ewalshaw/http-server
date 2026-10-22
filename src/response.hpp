#pragma once

#include <string>
#include <unordered_map>

struct Response {
    int status = 200;
    std::string body;
    std::unordered_map<std::string, std::string> headers;
};

std::string serialize(const Response& response);
