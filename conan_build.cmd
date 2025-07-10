FOR %%b IN (Debug Release RelWithDebInfo) DO (
    mkdir build\%%b
    ECHO building for %%b
    conan install . --output-folder=build/%%b --build=missing --settings=build_type=%%b
    cd build\%%b
    IF ERRORLEVEL 1 GOTO :EOF
    REM Change the visual studio version with the correct values on the next line
    cmake ../.. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -G "Visual Studio 17 2022"
    cmake --build . --config %%b -j
    cd ..\..
    IF ERRORLEVEL 1 GOTO :EOF
)