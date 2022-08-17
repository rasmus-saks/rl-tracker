import {RankImg} from "../../Components/RankImg";
import React from "react";
import {Match, Player} from "../../Protocol/Types";

export function PlayerName({player, match}: { player: Player, match: Match }) {
    return <><RankImg rank={player.ranks[match.playlist]} title={player.ranks[match.playlist].mmr.toFixed(1)}/> <a
        href={player.trackerLink} target={"_blank"}
        rel={"noreferrer"}>{player.name}</a></>
}
