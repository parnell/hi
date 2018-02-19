cmake_minimum_required(VERSION 3.9)
project(hi)

set(CMAKE_CXX_STANDARD 17)


include(MPI.cmake) ###### MPI

include(Boost.cmake) ###### Boost

include(GTest.cmake) ###### Google-Test


set(PROGRAMS src/main/hi.cpp src/test/testall.cpp)

set(SOURCE_FILES src/main/utils/MasterQ.cpp src/main/utils/MasterQ.h src/main/utils/WorkItem.cpp src/main/utils/WorkItem.h src/main/utils/vecutil.cpp src/main/debug.h src/main/datastructures/data.h src/main/global_includes.h)

foreach(program ${PROGRAMS})
    get_filename_component(progname ${program} NAME_WE)
    add_executable(${progname} ${SOURCE_FILES} ${program})
    target_link_libraries(${progname} gtest gtest_main)
    target_link_libraries(${progname} ${MPI_LIBRARIES})
    target_link_libraries(${progname} ${Boost_LIBRARIES} )
endforeach(program)

add_test(NAME Dummy COMMAND hi)
