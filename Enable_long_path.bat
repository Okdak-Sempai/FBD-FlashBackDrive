@echo off
reg add "HKLM\SYSTEM\CurrentControlSet\Control\FileSystem" /v LongPathsEnabled /t REG_DWORD /d 1 /f
echo Long paths have been enabled. You may need to restart your computer for the changes to take effect.
pause