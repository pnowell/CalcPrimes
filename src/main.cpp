
// local includes
#include "Types.h"
#include "BasicSieve.h"

// system includes
#include <vector>
#include <cstdio>

static const U64 kPrimeLimit = 500;

// ================================================================================================
int main(int argc, const char* argv[]) {
    BasicSieve basicSieve;
    std::vector<U64> baseSoln;
    basicSieve.ComputePrimes(baseSoln, kPrimeLimit);
    size_t numPrimes = baseSoln.size();
    for(size_t i = 0; i < numPrimes; ++i)
        printf("%llu ", baseSoln[i]);
    printf("\n");
    return 0;
}
