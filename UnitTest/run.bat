@echo off
setlocal

REM Проверка ECO_FRAMEWORK
if "%ECO_FRAMEWORK%"=="" (
  echo [ERROR] ECO_FRAMEWORK is not set.
  exit /b 1
)

REM Собираем DLL
pushd "..\AssemblyFiles\EcoOS\aarch64_gcc_13_2_1"
call build.bat
if errorlevel 1 (
  echo [ERROR] build.bat failed
  popd
  exit /b 1
)
popd

REM Собираем UnitTest и кладём exe рядом с DLL
set "OUTDIR=..\BuildFiles\EcoOS\windows_x86_64\DynamicDebug"
pushd "."
make
if errorlevel 1 (
  echo [ERROR] UnitTest build failed
  popd
  exit /b 1
)
popd

REM Запуск exe из папки DynamicDebug
pushd "%OUTDIR%"
lab4_unit_test.exe
set ERR=%ERRORLEVEL%
popd

if not "%ERR%"=="0" (
  echo [WARN] UnitTest returned %ERR%
) else (
  echo [OK] UnitTest finished successfully
)

endlocal