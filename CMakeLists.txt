project(zarlok)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)

set (CMAKE_CXX_FLAGS "-O2 -pipe -Wall -W -DQT_GUI_LIB")
# set (CMAKE_CXX_FLAGS "-march=core2 -O2 -pipe -Wall -W -DQT_GUI_LIB")
# set (CMAKE_EXE_LINKER_FLAGS "-Wl,-O1 -Wl,-rpath,/usr/lib64/qt4")

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} src src/ui src/gui src/db src/models)

set(zarlok_FORMS src/ui/MainWindow.ui src/ui/AddProductsRecordWidget.ui src/ui/AddBatchRecordWidget.ui)
set(zarlok_SRCS src/DataParser.cpp src/models/BatchTableModel.cpp src/models/ProductsTableModel.cpp src/gui/BatchTableView.cpp src/gui/ProductsTableView.cpp src/gui/AddProductsRecordWidget.cpp src/gui/AddBatchRecordWidget.cpp src/db/Database.cpp src/zarlok.cpp main.cpp)

qt4_automoc(${zarlok_SRCS})
qt4_wrap_ui(zarlok_FORMS_HEADERS ${zarlok_FORMS})

add_executable(zarlok ${zarlok_SRCS} ${zarlok_FORMS_HEADERS})

target_link_libraries(zarlok ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTSQL_LIBRARY})

# add_subdirectory(src/ui)
# add_subdirectory(src/gui)

MESSAGE(STATUS "<<< Configuration >>>
Build type      ${CMAKE_BUILD_TYPE}
Install path    ${CMAKE_INSTALL_PREFIX}
Compiler flags:
C               ${CMAKE_C_FLAGS}
C++             ${CMAKE_CXX_FLAGS}
Linker flags:
Executable      ${CMAKE_EXE_LINKER_FLAGS}
Module          ${CMAKE_MODULE_LINKER_FLAGS}
Shared          ${CMAKE_SHARED_LINKER_FLAGS}\n")