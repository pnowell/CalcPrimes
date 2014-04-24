
// local includes
#include "Sieve.h"

// ================================================================================================
// Constructor
// ================================================================================================
Sieve::Sieve() : sieveSize(0), offsets() {
}

// ================================================================================================
// Destructor
// ================================================================================================
Sieve::~Sieve() {
}

// ================================================================================================
// Add a prime
// ================================================================================================
void Sieve::AddPrime(U64 p) {
    // If this is the first prime we're adding, do our initial setup
    if(sieveSize == 0) {
        sieveSize = p;
        offsets.resize(p - 1, 1);
        offsets[p - 2] = 2;
    }
    // Otherwise grow the list of offsets
    else {
        sieveSize *= p;
        size_t size = offsets.size();
        offsets.resize(size * p, 0);

        // iterate through and copy the original list
        for(U64 rep = 1; rep < p; ++rep)
            for(size_t i = 0; i < size; ++i)
                offsets[size * size_t(rep) + i] = offsets[i];

        // iterate again to combine offsets that lead to multiples of p
        U64 sum = 1;
        size = offsets.size();
        for(size_t i = 0; i < size; ++i) {
            sum += offsets[i];
            while(sum % p == 0) {
                sum += offsets[i+1];
                offsets[i] += offsets[i+1];
                offsets.erase(offsets.begin() + (i+1));
                --size;
            }
        }
    }
}

