
find_package(HDF5 REQUIRED COMPONENTS C CXX)

if(NOT HDF5_INCLUDE_DIRS)
    message(SEND_ERROR "couldn't find include dirs!!!!!!")
endif(NOT HDF5_INCLUDE_DIRS)

include_directories(${HDF5_INCLUDE_DIRS})

### Put the following in CmakeLists.txt in link
#target_link_libraries(example ${_hdf5_libs})
