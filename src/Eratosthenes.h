
#pragma once

// local includes
#include "CalcMethod.h"

// ================================================================================================
class Eratosthenes : public CalcMethod {

public:

    // Constructor
    Eratosthenes(size_t hardLimit);

    // Compute primes up to a limit
    virtual void ComputePrimes(U64 limit);

    // The name of this method
    virtual const char* Name() const {
        return "Eratosthenes";
    }

protected:

    struct Slot {
        U32 offset;
        U32 fromSlot;
    };

    U32 curr;
    bool sqrPastLimit;
    std::vector<Slot> slots;
};

