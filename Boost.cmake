
cmake_minimum_required(VERSION 2.8)
set(Boost_DEBUG ON)
find_package(Boost COMPONENTS serialization mpi REQUIRED )
# find with required parts find_package(Boost REQUIRED mpi serialization)


include_directories( ${Boost_INCLUDE_DIR} )


### put this below executable
#target_link_libraries( myprog ${Boost_LIBRARIES} )
