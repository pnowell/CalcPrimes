
// local includes
#include "Atkin.h"
#include "Math.h"

// ================================================================================================
// Constructor
// ================================================================================================
Atkin::Atkin(size_t h) : CalcMethod(), hardLimit(U64(h)), flags(), sieve() {
}

// ================================================================================================
// Initialization
// ================================================================================================
void Atkin::Init() {
    // Allocate enough U64s and initialize them all to zero
    flags.resize(hardLimit / 64 + 1, 0);

    // Find all the solutions to the three equations flipping bits as necessary
    U64 sqrt = ISqrt(hardLimit);
    for(U64 x = 1; x <= sqrt; ++x) {
        for(U64 y = 1; y <= sqrt; ++y) {
            U64 n = 4 * x * x + y * y;
            if(n <= hardLimit) {
                U64 mod = n % 12;
                if(mod == 1 || mod == 5)
                    FlipFlag(n);
            }

            n = 3 * x * x + y * y;
            if(n <= hardLimit && (n % 12) == 7)
                FlipFlag(n);

            if(x > y) {
                n = 3 * x * x - y * y;
                if(n <= hardLimit && (n % 12) == 11)
                    FlipFlag(n);
            }
        }
    }

    primes.push_back(2);
    primes.push_back(3);
    primes.push_back(5);

    sieve.AddPrime(2);
    sieve.AddPrime(3);
    sieve.AddPrime(5);

    curr = 7;
    sievePos = 1;
}

// ================================================================================================
// Compute primes up to a limit
// ================================================================================================
void Atkin::ComputePrimes(U64 limit) {
    if(limit > hardLimit)
        limit = hardLimit;
    while(curr <= limit) {
        // Check to see if this number is prime
        if(GetFlag(curr)) {
            primes.push_back(curr);
            U64 sqr = curr * curr;
            for(U64 sqrMult = sqr; sqrMult <= hardLimit; sqrMult += sqr)
                ClearFlag(sqrMult);
        }

        // Move on to the next number
        curr += sieve.Offset(sievePos);
        // Update the offset index
        if(++sievePos == sieve.NumOffsets())
            sievePos = 0;
    }
}

