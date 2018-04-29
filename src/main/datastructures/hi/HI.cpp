
#include "HI.hpp"
#include "../../utils/vecutil.hpp"
#include <lshbox/matrix.h>

namespace hi {
const int START_SAMPLE_SIZE = 5000;
const float SAMPLE_SIZE_SLOPE = 0.01f;
const int END_SAMPLE_SIZE = 10000;

void HI::build(DataManager *pdata, int max_depth) {
    /// calculate variance
    const size_t S = pdata->getRows();
    if (S > START_SAMPLE_SIZE) {
        int n = START_SAMPLE_SIZE + static_cast<int>(S * SAMPLE_SIZE_SLOPE);
        n = std::min(n, END_SAMPLE_SIZE);
        std::vector<Dat*> rows = vecutil::pickPointers(S, pdata->getCols(), n, pdata->getDat());
        pdata->calculateVariance(rows);
//        MAX_THRESHOLD = 0.9f;
//        v= (MEANOA + (MAX_THRESHOLD*VAROA) );

    } else {
        pdata->calculateVariance();
    }

}

}