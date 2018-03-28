
#ifndef HI_DNA_H
#define HI_DNA_H

#include <vector>

#include "data.hpp"

//#define HAMMING_DIST
#define NW_DIST

#define kMismatch	1
#define kGap		2

class DNA : public Data {
public:
    DNA(const std::string& sequence, const size_t& s);
    virtual ~DNA();

    virtual std::string toString() const;

    virtual size_t size() const { return seq_len_; }

    virtual dtype d(const Data* other) const;

    static size_t LoadData(const char* file, std::vector<Data*>* data_arr);

protected:
    char* seq_;
    unsigned int seq_len_;

    dtype hammingDist(const DNA* other) const;
    dtype nwDist(const DNA* other) const;

    void ensureSize(const DNA* self, const DNA* other);
};



#endif //HI_DNA_H
