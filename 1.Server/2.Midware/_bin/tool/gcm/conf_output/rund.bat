echo Starting [zone.master] server
start "zone" bin\KFStartupd.exe appname=zone apptype=master appid=1.1.1.1 log=1 service=1.1

echo Starting [zone.gate] server
start "zone" bin\KFStartupd.exe appname=zone apptype=gate appid=1.3.1.1 log=1 service=1.1

echo Starting [zone.world] server
start "zone" bin\KFStartupd.exe appname=zone apptype=world appid=1.2.1.1 log=1 service=1.1

echo Starting [zone.game] server
start "zone" bin\KFStartupd.exe appname=zone apptype=game appid=1.5.1.1 log=1 service=1.1

echo Starting [zone.login] server
start "zone" bin\KFStartupd.exe appname=zone apptype=login appid=1.4.1.1 log=1 service=1.1

