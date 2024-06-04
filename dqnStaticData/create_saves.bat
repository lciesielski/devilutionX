@echo off

REM Create the first 4 folders
for /L %%i in (5,1,9) do (
    mkdir "Saves\3300%%i"
)

REM Create the remaining folders
for /L %%i in (10,1,50) do (
    mkdir "Saves\330%%i"
)

echo Folder structure created successfully.
pause