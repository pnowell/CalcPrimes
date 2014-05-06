
#pragma once

// local includes
#include "CalcMethod.h"
#include "Sieve.h"

// ================================================================================================
class BasicSieve : public CalcMethod {

public:

    // Constructor
    BasicSieve();

    // Initialization
    virtual void Init();

    // Compute primes up to a limit
    virtual void ComputePrimes(U64 limit);

    // The name of this method
    virtual const char* Name() const {
        return "BasicSieve";
    }

protected:

    Sieve sieve;
    size_t sievePos;
    U64 curr;
    U64 currSqrt;

    void NextNumber();
};

