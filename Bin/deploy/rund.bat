
echo Starting Deploy
start "Deploy" "KFStartupd.exe" "appid=100.0.2.1" "log=1" "startup=./setting/server.startup"
start "Agent" "KFStartupd.exe" "appid=100.0.3.1" "log=1" "startup=./setting/agent.startup"