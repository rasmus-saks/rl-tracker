#!/usr/bin/env bash

rm -rf ./build
mkdir -p build/frontend
deno bundle ./server/server.ts ./build/server.ts

pushd frontend || exit
npm run build
mv build/* ../build/frontend
popd || exit
