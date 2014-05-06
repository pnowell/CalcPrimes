
#pragma once

// local includes
#include "CalcMethod.h"
#include "Sieve.h"
#include "LockFreeQueue.h"

// system includes
#include <cstdio>
#include <pthread.h>

// ================================================================================================
class TokensWithBudget2 : public CalcMethod {

public:

    // Constructor / destructor
    TokensWithBudget2(size_t budget);
    ~TokensWithBudget2();

    // Compute primes up to a limit
    virtual void ComputePrimes(U64 limit);

    // The name of this method
    virtual const char* Name() const {
        return "TokensWithBudget2";
    }

protected:

    struct Token {
        U64 prime;
        size_t sievePos;
    };

    struct TokenWithPlace {
        Token* t;
        U64 place;
        TokenWithPlace(Token* t_ = NULL, U64 p_ = 0) : t(t_), place(p_) {}
    };

    Sieve sieve;
    size_t sievePos;
    U64 curr;
    Token** list[2];

    // These should only change when the main thread is blocking and its known that its
    // not accessing any of them
    U32 whichList;
    U64 listBase;
    U64 listSize;

    // For sending tokens to the overflow thread
    LockFreeQueue<TokenWithPlace> queue;

    // For setup of overflow thread
    pthread_t overflowThread;
    bool threadStarted;
    U32 threadResult;

    // For coordination between main thread and overflow thread
    volatile bool threadJoining;
    volatile bool fillingSecondary;
    volatile bool nextList;

    // A pair of files used for storing / loading overflow tokens
    std::vector<FILE*> overflowFile;
    size_t overflowBaseIdx;

    // Place the given token (with the given value) at it's next location
    void PlaceToken(Token* t, U64 place);

    // Interface for the overflow thread
    // Static method to call the virtual run method
    static void* OverflowThreadCB(void* arg);
    U32 OverflowThread();

    // Add a token to be put into overflow
    void AddOverflowToken(Token* t, U64 place);

    // Waits until it's safe to switch lists and then switches
    // Called in the main thread and blocks on the overflow thread resetting "nextList"
    void NextList();

    // Place a token in the secondary list
    void PlaceSecondaryToken(Token* t, U64 place);

    // Method used by overflow thread to write a token to a particular overflow file
    void WriteToOverflowFile(U64 listIdx, Token* t, U64 place);
};

