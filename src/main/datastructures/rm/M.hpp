
#ifndef HI_M_HPP
#define HI_M_HPP
#include <fstream>
#include <vector>
#include <assert.h>
#include <string.h>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/array.hpp>

#include <iostream>
#include "../../globals.hpp"

namespace rm {


/**
 * Dataset management class. A dataset is maintained as a matrix in memory.
 *
 * The file contains N D-dimensional vectors of single precision floating point numbers.
 *
 * Such binary files can be accessed using lshbox::Matrix<double>.
 */
template<class T>
class M {
    friend class boost::serialization::access;

    size_t dim;
    size_t N;

    T *dims;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & dim;
        ar & N;
        if (Archive::is_loading::value) {
            dims = new T[dim*N];
        }
        ar & boost::serialization::make_array(dims,dim*N);
    }

public:
    /**
     * Reset the size.
     *
     * @param _dim Dimension of each vector
     * @param _N   Number of vectors
     */
    void reset(size_t _dim, size_t _N) {
        dim = _dim;
        N = _N;
        if (dims != NULL) {
            delete[] dims;
        }
        dims = new T[dim * N];
    }

    M() : dim(0), N(0), dims(NULL) {}

    M(size_t _dim, size_t _N) : dims(NULL) {
        reset(_dim, _N);
    }

    ~M() {
        if (dims != NULL) {
            delete[] dims;
        }
    }

    /**
     * Access the ith vector.
     */
    inline const T *operator[](size_t i) const {
        return dims + i * dim;
    }

    /**
     * Access the ith vector.
     */
    inline T *operator[](size_t i) {
        return dims + i * dim;
    }

    /**
     * Get the dimension.
     */
    inline size_t getDim() const { return dim; }
    inline size_t getCols() const { return dim; }

    /**
     * Get the size.
     */
    inline size_t getSize() const { return N; }
    inline size_t getRows() const { return N; }

    /**
     * Get the data.
     */
    inline T *getData() const {
        return dims;
    }

    /**
     * Load the Matrix from a binary file.
     */
    void load(const std::string &path) {
        std::ifstream is(path.c_str(), std::ios::binary);
        unsigned header[3];
        assert(sizeof header == 3 * 4);
        is.read((char *) header, sizeof(header));
        reset(header[2], header[1]);
        is.read((char *) dims, sizeof(T) * dim * N);
        is.close();
    }

    /**
     * Load the Matrix from std::vector<T>.
     *
     * @param vec  The reference of std::vector<T>.
     * @param _N   Number of vectors
     * @param _dim Dimension of each vector
     */
    void load(std::vector<T> &vec, size_t _N, size_t _dim) {
        reset(_dim, _N);
        memcpy(dims, (void *) &vec[0], sizeof(T) * dim * N);
    }

    /**
     * Load the Matrix from T*.
     *
     * @param source The pointer to T*.
     * @param _N     Number of vectors
     * @param _dim   Dimension of each vector
     */
    void load(T *source, size_t _N, size_t _dim) {
        reset(_dim, _N);
        memcpy(dims, source, sizeof(T) * dim * N);
    }

    /**
 * Transfer the Matrix from T*.
 *
 * @param source The pointer to T*.
 * @param _N     Number of vectors
 * @param _dim   Dimension of each vector
 */
    void transfer(T *source, size_t _N, size_t _dim) {
        dims = source;
        dim = _dim;
        N = _N;
    }

    /**
     * Save the Matrix as a binary file.
     */
    void save(const std::string &path) {
        std::ofstream os(path.c_str(), std::ios::binary);
        size_t header[3];
        header[0] = sizeof(T);
        header[1] = N;
        header[2] = dim;
        os.write((char *) header, sizeof header);
        os.write((char *) dims, sizeof(T) * dim * N);
        os.close();
    }

    M(const std::string &path) : dims(NULL) {
        load(path);
    }

    M(const M &M) : dims(NULL) {
        reset(M.getDim(), M.getSize());
        memcpy(dims, M.getData(), sizeof(T) * dim * N);
    }

    M &operator=(const M &M) {
        dims = NULL;
        reset(M.getDim(), M.getSize());
        memcpy(dims, M.getData(), sizeof(T) * dim * N);
        return *this;
    }

    float dist(const Dat *vec1, const Dat *vec2) const
    {
        float dist_ = 0.0;
        for (unsigned i = 0; i != dim; ++i)
        {
            dist_ += sqr(vec1[i] - vec2[i]);
        }
        return std::sqrt(dist_);
    }

    /**
     * An accessor class to be used with LSH index.
     */
    class Accessor {
        const M &matrix_;
        std::vector<bool> flags_;
    public:
        typedef unsigned Key;
        typedef const T *Value;
        typedef T DATATYPE;

        Accessor(const M &matrix) : matrix_(matrix) {
            flags_.resize(matrix_.getSize());
        }

        void reset() {
            flags_.clear();
            flags_.resize(matrix_.getSize());
        }

        bool mark(unsigned key) {
            if (flags_[key]) {
                return false;
            }
            flags_[key] = true;
            return true;
        }

        const T *operator()(unsigned key) {
            return matrix_[key];
        }
    };


    void print() const {
        const size_t R = getSize();
        const size_t C = getDim();
        for (size_t r= 0; r< R; ++r){
            for (size_t c = 0; c < C; ++c) {
                std::cout << dims[r*C+c] << "  ";
            }
            std::cout << std::endl;
        }
    }
    void print_rowp() const {
        const size_t R = getSize();
        const size_t C = getDim();
        for (size_t r= 0; r< R; ++r){
            std::cout << dims[r*C] << std::endl;
        }
    }

    /**
     * WARNING! Setting this without corresponding ROW and SIZE is extremely
     * dangerous. Only used if confident you know what you are doing
     * @param pdat
     */
    void setData(Dat *pdat) {
        dims = pdat;
    }
};
}

#endif //HI_M_HPP
