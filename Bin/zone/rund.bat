@echo off

echo Starting Master Server
start "zone-master" "KFStartupd.exe" "0" "100" "./setting/master.startup"

echo Starting World Server
start "zone-world" "KFStartupd.exe" "0" "200" "./setting/world.startup"

echo Starting Gate Server
start "zone-gate" "KFStartupd.exe" "0" "300" "./setting/gate.startup"

echo Starting Login Server
start "zone-login" "KFStartupd.exe" "0" "400" "./setting/login.startup"

echo Starting Proxy Server
start "zone-proxy" "KFStartupd.exe" "0" "500" "./setting/proxy.startup"

echo Starting Game Server
start "zone-game" "KFStartupd.exe" "0" "600" "./setting/game.startup"
