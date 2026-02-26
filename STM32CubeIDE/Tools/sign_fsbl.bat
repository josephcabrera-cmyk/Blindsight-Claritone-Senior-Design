@echo off
setlocal

REM --- Locate SigningTool ---
set "SIGNTOOL="

REM 1) If already on PATH
where STM32_SigningTool_CLI.exe >nul 2>nul
if %ERRORLEVEL%==0 (
  set "SIGNTOOL=STM32_SigningTool_CLI.exe"
)

REM 2) Default install path (most common)
if not defined SIGNTOOL if exist "C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_SigningTool_CLI.exe" (
  set "SIGNTOOL=C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_SigningTool_CLI.exe"
)

REM 3) Program Files (x86) fallback
if not defined SIGNTOOL if exist "C:\Program Files (x86)\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_SigningTool_CLI.exe" (
  set "SIGNTOOL=C:\Program Files (x86)\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_SigningTool_CLI.exe"
)

if not defined SIGNTOOL (
  echo ERROR: Signing tool not found: "STM32_SigningTool_CLI.exe"
  echo Install STM32CubeProgrammer or add its bin folder to PATH.
  exit /b 1
)

set "KEYDIR=%~dp0..\STM32_KeyGen"

if not exist "%SIGNTOOL%" (
  echo ERROR: Signing tool not found: "%SIGNTOOL%"
  exit /b 1
)

if not exist "%KEYDIR%\privateKey.pem" (
  echo ERROR: Missing private key: "%KEYDIR%\privateKey.pem"
  exit /b 1
)

if not exist "%KEYDIR%\publicKey.pem" (
  echo ERROR: Missing public key: "%KEYDIR%\publicKey.pem"
  exit /b 1
)

if not exist "%~1" (
  echo ERROR: Input bin not found: "%~1"
  exit /b 1
)

echo Signing FSBL...
echo Input : %~1
echo Output: %~2
echo Using public key: %KEYDIR%\publicKey.pem

if exist "%~2" del /f /q "%~2"

"%SIGNTOOL%" ^
  -hv 2.3 -align ^
  -t fsbl ^
  -la 0x20002000 -ep 0x20002001 ^
  -iv 1 ^
  -prvk "%KEYDIR%\privateKey.pem" ^
  -pubk ^
    "%KEYDIR%\publicKey.pem" ^
    "%KEYDIR%\publicKey.pem" ^
    "%KEYDIR%\publicKey.pem" ^
    "%KEYDIR%\publicKey.pem" ^
    "%KEYDIR%\publicKey.pem" ^
    "%KEYDIR%\publicKey.pem" ^
    "%KEYDIR%\publicKey.pem" ^
    "%KEYDIR%\publicKey.pem" ^
  -bin "%~1" ^
  -o "%~2"

exit /b %ERRORLEVEL%