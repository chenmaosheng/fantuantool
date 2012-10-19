set build=debug
set version=0.10

del /F /S /Q .\Fantuan_v%version%\*.*
rd /S /Q .\Fantuan_v%version%

cd .\FTClient\
xcopy /C /D /H bin\%build%\*.exe ..\Fantuan_v%version%\Client\bin\
xcopy /C /D /H config\*.* ..\Fantuan_v%version%\Client\config\

cd ..\FTServer\
xcopy /C /D /H bin\%build%\*.exe ..\Fantuan_v%version%\Server\bin\%build%\
xcopy /C /D /H bin\%build%\*.dll ..\Fantuan_v%version%\Server\bin\%build%\
xcopy /C /D /H config\*.* ..\Fantuan_v%version%\Server\config\
xcopy /C /D /H Launcher.bat ..\Fantuan_v%version%\Server\
xcopy /C /D /H Shutdown.bat ..\Fantuan_v%version%\Server\