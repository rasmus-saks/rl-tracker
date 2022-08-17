#pragma once
#define WIN32_LEAN_AND_MEAN 
#include <asio.hpp>
#include "spdlog/spdlog.h"

using asio::ip::tcp;

class tcp_connection
    : public std::enable_shared_from_this<tcp_connection>
{
public:
    typedef std::shared_ptr<tcp_connection> pointer;
    typedef std::function<void(pointer connection)> closeCallback;

    static pointer create(asio::io_context& io_context, closeCallback connectionCloseCallback);
    tcp::socket& socket();
    void start();
    void send_message(asio::mutable_buffer message);
    void send_message(const std::string* message);
private:
    char read_buffer[1];
    tcp::socket socket_;
    closeCallback connectionCloseCallback;
    tcp_connection(asio::io_context& io_context, closeCallback connectionCloseCallback);
    void handle_write(const asio::error_code&, size_t);
    void handle_read(const asio::error_code&, size_t);
    void start_reading();
    std::shared_ptr<spdlog::logger> logger;
};

