FUNCTION(PREPEND var prefix)
    SET(listvar "")
    FOREACH(f ${ARGN})
        LIST(APPEND listvar "${prefix}/${f}")
    ENDFOREACH()
    SET(${var} "${listvar}" PARENT_SCOPE)
ENDFUNCTION()

SET(SRC_FILES fileutils.cpp fileutils.h filewatcher.cpp
        sshmanager.cpp sshmanager.h scpmanager.cpp scpmanager.h
        filemanager.cpp filemanager.h)
IF(WIN32)
LIST(APPEND SRC_FILES filewatcherwindows.cpp filewatcherwindows.h)
ELSE()
LIST(APPEND SRC_FILES filewatcherlinux.cpp filewatcherlinux.h)
ENDIF()

PREPEND(SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${SRC_FILES})
SET(SRC_FILES ${SRC_FILES} PARENT_SCOPE)
