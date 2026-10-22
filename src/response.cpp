#include "response.hpp"

namespace {

const char* reason_phrase(int status) {
    switch (status) {
    case 200:
        return "OK";
    case 404:
        return "Not Found";
    case 500:
        return "Internal Server Error";
    default:
        return "Internal Server Error";
    }
}

}  // namespace

std::string serialize(const Response& response) {
    auto headers = response.headers;
    if (headers.find("Content-Type") == headers.end()) {
        headers["Content-Type"] = "text/plain";
    }
    headers["Content-Length"] = std::to_string(response.body.size());
    headers["Connection"] = "close";

    std::string out = "HTTP/1.1 " + std::to_string(response.status) + " " +
                      reason_phrase(response.status) + "\r\n";
    for (const auto& header : headers) {
        out += header.first;
        out += ": ";
        out += header.second;
        out += "\r\n";
    }
    out += "\r\n";
    out += response.body;
    return out;
}
