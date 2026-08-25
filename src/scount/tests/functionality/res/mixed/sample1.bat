@echo off
REM First DOS Batch sample
set BUILD=release
echo Building %BUILD%
if "%1"=="clean" echo Cleaning
exit /b 0
