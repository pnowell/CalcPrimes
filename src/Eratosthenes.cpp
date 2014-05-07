
// local includes
#include "Eratosthenes.h"

// ================================================================================================
// Constructor
// ================================================================================================
Eratosthenes::Eratosthenes(size_t h) : CalcMethod(), hardLimit(U64(h)), flags() {
}

// ================================================================================================
// Initialization
// ================================================================================================
void Eratosthenes::Init() {
    // Allocate enough U64s and initialize them all to zero
    flags.resize(hardLimit / 64 + 1, 0);
    curr = 2;
}

// ================================================================================================
// Compute primes up to a limit
// ================================================================================================
void Eratosthenes::ComputePrimes(U64 limit) {
    if(limit > hardLimit)
        limit = hardLimit;
    while(curr <= limit) {
        if(!GetFlag(curr)) {
            primes.push_back(curr);
            for(U64 i = curr * curr; i <= hardLimit; i += curr)
                SetFlag(i);
        }

        // Move on to the next number
        ++curr;
    }
}

