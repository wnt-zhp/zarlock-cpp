project(zarlok)
cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_FLAGS "-O2 -pipe -W -DQT_GUI_LIB -std=c++0x")
# set (CMAKE_CXX_FLAGS "-march=core2 -O2 -pipe -Wall -W -DQT_GUI_LIB")
# set (CMAKE_EXE_LINKER_FLAGS "-Wl,-O1 -Wl,-rpath,/usr/lib64/qt4")
add_definitions(-Wall)

SET( QT_USE_QTCORE TRUE )
SET( QT_USE_QTGUI TRUE )
SET( QT_USE_QTSQL TRUE )

find_package(Qt4 REQUIRED)

# QT4_ADD_BINRESOURCES(outfiles inputfile ... )
# Oryginal QT4_ADD_RESOURCES Macro modified
# by me for special purpose
include (macros/user_macros.txt)

if( UNIX AND NOT APPLE )
	if( NOT DESKTOP_ENTRY )
		set( DESKTOP_ENTRY ${CMAKE_PROJECT_NAME}.desktop )
	endif( NOT DESKTOP_ENTRY )

	add_custom_command( OUTPUT ${DESKTOP_ENTRY}
		COMMAND touch ${DESKTOP_ENTRY}
		COMMAND sh ../${CMAKE_PROJECT_NAME}-desktop.sh ${CMAKE_INSTALL_PREFIX} > ${DESKTOP_ENTRY}
		DEPENDS ${CMAKE_PROJECT_NAME}-desktop.sh
		COMMENT "Generating desktop entry file"
	)
	add_custom_target( DESKTOP_ENTRY_FILE ALL
		DEPENDS ${DESKTOP_ENTRY}
	)
  
	set( APP_ICON ${CMAKE_PROJECT_NAME}.png )
endif( UNIX AND NOT APPLE )

IF( MINGW )
	set( APP_ICON appicon.o )
	set( APP_ICON_RC src/appicon.rc )
	# resource compilation for MinGW
	ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${APP_ICON}
		COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR}
			-i${CMAKE_CURRENT_SOURCE_DIR}/${APP_ICON_RC}
			-o ${CMAKE_CURRENT_BINARY_DIR}/${APP_ICON} )
	SET(appicon_SOURCES ${appicon_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/${APP_ICON})
ELSE( MINGW )
	SET(appicon_SOURCES ${appicon_SOURCES} appico.rc)
ENDIF( MINGW )

if (BUILD_STATIC)
	add_definitions(-DBUILD_STATIC)
endif (BUILD_STATIC)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} src src/misc src/ui src/gui src/gui/widgets src/gui/items src/gui/dialogs src/gui/views src/gui/inputs src/gui/validators src/db src/models)

set(zarlok_FORMS
									src/ui/MainWindow.ui
									src/ui/DatabaseBrowser.ui
									src/ui/ProductsRecordWidget.ui
									src/ui/BatchRecordWidget.ui
									src/ui/DistributorRecordWidget.ui
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

# resources

set(zarlok_db_RESOURCES src/database.qrc)
set(zarlok_reports_RESOURCES src/reports.qrc)
set(zarlok_icons_RESOURCES src/icons.qrc)
set(zarlok_splash_RESOURCES src/splash.qrc)
set(zarlok_translations_RESOURCES src/translations.qrc)

# sources

set(zarlok_src_gui_views_SRCS
									src/gui/views/AbstractTableView.cpp
									src/gui/views/BatchTableView.cpp
									src/gui/views/ProductsTableView.cpp
									src/gui/views/DistributorTableView.cpp
									src/gui/views/MealDaysTableView.cpp
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
									src/gui/widgets/AbstractRecordWidget.cpp
									src/gui/widgets/ProductsRecordWidget.cpp
									src/gui/widgets/BatchRecordWidget.cpp
									src/gui/widgets/DistributorRecordWidget.cpp
									src/gui/widgets/DBBrowser.cpp
									src/gui/widgets/DBItemWidget.cpp
									src/gui/widgets/TabProductsWidget.cpp
									src/gui/widgets/TabBatchWidget.cpp
									src/gui/widgets/TabDistributorWidget.cpp
									src/gui/widgets/TabMealWidget.cpp
									src/gui/widgets/MealTabWidget.cpp
									src/gui/widgets/MealFoodList.cpp
									src/gui/widgets/MealTabInsertWidget.cpp
 									src/gui/widgets/QProgressIndicator.cpp
)

set(zarlok_src_gui_dialogs_SRCS
									src/gui/dialogs/CampSettingsDialog.cpp
									src/gui/dialogs/AbstractDimmingWidget.cpp
									src/gui/dialogs/DimmingMessage.cpp
									src/gui/dialogs/DimmingWidget.cpp
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
									src/models/DistributorTableModelProxy.cpp
									src/models/DistributorTableModelProxyP.cpp
									src/models/AbstractTableModel.cpp
)
set(zarlok_src_db_SRCS
									src/db/DBReports.cpp
									src/db/Database.cpp
									src/db/CampProperties.cpp
									src/db/MealManager.cpp
)

set(zarlok_src_misc_SRCS
									src/misc/ApplicationUpdater.cpp
									src/misc/EventFilter.cpp
									src/misc/Logger.cpp
									src/misc/DataParser.cpp
									src/zarlok.cpp
									main.cpp
)

configure_file(config/config.h.in config.h @ONLY)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version.h
					DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/config/version.h.in
					COMMAND ${CMAKE_COMMAND} -D ZARLOK_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -P ${CMAKE_CURRENT_SOURCE_DIR}/macros/make_version_config.txt
					WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# add_custom_target(vfb DEPENDS version.h SOURCES VersionFileBuilder.cpp version.h)
add_executable(vfb VersionFileBuilder.cpp version.h)
# add_dependencies(vfb version.h)

set(zarlok_INCS config.h version.h)

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

# 
# translations

option (UPDATE_TRANSLATIONS "Update source translation translations/*.ts files (WARNING: make clean will delete the source .ts files! Danger!)")

if (UPDATE_TRANSLATIONS)
  qt4_create_translation(QM_FILES ${zarlok_SRCS} ${zarlok_TS})
else (UPDATE_TRANSLATIONS)
  qt4_add_translation(QM_FILES ${zarlok_TS})
endif (UPDATE_TRANSLATIONS) 

# set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES config.h)

# set(QT_LRELEASE_EXECUTABLE "${QT_LRELEASE_EXECUTABLE} -removeidentical -markuntranslated \"TODO:\"")

qt4_wrap_ui(zarlok_FORMS_HEADERS ${zarlok_FORMS} ${zarlok_widget_FORMS} ${zarlok_dialogs_FORMS})
# qt4_wrap_cpp(${zarlok_SRCS})
qt4_automoc(${zarlok_SRCS})

if (rcc_files)
	unset (rcc_files)
endif (rcc_files)

IF (NOT MINGW)
	add_custom_target(translations_target ALL DEPENDS ${QM_FILES})

	set(zarlok_db_RESOURCES_RCC database.rcc)
	set(zarlok_reports_RESOURCES_RCC reports.rcc)
	set(zarlok_icons_RESOURCES_RCC icons.rcc)
	set(zarlok_splash_RESOURCES_RCC splash.rcc)

	qt4_add_binresources(zarlok_db_RESOURCES_RCC ${zarlok_db_RESOURCES} OPTIONS -binary)
	qt4_add_binresources(zarlok_reports_RESOURCES_RCC ${zarlok_reports_RESOURCES} OPTIONS -binary)
	qt4_add_binresources(zarlok_icons_RESOURCES_RCC ${zarlok_icons_RESOURCES} OPTIONS -binary)
	qt4_add_binresources(zarlok_splash_RESOURCES_RCC ${zarlok_splash_RESOURCES} OPTIONS -binary)

	add_custom_target(resources ALL DEPENDS ${zarlok_db_RESOURCES_RCC}
		${zarlok_reports_RESOURCES_RCC} ${zarlok_icons_RESOURCES_RCC}
		${zarlok_splash_RESOURCES_RCC} )

	add_executable(zarlok ${zarlok_FORMS_HEADERS} ${zarlok_SRCS} ${QM})

ELSE (NOT MINGW)

# 	set(zarlok_translations_RESOURCES_RCC translations.rcc)

	qt4_add_resources(zarlok_db_RESOURCES_RCC ${zarlok_db_RESOURCES})
	qt4_add_resources(zarlok_reports_RESOURCES_RCC ${zarlok_reports_RESOURCES})
	qt4_add_resources(zarlok_icons_RESOURCES_RCC ${zarlok_icons_RESOURCES})
	qt4_add_resources(zarlok_splash_RESOURCES_RCC ${zarlok_splash_RESOURCES})
# 	qt4_add_resources(zarlok_translations_RESOURCES_RCC ${zarlok_translations_RESOURCES})

	SET( trans_file translations )
	SET( trans_qminfile ${QM_FILES})
	SET( trans_qmoutfile ${CMAKE_CURRENT_BINARY_DIR}/src/translations/ )

	SET( trans_srcfile ${CMAKE_CURRENT_SOURCE_DIR}/src/${trans_file}.qrc)
	SET( trans_infile ${CMAKE_CURRENT_BINARY_DIR}/src/${trans_file}.qrc)
	SET( trans_outfile ${CMAKE_CURRENT_BINARY_DIR}/qrc_${trans_file}.cxx)

	ADD_CUSTOM_COMMAND(
		OUTPUT ${trans_qmoutfile}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${trans_qmoutfile}
		COMMAND ${CMAKE_COMMAND} -E copy ${trans_qminfile} ${trans_qmoutfile}
		MAIN_DEPENDENCY ${trans_qminfile}
# 		DEPENDS ${trans_qmoutfile}
	)

	# Copy the QRC file to the output directory
	ADD_CUSTOM_COMMAND(
		OUTPUT ${trans_infile}
		COMMAND ${CMAKE_COMMAND} -E copy ${trans_srcfile} ${trans_infile}
		MAIN_DEPENDENCY ${trans_srcfile}
		DEPENDS ${trans_srcfile} ${trans_qmoutfile}
	)

	# Run the resource compiler (rcc_options should already be set)
	ADD_CUSTOM_COMMAND(
		OUTPUT ${trans_outfile}
		COMMAND ${QT_RCC_EXECUTABLE} ${rcc_options} -name ${trans_file} -o ${trans_outfile} ${trans_infile}
		MAIN_DEPENDENCY ${trans_infile}
# 		DEPENDS ${trans_infile}
	)

	SET( rcc_files ${zarlok_db_RESOURCES_RCC} ${zarlok_reports_RESOURCES_RCC} ${zarlok_icons_RESOURCES_RCC} ${zarlok_splash_RESOURCES_RCC} ${trans_outfile} )

	add_executable(zarlok WIN32 ${APP_ICON} ${zarlok_SRCS} ${zarlok_FORMS_HEADERS} ${rcc_files})

ENDIF (NOT MINGW)

add_dependencies(zarlok vfb)

target_link_libraries(zarlok ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTSQL_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QT_LIBRARY})

if ( MINGW )

find_program(iscc_exe iscc PATHS ${ISCC_BINARY_DIR} ENV PATH)

if (NOT iscc_exe STREQUAL "iscc_exe-NOTFOUND")
	MESSAGE(STATUS "Inno Setup Compiler : " ${iscc_exe})
else (NOT iscc_exe STREQUAL "iscc_exe-NOTFOUND")
	MESSAGE(FATAL_ERROR "iscc not found!")
endif (NOT iscc_exe STREQUAL "iscc_exe-NOTFOUND")

add_custom_command(OUTPUT iss_file
					COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/dist/win32-installer/zarlok_installer.iss.in zarlok_installer.iss.in
					COMMAND ${CMAKE_COMMAND} -D QT_BINARY_DIR=${QT_BINARY_DIR} -D QT_PLUGINS_DIR=${QT_PLUGINS_DIR} -D ZARLOK_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -P ${CMAKE_CURRENT_SOURCE_DIR}/macros/make_installator.txt
					COMMAND ${iscc_exe} zarlok_installer.iss
					WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
					MAIN_DEPENDENCY version.h
					COMMENT "Preparing Inno Setup script file"
)

add_custom_target(package DEPENDS iss_file)

endif ( MINGW )

if ( UNIX AND NOT APPLE )

add_custom_target(distprepare
				COMMAND ${CMAKE_COMMAND} -D ZARLOK_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -P ${CMAKE_CURRENT_SOURCE_DIR}/macros/make_cpack_configuration.txt
				COMMAND ${CMAKE_COMMAND} -D CMAKE_GENERATOR=${CMAKE_GENERATOR} -D ZARLOK_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -P macro_cpack_configuration.txt
				DEPENDS version.h
)

add_dependencies(distprepare vfb)

endif ( UNIX AND NOT APPLE )


if ( UNIX AND NOT APPLE )

install(FILES
				${APP_ICON}
				DESTINATION share/icons
)

install(FILES
				${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_ENTRY}
				DESTINATION share/applications
)

ENDIF ( UNIX AND NOT APPLE )

IF (NOT MINGW)

install(FILES
				${CMAKE_CURRENT_BINARY_DIR}/database.rcc
				${CMAKE_CURRENT_BINARY_DIR}/reports.rcc
				${CMAKE_CURRENT_BINARY_DIR}/icons.rcc
				${CMAKE_CURRENT_BINARY_DIR}/splash.rcc
				DESTINATION share/${CMAKE_PROJECT_NAME}/resources
)

install(FILES
				${QM_FILES}
				DESTINATION share/${CMAKE_PROJECT_NAME}/translations
)

ENDIF (NOT MINGW)
# if(UNIX AND NOT APPLE)
# 	install( FILES ${APP_ICON} DESTINATION share/icons )
# 	install( FILES qtm.1.gz DESTINATION ${MANPAGE_DIRECTORY} )
# endif(UNIX AND NOT APPLE)



add_custom_target(version_number
					COMMAND vfb
					WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_dependencies(zarlok version_number distprepare)

install(TARGETS zarlok DESTINATION bin)

# messages
MESSAGE(STATUS "<<< Configuration >>>
Project:	${CMAKE_PROJECT_NAME}
Generator:	${CMAKE_GENERATOR}

Static build	${BUILD_STATIC}

Source dir		${CMAKE_CURRENT_SOURCE_DIR}
Build dir		${CMAKE_CURRENT_BINARY_DIR}

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
