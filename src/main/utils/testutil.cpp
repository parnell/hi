
#include "testutil.hpp"

Dat* testutil::makeM(const int R, const int C){
    auto m = new Dat[R * C];

    for (int r= 0; r< R; ++r){
        for (int c = 0; c < C; ++c) {
            m[r*C+c] = r*(c+1); }
    }
    return m;
}
