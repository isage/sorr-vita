# Locate Tre
# This module defines XXX_FOUND, XXX_INCLUDE_DIRS and XXX_LIBRARIES standard variables
#
# $TREDIR is an environment variable that would
# correspond to the ./configure --prefix=$TREDIR
# used in building tre.

SET(TRE_SEARCH_PATHS
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local
    /usr
    /sw # Fink
    /opt/local # DarwinPorts
    /opt/csw # Blastwave
    /opt
)

SET(MSVC_YEAR_NAME)
IF (MSVC_VERSION GREATER 1599)      # >= 1600
    SET(MSVC_YEAR_NAME VS2010)
ELSEIF(MSVC_VERSION GREATER 1499)   # >= 1500
    SET(MSVC_YEAR_NAME VS2008)
ELSEIF(MSVC_VERSION GREATER 1399)   # >= 1400
    SET(MSVC_YEAR_NAME VS2005)
ELSEIF(MSVC_VERSION GREATER 1299)   # >= 1300
    SET(MSVC_YEAR_NAME VS2003)
ELSEIF(MSVC_VERSION GREATER 1199)   # >= 1200
    SET(MSVC_YEAR_NAME VS6)
ENDIF()

FIND_PATH(TRE_INCLUDE_DIR
    NAMES tre/tre.h
    HINTS
    $ENV{TREDIR}
    $ENV{TRE_PATH}
    PATH_SUFFIXES include
    PATHS ${TRE_SEARCH_PATHS}
)

FIND_LIBRARY(TRE_LIBRARY
    NAMES tre libtre
    HINTS
    $ENV{TREDIR}
    $ENV{TRE_PATH}
    PATH_SUFFIXES lib lib64 lib/i386-linux-gnu win32/Tre_Dynamic_Release "Win32/${MSVC_YEAR_NAME}/x64/Release" "Win32/${MSVC_YEAR_NAME}/Win32/Release"
    PATHS ${TRE_SEARCH_PATHS}
)

# First search for d-suffixed libs
FIND_LIBRARY(TRE_LIBRARY_DEBUG
    NAMES tred tre_d libtred libtre_d
    HINTS
    $ENV{TREDIR}
    $ENV{TRE_PATH}
    PATH_SUFFIXES lib lib64 lib/i386-linux-gnu win32/Tre_Dynamic_Debug "Win32/${MSVC_YEAR_NAME}/x64/Debug" "Win32/${MSVC_YEAR_NAME}/Win32/Debug"
    PATHS ${TRE_SEARCH_PATHS}
)

IF(NOT TRE_LIBRARY_DEBUG)
    # Then search for non suffixed libs if necessary, but only in debug dirs
    FIND_LIBRARY(TRE_LIBRARY_DEBUG
        NAMES tre libtre
        HINTS
        $ENV{TREDIR}
        $ENV{TRE_PATH}
        PATH_SUFFIXES win32/Tre_Dynamic_Debug "Win32/${MSVC_YEAR_NAME}/x64/Debug" "Win32/${MSVC_YEAR_NAME}/Win32/Debug"
        PATHS ${TRE_SEARCH_PATHS}
    )
ENDIF()


IF(TRE_LIBRARY)
    IF(TRE_LIBRARY_DEBUG)
        SET(TRE_LIBRARIES optimized "${TRE_LIBRARY}" debug "${TRE_LIBRARY_DEBUG}")
    ELSE()
        SET(TRE_LIBRARIES "${TRE_LIBRARY}")       # Could add "general" keyword, but it is optional
    ENDIF()
ENDIF()

# handle the QUIETLY and REQUIRED arguments and set XXX_FOUND to TRUE if all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TRE DEFAULT_MSG TRE_LIBRARIES TRE_INCLUDE_DIR)
