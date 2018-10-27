// This exercise takes a non-contiguous sorted list beginning
// at an arbitrary point in an array and finds the start using
// a binary search with goal of O ( log n ) performance.
//
// The array is sorted in ascending order. 
// Duplicate values may be present.
// The upper value bound for the array is unknown.

// Performance for sets with non-unique values is O (log n)
// Performance for sets containing duplicates is O (log n dupes n)
//
// idx[ value ]
//
// value := RANGE
// RANGE := <0-n>
//
// Copyright (C) 2018 Gregory Hedger

#include <stdio.h>
#include <math.h>
#include <iostream>
#include <time.h>
#include <assert.h>
#include <vector>

// Definitions
typedef unsigned short CONTAINER;
typedef __int32_t SIZE;
typedef __uint32_t UINT;
typedef __int32_t INT;
#define EL std::endl

// Constants
const unsigned CONTAINER_SIZE = 32;
const unsigned SAMPLE_ITERATIONS = 1000;
const unsigned RANGE = CONTAINER_SIZE;
const unsigned MAX_DEPTH = 1 << 24;
const unsigned INCREMENT_BOUND = 4;

// @freeContainer
void freeContainer( CONTAINER *container )
{
  if( container ) {
    delete( container );
  }
}

CONTAINER *allocContainer( SIZE size )
{
  return new CONTAINER[ size ];
}

void generateRamp( CONTAINER *container, SIZE size, UINT startIdx, bool dupes )
{
  UINT i = startIdx;
  CONTAINER j = 0;
  do {
    container[ i ] = j;
    j += 1; // rand() % INCREMENT_BOUND;
    if( !dupes ) {
      if( !j ) {
        j++;
      }
    }
    i = (i + 1) % size;
  } while( i != startIdx % size );
}

UINT findRecurse(
    CONTAINER *container,
    SIZE subsectionSize,
    SIZE fullSize,
    UINT idx,
    UINT depth,
    UINT *tries,
    UINT *lwm,
    UINT *lwmIdx
    )
{
  (*tries)++;
  // santiy check
  assert( depth < MAX_DEPTH );

  UINT valL = 0, valR = 0;
  UINT idxL, idxR, startIdx;
  idxL = (idx - ( subsectionSize + 1 ) / 2) % fullSize;
  idxR = (idx + ( subsectionSize + 1 ) / 2) % fullSize;

  // skip past any dupes, guarding against infinite loop
  valL = container[ idxL ];
  valR = container[ idxR ];
  startIdx = valL;
  while( valL == valR && valL != startIdx ) {
    idxL = ( idxL + 1 ) % fullSize;
    idxR = ( idxR + 1 ) % fullSize;
    idxL = container[ idxL ];
    valR = container[ idxR ];
  }

  // Update low water mark
  if( valL < *lwm ) {
    *lwm = valL;
    *lwmIdx = idxL;
  }
  if( valR < *lwm ) {
    *lwm = valR;
    *lwmIdx = idxR;
  }
  if( idx < *lwm ) {
    *lwm = container[ idx ];
    *lwmIdx = idx;
  }

  if( subsectionSize ) {
    return findRecurse(
        container,
        subsectionSize >> 1,
        fullSize,
        *lwmIdx,
        ++depth,
        tries,
        lwm,
        lwmIdx
        );
  } else {
    return *lwmIdx;
  }
}

// Find the transition between 0 and n (ramp start)
UINT findRampStart(
    CONTAINER *container,
    SIZE size,
    UINT startIdx,
    UINT *tries
    )
{
  int i = startIdx;
  UINT lwm = ~0;
  UINT lwmIdx = 0;
  return findRecurse(
      container,
      size / 2,
      size,
      startIdx,
      0,
      tries,
      &lwm,
      &lwmIdx
      );
}

void printUsage()
{
  std::cout << "FindRamp" << EL;
  std::cout << "Copyright (C) 2018 Gregory Hedger" << EL;
  std::cout << "Usage:" << EL;
  std::cout << "\tfindramp <container_size> <#_of_iterations> <duplications>" << EL;
  std::cout << "Example:" << EL;
  std::cout << "\tfindramp 250 10000 1" << EL;
}

int main( int argc, char *argv[])
{
  // Seed prandom with time and get startIdx
  srand( ( UINT ) time( NULL ) );

  // grab params
  UINT containerSize;
  UINT iterationTot;
  bool allowDuplicates;
  if( argc > 3 ) {
    sscanf( argv[1], "%d", &containerSize );
    sscanf( argv[2], "%d", &iterationTot );
    UINT dupes;
    sscanf( argv[3], "%d", &dupes );
    allowDuplicates = dupes ? true : false;
  }

  if(
      containerSize > 10000000 || !containerSize ||
      iterationTot > 10000000 || !iterationTot ||
      argc < 3
    ) {
    printUsage();
    return -1;
  }

  // Allocate and generate container
  CONTAINER *container = allocContainer( containerSize );

  // Perform test
  double sigma = 0.0, mu = 0.0;
  UINT triesAccum = 0;
  std::vector<UINT> triesVect;
  for( UINT i = 0; i < iterationTot; i++ )
  {
    UINT startIdx = rand() % containerSize;
    generateRamp( container, containerSize, startIdx, allowDuplicates );
    UINT tries = 0;
    UINT idx = findRampStart(
        container,
        containerSize,
        containerSize >> 1,
        &tries );
    triesAccum += tries;
    triesVect.push_back( tries );
  }

  // Calculate mean (mu)
  mu = ( double ) triesAccum / ( double ) iterationTot;

  // Calculate std deviation (sigma)
  double sigmaAccum = 0.0;
  while( !triesVect.empty()  ) {
    UINT compVal =triesVect.back();
    triesVect.pop_back();
    sigmaAccum += pow( ( ( double ) compVal - mu ), 2 );
  }
  sigma = sqrt( sigmaAccum / iterationTot );

  std::cout << "TRIES MU: " << mu << EL;
  std::cout << "TRIES SIGMA: " << sigma << EL;

  // Clean up and go home
  freeContainer( container );
  container = NULL;

  return 0;
}
