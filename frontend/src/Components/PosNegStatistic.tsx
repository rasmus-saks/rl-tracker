import {StatisticProps} from "antd/lib/statistic/Statistic";
import {Statistic} from "antd";
import {ArrowDownOutlined, ArrowUpOutlined} from "@ant-design/icons";

type PostNegStatisticProps =
    { type: "positive" | "negative" | "neutral", arrow?: boolean }
    & Omit<StatisticProps, "valueStyle">;

export function PosNegStatistic({type, arrow, ...rest}: PostNegStatisticProps) {
    const positive = type === "positive";
    const negative = type === "negative";
    return <Statistic valueStyle={{color: positive ? "rgb(73, 170, 25)" : negative ? "rgb(166, 29, 36)" : undefined}}
                      prefix={arrow ? (positive ? <ArrowUpOutlined/> : negative ? <ArrowDownOutlined/> : null) : null}
                      {...rest} />
}
