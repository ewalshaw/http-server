#include "server.hpp"

#include <windows.h>

#include <cstdint>
#include <stdexcept>
#include <string>

namespace {

[[noreturn]] void fail(const char* what) {
    const int code = WSAGetLastError();
    char* message = nullptr;
    const DWORD length = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, static_cast<DWORD>(code), 0, reinterpret_cast<LPSTR>(&message),
        0, nullptr);

    std::string text = std::string(what) + ": ";
    if (length != 0 && message != nullptr) {
        text.append(message, length);
        LocalFree(message);
        while (!text.empty() &&
               (text.back() == '\r' || text.back() == '\n' || text.back() == ' ')) {
            text.pop_back();
        }
    } else {
        text += std::to_string(code);
    }
    throw std::runtime_error(text);
}

}  // namespace

Client::Client(SOCKET fd) : fd_(fd) {}

Client::~Client() {
    if (fd_ != INVALID_SOCKET) {
        ::closesocket(fd_);
    }
}

Client::Client(Client&& other) noexcept : fd_(other.fd_) {
    other.fd_ = INVALID_SOCKET;
}

std::string Client::read_headers() {
    std::string buffer;
    char tmp[4096];
    while (buffer.find("\r\n\r\n") == std::string::npos) {
        const int n = ::recv(fd_, tmp, static_cast<int>(sizeof(tmp)), 0);
        if (n == SOCKET_ERROR) {
            if (WSAGetLastError() == WSAEINTR) {
                continue;
            }
            fail("recv");
        }
        if (n == 0) {
            break;
        }
        buffer.append(tmp, static_cast<std::size_t>(n));
    }
    return buffer;
}

std::string Client::read_exact(std::size_t count) {
    std::string out;
    out.reserve(count);
    while (out.size() < count) {
        char tmp[4096];
        std::size_t want = count - out.size();
        if (want > sizeof(tmp)) {
            want = sizeof(tmp);
        }
        const int n = ::recv(fd_, tmp, static_cast<int>(want), 0);
        if (n == SOCKET_ERROR) {
            if (WSAGetLastError() == WSAEINTR) {
                continue;
            }
            fail("recv");
        }
        if (n == 0) {
            break;
        }
        out.append(tmp, static_cast<std::size_t>(n));
    }
    return out;
}

void Client::send_all(const std::string& data) {
    std::size_t sent = 0;
    while (sent < data.size()) {
        const int n = ::send(fd_, data.data() + sent,
                             static_cast<int>(data.size() - sent), 0);
        if (n == SOCKET_ERROR) {
            if (WSAGetLastError() == WSAEINTR) {
                continue;
            }
            fail("send");
        }
        if (n == 0) {
            throw std::runtime_error("send: connection closed");
        }
        sent += static_cast<std::size_t>(n);
    }
}

Server::Server(int port) : listen_fd_(INVALID_SOCKET), wsa_started_(false) {
    WSADATA data{};
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        fail("WSAStartup");
    }
    wsa_started_ = true;

    listen_fd_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_fd_ == INVALID_SOCKET) {
        fail("socket");
    }

    const char reuse = 1;
    if (::setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) ==
        SOCKET_ERROR) {
        fail("setsockopt");
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(static_cast<std::uint16_t>(port));

    if (::bind(listen_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) ==
        SOCKET_ERROR) {
        fail("bind");
    }
    if (::listen(listen_fd_, 10) == SOCKET_ERROR) {
        fail("listen");
    }
}

Server::~Server() {
    if (listen_fd_ != INVALID_SOCKET) {
        ::closesocket(listen_fd_);
    }
    if (wsa_started_) {
        WSACleanup();
    }
}

Client Server::accept_client() {
    for (;;) {
        const SOCKET fd = ::accept(listen_fd_, nullptr, nullptr);
        if (fd != INVALID_SOCKET) {
            return Client(fd);
        }
        if (WSAGetLastError() != WSAEINTR) {
            fail("accept");
        }
    }
}
