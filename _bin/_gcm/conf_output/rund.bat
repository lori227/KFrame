echo Starting [route.shard] server
start "route" bin\KFStartupd.exe app=route.shard id=1.24.0.1 log=1.0 service=1.1

echo Starting [data.shard] server
start "data" bin\KFStartupd.exe app=data.shard id=1.31.0.1 log=1.0 service=1.1

echo Starting [auth.auth] server
start "auth" bin\KFStartupd.exe app=auth.auth id=1.21.0.1 log=1.0 service=1.1

echo Starting [route.master] server
start "route" bin\KFStartupd.exe app=route.master id=1.22.0.1 log=1.0 service=1.1

echo Starting [route.proxy] server
start "route" bin\KFStartupd.exe app=route.proxy id=1.23.0.1 log=1.0 service=1.1

echo Starting [zone.master] server
start "zone" bin\KFStartupd.exe app=zone.master id=1.91.1.1 log=1.0 service=1.1

echo Starting [zone.gate] server
start "zone" bin\KFStartupd.exe app=zone.gate id=1.93.1.1 log=1.0 service=1.1

echo Starting [zone.world] server
start "zone" bin\KFStartupd.exe app=zone.world id=1.92.1.1 log=1.0 service=1.1

echo Starting [zone.game] server
start "zone" bin\KFStartupd.exe app=zone.game id=1.95.1.1 log=1.0 service=1.1

echo Starting [zone.login] server
start "zone" bin\KFStartupd.exe app=zone.login id=1.94.1.1 log=1.0 service=1.1

