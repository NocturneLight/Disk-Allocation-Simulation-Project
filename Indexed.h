#pragma once

#include "Shared.h"

class IndexedAllocation
{
	public:
		// Our Constructor and Destructor.
		IndexedAllocation(std::string fileName);
	   ~IndexedAllocation();

	    // Create our variables here.
	    //int* ptrTable;
	    std::vector<int> ptrTable;
	    int indexBlock = 0;
		int indexBlockSize = 0;

	    std::string file = "";

		// Create our functions here.
		void WriteToTable(std::string fileName, intmax_t fileSize, int *table);

	private:

};

IndexedAllocation::IndexedAllocation(std::string fileName)
{
	this->file = fileName;
}

IndexedAllocation::~IndexedAllocation()
{
}

void IndexedAllocation::WriteToTable(std::string fileName, intmax_t fileSize, int *table)
{
	// Create variables exclusive to this function here.
	int maxLength = 0;
	bool startIndex = true;
	//std::vector<int> tmpVector;
	std::random_device rDevice;

	// Create a random engine and set the bounds. 
	std::default_random_engine randEngine(rDevice());
	std::uniform_int_distribution<int> uniDist(0, NUM_BLOCKS);

	// Store the name of the file in the object to remember it.
	this->file = fileName;

	// Loop until the fileSize is 0 or lower.
	while (fileSize > 0)
	{
		// Create a random number.
		int index = uniDist(randEngine);
		
		// If we get an empty spot...
		if (table[index] == NUM_BYTES && index != 0 && index != 1)
		{
			// If this is the first block, store the block number in the class.
			if (startIndex)
			{
				this->indexBlock = index;
				std::cout << "Index Block: " << this->indexBlock << std::endl;
				std::cout << "Name: " << this->file << std::endl;
			}
			// If startIndex is false, push the index into the temporary vector.
			else if (!startIndex)
			{
				this->ptrTable.push_back(index);
			}

			// Set startIndex to false to start pushing values into the 
			// temporary vector after the first iteration. 
			startIndex = false;

			// Increment the maxLength variable.
			maxLength++;

			// If filesize is less than NUM_BYTES, then we're at the end.
			// Occupy the block.
			if (fileSize <= NUM_BYTES || maxLength == 10)
			{
				fileSize = abs(fileSize - NUM_BYTES);
				table[index] = fileSize;
				break;
			}
			// Subtract NUM_BYTES from the fileSize and occupy the block. We go until we hit 0. 
			else if (fileSize > NUM_BYTES)
			{
				table[index] = 0;
				fileSize -= NUM_BYTES;
			}
		}
	}

	// When finished, we store the new length in the object.
	this->indexBlockSize = this->ptrTable.size();
}