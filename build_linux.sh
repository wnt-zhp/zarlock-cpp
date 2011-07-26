#!/bin/bash

export zarlok_src=`pwd`
mkdir ${zarlok_src}/build -p
cd ${zarlok_src}/build
cmake ${zarlok_src} -DCMAKE_INSTALL_PREFIX=${HOME}/usr -DCMAKE_BUILD_TYPE=Release
gmake
gmake install
export PATH=${HOME}/usr/bin:${PATH}
