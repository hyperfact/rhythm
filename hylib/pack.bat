@echo off

setlocal ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION

set RAR="D:\Program Files\WinRAR\rar.exe" a -ag(YYYYMMDD) -y -r -aphylib
set TARGET=hylib.rar

echo Remove olds...
del /Q hylib(*).rar

echo Packing...
%RAR% %TARGET% hylib.dsw
%RAR% %TARGET% hylib.sln
%RAR% %TARGET% history.txt
%RAR% %TARGET% pack.bat
%RAR% %TARGET% redist.bat
%RAR% %TARGET% redist_src.bat

set EXCLUDE=-x*\Debug -x*\Release -x*\.svn -x*.ncb -x*.tlh -x*.tli -x*.aps -x*.clw -x*.plg -x*.ncb -x*.opt -x*.sdf

:: Sources
%RAR% %EXCLUDE% %TARGET% include
%RAR% %EXCLUDE% %TARGET% src
%RAR% %EXCLUDE% %TARGET% tinyxml
%RAR% %EXCLUDE% %TARGET% loki
%RAR% %EXCLUDE% %TARGET% net

:: Projects
%RAR% %EXCLUDE% %TARGET% static
%RAR% %EXCLUDE% %TARGET% hydll
%RAR% %EXCLUDE% %TARGET% test
%RAR% %EXCLUDE% %TARGET% tools

endlocal

echo Packing finished!

:end
pause