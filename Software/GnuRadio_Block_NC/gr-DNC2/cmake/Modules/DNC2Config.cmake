INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_DNC2 DNC2)

FIND_PATH(
    DNC2_INCLUDE_DIRS
    NAMES DNC2/api.h
    HINTS $ENV{DNC2_DIR}/include
        ${PC_DNC2_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    DNC2_LIBRARIES
    NAMES gnuradio-DNC2
    HINTS $ENV{DNC2_DIR}/lib
        ${PC_DNC2_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DNC2 DEFAULT_MSG DNC2_LIBRARIES DNC2_INCLUDE_DIRS)
MARK_AS_ADVANCED(DNC2_LIBRARIES DNC2_INCLUDE_DIRS)

