@echo off
set "sourceFile=.\single_0.sv"
set "destinationFolder=."

for /d %%i in ("%destinationFolder%\*") do (
    copy "%sourceFile%" "%%i\"
)

echo Files copied successfully.
pause
