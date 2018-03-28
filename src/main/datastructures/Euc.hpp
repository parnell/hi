
#ifndef HI_EUC_HPP
#define HI_EUC_HPP


#include "data.hpp"
#include <fstream>
#include <sstream>
#include <cmath>
#include <boost/serialization/string.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

template<typename T>
class Euc : public Data {
    std::vector<T> coords;
public:

    Euc() : Data(0){

    }

    Euc(std::vector<T>& coords) : Data(coords.size()){

    }
    virtual ~Euc(){

    }

    virtual std::string toString() const{
        std::stringstream str;
        str << serial_number_ << ": ";
        return str.str();
    };

    virtual size_t size() const { return coords.size(); }

    virtual dtype d(const Data* other) const{
        return -1;
    };

    double l2() {
        double accum = 0.;
        for (int i = 0; i < coords.size(); ++i) {
            accum += coords[i] * coords[i];
        }
        return sqrt(accum);
    }

    static size_t LoadData(const char* file, std::vector<Data*>* data_arr){
        std::string name, seq;
        std::ifstream inf(file, std::ios::in);


//    size_t i = 0;
//    while (getline(inf, name)) {
//        if (!name.empty() && name[0] == '>') {
//            // Skip the '>' at the beginning
//            getline(inf, seq);
//        } else {
//            seq = name;
//        }
//
//        if (seq.empty())
//            return data_arr->size();
//
//        toLower(seq);
//        DNA* dna = new DNA(seq, i++);
//
//        data_arr->push_back(dna);
//    }

        inf.close();
        return data_arr->size();
    };

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & serial_number_;
        ar & dist_;
        ar & coords;
    }

    friend std::ostream& operator<<(std::ostream& os, Euc const& o) {
        return os << o.toString();
    }
};


#endif //HI_EUC_HPP
