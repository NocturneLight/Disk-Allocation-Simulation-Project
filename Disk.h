#pragma once

#include "Shared.h"
#include "Contiguous.h"
#include "Chained.h"
#include "Indexed.h"

class Disk
{
	public:
	  // My public variables go here.
	  int table[NUM_BLOCKS] = {};
	  std::vector<std::string> simulationFiles;

	  // The constructor.
	  Disk(int arg, char** argc);

	  // My public function prototypes go here.
	  Allocation Write(std::string fileName, intmax_t fileSize, Disk *disk);
	  Allocation RealToSimulation(std::string choice, Disk *disk);
	  Allocation UpdateTableFileAlloBitmap(Disk *disk, UpdateFunctionType mode);
	  ModeType GetMode();
	  void DisplayDiskBlock(std::vector<Allocation> *alloc, Disk *disk);
	  void DeleteFromSimulation(std::vector<Allocation> *alloc, Disk *disk);
	  void SimulationToReal(std::string choice, Disk *disk);
	  void DisplayFreeSpaceBitmap(std::vector<Allocation> *alloc);
	  void DisplayFileTable(std::vector<Allocation> *alloc);
	  bool FileExists(std::string choice);
	  static std::string ToLower(std::string arg);
	   
	   

	private:
	  // My private variables go here.
	  int mode = -1;

	  // My private function prototypes go here.
	  void InitializeSimulation();
	  
};




Disk::Disk(int arg, char** argc)
{
	std::string tempString = "";

	// Inform the user they have too many arguments.
	if (arg > 2)
	{
		std::cout << "Too many arguments given. Try again and only put one." << std::endl;
		exit(EXIT_SUCCESS);
	}
	// Inform the user they have too few arguments.
	else if (arg < 2)
	{
		std::cout << "Too few arguments given. Try again and only put one." << std::endl;
		exit(EXIT_SUCCESS);
	}
	// The user has the right amount.
	else if ((tempString = ToLower(std::string(argc[1]))) == "contiguous" || tempString == "indexed" || tempString == "chained")
	{
		// Store the mode that corresponds with their argument.
		mode = (tempString == "contiguous") ? Contiguous : (tempString == "indexed") ? Indexed : (tempString == "chained") ? Chained : -1;

		// Inform the user if the folder was unable to be made.
		if (mkdir("Simulation", 0777) == -1 && errno != EEXIST)
		{
			std::cerr << "Failed to create the Simulation Directory." << std::endl;
			exit(EXIT_FAILURE);
		}
		// Otherwise, after the folder is successfully created, start initializing everything.
		else
		{
			// We first populate the table array.
			std::fill_n(table, NUM_BLOCKS, NUM_BYTES);

			// Then we initialize the simulation.
			InitializeSimulation();
		}

	}
	// Exit the program.
	else
	{
		std::cout << "Invalid argument given. Please use either \"contiguous\", \"indexed\", or \"chained\". as arguments and nothing else." << std::endl;
		exit(EXIT_SUCCESS);
	}
}

void Disk::DeleteFromSimulation(std::vector<Allocation> *alloc, Disk *disk)
{
	// Create variables exclusive to this function here.
	std::string choice = "";
	bool match = false;

	while (true)
	{
		// Ask the user for a choice.
		std::cout << "What file would you like to delete from the Simulation?";

		// Store their response.
		std::cin >> choice;

		// Do the following depending on the mode the program is in.
		if (this->mode == ModeType::Contiguous)
		{
			// Traverse the Allocation vector...
			for (unsigned int i = 0; i < alloc->size(); i++)
			{
				// If we one of the allocation vector's instances has a name that matches the user's choice...
				if (choice == alloc->at(i).ContigInstance->file)
				{
					// Loop through that match's vector of occupied indices...
					for (unsigned int j = 0; j < alloc->at(i).ContigInstance->occupiedIndices.size(); j++)
					{
						// At every spot that was occupied in the table by this instance in the allocation vector, 
						// reset the number of free bytes in that spot in the table to NUM_BYTES. 
						this->table[alloc->at(i).ContigInstance->occupiedIndices.at(j)] = NUM_BYTES;
					}

					// Then we remove that instnace from the allocation vector, thereby removing it from the simulation.
					alloc->erase(alloc->begin() + i);

					// We then set match to true to break from the while loop.
					match = true;

					// We then break from this for loop.
					break;
				}
			}
		}
		else if (this->mode == ModeType::Indexed)
		{
			// Traverse the Allocation vector...
			for (unsigned int i = 0; i < alloc->size(); i++)
			{
				// If we one of the allocation vector's instances has a name that matches the user's choice...
				if (choice == alloc->at(i).IndexInstance->file)
				{
					// Loop through that match's vector of occupied indices...
					for (unsigned int j = 0; j < alloc->at(i).IndexInstance->ptrTable.size(); j++)
					{
						// At every spot that was occupied in the table by this instance in the allocation vector, 
						// reset the number of free bytes in that spot in the table to NUM_BYTES. 
						this->table[alloc->at(i).IndexInstance->ptrTable.at(j)] = NUM_BYTES;
					}

					// Then we remove the index block from the table.
					this->table[alloc->at(i).IndexInstance->indexBlock] = NUM_BYTES;

					// Then we remove that instance from the allocation vector, thereby removing it from the simulation.
					alloc->erase(alloc->begin() + i);

					// We then set match to true to break from the while loop.
					match = true;

					// We then break from this for loop.
					break;
				}
			}
		}
		else if (this->mode == ModeType::Chained)
		{
			// Traverse the Allocation vector...
			for (unsigned int i = 0; i < alloc->size(); i++)
			{
				// If we one of the allocation vector's instances has a name that matches the user's choice...
				if (choice == alloc->at(i).ChainInstance->file)
				{
					// Loop through that match's vector of occupied indices...
					for (unsigned int j = 0; j < alloc->at(i).ChainInstance->occupiedIndices.size(); j++)
					{
						// At every spot that was occupied in the table by this instance in the allocation vector, 
						// reset the number of free bytes in that spot in the table to NUM_BYTES. 
						this->table[alloc->at(i).ChainInstance->occupiedIndices.at(j)] = NUM_BYTES;
					}

					// Then we remove that instnace from the allocation vector, thereby removing it from the simulation.
					alloc->erase(alloc->begin() + i);

					// We then set match to true to break from the while loop.
					match = true;

					// We then break from this for loop.
					break;
				}
			}
		}

		if (!match)
		{
			std::cout << "The file you chose to remove does not exist in the simulation. Please choose another file." << std::endl;
		}
		else if (match)
		{
			break;
		}
	}
}

void Disk::DisplayDiskBlock(std::vector<Allocation> *alloc, Disk *disk)
{
	// Create variables exclusive to this function here.
	std::string stringTable[NUM_BLOCKS] = {""};

	// We iterate through the Allocation vector.
	for (unsigned int i = 0; i < alloc->size(); i++)
	{
		// We assemble a string version of the table. In each block that a file takes up will be its name.
		if (this->mode == ModeType::Contiguous)
		{
			for (unsigned int j = 0; j < ((alloc->at(i).ContigInstance->file != "fiAl.txt" && alloc->at(i).ContigInstance->file != "bmap.txt") ? alloc->at(i).ContigInstance->occupiedIndices.size() - 1 : alloc->at(i).ContigInstance->occupiedIndices.size()); j++)
			{
				stringTable[alloc->at(i).ContigInstance->occupiedIndices.at(j)] = alloc->at(i).ContigInstance->file;
			}
		}
		else if (this->mode == ModeType::Indexed)
		{
			for (unsigned int j = 0; j < alloc->at(i).IndexInstance->ptrTable.size(); j++)
			{
				stringTable[alloc->at(i).IndexInstance->ptrTable.at(j)] = alloc->at(i).IndexInstance->file;
			}
		}
		else if (this->mode == ModeType::Chained)
		{
			for (unsigned int j = 0; j < ((alloc->at(i).ChainInstance->file != "fiAl.txt" && alloc->at(i).ChainInstance->file != "bmap.txt") ? alloc->at(i).ChainInstance->occupiedIndices.size() - 1 : alloc->at(i).ChainInstance->occupiedIndices.size()); j++)
			{
				stringTable[alloc->at(i).ChainInstance->occupiedIndices.at(j)] = alloc->at(i).ChainInstance->file;
			}
		}

		
	}

	// Output the stringTable.
	for (unsigned int i = 0; i < NUM_BLOCKS; i++)
	{
		printf("----------------\n");
		printf("| %-12s |%i\n", stringTable[i].c_str(), i);
		printf("----------------\n");
	}
}

void Disk::DisplayFreeSpaceBitmap(std::vector<Allocation> *alloc)
{
	// Create our variables exclusive to this function here.
	int bitmap[NUM_BLOCKS] = {0};
	FILE *bitmapFile = fopen("Simulation/bmap.txt", "w");

	// We look at our table, and according to it's contents,
	// create the corresponding bitmap.
	for (int i = 0; i < NUM_BLOCKS; i++)
	{
		if (this->table[i] < NUM_BYTES)
		{
			bitmap[i] = 1;
		}
		else if (this->table[i] == NUM_BYTES)
		{
			bitmap[i] = 0;
		}
	}

	// We then display that freespace bitmap and also store it in it's file.
	for (int i = 0; i < NUM_BLOCKS; i++)
	{
		std::cout << bitmap[i];
		fprintf(bitmapFile, "%i", bitmap[i]);

		if ((i % 32 == 0 && i != 0) || i == NUM_BLOCKS - 1)
		{
			std::cout << std::endl;
			fprintf(bitmapFile, "\r\n");
		}
	}

	// We close the file when finished.
	fclose(bitmapFile);
}

void Disk::DisplayFileTable(std::vector<Allocation> *alloc)
{
	// If the vector is empty, inform the user and exit.
	if (alloc->size() == 0)
	{
		std::cout << "There are no files in the simulation to display. Add a file to the simulation first." << std::endl;
	}
	// Otherwise output a file table unique to the allocation type the program is set to and then store it in the file in the simulation folder.
	else if (GetMode() == ModeType::Contiguous)
	{
		FILE *fileTable = fopen("Simulation/fiAl.txt", "w");

		printf("---------------------------------------\n");
		printf("| %-12s | %-11s | %-6s |\n", "File Name", "Start Block", "Length");
		printf("---------------------------------------\n");

		fprintf(fileTable, "---------------------------------------\r\n");
		fprintf(fileTable, "| %-12s | %-11s | %-6s |\r\n", "File Name", "Start Block", "Length");
		fprintf(fileTable, "---------------------------------------\r\n");
		
		for (auto allo : *alloc)
		{
			printf("---------------------------------------\n");
			printf("| %-12s | %-11i | %-6i |\n", allo.ContigInstance->file.c_str(), allo.ContigInstance->startIndex, allo.ContigInstance->length - 1 > 0 ? allo.ContigInstance->length - 1 : 1);
			printf("---------------------------------------\n");

			fprintf(fileTable, "---------------------------------------\r\n");
			fprintf(fileTable, "| %-12s | %-11i | %-6i |\r\n", allo.ContigInstance->file.c_str(), allo.ContigInstance->startIndex, allo.ContigInstance->length - 1 > 0 ? allo.ContigInstance->length - 1 : 1);
			fprintf(fileTable, "---------------------------------------\r\n");
		}
		
		fclose(fileTable);
	}
	else if (GetMode() == ModeType::Indexed)
	{
		FILE *fileTable = fopen("Simulation/fiAl.txt", "w");

		printf("------------------------------\n");
		printf("| %-12s | %-11s |\n", "File Name", "Index Block");
		printf("------------------------------\n");

		fprintf(fileTable, "------------------------------\r\n");
		fprintf(fileTable, "| %-12s | %-11s |\r\n", "File Name", "Index Block");
		fprintf(fileTable, "------------------------------\r\n");

		for (auto allo : *alloc)
		{
			printf("------------------------------\n");
			printf("| %-12s | %-11i |\n", allo.IndexInstance->file.c_str(), allo.IndexInstance->indexBlock);
			printf("------------------------------\n");

			fprintf(fileTable, "------------------------------\r\n");
			fprintf(fileTable, "| %-12s | %-11i |\r\n", allo.IndexInstance->file.c_str(), allo.IndexInstance->indexBlock);
			fprintf(fileTable, "------------------------------\r\n");
		}

		fclose(fileTable);
	}
	else if (GetMode() == ModeType::Chained)
	{
		FILE *fileTable = fopen("Simulation/fiAl.txt", "w");

		printf("-------------------------------------------\n");
		printf("| %-12s | %-6s | %-6s | %-6s |\n", "File Name", "Start", "End", "Length");
		printf("-------------------------------------------\n");

		fprintf(fileTable, "-------------------------------------------\r\n");
		fprintf(fileTable, "| %-12s | %-6s | %-6s | %-6s |\r\n", "File Name", "Start", "End", "Length");
		fprintf(fileTable, "-------------------------------------------\r\n");

		for (auto allo : *alloc)
		{
			printf("-------------------------------------------\n");
			printf("| %-12s | %-6i | %-6i | %-6i |\n", allo.ChainInstance->file.c_str(), allo.ChainInstance->startIndex, allo.ChainInstance->endIndex, ((int)allo.ChainInstance->occupiedIndices.size() - 1) > 0 ? (int)allo.ChainInstance->occupiedIndices.size() - 1 : 1);
			printf("-------------------------------------------\n");

			fprintf(fileTable, "-------------------------------------------\r\n");
			fprintf(fileTable, "| %-12s | %-6i | %-6i | %-6i |\r\n", allo.ChainInstance->file.c_str(), allo.ChainInstance->startIndex, allo.ChainInstance->endIndex, ((int)allo.ChainInstance->occupiedIndices.size() - 1) > 0 ? (int)allo.ChainInstance->occupiedIndices.size() - 1 : 1);
			fprintf(fileTable, "-------------------------------------------\r\n");
		}

		fclose(fileTable);
	}	
}

ModeType Disk::GetMode()
{
	// Return the value that corresponds with the number assigned to mode.
	switch (this->mode)
	{
		case 0:
			return ModeType::Contiguous;
		case 1:
			return ModeType::Indexed;
		case 2:
			return ModeType::Chained;
	}

	// In the impossible event that we reach here, return a ModeType casted null.
	return (ModeType)NULL;
}

Allocation Disk::UpdateTableFileAlloBitmap(Disk *disk, UpdateFunctionType mode)
{
	// Create variables exclusive to this function here.
	std::string file = (mode == UpdateFunctionType::FileAllocationTable) ? "fiAl.txt" : (mode == UpdateFunctionType::Bitmap) ? "bmap.txt" : "";
	struct stat statBuffer;
	Allocation *conIn = new Allocation();
	

	if (stat(("Simulation/" + file).c_str(), &statBuffer) != -1)
	{
		// Do the following for each mode...
		if (this->mode == ModeType::Contiguous)
		{
			// Create an instance of the continuous allocation.
			ContiguousAllocation *conti = new ContiguousAllocation(file);

			// The file allocation table resides in block 0, it's length 1.
			conti->startIndex = (mode == UpdateFunctionType::FileAllocationTable) ? 0 : 1;
			conti->length = 1;

			// Finally, we push their occupied index into the vector.
			if (conti->occupiedIndices.size() == 0)
			{
				conti->occupiedIndices.push_back(conti->startIndex);
			}

			// Change the number in block 0 or 1 to reflect the new amount of bytes remaining
			// when the table takes up space.
			disk->table[(mode == UpdateFunctionType::FileAllocationTable) ? 0 : 1] = NUM_BYTES - statBuffer.st_size;

			// Store the instance in our Allocation return type.
			conIn->ContigInstance = conti;
		}
		else if (this->mode == ModeType::Indexed)
		{
			// Create an instance of the indexed allocation.
			IndexedAllocation *index = new IndexedAllocation(file);
			
			// The file allocation table's index block is 0.
			index->indexBlock = (mode == UpdateFunctionType::FileAllocationTable) ? 0 : 1;

			// Push the index block into the vector.
			// Finally, we push their occupied index into the vector.
			if (index->ptrTable.size() == 0)
			{
				index->ptrTable.push_back(index->indexBlock);
			}

			// Change the number in block 0 to reflect the new amount of bytes remaining
			// when the table takes up space.
			disk->table[(mode == UpdateFunctionType::FileAllocationTable) ? 0 : 1] = NUM_BYTES - statBuffer.st_size;

			// Store the instance in our Allocation return type.
			conIn->IndexInstance = index;
		}
		else if (this->mode == ModeType::Chained)
		{
			// Create an instnace of the chained allocation.
			ChainedAllocation *chain = new ChainedAllocation(file);

			// The table is all in one block, so start and end are either 0 or 1.
			chain->startIndex = (mode == UpdateFunctionType::FileAllocationTable) ? 0 : 1;
			chain->endIndex = (mode == UpdateFunctionType::FileAllocationTable) ? 0 : 1;
			
			// Finally, we push their occupied index into the vector.
			if (chain->occupiedIndices.size() == 0)
			{
				chain->occupiedIndices.push_back(chain->startIndex);
			}

			// Change the number in block 0 to reflect the new amount of bytes remaining
			// when the table takes up space.
			disk->table[(mode == UpdateFunctionType::FileAllocationTable) ? 0 : 1] = NUM_BYTES - statBuffer.st_size;

			// Store the instance in our Allocation return type.
			conIn->ChainInstance = chain;
		}
	}

	return *conIn;
}

Allocation Disk::Write(std::string fileName, intmax_t fileSize, Disk *disk)
{
	// Create variables exclusive to this function here.
	Allocation *conIn = new Allocation();
	
	
	// In contiguous mode...
	if (mode == ModeType::Contiguous)
	{
		// Create an instance of our allocation type.
		ContiguousAllocation *contig = new ContiguousAllocation(fileName);

		// Write to this instance of the contig object.
		contig->WriteToTable(fileName, fileSize, disk->table);
		
		// Then we store the instance in our Allocation return type conIn.
		conIn->ContigInstance = contig;
	}
	// In chained mode...
	else if (mode == ModeType::Chained)
	{
		// Create an instance of our allocation type.
		ChainedAllocation *chain = new ChainedAllocation(fileName);

		// Write to this instance of the chain object.
		chain->WriteToTable(fileName, fileSize, disk->table);

		// Then we store the instance in our Allocation return type conIn.
		conIn->ChainInstance = chain;
	}
	// In indexed mode...
	else if (mode == ModeType::Indexed)
	{
		// Create an instance of our allocation type.
		IndexedAllocation *index = new IndexedAllocation(fileName);

		// Write to this instance of the chain object.
		index->WriteToTable(fileName, fileSize, disk->table);
		
		// Then we store the instance in our Allocation return type conIn.
		conIn->IndexInstance = index;
	}

	// Then we return our Allocation storage object.
	return *conIn;
}

void Disk::SimulationToReal(std::string choice, Disk *disk)
{
	// First, we create a source stream.
	std::ifstream source(("Simulation/" + choice).c_str(), std::ios::binary);

	// Then we create a destination stream.
	std::ofstream destin(choice.c_str(), std::ios::binary);

	// We then copy the file in the simulation folder and paste it outside the simulation to the folder 
	//the program resides in.
	destin << source.rdbuf();
}

Allocation Disk::RealToSimulation(std::string choice, Disk *disk)
{
	// Create variables exclusive to this function here.
	struct stat statBuffer;
	Allocation conIn;
	
	// Inform the user and exit on failure to place file information in statBuffer.
	if (stat(choice.c_str(), &statBuffer) == -1)
	{
		std::cout << "This file does not exist in the main folder this program resides in." << std::endl;
		exit(EXIT_FAILURE);
	}
	// Otherwise, copy the file over and store the relevent information in variables.
	else if (stat(choice.c_str(), &statBuffer) != -1)
	{
		
		// Capture the file we want to copy and the folder we want to copy it to.
		std::ifstream source(choice.c_str(), std::ios::binary);
		std::ofstream destin(("Simulation/" + choice).c_str(), std::ios::binary);

		// Copy the file over to the Simulation folder.
		destin << source.rdbuf();

		// Send the file name and the file size to the write function.
		conIn = Write(choice, statBuffer.st_size, disk);
		
		// Inform the user of the success.
		std::cout << "File successfully added to the simulation." << std::endl;
	}	
	
	return conIn;
}

void Disk::InitializeSimulation()
{
	// Create variables exclusive to this function here.
	DIR *directory;
	//struct dirent *directoryEntry;

	// Open the simulation directory.
	if ((directory = opendir("Simulation")) != NULL)
	{
		// Create the files for the bitmap and the file allocation table.
		std::ofstream fileAllocation("Simulation/fiAl.txt");
		std::ofstream bitmap("Simulation/bmap.txt");

		// Then close the files and folders.
		fileAllocation.close();
		bitmap.close();
		closedir(directory);
	}
	// Inform the user of the failure and exit.
	else
	{
		std::cerr << "Failed to open the Simulation folder." << std::endl;
		exit(EXIT_FAILURE);
	}
}

bool Disk::FileExists(std::string choice)
{
	// Create variables exclusive to this function here.
	DIR *directory;
	struct dirent *directoryEntry;

	// Open the simulation directory.
	if ((directory = opendir("./")) != NULL)
	{
		// Check the directory for the user's choice.
		while ((directoryEntry = readdir(directory)) != NULL)
		{
			// If it exists, return true.
			if (directoryEntry->d_name == choice)
			{
				return true;
			}
		}
	}
	
	// If we could not find it, return false.
	return false;
}

std::string Disk::ToLower(std::string arg)
{
	// A variable with all letters made lowercase if they already weren't.
	for (u_int i = 0; i < arg.length(); i++)
	{
		arg[i] = std::tolower(arg[i]);
	}

	return arg;
}