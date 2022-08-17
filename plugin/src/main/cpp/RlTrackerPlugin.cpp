#include "RlTrackerPlugin.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "log/bakkes_sink.h"
#define SERVER_PORT 8081

BAKKESMOD_PLUGIN(RlTrackerPlugin, "RL Tracker", "0.1", PLUGINTYPE_THREADED)

using asio::ip::tcp;

void RlTrackerPlugin::onLoad()
{
	std::vector<spdlog::sink_ptr> sinks;
	sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	sinks.push_back(std::make_shared<bakkes_sink_mt>(cvarManager));

	auto log = std::make_shared<spdlog::logger>("default", begin(sinks), end(sinks));
	spdlog::register_logger(log);
	logger = spdlog::get("default");

	gameWrapper->HookEvent("Function GameEvent_TA.Countdown.BeginState",
		std::bind(&RlTrackerPlugin::countdownBegin, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded",
		std::bind(&RlTrackerPlugin::matchEnded, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.EventPlayerScored",
		std::bind(&RlTrackerPlugin::playerScored, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.OnlineGameJoinGame_TA.OnInit",
		std::bind(&RlTrackerPlugin::testEvent, this, std::placeholders::_1));
	mmrNotifier = gameWrapper->GetMMRWrapper().RegisterMMRNotifier(std::bind(&RlTrackerPlugin::mmrCallback, this, std::placeholders::_1));
	// maybe enable this one day
	//gameWrapper->SetTimeout(std::bind(&RlTrackerPlugin::periodicStats, this, std::placeholders::_1), 2.0f);
	logger->info("Hooked into game events");
	server = new tcp_server(io_context, SERVER_PORT);
	logger->info("Starting TCP server");
	io_context.run();
}

void RlTrackerPlugin::onUnload()
{
	io_context.stop();
	logger->info("Stopped TCP server");
}

void RlTrackerPlugin::countdownBegin(const std::string& eventName)
{
	updateLastPlaylist();
 	auto game = getCurrentGameState();
	CountdownBeginEvent event(buildMatch(game));
	server->broadcast_json(event);
}

void RlTrackerPlugin::periodicStats(GameWrapper* gameWrapper) {
	auto game = getCurrentGameState(gameWrapper);
	if (!game.IsNull()) {
		PeriodicStatsEvent event(buildMatch(game));
		server->broadcast_json(event);
	}
	gameWrapper->SetTimeout(std::bind(&RlTrackerPlugin::periodicStats, this, std::placeholders::_1), 2.0f);
}

void RlTrackerPlugin::mmrCallback(UniqueIDWrapper uniqueId) {
}

void RlTrackerPlugin::matchEnded(const std::string& eventName)
{
	updateLastPlaylist();
	auto game = getCurrentGameState(gameWrapper.get());
	if (!game.IsNull()) {
		auto winningTeam = game.GetWinningTeam();
		if (!winningTeam.IsNull()) {
			MatchEndEvent event(buildMatch(game), winningTeam.GetTeamIndex() == 0 ? Team::BLUE : Team::ORANGE);
			server->broadcast_json(event);
		}
	}

	gameWrapper->SetTimeout([this](GameWrapper* gameWrapper) {
		std::list<Rank> ranks;
		injectPlayerRanks(&ranks, currentPlayerID);
		MmrUpdateEvent event(ranks);
		server->broadcast_json(event);
	}, 6.0f);
}

void RlTrackerPlugin::updateLastPlaylist()
{
	auto game = getCurrentGameState();
	if (!game.IsNull()) {
		lastPlaylist = game.GetPlaylist().GetPlaylistId();
	}
	currentPlayerID = gameWrapper->GetUniqueID();
}

void RlTrackerPlugin::playerScored(const std::string& eventName)
{
	updateLastPlaylist();
	GoalEvent event(buildMatch(getCurrentGameState()));
	server->broadcast_json(event);
}

ServerWrapper RlTrackerPlugin::getCurrentGameState()
{
	return getCurrentGameState(gameWrapper.get());
}


ServerWrapper RlTrackerPlugin::getCurrentGameState(GameWrapper* gameWrapper)
{
	if (gameWrapper->IsInReplay())
	{
		return gameWrapper->GetGameEventAsReplay().memory_address;
	}
	else if (gameWrapper->IsInOnlineGame())
	{
		return gameWrapper->GetOnlineGame();
	}
	else
	{
		return gameWrapper->GetGameEventAsServer();
	}
}

std::list<Player> RlTrackerPlugin::getMatchPlayers(ServerWrapper state)
{
	std::list<Player> players;
	if (!state.IsNull())
	{
		auto members = state.GetPRIs();
		for (int i = 0; i < members.Count(); i++)
		{
			auto member = members.Get(i);
			auto player = getPlayer(member);
			players.push_back(player);
		}
	}
	return players;
}

PriWrapper RlTrackerPlugin::findMe()
{
	auto state = getCurrentGameState();
	return state.GetLocalPrimaryPlayer().GetPRI();
}

Player RlTrackerPlugin::getPlayer(PriWrapper priWrapper)
{
	Player player;
	auto steamid = priWrapper.GetUniqueIdWrapper().GetUID();
	auto mySteamId = gameWrapper->GetSteamID();
	if (!priWrapper.GetTeam().IsNull()) {
		auto team = priWrapper.GetTeam().GetTeamIndex() == 0 ? Team::BLUE : Team::ORANGE;
		player.team = team;
	}
	player.steamid = std::to_string(steamid);
	player.epic_account_id = priWrapper.GetUniqueIdWrapper().GetEpicAccountID();
	player.platform = static_cast<Platform>(priWrapper.GetUniqueIdWrapper().GetPlatform());
	player.goals = priWrapper.GetMatchGoals();
	player.score = priWrapper.GetMatchScore();
	player.assists = priWrapper.GetMatchAssists();
	player.shots = priWrapper.GetMatchShots();
	player.saves = priWrapper.GetMatchSaves();
	player.demos = priWrapper.GetMatchDemolishes();
	player.self = steamid == mySteamId;
	injectPlayerRanks(&player.ranks, priWrapper.GetUniqueIdWrapper());

	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::string name = converter.to_bytes(priWrapper.GetPlayerName().ToWideString());
	player.name = name;

	return player;
}

void RlTrackerPlugin::injectPlayerRanks(std::list<Rank>* ranks, UniqueIDWrapper uniqueId)
{
	std::set<int> playlists{ PlaylistType::RANKEDSTANDARD, PlaylistType::RANKEDDOUBLES, PlaylistType::RANKEDDUEL };

	if (lastPlaylist != -1) {
		playlists.insert(lastPlaylist);
	}
	for (int playlist : playlists) {
		float mmr = gameWrapper->GetMMRWrapper().GetPlayerMMR(uniqueId, playlist);
		SkillRank skillRank = gameWrapper->GetMMRWrapper().GetPlayerRank(uniqueId, playlist);
		Rank rank;
		rank.mmr = mmr;
		rank.playlist = static_cast<PlaylistType>(playlist);
		rank.tier = skillRank.Tier;
		rank.division = skillRank.Division;
		rank.matches_played = skillRank.MatchesPlayed;
		ranks->push_back(rank);
	}
}

void RlTrackerPlugin::testEvent(const std::string& eventName)
{
 	TestEvent event(eventName);
 	server->broadcast_json(event);
}

Match RlTrackerPlugin::buildMatch(ServerWrapper state)
{
	Match match;
	std::list<Player> players = getMatchPlayers(state);
	match.guid = state.GetMatchGUID();
	auto playlist = state.GetPlaylist();
	match.playlist = static_cast<PlaylistType>(playlist.GetPlaylistId());
	match.ranked = playlist.GetbRanked();
	match.players = players;
	return match;
}
