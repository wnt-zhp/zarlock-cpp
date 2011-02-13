set zarlok_src=`pwd`
mkdir %zarlok_src%/build
cd %zarlok_src%/build
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%UserProfile%/zarlok -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER="C:/MinGW/bin/gcc" -DCMAKE_CXX_COMPILER="C:/MinGW/bin/g++.exe" ..
gmake
gmake install
set PATH=%UserProfile%/zarlok;%PATH%
echo %PATH%

