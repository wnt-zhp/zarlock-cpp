project(zarlok CXX)
cmake_minimum_required(VERSION 2.8)

if(APPLE AND CMAKE_INSTALL_PREFIX MATCHES "/usr/local")
	set(CMAKE_INSTALL_PREFIX "/Applications")
endif()

find_package(Qt4 COMPONENTS QtCore QtGui QtSQL QtNetwork REQUIRED)
include(UseQt4)
# set(CMAKE_AUTOMOC TRUE)

set(PROJECT_VERSION_MAJOR "3")
set(PROJECT_VERSION_MINOR "1")
set(PROJECT_VERSION_PATCH "0")
set(PROJECT_VERSION_COUNT 3)
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(PROJECT_VENDOR "WNT GK ZHP")
set(PROJECT_COPYRIGHT_YEAR "2012")
set(PROJECT_DOMAIN_FIRST "zarlok")
set(PROJECT_DOMAIN_SECOND "zhp")
set(PROJECT_DOMAIN_THIRD "zhp")
set(PROJECT_DOMAIN "${PROJECT_DOMAIN_FIRST}.${PROJECT_DOMAIN_SECOND}.${PROJECT_DOMAIN_THIRD}")

if ( UNIX AND NOT APPLE )
	string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)
	set(BIN_INSTALL_DIR "bin")
	set(DOC_INSTALL_DIR "share/doc/${PROJECT_NAME_LOWERCASE}/")
	set(SHARE_INSTALL_DIR "share/${PROJECT_NAME_LOWERCASE}/")
else ()
	set(BIN_INSTALL_DIR ".")
	set(DOC_INSTALL_DIR ".")
	set(SHARE_INSTALL_DIR ".")
endif ()

if ( CMAKE_COMPILER_IS_GNUCXX OR APPLE )
	add_definitions(-Wall -Wextra)
	if ( CMAKE_BUILD_RELEASE )
		add_definitions(-Werror)
	endif ()
endif ()

add_definitions(-DPROJECT_VERSION=\"${PROJECT_VERSION}\")

set (CMAKE_CXX_FLAGS "-O2 -pipe -W -DQT_GUI_LIB -std=c++0x")

include (CMakeMacros/UserMacros.cmake)

include_directories(
		${QT_INCLUDES}
		${CMAKE_CURRENT_BINARY_DIR}
		src
		src/misc
		src/ui
		src/gui
		src/gui/widgets
		src/gui/items
		src/gui/dialogs
		src/gui/views
		src/gui/inputs
		src/gui/validators
		src/db src/models
)

####################################################
# FORMS
####################################################
set(${CMAKE_PROJECT_NAME}_FORMS
		src/ui/MainWindow.ui
		src/ui/DatabaseBrowser.ui
		src/ui/ProductsRecordWidget.ui
		src/ui/BatchRecordWidget.ui
		src/ui/DistributorRecordWidget.ui

		src/ui/DBItem.ui
		src/ui/TabProductsWidget.ui
		src/ui/TabBatchWidget.ui
		src/ui/TabDistributorWidget.ui
		src/ui/TabMealWidget.ui
		src/ui/MealFoodListItemDataWidget.ui
		src/ui/MealTabInsertWidget.ui

		src/ui/AboutDialog.ui
		src/ui/CampSettingsDialog.ui
		src/ui/SettingsDialog.ui
)

####################################################
# RESOURCES
####################################################
set(${CMAKE_PROJECT_NAME}_db_RESOURCES src/database.qrc)
set(${CMAKE_PROJECT_NAME}_reports_RESOURCES src/reports.qrc)
set(${CMAKE_PROJECT_NAME}_icons_RESOURCES src/icons.qrc)
set(${CMAKE_PROJECT_NAME}_splash_RESOURCES src/splash.qrc)
set(${CMAKE_PROJECT_NAME}_translations_RESOURCES src/translations.qrc)

####################################################
# SOURCES
####################################################
set(${CMAKE_PROJECT_NAME}_SRCS
		src/gui/views/AbstractTableView.cpp
		src/gui/views/BatchTableView.cpp
		src/gui/views/ProductsTableView.cpp
		src/gui/views/DistributorTableView.cpp
		src/gui/views/MealDaysTableView.cpp
		src/gui/views/TableDelegates.cpp

		src/gui/inputs/CustomLineEdit.cpp
		src/gui/inputs/DateInput.cpp
		src/gui/inputs/UnitInput.cpp
		src/gui/inputs/PriceInput.cpp
		src/gui/inputs/QtyInput.cpp
		src/gui/inputs/TextInput.cpp

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

		src/gui/dialogs/AboutDialog.cpp
		src/gui/dialogs/CampSettingsDialog.cpp
		src/gui/dialogs/AbstractDimmingWidget.cpp
		src/gui/dialogs/DimmingMessage.cpp
		src/gui/dialogs/DimmingWidget.cpp
		src/gui/dialogs/SettingsDialog.cpp

		src/gui/items/MealFoodListItemDataWidget.cpp

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

		src/db/DBReports.cpp
		src/db/Database.cpp
		src/db/CampProperties.cpp
		src/db/MealManager.cpp

		src/misc/ApplicationUpdater.cpp
		src/misc/EventFilter.cpp
		src/misc/Logger.cpp
		src/misc/DataParser.cpp
		src/${CMAKE_PROJECT_NAME}.cpp

		main.cpp
)

####################################################
# GENERAL INCLUDES
####################################################
set(${CMAKE_PROJECT_NAME}_INCS
		config.h
		version.h
)

configure_file(config/config.h.in config.h @ONLY)

add_custom_command(
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version.h
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/config/version.h.in
		COMMAND ${CMAKE_COMMAND} -D PROJECT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -P ${CMAKE_CURRENT_SOURCE_DIR}/CMakeMacros/MakeVersionConfig.cmake
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

####################################################
# TRANSLATIONS
####################################################
file (GLOB TS_FILES src/translations/${CMAKE_PROJECT_NAME}_*.ts)
set(${CMAKE_PROJECT_NAME}_TS ${TS_FILES})

option (UPDATE_TRANSLATIONS "Update source translation translations/*.ts files (WARNING: make clean will delete the source .ts files! Danger!)")

if (UPDATE_TRANSLATIONS)
  qt4_create_translation(QM_FILES ${${CMAKE_PROJECT_NAME}_SRCS} ${TS_FILES})
else (UPDATE_TRANSLATIONS)
  qt4_add_translation(QM_FILES ${TS_FILES})
endif (UPDATE_TRANSLATIONS) 

####################################################
# GENERATE MOC and UI
####################################################
qt4_wrap_ui(${CMAKE_PROJECT_NAME}_FORMS_HEADERS ${${CMAKE_PROJECT_NAME}_FORMS})
qt4_wrap_cpp(${${CMAKE_PROJECT_NAME}_MOC_INCS} ${${CMAKE_PROJECT_NAME}_SRCS})
qt4_automoc(${${CMAKE_PROJECT_NAME}_MOC_SRCS} ${${CMAKE_PROJECT_NAME}_SRCS})

message(STATUS "QM files" ${QM_FILES})
##################
# TODO for MACOSX
##################
# if(APPLE)
# 	set(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
# 	set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_NAME} ${PROJECT_VERSION}")
# 	set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
# 	set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
# 	set(MACOSX_BUNDLE_COPYRIGHT "${PROJECT_COPYRIGHT_YEAR} ${PROJECT_VENDOR}")
# 	set(MACOSX_BUNDLE_ICON_FILE "audio-input-microphone.icns")
# 	set(MACOSX_BUNDLE_GUI_IDENTIFIER "${PROJECT_DOMAIN_SECOND}.${PROJECT_DOMAIN_FIRST}")
# 	set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
# 
# 	set(MACOSX_BUNDLE_RESOURCES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app/Contents/Resources")
# 	set(MACOSX_BUNDLE_ICON "${ICONS_DIR}/${MACOSX_BUNDLE_ICON_FILE}")
# 	execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${MACOSX_BUNDLE_RESOURCES})
# 	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MACOSX_BUNDLE_ICON} ${MACOSX_BUNDLE_RESOURCES})
# endif()


# set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES config.h)

# set(QT_LRELEASE_EXECUTABLE "${QT_LRELEASE_EXECUTABLE} -removeidentical -markuntranslated \"TODO:\"")

if (rcc_files)
	unset (rcc_files)
endif (rcc_files)

if (UNIX OR APPLE)
	add_custom_target(translations_target ALL DEPENDS ${QM_FILES})

	set(${CMAKE_PROJECT_NAME}_db_RESOURCES_RCC database.rcc)
	set(${CMAKE_PROJECT_NAME}_reports_RESOURCES_RCC reports.rcc)
	set(${CMAKE_PROJECT_NAME}_icons_RESOURCES_RCC icons.rcc)
	set(${CMAKE_PROJECT_NAME}_splash_RESOURCES_RCC splash.rcc)

	qt4_add_binresources(${CMAKE_PROJECT_NAME}_db_RESOURCES_RCC ${${CMAKE_PROJECT_NAME}_db_RESOURCES} OPTIONS -binary)
	qt4_add_binresources(${CMAKE_PROJECT_NAME}_reports_RESOURCES_RCC ${${CMAKE_PROJECT_NAME}_reports_RESOURCES} OPTIONS -binary)
	qt4_add_binresources(${CMAKE_PROJECT_NAME}_icons_RESOURCES_RCC ${${CMAKE_PROJECT_NAME}_icons_RESOURCES} OPTIONS -binary)
	qt4_add_binresources(${CMAKE_PROJECT_NAME}_splash_RESOURCES_RCC ${${CMAKE_PROJECT_NAME}_splash_RESOURCES} OPTIONS -binary)

	add_custom_target(
		resources ALL
		DEPENDS
			${${CMAKE_PROJECT_NAME}_db_RESOURCES_RCC}
			${${CMAKE_PROJECT_NAME}_reports_RESOURCES_RCC}
			${${CMAKE_PROJECT_NAME}_icons_RESOURCES_RCC}
			${${CMAKE_PROJECT_NAME}_splash_RESOURCES_RCC}
	)

	set(RCC_SRCS ${QM})
elseif (WIN32)
	qt4_add_resources(${CMAKE_PROJECT_NAME}_db_RESOURCES_RCC ${${CMAKE_PROJECT_NAME}_db_RESOURCES})
	qt4_add_resources(${CMAKE_PROJECT_NAME}_reports_RESOURCES_RCC ${${CMAKE_PROJECT_NAME}_reports_RESOURCES})
	qt4_add_resources(${CMAKE_PROJECT_NAME}_icons_RESOURCES_RCC ${${CMAKE_PROJECT_NAME}_icons_RESOURCES})
	qt4_add_resources(${CMAKE_PROJECT_NAME}_splash_RESOURCES_RCC ${${CMAKE_PROJECT_NAME}_splash_RESOURCES})

	SET( trans_file translations )
	SET( trans_qmoutfile ${CMAKE_CURRENT_BINARY_DIR}/src/translations/ )

	SET( trans_srcfile ${CMAKE_CURRENT_SOURCE_DIR}/src/${trans_file}.qrc)
	SET( trans_infile ${CMAKE_CURRENT_BINARY_DIR}/src/${trans_file}.qrc)
	SET( trans_outfile ${CMAKE_CURRENT_BINARY_DIR}/qrc_${trans_file}.cxx)

	# Run the resource compiler (rcc_options should already be set)
	ADD_CUSTOM_COMMAND(
		OUTPUT ${trans_outfile}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${trans_qmoutfile}
		COMMAND ${CMAKE_COMMAND} -E copy ${QM_FILES} ${trans_qmoutfile}
		COMMAND ${CMAKE_COMMAND} -E copy ${trans_srcfile} ${trans_infile}
		COMMAND ${QT_RCC_EXECUTABLE} ${rcc_options} -name ${trans_file} -o ${trans_outfile} ${trans_infile}
		DEPENDS ${TS_FILES} ${trans_infile}
	)

	SET(RCC_SRCS ${${CMAKE_PROJECT_NAME}_db_RESOURCES_RCC} ${${CMAKE_PROJECT_NAME}_reports_RESOURCES_RCC} ${${CMAKE_PROJECT_NAME}_icons_RESOURCES_RCC} ${${CMAKE_PROJECT_NAME}_splash_RESOURCES_RCC} ${trans_outfile} )

endif ()

####################################################
# TARGETS
####################################################
add_executable(vfb VersionFileBuilder.cpp version.h)

add_executable(zarlok WIN32 MACOSX_BUNDLE
		${APP_ICON}
		${${CMAKE_PROJECT_NAME}_SRCS}
		${${CMAKE_PROJECT_NAME}_FORMS_HEADERS}
		${${CMAKE_PROJECT_NAME}_MOC_SRCS}
		${${CMAKE_PROJECT_NAME}_MOC_INCS}
		${RCC_SRCS}
)

add_dependencies(zarlok vfb)

target_link_libraries(
		${CMAKE_PROJECT_NAME}
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_QTSQL_LIBRARY}
		${QT_QTNETWORK_LIBRARY}
		${QT_QT_LIBRARY}
)

add_custom_target(version_number COMMAND vfb WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

if ( UNIX OR APPLE )

	install(FILES
			${APP_ICON}
			DESTINATION share/icons
	)

	install(FILES
			${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.desktop
			DESTINATION share/applications
	)

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

endif ( UNIX OR APPLE )

set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "${BIN_INSTALL_DIR}")
include(InstallRequiredSystemLibraries)

if ( APPLE )
    set(EXECUTABLE "${CMAKE_PROJECT_NAME}.app")
elseif ( WIN32 )
    set(EXECUTABLE "${CMAKE_PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}")
else ()
    set(EXECUTABLE "${BIN_INSTALL_DIR}/${CMAKE_PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}")
endif ()

if ( APPLE OR WIN32 )

    include(DeployQt4)
    fixup_qt4_executable(${EXECUTABLE}
			"qsqlite"
			""
			""
			""
	)

elseif ( UNIX )

	install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION ${BIN_INSTALL_DIR})

endif()

install(FILES COPYING.txt DESTINATION ${DOC_INSTALL_DIR} RENAME COPYING)
install(FILES README.txt DESTINATION ${DOC_INSTALL_DIR} RENAME README)
install(FILES NEWS.txt DESTINATION ${DOC_INSTALL_DIR} RENAME NEWS)
####################################################
# Inno Setup Installator
####################################################
if ( WIN32 )

	find_program(iscc_exe iscc PATHS ${ISCC_BINARY_DIR} ENV PATH)

	if (NOT iscc_exe STREQUAL "iscc_exe-NOTFOUND")
		MESSAGE(STATUS "Inno Setup Compiler : " ${iscc_exe})
	else (NOT iscc_exe STREQUAL "iscc_exe-NOTFOUND")
		MESSAGE(FATAL_ERROR "iscc not found!")
	endif (NOT iscc_exe STREQUAL "iscc_exe-NOTFOUND")

	add_custom_target(isspackage
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/dist/win32-installer/${CMAKE_PROJECT_NAME}_installer.iss.in ${CMAKE_PROJECT_NAME}_installer.iss.in
		COMMAND ${CMAKE_COMMAND} -D QT_BINARY_DIR=${QT_BINARY_DIR} -D QT_PLUGINS_DIR=${QT_PLUGINS_DIR} -D PROJECT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -P ${CMAKE_CURRENT_SOURCE_DIR}/CMakeMacros/MakeInstallator.cmake
		COMMAND ${iscc_exe} ${CMAKE_PROJECT_NAME}_installer.iss
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		DEPENDS version.h
		COMMENT "Preparing Inno Setup script file"
	)

	add_dependencies(${CMAKE_PROJECT_NAME} version_number)
	add_dependencies(isspackage ${CMAKE_PROJECT_NAME})

endif ( WIN32 )

####################################################
# CPACK
####################################################
if ( UNIX )
add_custom_target(distprepare
	COMMAND ${CMAKE_COMMAND} -D PROJECT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -P ${CMAKE_CURRENT_SOURCE_DIR}/CMakeMacros/CPackConfigurationPrepare.cmake
	COMMAND ${CMAKE_COMMAND} -D CMAKE_GENERATOR=${CMAKE_GENERATOR} -D CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME} -D PROJECT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -P CPackConfigurationLinux.cmake
	DEPENDS version.h
	COMMENT "Preparing CPack files for GNU/Linux"
)
endif ( UNIX )

# if ( APPLE )
# add_custom_target(distprepare
# 	COMMAND ${CMAKE_COMMAND} -D PROJECT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -P ${CMAKE_CURRENT_SOURCE_DIR}/CMakeMacros/CPackConfigurationPrepare.cmake
# 	COMMAND ${CMAKE_COMMAND} -D CMAKE_GENERATOR=${CMAKE_GENERATOR} -D CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME} -D PROJECT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -P CPackConfigurationApple.cmake
# 	DEPENDS version.h
# 	COMMENT "Preparing CPack files for Apple"
# )
# endif ( APPLE )

if ( WIN32 )
add_custom_target(distprepare
	COMMAND ${CMAKE_COMMAND} -D PROJECT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -P ${CMAKE_CURRENT_SOURCE_DIR}/CMakeMacros/CPackConfigurationPrepare.cmake
	COMMAND ${CMAKE_COMMAND} -D CMAKE_GENERATOR=${CMAKE_GENERATOR} -D CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME} -D PROJECT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -P CPackConfigurationWin.cmake
	DEPENDS version.h
	COMMENT "Preparing CPack files for Windows"
)
endif ( WIN32 )

add_dependencies(distprepare vfb)
add_dependencies(${CMAKE_PROJECT_NAME} version_number distprepare)

####################################################
# SUMMARY
####################################################
MESSAGE(STATUS "<<< Configuration >>>
Project:		${CMAKE_PROJECT_NAME}
Generator:		${CMAKE_GENERATOR}

System name:	${CMAKE_SYSTEM_NAME}
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
