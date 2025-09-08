REM @echo off
call "%~dp0\vars.bat"

call "%UE5_BUILDTOOL_EXE%" -projectfiles -project="%UPROJECT_PATH%" -game -rocket -progress -log="%ROOTDIR%/Saved/Logs/GenerateProjectFiles.log"
