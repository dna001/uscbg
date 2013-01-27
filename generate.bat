cd bin
cmake -G "CodeBlocks - MinGW Makefiles" -D CMAKE_C_COMPILER=C:/dev/MinGW/bin/gcc.exe -D CMAKE_CXX_COMPILER=C:/dev/MinGW/bin/g++.exe -D SDL_INCLUDE_DIR=C:/dev/MinGW/include -D SDLIMAGE_INCLUDE_DIR=C:/dev/MinGW/include -D SDLTTF_INCLUDE_DIR=C:/dev/MinGW/include -DCMAKE_CXX_FLAGS="-DWIN32" -DCMAKE_C_FLAGS="-DWIN32" ..
pause
