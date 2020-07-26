@echo off
set /p path="Enter the path: "
for %%i in (%path%\*.jpg) do echo %path%\%%i >> train.txt
echo Finish.
pause