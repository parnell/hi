
#ifndef HI_EUC_HPP
#define HI_EUC_HPP


#include "../data.hpp"
#include <fstream>
#include <sstream>
#include <cmath>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/container/vector.hpp>

template<typename T>
T sqr2(T x) { return x * x; }

template<typename T>
class Euc : public Data {

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar.template register_type< Euc >();
        ar & boost::serialization::base_object<Data>(*this);
//        ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Data);
        ar & coords;
    }
    std::vector<T> coords;
    T* pdat = nullptr;

public:

    Euc() : Data(){

    }

    Euc(T* dat, size_t C) : Data(), coords(dat,dat+C){
    }

    Euc(std::vector<T>& coords) : Data(), coords(coords.begin(), coords.end()){

    }
    virtual ~Euc(){

    }

    void setCoords(std::vector<T>& points){
        std::copy(coords.begin(), coords.end(), points.begin());
    }

    std::vector<T>& getCoords(){
        return coords;
    }

    const std::vector<T>& getCoords() const{
        return coords;
    }

    T* getDat(){
        if (pdat == nullptr){
            pdat = new T[coords.size()];
            std::copy(coords.begin(), coords.end(), pdat);
        }
        return pdat;
    }

    inline const T operator[](size_t i) const{
        return coords[i];
    }

    inline T operator[](size_t i){
        return coords[i];
    }

    std::string toString() const override{
        std::stringstream str;
//        str << serial_number_ << ": ";
        return str.str();
    };

    size_t size() const override { return coords.size(); }

    float d(const Data* other) const override{
        float accum = 0.0f;
        auto& oc = static_cast<const Euc*>(other)->coords;
        for (int i = 0; i < coords.size(); ++i) {
            accum += sqr2(coords[i] - oc[i]);
        }
        return sqrt(accum);
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

        inf.close();
        return data_arr->size();
    };



    friend std::ostream& operator<<(std::ostream& os, Euc const& o) {
        return os << o.toString();
    }
};


BOOST_CLASS_EXPORT_KEY(Euc<float>);
BOOST_CLASS_EXPORT_KEY(Euc<int>);




#endif //HI_EUC_HPP
