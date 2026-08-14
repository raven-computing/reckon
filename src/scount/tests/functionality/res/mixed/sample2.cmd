@echo off
set TARGET=all
if "%1"=="" set TARGET=default
echo Target: %TARGET%
call :done
:done
