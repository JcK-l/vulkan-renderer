@echo off
setlocal enabledelayedexpansion

REM This script will rename all files in the current directory with the following format: {number}_{month}_{day}.md
REM It also creates a new file with the format: 1_{month}_{day}.md

REM Get current date
for /f "tokens=2-4 delims=/ " %%a in ('date /t') do (set month=%%a& set day=%%b& set year=%%c)

REM Get the highest number prefix
set highest=0
for /r %%i in (*.md) do (
    for /f "tokens=1 delims=_" %%a in ("%%~nxi") do (
        if %%a gtr !highest! (
            set highest=%%a
        )
    )
)

REM Rename existing files
for /l %%x in (!highest!,-1,1) do (
    for /r %%i in (%%x_*.md) do (
        set /a newnum=%%x+1
        for /f "tokens=2-4 delims=_." %%a in ("%%~nxi") do (
            ren "%%i" "!newnum!_%%a_%%b.%%c"
        )
    )
)

REM Create new file
echo. > "1_%month%_%day%.md"