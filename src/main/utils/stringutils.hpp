
#ifndef HI_STRINGUTILS_HPP
#define HI_STRINGUTILS_HPP

#include <string>
#include <iostream>
#include <memory>
#include <stdarg.h>  // For va_start, etc.

namespace sutil{
#if __cplusplus >= 201403L
    /**
 * Convert all std::strings to const char* using constexpr if (C++17)
 */
    template<typename T>
    auto convert(T&& t) {
        if constexpr (std::is_same<std::remove_cv_t<std::remove_reference_t<T>>, std::string>::value) {
            return std::forward<T>(t).c_str();
        }
        else {
            return std::forward<T>(t);
        }
    }

    /**
 * printf like formatting for C++ with std::string
 * Original source: https://stackoverflow.com/a/26221725/11722
 */
    template<typename ... Args>
    std::string stringFormatInternal(const std::string& format, Args&& ... args)
    {
        size_t size = snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args) ...) + 1;
        std::unique_ptr<char[]> buf(new char[size]);
        snprintf(buf.get(), size, format.c_str(), args ...);
        return std::string(buf.get(), buf.get() + size - 1);
    }

    template<typename ... Args>
    std::string sformat(std::string fmt, Args&& ... args) {
//        return stringFormatInternal(fmt, convert(std::forward<Args>(args))...);
    }

#else

    std::string sformat(const std::string fmt_str, ...) {
        int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
        std::unique_ptr<char[]> formatted;
        va_list ap;
        while(1) {
            formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
            strcpy(&formatted[0], fmt_str.c_str());
            va_start(ap, fmt_str);
            final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
            va_end(ap);
            if (final_n < 0 || final_n >= n)
                n += abs(final_n - n + 1);
            else
                break;
        }
        return std::string(formatted.get());
    }

#endif

}


#endif //HI_STRINGUTILS_HPP
