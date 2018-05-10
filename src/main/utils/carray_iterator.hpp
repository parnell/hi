
#ifndef HI_CARRAY_ITERATOR_HPP
#define HI_CARRAY_ITERATOR_HPP


#include <cstddef>

namespace carray_iterator {

//
//// non-const version
//template<class Cont>
//typename Cont::iterator begin(Cont& c){
//    return c.begin();
//}
//
//template<class Cont>
//typename Cont::iterator end(Cont& c){
//    return c.end();
//}
//
//// const version
//template<class Cont>
//typename Cont::const_iterator begin(Cont const& c){
//    return c.begin();
//}
//
//template<class Cont>
//typename Cont::const_iterator end(Cont const& c){
//    return c.end();
//}
//
//// overloads for C style arrays
//template<class T, std::size_t R, std::size_t C>
//T* begin(T* arr){
//    return arr;
//}
//
//template<class T, std::size_t R, std::size_t C>
//T* end(T* arr){
//    return arr + R*C;
//}




template <class T, size_t N>
T *begin(T (&array)[N]) {
    return array;
}

template <class T, size_t N>
T *end(T (&array)[N]) {
    return array + N;
}

};


#endif //HI_CARRAY_ITERATOR_HPP
