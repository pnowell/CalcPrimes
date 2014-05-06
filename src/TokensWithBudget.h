
#pragma once

// local includes
#include "CalcMethod.h"
#include "Sieve.h"

// system includes
#include <cstdio>

// ================================================================================================
class TokensWithBudget : public CalcMethod {

public:

    // Constructor / destructor
    TokensWithBudget(size_t budget);
    ~TokensWithBudget();

    // Initialization
    virtual void Init();

    // Compute primes up to a limit
    virtual void ComputePrimes(U64 limit);

    // The name of this method
    virtual const char* Name() const {
        return "TokensWithBudget";
    }

protected:

    struct Token {
        U64 prime;
        size_t sievePos;
    };

    size_t budget;
    Sieve sieve;
    size_t sievePos;
    U64 curr;
    Token** list;
    U64 listBase;
    U64 listSize;
    bool overflowSwitch;
    FILE* overflowFile;

    // Place the given token (with the given value) at it's next location
    void PlaceToken(Token* t, U64 place);

    // Repopulate the list from our waiting list file
    void LoadList();
};

