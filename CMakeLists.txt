cmake_minimum_required(VERSION 3.10)
project(hi)

#set(CMAKE_CXX_COMPILER "/usr/local/bin/mpicc")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -I/usr/local/opt/libomp/include")
#set(OpenMP_CXX_LIB_NAMES "omp")
#set(OpenMP_omp_LIBRARY /usr/local/opt/libomp/lib/libomp.dylib)

#find_package(OpenMP REQUIRED)
#if (OPENMP_FOUND)
#    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#endif()

include(MPI.cmake) ###### MPI

include(Boost.cmake) ###### Boost

include(GTest.cmake) ###### Google-Test

link_directories(/usr/local/include/flann)

include_directories($ENV{HOME}/src/LSHBOX/include)

set(PROGRAMS src/main/hi.cpp src/test/testall.cpp)

set(TPROGRAMS src/main/hi2.cpp)

set(SOURCE_FILES src/main/utils/MasterQ.cpp src/main/utils/MasterQ.hpp src/main/datastructures/rm/WorkItem.cpp
        src/main/datastructures/rm/WorkItem.hpp src/main/utils/vecutil.cpp src/main/debug.hpp src/main/datastructures/data.hpp
        src/main/globals.hpp src/main/datastructures/dna.cpp src/main/datastructures/dna.hpp
        src/main/datastructures/hi/HI.cpp src/main/datastructures/hi/HI.hpp src/main/datastructures/rm/Worker.cpp
        src/main/datastructures/rm/Worker.hpp src/main/datastructures/rm/Master.cpp src/main/datastructures/rm/Master.hpp
        src/main/datastructures/hi/Forest.cpp src/main/datastructures/hi/Forest.hpp src/main/datastructures/Euc.cpp
        src/main/datastructures/Euc.hpp src/main/datastructures/Index.cpp src/main/datastructures/Index.hpp
        src/main/loaders/loader.cpp src/main/loaders/loader.hpp src/main/dtypes.hpp src/main/datastructures/hi/HIBuildItem.cpp
        src/main/datastructures/hi/HIBuildItem.hpp src/main/datastructures/hi/QueryItem.cpp src/main/datastructures/hi/QueryItem.hpp
        src/main/datastructures/rm/Job.cpp src/main/datastructures/rm/Job.hpp src/main/datastructures/hi/HIJob.cpp
        src/main/datastructures/hi/HIJob.cpp src/main/datastructures/hi/HIJob.hpp src/main/datastructures/rm/JobHandler.cpp
        src/main/datastructures/rm/JobHandler.hpp src/main/datastructures/min/MinJob.cpp src/main/datastructures/min/MinJob.hpp
        src/main/datastructures/rm/ReturnItem.cpp src/main/datastructures/rm/ReturnItem.hpp src/main/datastructures/Timer.cpp
        src/main/datastructures/Timer.hpp src/main/utils/stringutils.hpp)

foreach(program ${PROGRAMS})
    get_filename_component(progname ${program} NAME_WE)
    add_executable(${progname} ${SOURCE_FILES} ${program})
    target_link_libraries(${progname} gtest gtest_main)
    target_link_libraries(${progname} ${MPI_LIBRARIES})
    target_link_libraries(${progname} ${Boost_LIBRARIES} )
    target_link_libraries(${progname} ${_hdf5_libs})
endforeach(program)

foreach(program ${TPROGRAMS})
    get_filename_component(progname ${program} NAME_WE)
    add_executable(${progname}  ${program})
    target_link_libraries(${progname} ${MPI_LIBRARIES})
    target_link_libraries(${progname} ${Boost_LIBRARIES} )
endforeach(program)

add_test(NAME Dummy COMMAND hi)
