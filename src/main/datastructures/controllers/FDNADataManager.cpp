
#include "FDNADataManager.hpp"


FDNADataManager *FDNADataManager::loadData(std::string filename, size_t only, size_t nsplits) {
    auto pdata_arr = new std::vector< Data* >();
    DNA::LoadData(filename.c_str(), pdata_arr );
    return new FDNADataManager(pdata_arr);
}


#if COMPILE_TESTS
#include "gtest/gtest.h"
#include "../../utils/stringutils.hpp"

TEST(controllers, FDNA_test_serialization)
{
    std::string filename = sutil::sformat("%s/../data/tests/1028.dna", CMAKE_CURRENT_BINARY_DIR);
    FDNADataManager *pdat = FDNADataManager::loadData(filename, 0, 0);
    EXPECT_TRUE(pdat != nullptr);
    EXPECT_EQ(pdat->size(), 1028);
    {
        std::ofstream ofs("FDNA_test_serialization.idx");
        boost::archive::text_oarchive oa(ofs);
        oa << *pdat;
    }
    FDNADataManager *pdat2;
    {
        std::ifstream ifs("FDNA_test_serialization.idx");
        boost::archive::text_iarchive ia(ifs);
        ia >> pdat2;
    }
    EXPECT_EQ(pdat->size(), pdat2->size());

}


TEST(controllers, FDNA_test_to_euc)
{
    std::string filename = sutil::sformat("%s/../data/tests/1028.dna", CMAKE_CURRENT_BINARY_DIR);
    FDNADataManager *pdat = FDNADataManager::loadData(filename, 0, 0);
    EucDataManager<bool>* peuc = pdat->toEuclidean<bool>();
    EXPECT_TRUE(peuc != nullptr);
    EXPECT_EQ(peuc->getRows(), 1028);
    EXPECT_EQ(peuc->getCols(), 5*4); /// only 5 length dna sequences (*4 converted to bits)
}
#endif