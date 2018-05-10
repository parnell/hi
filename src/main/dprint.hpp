
#ifndef HI_GLOBAL_DEBUG_PRINT_H
#define HI_GLOBAL_DEBUG_PRINT_H

#if DEBUG_PRINT
#include <cstdio>
#define dcerr(x) (std::cerr << (x))
#define dcout(x) (std::cout << (x))
#define dcoutl(x) (std::cout << (x) << std::endl)
#define dprintf(fmt, args...) printf(fmt, ##args)
#define dprintca(a,R,C) do {\
        std::cout << " [";\
for (size_t __r = 0; __r< R; ++__r){ \
    std::cout << "[ ";\
    for (size_t __c = 0; __c < C; ++__c) { \
            std::cout << a[__r*C+__c] << " "; } \
        std::cout << "]";\
        if (__r != R-1) std::cout << std::endl << "  "; \
    } std::cout << "]" << std::endl; \
} while (0)

/** Print c style array row pointers */
#define dprintcap(a,R,C) do {\
    for (size_t __r= 0; __r< R; ++__r){ \
        std::cout << a[__r*C] << std::endl; \
    } \
} while (0)

#else
#define dcerr(x)
#define dcout(x)
#define dcoutl(x)
#define dprintf(fmt, args...)
#define dprintca(a,R,C)
#define dprintcap(a,R,C)
#endif
#endif //HI_GLOBAL_INCLUDES_H
