
#pragma once

// local includes
#include "CalcMethod.h"
#include "Sieve.h"

// ================================================================================================
class ContinuedEratosthenes : public CalcMethod {

public:

    // Constructor
    ContinuedEratosthenes(size_t budget);

    // Initialization
    virtual void Init();

    // Compute primes up to a limit
    virtual void ComputePrimes(U64 limit);

    // The name of this method
    virtual const char* Name() const {
        return "ContinuedEratosthenes";
    }

protected:

    size_t budget;
    U64 curr;
    U64 flagBase;
    std::vector<U64> flags;
    U64 sqrtLimit;
    bool pastSqrtLimit;

    // Utilities for accessing and modifying flags
    bool GetFlag(U64 n);
    void SetFlag(U64 n);
};

// ================================================================================================
// Utilities for accessing and modifying flags
// ================================================================================================
inline bool ContinuedEratosthenes::GetFlag(U64 n) {
    U64 idx = n / 64LL;
    U64 bit = n % 64LL;
    return (flags[idx] & (1LL << bit)) != 0;
}

inline void ContinuedEratosthenes::SetFlag(U64 n) {
    U64 idx = n / 64LL;
    U64 bit = n % 64LL;
    U64& f = flags[idx];
    f = f | (1LL << bit);
}

