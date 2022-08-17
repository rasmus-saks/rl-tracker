#include "tcp_server.h"
using namespace std;

tcp_server::tcp_server(asio::io_context& io_context,
	unsigned short port)
	: io_context_(io_context),
	  acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
	  connections(std::list<tcp_connection::pointer>())
{
	logger = spdlog::get("default");
	start_accept();
}

void tcp_server::broadcast_message(const std::string& message)
{
	for (const auto& connection : connections)
	{
		connection->send_message(&message);
	}
}

void tcp_server::start_accept()
{
	tcp_connection::pointer new_connection =
		tcp_connection::create(io_context_, std::bind(&tcp_server::connection_closed, this, std::placeholders::_1));
	acceptor_.async_accept(
		new_connection->socket(),
		std::bind(&tcp_server::handle_accept, this, new_connection, std::placeholders::_1));
}

void tcp_server::connection_closed(const tcp_connection::pointer& connection)
{
	connections.remove(connection);
	logger->info("Connection is closed");
}

void tcp_server::handle_accept(const tcp_connection::pointer& new_connection, const asio::error_code& error)
{
	if (!error)
	{
		logger->info("Received new connection");
		new_connection->start();
		connections.push_back(new_connection);
		TestEvent event(u8"Ä");
 		broadcast_json(event);
	}
	else
	{
		logger->error("handle_accept had an error: {}", error.message());
	}
	start_accept();
}
