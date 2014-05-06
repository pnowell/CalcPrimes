
// local includes
#include "BasicSieve.h"
#include "SievePrimes.h"

// ================================================================================================
// Constructor
// ================================================================================================
BasicSieve::BasicSieve() : CalcMethod(), sieve(), sievePos(0), curr(1), currSqrt(1) {
}

// ================================================================================================
// Initialization
// ================================================================================================
void BasicSieve::Init() {
    for(size_t i = 0; i < kNumSievePrimes; ++i)
        sieve.AddPrime(kSievePrimes[i]);

    primes.resize(kNumSievePrimes);
    for(size_t i = 0; i < kNumSievePrimes; ++i)
        primes[i] = kSievePrimes[i];

    NextNumber();
}

// ================================================================================================
// Compute primes up to a limit
// ================================================================================================
void BasicSieve::ComputePrimes(U64 limit) {
    while(curr <= limit) {
        // Check to see if curr is prime
        bool isPrime = true;
        // We don't need to check the primes that went into the sieve
        for(size_t i = kNumSievePrimes; i < primes.size() && primes[i] < currSqrt; ++i) {
            if(curr % primes[i] == 0) {
                isPrime = false;
                break;
            }
        }
        if(isPrime)
            primes.push_back(curr);

        NextNumber();
    }
}

// ================================================================================================
void BasicSieve::NextNumber() {
    // Move on to the next number
    curr += sieve.Offset(sievePos);
    // Update the offset index
    if(++sievePos == sieve.NumOffsets())
        sievePos = 0;

    // Update the square root
    while(currSqrt * currSqrt <= curr)
        ++currSqrt;
}

