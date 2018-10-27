// This exercise takes a non-contiguous sorted list beginning
// at an arbitrary point in an array and finds the start using
// a binary search with goal of O ( log n ) performance.
//
// This implementation is in C to avoid the semantic clutter of C++
// and help with the understanding of the algorithm.
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

using namespace std;

// Definitions
typedef unsigned short CONTAINER;
typedef __int32_t SIZE;
typedef __uint32_t UINT;
typedef __int32_t INT;

// Constants
const unsigned CONTAINER_SIZE = 32;
const unsigned SAMPLE_ITERATIONS = 1000;
const unsigned RANGE = CONTAINER_SIZE;
const unsigned MAX_DEPTH = 1 << 24;
const unsigned INCREMENT_BOUND = 4;

// freeContainer
// Deallocate container resources
// Entry: pointer to container
void freeContainer( CONTAINER *container )
{
  if( container ) {
    delete( container );
  }
}

// allocContainer
// Allocate container resources
// Entry: size of container
// Exit: pointer to container
CONTAINER *allocContainer( SIZE size )
{
  return new CONTAINER[ size ];
}

void printContainer( CONTAINER *container, SIZE size )
{
  for( auto i = 0; i < size; i++ )
    cout << container[ i ] << " ";
  cout << endl;
}

void generateRamp( CONTAINER *container, SIZE size, UINT startIdx, bool dupes )
{
  UINT i = startIdx;
  CONTAINER j = 0;
  do {
    container[ i ] = j;
    j += dupes ? rand() % INCREMENT_BOUND : 1;
    if( !dupes ) {
      if( !j ) {
        j++;
      }
    }
    i = (i + 1) % size;
  } while( i != startIdx % size );
  printContainer( container, size );
}

// findRampPivot
// Find the beginning of the ramp, or "pivot" within a rotated sorted table.
// Takes the high and low indexes, calculates a midpoint, and recurses into itself
// zeroing in on the target.
// Entry: pointer to container
//        low index
//        high index
//        pointer to tries (for complexity analyis)
UINT findRampPivot(
    const CONTAINER *container,
    UINT lowIdx,
    UINT highIdx,
    UINT *tries )
{
  (*tries)++;
  // sanity check
  if( highIdx == lowIdx )
    return lowIdx;
  if( highIdx < lowIdx )
    return ~0;

  // Zero in on the pivot point based on the relative quantities
  // at the different indexes.
  UINT midIdx = ( lowIdx + highIdx ) >> 1;
  if( midIdx < highIdx && container[ midIdx ] > container[ midIdx + 1 ] )
    return midIdx;
  if( midIdx > lowIdx && container[ midIdx ] < container[ midIdx - 1 ] )
    return midIdx - 1;
  if( container[ lowIdx ] >= container[ midIdx ] ) {
    return findRampPivot( container, lowIdx, midIdx - 1, tries );
  }
  return findRampPivot( container, midIdx + 1, highIdx, tries );
}

// skipRepeating
// Skip repeating values in an array of a given size
// Entry: pointer to container
//        size of container in elements
//        pointer to idx to skip
//        delta (use -1)
void skipRepeating( const CONTAINER *container, UINT size, UINT *idx, INT delta )
{
  UINT startIdx = *idx;
  bool wrapped = false; // keep track of this in case we cross the seam, need
  while( container[ *idx ] == container[ ( (*idx) + delta ) % size ] ) {
    if( !*idx ) {
      (*idx) = size + delta;
      wrapped = true;
    } else {
      (*idx) += delta;
    }
    // Prevent infinite loop
    if( *idx == startIdx ) {
      // TODO: LOG ERROR
      break;
    }
  }
  // One last little bump to the left and then we're done...
  if( !*idx ) {
    (*idx) = size + delta;
    wrapped = true;
  } else {
    (*idx) += delta;
  }
}


// findRampPivotWithDupes
// Find the beginning of the ramp, or "pivot" within a rotated sorted table
// which can have duplicates.
// Takes the high and low indexes, calculates a midpoint, and recurses into itself
// zeroing in on the target.
// Entry: pointer to container
//        low index
//        high index
//        pointer to tries (for complexity analyis)
UINT findRampPivotWithDupes(
    const CONTAINER *container,
    UINT size,
    UINT lowIdx,
    UINT highIdx,
    UINT *tries )
{
  (*tries)++;
  // sanity check
  if( highIdx == lowIdx )
    return lowIdx;
  if( highIdx < lowIdx )
    return ~0;

  // Zero in on the pivot point based on the relative quantities
  // at the different indexes.
  UINT midIdx = ( lowIdx + highIdx ) >> 1;
  if( midIdx < highIdx && container[ midIdx ] > container[ midIdx + 1 ] )
    return midIdx;
  if( midIdx > lowIdx && container[ midIdx ] < container[ midIdx - 1 ] )
    return midIdx - 1;
  if( container[ lowIdx ] == container[ midIdx ] ) {
    if( container[ highIdx ] != container[ midIdx ] ) {
      // Account for special case where lowIdx and / or midIdx are in the middle
      // of a repeating run.
      skipRepeating( container, &lowIdx, -1, size );
      skipRepeating( container, &midIdx, -1, size );
      // Enforce search boundary integrity (low < high)
      if( highIdx < lowIdx ) {
        UINT swap = highIdx;
        highIdx = lowIdx;
        lowIdx = swap;
      }
      return findRampPivotWithDupes( container, size, midIdx, highIdx, tries );
    }
    return findRampPivotWithDupes( container, size, lowIdx, midIdx - 1, tries );
  }
  if( container[ lowIdx ] > container[ midIdx ] )
    return findRampPivotWithDupes( container, size, lowIdx, midIdx - 1, tries );

  return findRampPivotWithDupes( container, size, midIdx + 1, highIdx, tries );
}


// findRampStart
// Find the transition between 0 and n (ramp start)
// Entry: pointer to container
//        size of container in elements
//        pointer to tries count (for complexity analysis)
UINT findRampStart(
    CONTAINER *container,
    SIZE size,
    UINT *tries
    )
{
  assert( size );
  UINT pivot;

  // First, check for edge case where the pivot seam matches the bounds of the array
  // (i.e. array is not rotated)
  if( !container[ 0 ] )
    return 0;
  pivot = findRampPivotWithDupes( container, size, 0, size - 1, tries );

  // EDGE CASE: Skip any repeated entries
  while( container[ ( pivot + 1 ) % size ] == container[ pivot ] )
    pivot = ( pivot + 1 ) % size;

  if( ~0 != pivot )
    pivot = ( pivot + 1 ) % size;
  return pivot;
}

void printUsage()
{
  cout << "FindRamp" << endl;
  cout << "Copyright (C) 2018 Gregory Hedger" << endl;
  cout << "Usage:" << endl;
  cout << "\tfindramp <container_size> <#_of_iterations> <duplications>" << endl;
  cout << "Example:" << endl;
  cout << "\tfindramp 250 10000 1" << endl;
}

int main( int argc, char *argv[])
{
  // Seed prandom with time and get startIdx
  //srand( ( UINT ) time( NULL ) );
  srand( 428 );

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
  if( !container ) {
    cout << "Memory allocation error" << endl;
    return -1;
  }

  // Perform test
  double sigma = 0.0, mu = 0.0;
  UINT triesAccum = 0;
  std::vector<UINT> triesVect;
  for( UINT i = 0; i < iterationTot; i++ )
  {
    //srand( i );
    UINT startIdx = rand() % containerSize;
    generateRamp( container, containerSize, startIdx, allowDuplicates );
    UINT tries = 0;
    UINT idx = findRampStart(
        container,
        containerSize,
        &tries );
    if( ~0 == idx ) {
      cout << "Error in search parameters." << endl;
    }
    // In this test, it should always find 0.
    // If it does not, that is noteworthy and indicates a bug
    else if( container[ idx ] ) {
      cout << "TEST " << i << " Error finding element. idx 0:" << container[0] << " idx:" << idx << endl;
      cout << "Reported: " << idx << ":" << container[idx] << "  ";

    }
    if( ~0 == idx || container[ idx ] ) {
      cout << "TEST " << i << ": Actual: " << ( containerSize - container[0] ) % containerSize << ":" <<
        container[ containerSize - container[0] ] << endl;
    }

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

  cout << "TRIES MU: " << mu << endl;
  cout << "TRIES SIGMA: " << sigma << endl;

  // Clean up and go home
  freeContainer( container );
  container = NULL;

  return 0;
}
