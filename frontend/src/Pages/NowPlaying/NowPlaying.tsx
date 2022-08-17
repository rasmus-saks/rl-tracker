import {ColumnsType} from "antd/es/table";
import React from "react";
import {Empty, Table} from "antd";
import Broker from "../../Util/Broker";
import {RankSummary} from "../../Components/RankSummary";
import "./NowPlaying.css";
import {Match, Player, Ranks} from "../../Protocol/Types";
import {PlayerName} from "./PlayerName";



export function NowPlaying() {
    const [match, setMatch] = React.useState<Match>()
    React.useEffect(() => {
        const sub = Broker.subscribe("/rocket-league", "STATS", (match: Match) => {
            match.players
                .sort((a: Player, b: Player) => b.score - a.score)
                .sort((a: Player, b: Player) => a.team.localeCompare(b.team))
            setMatch(match);
        });

        return function cleanup() {
            Broker.unsubscribe(sub);
        }
    }, []);

    const columns: ColumnsType<Player> = React.useMemo(() => [
        {
            title: "Name",
            dataIndex: "name",
            key: "name",
            render: (value: string, p: Player) => <PlayerName player={p} match={match!!}/>
        },
        {
            title: "Ranks",
            dataIndex: "ranks",
            key: "ranks",
            render: (ranks: Ranks) => <RankSummary ranks={ranks}/>,
            width: 150
        },
        {
            title: "Score",
            dataIndex: "score",
            key: "score"
        },
        {
            title: "Goals",
            dataIndex: "goals",
            key: "goals"
        },
        {
            title: "Saves",
            dataIndex: "saves",
            key: "saves"
        },
        {
            title: "Assists",
            dataIndex: "assists",
            key: "assists"
        },
        {
            title: "Shots",
            dataIndex: "shots",
            key: "shots"
        },
    ], [match]);
    return <Table columns={columns}
                  dataSource={match?.players}
                  pagination={false}
                  rowClassName={(record: Player) => record.team === "ORANGE" ? "table-row-orange" : "table-row-blue"}
                  locale={{emptyText: <Empty image={Empty.PRESENTED_IMAGE_SIMPLE} description={"Not playing anything"}/>}}
    >
    </Table>
}
