
#pragma once

// local includes
#include "Sieve.h"
#include "Types.h"

// system includes
#include <vector>

// ================================================================================================
class BasicSieve {

public:

    // Constructor
    BasicSieve();

    // Compute primes up to a limit
    void ComputePrimes(std::vector<U64>& primes, U64 limit);

protected:

    Sieve sieve;
    size_t sievePos;
};

