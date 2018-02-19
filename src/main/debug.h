
#ifndef HI_DEBUG_H
#define HI_DEBUG_H

#include <cstdio>

#ifdef DEBUG
extern size_t kNDIST_COMP;
#define DPRINTF(file, fmt, args...) fprintf(file, fmt, ##args)
#else
#define DPRINTF(file, fmt, args...)
#endif

extern int print_tabs;
#define print_indent() print_tabs++
#define print_undent() print_tabs--
#define print_tabbed(fmt,args...) \
	for(int i=0; i<print_tabs; ++i) fprintf(stdout, "  "); \
	fprintf(stdout, fmt, ##args)



#include <sys/time.h>
inline double When() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return ((double) tp.tv_sec + (double) tp.tv_usec * 1e-6);
}

#endif //HI_DEBUG_H
