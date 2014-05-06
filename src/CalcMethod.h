
#pragma once

// local includes
#include "Types.h"

// system includes
#include <vector>

// ================================================================================================
// Abstract class for implementing methods of computing primes
// ================================================================================================
class CalcMethod {

public:

    std::vector<U64> primes;

    // Initialization
    virtual void Init() = 0;

    // Abstract method for computing primes up to a certain limit
    virtual void ComputePrimes(U64 limit) = 0;

    // Abstract method for getting the name of this method
    virtual const char* Name() const = 0;
};

