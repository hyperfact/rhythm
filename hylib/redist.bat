@echo off

SET TARGET=.\redist

echo Deleting olds...
rd /S /Q %TARGET%
md %TARGET%

echo Copying includes...
xcopy /Y /E .\include\*.* %TARGET%

::echo Copying sources...
::xcopy /Y /E .\src\*.* %TARGET%

echo Copying tinyxml includes...
md %TARGET%\tinyxml
xcopy /Y /E .\tinyxml\*.h %TARGET%\tinyxml

echo Copying loki includes...
md %TARGET%\loki
xcopy /Y /E .\loki\include\noncc\*.h %TARGET%\loki

echo Copying net includes...
md %TARGET%\net
xcopy /Y /E .\net\*.h %TARGET%\net

echo Copying lib(s)...
xcopy /Y /E .\bin\*.lib %TARGET%

echo Copying dll(s)...
xcopy /Y /E .\bin\*.dll %TARGET%

echo Copying configs(s)...
xcopy /Y /E .\bin\*.xml %TARGET%
xcopy /Y /E .\bin\*.ini %TARGET%

echo Redistributing finished!
pause