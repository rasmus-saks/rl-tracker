import {Space} from "antd";
import {RankImg} from "./RankImg";
import {Ranks} from "../Protocol/Types";


export function RankSummary({ranks}: {ranks: Ranks}) {
    return <Space direction={"horizontal"}>
        <RankImg rank={ranks["RANKEDDUEL"]} title={"Duel"}/>
        <RankImg rank={ranks["RANKEDDOUBLES"]} title={"Doubles"}/>
        <RankImg rank={ranks["RANKEDSTANDARD"]} title={"Standard"}/>
    </Space>
}
