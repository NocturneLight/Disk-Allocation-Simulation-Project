#pragma once

#include "Shared.h"

class ContiguousAllocation
{
	public:
		 // Our Constructor and Destructor.
		 ContiguousAllocation(std::string fileName);
		~ContiguousAllocation();

		// Create our variables here.
		int length = 0;
		int startIndex = 0;

		std::string file = "";
		std::vector<int> occupiedIndices;
		
		// Create our functions here.
		void ReadFromTable();
		void WriteToTable(std::string fileName, intmax_t fileSize, int *table);
		
		

	private:

};

ContiguousAllocation::ContiguousAllocation(std::string fileName)
{
	this->file = fileName;
}

ContiguousAllocation::~ContiguousAllocation()
{
}

void ContiguousAllocation::WriteToTable(std::string fileName, intmax_t fileSize, int *table)
{
	// Create variables exclusive to this function here.
	int maxLength = 0;
	bool startIndex = true;
	
	// Store the name of the file in the object to remember it.
	this->file = fileName;

	// Traverse the table array.
	for (int i = 0; i < NUM_BLOCKS; i++)
	{
		// If we get an empty spot...
		if (table[i] == NUM_BYTES && i != 0 && i != 1)
		{
			// Store the start point and set the startIndex bool to false.
			if (startIndex)
			{
				this->startIndex = i;

				startIndex = !startIndex;
			}

			// Increment the maxLength variable.
			maxLength++;
			
			// Push the index into the occupiedIndices vector to remember it.
			occupiedIndices.push_back(i);

			// If filesize is less than NUM_BYTES, then we're at the end.
			// Occupy the block and store the length.
			if (fileSize <= NUM_BYTES || maxLength == 10)
			{
				fileSize = abs(fileSize - NUM_BYTES);
				table[i] = fileSize;
				this->length = maxLength;
				break;
			}
			// Subtract NUM_BYTES from the fileSize and occupy the block. We go until we hit 0. 
			else if (fileSize > NUM_BYTES)
			{
				table[i] = 0;
				fileSize -= NUM_BYTES;
			}
		}
	}
}