
#pragma once

// local includes
#include "Types.h"

// ================================================================================================
class Timer {

public:

    // Static method that needs to be called before any timing can be done
    static void StaticInit();

    // Constructor
    Timer();

    // Take a timing sample
    void Sample();

    // Check the timing between the last two samples
    F32 Elapsed() const;

protected:

    U64 curr;
    U64 last;

    U64 Cycles() const;
};

