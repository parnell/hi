
#ifndef HI_VECTUTIL_H
#define HI_VECTUTIL_H
#include <vector>
#include <iostream>


namespace vecutil{

    // Functor for deleting pointers in vector.
    template<class T> class DeleteVector {
    public:
        // Overloaded () operator.
        // This will be called by for_each() function.
        bool operator()(T x) const {
            // Delete pointer.
            delete x;
            return true;
        }
    };

    template <typename T>
    std::vector<T> split(std::vector<T>& v, const int nparts, const int iteration=0) {
        int workper = v.size() / nparts;
        int rem = v.size() % nparts;
        auto it = v.begin();
        int start = iteration < rem ? workper*iteration : (workper+1)*(rem) + workper*(iteration-rem);
        int end = start + (iteration < rem ? workper +1 : workper);
        std::cout << workper << " i=" <<iteration << "  " << start<< "   " << end << "  " << (end-start) << "      " << end - start << std::endl;
        return std::vector<T>(it + start, it + end);
    }
//
//        template <typename T>
//    std::vector< std::vector<T> > splitvector(std::vector<T>& v, int nparts){
//        int workper = v.size() / nparts;
//        int rem = v.size() % nparts;
//        std::vector< std::vector<T> > splits(nparts);
//        int cur = 0;
//        auto it = v.begin();
////        for (int i = 0; i < splits.size();++i){
////            int next = cur +  i>=rem ? workper : workper+1;
////            std::cout << "i=" << i << "   " << cur << " " << next << "  " << std::endl;
////            splits.insert(splits.begin()+i, std::vector<T>(it+cur, it+next));
////            cur += next;
////        }
////        std::vector< std::vector<T> > splits(nparts, std::vector<T>(workper+1));
////        for (int i = 0; i < splits.size();++i){
////            if (i >= rem){
////                splits[i].resize(splits[i].size() -1);
////            }
////        }
////        return splits;
//
//
//    }

}

#endif //HI_VECTUTIL_H
