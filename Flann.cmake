
if(APPLE)
    link_directories(/usr/local/include/flann)
elseif(UNIX)
    link_directories($ENV{HOME}/usr/local/flann-1.8.4/include/flann)
endif()

