import React from "react";
import {PosNegStatistic} from "./PosNegStatistic";

type ChangeStatisticProps = { value: number, title: string, arrow?: boolean, digits?: number};

export function ChangeStatistic({value, title, arrow, digits}: ChangeStatisticProps) {
    const type = value < 0 ? "negative" : value > 0 ? "positive" : "neutral";
    const formattedValue = digits ? value.toFixed(digits) : value
    return <PosNegStatistic type={type}
                            value={type === "neutral" ? "-" : ((type === "positive" ? "+" : "") + formattedValue)}
                            title={title} arrow={!arrow ? arrow : true}/>
}
