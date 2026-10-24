#include "request.hpp"

#include <cctype>
#include <sstream>
#include <stdexcept>

namespace {

std::string trim(const std::string& text) {
    std::size_t begin = 0;
    while (begin < text.size() &&
           std::isspace(static_cast<unsigned char>(text[begin]))) {
        ++begin;
    }
    std::size_t end = text.size();
    while (end > begin &&
           std::isspace(static_cast<unsigned char>(text[end - 1]))) {
        --end;
    }
    return text.substr(begin, end - begin);
}

std::string lower_copy(std::string text) {
    for (char& ch : text) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return text;
}

std::string strip_cr(std::string line) {
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    return line;
}

}  // namespace

Request parse_request(const std::string& raw) {
    const auto header_end = raw.find("\r\n\r\n");
    if (header_end == std::string::npos) {
        throw std::runtime_error("incomplete request");
    }

    Request request;
    request.body = raw.substr(header_end + 4);

    std::istringstream lines(raw.substr(0, header_end));
    std::string line;
    if (!std::getline(lines, line)) {
        throw std::runtime_error("empty request");
    }

    std::istringstream request_line(trim(strip_cr(line)));
    if (!(request_line >> request.method >> request.path >> request.version)) {
        throw std::runtime_error("bad request line");
    }

    const auto query_pos = request.path.find('?');
    if (query_pos != std::string::npos) {
        request.query = request.path.substr(query_pos + 1);
        request.path.resize(query_pos);
    }

    while (std::getline(lines, line)) {
        line = trim(strip_cr(line));
        if (line.empty()) {
            continue;
        }
        const auto colon = line.find(':');
        if (colon == std::string::npos) {
            continue;
        }
        request.headers[lower_copy(trim(line.substr(0, colon)))] =
            trim(line.substr(colon + 1));
    }

    return request;
}
