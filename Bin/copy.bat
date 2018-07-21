@echo off

set path1=E:\Work\Server\Bin
set path2=.
xcopy /y %path1%\*.bat %path2%\

if not exist deploy (mkdir deploy)
xcopy /y %path1%\deploy\*.bat %path2%\deploy

if not exist auth (mkdir auth)
xcopy /y %path1%\auth\*.bat %path2%\auth

if not exist battlecluster (mkdir battlecluster)
xcopy /y %path1%\battlecluster\*.bat %path2%\battlecluster

if not exist datacluster (mkdir datacluster)
xcopy /y %path1%\datacluster\*.bat %path2%\datacluster

if not exist relationcluster (mkdir relationcluster)
xcopy /y %path1%\relationcluster\*.bat %path2%\relationcluster

if not exist groupcluster (mkdir groupcluster)
xcopy /y %path1%\groupcluster\*.bat %path2%\groupcluster

if not exist mailcluster (mkdir mailcluster)
xcopy /y %path1%\mailcluster\*.bat %path2%\mailcluster

if not exist matchcluster (mkdir matchcluster)
xcopy /y %path1%\matchcluster\*.bat %path2%\matchcluster

if not exist logcluster (mkdir logcluster)
xcopy /y %path1%\logcluster\*.bat %path2%\logcluster

if not exist platform (mkdir platform)
xcopy /y %path1%\platform\*.bat %path2%\platform

if not exist publiccluster (mkdir publiccluster)
xcopy /y %path1%\publiccluster\*.bat %path2%\publiccluster

if not exist robot (mkdir robot)
xcopy /y %path1%\robot\*.bat %path2%\robot

if not exist routecluster (mkdir routecluster)
xcopy /y %path1%\routecluster\*.bat %path2%\routecluster

if not exist guildcluster (mkdir guildcluster)
xcopy /y %path1%\routecluster\*.bat %path2%\guildcluster

if not exist rankcluster (mkdir rankcluster)
xcopy /y %path1%\routecluster\*.bat %path2%\rankcluster

if not exist zone (mkdir zone)
xcopy /y %path1%\zone\*.bat %path2%\zone
