
if(APPLE)
    find_package (HDF5)

    include_directories(${HDF5_INCLUDE_DIRS})
    set(_hdf5_libs hdf5 hdf5_cpp)

    #    link_directories(/usr/local/include/flann)
    link_directories(/usr/local/Cellar/hdf5/1.10.1_2/lib)
    link_directories($ENV{HOME}/src/LSHBOX/include)

elseif(UNIX)
    find_package(HDF5 REQUIRED COMPONENTS C CXX)

    if(NOT HDF5_INCLUDE_DIRS)
        message(SEND_ERROR "couldn't find include dirs!!!!!!")
    endif(NOT HDF5_INCLUDE_DIRS)

    include_directories(${HDF5_INCLUDE_DIRS})
endif()

### Put the following in CmakeLists.txt in link
#target_link_libraries(example ${_hdf5_libs})
