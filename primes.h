/*****************************************************************************
 * FILE: primes.h
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 2/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Functions for finding prime numbers. Needed for hashing.
 * Copyright (c) Columbia University
 *****************************************************************************/

#ifndef PRIMES_H
#define PRIMES_H

#include "utils.h"

/*****************************************************************************
 * isPrime - test whether a number is prime.
 *****************************************************************************/
BOOLEAN_T isPrime(int n);

/*****************************************************************************
 * nextPrime - get the next prime number greater than n
 *****************************************************************************/
int nextPrime(int n);


#endif

