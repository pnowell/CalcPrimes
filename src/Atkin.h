
#pragma once

// local includes
#include "CalcMethod.h"

// ================================================================================================
class Atkin : public CalcMethod {

public:

    // Constructor
    Atkin(size_t hardLimit);

    // Initialization
    virtual void Init();

    // Compute primes up to a limit
    virtual void ComputePrimes(U64 limit);

    // The name of this method
    virtual const char* Name() const {
        return "Atkin";
    }

protected:

    size_t hardLimit;
    U32 curr;
    std::vector<U32> flags;
};

