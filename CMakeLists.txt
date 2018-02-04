cmake_minimum_required(VERSION 3.9)
project(hi)

set(CMAKE_CXX_STANDARD 17)

find_package(MPI REQUIRED)
if (MPI_FOUND)
    include_directories(SYSTEM ${MPI_INCLUDE_PATH})
#    set(CMAKE_C_COMPILER /usr/local/bin/mpicc)
#    set(CMAKE_CXX_COMPILER /usr/local/bin/mpic++)

    #    set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
#    set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})
else (MPI_FOUND)
    message(SEND_ERROR "This application cannot compile without MPI")
endif (MPI_FOUND)

###### Google-Test
include(GTest.cmake)

add_executable(hi src/main.cpp)

target_link_libraries(hi gtest gmock_main)
target_link_libraries(hi ${MPI_LIBRARIES})

add_test(NAME example_test COMMAND hi)
