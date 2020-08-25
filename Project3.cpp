#include "Shared.h"

#include "Disk.h"
#include "UserInterface.h"
#include "Contiguous.h"
#include "Chained.h"
#include "Indexed.h"


// Debug functions.
void View(std::vector<Allocation> object);

int main(int arg, char** argc)
{
	// Define our variables here.
	Disk* disk = new Disk(arg, argc);
	UserInterface* ui = new UserInterface();

	bool first = true;
	bool fatDuplicate = false;
	bool bapDuplicate = false;
	std::vector<Allocation> *allocMemory = new std::vector<Allocation>;

	
	while (true)
	{
		// When the ui object hasn't just been initialized...
		if (!first)
		{
			// First, we display the user's choices.
			for (auto string : ui->optionList)
			{
				std::cout << string;
			}
		}

		// Then we get their choice. We're passing nothing because we don't need it here
		// but will in other functions.
		ui->GetChoice(GetChoiceType::MainMenu, disk->GetMode(), allocMemory);

		// On first iteration, set first to false to enable the output of their options again.
		first = false;

		// Display the file.
		if (ui->userInput == 1)
		{
			ui->DisplayFileContents(allocMemory, disk->GetMode());
		}
		// Display the file table.
		else if (ui->userInput == 2)
		{
			// First, we display the user files in the simulation.
			disk->DisplayFileTable(allocMemory);
			
			// Next, we update the block in the table that contains the file allocation table
			// and also define an instance for the allocation table.
			Allocation temp = disk->UpdateTableFileAlloBitmap(disk, UpdateFunctionType::FileAllocationTable);
			
			for (auto& allo : *allocMemory)
			{
				// Depending on the mode of the program, and if we found a match for the allocation table's
				// instance, update the instance in the vector.
				if (disk->GetMode() == ModeType::Contiguous && allo.ContigInstance->file == "fiAl.txt") // Make chained and indexed versions.
				{
					allo = temp;
					fatDuplicate = true;
					break;
				}
				else if (disk->GetMode() == ModeType::Indexed && allo.IndexInstance->file == "fiAl.txt")
				{
					allo = temp;
					fatDuplicate = true;
					break;
				}
				else if (disk->GetMode() == ModeType::Chained && allo.ChainInstance->file == "fiAl.txt")
				{
					allo = temp;
					fatDuplicate = true;
					break;
				}
			}

			// If there was no instance of the allocation table, add one to the vector. 
			if (!fatDuplicate)
			{
				allocMemory->push_back(temp);
			}
			
		}
		// Display the free space bitmap.
		else if (ui->userInput == 3)
		{
			// First, we display the bitmap while storing it in a file.
			disk->DisplayFreeSpaceBitmap(allocMemory);

			// We then create an Allocation instance of the bitmap to add it to the simulation.
			Allocation temp = disk->UpdateTableFileAlloBitmap(disk, UpdateFunctionType::Bitmap);

			for (unsigned int i = 0; i < allocMemory->size(); i++)
			{
				// Depending on the mode of the program, and if we found a match for the bitmap's
				// instance, update the instance in the vector.
				if (disk->GetMode() == ModeType::Contiguous && allocMemory->at(i).ContigInstance->file == "bmap.txt")
				{
					allocMemory->at(i) = temp;
					bapDuplicate = true;
					break;
				}
				else if (disk->GetMode() == ModeType::Indexed && allocMemory->at(i).IndexInstance->file == "bmap.txt")
				{
					allocMemory->at(i) = temp;
					
					bapDuplicate = true;
					break;
				}
				else if (disk->GetMode() == ModeType::Chained && allocMemory->at(i).ChainInstance->file == "bmap.txt")
				{
					allocMemory->at(i) = temp;
					bapDuplicate = true;
					break;
				}
			}

			// If there was no instance of the bitmap, add one to the vector. 
			if (!bapDuplicate)
			{
				allocMemory->push_back(temp);
			}
		}
		// Display a disk block.
		else if (ui->userInput == 4)
		{
			disk->DisplayDiskBlock(allocMemory, disk);
		}
		// Copy a file from the simulation to a file on the real system.
		else if (ui->userInput == 5)
		{
			// Store the user's choice inside a variable.
			std::string userChoice = ui->GetChoice(GetChoiceType::GetFileNameReal, disk->GetMode(), allocMemory);

			// Copy and paste the file with the same name as the user to the file this program is in.
			if (userChoice != "" && userChoice != "quit")
			{
				disk->SimulationToReal(userChoice, disk);
			}
		}
		// Copy a file from the real system to a file in the simulation.
		else if (ui->userInput == 6)
		{
			// Store the user's choice inside a variable.
			std::string userChoice = ui->GetChoice(GetChoiceType::GetFileName, disk->GetMode(), allocMemory);
			
 			// Determine if the file exists in the main folder.
			bool exists = disk->FileExists(userChoice);

			// If the user didn't choose to quit, and the file is in the same folder as this one...
			if (userChoice != "quit" && exists)
			{
				// Push the instance of the Allocation return type into the Allocation vector.
				allocMemory->push_back(disk->RealToSimulation(userChoice, disk));
			}
			// If the file is not in the same folder as this program...
			else if (userChoice != "quit" && !exists)
			{
				// Inform the user of the problem.
				std::cout << "The specified file does not exist in the main folder this program is in." << std::endl;
			}
		}
		// Delete a file of the user's choice if there are files in the simulation.
		else if (ui->userInput == 7)
		{
			if (allocMemory->size() == 0)
			{
				std::cout << "There are no files in the simulation. Please add a file first before deleting." << std::endl;
			}
			else
			{
				disk->DeleteFromSimulation(allocMemory, disk);
			}
		}
		// Exit the program.
		else if (ui->userInput == 8)
		{
			std::cout << "Exiting the program." << std::endl;
			return 0;
		}
	}
}

void View(std::vector<Allocation> object)
{
	//ContiguousAllocation *temp;
	//ChainedAllocation *temp;
	IndexedAllocation *temp;

	for (unsigned int i = 0; i < object.size(); i++)
	{
		//temp = object.at(i).ContigInstance;
		//temp = object.at(i).ChainInstance;
		temp = object.at(i).IndexInstance;
		//std::cout << "Name: " << temp->file << ", Length: " << temp->length << ", Start: " << temp->startIndex << std::endl;
		//std::cout << "Name: " << temp->file << ", End: " << temp->endIndex << ", Start: " << temp->startIndex << std::endl;

		std::cout << "Name: " << temp->file << ", indexBlock: " << temp->indexBlock << ", indexBlockSize: " << temp->indexBlockSize << std::endl;

		for (unsigned int i = 0; i < (unsigned int)temp->indexBlockSize; i++) 
		{ 
			std::cout << "Occupied Indices: " << temp->ptrTable.at(i) << std::endl;
		}
	}
}