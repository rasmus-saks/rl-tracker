import React from 'react';
import './App.css';
import {Col, Layout, Menu, Row} from "antd";
import {NowPlaying} from "./Pages/NowPlaying/NowPlaying";
import {PlayCircleFilled} from "@ant-design/icons";
import {SessionStats} from "./Pages/SessionStats/SessionStats";

function App() {
    return (
        <Layout>
            <Layout.Header>
                <img src={"/RocketLeagueLogoText.png"} className={"logo"} alt={""} />
                <Menu theme={"dark"} mode={"horizontal"} selectedKeys={["now-playing"]}>
                    <Menu.Item key={"now-playing"} icon={<PlayCircleFilled/>}>Now Playing</Menu.Item>
                </Menu>
            </Layout.Header>
            <Layout.Content style={{padding: '50px 50px'}}>
                <Row gutter={16}>
                    <Col span={18}><NowPlaying/></Col>
                    <Col span={6}><SessionStats/></Col>
                </Row>
            </Layout.Content>
        </Layout>
    );
}

export default App;
