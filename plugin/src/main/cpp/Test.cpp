#include "Test.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#define with(T, ...)\
    ([&]{ T ${}; __VA_ARGS__; return $; }())

int main() {
	std::vector<spdlog::sink_ptr> sinks;
	sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	auto log = std::make_shared<spdlog::logger>("default", begin(sinks), end(sinks));
	spdlog::register_logger(log);
	auto logger = spdlog::get("default");
	logger->info("Hello world!");
	Rank rank = with(Rank,
		$.playlist = PlaylistType::RANKEDDUEL,
		$.tier = 1,
		$.division = 4,
		$.matches_played = 420,
		$.mmr = 123.45
		);
	json j = rank;
	logger->info(j.dump());

	asio::io_context io_context;
	tcp_server server(io_context, 8081);
	io_context.run();


	logger->info("Goodbye.");
	return -1;
}
