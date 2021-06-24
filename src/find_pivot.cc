// This exercise takes a non-contiguous sorted list beginning
// at an arbitrary point in an array and finds the start using
// a binary search with goal of O (log n) performance.
//
// This implementation is in C to avoid the semantic clutter of C++
// and help with the understanding of the algorithm.
//
// The array is sorted in ascending order.

// Performance for sets with non-unique values is O (log n)
//
// idx[ value ]
//
// value := RANGE
// RANGE := <0-n>
//
// Copyright (C) 2018 Gregory Hedger

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <ctime>
#include <cassert>
#include <vector>

// Definitions
typedef __int32_t SIZE;
typedef __uint32_t UINT;
typedef UINT CONTAINER;

// Constants
const unsigned INCREMENT_BOUND = 4;

// FreeContainer
// Deallocate container resources
// Entry: pointer to container
void FreeContainer(const CONTAINER *container)
{

    delete container;

}

// AllocContainer
// Allocate container resources
// Entry: size of container
// Exit: pointer to container
CONTAINER *AllocContainer(SIZE size)
{
  return new CONTAINER[ size ];
}

// PrintContainer
// Print the contents of the container to stdout
// Entry: pointer to container
//        size of container
void PrintContainer(CONTAINER *container, SIZE size)
{
  for (auto i = 0; i < size; i++)
    std::cout << container[ i ] << " ";
  std::cout << std::endl;
}

// GenerateRamp
// Entry: pointer to container
//        size of container
//        start index in container
//        true == allow duplicates, false == increment by one
void GenerateRamp(CONTAINER *container, SIZE size, UINT startIdx, bool dupes)
{
  UINT i = startIdx;
  CONTAINER j = 0;
  do {
    container[ i ] = j;
    if (dupes) {
      j += rand() % INCREMENT_BOUND;
    } else {
      j += 1;
    }
    if (!dupes) {
      if (!j) {
        j++;
      }
    }
    i = (i + 1) % size;
  } while (i != startIdx % size);
  //PrintContainer(container, size);
}

// FindRampPivot
// Find the beginning of the ramp, or "pivot" within a rotated sorted table.
// Takes the high and low indexes, calculates a midpoint, and recurses into itself
// zeroing in on the target.
// Entry: pointer to container
//        low index
//        high index
//        pointer to tries (for complexity analyis)
// Exit: pivot
// NOTE: Recursive function
UINT FindRampPivot(
    const CONTAINER *container,
    UINT left_idx,
    UINT right_idx,
    UINT *tries)
{
  (*tries)++;     // bookkeeping/analysis
  // sanity check
  if (right_idx == left_idx)
    return left_idx;
  if (right_idx < left_idx)
    return ~0;

  // Zero in on the pivot point based on the relative quantities
  // at the different indexes.
  UINT mid_idx = (left_idx + right_idx) >> 1;
  if (mid_idx < right_idx && container[ mid_idx ] > container[ mid_idx + 1 ])
    return mid_idx;
  if (mid_idx > left_idx && container[ mid_idx ] < container[ mid_idx - 1 ])
    return mid_idx - 1;
  if (container[ left_idx ] > container[ mid_idx ]) {
    return FindRampPivot(container, left_idx, mid_idx - 1, tries);
  }
  return FindRampPivot(container, mid_idx + 1, right_idx, tries);
}

// FindRampStart
// Find the transition between 0 and n (ramp start)
// Entry: pointer to container
//        size of container in elements
//        pointer to tries count (for complexity analysis)
UINT FindRampStart(
    CONTAINER *container,
    SIZE size,
    UINT *tries
  )
{
  assert(size);
  UINT pivot;

  // First, check for edge case where the pivot seam matches the bounds of the array
  // (i.e. array is not rotated)
  if (!container[ 0 ])
    return 0;

  pivot = FindRampPivot(container, 0, size - 1, tries);

  // EDGE CASE: Skip any repeated entries
  while (container[ (pivot + 1) % size ] == container[ pivot ])
    pivot = (pivot + 1) % size;

  if ((UINT) ~0 != pivot)
    pivot = (pivot + 1) % size;
  return pivot;
}

void PrintUsage()
{
  std::cout << "FindRamp" << std::endl;
  std::cout << "Copyright (C) 2018 Gregory Hedger" << std::endl;
  std::cout << "Usage:" << std::endl;
  std::cout << "\tfindramp <container_size> <#_of_iterations>" << std::endl;
  std::cout << "Example:" << std::endl;
  std::cout << "\tfindramp 250 10000" << std::endl;
}

int main(int argc, char *argv[])
{
  // Seed prandom with time and get startIdx
  srand(( UINT) time(nullptr));
  //srand(428);

  // grab params
  SIZE container_size;
  UINT iteration_tot;
  bool allowDuplicates;
  bool printContainer = false;
  if (argc > 2) {
    strtol(argv[1], reinterpret_cast<char **>(&container_size), 10);
    strtol(argv[1], reinterpret_cast<char **>(&iteration_tot), 10);
    //sscanf(argv[2], "%d", &iteration_tot);

    //sscanf(argv[1], "%d", &container_size);
    //sscanf(argv[2], "%d", &iteration_tot);
    allowDuplicates = false;
    if (argc > 3) {
      printContainer = true;
    }
  } else {
    PrintUsage();
    return -1;
  }

  if (
      container_size > 10000000 || !container_size ||
      iteration_tot > 10000000 || !iteration_tot ||
      argc < 2
  ) {
    PrintUsage();
    return -1;
  }

  // Allocate and generate container
  CONTAINER *container = AllocContainer(container_size);

  // Perform test
  UINT tries_accum = 0;
  std::vector<UINT> tries_vect;
  for (UINT i = 0; i < iteration_tot; i++)
  {
    //srand(i);
    UINT startIdx = rand() % container_size;
    GenerateRamp(container, container_size, startIdx, allowDuplicates);
    UINT tries = 0;
    UINT idx = FindRampStart(
        container,
        container_size,
        &tries);
    if ((UINT) ~0 == idx) {
      std::cout << "Error in search parameters." << std::endl;
    }
    // In this test, it should always find 0.
    // If it does not, that is noteworthy and indicates a bug
    else if (container[ idx ]) {
      std::cout << "TEST " << i << " Error finding element. idx 0:" << container[0] << " idx:" << idx << std::endl;
      std::cout << "Reported: " << idx << ":" << container[idx] << "  ";
    }
    if ((UINT) ~0 == idx || container[ idx ]) {
      std::cout << "TEST " << i << ": Actual: " << (container_size - container[0]) % container_size << ":" <<
        container[ container_size - container[0] ] << std::endl;
    }

    tries_accum += tries;
    tries_vect.push_back(tries);
  }

  // Calculate mean (mu)
  double sigma;
  double mu = (double) tries_accum / (double) iteration_tot;

  // Calculate std deviation (sigma)
  double sigma_accum = 0.0;
  while (!tries_vect.empty()) {
    UINT compVal =tries_vect.back();
    tries_vect.pop_back();
    sigma_accum += pow(( (double) compVal - mu), 2);
  }
  sigma = sqrt(sigma_accum / iteration_tot);

  std::cout << "TRIES MU: " << mu << std::endl;
  std::cout << "TRIES SIGMA: " << sigma << std::endl;

  if (printContainer) PrintContainer(container, container_size);

  // Clean up and go home
  FreeContainer(container);

  return 0;
}
