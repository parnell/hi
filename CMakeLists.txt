cmake_minimum_required(VERSION 3.10)
project(hi)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COMPILE_TESTS 0)

if (COMPILE_TESTS)
    set(TEST_PROGRAMS src/test/testall.cpp)

    include(GTest.cmake) ###### Google-Test
endif(COMPILE_TESTS)

include(MPI.cmake) ###### MPI

include(Boost.cmake) ###### Boost

include(PatchOpenMPApple.cmake) ###### MPI

if(APPLE)
    find_package (HDF5)

    include_directories(${HDF5_INCLUDE_DIRS})
    set(_hdf5_libs hdf5 hdf5_cpp)

    include_directories($ENV{HOME}/src/LSHBOX/include)

    link_directories(/usr/local/include/flann)
    #link_directories($ENV{HOME}/src/flann/b)
    link_directories(/usr/local/Cellar/hdf5/1.10.1_2/lib)
    link_directories($ENV{HOME}/src/LSHBOX/include)

elseif(UNIX)
    include(Flann.cmake) ###### Flann

    include(HDF5.cmake) ###### HDF5
endif()

include(LSHBox.cmake) ###### LSHBox

set(PROGRAMS src/main/run_hi.cpp)

set(T2PROGRAMS src/test/hi2.cpp)

set(T3PROGRAMS src/test/hi3.cpp)

set(SOURCE_FILES src/main/utils/MasterQ.cpp src/main/utils/MasterQ.hpp src/main/datastructures/rm/WorkItem.cpp
        src/main/datastructures/rm/WorkItem.hpp src/main/utils/vecutil.cpp src/main/debug.hpp src/main/datastructures/data.hpp
        src/main/globals.hpp src/main/datastructures/dna.cpp src/main/datastructures/dna.hpp
        src/main/datastructures/hi/HI.cpp src/main/datastructures/hi/HI.hpp
        src/main/datastructures/rm/Worker.cpp src/main/datastructures/rm/Worker.hpp
        src/main/datastructures/rm/Master.cpp src/main/datastructures/rm/Master.hpp
        src/main/datastructures/hi/Forest.cpp src/main/datastructures/hi/Forest.hpp src/main/datastructures/Euc.cpp
        src/main/datastructures/Euc.hpp src/main/datastructures/Index.cpp src/main/datastructures/Index.hpp
        src/main/loaders/loader.cpp src/main/loaders/loader.hpp src/main/dtypes.hpp src/main/datastructures/hi/HIBuildItem.cpp
        src/main/datastructures/hi/HIBuildItem.hpp src/main/datastructures/hi/QueryItem.cpp src/main/datastructures/hi/QueryItem.hpp
        src/main/datastructures/rm/Job.cpp src/main/datastructures/rm/Job.hpp src/main/datastructures/hi/HIJob.cpp
        src/main/datastructures/hi/HIJob.cpp src/main/datastructures/hi/HIJob.hpp src/main/datastructures/rm/JobHandler.cpp
        src/main/datastructures/rm/JobHandler.hpp src/main/datastructures/min/MinJob.cpp src/main/datastructures/min/MinJob.hpp
        src/main/datastructures/Timer.cpp
        src/main/datastructures/Timer.hpp src/main/utils/stringutils.hpp src/main/myomp.hpp src/main/datastructures/hi/HIBuild.cpp src/main/datastructures/hi/HIBuild.hpp)


foreach(program ${PROGRAMS})
    get_filename_component(progname ${program} NAME_WE)
    add_executable(${progname} ${SOURCE_FILES} ${program})
    if(APPLE)
        target_link_libraries(${progname} PUBLIC ${_hdf5_libs})
    elseif(UNIX)
        target_link_libraries(${progname} PUBLIC ${FLANN_LIBRARIES})
    endif()
#    target_link_libraries(${progname} PUBLIC gtest gtest_main)
    target_link_libraries(${progname} PUBLIC ${MPI_LIBRARIES})
    target_link_libraries(${progname} PUBLIC ${Boost_LIBRARIES} )

    if(${OpenMP_FOUND})
        target_link_libraries(${progname} PUBLIC ${OpenMP_CXX_LIBRARIES} ${PROJECT_LINK_LIBS})
        target_link_libraries(${progname} PUBLIC OpenMP::OpenMP_CXX)
    endif()
endforeach(program)

foreach(program ${T2PROGRAMS})
    get_filename_component(progname ${program} NAME_WE)
    add_executable(${progname} ${SOURCE_FILES} ${program})
    if(APPLE)
        target_link_libraries(${progname} PUBLIC ${_hdf5_libs})
    elseif(UNIX)
        target_link_libraries(${progname} PUBLIC ${FLANN_LIBRARIES})
    endif()
    target_link_libraries(${progname} PUBLIC ${MPI_LIBRARIES})
    target_link_libraries(${progname} PUBLIC ${Boost_LIBRARIES} )
    if(${OpenMP_FOUND})
        target_link_libraries(${progname} PUBLIC ${OpenMP_CXX_LIBRARIES} ${PROJECT_LINK_LIBS})
        target_link_libraries(${progname} PUBLIC OpenMP::OpenMP_CXX)
    endif()
endforeach(program)

foreach(program ${T3PROGRAMS})
    get_filename_component(progname ${program} NAME_WE)
    add_executable(${progname}  ${program})
#    add_executable(${progname} ${SOURCE_FILES} ${program})

    target_link_libraries(${progname} PUBLIC gtest gtest_main)
    target_link_libraries(${progname} PUBLIC ${MPI_LIBRARIES})
    target_link_libraries(${progname} PUBLIC ${Boost_LIBRARIES} )
endforeach(program)

add_test(NAME Dummy COMMAND hi)
