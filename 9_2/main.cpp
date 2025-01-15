#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <memory>
#include <mutex>

using boost::asio::ip::tcp;

class DrawingServer {
public:
    DrawingServer(boost::asio::io_context& io_context, short port)
            : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        startAccept();
    }

private:
    void startAccept() {
        // Передаем executor из acceptor, который уже связан с io_context
        auto socket = std::make_shared<tcp::socket>(acceptor_.get_executor());

        acceptor_.async_accept(*socket, [this, socket](boost::system::error_code ec) {
            if (!ec) {
                std::lock_guard<std::mutex> lock(mutex_);
                clients_.push_back(socket);
                startRead(socket);
            }
            startAccept();
        });
    }

    void startRead(const std::shared_ptr<tcp::socket>& socket) {
        auto buffer = std::make_shared<std::vector<char>>(1024);
        socket->async_read_some(boost::asio::buffer(*buffer), [this, socket, buffer](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                broadcastMessage(std::string(buffer->data(), length));
                startRead(socket);
            } else {
                std::lock_guard<std::mutex> lock(mutex_);
                clients_.erase(std::remove(clients_.begin(), clients_.end(), socket), clients_.end());
            }
        });
    }

    void broadcastMessage(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& client : clients_) {
            boost::asio::async_write(*client, boost::asio::buffer(message), [](boost::system::error_code, std::size_t) {});
        }
    }

    tcp::acceptor acceptor_;
    std::vector<std::shared_ptr<tcp::socket>> clients_;
    std::mutex mutex_;
};
