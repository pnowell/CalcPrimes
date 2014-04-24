
// local includes
#include "Timer.h"

// system includes
#ifdef __MACH__
    #include <mach/mach_time.h>
#else
    #include <time.h>
#endif

// static variables
static U64 sFreq = 1000000000;

// ================================================================================================
// Static method that needs to be called before any timing can be done
// ================================================================================================
void Timer::StaticInit() {
    #ifdef __MACH__
        mach_timebase_info_data_t info;
        mach_timebase_info(&info);
        sFreq = U64(1000000000.0 * F64(info.denom) / F64(info.numer) + 0.5);
    #endif
}

// ================================================================================================
// Constructor
// ================================================================================================
Timer::Timer() {
    curr = last = Cycles();
}

// ================================================================================================
// Take a timing sample
// ================================================================================================
void Timer::Sample() {
    last = curr;
    curr = Cycles();
}

// ================================================================================================
// Check the timing between the last two samples
// ================================================================================================
F32 Timer::Elapsed() const {
    return (curr - last) / F32(sFreq);
}

// ================================================================================================
// Get timing information
// ================================================================================================
U64 Timer::Cycles() const {
    #ifdef __MACH__
        return mach_absolute_time();
    #else
        struct timespec res;
        clock_gettime(CLOCK_MONOTONIC, &res);
        return res.tv_sec * sFreq + res.tv_nsec;
    #endif
}

