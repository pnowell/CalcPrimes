
// local includes
#include "BruteForce.h"
#include "BasicSieve.h"
#include "Timer.h"

// system includes
#include <vector>
#include <cstdio>

static const U64 kPrimeLimit = 1000000;
static const U64 kChunkSize = 100000;

// ================================================================================================
static void Analyze(CalcMethod& method, std::vector<U64>* correct = NULL) {
    printf("%s\t", method.Name());

    Timer timer;
    F32 sum = 0.0f;
    for(U64 i = 0; i < kPrimeLimit;) {
        i += kChunkSize;
        if(i > kPrimeLimit)
            i = kPrimeLimit;

        // Time the calculation of the next group of primes
        timer.Sample();
        method.ComputePrimes(i);
        timer.Sample();

        sum += timer.Elapsed();
        printf("%f%s", sum, i == kPrimeLimit ? "\n" : "\t");
    }

    // Check the primes against the correct list, if provided
    if(correct != NULL) {
        size_t correctSize = correct->size();
        size_t methodSize = method.primes.size();
        if(correctSize != methodSize)
            printf("ERROR - Method %s computed %u primes (should have %u)\n",
                   method.Name(), U32(methodSize), U32(correctSize));
        for(size_t i = 0, j = 0; j < methodSize || i < correctSize;) {
            if(j >= methodSize || (i < correctSize && (*correct)[i] < method.primes[j])) {
                printf("ERROR - Should have %llu but didn't include it\n", (*correct)[i]);
                ++i;
            }
            else if(i >= correctSize || (j < methodSize && (*correct)[i] > method.primes[j])) {
                printf("ERROR - Included %llu but shouldn't have\n", method.primes[j]);
                ++j;
            }
            else {
                ++i;
                ++j;
            }
        }
    }
}

// ================================================================================================
int main(int argc, const char* argv[]) {
    // Initialize timing functionality
    Timer::StaticInit();

    // Brute force solution (used as a correct list of primes for everything else)
    BruteForce bruteForce;
    Analyze(bruteForce);

    // Basic sieve usage, still dividing by recorded primes up to the square root
    BasicSieve basicSieve;
    Analyze(basicSieve, &bruteForce.primes);

    return 0;
}
