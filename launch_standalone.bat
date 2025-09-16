@echo off
call build_standalone.bat
if %errorlevel% neq 0 (
    echo Build failed. Exiting...
    exit /b %errorlevel%
)
call run_editor_standalone.bat
