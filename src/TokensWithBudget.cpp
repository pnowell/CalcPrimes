
// local includes
#include "TokensWithBudget.h"
#include "SievePrimes.h"

// constants
static const char* kOverflowName[] = {
    ".overflow1",
    ".overflow2"
};

// ================================================================================================
// Constructor
// ================================================================================================
TokensWithBudget::TokensWithBudget(size_t budget) : CalcMethod(), sieve() {
    for(size_t i = 0; i < kNumSievePrimes; ++i)
        sieve.AddPrime(kSievePrimes[i]);

    // Add the sieve primes
    primes.resize(kNumSievePrimes);
    for(size_t i = 0; i < kNumSievePrimes; ++i)
        primes[i] = kSievePrimes[i];

    // Create our list
    list = new Token*[budget];
    for(size_t i = 0; i < budget; ++i)
        list[i] = NULL;
    listBase = 0;
    listSize = budget;

    // Set ourselves up to process the first candidate
    curr = 1 + sieve.Offset(0);
    sievePos = 1;

    // Set up our overflow file
    overflowSwitch = false;
    overflowFile = ::fopen(kOverflowName[overflowSwitch ? 0 : 1], "wb");
}

// ================================================================================================
// Destructor
// ================================================================================================
TokensWithBudget::~TokensWithBudget() {
    if(list != NULL) {
        // Delete any remaining tokens
        for(U64 i = curr - listBase; i < listSize; ++i) {
            if(list[i] != NULL)
                delete list[i];
        }

        delete [] list;
        list = NULL;
    }

    // Close our overflow file
    if(overflowFile != NULL) {
        ::fclose(overflowFile);
        overflowFile = NULL;
    }

    // delete the overflow files, in case we created them
    for(U32 i = 0; i < 2; ++i)
        ::remove(kOverflowName[i]);
}

// ================================================================================================
// Compute primes up to a limit
// ================================================================================================
void TokensWithBudget::ComputePrimes(U64 limit) {
    while(curr <= limit) {
        // If there's no token, we have a prime
        Token* t = list[curr - listBase];
        if(t == NULL) {
            // Add this as a prime
            primes.push_back(curr);

            // Create a token and place it
            t = new Token;
            t->prime = curr;
            t->sievePos = 1;
            PlaceToken(t, curr * (1 + sieve.Offset(0)));
        }
        // Otherwise move this token to its next position
        else {
            // Clear the current spot
            list[curr - listBase] = NULL;

            // Place the token at its next spot
            U64 place = curr + t->prime * sieve.Offset(t->sievePos);
            if(++t->sievePos == sieve.NumOffsets())
                t->sievePos = 0;
            PlaceToken(t, place);
        }

        curr += sieve.Offset(sievePos);
        if(++sievePos == sieve.NumOffsets())
            sievePos = 0;

        // If we've gone past the end of the list, load the next list
        if(curr >= listBase + listSize)
            LoadList();
    }
}

// ================================================================================================
// Place the given token (with the given value) at it's next location
// ================================================================================================
void TokensWithBudget::PlaceToken(Token* t, U64 place) {
    while(t != NULL) {
        U64 idx = place - listBase;
        if(idx >= listSize) {
            // Write the place and token to our file
            ::fwrite(&place, sizeof(place), 1, overflowFile);
            ::fwrite(t, sizeof(*t), 1, overflowFile);

            // Delete the token
            delete t;

            // There's nothing else to do here
            return;
        }

        Token* swap = list[idx];
        list[idx] = t;
        t = swap;

        // If there was no token there, we're done
        if(t == NULL)
            return;

        // Move this token to its next location
        place += t->prime * sieve.Offset(t->sievePos);
        if(++t->sievePos == sieve.NumOffsets())
            t->sievePos = 0;
    }
}

// ================================================================================================
// Repopulate the list from our overflow file
// ================================================================================================
void TokensWithBudget::LoadList() {
    // Update the base of our list
    listBase += listSize;

    // Close the current file
    ::fclose(overflowFile);

    // Open again for reading
    FILE* f = ::fopen(kOverflowName[overflowSwitch ? 0 : 1], "rb");

    // Open the other overflow file so that we can write overflowing tokens during our
    overflowSwitch = !overflowSwitch;
    overflowFile = ::fopen(kOverflowName[overflowSwitch ? 0 : 1], "wb");

    // Read and place all the tokens from the previous file
    while(true) {
        U64 place;
        Token* t = new Token;
        bool placeRead = ::fread(&place, sizeof(place), 1, f) == 1;
        bool tokenRead = ::fread(t, sizeof(*t), 1, f) == 1;
        if(!placeRead || !tokenRead) {
            delete t;
            break;
        }
        PlaceToken(t, place);
    }

    // Close the previous overflow file
    ::fclose(f);
}

