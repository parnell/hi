
cmake_minimum_required(VERSION 2.8)
set(Boost_DEBUG ON)

#set(BOOST_ROOT "$ENV{HOME}/usr/local/boost-1.66.0" CACHE PATH "/home1/01208/parnell/usr/local/boost-1.66.0" )
find_package(Boost COMPONENTS serialization mpi REQUIRED )
# find with required parts find_package(Boost REQUIRED mpi serialization)


include_directories( ${Boost_INCLUDE_DIR} )


### put this below executable
#target_link_libraries( myprog ${Boost_LIBRARIES} )
