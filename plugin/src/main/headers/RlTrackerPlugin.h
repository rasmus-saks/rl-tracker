#pragma once
#define WIN32_LEAN_AND_MEAN
//#define _WIN32_WINNT 0x0601
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include <asio.hpp>
#include <locale>
#include <codecvt>
#include <string>
#include <list>
#include <set>
#include "tcp_server.h"
#include "spdlog/spdlog.h"
#include "json.hpp"
#include "data_types.h"


using json = nlohmann::json;


class RlTrackerPlugin : public BakkesMod::Plugin::BakkesModPlugin
{
public:
	virtual void onLoad();
	virtual void onUnload();
	void countdownBegin(const std::string& eventName);
	void matchEnded(const std::string& eventName);
	void startRound(const std::string& eventName);
	void playerScored(const std::string& eventName);
	void testEvent(const std::string& eventName);
	void periodicStats(GameWrapper* gameWrapper);
private:
	std::list<Player> getMatchPlayers(ServerWrapper state);
	bool matchActive = false;
	int lastPlaylist = -1;
	std::unique_ptr<MMRNotifierToken> mmrNotifier;
	UniqueIDWrapper currentPlayerID;

	void updateLastPlaylist();
	void mmrCallback(UniqueIDWrapper uniqueId);
	Player getPlayer(PriWrapper priWrapper);
 	void injectPlayerRanks(std::list<Rank>* ranks, UniqueIDWrapper uniqueId);
 	Match buildMatch(ServerWrapper state);
	ServerWrapper getCurrentGameState();
	ServerWrapper getCurrentGameState(GameWrapper* gameWrapper);
	PriWrapper findMe();
	asio::io_context io_context;
	tcp_server* server = nullptr;
	std::shared_ptr<spdlog::logger> logger;
};

