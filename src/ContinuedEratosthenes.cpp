
// local includes
#include "ContinuedEratosthenes.h"
#include "Math.h"

// ================================================================================================
// Constructor
// ================================================================================================
ContinuedEratosthenes::ContinuedEratosthenes(size_t b) : CalcMethod(), budget(b), flags() {
}

// ================================================================================================
// Initialization
// ================================================================================================
void ContinuedEratosthenes::Init() {
    // Allocate enough U64s and initialize them all to zero
    flags.resize(budget / 64 + 1, 0);
    flagBase = 0;
    curr = 2;
    sqrtLimit = 0x00000000ffffffffLL;
    pastSqrtLimit = false;
}

// ================================================================================================
// Compute primes up to a limit
// ================================================================================================
void ContinuedEratosthenes::ComputePrimes(U64 limit) {
    // Safeguard against limits being requested out of order
    if(limit <= curr)
        return;

    // bound will be limit + 1 so that we only need to consider curr < bound rather than
    // curr <= limit
    // We also adjust it by flagBase to do all our calculations within our budget range
    U64 bound = limit + 1 - flagBase;

    // Do a full computation of our budget range over and over until we've exhausted the
    // remaining bound
    while(bound > 0) {
        U64 remainingBound = 0;
        if(bound > U64(budget)) {
            remainingBound = bound - budget;
            bound = budget;
        }

        U64 currBound = flagBase + bound;
        while(curr < currBound) {
            U64 adjusted = curr - flagBase;
            if(!GetFlag(adjusted)) {
                primes.push_back(curr);
                // Make sure we haven't passed the sqrt of the max value representable in U64
                pastSqrtLimit = pastSqrtLimit || curr > sqrtLimit;
                // If we have, we don't need to do any more flag setting (if we tried we'd overflow
                // our number and mess things up)
                if(pastSqrtLimit)
                    continue;

                // Mark off multiples starting at the square of this number
                for(U64 i = curr * curr - flagBase; i < budget; i += curr)
                    SetFlag(i);
            }

            // Move on to the next number
            ++curr;
        }

        // If there's no remaining bound, we're done
        if(remainingBound == 0)
            return;

        // Repeat the process with our remaining bound
        bound = remainingBound;

        // Clear all the flags
        size_t size = flags.size();
        for(size_t i = 0; i < size; ++i)
            flags[i] = 0;

        // Adjust our range up by the number of flags we have
        flagBase += budget;

        // Then go through all the primes up to the square root of the new bound to mark multiples
        size = primes.size();
        U64 realBoundSqrt = ISqrt(flagBase + budget);
        for(size_t i = 0; i < size; ++i) {
            U64 p = primes[i];
            if(p > realBoundSqrt)
                break;

            // We want to start marking flags at the sqaure of this prime
            U64 n = p * p;
            // If that square is greater than our flagBase, we adjust it down
            if(n >= flagBase)
                n -= flagBase;
            // Otherwise we need to just start on the first multiple of p greater than
            // or equal to flagBase, so we use (-flagBase % p), which is p - (flagBase % p)
            // remembering to leave the zero case alone (we don't need to negate it)
            else {
                n = flagBase % p;
                if(n != 0)
                    n = p - n;
            }

            // Mark off all the multiples of the current prime
            for(; n < budget; n += p)
                SetFlag(n);
        }
    }
}

