@echo off
set /p path="Enter the path: "
for %%i in (%path%\*.jpg) do echo %%~ni >> list.txt
echo Finish.
pause