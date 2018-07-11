
echo Starting Deploy
start "Deploy" "KFStartupd.exe" "0" "1" "./setting/server.startup"
start "Agent" "KFStartupd.exe" "0" "101" "./setting/agent.startup"