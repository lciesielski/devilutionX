@echo off
setlocal enabledelayedexpansion

REM Set base path relative to the current directory (dqnStaticData)
set "BUILD_RELEASE_DIR=..\build\x64-RelWithDebInfo"
set "SAVES_BASE_DIR=%BUILD_RELEASE_DIR%\Saves"
set "CONFIG_DIR=%BUILD_RELEASE_DIR%\Config"

REM Source files (expected in the current directory)
set "SOURCE_SAVE_FILE=single_0.sv"
set "SOURCE_CONFIG_FILE=diablo.ini"

REM Check if source files exist
if not exist "%SOURCE_SAVE_FILE%" (
	echo ERROR: Source save file "%SOURCE_SAVE_FILE%" not found in current directory.
	pause
	exit /b 1
)
if not exist "%SOURCE_CONFIG_FILE%" (
	echo ERROR: Source config file "%SOURCE_CONFIG_FILE%" not found in current directory.
	pause
	exit /b 1
)

echo Creating Save directories and copying %SOURCE_SAVE_FILE%...

REM Loop from 33005 to 33055
for /L %%G in (33005, 1, 33055) do (
	set "TARGET_SAVE_DIR=%SAVES_BASE_DIR%\%%G"
	echo Creating directory: "!TARGET_SAVE_DIR!"
	mkdir "!TARGET_SAVE_DIR!" > nul 2>&1
	if errorlevel 1 (
		echo   WARN: Directory may already exist or could not be created.
	)

	echo Copying %SOURCE_SAVE_FILE% to "!TARGET_SAVE_DIR!"
	copy "%SOURCE_SAVE_FILE%" "!TARGET_SAVE_DIR!\" > nul
	if errorlevel 1 (
		echo   ERROR: Failed to copy %SOURCE_SAVE_FILE% to "!TARGET_SAVE_DIR!"
	)
)

echo.
echo Creating Config directory...
echo Creating directory: "%CONFIG_DIR%"
mkdir "%CONFIG_DIR%" > nul 2>&1
if errorlevel 1 (
	echo   WARN: Directory may already exist or could not be created.
)

echo Copying %SOURCE_CONFIG_FILE% to "%CONFIG_DIR%"
copy "%SOURCE_CONFIG_FILE%" "%CONFIG_DIR%\" > nul
if errorlevel 1 (
	echo   ERROR: Failed to copy %SOURCE_CONFIG_FILE% to "%CONFIG_DIR%"
)

echo.
echo Script finished.
pause
endlocal
exit /b 0
