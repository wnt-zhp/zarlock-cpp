find_program (vfb_exe vfb)
MESSAGE (STATUS "VFB: " ${vfb_exe})

execute_process (COMMAND ${vfb_exe} OUTPUT_VARIABLE RELEASE_VERSION ERROR_VARIABLE RELEASE_VERSION_ERROR OUTPUT_STRIP_TRAILING_WHITESPACE)
MESSAGE (STATUS ${RELEASE_VERSION})

if ( UNIX AND NOT APPLE )
configure_file(${ZARLOK_SOURCE_DIR}/CMakeMacros/CPackConfigurationLinux.cmake.in CPackConfigurationLinux.cmake @ONLY)
MESSAGE (STATUS "CPack configure script prepared for GNU/Linux")
endif ( UNIX AND NOT APPLE )

if ( WIN32 )
configure_file(${ZARLOK_SOURCE_DIR}/CMakeMacros/CPackConfigurationWin.cmake.in CPackConfigurationWin.cmake @ONLY)
MESSAGE (STATUS "CPack configure script prepared for WIN32")
endif ( WIN32 )