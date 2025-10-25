@echo off

:: Fixes Menu
:Menu
cls
echo Choose a fix/mod:
echo 1. (Requires Admin Privileges) Disables Win11's (and Win10's) DogSHIT ^Start Menu Web Search.
echo 2. (Requires Admin Privileges) Completely removes the DogShit Recommended section from the ^Start Menu (Win11 SE/EDU).
echo 3. Disables win 11's DogSHIT Right-Click Menu and uses the old and gold Win10's menu instead.
echo 4. Enables Self-Signed Certificates for BitLocker.
echo 5. Quit the Script.

:: Prompt user with a choice
set /p choice="Pick a fix: "

:: Go to commands 
if %choice% == 1 goto DisableOnlineSearch
if %choice% == 2 goto RemoveRecommendedSection
if %choice% == 3 goto UseOldContextMenu
if %choice% == 4 goto EnableSelfSignedCertificates
if %choice% == 5 goto END

echo Invalid choice. Please try again.
pause
goto Menu


:DisableOnlineSearch
echo Fixing Start Menu Search...
set "tempFile=%temp%\DisableSearch_%random%.bat"
(
    echo @echo off
    echo :: Check if the script is running as administrator
    echo net session ^>nul 2^>^&1 
    echo if %%errorLevel%% neq 0 ^(
    echo     echo Requesting administrative privileges...
    echo     powershell -Command "Start-Process '%%~f0' -Verb RunAs"
    echo     exit
    echo ^)
    echo reg.exe add "HKLM\Software\Policies\Microsoft\Windows\Explorer" /v DisableSearchBoxSuggestions /t REG_DWORD /d 1 /f
    echo pause
    echo exit
) > %tempFile%
start "" "%tempFile%"
pause
del "%tempFile%"
goto Menu

:RemoveRecommendedSection
echo Fixing Start Menu...
set "tempFile=%temp%\RemoveRecommended_%random%.bat"
(
    echo @echo off
    echo :: Check if the script is running as administrator
    echo net session ^>nul 2^>^&1 
    echo if %%errorLevel%% neq 0 ^(
    echo     echo Requesting administrative privileges...
    echo     powershell -Command "Start-Process '%%~f0' -Verb RunAs"
    echo     exit
    echo ^)
    echo reg.exe add "HKLM\SOFTWARE\Policies\Microsoft\Windows\Explorer" /v HideRecommendedSection /t REG_DWORD /d 1 /f
    echo pause
    echo exit
) > %tempFile%
start "" "%tempFile%"
pause
del "%tempFile%"
goto Menu

:UseOldContextMenu
echo Fixing Right-Click Menu...
reg.exe add "HKCU\Software\Classes\CLSID\{86ca1aa0-34aa-4e8b-a509-50c905bae2a2}\InprocServer32" /f /ve
pause
goto Menu

:EnableSelfSignedCertificates
echo Enabling Self-Signed Certificates...
set "tempFile=%temp%\RemoveRecommended_%random%.bat"
(
    echo @echo off
    echo :: Check if the script is running as administrator
    echo net session ^>nul 2^>^&1 
    echo if %%errorLevel%% neq 0 ^(
    echo     echo Requesting administrative privileges...
    echo     powershell -Command "Start-Process '%%~f0' -Verb RunAs"
    echo     exit
    echo ^)
    echo reg.exe add "HKLM\SOFTWARE\Policies\Microsoft\FVE" /v SelfSignedCertificates /t REG_DWORD /d 1 /f
    echo pause
    echo exit
) > %tempFile%
start "" "%tempFile%"
pause
del "%tempFile%"
goto Menu

:END
echo Exiting the script...
exit