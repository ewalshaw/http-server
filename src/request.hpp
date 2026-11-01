#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

struct Request {
    std::string method;
    std::string path;
    std::string query;
    std::vector<std::pair<std::string, std::string>> query_params;
    std::string version;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};

Request parse_request(const std::string& raw);
