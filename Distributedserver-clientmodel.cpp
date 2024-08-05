// Simplified distributed key-value store (client-server model)
#include <iostream>
#include <unordered_map>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class KeyValueStore {
public:
    void put(const std::string& key, const std::string& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        store_[key] = value;
    }

    std::string get(const std::string& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        return store_[key];
    }

private:
    std::unordered_map<std::string, std::string> store_;
    std::mutex mutex_;
};

class Server {
public:
    Server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::thread(&Server::handle_client, this, std::move(socket)).detach();
                }
                do_accept();
            });
    }

    void handle_client(tcp::socket socket) {
        try {
            char data[1024];
            boost::system::error_code error;
            while (true) {
                size_t length = socket.read_some(boost::asio::buffer(data), error);
                if (error == boost::asio::error::eof) break;
                else if (error) throw boost::system::system_error(error);

                std::string request(data, length);
                std::string response = handle_request(request);
                boost::asio::write(socket, boost::asio::buffer(response), error);
            }
        } catch (std::exception& e) {
            std::cerr << "Exception: " << e.what() << "\n";
        }
    }

    std::string handle_request(const std::string& request) {
        std::istringstream iss(request);
        std::string command, key, value;
        iss >> command >> key;

        if (command == "PUT") {
            iss >> value;
            store_.put(key, value);
            return "OK\n";
        } else if (command == "GET") {
            return store_.get(key) + "\n";
        }
        return "ERROR\n";
    }

    tcp::acceptor acceptor_;
    KeyValueStore store_;
};

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: server <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;
        Server server(io_context, std::atoi(argv[1]));
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
