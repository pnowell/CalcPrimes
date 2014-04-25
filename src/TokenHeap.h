
#pragma once

// local includes
#include "CalcMethod.h"
#include "Sieve.h"

// ================================================================================================
class TokenHeap : public CalcMethod {

public:

    // Constructor
    TokenHeap();

    // Compute primes up to a limit
    virtual void ComputePrimes(U64 limit);

    // The name of this method
    virtual const char* Name() const {
        return "TokenHeap";
    }

protected:

    struct Token {
        U64 prime;
        U64 val;
        size_t sievePos;
    };

    Sieve sieve;
    size_t sievePos;
    U64 curr;
    std::vector<Token> tokens;

    // Methods for managing the heap of tokens
    void FloatUp(size_t idx);
    void FloatDown(size_t idx);
};

