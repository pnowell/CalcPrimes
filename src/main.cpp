
// local includes
#include "BasicSieve.h"
#include "BruteForce.h"
#include "Timer.h"
#include "TokenHeap.h"
#include "TokensWithBudget.h"

// system includes
#include <cstdio>
#include <vector>

static const U64 kPrimeLimit = 100 * 1000 * 1000;
static const U64 kChunkSize = 1000 * 1000;
static const size_t kBudget = 512 << 10;

// ================================================================================================
static void Analyze(CalcMethod& method, std::vector<U64>* correct = NULL) {
    printf("%s, %f, ", method.Name(), 0.0f);

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
        printf("%f%s", sum, i == kPrimeLimit ? "\n" : ", ");
        fflush(stdout);
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

    // Print the table header
    printf("Name, 0, ");
    for(U64 i = 0; i < kPrimeLimit;) {
        i += kChunkSize;
        if(i > kPrimeLimit)
            i = kPrimeLimit;
        printf("%llu%s", i, i == kPrimeLimit ? "\n" : ", ");
    }

    // Brute force solution (used as a correct list of primes for everything else)
    BruteForce bruteForce;
    Analyze(bruteForce);

    {
        // Basic sieve usage, still dividing by recorded primes up to the square root
        BasicSieve basicSieve;
        Analyze(basicSieve, &bruteForce.primes);
    }

    {
        // Track tokens for each prime that visit all the multiples of that prime
        // New primes are numbers that you reach that have no tokens
        TokenHeap tokenHeap;
        Analyze(tokenHeap, &bruteForce.primes);
    }

    {
        // Instead of keeping a heap of tokens, put them in a fixed array and write
        // tokens to a scratch file when they fall off the back end to be loaded once
        // you reach the end of the list
        TokensWithBudget tokensWithBudget(kBudget);
        Analyze(tokensWithBudget, &bruteForce.primes);
    }

    return 0;
}
