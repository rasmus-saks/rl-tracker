import {RankImg} from "./RankImg";
import {Statistic} from "antd";
import React from "react";
import {PlaylistRank} from "../Protocol/Types";

export function RankStatistic({rank, title}: { rank: PlaylistRank, title: string }) {
    return <Statistic value={rank.mmr} title={title} prefix={<RankImg rank={rank}/>} precision={1}/>
}
