import React from "react";
import Broker from "../../Util/Broker";
import {Badge, Card, Col, Empty, Row, Tabs} from "antd";
import {RankStatistic} from "../../Components/RankStatistic";
import {ChangeStatistic} from "../../Components/ChangeStatistic";
import {PosNegStatistic} from "../../Components/PosNegStatistic";
import {Playlist, SessionStatsType, StatsType} from "../../Protocol/Types";

const PLAYLIST_NAMES: { [key: string]: string } = {
    PRIVATEMATCH: "Private Match",
    RANKEDDUEL: "Ranked 1v1",
    RANKEDDOUBLES: "Ranked 2v2",
    RANKEDSTANDARD: "Ranked 3v3",
    DUEL: "Unranked 1v1",
    DOUBLES: "Unranked 2v2",
    STANDARD: "Unranked 3v3",
    CHAOS: "Unranked 4v4",
    RANKEDRUMBLE: "Rumble",
    RANKEDHOOPS: "Hoops",
    RANKEDDROPSHOT: "Dropshot",
    RANKEDSNOWDAY: "Snow Day",
    TOURNAMENT_MATCH_AUTOMATIC: "Tournament",
}

export function SessionStats() {
    const [stats, setStats] = React.useState<StatsType>({})
    const [currentPlaylist, setCurrentPlaylist] = React.useState<Playlist | undefined>(undefined)
    const [selectedTab, setSelectedTab] = React.useState<Playlist | undefined>(currentPlaylist || Object.keys(stats)[0] as Playlist)
    React.useEffect(() => {
        const sub = Broker.subscribe("/rocket-league", "SESSION_STATS", (message: SessionStatsType) => {
            setStats(message.stats);
            setCurrentPlaylist((lastPlaylist: Playlist | undefined) => {
                if (lastPlaylist !== message.currentPlaylist) {
                    setSelectedTab(message.currentPlaylist);
                }
                if (message.currentPlaylist === null && Object.keys(message.stats).length > 0) {
                    setSelectedTab(Object.keys(message.stats)[0] as Playlist)
                }
                return message.currentPlaylist
            });
        });

        return function cleanup() {
            Broker.unsubscribe(sub);
        }
    }, [setSelectedTab]);

    const onTabChanged = React.useCallback((key: string) => {
        setSelectedTab(key as Playlist)
    }, [setSelectedTab]);

    const tabContent = React.useCallback((playlist: string) => {
        const {start, current, wins, losses, streak} = stats[playlist as Playlist]!!;
        const mmrChange = current.mmr - start.mmr;
        return <>
            <Row align={"middle"} justify={"center"} style={{textAlign: "center"}}>
                <Col span={8}>
                    <RankStatistic title={"Start MMR"} rank={start}/>
                </Col>
                <Col span={8}>
                    <ChangeStatistic value={mmrChange} title={"Change"} digits={1} arrow={true}/>
                </Col>
                <Col span={8}>
                    <RankStatistic title={"Current MMR"} rank={current}/>
                </Col>
            </Row>
            <Row align={"middle"} justify={"center"} style={{textAlign: "center"}}>
                <Col span={8}>
                    <PosNegStatistic type={"positive"} value={wins} title={"Wins"}/>
                </Col>
                <Col span={8}>
                    <ChangeStatistic value={streak} title={"Streak"} digits={0} />
                </Col>
                <Col span={8}>
                    <PosNegStatistic type={"negative"} value={losses} title={"Losses"}/>
                </Col>
            </Row>
        </>;
    }, [stats]);

    const cardContent = React.useCallback(() => {
        const tabs = []
        for (const playlist of Object.keys(stats).sort((a, b) => playlistName(a).localeCompare(playlistName(b)))) {
            tabs.push(
                <Tabs.TabPane key={playlist}
                              tab={<Badge dot={currentPlaylist === playlist}>{playlistName(playlist)}</Badge>}>
                    {tabContent(playlist)}
                </Tabs.TabPane>
            )
        }
        return <Tabs onChange={onTabChanged} activeKey={selectedTab} size={"small"}>
            {tabs}
        </Tabs>;
    }, [tabContent, onTabChanged, stats, selectedTab, currentPlaylist]);

    const emptyCardContent = React.useCallback(() =>
        <Empty image={Empty.PRESENTED_IMAGE_SIMPLE} description={"No active session"}/>, []);

    return <Card title={"Session stats"} style={{height: "100%"}}>
        {Object.keys(stats).length !== 0 ? cardContent() : emptyCardContent()}
    </Card>
}

function playlistName(playlist: string | undefined): string {
    return (playlist && PLAYLIST_NAMES[playlist]) || playlist || ""
}
