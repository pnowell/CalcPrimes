
// local includes
#include "BasicSieve.h"
#include "Timer.h"

// system includes
#include <vector>
#include <cstdio>

static const U64 kPrimeLimit = 500;

// ================================================================================================
int main(int argc, const char* argv[]) {
    // Initialize timing functionality
    Timer::StaticInit();

    std::vector<U64> baseSoln;
    Timer timer;

    // Time the basic sieve method
    timer.Sample();
    BasicSieve basicSieve;
    basicSieve.ComputePrimes(baseSoln, kPrimeLimit);
    timer.Sample();

    // Print how long it took
    printf("Basic Sieve method took %f seconds\n", timer.Elapsed());

    // Print the primes
    size_t numPrimes = baseSoln.size();
    for(size_t i = 0; i < numPrimes; ++i)
        printf("%llu ", baseSoln[i]);
    printf("\n");

    return 0;
}
