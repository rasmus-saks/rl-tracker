import {Client, StompSubscription} from "@stomp/stompjs";
import {Frame} from "../Protocol/Types";

export function getBrokerUrl(): string {
    return new URLSearchParams(window.location.search).get("websocketUrl") ?? localStorage.getItem("websocketUrl") ?? process.env.REACT_APP_WEBSOCKET_URL!!;
}

export type MessageSubscription<T> = {
    type: string
    destination: string
    onMessage: (message: T) => void
}

export interface BrokerSubscription<T> {
    messageSubs: { [key: string]: MessageSubscription<T>[] }
    stompSub: StompSubscription | null
}

interface Broker {
    subscribe<T>(destination: string, type: string, onMessage: (message: T) => void): MessageSubscription<T>

    unsubscribe(sub: MessageSubscription<any> | null): void
}

class TheBroker implements Broker {
    private readonly subscriptions: { [key: string]: BrokerSubscription<any> } = {};
    private readonly client = new Client();

    constructor() {
        this.client.brokerURL = getBrokerUrl();
        this.client.onConnect = () => {
            this.subscribeAll();
        };
        this.client.activate();
    }

    private subscribeAll() {
        for (const destination of Object.keys(this.subscriptions)) {
            this.doSubscription(destination)
        }
    }

    private doSubscription<T>(destination: string) {
        if (this.client.connected) {
            this.subscriptions[destination].stompSub = this.client.subscribe(destination, (message) => {
                const frame: Frame<T> = JSON.parse(message.body)
                const subs = this.subscriptions[destination].messageSubs[frame.type] || []
                for (const sub of subs) {
                    sub.onMessage(frame.data)
                }
            })
        }
    }

    private doUnsubscription(sub: MessageSubscription<any>) {
        const brokerSub = this.subscriptions[sub.destination]
        if (brokerSub.stompSub !== null && this.client.connected) {
            const messageSubs = brokerSub.messageSubs[sub.type];
            const msgSubIdx = messageSubs.indexOf(sub)
            if (msgSubIdx !== -1) {
                messageSubs.splice(msgSubIdx, 1)
            }
            if (messageSubs.length === 0) {
                brokerSub.stompSub.unsubscribe()
                brokerSub.stompSub = null;
            }
        }
    }

    subscribe<T>(destination: string, type: string, onMessage: (message: T) => void): MessageSubscription<T> {
        if (!this.subscriptions[destination]) {
            this.subscriptions[destination] = {messageSubs: {}, stompSub: null}
            this.doSubscription(destination)
        }
        if (!this.subscriptions[destination].messageSubs[type]) {
            this.subscriptions[destination].messageSubs[type] = []
        }
        const messageSub = {destination, type, onMessage};
        this.subscriptions[destination].messageSubs[type].push(messageSub)
        return messageSub
    }

    unsubscribe(sub: MessageSubscription<any> | null) {
        if (sub == null) return
        this.doUnsubscription(sub)
    }
}

class SimpleBroker implements Broker {
    private subs: { [key: string]: MessageSubscription<any>[] } = {}
    private socket: WebSocket

    constructor() {
        this.socket = this.initSocket()
    }

    private initSocket(): WebSocket {
        const socket = new WebSocket(getBrokerUrl())
        socket.onclose = () => {
            console.log("Lost connection to WebSocket at " + socket.url)
            setTimeout(() => this.socket = this.initSocket(), 3000)
        }
        socket.onmessage = (ev) => {
            const frame = JSON.parse(ev.data)
            for (const sub of (this.subs[frame.type] || [])) {
                sub.onMessage(frame.data)
            }
        }
        socket.onopen = () => {
            console.log("Connected to WebSocket at " + socket.url)
        }
        return socket
    }

    subscribe<T>(destination: string, type: string, onMessage: (message: T) => void): MessageSubscription<T> {
        if (!this.subs[type]) {
            this.subs[type] = []
        }
        const messageSub = {type, destination, onMessage}
        this.subs[type].push(messageSub)
        return messageSub;
    }

    unsubscribe(sub: MessageSubscription<any> | null): void {
        if (sub === null) return
        const idx = this.subs[sub.type].indexOf(sub)
        if (idx !== -1) {
            this.subs[sub.type].splice(idx, 1)
        }
    }

}

const BROKER = new SimpleBroker();

export default BROKER;
