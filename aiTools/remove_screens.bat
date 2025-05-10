@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION

REM --- Configuration ---
set "TargetFolder=..\build\x64-RelWithDebInfo"
REM --- End Configuration ---

ECHO ===============================================
ECHO  PNG File Deletion Script
ECHO ===============================================
ECHO.
ECHO Target Base Folder: "%TargetFolder%"
ECHO.
ECHO This script will search for all direct subfolders within the target folder
ECHO (e.g., 33005, 33006) and attempt to delete ALL .png files inside each one.
ECHO.
ECHO WARNING: This action is PERMANENT and cannot be undone!
ECHO.
PAUSE Press Ctrl+C to CANCEL, or any other key to CONTINUE...
ECHO.

IF NOT EXIST "%TargetFolder%\" (
    ECHO ERROR: The target folder "%TargetFolder%" was not found.
    ECHO Please check the path in the script.
    PAUSE
    EXIT /B 1
)

ECHO Starting deletion process...
ECHO.

FOR /D %%D IN ("%TargetFolder%\*") DO (
    ECHO Checking folder: "%%~nxD"
    IF EXIST "%%D\*.png" (
        ECHO   Deleting PNG files in "%%~fD"...
        DEL /Q /F "%%D\*.png" 2>nul
        IF ERRORLEVEL 1 (
            ECHO     Warning: Could not delete some files in "%%~fD". Check permissions.
        ) ELSE (
            ECHO     PNG files deleted successfully.
        )
    ) ELSE (
        ECHO   No PNG files found in "%%~fD". Skipping.
    )
)

ECHO.
ECHO ===============================================
ECHO  Deletion process finished.
ECHO ===============================================
PAUSE
EXIT /B 0
