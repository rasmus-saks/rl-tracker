import {serve} from "https://deno.land/std@0.152.0/http/server.ts";
import {serveDir, serveFile} from "https://deno.land/std@0.152.0/http/file_server.ts";

const pause = (ms: number) => new Promise(res => setTimeout(res, ms))
const sockets = new Map<string, WebSocket>()

type Session = {
    currentPlaylist: string | null
    stats: {[key: string]: PlaylistStats}
}

type PlaylistStats = {
    start: PlaylistRank
    current: PlaylistRank
    playlist: Playlist
    wins: number
    losses: number
    streak: number
}

type PlaylistRank = {
    playlist: Playlist
    mmr: number
    tier: number
    division: number
    matchesPlayed: number
}

type EventMatch = {
    players: EventPlayer[]
    guid: string
    playlist: Playlist
    ranked: boolean
}

type EventPlayer = {
    name: string,
    steamid: string,
    platform: string
    ranks: Rank[]
    self: boolean
    team: Team
}

type Rank = {
    playlist: Playlist
    tier: number
    division: number
    matches_played: number
    mmr: number
}

type Team = 'UNKNOWN_TEAM' | 'BLUE' | 'ORANGE'
type Playlist = string

type MatchEndEvent = {
    type: "MATCH_END_EVENT"
    match: EventMatch
    winner: Team
}

type CountdownBeginEvent = {
    type: "COUNTDOWN_BEGIN_EVENT"
    match: EventMatch
}

type GoalEvent = {
    type: "GOAL_EVENT"
    match: EventMatch
}

type MmrUpdateEvent = {
    type: "MMR_UPDATE_EVENT"
    ranks: Rank[]
}

type Event = MatchEndEvent | CountdownBeginEvent | GoalEvent | MmrUpdateEvent

let lastSessionUpdate = new Date()
const SESSION_TIMEOUT = 60 * 60 * 6 * 1000 // 6 hours
const session: Session = {
    stats: {},
    currentPlaylist: null
}
let lastMatch: EventMatch | undefined = undefined

async function readPacket(connection: Deno.Conn) {
    // Read 8 bytes of packet length (unsigned 64-bit integer)
    const lengthBytes = await readBytes(connection, 8)
    const dataView = new DataView(lengthBytes.buffer)
    const length = Number(dataView.getBigUint64(0, true))

    // Read <length> bytes of message
    const data = await readBytes(connection, length)
    const decoder = new TextDecoder()
    const rawJson = decoder.decode(data);
    try {
        // Decode JSON message and handle it
        const dataObj = JSON.parse(rawJson)
        handleEvent(dataObj)
    } catch (e) {
        console.log("Failed to handle packet:")
        console.log(rawJson)
        console.error(e)
    }
}

function broadcast(message: any) {
    for (const [, value] of sockets) {
        value.send(JSON.stringify(message))
    }
}

function handleEvent(event: Event): void {
    console.log(`Received ${event.type}`)
    if (event.type === 'COUNTDOWN_BEGIN_EVENT' || event.type === 'GOAL_EVENT') {
        updateStats(event.match)
        broadcastStats(event.match)
    } else if (event.type === 'MATCH_END_EVENT') {
        updateStreak(event)
        updateStats(event.match)
        broadcastStats(event.match)
    } else if (event.type === 'MMR_UPDATE_EVENT') {
        updateMmr(event)
        broadcastStats()
    }
}

function broadcastStats(match?: EventMatch) {
    broadcast({type: "SESSION_STATS", data: session})
    if (match) {
        const {players, ...rest} = match
        const frameMatch = {...rest, players: players.map(toFramePlayer)}
        broadcast({type: "STATS", data: frameMatch})
    }
}

function updateMmr(event: MmrUpdateEvent) {
    event.ranks.forEach(rank => {
        if(session.stats[rank.playlist]) {
            session.stats[rank.playlist].current = toPlaylistRank(rank)
        }
    })
}

function toPlaylistRank(rank: Rank): PlaylistRank {
    return {
        playlist: rank.playlist,
        division: rank.division,
        tier: rank.tier,
        matchesPlayed: rank.matches_played,
        mmr: rank.mmr
    }
}

function updateStreak(event: MatchEndEvent) {
    const match = event.match
    const winner = event.winner
    if (winner === 'UNKNOWN_TEAM') return
    const me = match.players.find(p => p.self)!
    const playlist = match.playlist
    const diff = new Date().getTime() - lastSessionUpdate.getTime()
    if (diff > SESSION_TIMEOUT) {
        session.currentPlaylist = null
        session.stats = {}
        console.log("Cleared session stats due to timeout")
    }
    lastSessionUpdate = new Date()
    if (!session.stats[playlist]) {
        const current = toPlaylistRank(me.ranks.find(r => r.playlist === playlist)!)
        session.stats[playlist] = {
            playlist, start: current, current, wins: 0, losses: 0, streak: 0
        }
    }
    const playlistSession = session.stats[playlist]
    if (me.team === winner) {
        playlistSession.wins += 1
        playlistSession.streak = Math.max(1, playlistSession.streak + 1)
    } else {
        playlistSession.losses += 1
        playlistSession.streak = Math.min(-1, playlistSession.streak - 1)
    }
}

function updateStats(match: EventMatch) {
    const me = match.players.find(p => p.self)!
    const playlist = match.playlist
    const current = me.ranks.find(r => r.playlist === playlist)
    if (!current) return
    const currentPlaylistRank = toPlaylistRank(current)
    session.currentPlaylist = playlist
    if (!session.stats[playlist]) {
        session.stats[playlist] = {
            playlist, start: currentPlaylistRank, current: currentPlaylistRank, wins: 0, losses: 0, streak: 0
        }
    } else {
        session.stats[playlist].current = currentPlaylistRank
    }
    lastMatch = match
}

function toFramePlayer(player: EventPlayer): any {
    return {
        ...player,
        ranks: player.ranks.reduce((ac: any, curr) => (ac[curr.playlist] = curr, ac), {}),
        trackerLink: `https://rocketleague.tracker.network/rocket-league/profile/${trackerPath(player)}`
    }
}

function trackerPath(player: EventPlayer): string | null {
    const platform = player.platform;
    let slug: string
    if (platform === 'PS3' || platform === 'PS4') slug = 'psn'
    else if (platform === 'XBOX') slug = 'xbl'
    else if (platform === 'STEAM') slug = 'steam'
    else if (platform === 'EPIC') slug = 'epic'
    else return null

    let identifier: string
    if (platform === 'PS3' || platform === 'PS4' || platform === 'XBOX' || platform === 'EPIC') identifier = player.name
    else identifier = player.steamid
    return `${slug}/${identifier}`
}

async function readBytes(connection: Deno.Conn, length: number) {
    const data = new Uint8Array(length)
    await connection.read(data)
    return data
}

async function pluginListener() {
    let lasterror = {name: ""}
    const hostname = Deno.env.get("PLUGIN_HOST") ?? "localhost"
    while (true) {
        try {
            const connection = await Deno.connect({hostname, port: 8081})
            lasterror = {name: ""}
            while (true) {
                await readPacket(connection)
            }
        } catch (e) {
            if (lasterror.name !== e.name && lasterror.name !== "") {
                console.error(e)
            }
            lasterror = e
        }
        await pause(5000)
    }
}

pluginListener()
const port = Deno.env.get("PORT") ?? 8000
serve(req => {
    const {pathname} = new URL(req.url)
    const fsRoot = Deno.env.get("FRONTEND_ROOT") ?? "frontend"
    if (pathname === '/') {
        return serveFile(req, fsRoot + "/index.html")
    }
    if (pathname === '/ws') {
        const {response, socket} = Deno.upgradeWebSocket(req)
        const socketId = crypto.randomUUID();
        sockets.set(socketId, socket)
        socket.onclose = () => {
            console.log(`Client id ${socketId} disconnected`)
            sockets.delete(socketId);
        }
        socket.onopen = () => {
            console.log(`Client id ${socketId} connected`)
            broadcastStats(lastMatch)
        }
        return response
    }
    return serveDir(req, {fsRoot, quiet: true})
}, {port: +port})

