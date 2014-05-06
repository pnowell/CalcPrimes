
// local includes
#include "Eratosthenes.h"

// ================================================================================================
// Constructor
// ================================================================================================
Eratosthenes::Eratosthenes(size_t h) : CalcMethod(), hardLimit(U32(h)), curr(2),
                                       sqrPastLimit(false) {
}

// ================================================================================================
// Initialization
// ================================================================================================
void Eratosthenes::Init() {
    // First create our array (we make a 2-based array, and we want to be able to tell if
    // hardLimit itself is a prime, so we just need an array of sized hardLimit - 1
    size_t size = hardLimit - 1;
    slots.resize(size);
    for(size_t i = 0; i < size; ++i) {
        Slot& s = slots[i];
        s.offset = 1;
        s.fromSlot = i - 1; // the from slot being incorrect for the first slot is fine
    }
}

// ================================================================================================
// Compute primes up to a limit
// ================================================================================================
void Eratosthenes::ComputePrimes(U64 limit) {
    U32 lim = U32(limit);
    if(lim >= hardLimit)
        lim = hardLimit;
    for(; curr <= lim; curr += slots[curr - 2].offset) {
        // The current number is a prime
        primes.push_back(curr);

        // Mark off it's multiples (starting with its square)
        if(sqrPastLimit)
            continue;

        U64 init = U64(curr) * U64(curr);
        if(init >= hardLimit) {
            sqrPastLimit = true;
            continue;
        }

        for(U32 m = U32(init); m <= hardLimit; m += curr) {
            U32 idx = m - 2;
            Slot& s = slots[idx];

            // If this slot has already been marked as non-prime, move on
            if(s.offset == 0)
                continue;

            // Get the slot that was pointing to us and point to what this slot had been
            Slot& from = slots[s.fromSlot];
            from.offset += s.offset;

            // If the thing we're pointing to is within the table, let it know who's pointing to it
            if(idx + s.offset <= hardLimit) {
                Slot& dest = slots[idx + s.offset];
                dest.fromSlot = s.fromSlot;
            }

            // Make sure we know not to mess with this one again
            s.offset = s.fromSlot = 0;
        }
    }
}

