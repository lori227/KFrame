	
echo Starting Platform Server
start "Platform" "KFStartupd.exe" "appid=100.0.5.1" "log=1" "startup=./setting/platform.startup"
start "Platform" "KFStartupd.exe" "appid=100.0.5.2" "log=1" "startup=./setting/platform.startup"