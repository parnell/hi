
#ifndef HI_FILEUTIL_HPP
#define HI_FILEUTIL_HPP

#include <string>
#include <iostream>
#include <unistd.h>

#define VERIFY_WITH_MSG(cond, msg) do{ \
    if (!(cond)) { \
        std::ostringstream str; str << msg; std::cerr << str.str(); std::cerr << std::endl; BOOST_VERIFY(cond); }\
    } while(0)



namespace futil{

inline bool file_exists(const std::string& name) {
    return ( access( name.c_str(), F_OK ) != -1 );
}


inline bool file_writable(const std::string& name) {
    std::ofstream ofs(name);
    return ofs.is_open();
}

}

#endif //HI_FILEUTIL_HPP
