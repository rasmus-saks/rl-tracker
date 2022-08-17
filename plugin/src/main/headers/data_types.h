#pragma once
#include <string>
#include <list>
#include "json.hpp"
#include <bakkesmod\wrappers\wrapperstructs.h>

using json = nlohmann::json;

enum PlaylistType {
	UNKNOWN_PLAYLIST = 0,
	DUEL = 1,
	DOUBLES = 2,
	STANDARD = 3,
	CHAOS = 4,
	PRIVATEMATCH = 6,
	OFFLINESEASON = 7,
	OFFLINESPLITSCREEN = 8,
	TRAINING = 9,
	RANKEDDUEL = 10,
	RANKEDDOUBLES = 11,
	RANKEDSOLOSTANDARD = 12,
	RANKEDSTANDARD = 13,
	MUTATORMASHUP = 14,
	SNOWDAY = 15,
	ROCKETLABS = 16,
	HOOPS = 17,
	RUMBLE = 18,
	WORKSHOP = 19,
	TRAININGEDITOR = 20,
	CUSTOMTRAINING = 21,
	TOURNAMENT = 22,
	DROPSHOT = 23,
	RANKEDHOOPS = 27,
	RANKEDRUMBLE = 28,
	RANKEDDROPSHOT = 29,
	RANKEDSNOWDAY = 30,
	GHOST_HUNT = 31,
	BEACH_BALL = 32,
	SPIKE_RUSH = 33,
	TOURNAMENT_MATCH_AUTOMATIC = 34,
	ROCKET_LABS = 35,
	DROPSHOT_RUMBLE = 36,
	HEATSEEKER = 38,
	BOOMER_BALL = 41,
	HEATSEEKER_DOUBLES = 43,
	WINTER_BREAKAWAY = 44,
	GRIDIRON = 46,
	SUPER_CUBE = 47
};
NLOHMANN_JSON_SERIALIZE_ENUM(PlaylistType, {
	{UNKNOWN_PLAYLIST, "UNKNOWN_PLAYLIST"},
	{DUEL, "DUEL"},
	{DOUBLES, "DOUBLES"},
	{STANDARD, "STANDARD"},
	{CHAOS, "CHAOS"},
	{PRIVATEMATCH, "PRIVATEMATCH"},
	{OFFLINESEASON, "OFFLINESEASON"},
	{OFFLINESPLITSCREEN, "OFFLINESPLITSCREEN"},
	{TRAINING, "TRAINING"},
	{RANKEDDUEL, "RANKEDDUEL"},
	{RANKEDDOUBLES, "RANKEDDOUBLES"},
	{RANKEDSOLOSTANDARD, "RANKEDSOLOSTANDARD"},
	{RANKEDSTANDARD, "RANKEDSTANDARD"},
	{MUTATORMASHUP, "MUTATORMASHUP"},
	{SNOWDAY, "SNOWDAY"},
	{ROCKETLABS, "ROCKETLABS"},
	{HOOPS, "HOOPS"},
	{RUMBLE, "RUMBLE"},
	{WORKSHOP, "WORKSHOP"},
	{TRAININGEDITOR, "TRAININGEDITOR"},
	{CUSTOMTRAINING, "CUSTOMTRAINING"},
	{TOURNAMENT, "TOURNAMENT"},
	{DROPSHOT, "DROPSHOT"},
	{RANKEDHOOPS, "RANKEDHOOPS"},
	{RANKEDRUMBLE, "RANKEDRUMBLE"},
	{RANKEDDROPSHOT, "RANKEDDROPSHOT"},
	{RANKEDSNOWDAY, "RANKEDSNOWDAY"},
	{GHOST_HUNT, "GHOST_HUNT"},
	{BEACH_BALL, "BEACH_BALL"},
	{SPIKE_RUSH, "SPIKE_RUSH"},
	{TOURNAMENT_MATCH_AUTOMATIC, "TOURNAMENT_MATCH_AUTOMATIC"},
	{ROCKET_LABS, "ROCKET_LABS"},
	{DROPSHOT_RUMBLE, "DROPSHOT_RUMBLE"},
	{HEATSEEKER, "HEATSEEKER"},
	{BOOMER_BALL, "BOOMER_BALL"},
	{HEATSEEKER_DOUBLES, "HEATSEEKER_DOUBLES"},
	{WINTER_BREAKAWAY, "WINTER_BREAKAWAY"},
	{GRIDIRON, "GRIDIRON"},
	{SUPER_CUBE, "SUPER_CUBE"}
	}
)

enum Team {
	UNKNOWN_TEAM = 0,
	BLUE = 1,
	ORANGE = 2
};
NLOHMANN_JSON_SERIALIZE_ENUM(Team, {
	{UNKNOWN_TEAM, "UNKNOWN_TEAM"},
	{BLUE, "BLUE"},
	{ORANGE, "ORANGE"}
	}
)

enum Platform
{
	Platform_Unknown = 0,
	Platform_Steam = 1,
	Platform_PS4 = 2,
	Platform_PS3 = 3,
	Platform_Dingo = 4, //XBox?
	Platform_QQ = 5,
	Platform_OldNNX = 6,
	Platform_NNX = 7,
	Platform_PsyNet = 8,
	Platform_Deleted = 9,
	Platform_WeGame = 10,
	Platform_Epic = 11,
	Platform_MAX = 12
};
NLOHMANN_JSON_SERIALIZE_ENUM(Platform, {
	{Platform_Unknown, "UNKNOWN"},
	{Platform_Steam, "STEAM"},
	{Platform_PS4, "PS4"},
	{Platform_PS3, "PS3"},
	{Platform_Dingo, "XBOX"}, //XBox?
	{Platform_QQ, "QQ"},
	{Platform_OldNNX, "OLD_NNX"},
	{Platform_NNX, "NNX"},
	{Platform_PsyNet, "PSYNET"},
	{Platform_Deleted, "DELETED"},
	{Platform_WeGame, "WEGAME"},
	{Platform_Epic, "EPIC"},
	{Platform_MAX, "MAX"}
	})

struct Rank {
	PlaylistType playlist;
	signed int tier;
	signed int division;
	signed int matches_played;
	float mmr;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Rank, playlist, tier, division, matches_played, mmr)

struct Player {
	std::string steamid;
	std::string epic_account_id;
	Platform platform;
	bool self;
	signed int score;
	signed int goals;
	signed int assists;
	signed int saves;
	signed int shots;
	signed int demos;
	std::string name;
	std::list<Rank> ranks;
	Team team;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Player, steamid, epic_account_id, platform, self, score, goals, assists, saves, shots, demos, name, ranks, team)



struct Match {
	std::list<Player> players;
	std::string guid;
	PlaylistType playlist;
	bool ranked;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Match, players, guid, playlist, ranked)

enum EventType {
	UNKNOWN_EVENT = 0,
	TEST_EVENT = 1,
	COUNTDOWN_BEGIN_EVENT = 2,
	GOAL_EVENT = 3,
	MATCH_END_EVENT = 4,
	PERIODIC_STATS_EVENT = 5,
	MMR_UPDATE_EVENT = 6,
};
NLOHMANN_JSON_SERIALIZE_ENUM(EventType, {
	{UNKNOWN_EVENT, "UNKNOWN_EVENT"},
	{TEST_EVENT, "TEST_EVENT"},
	{COUNTDOWN_BEGIN_EVENT, "COUNTDOWN_BEGIN_EVENT"},
	{GOAL_EVENT, "GOAL_EVENT"},
	{MATCH_END_EVENT, "MATCH_END_EVENT"},
	{PERIODIC_STATS_EVENT, "PERIODIC_STATS_EVENT"},
	{MMR_UPDATE_EVENT, "MMR_UPDATE_EVENT"},
	});


struct BaseEvent {
	EventType type;
	BaseEvent(EventType type) : type(type) {}
};

struct CountdownBeginEvent : BaseEvent {
	Match match;
	CountdownBeginEvent(Match match) : BaseEvent(COUNTDOWN_BEGIN_EVENT), match(match) {}
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CountdownBeginEvent, type, match)

struct PeriodicStatsEvent : BaseEvent {
	Match match;
	PeriodicStatsEvent(Match match) : BaseEvent(PERIODIC_STATS_EVENT), match(match) {}
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PeriodicStatsEvent, type, match)

struct MmrUpdateEvent : BaseEvent {
	std::list<Rank> ranks;
	MmrUpdateEvent(std::list<Rank> ranks) : BaseEvent(MMR_UPDATE_EVENT), ranks(ranks) {}
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MmrUpdateEvent, type, ranks)

struct GoalEvent : BaseEvent {
	Match match;
	GoalEvent(Match match) : BaseEvent(GOAL_EVENT), match(match) {}
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GoalEvent, type, match)

struct TestEvent : BaseEvent {
	std::string message;
	TestEvent(std::string message) : BaseEvent(TEST_EVENT), message(message) {}
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TestEvent, type, message)

struct MatchEndEvent : BaseEvent {
	Match match;
	Team winner;
	MatchEndEvent(Match match, Team winner) : BaseEvent(MATCH_END_EVENT), match(match), winner(winner) {}
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MatchEndEvent, type, match, winner)
