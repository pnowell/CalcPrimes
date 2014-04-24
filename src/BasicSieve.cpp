
// local includes
#include "BasicSieve.h"
#include "SievePrimes.h"

// ================================================================================================
// Constructor
// ================================================================================================
BasicSieve::BasicSieve() : sieve(), sievePos(0) {
    for(size_t i = 0; i < kNumSievePrimes; ++i)
        sieve.AddPrime(kSievePrimes[i]);
}

// ================================================================================================
// Compute primes up to a limit
// ================================================================================================
void BasicSieve::ComputePrimes(std::vector<U64>& primes, U64 limit) {
    primes.resize(kNumSievePrimes);
    for(size_t i = 0; i < kNumSievePrimes; ++i)
        primes[i] = kSievePrimes[i];

    U64 curr = 1;
    U64 currSqrt = 1;
    while(true) {
        // Move on to the next number
        curr += sieve.Offset(sievePos);
        // Update the offset index
        if(++sievePos == sieve.NumOffsets())
            sievePos = 0;

        if(curr > limit)
            break;

        // Update the square root
        while(currSqrt * currSqrt <= curr)
            ++currSqrt;

        // Check to see if curr is prime
        bool isPrime = true;
        // We don't need to check the primes that went into the sieve
        for(size_t i = kNumSievePrimes; i < primes.size() && primes[i] <= currSqrt; ++i) {
            if(curr % primes[i] == 0) {
                isPrime = false;
                break;
            }
        }
        if(isPrime)
            primes.push_back(curr);
    }
}

