
// local includes
#include "TokenHeap.h"
#include "SievePrimes.h"

// ================================================================================================
// Constructor
// ================================================================================================
TokenHeap::TokenHeap() : CalcMethod(), sieve() {
    for(size_t i = 0; i < kNumSievePrimes; ++i)
        sieve.AddPrime(kSievePrimes[i]);

    // Add the sieve primes, plus one more
    primes.resize(kNumSievePrimes + 1);
    for(size_t i = 0; i < kNumSievePrimes; ++i)
        primes[i] = kSievePrimes[i];
    primes[kNumSievePrimes] = kNextPostSievePrime;

    // Go ahead and set up the token for the first non-sieve prime so that we don't have to
    // worry about the case of having no tokens during our iteration
    tokens.push_back(Token());
    Token& t = tokens.back();
    t.prime = kNextPostSievePrime;
    t.val = t.prime * (1 + sieve.Offset(0));
    t.sievePos = 1;

    curr = 1 + sieve.Offset(0) + sieve.Offset(1);
    sievePos = 2;
}

// ================================================================================================
// Compute primes up to a limit
// ================================================================================================
void TokenHeap::ComputePrimes(U64 limit) {
    while(curr <= limit) {
        // If the current number is less than the next token, this number is prime
        if(curr < tokens[0].val) {
            // Record this number as a prime
            primes.push_back(curr);

            // Generate a token for this value
            size_t idx = tokens.size();
            tokens.push_back(Token());
            Token& t = tokens[idx];
            t.prime = curr;
            t.val = t.prime * (1 + sieve.Offset(0));
            t.sievePos = 1;

            FloatUp(idx);
        }
        // otherwise there's a token on the current number, meaning it has a divisor
        else {
            // For all the tokens that have this number as their value, increment their value
            // and float them down the heap
            while(curr == tokens[0].val) {
                Token& t = tokens[0];
                t.val += t.prime * sieve.Offset(t.sievePos);
                if(++t.sievePos == sieve.NumOffsets())
                    t.sievePos = 0;
                FloatDown(0);
            }
        }

        curr += sieve.Offset(sievePos);
        if(++sievePos == sieve.NumOffsets())
            sievePos = 0;
    }
}

// ================================================================================================
// Methods for managing the heap of tokens
// ================================================================================================
void TokenHeap::FloatUp(size_t idx) {
    while(idx > 0) {
        size_t p = (idx - 1) >> 1;

        Token& t = tokens[idx];
        Token& parent = tokens[p];

        // If we're in order, we can stop
        if(parent.val <= t.val)
            break;

        // Otherwise we need to swap with our parent and move on
        Token swap = t;
        t = parent;
        parent = swap;

        // Move up to the parent
        idx = p;
    }
}

// ================================================================================================
void TokenHeap::FloatDown(size_t idx) {
    size_t size = tokens.size();

    while(true) {
        Token& t = tokens[idx];
        size_t c = idx * 2 + 1;

        // If we have no children, we're done
        if(c >= size)
            break;

        // Update c to the lesser of the two children
        if(c + 1 < size && tokens[c + 1].val < tokens[c].val)
            c++;

        // If we're less than the lesser child, we're done
        if(t.val <= tokens[c].val)
            break;

        // Swap
        Token& child = tokens[c];
        Token swap = t;
        t = child;
        child = swap;

        // Move to our new location
        idx = c;
    }
}

