
// local includes
#include "Timer.h"
#include "Eratosthenes.h"
#include "ContinuedEratosthenes.h"

// system includes
#include <cstdio>
#include <vector>

static const size_t kReserveSize = 700000;
static const U64 kPrimeLimit = 10000000;
static const size_t kBudgets[] = {
    10000001,
    5000001,
    1000001,
    500001,
    100001,
    50001,
    10001,
    5001,
    1001,
    501,
    101,
    51,
    11,
};
static const size_t kNumBudgets = sizeof(kBudgets) / sizeof(kBudgets[0]);
static const char* kNames[] = {
    "10 million",
    "5 million",
    "1 million",
    "500 thousand",
    "100 thousand",
    "50 thousand",
    "10 thousand",
    "5 thousand",
    "1 thousand",
    "500",
    "100",
    "50",
    "10",
};
static const U64 kChunkSize = 10000;

// ================================================================================================
static void Analyze(CalcMethod& method, const char* name) {
    // Make sure the method has enough room for the primes it will generate before
    // we get started
    method.primes.reserve(kReserveSize);

    Timer timer;
    F32 secs = 0.0f;

    timer.Sample();
    method.Init();
    timer.Sample();
    secs += timer.Elapsed();
    printf("%s, %f, ", name, secs);

    for(U64 i = 0; i < kPrimeLimit;) {
        i += kChunkSize;
        if(i > kPrimeLimit)
            i = kPrimeLimit;

        // Time the calculation of the next group of primes
        timer.Sample();
        method.ComputePrimes(i);
        timer.Sample();

        secs += timer.Elapsed();
        printf("%f%s", secs, i == kPrimeLimit ? "\n" : ", ");
        fflush(stdout);
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

    // Analyze the normal eratosthenes method
    {
        Eratosthenes eratosthenes(kPrimeLimit);
        Analyze(eratosthenes, "Base");
    }

    // Run the ContinuedEratosthenes method once for each budget
    for(size_t i = 0; i < kNumBudgets; ++i) {
        // Eratosthenes with the ability to continue past the initially designated limit
        ContinuedEratosthenes eratosthenes(kBudgets[i]);
        Analyze(eratosthenes, kNames[i]);
    }

    return 0;
}
