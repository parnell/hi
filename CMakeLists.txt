cmake_minimum_required(VERSION 3.10)
project(hi)

message(" OS SYSTEM Apple=" ${APPLE} ", Unix=" ${UNIX})
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COMPILE_TESTS 1)
set(COMPILE_PROFILING 0)

add_definitions(-DDEBUG_PRINT=1)
add_definitions(-DDEBUG_LSH=0)
add_definitions("-DCMAKE_CURRENT_BINARY_DIR=\"${CMAKE_CURRENT_BINARY_DIR}\"")
if (COMPILE_TESTS)
    set(TEST_PROGRAMS src/test/testall.cpp)

    include(GTest.cmake) ###### Google-Test
    add_definitions(-DCOMPILE_TESTS=1)
endif(COMPILE_TESTS)
if (COMPILE_PROFILING)
    SET(GCC_COVERAGE_LINK_FLAGS "-Wl,-no_pie")
    add_definitions(${GCC_COVERAGE_LINK_FLAGS})
    add_definitions(-DCOMPILE_PROFILING=1)
endif(COMPILE_PROFILING)

include (CTest)

include(MPI.cmake) ###### MPI

include(Boost.cmake) ###### Boost

include(PatchOpenMPApple.cmake) ###### MPI

include(FindGperftools.cmake)

#include(GSparseHash.cmake) ###### Googles Sparse Hash

if(APPLE)
    find_package (HDF5)

    include_directories(${HDF5_INCLUDE_DIRS})
    set(_hdf5_libs hdf5 hdf5_cpp)

    include_directories($ENV{HOME}/src/LSHBOX/include)

#    link_directories(/usr/local/include/flann)
    #link_directories($ENV{HOME}/src/flann/b)
    link_directories(/usr/local/Cellar/hdf5/1.10.1_2/lib)
    link_directories($ENV{HOME}/src/LSHBOX/include)

elseif(UNIX)
#    include(Flann.cmake) ###### Flann

    include(HDF5.cmake) ###### HDF5
endif()

include(LSHBox.cmake) ###### LSHBox

set(PROGRAMS src/main/run_hi.cpp)

set(T2PROGRAMS src/test/mpi_test.cpp src/test/openmp_test.cpp)

set(T3PROGRAMS src/test/profile_test.cpp)

set(SOURCE_FILES src/main/datastructures/rm/WorkItem.cpp
        src/main/datastructures/rm/WorkItem.hpp src/main/utils/vecutil.cpp src/main/debug.hpp src/main/datastructures/data.hpp
        src/main/globals.hpp src/main/datastructures/datatypes/dna.cpp src/main/datastructures/datatypes/dna.hpp
        src/main/datastructures/indexes/hi/HI.cpp src/main/datastructures/indexes/hi/HI.hpp
        src/main/datastructures/rm/Secondary.cpp src/main/datastructures/rm/Secondary.hpp
        src/main/datastructures/rm/Primary.cpp src/main/datastructures/rm/Primary.hpp
        src/main/datastructures/datatypes/Euc.cpp
        src/main/datastructures/datatypes/Euc.hpp src/main/datastructures/indexes/Index.cpp src/main/datastructures/indexes/Index.hpp
        src/main/loaders/loader.cpp src/main/loaders/loader.hpp src/main/dtypes.hpp src/main/datastructures/indexes/hi/job/HIBuildItem.cpp
        src/main/datastructures/indexes/hi/job/HIBuildItem.hpp src/main/datastructures/indexes/hi/job/QueryItem.cpp src/main/datastructures/indexes/hi/job/QueryItem.hpp
        src/main/datastructures/rm/Job.cpp src/main/datastructures/rm/Job.hpp src/main/datastructures/indexes/hi/job/HIJob.cpp
        src/main/datastructures/indexes/hi/job/HIJob.cpp src/main/datastructures/indexes/hi/job/HIJob.hpp src/main/datastructures/rm/JobHandler.cpp
        src/main/datastructures/rm/JobHandler.hpp src/main/datastructures/min/TestJob.cpp src/main/datastructures/min/TestJob.hpp
        src/main/utils/Timer.cpp
        src/main/utils/Timer.hpp src/main/utils/stringutils.hpp src/main/myomp.hpp
        src/main/datastructures/indexes/hi/job/HIBuild.cpp src/main/datastructures/indexes/hi/job/HIBuild.hpp src/main/datastructures/rm/LoadBalancer.cpp
        src/main/datastructures/rm/LoadBalancer.hpp src/main/datastructures/controllers/DataManager.cpp src/main/datastructures/controllers/DataManager.hpp
        src/main/datastructures/indexes/hi/HITree.cpp src/main/datastructures/indexes/hi/HITree.hpp src/main/utils/statutil.cpp src/main/utils/statutil.hpp
        src/main/datastructures/rm/M.cpp src/main/datastructures/rm/M.hpp src/main/datastructures/Stat.cpp src/main/datastructures/Stat.hpp
        src/main/datastructures/indexes/controllers/IndexDecider.cpp src/main/datastructures/indexes/controllers/IndexDecider.hpp
        src/main/datastructures/indexes/controllers/Partitioner.cpp src/main/datastructures/indexes/controllers/Partitioner.hpp
        src/main/datastructures/indexes/hi/indexwrappers/LSHWrapper.cpp src/main/datastructures/indexes/hi/indexwrappers/LSHWrapper.hpp
        src/main/datastructures/indexes/controllers/PivotSelector.cpp src/main/datastructures/indexes/controllers/PivotSelector.hpp
        src/main/datastructures/indexes/hi/HINode.cpp src/main/datastructures/indexes/hi/HINode.hpp
        src/main/datastructures/indexes/controllers/PivotSorter.cpp src/main/datastructures/indexes/controllers/PivotSorter.hpp
        src/main/utils/testutil.cpp src/main/utils/testutil.hpp src/main/datastructures/indexes/hi/indexwrappers/SpatialWrapper.cpp
        src/main/datastructures/indexes/hi/indexwrappers/SpatialWrapper.hpp src/main/datastructures/indexes/hi/params.cpp
        src/main/datastructures/indexes/hi/params.hpp src/main/datastructures/indexes/controllers/Pivot.hpp src/main/datastructures/indexes/controllers/Pivot.cpp
        )


foreach(program ${PROGRAMS} ${TEST_PROGRAMS} ${T3PROGRAMS} ${T2PROGRAMS})
    get_filename_component(progname ${program} NAME_WE)
    add_executable(${progname} ${SOURCE_FILES} ${program})
    if(APPLE)
        target_link_libraries(${progname} PUBLIC ${_hdf5_libs})
    elseif(UNIX)
#        target_link_libraries(${progname} PUBLIC ${FLANN_LIBRARIES})
    endif()

    target_link_libraries(${progname} PUBLIC ${MPI_LIBRARIES})
    target_link_libraries(${progname} PUBLIC ${Boost_LIBRARIES} )

    if(${OpenMP_FOUND})
        target_link_libraries(${progname} PUBLIC ${OpenMP_CXX_LIBRARIES} ${PROJECT_LINK_LIBS})
        target_link_libraries(${progname} PUBLIC OpenMP::OpenMP_CXX)
    endif()
    if (COMPILE_TESTS)
        target_link_libraries(${progname} PUBLIC gtest gtest_main)
    endif(COMPILE_TESTS)
    if (COMPILE_PROFILING)
        target_link_libraries(${progname} PUBLIC ${GPERFTOOLS_PROFILER})
    endif(COMPILE_PROFILING)

endforeach(program)



