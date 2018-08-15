@echo off

echo Starting Master Server
start "zone-master" "KFStartupd.exe" "appid=100.1.241.1" "log=1" "startup=./setting/master.startup"

echo Starting World Server
start "zone-world" "KFStartupd.exe" "appid=100.1.242.1" "log=1" "startup=./setting/world.startup"

echo Starting Gate Server
start "zone-gate" "KFStartupd.exe" "appid=100.1.243.1" "log=1" "startup=./setting/gate.startup"

echo Starting Login Server
start "zone-login" "KFStartupd.exe" "appid=100.1.244.1" "log=1" "startup=./setting/login.startup"

echo Starting Proxy Server
start "zone-proxy" "KFStartupd.exe" "appid=100.1.245.1" "log=1" "startup=./setting/proxy.startup"

echo Starting Game Server
start "zone-game" "KFStartupd.exe" "appid=100.1.246.1" "log=1" "startup=./setting/game.startup"
