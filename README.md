CalcPrimes
==========

Profiling various methods for computing primes

Contents of src
* main.cpp
  * Main method that profiles the different prime calculation methods
* Types.h
  * Basic typedefs that I like
* Sieve.h/.cpp
  * Sieve class used in the various prime calculation methods.  It's a basic class that helps you easily and quickly walk over non-multiples of a set of primes
* SievePrimes.h / .cpp
  * Primes that should be used to initialize sieves in the various prime calculation methods
* BasicSieve.h/.cpp
  * Method that builds a sieve with a few of the lowest primes and then builds a table of primes by checking for divisibility of each candidate by all known primes up to its square root.  Iteration over candidates is done with the help of a sieve.
* TokenHeap.h/.cpp
  * Iterate over all integers (using a sieve), and each time an integer is encountered with no tokens, mark it as prime and generate a token for it, placing it on a future multiple of that prime.  Uses a min-heap to store the addresses of all the tokens and for knowing the next in order token address.
* TokensWithBudget.h/.cpp
  * Iterates over numbers (using a sieve) and moves tokens in a fixed size array of a given size.  When tokens move past the end of the array, they are written to a file.  When iteration reaches the end of the array, the file is read and a new array is initialized and the process continues.  Same as above, primes are determined to be numbers that have no tokens by the time iteration reaches them.
