
#ifndef HI_EUCDATAMANAGER_HPP
#define HI_EUCDATAMANAGER_HPP

#include <string>
#include <lshbox/basis.h>
#include <lshbox/metric.h>
#include <lshbox/topk.h>
#include <lshbox/eval.h>
#include <lshbox/matrix.h>
//#include <lshbox.h>
#include "../../globals.hpp"
#include "../rm/M.hpp"
#include "../indexes/controllers/Pivot.hpp"
#include "DataManager.hpp"
#include "../../utils/statutil.hpp"
#include "../../utils/vecutil.hpp"
#include "../datatypes/EucBare.hpp"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <unordered_set>

template<typename T>
class EucDataManager : public DataManager {

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar.template register_type< EucDataManager<T> >();
        ar & boost::serialization::base_object<DataManager>(*this);
        ar & m;
        ar & deleteData;
    }
    bool deleteData;
public:
    rm::M<T> m;

public:
    EucDataManager() : deleteData(true) {};

    EucDataManager(T *pdata, size_t rows, size_t cols, bool shouldTransfer, bool deleteData, std::vector<size_t> idxs) :
            DataManager(idxs),deleteData(deleteData) {
        if (shouldTransfer){
            m.transfer(pdata, rows, cols,deleteData);
        } else {
            m.load(pdata, rows, cols);
        }
    }
    EucDataManager(rm::M<T>& m, size_t R, bool shouldTransfer): DataManager(R), deleteData(true)  {
        if (shouldTransfer){
            this->m.transfer(m.getData(), m.getRows(), m.getCols(), true);
        } else {
            this->m.load(m.getData(), m.getRows(), m.getCols());
        }

    }

    EucDataManager(T* pdata, size_t rows, size_t cols, bool shouldTransfer, bool deleteData, size_t startingIdx):
            DataManager(rows, startingIdx), deleteData(deleteData){
        if (shouldTransfer){
            m.transfer(pdata, rows, cols, deleteData);
        } else {
            m.load(pdata, rows, cols);
        }
    }
    ~EucDataManager() override {
        if (!deleteData){
            m.setData(nullptr); /// this will prevent the natural deletion of the data
        }
    }

    DataManager *sliceData(size_t _begin, size_t _end) override {
//    dprintf("     ### slicing  %ld   %ld    %ld    %p\n", _begin, _end,   _end - _begin, m[_begin]);
        return new EucDataManager<T>(m[_begin], _end - _begin, getCols(), true, false,
                                  std::vector<size_t>(idxs.begin()+_begin, idxs.begin()+_end));
    }

    static EucDataManager* loadData(std::string filename){
        rm::M<T> m;
        m.load(filename);
        auto pd = new EucDataManager<T>(m, m.getRows(), true);
        m.setData(nullptr); /// prevent deletion on exit as we transferred
        return pd;
    }

    static EucDataManager* loadData(std::string filename, size_t only, size_t nsplits){
        rm::M<T> m;
        m.load(filename, only, nsplits);
        auto pd = new EucDataManager<T>(m, m.getRows(), true);
        m.setData(nullptr); /// prevent deletion on exit as we transferred
        return pd;
    }

    size_t getCols() const {
        return m.getDim();
    }


    void print() const {
        m.print();
    }
    void reorder(T* pdata, std::vector<Tup>& tups, size_t R, size_t C)
    {
        for (size_t i=0; i<R; ++i) {
            while (tups[i].sort_index != i) {
                std::swap_ranges(
                        &pdata[tups[i].sort_index*C],
                        &pdata[tups[i].sort_index*C+C],
                        &pdata[tups[tups[i].sort_index].sort_index*C]
//                    &pdata[std::get<0>(idxdist[std::get<0>(idxdist[i])])*C]
                );
                std::swap(tups[i].sort_index, tups[tups[i].sort_index].sort_index);
            }
        }
    }

    tstat<dist_type> stat(Euc<T>* ppivot){
        const size_t R = getRows();
        const size_t C = getCols();
        auto mdat = m.getData();
        tstat<dist_type> stat;
        #pragma omp parallel for
        for (size_t i = 0; i < R; ++i) {
            T dist_ = 0;
            for (unsigned j = 0; j != C; ++j) {
                dist_ += sqr( (*ppivot)[j] - mdat[i*C+j]);
            }
            const float d = std::sqrt(dist_);
            stat.addStat(d);
        }
        return stat;
    }

    void sort(Pivot& pivot, bool retainIdxs) override{
        const size_t R = getRows();
        if (pivot.pdistances == nullptr){
            pivot.pdistances = new std::vector<Tup>(R);
        }
        auto pdistances = pivot.pdistances;
        if (pdistances->size() != R){
            pdistances->resize(R);
        }

        const size_t C = getCols();

        assert(pivot.ppivot->size() == C);
        auto pivotdat = static_cast<Euc<T>*>(pivot.ppivot);
        auto mdat = m.getData();
        /// Calculate distance to the pivot for all points
        for (size_t i = 0; i < R; ++i) {
            T dist_ = 0;
            for (unsigned j = 0; j != C; ++j) {
                dist_ += sqr( (*pivotdat)[j] - mdat[i*C+j]);
            }
            const float d = std::sqrt(dist_);
//        dprintf(" -- %f   %zu   %zu\n", mdat[i*C], i, idxs[i]);
            (*pdistances)[i] = Tup(idxs[i], d, i);
        }

        std::sort(std::begin(*pdistances ), std::end(*pdistances ), [](auto &left, auto &right) {
            return left.distance < right.distance;
        });
        if (retainIdxs){
            /// copy over new index order
            for (size_t i=0; i<R; ++i){
                idxs[i] = (*pdistances)[i].oindex;
            }
        }
        /// copy new locations of Dat* array
        reorder(m.getData(), *pdistances, R, C);
    }

    void print_rowp() const {
        m.print_rowp();
    }

    friend std::ostream &operator<<(std::ostream &os, EucDataManager<T> &dm) {
        const size_t R = dm.getRows();
        const size_t C = dm.getCols();
        auto a = dm.getDat();
        os << " [";
        for (size_t __r = 0; __r< R; ++__r){
            os << "\t" << __r << "\t" << dm.idxs[__r] << "[ ";\
        for (size_t __c = 0; __c < C; ++__c) {
                os << a[__r*C+__c] << " ";
            }
            os << "]";
            if (__r != R-1)
                os << std::endl << "  ";
        }
        os << "]" << std::endl;
        return os;
    }

    std::vector<T*> getCandidateSet(){
        const size_t R = m.getRows();
        size_t n = R > PIV_MAX_CAND_SAMPLES ? PIV_MAX_CAND_SAMPLES : R;
        return vecutil::fft<T, dist_type>(R, m.getCols(), n, getDat(), rm::M<T>::dist);
    }

    std::vector<T*>* getEvaluationSet(std::vector<T*>& exclude_ptrs ){
        const size_t R = m.getRows();
        const size_t C = m.getCols();
        std::unordered_set<size_t> exclude;
        std::vector<T*>* pchoose_vect = new std::vector<T*>();
        #pragma omp parallel for
        for (size_t k = 0; k < exclude_ptrs.size(); ++k) {
            exclude.insert(size_t((exclude_ptrs[k] - m.getData())/C)); /// convert to index
        }
        #pragma omp parallel for
        for (size_t i = 0; i < R; ++i) {
            if (exclude.find(i) != exclude.end()){
                continue;}
            pchoose_vect->emplace_back(&m.getData()[i*C]);
        }
        size_t nr = pchoose_vect->size();
        size_t n = nr > PIV_MAX_EVAL_SAMPLES ? PIV_MAX_EVAL_SAMPLES : nr;
        if (n == nr){
            return pchoose_vect;
        }
        return vecutil::randRowPointers<T>(m.getCols(), n, *pchoose_vect);
    }

    tstat<dist_type> evaluate(Euc<T>* pivot, std::vector<T*>& eset, const size_t C ) {
        EucDataManager<T> dm(eset[0], eset.size(), C, true, false, 0);

        return dm.stat(pivot);
    }

    std::vector<Pivot *> pickPivots(int numPivots) override{
        std::vector<Pivot*> res(numPivots);
        if (numPivots == 0){
            return res;
        }

        const size_t R = m.getRows();
        const size_t C = m.getCols();
        if (numPivots >= R){
            auto pdat = m.getData();
            for (size_t i = 0; i < R; ++i) {
                auto pe = new Euc<T>(&pdat[i*C],C);
                res.emplace_back(new Pivot(pe, 0, R));
            }
            return res;
        }
        auto cset = getCandidateSet();
        std::vector<T*>* eset;
        if (cset.size() < PIV_MAX_CAND_SAMPLES){ /// we ate all the points
            const size_t ns = static_cast<size_t>(cset.size()/2);
            eset = new std::vector<T*>(cset.size() - ns);
            std::copy(cset.begin(), cset.begin() + ns, eset->begin());
            cset.resize(ns);
        } else {
            eset = getEvaluationSet(cset);
        }

        if (numPivots >= cset.size()){
            res.resize(cset.size());
            for (size_t i = 0; i < cset.size(); ++i) {
                res[i] = new Pivot(new Euc<T>(cset[i],C), 0, R);
            }
            return res;
        }

        assert(cset.size() > numPivots);
        assert(eset->size() > numPivots);
        Pivot* ppivot = nullptr;
        for (size_t i = 0; i <numPivots; ++i) {
            float bestDist = -1;
            for (size_t j = 0; j < cset.size(); ++j) {
                auto pe = new Euc<T>(cset[j],C);
                auto stat = evaluate(pe, *eset, C);

                if (stat.mean() > bestDist){
                    bestDist = stat.mean();
                    ppivot = new Pivot(pe, 0, R);
                    res[i] = ppivot;
                    auto idx = size_t((cset[j] - m.getData())/C);
//                    dprintf(" idx=%zu   ppivot=", idx);
//                    dprintca(pe->getDat(), 1, C);
                    ppivot->index = idx;
                }
            }
        }
        delete eset;
        return res;
    }

    void scan(Data *queryPoint, int i, hi::HIQueryResults& qr) override {
//    leafPoints->scan(queryPoint, depth, parent->queryResults);
        auto qp = static_cast<Euc<T>*>(queryPoint);
        const size_t C = getCols();
        for (size_t i = 0; i < getRows(); ++i) {
            ++qr.npointsInLeaf;
            ++qr.distcalcs;
            float d = rm::M<T>::dist(qp->getCoords(), m[i], C);
//        float d = rm::M<Dat>::dist((*this)[i], queryPoint, getCols());
//        dprintf("  adding leaf %lu    %f ,       %f:%f\n", leafPoints->idxs[i], d, leafPoints->m[i][0], leafPoints->m[i][1]);

            qr.add(idxs[i], d);
        }
    }

    T *getDat() const{
        return m.getData();
    }

    size_t getRows() const override{
        return m.getSize();
    }
    const Dat *operator[](size_t i) const{
        return m[i];
    }

    T *operator[](size_t i){
        return m[i];
    }

    T get(size_t r, size_t c){
        return m[r][c];
    }

    T* getRow(size_t r) {return m[r];}

    Data *getElement(size_t row) override{
        return new Euc<T>(m[row], m.getCols());
    }

public:


    DataManager* new_instance() override{
        return new EucDataManager<T>(getDat(), getRows(), getCols(), true, false, this->idxs);
    }

    Stat calculateStat() override {
        return statutil::calculateStats(getDat(), getRows(), getCols());
    }

    tstat<dist_type> calculateStat(Pivot* ppivot) override {
        return stat(static_cast<Euc<T>*>(ppivot->ppivot));
    }

    bool isEuclidean() override{
        return true;
    }
};

BOOST_CLASS_EXPORT_KEY(EucDataManager<float>);
BOOST_CLASS_EXPORT_KEY(EucDataManager<int>);

#endif //HI_EUCDATAMANAGER_HPP
