# This file adds OpenMP to your project if you are using Apple Clang.

option(APPLE_OMP_AUTOADD "Add OpenMP if using AppleClang" ON)

find_package(OpenMP QUIET)
if(NOT "${OpenMP_FOUND}" OR NOT "${OpenMP_CXX_FOUND}")
    if("${APPLE_OMP_AUTOADD}" AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "7")

        message(STATUS "AppleClang >= 7.0 detected, adding OpenMP. Disable with -DAPPLE_OMP_AUTOADD=OFF")

        find_program(BREW NAMES brew)
        if(BREW)
            execute_process(COMMAND ${BREW} ls libomp RESULT_VARIABLE BREW_RESULT_CODE OUTPUT_QUIET ERROR_QUIET)
            if(BREW_RESULT_CODE)
                message(STATUS "This program supports OpenMP on Mac through Brew. Please run \"brew install libomp\"")
            else()
                execute_process(COMMAND ${BREW} --prefix libomp OUTPUT_VARIABLE BREW_LIBOMP_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
                set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -I/usr/local/opt/libomp/include")
                set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -Iw")
                set(OpenMP_CXX_LIB_NAMES "omp")
                set(OpenMP_omp_LIBRARY "${BREW_LIBOMP_PREFIX}/lib/libomp.dylib")
                include_directories("${BREW_LIBOMP_PREFIX}/include")
                message(STATUS "Using Homebrew libomp from ${BREW_LIBOMP_PREFIX}")
            endif()
        else()
            message(STATUS "This program supports OpenMP on Mac through Homebrew, installing Homebrew recommmended https://brew.sh")
        endif()
    endif()
endif()

if(${OpenMP_FOUND})
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    add_definitions(-DENABLE_OPENMP=1)
    if(NOT TARGET OpenMP::OpenMP_CXX)
        add_library(OpenMP_TARGET INTERFACE)
        add_library(OpenMP::OpenMP_CXX ALIAS OpenMP_TARGET)
        target_compile_options(OpenMP_TARGET INTERFACE ${OpenMP_CXX_FLAGS})
        find_package(Threads REQUIRED)
        target_link_libraries(OpenMP_TARGET INTERFACE Threads::Threads)
        target_link_libraries(OpenMP_TARGET INTERFACE ${OpenMP_CXX_FLAGS})
    endif()
endif()


# add to program
# target_link_libraries(${progname} ${OpenMP_CXX_LIBRARIES} ${PROJECT_LINK_LIBS})
#add_definitions(-DOMPI_SKIP_MPICXX)