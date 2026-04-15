Build the real project
`cmake -B "./build/MSVC" -G "Visual Studio 17 2022"`
Create the compile_commands.json for clangd
`cmake -B "./build/Makefile" -G "MinGW Makefiles"`

# VSC
On selecting mingw:
```
[driver] Switching to configure preset: Makefile
[driver] Unsetting test preset
[driver] Switching to build preset: Makefile
[driver] Switching to test preset: __defaultTestPreset__
[driver] Switching to package preset: __defaultPackagePreset__
[driver] Switching to workflow preset: __defaultWorkflowPreset__
[main] Configuring project: Rendering Project
[proc] Executing command: "C:\Program Files\CMake\bin\cmake.EXE" -S "D:/#Documents/#CompSci/C++/Rendering Project" -B "D:/#Documents/#CompSci/C++/Rendering Project/build/Makefile" -G "MinGW Makefiles"
```
And then the compile_commands.json file was made

Building with mingw:
```
[main] Building folder: D:/#Documents/#CompSci/C++/Rendering Project/build/Makefile
[build] Starting build
[proc] Executing command: "C:\Program Files\CMake\bin\cmake.EXE" --build "D:/#Documents/#CompSci/C++/Rendering Project/build/Makefile"
```
And then it didn't find the SFML stuff

On selecting vs 2022 x64:
```
[driver] Switching to configure preset: MSVC
[driver] Unsetting test preset
[driver] Switching to build preset: MSVC-debug
[driver] Switching to test preset: __defaultTestPreset__
[driver] Switching to package preset: __defaultPackagePreset__
[driver] Switching to workflow preset: __defaultWorkflowPreset__
[main] Configuring project: Rendering Project
[proc] Executing command: "C:\Program Files\CMake\bin\cmake.EXE" "-DCMAKE_INSTALL_PREFIX=D:/#Documents/#CompSci/C++/Rendering Project/build/install/MSVC" -DCMAKE_C_COMPILER=cl.exe -DCMAKE_CXX_COMPILER=cl.exe -S "D:/#Documents/#CompSci/C++/Rendering Project" -B "D:/#Documents/#CompSci/C++/Rendering Project/build/MSVC" -G "Visual Studio 17 2022" -T host=x64 -A x64
```
And then it populated some stuff in `build/MSVC`

Building with vs 2022 x64:
```
[main] Building folder: D:/#Documents/#CompSci/C++/Rendering Project/build/MSVC
[build] Starting build
[proc] Executing command: "C:\Program Files\CMake\bin\cmake.EXE" --build "D:/#Documents/#CompSci/C++/Rendering Project/build/MSVC" --config Debug
```
And it worked

# Tests
So it seems like I need the following three commands:
- To create `compile_commands.json`:
	- "C:\Program Files\CMake\bin\cmake.EXE" -S "D:/#Documents/#CompSci/C++/Rendering Project" -B "D:/#Documents/#CompSci/C++/Rendering Project/build/Makefile" -G "MinGW Makefiles"
	- Worked!
- To set up MSVC:
	- "C:\Program Files\CMake\bin\cmake.EXE" "-DCMAKE_INSTALL_PREFIX=D:/#Documents/#CompSci/C++/Rendering Project/build/install/MSVC" -DCMAKE_C_COMPILER=cl.exe -DCMAKE_CXX_COMPILER=cl.exe -S "D:/#Documents/#CompSci/C++/Rendering Project" -B "D:/#Documents/#CompSci/C++/Rendering Project/build/MSVC" -G "Visual Studio 17 2022" -T host=x64 -A x64
	- Worked!
- To build with MSVC:
	- "C:\Program Files\CMake\bin\cmake.EXE" --build "D:/#Documents/#CompSci/C++/Rendering Project/build/MSVC" --config Debug
	- Worked!

Now to try simplifying them:
**Assume we are in "D:/#Documents/#CompSci/C++/Rendering Project", aka the main project directory**
### "C:\Program Files\CMake\bin\cmake.EXE" -S "D:/#Documents/#CompSci/C++/Rendering Project" -B "D:/#Documents/#CompSci/C++/Rendering Project/build/Makefile" -G "MinGW Makefiles"
- cmake -S "D:/#Documents/#CompSci/C++/Rendering Project" -B "D:/#Documents/#CompSci/C++/Rendering Project/build/Makefile" -G "MinGW Makefiles"
- cmake -S . -B "D:/#Documents/#CompSci/C++/Rendering Project/build/Makefile" -G "MinGW Makefiles"
- cmake -S . -B "./build/Makefile" -G "MinGW Makefiles"
- cmake -S . -B ./build/Makefile -G "MinGW Makefiles"
Working!

### "C:\Program Files\CMake\bin\cmake.EXE" "-DCMAKE_INSTALL_PREFIX=D:/#Documents/#CompSci/C++/Rendering Project/build/install/MSVC" -DCMAKE_C_COMPILER=cl.exe -DCMAKE_CXX_COMPILER=cl.exe -S "D:/#Documents/#CompSci/C++/Rendering Project" -B "D:/#Documents/#CompSci/C++/Rendering Project/build/MSVC" -G "Visual Studio 17 2022" -T host=x64 -A x64
- cmake "-DCMAKE_INSTALL_PREFIX=D:/#Documents/#CompSci/C++/Rendering Project/build/install/MSVC" -DCMAKE_C_COMPILER=cl.exe -DCMAKE_CXX_COMPILER=cl.exe -S "D:/#Documents/#CompSci/C++/Rendering Project" -B "D:/#Documents/#CompSci/C++/Rendering Project/build/MSVC" -G "Visual Studio 17 2022" -T host=x64 -A x64
- cmake "-DCMAKE_INSTALL_PREFIX=D:/#Documents/#CompSci/C++/Rendering Project/build/install/MSVC" -DCMAKE_C_COMPILER=cl.exe -DCMAKE_CXX_COMPILER=cl.exe -S . -B "./build/MSVC" -G "Visual Studio 17 2022" -T host=x64 -A x64
- cmake "-DCMAKE_INSTALL_PREFIX=./build/install/MSVC" -DCMAKE_C_COMPILER=cl.exe -DCMAKE_CXX_COMPILER=cl.exe -S . -B "./build/MSVC" -G "Visual Studio 17 2022" -T host=x64 -A x64
- cmake "-DCMAKE_INSTALL_PREFIX=./build/install/MSVC" -DCMAKE_C_COMPILER=cl.exe -DCMAKE_CXX_COMPILER=cl.exe -S . -B ./build/MSVC -G "Visual Studio 17 2022" -T host=x64 -A x64
- cmake -DCMAKE_INSTALL_PREFIX=./build/install/MSVC -DCMAKE_C_COMPILER=cl.exe -DCMAKE_CXX_COMPILER=cl.exe -S . -B ./build/MSVC -G "Visual Studio 17 2022" -T host=x64 -A x64
- cmake -DCMAKE_INSTALL_PREFIX=./build/install/MSVC -DCMAKE_CXX_COMPILER=cl.exe -S . -B ./build/MSVC -G "Visual Studio 17 2022" -T host=x64 -A x64
- cmake -DCMAKE_CXX_COMPILER=cl.exe -S . -B ./build/MSVC -G "Visual Studio 17 2022" -T host=x64 -A x64
- cmake -S . -B ./build/MSVC -G "Visual Studio 17 2022" -T host=x64 -A x64
- cmake -S . -B ./build -G "Visual Studio 17 2022" -T host=x64 -A x64
Working!

### "C:\Program Files\CMake\bin\cmake.EXE" --build "D:/#Documents/#CompSci/C++/Rendering Project/build/MSVC" --config Debug
- cmake --build "D:/#Documents/#CompSci/C++/Rendering Project/build/MSVC" --config Debug
- cmake --build "./build/MSVC" --config Debug
- cmake --build ./build/MSVC --config Debug
- cmake --build ./build --config Debug
Working!
