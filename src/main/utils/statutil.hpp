
#ifndef HI_VARUTIL_HPP
#define HI_VARUTIL_HPP

#include <vector>
#include <complex>
#include <iostream>
#include "../datastructures/Stat.hpp"

namespace statutil {

template <typename T>
double variance(T* p, size_t len, bool isSample=true)
{
    double sq_sum = 0.0;
    std::complex<double> sum = 0.0;
    for (int i = 0; i < len; i++, p++) {
        sum += *p;
        sq_sum += std::norm(*p);
    }
    return isSample ? (sq_sum - std::norm(sum) / len) / (len-1) :(sq_sum - std::norm(sum) / len) / (len);
}


template <typename T>
Stat calculateVariance(T* points, const size_t size, const size_t D)
{
    Stat stat(size, D);

    /// i = 0, i_n = 1
//    std::copy_n(m[0], m.getDim(), avg);
    for (size_t i = 0; i < D; ++i) {
        stat.mean[i] = points[i];
        stat.sum[i] += points[i];
    }
    /// i = 1, i_n = 0.5
    if (size >= 2) {
        //assert(points[1].dim() == D);
        for (int d = D - 1; d >= 0; --d) {
            stat.sum[d] += points[1 * D + d];
            float const delta = points[1 * D + d] - stat.mean[d];
            stat.mean[d] += delta * 0.5f;
            stat.var[d] = delta * (points[1 * D + d] - stat.mean[d]);
        }
    } else {
//        std::fill_n(var, D, 0.0f);
        for (size_t i = 0; i < D; ++i) {
            stat.var[i] = 0.0f; }
    }


    // i = 2, ...
    for (size_t i = 2; i < size; ) {
        {
            const float i_n = 1.0f / (1.0f + i);
            //assert(points[i].dim() == D);
            for (size_t d = 0; d < D; ++d) {
                stat.sum[d] += points[i * D + d];
                float const delta = points[i * D + d] - stat.mean[d];
                stat.mean[d] += delta * i_n;
                stat.var[d] += delta * (points[i * D + d] - stat.mean[d]);
            }
        }
        ++i;

        if (i >= size) break;
        {
            const float i_n = 1.0f / (1.0f + i);
            //assert(points[i].dim() == D);
            for (long int d = D - 1; d >= 0; --d) {
                stat.sum[d] += points[i * D + d];
                float const delta = points[i * D + d] - stat.mean[d];
                stat.mean[d] += delta * i_n;
                stat.var[d] += delta * (points[i * D + d] - stat.mean[d]);
            }
        }
        ++i;
    }

    for (int j = 0; j < D; ++j) {
        stat.var[j] /= size;
    }
    return stat;
}

template <typename T>
Stat calculateVariance(std::vector<T*> points, const size_t D)
{
    const size_t size = points.size();
    Stat stat(size, D);

//    const Dat* points = m.getData();
    /// i = 0, i_n = 1
//    std::copy_n(m[0], m.getDim(), avg);
    for (size_t i = 0; i < D; ++i) {
        stat.mean[i] = points[0][i];
        stat.sum[i] += points[0][i];
    }

    T* row;
    /// i = 1, i_n = 0.5
    if (size >= 2) {
        //assert(points[1].dim() == D);
        for (int d = D - 1; d >= 0; --d) {
            stat.sum[d] += points[1][d];
            float const delta = points[1][d] - stat.mean[d];
            stat.mean[d] += delta * 0.5f;
            stat.var[d] = delta * (points[1][d] - stat.mean[d]);
        }
    } else {
//        std::fill_n(var, D, 0.0f);
        for (size_t i = 0; i < D; ++i) {
            stat.var[i] = 0.0f; }
    }


    // i = 2, ...
    for (size_t i = 2; i < size; ) {
        row = points[i];
        {
            const float i_n = 1.0f / (1.0f + i);
            //assert(points[i].dim() == D);
            for (size_t d = 0; d < D; ++d) {
                stat.sum[d] += row[d];
                float const delta = row[d] - stat.mean[d];
                stat.mean[d] += delta * i_n;
                stat.var[d] += delta * (row[d] - stat.mean[d]);
            }
        }
        ++i;

        row = points[i];
        if (i >= size) break;
        {
            const float i_n = 1.0f / (1.0f + i);
            //assert(points[i].dim() == D);
            for (long int d = D - 1; d >= 0; --d) {
                stat.sum[d] += row[d];
                float const delta = row[d] - stat.mean[d];
                stat.mean[d] += delta * i_n;
                stat.var[d] += delta * (row[d] - stat.mean[d]);
            }
        }
        ++i;
    }

    for (int j = 0; j < D; ++j) {
        stat.var[j] /= size;
    }
    return stat;
}

};

#endif //HI_VARUTIL_HPP
