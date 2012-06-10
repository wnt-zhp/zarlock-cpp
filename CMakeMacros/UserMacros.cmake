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

# macro (MAKE_ICON iconfile)
# 	QT4_EXTRACT_OPTIONS(rc_file rc_option ${ARGN})
# 
# 	FOREACH (it ${rc_file})
# 
# 	IF( MINGW )
# 		set( APP_ICON appicon.o )
# 		set( APP_ICON_RC src/appicon.rc )
# 	# resource compilation for MinGW
# 	ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${APP_ICON}
# 		COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR}
# 			-i${CMAKE_CURRENT_SOURCE_DIR}/${APP_ICON_RC}
# 			-o ${CMAKE_CURRENT_BINARY_DIR}/${APP_ICON} )
# 	SET(speedcrunch_SOURCES ${speedcrunch_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/${APP_ICON})
# ELSE( MINGW )
# 	SET(speedcrunch_SOURCES ${speedcrunch_SOURCES} crunchico.rc)
# ENDIF ( MINGW )
# endmacro (MAKE_ICON)