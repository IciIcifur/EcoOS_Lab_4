@echo off
setlocal

echo Build Eco.TaskScheduler1Lab (Windows)
echo ECO_FRAMEWORK=%ECO_FRAMEWORK%
echo ECO_FRAMEWORK_RT=%ECO_FRAMEWORK_RT%

if "%ECO_FRAMEWORK%"=="" (
  echo [ERROR] ECO_FRAMEWORK is not set.
  exit /b 1
)
if "%ECO_FRAMEWORK_RT%"=="" (
  echo [ERROR] ECO_FRAMEWORK_RT is not set.
  exit /b 1
)

REM Сборка динамической Debug DLL
call make clean -f Makefile TARGET=0 DEBUG=1
if errorlevel 1 echo [WARN] clean had issues, continuing...

call make -f Makefile TARGET=0 DEBUG=1
if errorlevel 1 (
  echo [ERROR] Build failed
  exit /b 1
)

set "OUT_DLL=..\..\..\BuildFiles\EcoOS\windows_x86_64\DynamicDebug\902ABA722D34417BB714322CC761620F.dll"
if not exist "%OUT_DLL%" (
  echo [ERROR] Built DLL not found: %OUT_DLL%
  exit /b 1
)

set "RT_DIR=%ECO_FRAMEWORK_RT%\902ABA722D34417BB714322CC761620F"
if not exist "%RT_DIR%" (
  mkdir "%RT_DIR%"
  if errorlevel 1 (
    echo [ERROR] Could not create RT_DIR: %RT_DIR%
    exit /b 1
  )
)

xcopy /Y "%OUT_DLL%" "%RT_DIR%"
if errorlevel 1 (
  echo [ERROR] Copy failed
  exit /b 1
)

echo [OK] Build and copy completed: %RT_DIR%\902ABA722D34417BB714322CC761620F.dll
endlocal