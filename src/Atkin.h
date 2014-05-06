
#pragma once

// local includes
#include "CalcMethod.h"
#include "Sieve.h"

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

    U64 hardLimit;
    U64 curr;
    std::vector<U64> flags;
    Sieve sieve;
    size_t sievePos;

    // Utilities for accessing and modifying flags
    bool GetFlag(U64 n);
    void FlipFlag(U64 n);
    void ClearFlag(U64 n);
};

// ================================================================================================
// Utilities for accessing and modifying flags
// ================================================================================================
inline bool Atkin::GetFlag(U64 n) {
    U64 idx = n / 64LL;
    U64 bit = n % 64LL;
    return (flags[idx] & (1LL << bit)) != 0;
}

inline void Atkin::FlipFlag(U64 n) {
    U64 idx = n / 64LL;
    U64 bit = n % 64LL;
    U64& f = flags[idx];
    f = f ^ (1LL << bit);
}

inline void Atkin::ClearFlag(U64 n) {
    U64 idx = n / 64LL;
    U64 bit = n % 64LL;
    U64& f = flags[idx];
    f = f & ~(1LL << bit);
}

