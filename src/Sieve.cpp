
// local includes
#include "Sieve.h"

// ================================================================================================
// Constructor
// ================================================================================================
Sieve::Sieve() : sieveSize(1), offsets() {
    offsets.push_back(1);
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

// ================================================================================================
// Get the initial offset you need to start using a sieve (also return the value to start
// at instead of "v" in case v didn't align with a spot in the sieve)
// ================================================================================================
U64 Sieve::GetInitialOffset(U64 v, size_t& index) const {
    // reduce the value to its offset from the beginning of the sieve
    U64 remainder = U64(v % sieveSize);

    // handle the 0 and 1 case specially
    if(remainder == 0 || remainder == 1) {
        index = 0;
        return v + (1 - remainder);
    }

    // otherwise subtract out the implicit one
    --remainder;

    // for each index
    size_t size = offsets.size();
    for(index = 1; index < size; ++index) {
        // check to see if the remainder is less than the last index
        if(remainder <= offsets[index-1])
            return v + (offsets[index-1] - remainder);

        // otherwise decrement it and move on
        remainder -= offsets[index-1];
    }

    // if we made it out here, the remainder is within the last offset
    index = 0;
    return v + (offsets[index] - remainder);
}

