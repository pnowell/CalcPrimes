
// local includes
#include "TokensWithBudget2.h"
#include "SievePrimes.h"

// system includes
#include <cstdlib>

// constants
static const size_t kPoolSize = 512;
static const size_t kFilenameLen = 32;
static char kTempFilename[kFilenameLen];

// ================================================================================================
// Constructor
// ================================================================================================
TokensWithBudget2::TokensWithBudget2(size_t budget) : CalcMethod(), sieve(), queue(kPoolSize),
                                                      threadStarted(false), threadResult(0),
                                                      threadJoining(false),
                                                      fillingSecondary(false), nextList(false) {
    for(size_t i = 0; i < kNumSievePrimes; ++i)
        sieve.AddPrime(kSievePrimes[i]);

    // Add the sieve primes
    primes.resize(kNumSievePrimes);
    for(size_t i = 0; i < kNumSievePrimes; ++i)
        primes[i] = kSievePrimes[i];

    // Create our lists
    for(size_t i = 0; i < 2; ++i) {
        list[i] = new Token*[budget];
        for(size_t j = 0; j < budget; ++j)
            list[i][j] = NULL;
    }
    whichList = 0;
    listBase = 0;
    listSize = budget;

    // Set ourselves up to process the first candidate
    curr = 1 + sieve.Offset(0);
    sievePos = 1;

    // Set up our overflow file
    overflowBaseIdx = 2;
    sprintf(kTempFilename, ".overflow%u", U32(overflowBaseIdx));
    overflowFile.push_back(fopen(kTempFilename, "wb"));

    // Set up our overflow thread
    pthread_create(&overflowThread, NULL, OverflowThreadCB, this);
}

// ================================================================================================
// Destructor
// ================================================================================================
TokensWithBudget2::~TokensWithBudget2() {
    // Shutdown our overflow thread
    if(threadStarted) {
        threadJoining = true;
        pthread_join(overflowThread, NULL);
    }

    // Delete any tokens left in our lists
    if(list != NULL) {
        // Delete any remaining tokens
        for(size_t i = 0; i < 2; ++i) {
            for(U64 j = 0; j < listSize; ++j) {
                if(list[i][j] != NULL)
                    delete list[i][j];
            }

            delete [] list[i];
            list[i] = NULL;
        }
    }

    // delete any overflow files we might have left
    size_t size = overflowFile.size();
    for(size_t i = 0; i < size; ++i) {
        fclose(overflowFile[i]);
        sprintf(kTempFilename, ".overflow%u", U32(overflowBaseIdx + i));
        remove(kTempFilename);
    }
}

// ================================================================================================
// Compute primes up to a limit
// ================================================================================================
void TokensWithBudget2::ComputePrimes(U64 limit) {
    while(curr <= limit) {
        // If there's no token, we have a prime
        Token* t = list[whichList][curr - listBase];
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
            list[whichList][curr - listBase] = NULL;

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
            NextList();
    }
}

// ================================================================================================
// Place the given token (with the given value) at it's next location
// ================================================================================================
void TokensWithBudget2::PlaceToken(Token* t, U64 place) {
    while(t != NULL) {
        U64 idx = place - listBase;
        if(idx >= listSize) {
            // Have the overflowMgr place the token
            AddOverflowToken(t, place);

            // There's nothing else to do here
            return;
        }

        Token* swap = list[whichList][idx];
        list[whichList][idx] = t;
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
// Callback for our overflow thread
// ================================================================================================
void* TokensWithBudget2::OverflowThreadCB(void* arg) {
    TokensWithBudget2& self = *reinterpret_cast<TokensWithBudget2*>(arg);
    self.threadStarted = true;
    self.threadResult = self.OverflowThread();
    return &self.threadResult;
}

// ================================================================================================
// Main method for our overflow thread
// ================================================================================================
U32 TokensWithBudget2::OverflowThread() {
    TokenWithPlace t;

    // Loop as long as someone isn't trying to end this thread
    while(!threadJoining) {
        // Attempt to get a token and its place from the queue
        while(queue.Pop(t))
            PlaceSecondaryToken(t.t, t.place);

        // If the main thread has told us to proceed to the next list
        if(nextList) {
            if(!queue.Empty())
                continue;

            // At this point we know the producer thread is blocking, waiting for us to finish
            // so we can go ahead and modify its parameters

            // Now we can progress to the next list and let our thread begin processing the secondary list
            listBase += listSize;
            whichList = 1 - whichList;

            // Let the main thread know that we're still filling in the secondary list
            fillingSecondary = true;

            // And go ahead and let the main thread proceed
            nextList = false;

            // Close the lowest file
            fclose(overflowFile[0]);

            // Open again for reading
            char kBuffer[kFilenameLen];
            sprintf(kBuffer, ".overflow%u", U32(overflowBaseIdx));
            FILE* f = fopen(kBuffer, "rb");

            // Adjust our file list and base index
            overflowFile.erase(overflowFile.begin());
            ++overflowBaseIdx;

            // Fill in the secondary list
            while(true) {
                U64 place;
                Token* t = new Token;
                bool placeRead = fread(&place, sizeof(place), 1, f) == 1;
                bool tokenRead = fread(t, sizeof(*t), 1, f) == 1;
                if(!placeRead || !tokenRead) {
                    delete t;
                    break;
                }
                PlaceSecondaryToken(t, place);
            }
            fclose(f);
            remove(kBuffer);

            // Let the main thread know that we're done filling in the secondary list
            fillingSecondary = false;
        }
    }

    return 0;
}

// ================================================================================================
// Add a token to be put into overflow
// ================================================================================================
void TokensWithBudget2::AddOverflowToken(Token* t, U64 place) {
    // Keep trying until we're able to push this token into the queue
    TokenWithPlace toPush(t, place);
    while(!queue.Push(toPush));
}

// ================================================================================================
// Waits until it's safe to switch lists and then switches
// Called in the main thread and blocks on the overflow thread resetting "nextList"
// ================================================================================================
void TokensWithBudget2::NextList() {
    // Wait until we're not processing the secondary list
    while(fillingSecondary);

    // Tell the overflow thread to switch to the next list as soon as possible
    nextList = true;
    // Wait for it to complete
    while(nextList);
}

// ================================================================================================
void TokensWithBudget2::PlaceSecondaryToken(Token* t, U64 place) {
    while(t != NULL) {
        U64 idx = place - listBase;
        if(idx < listSize) {
            printf("ERROR : PlaceSecondaryToken given a token not yet in secondary list");
            exit(EXIT_FAILURE);
        }
        idx -= listSize;

        // If this index is beyond even the secondary list, write it to a file
        if(idx >= listSize) {
            // Write it directly to an overflow file
            WriteToOverflowFile(place / listSize, t, place);

            // There's nothing else to do here
            return;
        }

        Token* swap = list[1 - whichList][idx];
        list[1 - whichList][idx] = t;
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
void TokensWithBudget2::WriteToOverflowFile(U64 listIdx, Token* t, U64 place) {
    // Make sure we have the appropriate file opened
    size_t idx = size_t(listIdx - overflowBaseIdx);
    size_t size = overflowFile.size();
    while(idx >= size) {
        sprintf(kTempFilename, ".overflow%u", U32(size + overflowBaseIdx));
        FILE* f = fopen(kTempFilename, "wb");
        overflowFile.push_back(f);
        ++size;
    }
    FILE* f = overflowFile[idx];
    fwrite(&place, sizeof(place), 1, f);
    fwrite(t, sizeof(*t), 1, f);
}

