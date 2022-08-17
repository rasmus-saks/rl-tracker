#include "tcp_connection.h"


tcp_connection::pointer tcp_connection::create(
	asio::io_context& io_context, closeCallback connectionCloseCallback)
{
	return pointer(new tcp_connection(io_context, connectionCloseCallback));
}

tcp::socket& tcp_connection::socket()
{
	return socket_;
}

void tcp_connection::start()
{
	start_reading();
}


void tcp_connection::send_message(asio::mutable_buffer buf)
{
	asio::write(
		socket_,
		buf
	);
}

void tcp_connection::send_message(const std::string* message)
{
 	size_t size = message->length();
 	logger->info("Message size {}", size);
 	char* header = new char[sizeof(size)];
 	memcpy(header, (char*)&size, sizeof(size));
 
 	send_message(asio::buffer(header, sizeof(size)));
 
 	char* data = new char[size];
	strcpy(data, message->c_str());
 
 	send_message(asio::buffer(data, size));
 
 	delete[] data;
}

void tcp_connection::start_reading() {
	asio::async_read(
		socket_,
		asio::buffer(read_buffer, 1),
		std::bind(&tcp_connection::handle_read, shared_from_this(),
			std::placeholders::_1, std::placeholders::_2)
	);
}

tcp_connection::tcp_connection(asio::io_context& io_context, closeCallback connectionCloseCallback)
	: socket_(io_context), read_buffer{ '\0' }, connectionCloseCallback(connectionCloseCallback)
{
	logger = spdlog::get("default");
}

void tcp_connection::handle_write(
	const asio::error_code&,
	size_t)
{
}

void tcp_connection::handle_read(
	const asio::error_code& error,
	size_t)
{
	if (asio::error::eof != error && asio::error::connection_reset != error)
	{
		start_reading();
	}
	else
	{
		logger->info("Closing connection");
		socket().close();
		connectionCloseCallback(shared_from_this());
	}
}
