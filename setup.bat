@ECHO OFF
SETLOCAL
SET ROOT="%cd%"
SET VSWHERE="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
FOR /f "usebackq delims=" %%i IN (`%VSWHERE% -prerelease -latest -property installationPath`) DO (
  IF EXIST "%%i\Common7\Tools\vsdevcmd.bat" (
    CALL "%%i\Common7\Tools\vsdevcmd.bat"
  )
)
IF NOT EXIST Catch2-2.6.1 (
  git clone --branch v2.6.1 https://github.com/catchorg/Catch2.git Catch2-2.6.1
)
IF NOT EXIST pybind11-2.2.4 (
  git clone --branch v2.2.4 https://github.com/pybind/pybind11.git pybind11-2.2.4
)
IF NOT EXIST dlib-19.17 (
  wget https://github.com/davisking/dlib/archive/v19.17.zip -O dlib-v19.17.zip --no-check-certificate
  unzip dlib-v19.17.zip
  PUSHD dlib-19.17
  MD Build
  MD Install
  PUSHD Build
  cmake ../dlib -DCMAKE_INSTALL_PREFIX=../Install -DENABLE_ASSERTS=OFF
  cmake --build . --config Debug --target install
  cmake --build . --config Release --target install
  POPD
  POPD
)
ENDLOCAL
