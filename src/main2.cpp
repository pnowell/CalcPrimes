
// local includes
#include "Timer.h"
#include "TokensWithBudget2.h"
#include "ContinuedEratosthenes.h"

// system includes
#include <cstdio>
#include <vector>

struct STestInfo {
    U64 primeLimit;
    U64 chunkSize;
    size_t budget;
};

static const STestInfo kTests[] = {
    {100 * 1000 * 1000, 100 * 1000, 10 * 1000 * 1000}, // Used for reference method
    {64 * 1024 * 1024, 64 * 1024, 8 * 1024 * 1024},
    {100 * 1000 * 1000, 100 * 1000, 10 * 1000},
    {10000000000LL, 1000 * 1000, 10 * 1000 * 1000},
};
static const size_t kNumTests = sizeof(kTests) / sizeof(kTests[0]);

// Tests to interpolate between
static const STestInfo kInterpTestA = {
    1000 * 1000, 1000, 100 * 1000,
};
static const STestInfo kInterpTestB = {
    1000 * 1000, 100 * 1000, 1000,
};
static const U64 kNumInterpSteps = 50;

// ================================================================================================
static void PrintProgress(F32 progress, F32 time) {
    printf("\r[ ");
    const U32 totalSpaces = 40;
    U32 ticks = U32(totalSpaces * progress + 0.5f);
    for(U32 i = 0; i < ticks; ++i)
        printf("*");
    for(U32 i = ticks; i < totalSpaces; ++i)
        printf(" ");
    printf(" ] : %6.2f%% (%.2f seconds)", progress * 100.0f, time);
    fflush(stdout);
}

// ================================================================================================
static void Analyze(CalcMethod& method, const STestInfo& testInfo,
                    std::vector<U64>* ref = NULL, bool sizeShouldMatch = false) {
    Timer timer;
    F32 secs = 0.0f;

    timer.Sample();
    method.Init();
    timer.Sample();
    secs += timer.Elapsed();

    PrintProgress(0.0f, secs);

    for(U64 i = 0; i < testInfo.primeLimit;) {
        i += testInfo.chunkSize;
        if(i > testInfo.primeLimit)
            i = testInfo.primeLimit;

        // Time the calculation of the next group of primes
        timer.Sample();
        method.ComputePrimes(i);
        timer.Sample();

        secs += timer.Elapsed();
        PrintProgress(F32(i) / F32(testInfo.primeLimit), secs);
    }

    PrintProgress(1.0f, secs);
    printf("\nPrimes found : %llu\n", U64(method.primes.size()));

    if(ref == NULL)
        return;

    // Check the primes against the ref list, if provided
    size_t refSize = ref->size();
    size_t methodSize = method.primes.size();
    if(sizeShouldMatch && refSize != methodSize)
        printf("ERROR - Method %s computed %u primes (should have %u)\n",
               method.Name(), U32(methodSize), U32(refSize));
    for(size_t i = 0, j = 0; j < methodSize || i < refSize;) {
        if(!sizeShouldMatch && (j >= methodSize || i >= refSize))
            break;
        if(j >= methodSize || (i < refSize && (*ref)[i] < method.primes[j])) {
            printf("ERROR - Should have %llu but didn't include it\n", (*ref)[i]);
            ++i;
        }
        else if(i >= refSize || (j < methodSize && (*ref)[i] > method.primes[j])) {
            printf("ERROR - Included %llu but shouldn't have\n", method.primes[j]);
            ++j;
        }
        else {
            ++i;
            ++j;
        }
    }
}
// ================================================================================================
int main(int argc, const char* argv[]) {
    // Initialize timing functionality
    Timer::StaticInit();

    std::vector<U64> ref;
    {
        TokensWithBudget2 refMethod(kTests[0].budget);
        printf("Reference method (%s)\n", refMethod.Name());
        Analyze(refMethod, kTests[0]);
        printf("\n");

        // Copy the list of primes before destroying this method
        ref = refMethod.primes;
    }

    // Run the fixed tests
    for(size_t i = 0; i < kNumTests; ++i) {
        // Eratosthenes with the ability to continue past the initially designated limit
        ContinuedEratosthenes eratosthenes(kTests[i].budget);
        printf("Fixed test %u :\n  limit = %llu\n  chunk = %llu\n  budget = %llu\n",
               U32(i), kTests[i].primeLimit, kTests[i].chunkSize, U64(kTests[i].budget));
        Analyze(eratosthenes, kTests[i], &ref, i == 0);
        printf("\n");
    }

    // Run the interpolated tests
    for(U64 i = 0; i <= kNumInterpSteps; ++i) {
        F64 t = F64(i) / F64(kNumInterpSteps);
        STestInfo info;
        F64 diff = F64(kInterpTestB.primeLimit) - F64(kInterpTestA.primeLimit);
        info.primeLimit = U64(t * diff + kInterpTestA.primeLimit + 0.5);
        diff = F64(kInterpTestB.chunkSize) - F64(kInterpTestA.chunkSize);
        info.chunkSize = U64(t * diff + kInterpTestA.chunkSize + 0.5);
        diff = F64(kInterpTestB.budget) - F64(kInterpTestA.budget);
        info.budget = size_t(t * diff + kInterpTestA.budget + 0.5);

        ContinuedEratosthenes eratosthenes(info.budget);
        printf("Interpolated test %.2f :\n  limit = %llu\n  chunk = %llu\n  budget = %llu\n",
               t, info.primeLimit, info.chunkSize, U64(info.budget));
        Analyze(eratosthenes, info, &ref, false);
    }

    return 0;
}
