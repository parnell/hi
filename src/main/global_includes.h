
#ifndef HI_GLOBAL_INCLUDES_H
#define HI_GLOBAL_INCLUDES_H

typedef float dist_type;
#define dist_type_f	"%.4f"
#define dist_type_inf "%f"

extern unsigned int kMaxLeafPoints;

class Data;

#include <vector>

extern std::vector<Data*> vGlobData;

#endif //HI_GLOBAL_INCLUDES_H
