
cmake_minimum_required(VERSION 2.8)

find_package(Boost REQUIRED)

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )


### put this below executable
#TARGET_LINK_LIBRARIES( myprog LINK_PUBLIC ${Boost_LIBRARIES} )
