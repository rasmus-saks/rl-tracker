#pragma once
#define WIN32_LEAN_AND_MEAN
#include "tcp_connection.h"
#include <asio.hpp>
#include "spdlog/spdlog.h"
#include <list>
#include "json.hpp"
#include "data_types.h"

using json = nlohmann::json;

class tcp_server
{
public:
	tcp_server(asio::io_context& io_context,
		unsigned short port);
	void broadcast_message(const std::string& message);


	void broadcast_event(const std::string* event)
	{
 		for (const auto& connection : connections)
 		{
 			connection->send_message(event);
 		}
	}

	void broadcast_json(json j)
	{
		broadcast_event(&j.dump());
	}

private:
	void start_accept();
	void handle_accept(const tcp_connection::pointer& new_connection, const asio::error_code& error);

	asio::io_context& io_context_;
	tcp::acceptor acceptor_;
	std::list<tcp_connection::pointer> connections;
	std::shared_ptr<spdlog::logger> logger;
	void connection_closed(const tcp_connection::pointer& connection);
};
