#include "dna.hpp"

#include <cassert>
#include <cstring>
#include <fstream>
#include <sstream>

#include "../../debug.hpp"
BOOST_CLASS_EXPORT_IMPLEMENT(DNA)

using std::string;

// This is going to be memory-leaked, but there will only be one of them, so
// it won't be a big deal.
static dist_type* dp_arr_ = 0;
static size_t dp_arr_size_ = 0;

DNA::DNA(const string& sequence, const size_t& s) : Data() {
    seq_ = new char[sequence.size()];
    seq_len_ = sequence.size();
    strncpy(seq_, sequence.c_str(), seq_len_);
}
DNA::~DNA() {
    delete[] seq_;
    seq_len_ = 0;
}

string DNA::toString() const {
    std::stringstream str;
    str << ": " << seq_;
    return str.str();
}

inline unsigned int min(unsigned int a, unsigned int b) {
    return a < b ? a : b;
}
inline unsigned int abs(unsigned int a, unsigned int b) {
    return a >= b ? a - b : b - a;
}

float DNA::d(const Data* other) const {
#ifdef DEBUG
    kNDIST_COMP++;
#endif
#ifdef HAMMING_DIST
    return hammingDist(static_cast<const DNA*>(other));
#else
    return nwDist(static_cast<const DNA*>(other));
#endif
}

dist_type DNA::hammingDist(const DNA* other) const {
    dist_type dist = 0;

    unsigned int min_size = seq_len_ < other->seq_len_ ? seq_len_ : other->seq_len_;

    for (unsigned int i=0; i<min_size; i++) {
        if (seq_[i] != other->seq_[i])
            dist += kMismatch;
    }

    // if they're not the same, return the difference
    dist += abs(seq_len_, other->seq_len_)*kMismatch;
    return dist;
}

void DNA::ensureSize(const DNA* self, const DNA* other) {
    if (dp_arr_size_ < (self->size()+1) * (other->size()+1)) {
        if (dp_arr_)
            delete[] dp_arr_;
        dp_arr_ = new dist_type[(self->size()+1) + (other->size()+1)];
        dp_arr_size_ = (self->size()+1) * (other->size()+1);
    }
}

inline dist_type min(dist_type a, dist_type b) {
    if (a < b) return a; return b;
}
inline dist_type max(dist_type a, dist_type b) {
    if (a > b) return a; return b;
}
inline dist_type min3(dist_type a, dist_type b, dist_type c) {
    return min(min(a,b),c);
}

dist_type DNA::nwDist(const DNA* other) const {
    const DNA* dna = static_cast<const DNA*>(other);

    // dp_matrix:
    //    d n a
    // s |_|_|_|
    // e |_|_|_|
    // l |_|_|_|
    // f |_|_|_|

    dist_type* dp_matrix = new dist_type[(dna->seq_len_+1) * (seq_len_+1)];
    size_t width = dna->seq_len_+1;

    for (size_t i=0; i<seq_len_+1; ++i) {
        for (size_t j=0; j<dna->seq_len_+1; ++j) {
            if (i == 0) {
                dp_matrix[i*width + j] = j*kGap;
            } else if (j == 0) {
                dp_matrix[i*width + j] = i*kGap;
            } else {
                dist_type same_val = seq_[i-1] == dna->seq_[j-1] ? 0 : kMismatch;
                dp_matrix[i*width + j] =
                        min3(dp_matrix[(i-1)*width + j] + kGap,
                             dp_matrix[i*width + (j-1)] + kGap,
                             dp_matrix[(i-1)*width + (j-1)] + same_val);
            }
        }
    }

    /*
    for (unsigned int i=0; i<seq_len_+1; ++i) {
        for (unsigned int j=0; j<dna->seq_len_+1; ++j) {
            fprintf(stderr, "%f\t", dp_matrix[i*width + j]);
        }
        fprintf(stderr, "\n");
    }
    */

    dist_type answer = dp_matrix[(seq_len_+1) * (dna->seq_len_+1) - 1];
    delete[] dp_matrix;
    return answer;
}

void toLower(string& str) {
    for (unsigned int i=0; i<str.size(); ++i)
        str[i] = tolower(str[i]);
}

size_t DNA::LoadData(const char* file, std::vector< Data* >* data_arr) {
    string name, seq;
    std::ifstream inf(file, std::ios::in);

    size_t i = 0;
    while (getline(inf, seq)) {
        if (!seq.empty() && seq[0] == '>') {
            // Skip the '>' at the beginnings
//            getline(inf, seq);
            continue;
        }

        if (seq.empty())
            continue;

        toLower(seq);
        DNA* dna = new DNA(seq, i++);

        data_arr->push_back(dna);
    }

    inf.close();
    return data_arr->size();
}




#if COMPILE_TESTS
#include "gtest/gtest.h"
#include "../../utils/Timer.hpp"
#include "../../utils/stringutils.hpp"
#include "../../loaders/loader.hpp"
#include "../../dprint.hpp"
#include <limits>

TEST(data, string_dna_test_load)
{
    std::string filename = sutil::sformat("%s/../data/tests/1028.dna", CMAKE_CURRENT_BINARY_DIR);
    {
//        Timer("DNA Load Time");
        std::vector<Data*> index;
        Loader<DNA>::LoadData(filename.c_str(), &index);
        EXPECT_EQ(1028, index.size());
//        dcoutl(index.size());
    }
}

#endif
