
#ifndef HI_HI2_H
#define HI_HI2_H

#include <flann/flann.hpp>
#include <flann/io/hdf5.h>
#include <flann/util/allocator.h>
#include <lshbox.h>
#include "Forest.hpp"
#include "../rm/WorkItem.hpp"
#include <flann/util/random.h>
#include <flann/util/params.h>
#include <flann/algorithms/nn_index.h>

namespace hi {

    using namespace flann;

    struct HIIndexParams : public IndexParams {
        HIIndexParams (int trees = 4) {
            (*this)["algorithm"] = FLANN_INDEX_HI;
            (*this)["trees"] = trees;
        }
    };


    template <typename Distance>
    class HI : public NNIndex<Distance> {

        friend class HIBuildItem;

        typedef typename Distance::ElementType ElementType;
        typedef typename Distance::ResultType DistanceType;
        typedef NNIndex<Distance> BaseClass;

        enum
        {
            /**
             * To improve efficiency, only SAMPLE_MEAN random values are used to
             * compute the mean and variance at each level when building a tree.
             * A value of 100 seems to perform as well as using all values.
             */
                    SAMPLE_MEAN = 100,
            /**
             * Top random dimensions to consider
             *
             * When creating random trees, the dimension on which to subdivide is
             * selected at random from among the top RAND_DIM dimensions with the
             * highest variance.  A value of 5 works well.
             */
                    RAND_DIM=5
        };


        /*--------------------- Internal Data Structures --------------------------*/
        struct Node
        {
            /**
             * Dimension used for subdivision.
             */
            int divfeat;
            /**
             * The values used for subdivision.
             */
            DistanceType divval;
            /**
             * Point data
             */
            ElementType* point;
            /**
             * The child nodes.
             */
            Node* child1, * child2;

            ~Node() {
                if (child1!=NULL) child1->~Node();
                if (child2!=NULL) child2->~Node();
            }

        private:
//        template<typename Archive>
//        void serialize(Archive& ar)
//        {
//            typedef KDTreeIndex<Distance> Index;
//            Index* obj = static_cast<Index*>(ar.getObject());
//
//            ar & divfeat;
//            ar & divval;
//
//            bool leaf_node = false;
//            if (Archive::is_saving::value) {
//                leaf_node = ((child1==NULL) && (child2==NULL));
//            }
//            ar & leaf_node;
//
//            if (leaf_node) {
//                if (Archive::is_loading::value) {
//                    point = obj->points_[divfeat];
//                }
//            }
//
//            if (!leaf_node) {
//                if (Archive::is_loading::value) {
//                    child1 = new(obj->pool_) Node();
//                    child2 = new(obj->pool_) Node();
//                }
//                ar & *child1;
//                ar & *child2;
//            }
//        }
//        friend struct serialization::access;
        }; /* ------------------------ END Node ------------------------ */

        /**
     * Pooled memory allocator.
     *
     * Using a pooled memory allocator is more efficient
     * than allocating memory directly when there is a large
     * number small of memory allocations.
     */
        PooledAllocator pool_;

        typedef Node* NodePtr;

        DistanceType* mean_;
        DistanceType* var_;

        std::vector<Forest *> hi;

        /**
         * Number of randomized trees that are used
         */
        int trees_;

        /**
     * Array of k-d trees used to find neighbours.
     */
        std::vector<NodePtr> tree_roots_;


    public:

        HI(const IndexParams& params = HIIndexParams (), Distance d = Distance() ) :
                BaseClass(params, d), mean_(NULL), var_(NULL)
        {
            trees_ = get_param(index_params_,"trees",4);
        }
        /**
     * KDTree constructor
     *
     * Params:
     *          inputData = dataset with the input features
     *          params = parameters passed to the kdtree algorithm
     */
        HI(const Matrix<ElementType>& dataset, const IndexParams& params = HIIndexParams(),
           Distance d = Distance() ) : BaseClass(params,d ), mean_(NULL), var_(NULL)
        {
            trees_ = get_param(index_params_,"trees",4);

            setDataset(dataset);
        }

        virtual void freeIndex(){

        }


        /**
         * Builds the index
         */
        void buildIndexImpl()
        {
            // Create a permutable array of indices to the input vectors.
            std::vector<int> ind(size_);
            for (size_t i = 0; i < size_; ++i) {
                ind[i] = int(i);
            }

            mean_ = new DistanceType[veclen_];
            var_ = new DistanceType[veclen_];

            tree_roots_.resize(trees_);
            /* Construct the randomized trees. */
            for (int i = 0; i < trees_; i++) {
                /* Randomize the order of vectors to allow for unbiased sampling. */
#if __cplusplus > 201406L
                std::shuffle(ind.begin(), ind.end(), std::default_random_engine());
#else
                std::random_shuffle(ind.begin(), ind.end());
#endif
                tree_roots_[i] = divideTree(&ind[0], int(size_) );
            }
            delete[] mean_;
            delete[] var_;
        }


        /**
         * Create a tree node that subdivides the list of vecs from vind[first]
         * to vind[last].  The routine is called recursively on each sublist.
         * Place a pointer to this new tree node in the location pTree.
         *
         * Params: pTree = the new node to create
         *                  first = index of the first vector
         *                  last = index of the last vector
         */
        NodePtr divideTree(int* ind, int count)
        {
            NodePtr node = new(pool_) Node(); // allocate memory

            /* If too few exemplars remain, then make this a leaf node. */
            if (count == 1) {
                node->child1 = node->child2 = NULL;    /* Mark as leaf node. */
                node->divfeat = *ind;    /* Store index of this vec. */
                node->point = points_[*ind];
            }
            else {
                int idx;
                int cutfeat;
                DistanceType cutval;
                meanSplit(ind, count, idx, cutfeat, cutval);

                node->divfeat = cutfeat;
                node->divval = cutval;
                node->child1 = divideTree(ind, idx);
                node->child2 = divideTree(ind+idx, count-idx);
            }

            return node;
        }


        /**
         * Choose which feature to use in order to subdivide this set of vectors.
         * Make a random choice among those with the highest variance, and use
         * its variance as the threshold value.
         */
        void meanSplit(int* ind, int count, int& index, int& cutfeat, DistanceType& cutval)
        {
            memset(mean_,0,veclen_*sizeof(DistanceType));
            memset(var_,0,veclen_*sizeof(DistanceType));

            /* Compute mean values.  Only the first SAMPLE_MEAN values need to be
                sampled to get a good estimate.
             */
            int cnt = std::min((int)SAMPLE_MEAN+1, count);
            for (int j = 0; j < cnt; ++j) {
                ElementType* v = points_[ind[j]];
                for (size_t k=0; k<veclen_; ++k) {
                    mean_[k] += v[k];
                }
            }
            DistanceType div_factor = DistanceType(1)/cnt;
            for (size_t k=0; k<veclen_; ++k) {
                mean_[k] *= div_factor;
            }

            /* Compute variances (no need to divide by count). */
            for (int j = 0; j < cnt; ++j) {
                ElementType* v = points_[ind[j]];
                for (size_t k=0; k<veclen_; ++k) {
                    DistanceType dist = v[k] - mean_[k];
                    var_[k] += dist * dist;
                }
            }
            /* Select one of the highest variance indices at random. */
            cutfeat = selectDivision(var_);
            cutval = mean_[cutfeat];

            int lim1, lim2;
            planeSplit(ind, count, cutfeat, cutval, lim1, lim2);

            if (lim1>count/2) index = lim1;
            else if (lim2<count/2) index = lim2;
            else index = count/2;

            /* If either list is empty, it means that all remaining features
             * are identical. Split in the middle to maintain a balanced tree.
             */
            if ((lim1==count)||(lim2==0)) index = count/2;
        }


        /**
         * Select the top RAND_DIM largest values from v and return the index of
         * one of these selected at random.
         */
        int selectDivision(DistanceType* v)
        {
            int num = 0;
            size_t topind[RAND_DIM];

            /* Create a list of the indices of the top RAND_DIM values. */
            for (size_t i = 0; i < veclen_; ++i) {
                if ((num < RAND_DIM)||(v[i] > v[topind[num-1]])) {
                    /* Put this element at end of topind. */
                    if (num < RAND_DIM) {
                        topind[num++] = i;            /* Add to list. */
                    }
                    else {
                        topind[num-1] = i;         /* Replace last element. */
                    }
                    /* Bubble end value down to right location by repeated swapping. */
                    int j = num - 1;
                    while (j > 0  &&  v[topind[j]] > v[topind[j-1]]) {
                        std::swap(topind[j], topind[j-1]);
                        --j;
                    }
                }
            }
            /* Select a random integer in range [0,num-1], and return that index. */
            int rnd = rand_int(num);
            return (int)topind[rnd];
        }

        /**
         *  Subdivide the list of points by a plane perpendicular on axe corresponding
         *  to the 'cutfeat' dimension at 'cutval' position.
         *
         *  On return:
         *  dataset[ind[0..lim1-1]][cutfeat]<cutval
         *  dataset[ind[lim1..lim2-1]][cutfeat]==cutval
         *  dataset[ind[lim2..count]][cutfeat]>cutval
         */
        void planeSplit(int* ind, int count, int cutfeat, DistanceType cutval, int& lim1, int& lim2)
        {
            /* Move vector indices for left subtree to front of list. */
            int left = 0;
            int right = count-1;
            for (;; ) {
                while (left<=right && points_[ind[left]][cutfeat]<cutval) ++left;
                while (left<=right && points_[ind[right]][cutfeat]>=cutval) --right;
                if (left>right) break;
                std::swap(ind[left], ind[right]); ++left; --right;
            }
            lim1 = left;
            right = count-1;
            for (;; ) {
                while (left<=right && points_[ind[left]][cutfeat]<=cutval) ++left;
                while (left<=right && points_[ind[right]][cutfeat]>cutval) --right;
                if (left>right) break;
                std::swap(ind[left], ind[right]); ++left; --right;
            }
            lim2 = left;
        }

        virtual void findNeighbors(ResultSet<DistanceType>& result, const ElementType* vec,
                                   const SearchParams& searchParams) const{

        }
        USING_BASECLASS_SYMBOLS
    };
}

#endif //HI_HI2_H
