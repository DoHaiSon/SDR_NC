INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_DNC DNC)

FIND_PATH(
    DNC_INCLUDE_DIRS
    NAMES DNC/api.h
    HINTS $ENV{DNC_DIR}/include
        ${PC_DNC_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    DNC_LIBRARIES
    NAMES gnuradio-DNC
    HINTS $ENV{DNC_DIR}/lib
        ${PC_DNC_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DNC DEFAULT_MSG DNC_LIBRARIES DNC_INCLUDE_DIRS)
MARK_AS_ADVANCED(DNC_LIBRARIES DNC_INCLUDE_DIRS)

