
#pragma once

// local includes
#include "CalcMethod.h"

// ================================================================================================
class BruteForce : public CalcMethod {

public:

    // Constructor
    BruteForce();

    // Initialization
    virtual void Init();

    // Compute primes up to a limit
    virtual void ComputePrimes(U64 limit);

    // The name of this method
    virtual const char* Name() const {
        return "BruteForce";
    }

protected:

    U64 curr;
    U64 currSqrt;
};

