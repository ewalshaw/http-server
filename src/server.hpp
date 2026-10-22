#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>

#include <cstddef>
#include <string>

class Client {
public:
    explicit Client(SOCKET fd);
    ~Client();

    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;
    Client(Client&& other) noexcept;

    std::string read_headers();
    std::string read_exact(std::size_t count);
    void send_all(const std::string& data);

private:
    SOCKET fd_;
};

class Server {
public:
    explicit Server(int port);
    ~Server();

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    Client accept_client();

private:
    SOCKET listen_fd_;
    bool wsa_started_;
};
