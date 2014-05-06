
#pragma once

// local includes
#include "Types.h"

// ================================================================================================
// Integer square root methods
// ================================================================================================
inline U32 ISqrt(U32 n) {
    U32 result = 0;
    S32 diff = n;

    while(diff != 0) {
        result += diff;
        diff = S32(n/result - result - 1) / 2;
    };

    return result;
}

inline U64 ISqrt(U64 n) {
    U64 result = 0;
    S64 diff = n;

    while(diff != 0) {
        result += diff;
        diff = S64(n/result - result - 1) / 2;
    };

    return result;
}

