@echo off

cd  zone
call kill.bat
cd ..

cd routecluster
call kill.bat
cd ..

cd publiccluster
call kill.bat
cd ..

cd matchcluster
call kill.bat
cd ..

cd battlecluster
call kill.bat
cd ..

cd mailcluster
call kill.bat
cd ..

cd friendcluster
call kill.bat
cd ..

cd groupcluster
call kill.bat
cd ..

cd datacluster
call kill.bat
cd ..

cd rankcluster
call kill.bat
cd ..