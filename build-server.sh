#!/usr/bin/env bash

if [[ -z "${WEBSOCKET_URL}" ]]; then
    WEBSOCKET_URL="ws://localhost:8000/ws"
fi

rm -rf ./build
mkdir -p build/frontend
deno bundle ./server/server.ts ./build/server.ts

pushd frontend || exit
REACT_APP_WEBSOCKET_URL="${WEBSOCKET_URL}" npm run build
mv build/* ../build/frontend
popd || exit

echo "#!/usr/bin/env bash

deno run --allow-net --allow-env --allow-read server.ts" > ./build/run.sh

chmod +x ./build/run.sh

pushd build || exit
zip -9 -r -q ../rl-tracker.zip .
popd || exit
