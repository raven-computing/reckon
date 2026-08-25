@echo off
set TARGET=all
REM Second DOS Batch sample
if "%1"=="" set TARGET=default
echo Target: %TARGET%
call :done
:done
