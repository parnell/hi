#include <mpi.h>
#include <iostream>
#include "gtest/gtest.h"

// Simple test, does not use gmock
TEST(Dummy, foobar)
{
EXPECT_EQ(1, 1);
}

void rankf(int rank){
    printf("  RANK=%d\n", rank);
}


int main(int argc, char** argv) {
    int rank, size;
    std::cout << "rank=" << rank << "\tsize=" << size << std::endl;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    switch(rank){
        default:
            rankf(rank);
    }
    MPI_Finalize();
    return 0;
}
