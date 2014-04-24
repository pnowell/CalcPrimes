#!/bin/sh

# Make sure we have our build directory
mkdir -p _build

# Determine build type
if [[ -z $1 || `echo $1 | tr '[:upper:]' '[:lower:]'` = "release" ]]; then
    OPTS="-O2"
elif [[ `echo $1 | tr '[:upper:]' '[:lower:]'` = "debug" ]]; then
    OPTS="-O0 -g"
else
    echo "Unknown build type\"$1\""
    exit 1
fi

# Set up compile options
OPTS="-c $OPTS"

# Compile everything (pretty small, so we don't worry about dependencies and time stamps)
g++ $OPTS -o _build/Sieve.o src/Sieve.cpp
g++ $OPTS -o _build/SievePrimes.o src/SievePrimes.cpp
g++ $OPTS -o _build/BasicSieve.o src/BasicSieve.cpp
g++ $OPTS -o _build/main.o src/main.cpp
g++ -o CalcPrimes _build/Sieve.o _build/SievePrimes.o _build/BasicSieve.o _build/main.o

