EM THIS BATCH SCRIOT ISN'T UP TO DATE WITH THE REST OF THE SOFTWARE NORE IS IT COMPLETE, PLEASE BE PATIENT <3
@echo off

NET SESSION >nul 2>&1
IF %ERRORLEVEL% NEQ 0 (
    echo Restarting with administrator rights...
    powershell -Command "Start-Process cmd.exe -ArgumentList '/c, %~s0' -Verb RunAs"
    exit /b
)

set NASM_URL=https://www.nasm.us/pub/nasm/releasebuilds/2.15.05/win64/nasm-2.15.05-installer.exe
set QEMU_URL=https://qemu.weilnetz.de/w64/qemu-w64-setup-20220119.exe
set INSTALL_DIR=C:\Tools

echo Creating installation directory: %INSTALL_DIR%
mkdir "%INSTALL_DIR%" 2>nul

echo Checking if NASM is installed...
where nasm >nul 2>&1
IF %ERRORLEVEL% NEQ 0 (
    echo NASM not found. Installing NASM...
    powershell -Command "Invoke-WebRequest -Uri %NASM_URL% -OutFile '%INSTALL_DIR%\nasm_installer.exe'"
    echo Running NASM installer...
    start /wait %INSTALL_DIR%\nasm_installer.exe
    echo NASM installation complete.
) ELSE (
    echo NASM is already installed.
)

echo Checking if QEMU is installed...
where qemu-system-x86_64 >nul 2>&1
IF %ERRORLEVEL% NEQ 0 (
    echo QEMU not found. Installing QEMU...
    powershell -Command "Invoke-WebRequest -Uri %QEMU_URL% -OutFile '%INSTALL_DIR%\qemu_installer.exe'"
    echo Running QEMU installer...
    start /wait %INSTALL_DIR%\qemu_installer.exe
    echo QEMU installation complete.
) ELSE (
    echo QEMU is already installed.
)

:echo Running the boot sector overwrite script...
:call run_boot_script.bat


:IF EXIST setup_kernel.bat (
:    echo Running setup_kernel.bat script...
:    call setup_kernel.bat
:) ELSE (
:    echo setup_kernel.bat not found. Skipping.
:)

echo DONE.
pause
exit
