#!/bin/sh

# Make sure we have our build directory
mkdir -p _build

# Determine build type
if [[ -z $1 || `echo $1 | tr '[:upper:]' '[:lower:]'` = "release" ]]; then
    COPTS="-O2"
elif [[ `echo $1 | tr '[:upper:]' '[:lower:]'` = "debug" ]]; then
    COPTS="-O0 -g"
else
    echo "Unknown build type\"$1\""
    exit 1
fi

# Set up compiler / linker options
COPTS="-c $COPTS"
LOPTS=""
if [ "$(uname -s)" == "Linux" ]; then
    COPTS="$COPTS -std=c++0x"
    LOPTS="$LOPTS -lrt"
fi

# Compile everything (pretty small, so we don't worry about conditional compilation with
# dependencies and time stamps)
g++ $COPTS -o _build/Timer.o src/Timer.cpp
g++ $COPTS -o _build/Sieve.o src/Sieve.cpp
g++ $COPTS -o _build/SievePrimes.o src/SievePrimes.cpp
g++ $COPTS -o _build/BasicSieve.o src/BasicSieve.cpp
g++ $COPTS -o _build/main.o src/main.cpp
g++ $LOPTS -o CalcPrimes \
    _build/Timer.o \
    _build/Sieve.o \
    _build/SievePrimes.o \
    _build/BasicSieve.o \
    _build/main.o

