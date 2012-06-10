cmake_minimum_required(VERSION 2.8)

find_program(git_exe git)

unset (gitinfo_TAGNAME CACHE)
unset (gitinfo_COMMITDATE CACHE)
unset (gitinfo_TAGDESC CACHE) 

if (git_exe)
# 	execute_process (COMMAND git tag OUTPUT_VARIABLE gitinfo_TAGNAME ERROR_VARIABLE gitinfo_TAGNAME_ERROR OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process (COMMAND git describe OUTPUT_VARIABLE gitinfo_TAGDESC ERROR_VARIABLE gitinfo_TAGDESC_ERROR OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process (COMMAND git show -s --format="%ct" OUTPUT_VARIABLE gitinfo_COMMITDATE ERROR_VARIABLE gitinfo_COMMITDATE_ERROR OUTPUT_STRIP_TRAILING_WHITESPACE)
	STRING(REPLACE "\"" "" gitinfo_COMMITDATE ${gitinfo_COMMITDATE})
# 	add_definitions(-DGITREVNUM=\"${info_GITREVNUM}\")
	MESSAGE(STATUS "Build version : " ${gitinfo_TAGNAME})
	MESSAGE(STATUS "Build commit date (UNIX format) : " ${gitinfo_COMMITDATE})
else (git_exe)
	MESSAGE(WARNING "Git not found!")
	set (gitinfo_TAGNAME "unknown")
	set (gitinfo_COMMITDATE "unknown")
	set (gitinfo_TAGDESC "unknown") 
endif (git_exe)

configure_file(${ZARLOK_SOURCE_DIR}/config/version.h.in version.h @ONLY)
