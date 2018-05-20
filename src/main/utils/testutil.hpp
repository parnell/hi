
#ifndef HI_TESTUTIL_HPP
#define HI_TESTUTIL_HPP


namespace testutil {

template <typename T>
T* makeM(const int R, const int C){
    auto m = new T[R * C];

    for (int r= 0; r< R; ++r){
        for (int c = 0; c < C; ++c) {
            m[r*C+c] = r*(c+1); }
    }
    return m;
}

template <typename T>
T* makeMZC(const int R, const int C){
    auto m = new T[R * C];
    T max = R*(C+1);
    for (int r= 0; r< R; ++r){
        for (int c = 0; c < C; ++c) {
            m[r*C+c] = (r*(c+1) - (R*(c+1))/2.0f)/max; }
    }
    return m;
}

};


#endif //HI_TESTUTIL_HPP
