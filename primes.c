/*****************************************************************************
 * FILE: primes.c
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 2/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Functions for finding prime numbers. Needed for hashing.
 *****************************************************************************/
#include "utils.h"
#include "primes.h"

/*****************************************************************************
 * isPrime - test whether a number is prime.
 *****************************************************************************/
BOOLEAN_T isPrime(int n) {
  int i;
  if (n==3 || n==2) {
    return TRUE;
  }
  if (n == 1 || n % 2 == 0) {
    return FALSE;
  }
  for (i = 3; i * i < n; i+=2) {
    if (n % i == 0) {
      return FALSE;
    }
  }
  return TRUE;
}

/*****************************************************************************
 * nextPrime - get the next prime number greater than n
 *****************************************************************************/
int nextPrime(int n) {
  if (n % 2 == 0)    n++;
  for (; !isPrime(n); n+=2) ;
  return n;
}
/*
 * primes.c
 */
