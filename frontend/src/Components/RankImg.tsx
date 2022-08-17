import "./RankImg.css";
import {Tooltip} from "antd";
import {PlaylistRank} from "../Protocol/Types";

export type RankImgProps = {
    rank: PlaylistRank,
    title?: string
}

export function RankImg({rank: {tier, division}, title}: RankImgProps) {
    let divText = "";
    if (tier !== 0) {
        if (division === 0) {
            divText = "I"
        } else if (division === 1) {
            divText = "II"
        } else if (division === 2) {
            divText = "III"
        } else if (division === 3) {
            divText = "IV"
        }
    }
    return (<Tooltip title={title}>
        <span className="rank-image">
            <img src={`tiers/${tier}.png`} alt="" width="32px" height="32px"/>
            <span className="rank-image-division">{divText}</span>
    </span>
    </Tooltip>);
}
