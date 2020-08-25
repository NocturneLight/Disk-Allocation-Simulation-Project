#pragma once

#include <string>
#include <list>
#include <unistd.h>
#include <stdio.h>
#include <vector>
#include <locale>
#include <dirent.h>
#include <algorithm>
#include <iterator>
#include <sys/stat.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <random>
#include <array>

#define NUM_BLOCKS 256
#define NUM_BYTES 512

// Forward class declarations go here.
class ContiguousAllocation;
class ChainedAllocation;
class IndexedAllocation;

// My enums go here.
enum GetChoiceType { MainMenu, GetFileName, GetFileNameReal };	// An enum for making the GetChoice function easier to understand.
enum ModeType { Contiguous, Indexed, Chained };					// An enum for making understanding what mode does what easier.
enum UpdateFunctionType { FileAllocationTable, Bitmap };		// An enum to make the update allocation or bitmap function easier to understand.

// My structs go here.
struct Allocation
{
	/* 
	   The Allocation Structure is a custom return type to allow me to
	   bypass the one return type per function limit. With this, I can
	   return either a Contiguous, Chained, or Indexed Allocation instance. 
	*/

	ContiguousAllocation *ContigInstance;
	ChainedAllocation *ChainInstance;
	IndexedAllocation *IndexInstance;
};
