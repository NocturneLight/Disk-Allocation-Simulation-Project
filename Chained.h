#pragma once

#include "Shared.h"

class ChainedAllocation
{
	public:
		// Our Constructor and Destructor.
		ChainedAllocation(std::string fileName);
	   ~ChainedAllocation();

	    // Create our variables here.
	    std::string file = "";
		std::vector<int> occupiedIndices;

		int startIndex = 0;
		int endIndex = 0;

		// Create our functions here.
		void WriteToTable(std::string fileName, intmax_t fileSize, int *table);


	private:

};

ChainedAllocation::ChainedAllocation(std::string fileName)
{
	this->file = fileName;
}

ChainedAllocation::~ChainedAllocation()
{
}

void ChainedAllocation::WriteToTable(std::string fileName, intmax_t fileSize, int *table)
{
	// Create variables exclusive to this function here.
	int maxLength = 0;
	bool startIndex = true;
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
			// Store the start point and set the startIndex bool to false.
			if (startIndex)
			{
				this->startIndex = index;

				startIndex = !startIndex;
			}

			// Increment the maxLength variable.
			maxLength++;

			// Push the index into the occupiedIndices vector to remember it.
			occupiedIndices.push_back(index);

			// If filesize is less than NUM_BYTES, then we're at the end.
			// Occupy the block.
			if (fileSize <= NUM_BYTES || maxLength == 10)
			{
				fileSize = abs(fileSize - NUM_BYTES);
				table[index] = fileSize;
				this->endIndex = index;
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
}