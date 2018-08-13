
echo Starting Deploy
start "Deploy" "KFStartupd.exe" "appid=1" "log=1" "startup=./setting/server.startup"
start "Agent" "KFStartupd.exe" "appid=101" "log=1" "startup=./setting/agent.startup"