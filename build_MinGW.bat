mkdir build
cd build

cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%UserProfile%/zarlok -DCMAKE_BUILD_TYPE=Release ..
mimgw32-make
mingw32-make install

set PATH=%UserProfile%/zarlok;%PATH%
echo %PATH%
