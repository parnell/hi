
include_directories(${HDF5_INCLUDE_DIRS})
set(_hdf5_libs hdf5 hdf5_cpp)


### Put the following in CmakeLists.txt in link
#target_link_libraries(example ${_hdf5_libs})