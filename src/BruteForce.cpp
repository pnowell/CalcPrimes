
// local includes
#include "BruteForce.h"

// ================================================================================================
// Constructor
// ================================================================================================
BruteForce::BruteForce() : CalcMethod(), curr(2), currSqrt(1) {
}

// ================================================================================================
// Initialization
// ================================================================================================
void BruteForce::Init() {
}

// ================================================================================================
// Compute primes up to a limit
// ================================================================================================
void BruteForce::ComputePrimes(U64 limit) {
    while(curr <= limit) {
        // Check to see if curr is prime
        bool isPrime = true;
        // We don't need to check the primes that went into the sieve
        for(size_t i = 0; i < primes.size() && primes[i] <= currSqrt; ++i) {
            if(curr % primes[i] == 0) {
                isPrime = false;
                break;
            }
        }
        if(isPrime)
            primes.push_back(curr);

        // Move on to the next number
        ++curr;

        // Update the square root
        while(currSqrt * currSqrt <= curr)
            ++currSqrt;
    }
}

