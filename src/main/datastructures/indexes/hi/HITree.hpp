
#ifndef HI_HITREE_HPP
#define HI_HITREE_HPP


#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include "HINode.hpp"
#include "../../controllers/DataManager.hpp"
#include "../controllers/IndexDecider.hpp"
#include "params.hpp"

namespace hi {

//class HINode<template float>;
//class HINode<char>;
class HINode;

/**
 * The DataManager here is the Matrix for all of the data in all the nodes
 */
class HITree {
    friend class HINode;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & id;
        ar & proot;
        ar & buildParams;
        ar & queryParams;
        ar & buildResults;
        ar & queryResults;
        ar & pdata;

    }

    DataManager *pdata;
    IndexDecider *pdecider;
    HINode *proot;

public:
    HIBuildResults buildResults;
    HIQueryResults queryResults;
    HIBuildParams buildParams;
    HIQueryParams queryParams;
    int id;

public:

    HITree() :
            pdata(nullptr), pdecider(nullptr), proot(nullptr), id(-1) {
    }


    explicit HITree(DataManager *pdata) :
            pdata(pdata), pdecider(new IndexDecider()), proot(nullptr), id(-1) {
    }

    HITree(DataManager *pdata, int id):
            pdata(pdata), pdecider(new IndexDecider()), proot(nullptr), id(id){

    }
    ~HITree();
    IndexDecider *getDecider() const {
        return pdecider;
    }


    DataManager *getDataManager() const {
        return pdata;
    }


    void setDecider(IndexDecider *pdecider) {
        if (pdecider)
            delete pdecider;
        this->pdecider = pdecider;
    }
    void setDataManager(DataManager *pdata);

    HINode *getRoot() const;
    size_t size() const;
    void setRoot(HINode *proot);

    void printQueryResults() ;

public:


    virtual void build(int depth);
    virtual void knnquery(Data* queryPoint, int depth);
};


}

#endif //HI_HITREE_HPP
