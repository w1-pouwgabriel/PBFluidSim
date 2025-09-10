@echo off
REM -----------------------------------------------------------------------------
REM Generate compile_commands.json for PBFluidSim
REM -----------------------------------------------------------------------------

REM Load environment variables (UE5_DIR, PROJECT, UPROJECT_PATH, etc.)
call "%~dp0\vars.bat"

echo Generating compile_commands.json for %PROJECT% ...

REM Call UnrealBuildTool in GenerateClangDatabase mode
call "%UE5_BUILDTOOL_EXE%" %PROJECT%Editor Win64 Development ^
    -Project="%UPROJECT_PATH%" ^
    -Mode=GenerateClangDatabase ^
    -Game -Engine -Progress ^
    -Log="%ROOTDIR%/Saved/Logs/GenerateCompileCommands.log"

echo Done! If successful, compile_commands.json should be in:
echo   %ROOTDIR%\compile_commands.json
pause
