
#pragma once

// local includes
#include "Types.h"

// system includes
#include <vector>

// ================================================================================================
class Sieve {

public:

    // Constructor / destructor
    Sieve();
    ~Sieve();

    // Add a prime to the sieve (assumes the passed in number is a prime and is unique from all
    // other primes passed to this method)
    void AddPrime(U64 p);

    // Get the initial offset you need to start using a sieve (also return the value to start
    // at instead of "v" in case v didn't align with a spot in the sieve)
    U64 GetInitialOffset(U64 v, size_t& index) const;

    // Access to offset array
    size_t NumOffsets() const;
    U64 Offset(size_t i) const;

protected:
    
    U64 sieveSize;
    std::vector<U64> offsets;
};

// ================================================================================================
// Access to offset array
// ================================================================================================
inline size_t Sieve::NumOffsets() const {
    return offsets.size();
}

inline U64 Sieve::Offset(size_t i) const {
    return offsets[i];
}

