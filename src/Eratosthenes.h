
#pragma once

// local includes
#include "CalcMethod.h"
#include "Sieve.h"

// ================================================================================================
class Eratosthenes : public CalcMethod {

public:

    // Constructor
    Eratosthenes(size_t hardLimit);

    // Initialization
    virtual void Init();

    // Compute primes up to a limit
    virtual void ComputePrimes(U64 limit);

    // The name of this method
    virtual const char* Name() const {
        return "Eratosthenes";
    }

protected:

    U64 hardLimit;
    U64 curr;
    std::vector<U64> flags;

    // Utilities for accessing and modifying flags
    bool GetFlag(U64 n);
    void SetFlag(U64 n);
};

// ================================================================================================
// Utilities for accessing and modifying flags
// ================================================================================================
inline bool Eratosthenes::GetFlag(U64 n) {
    U64 idx = n / 64LL;
    U64 bit = n % 64LL;
    return (flags[idx] & (1LL << bit)) != 0;
}

inline void Eratosthenes::SetFlag(U64 n) {
    U64 idx = n / 64LL;
    U64 bit = n % 64LL;
    U64& f = flags[idx];
    f = f | (1LL << bit);
}

