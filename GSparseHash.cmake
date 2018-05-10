
# - Find google-sparsehash
# Find the google-sparsehash includes
# This module defines
# SPARSEHASH_INCLUDE_DIR, root jreen include dir.
# SPARSEHASH_FOUND, whether libjreen was found
#
#FIND_PACKAGE(PkgConfig QUIET)
#PKG_CHECK_MODULES(PC_SPARSEHASH QUIET libsparsehash)
#
#FIND_PATH(SPARSEHASH_INCLUDE_DIR NAMES google/sparsetable
#        HINTS
#        ${PC_SPARSEHASH_INCLUDEDIR}
#        ${PC_SPARSEHASH_INCLUDE_DIRS}
#        ${CMAKE_INSTALL_INCLUDEDIR}
#        )
#
#IF(SPARSEHASH_INCLUDE_DIR AND NOT PC_SPARSEHASH_VERSION)
#    MESSAGE(WARNING "You don't have pkg-config and so the google-sparsehash version check does not work!")
#ENDIF()
#
#INCLUDE(FindPackageHandleStandardArgs)
#FIND_PACKAGE_HANDLE_STANDARD_ARGS(Sparsehash
#        REQUIRED_VARS SPARSEHASH_INCLUDE_DIR)
#
#MARK_AS_ADVANCED(SPARSEHASH_INCLUDE_DIR)

if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif()

include(DownloadProject.cmake)
download_project(PROJ                google_sparsehash
        GIT_REPOSITORY      https://github.com/sparsehash/sparsehash-c11.git
        GIT_TAG             master
        ${UPDATE_DISCONNECTED_IF_AVAILABLE}
        )

add_subdirectory(${google_sparsehash_SOURCE_DIR} ${google_sparsehash_BINARY_DIR})

find_package(google_sparsehash REQUIRED)
if (NOT SPARSEHASH_FOUND)
    message(FATAL_ERROR "Could not find google-sparsehash.")
endif()


#if (NOT TARGET gtest)
 #   add_library(gtest gmock-gtest-all.cc)
#endif()
# This will have to be added to the CMakeLists.txt of the conflicting files