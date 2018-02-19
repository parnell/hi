#include "vecutil.hpp"

namespace vecutil{
    TEST(utils, splitvec)
    {
        std::vector<int> v(101) ;
        int nparts = 2;
        for (int i =0; i<nparts;i++){
            std::vector<int> ret = vecutil::split<int>(v, nparts, i);
            if (i==0) EXPECT_EQ(ret.size(), 51);
            else if (i==1) EXPECT_EQ(ret.size(), 50);
        }
        nparts = 3;
        for (int i =0; i<nparts;i++){
            std::vector<int> ret = vecutil::split<int>(v, nparts, i);
            if (i==0) EXPECT_EQ(ret.size(), 34);
            else if (i==1) EXPECT_EQ(ret.size(), 34);
            else if (i==2) EXPECT_EQ(ret.size(), 33);
        }
        nparts = 4;
        for (int i =0; i<nparts;i++){
            std::vector<int> ret = vecutil::split<int>(v, nparts, i);
            if (i==0) EXPECT_EQ(ret.size(), 26);
            else if (i==1) EXPECT_EQ(ret.size(), 25);
            else if (i==2) EXPECT_EQ(ret.size(), 25);
            else if (i==3) EXPECT_EQ(ret.size(), 25);
        }

    }


}

