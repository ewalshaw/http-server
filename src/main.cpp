#include "request.hpp"
#include "response.hpp"
#include "router.hpp"
#include "server.hpp"

#include <chrono>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void handle_client(Client& client, const Router& router) {
    const std::string raw = client.read_headers();
    if (raw.empty()) {
        return;
    }

    Request request = parse_request(raw);
    if (request.method == "POST") {
        const auto it = request.headers.find("content-length");
        if (it != request.headers.end()) {
            const auto length = static_cast<std::size_t>(std::stoul(it->second));
            if (request.body.size() > length) {
                request.body.resize(length);
            } else if (request.body.size() < length) {
                request.body += client.read_exact(length - request.body.size());
            }
        }
    }

    client.send_all(serialize(router.route(request)));
}

}  // namespace

int main() {
    constexpr int port = 8080;
    const auto started = std::chrono::steady_clock::now();

    Router router;
    router.get("/", [](const Request&) {
        Response response;
        response.body = "Hello from C++ HTTP server\n";
        return response;
    });
    router.get("/hello-world", [](const Request&) {
        Response response;
        response.body =
            " _   _      _ _        __        __         _     _ _ \n"
            "| | | | ___| | | ___   \\ \\      / /__  _ __| | __| | |\n"
            "| |_| |/ _ \\ | |/ _ \\   \\ \\ /\\ / / _ \\| '__| |/ _` | |\n"
            "|  _  |  __/ | | (_) |   \\ V  V / (_) | |  | | (_| |_|\n"
            "|_| |_|\\___|_|_|\\___/     \\_/\\_/ \\___/|_|  |_|\\__,_(_)\n";
        return response;
    });
    router.get("/health", [started, port](const Request&) {
        const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(
                                 std::chrono::steady_clock::now() - started)
                                 .count();
        Response response;
        response.body = "status: ok\nport: " + std::to_string(port) +
                        "\nuptime_seconds: " + std::to_string(seconds) + "\n";
        return response;
    });
    router.get("/time", [](const Request&) {
        const std::time_t now = std::time(nullptr);
        const char* text = std::ctime(&now);
        if (text == nullptr) {
            throw std::runtime_error("time failed");
        }
        Response response;
        response.body = text;
        return response;
    });

    try {
        Server server(port);
        std::cerr << "listening on port " << port << "\n";
        for (;;) {
            Client client = server.accept_client();
            try {
                handle_client(client, router);
            } catch (const std::exception& ex) {
                std::cerr << ex.what() << '\n';
                Response response;
                response.status = 500;
                response.body = "Internal Server Error\n";
                try {
                    client.send_all(serialize(response));
                } catch (const std::exception&) {
                }
            }
        }
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }
    return 0;
}
