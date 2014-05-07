
// local includes
#include "BasicSieve.h"
#include "BruteForce.h"
#include "Timer.h"
#include "TokenHeap.h"
#include "TokensWithBudget.h"
#include "TokensWithBudget2.h"
#include "Eratosthenes.h"
#include "ContinuedEratosthenes.h"
#include "Atkin.h"

// system includes
#include <cstdio>
#include <vector>

static const U64 kPrimeLimit = 10 * 1000 * 1000;
static const U64 kChunkSize = 10 * 1000;
static const size_t kBudget = 1000 * 1000;

// ================================================================================================
static void Analyze(CalcMethod& method, std::vector<U64>* correct = NULL) {
    Timer timer;
    F32 sum = 0.0f;

    timer.Sample();
    method.Init();
    timer.Sample();
    sum += timer.Elapsed();

    printf("%s, %f, ", method.Name(), sum);

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

    {
        // Same as above, but it keeps a secondary list plus one file per range to try to mitigate
        // the stalls from the previous method
        TokensWithBudget2 tokensWithBudget(kBudget);
        Analyze(tokensWithBudget, &bruteForce.primes);
    }

    {
        // Classic sieve of Eratosthenes
        Eratosthenes eratosthenes(kPrimeLimit);
        Analyze(eratosthenes, &bruteForce.primes);
    }

    {
        // Eratosthenes with the ability to continue past the initially designated limit
        ContinuedEratosthenes eratosthenes(kBudget);
        Analyze(eratosthenes, &bruteForce.primes);
    }

    {
        // Implementation of the sieve of atkin
        Atkin atkin(kPrimeLimit);
        Analyze(atkin, &bruteForce.primes);
    }

    return 0;
}
