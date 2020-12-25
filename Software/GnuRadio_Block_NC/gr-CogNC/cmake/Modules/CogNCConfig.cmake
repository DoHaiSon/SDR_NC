INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_COGNC CogNC)

FIND_PATH(
    COGNC_INCLUDE_DIRS
    NAMES CogNC/api.h
    HINTS $ENV{COGNC_DIR}/include
        ${PC_COGNC_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    COGNC_LIBRARIES
    NAMES gnuradio-CogNC
    HINTS $ENV{COGNC_DIR}/lib
        ${PC_COGNC_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(COGNC DEFAULT_MSG COGNC_LIBRARIES COGNC_INCLUDE_DIRS)
MARK_AS_ADVANCED(COGNC_LIBRARIES COGNC_INCLUDE_DIRS)

