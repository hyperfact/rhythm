@echo off

SET TARGET=.\redist_src

echo Deleting olds...
rd /S /Q %TARGET%
md %TARGET%

echo Copying includes...
xcopy /Y /E .\include\*.* %TARGET%

echo Copying sources...
xcopy /Y /E .\src\*.* %TARGET%

echo Copying tinyxml ...
md %TARGET%\tinyxml
xcopy /Y /E .\tinyxml\*.* %TARGET%\tinyxml

echo Copying loki ...
md %TARGET%\loki
md %TARGET%\loki\include
md %TARGET%\loki\src
xcopy /Y /E .\loki\include\*.* %TARGET%\loki\include
xcopy /Y /E .\loki\src\*.* %TARGET%\loki\src

echo Copying net ...
md %TARGET%\net
xcopy /Y /E .\net\*.* %TARGET%\net

echo Copying configs(s)...
xcopy /Y /E .\bin\*.xml %TARGET%
xcopy /Y /E .\bin\*.ini %TARGET%

echo Redistributing finished!
pause