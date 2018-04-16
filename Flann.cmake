###############################################################################
# Find Flann
#
# This sets the following variables:
# FLANN_FOUND - True if FLANN was found.
# FLANN_INCLUDE_DIRS - Directories containing the FLANN include files.
# FLANN_LIBRARIES - Libraries needed to use FLANN.
# FLANN_DEFINITIONS - Compiler flags for FLANN.
# export PKG_CONFIG_PATH=/home1/01208/parnell/usr/local/flann-1.8.4/lib/pkgconfig:$PKG_CONFIG_PATH


find_package(PkgConfig)
pkg_check_modules(PC_FLANN flann)


if(APPLE)
    link_directories(/usr/local/include/flann)
elseif(UNIX)
    #    link_directories($ENV{HOME}/usr/local/flann-1.8.4/include/flann)
    include_directories($ENV{HOME}/usr/local/flann-1.8.4/include/flann)
    set(PC_FLANN_INCLUDEDIR $ENV{HOME}/usr/local/flann-1.8.4/include)
    set(PC_FLANN_LIBDIR $ENV{HOME}/usr/local/flann-1.8.4/include)

    set(FLANN_DEFINITIONS ${PC_FLANN_CFLAGS_OTHER})

    find_path(FLANN_INCLUDE_DIR flann/flann.hpp
            HINTS ${PC_FLANN_INCLUDEDIR} ${PC_FLANN_INCLUDE_DIRS})

    find_library(FLANN_LIBRARY flann
            HINTS ${PC_FLANN_LIBDIR} ${PC_FLANN_LIBRARY_DIRS})

    set(FLANN_INCLUDE_DIRS ${FLANN_INCLUDE_DIR})
    set(FLANN_LIBRARIES ${FLANN_LIBRARY})

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(Flann DEFAULT_MSG
            FLANN_LIBRARY FLANN_INCLUDE_DIR)

    mark_as_advanced(FLANN_LIBRARY FLANN_INCLUDE_DIR)
    #find_package(Flann REQUIRED )

    include_directories( ${FLANN_INCLUDE_DIRS} )
endif()
