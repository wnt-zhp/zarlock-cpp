project(zarlok)
cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_FLAGS "-O2 -pipe -W -DQT_GUI_LIB")
# set (CMAKE_CXX_FLAGS "-march=core2 -O2 -pipe -Wall -W -DQT_GUI_LIB")
# set (CMAKE_EXE_LINKER_FLAGS "-Wl,-O1 -Wl,-rpath,/usr/lib64/qt4")
add_definitions(-Wall)
find_package(Qt4 REQUIRED)

find_path(GMP_INCLUDE_DIR gmp.h)
find_path(GMPXX_INCLUDE_DIR gmpxx.h)
find_library(GMP_LIBRARY NAMES gmp)
find_library(GMPXX_LIBRARY NAMES gmpxx)

if (GMP_INCLUDE_DIR AND GMP_LIBRARY AND GMPXX_INCLUDE_DIR AND GMPXX_LIBRARY)
	set (GMP_FOUND TRUE)
endif (GMP_INCLUDE_DIR AND GMP_LIBRARY AND GMPXX_INCLUDE_DIR AND GMPXX_LIBRARY)

if (GMP_FOUND)
	if (NOT GMP_FIND_QUIETLY)
		MESSAGE(STATUS "Found GMP: ${GMP_LIBRARY}")
	endif (NOT GMP_FIND_QUIETLY)
else (GMP_FOUND)
	if (GMP_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could not find GMP")
	endif (GMP_FIND_REQUIRED)
endif (GMP_FOUND)

# QT4_ADD_BINRESOURCES(outfiles inputfile ... )
# Oryginal QT4_ADD_RESOURCES Macro modified
# by me for special purpose
macro (QT4_ADD_BINRESOURCES outfiles )
  QT4_EXTRACT_OPTIONS(rcc_files rcc_options ${ARGN})

  FOREACH (it ${rcc_files})
    GET_FILENAME_COMPONENT(outfilename ${it} NAME_WE)
    GET_FILENAME_COMPONENT(infile ${it} ABSOLUTE)
    GET_FILENAME_COMPONENT(rc_path ${infile} PATH)
    SET(outfile ${CMAKE_CURRENT_BINARY_DIR}/${outfilename}.rcc)
    #  parse file for dependencies 
    #  all files are absolute paths or relative to the location of the qrc file
    FILE(READ "${infile}" _RC_FILE_CONTENTS)
    STRING(REGEX MATCHALL "<file[^<]+" _RC_FILES "${_RC_FILE_CONTENTS}")
    SET(_RC_DEPENDS)
    FOREACH(_RC_FILE ${_RC_FILES})
      STRING(REGEX REPLACE "^<file[^>]*>" "" _RC_FILE "${_RC_FILE}")
      IF(NOT IS_ABSOLUTE "${_RC_FILE}")
        SET(_RC_FILE "${rc_path}/${_RC_FILE}")
      ENDIF(NOT IS_ABSOLUTE "${_RC_FILE}")
      SET(_RC_DEPENDS ${_RC_DEPENDS} "${_RC_FILE}")
    ENDFOREACH(_RC_FILE)
    ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
      COMMAND ${QT_RCC_EXECUTABLE}
      ARGS ${rcc_options} -binary -name ${outfilename} -o ${outfile} ${infile}
      MAIN_DEPENDENCY ${infile}
      DEPENDS ${_RC_DEPENDS})
    SET(${outfiles} ${${outfiles}} ${outfile})
  ENDFOREACH (it)
endmacro (QT4_ADD_BINRESOURCES)

configure_file(config.h.in config.h @ONLY)
# set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES config.h)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} src src/ui src/gui src/gui/widgets src/gui/items src/gui/dialogs src/gui/views src/gui/inputs src/gui/validators src/db src/models)

set(zarlok_FORMS
									src/ui/MainWindow.ui
									src/ui/DatabaseBrowser.ui
									src/ui/AddProductsRecordWidget.ui
									src/ui/AddBatchRecordWidget.ui
									src/ui/AddDistributorRecordWidget.ui
)
set(zarlok_widget_FORMS
									src/ui/DBItem.ui
									src/ui/TabProductsWidget.ui
									src/ui/TabBatchWidget.ui
									src/ui/TabDistributorWidget.ui
									src/ui/TabMealWidget.ui
									src/ui/MealFoodListItemDataWidget.ui
									src/ui/MealTabInsertWidget.ui
)
set(zarlok_dialogs_FORMS
									src/ui/CampSettingsDialog.ui
)

set(zarlok_db_RESOURCES src/database.qrc)
set(zarlok_db_RESOURCES_RCC database.rcc)
set(zarlok_reports_RESOURCES src/reports.qrc)
set(zarlok_reports_RESOURCES_RCC reports.rcc)
set(zarlok_icons_RESOURCES src/icons.qrc)
set(zarlok_icons_RESOURCES_RCC icons.rcc)

set(zarlok_src_gui_views_SRCS		src/gui/views/BatchTableView.cpp
									src/gui/views/ProductsTableView.cpp
									src/gui/views/DistributorTableView.cpp
									src/gui/views/TableDelegates.cpp
)

set(zarlok_src_gui_inputs_SRCS
									src/gui/inputs/CustomLineEdit.cpp
									src/gui/inputs/DateInput.cpp
									src/gui/inputs/UnitInput.cpp
									src/gui/inputs/PriceInput.cpp
									src/gui/inputs/QtyInput.cpp
									src/gui/inputs/TextInput.cpp
)
set(zarlok_src_gui_widgets_SRCS
									src/gui/widgets/AddProductsRecordWidget.cpp
									src/gui/widgets/AddBatchRecordWidget.cpp
									src/gui/widgets/AddDistributorRecordWidget.cpp
									src/gui/widgets/DBBrowser.cpp
									src/gui/widgets/DBItemWidget.cpp
									src/gui/widgets/TabProductsWidget.cpp
									src/gui/widgets/TabBatchWidget.cpp
									src/gui/widgets/TabDistributorWidget.cpp
									src/gui/widgets/TabMealWidget.cpp
									src/gui/widgets/MealTabWidget.cpp
									src/gui/widgets/MealFoodList.cpp
									src/gui/widgets/MealTabInsertWidget.cpp
)

set(zarlok_src_gui_dialogs_SRCS
									src/gui/dialogs/CampSettingsDialog.cpp
)
set(zarlok_src_gui_items_SRCS
									src/gui/items/MealFoodListItemDataWidget.cpp
)
set(zarlok_src_models_SRCS
									src/models/ModelsCommon.cpp
									src/models/BatchTableModel.cpp
									src/models/ProductsTableModel.cpp
									src/models/DistributorTableModel.cpp
									src/models/BatchTableModelProxy.cpp
									src/models/BatchTableModelProxyP.cpp
									src/models/MealDayTableModel.cpp
									src/models/MealTableModel.cpp
									src/models/MealTableModelProxy.cpp
									src/models/DistributorTableModelProxyP.cpp
)
set(zarlok_src_db_SRCS
									src/db/DBReports.cpp
									src/db/Database.cpp
									src/db/CampProperties.cpp
)
set(zarlok_src_misc_SRCS src/Logger.cpp src/DataParser.cpp src/zarlok.cpp main.cpp)
set(zarlok_INCS config.h)
set(zarlok_SRCS
									${zarlok_INCS}
									${zarlok_src_gui_inputs_SRCS}
									${zarlok_src_gui_items_SRCS}
									${zarlok_src_gui_views_SRCS}
									${zarlok_src_gui_widgets_SRCS}
									${zarlok_src_gui_dialogs_SRCS}
									${zarlok_src_models_SRCS}
									${zarlok_src_db_SRCS}
									${zarlok_src_misc_SRCS}
)

file (GLOB TS_FILES src/translations/zarlok_*.ts)
set(zarlok_TS ${TS_FILES})

# translations

option (UPDATE_TRANSLATIONS "Update source translation translations/*.ts files (WARNING: make clean will delete the source .ts files! Danger!)")
if (UPDATE_TRANSLATIONS)
  qt4_create_translation(QM_FILES ${zarlok_SRCS} ${zarlok_TS})
else (UPDATE_TRANSLATIONS)
  qt4_add_translation(QM_FILES ${zarlok_TS})
endif (UPDATE_TRANSLATIONS) 

# set(QT_LRELEASE_EXECUTABLE "${QT_LRELEASE_EXECUTABLE} -removeidentical -markuntranslated \"TODO:\"")
add_custom_target(translations_target ALL DEPENDS ${QM_FILES})

qt4_automoc(${zarlok_SRCS})
qt4_wrap_ui(zarlok_FORMS_HEADERS ${zarlok_FORMS} ${zarlok_widget_FORMS} ${zarlok_dialogs_FORMS})
qt4_add_binresources(zarlok_db_RESOURCES_RCC ${zarlok_db_RESOURCES} OPTIONS -binary)
qt4_add_binresources(zarlok_reports_RESOURCES_RCC ${zarlok_reports_RESOURCES} OPTIONS -binary)
qt4_add_binresources(zarlok_icons_RESOURCES_RCC ${zarlok_icons_RESOURCES} OPTIONS -binary)

# add_custom_target(config.h ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/CMakeCache.txt)
add_custom_target(resources ALL DEPENDS ${zarlok_db_RESOURCES_RCC}
	${zarlok_reports_RESOURCES_RCC} ${zarlok_icons_RESOURCES_RCC}
	${CMAKE_CURRENT_SOURCE_DIR}/src/resources/*.tpl )

add_executable(zarlok ${zarlok_SRCS} ${zarlok_FORMS_HEADERS} ${QM})
target_link_libraries(zarlok ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTSQL_LIBRARY} ${GMP_LIBRARY} ${GMPXX_LIBRARY})

# installation
install(FILES
				${CMAKE_CURRENT_BINARY_DIR}/database.rcc
				${CMAKE_CURRENT_BINARY_DIR}/reports.rcc
				${CMAKE_CURRENT_BINARY_DIR}/icons.rcc
				DESTINATION share/${CMAKE_PROJECT_NAME}/resources
)

install(FILES
				${QM_FILES}
				DESTINATION share/${CMAKE_PROJECT_NAME}/translations
)

install(TARGETS zarlok DESTINATION bin)

# messages
MESSAGE(STATUS "<<< Configuration >>>
Project:	${CMAKE_PROJECT_NAME}

Build type      ${CMAKE_BUILD_TYPE}
Install path    ${CMAKE_INSTALL_PREFIX}
Compiler:
C				${CMAKE_C_COMPILER}
C++				${CMAKE_CXX_COMPILER}
Linker:
Ld				${CMAKE_LINKER}
Compiler flags:
C               ${CMAKE_C_FLAGS}
C++             ${CMAKE_CXX_FLAGS}
Linker flags:
Executable      ${CMAKE_EXE_LINKER_FLAGS}
Module          ${CMAKE_MODULE_LINKER_FLAGS}
Shared          ${CMAKE_SHARED_LINKER_FLAGS}\n")
