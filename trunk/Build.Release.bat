call build.bat Release x64 LevelZap
call build.bat Release Win32 LevelZap
@if %ERRORLEVEL% equ 0 (@echo All builds succeeded)
pause