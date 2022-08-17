export type SessionStatsType = {
    stats: StatsType,
    currentPlaylist?: Playlist
}

export type StatsType = { [key in Playlist]?: PlaylistStats }

export type Frame<T> = {
    type: string,
    data: T
}

export type Match = {
    players: Player[],
    guid: string,
    playlist: Playlist,
    ranked: boolean
}

export type Player = {
    steamId: string,
    platform: Platform,
    epicAccountId: string,
    trackerLink?: string,
    name: string,
    self: boolean,
    team: Team,
    score: number,
    goals: number,
    assists: number,
    saves: number,
    shots: number,
    ranks: Ranks
}

export type Ranks = { [key in Playlist]: PlaylistRank}

export type Team = keyof typeof TeamEnum

const enum TeamEnum {
    UNKNOWN_TEAM,
    BLUE,
    ORANGE
}

export type Platform = keyof typeof PlatformEnum

const enum PlatformEnum {
    UNKNOWN,
    STEAM,
    PS4,
    PS3,
    XBOX,
    QQ,
    OLD_NNX,
    NNX,
    PSYNET,
    DELETED,
    WEGAME,
    EPIC,
    MAX
}

export type PlaylistRank = {
    playlist: string,
    mmr: number,
    tier: number,
    division: number,
    matchesPlayer: number
}

export type PlaylistStats = {
    start: PlaylistRank,
    current: PlaylistRank,
    playlist: Playlist,
    wins: number,
    losses: number
    streak: number
}

export type Playlist = keyof typeof PlaylistEnum

const enum PlaylistEnum {
    UNKNOWN_PLAYLIST,
    DUEL,
    DOUBLES,
    STANDARD,
    CHAOS,
    PRIVATEMATCH,
    OFFLINESEASON,
    OFFLINESPLITSCREEN,
    TRAINING,
    RANKEDDUEL,
    RANKEDDOUBLES,
    RANKEDSOLOSTANDARD,
    RANKEDSTANDARD,
    MUTATORMASHUP,
    SNOWDAY,
    ROCKETLABS,
    HOOPS,
    RUMBLE,
    WORKSHOP,
    TRAININGEDITOR,
    CUSTOMTRAINING,
    TOURNAMENT,
    DROPSHOT,
    RANKEDHOOPS,
    RANKEDRUMBLE,
    RANKEDDROPSHOT,
    RANKEDSNOWDAY,
    GHOST_HUNT,
    BEACH_BALL,
    SPIKE_RUSH,
    TOURNAMENT_MATCH_AUTOMATIC,
    ROCKET_LABS,
    DROPSHOT_RUMBLE,
    HEATSEEKER,
    BOOMER_BALL,
    HEATSEEKER_DOUBLES,
    WINTER_BREAKAWAY,
    GRIDIRON,
    SUPER_CUBE
}
