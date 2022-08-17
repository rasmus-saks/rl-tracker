# Stats tracker for BakkesMod

![screenshot](docs/images/screenshot.png)

## Features

* See a list of all the players in the match and their ranks
* Click on a player to open their Tracker Network page
* Keep track of your current session - wins, losses, streak, MMR change

## Building

### Plugin

Requirements

* Boost >=1.72.0 installed

Steps: (todo)

### Server

Run `./build-server.sh`. It will write the server and frontend to the `build` folder.

## Running the server

Run `deno run --allow-net --allow-env --allow-read server.ts`

### Configuration

Use environment variables to configure the server:

| Variable name | Description                                                             |
|---------------|-------------------------------------------------------------------------|
| PORT          | The port that the server will run under. Defaults to 8000               |
| FRONTEND_ROOT | Location of the frontend (the `build` folder). Defaults to `./frontend` |

