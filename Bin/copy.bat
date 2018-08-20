@echo off

set path1=H:\Work\Server\Bin
set path2=.
xcopy /y %path1%\*.bat %path2%\

if not exist deploy (mkdir deploy)
xcopy /y %path1%\deploy\*.bat %path2%\deploy

if not exist auth (mkdir auth)
xcopy /y %path1%\auth\*.bat %path2%\auth

if not exist battle (mkdir battle)
xcopy /y %path1%\battle\*.bat %path2%\battle

if not exist data (mkdir data)
xcopy /y %path1%\data\*.bat %path2%\data

if not exist relation (mkdir relation)
xcopy /y %path1%\relation\*.bat %path2%\relation

if not exist group (mkdir group)
xcopy /y %path1%\group\*.bat %path2%\group

if not exist mail (mkdir mail)
xcopy /y %path1%\mail\*.bat %path2%\mail

if not exist match (mkdir match)
xcopy /y %path1%\match\*.bat %path2%\match

if not exist log (mkdir log)
xcopy /y %path1%\log\*.bat %path2%\log

if not exist public (mkdir public)
xcopy /y %path1%\public\*.bat %path2%\public

if not exist robot (mkdir robot)
xcopy /y %path1%\robot\*.bat %path2%\robot

if not exist route (mkdir route)
xcopy /y %path1%\route\*.bat %path2%\route

if not exist guild (mkdir guild)
xcopy /y %path1%\guild\*.bat %path2%\guild

if not exist rank (mkdir rank)
xcopy /y %path1%\rank\*.bat %path2%\rank

if not exist dir (mkdir dir)
xcopy /y %path1%\dir\*.bat %path2%\dir

if not exist zone (mkdir zone)
xcopy /y %path1%\zone\*.bat %path2%\zone
