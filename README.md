CalcPrimes
==========

Profiling various methods for computing primes

Contents of src
* main.cpp
  * Main method that profiles the different prime calculation methods
* Types.h
  * Basic typedefs that I like
* Timer.h/.cpp
  * Class to use for profiling the different calculation methods
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
* TokensWithBudget2.h/.cpp
  * Same as above, but keeps a second list of the same size so that it can immediately start the next list upon finishing the current one, and a separate thread is responsible for filling the new secondary list from the overflow files.  Also, instead of keeping just one overflow file, it keeps one per budgeted window.  It keeps file handles to all of these to avoid stalling on numerous calls to fopen.
* Eratosthenes.h/.cpp
  * Implements the classic Sieve of Eratosthenes, but with an extra optimization.  It keeps track of an offset for each number which is the next number you could possibly need to check.  As non-primes are ruled out, offsets are adjusted so that you don't visit that number at all.  From this, the only numbers visited are primes.  Also, since it starts marking off non-primes beginning at the square of each prime, as soon as it reaches a prime where the square of that prime is greater than the extent of the table, the only thing left to do is walk over the remaining primes and record them (each prime already pointing to the next prime in the table).
