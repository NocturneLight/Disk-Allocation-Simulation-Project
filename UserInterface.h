#pragma once

#include "Shared.h"

class UserInterface
{
  public:
	
	 // My variables go here.
	 int userInput;
	 
	 // The constructor and destructor.
	 UserInterface();
	~UserInterface();

	 // Public function prototypes go here.
	 int ReturnChoice();
	 void DisplayFileContents(std::vector<Allocation> *alloc, ModeType mode);
	 void DeleteFromSimulation();
	 std::string GetChoice(GetChoiceType mode, ModeType allocationMode, std::vector<Allocation> *alloc);

	 std::list<std::string> optionList =
	 {
	   "1) Display a file\n",
	   "2) Display the file table\n",
	   "3) Display the free space bitmap\n",
	   "4) Display a disk block\n",
	   "5) Copy a file from the simulation to a file on the real system\n",
	   "6) Copy a file from the real system to a file in the simulation\n",
	   "7) Delete a file\n",
	   "8) Exit\n"
	 };

  private:
	 // Private variables go here.

     // Private functions prototypes go here.
	
	
};

UserInterface::UserInterface()
{
	// Create variables for this function here.
	//std::vector<Allocation> *nothing = new std::vector<Allocation>;

	// First, we display the user's choices.
	for (auto string : optionList)
	{
		std::cout << string;
	}
}

void UserInterface::DisplayFileContents(std::vector<Allocation> *alloc, ModeType mode)
{
	// Create variables exclusive to this function here.
	bool match = false;
	std::string chosenName = "";

	if (alloc->size() == 0)
	{
		std::cout << "There are no files in the simulation to display. Add a file first." << std::endl;
	}
	else
	{
		while (true)
		{
			// Ask the user to pick a file to move into the simulation.
			std::cout << "Type the name of the file you wish to view the contents of: ";

			// Store their response in a variable.
			std::cin >> chosenName;

			// Loop through the vector and check if the chosen file is in the simulation.
			for (auto allo : *alloc)
			{
				// In contiguous mode, check the contiguous instance.
				if ((mode == ModeType::Contiguous) && (chosenName == allo.ContigInstance->file))
				{
					match = !match;
					break;
				}
				// In indexed mode, check the indexed instance.
				else if ((mode == ModeType::Indexed) && (chosenName == allo.IndexInstance->file))
				{
					match = !match;
					break;
				}
				// In chained mode, check the chained instance.
				else if ((mode == ModeType::Chained) && (chosenName == allo.ChainInstance->file))
				{
					match = !match;
					break;
				}
			}

			// If there was a match, break from the while loop.
			if (match)
			{
				break;
			}
			// If there was no match, tell the user and continue looping.
			else if (!match)
			{
				std::cout << "The file you chose does not exist in the simulation. Please try again." << std::endl;
			}
		}

		// Capture the file in the stream.
		std::ifstream file(("Simulation/" + chosenName).c_str());

		// Open up the file and display it's contents.
		if (file.is_open())
		{
			std::cout << file.rdbuf() << std::endl;
		}
		// Inform the user that the file could not be opened.
		else
		{
			std::cout << "There was problem with opening the file. It failed to be opened. Check to make sure it is still in the simulation folder." << std::endl;
		}
	}
}

std::string UserInterface::GetChoice(GetChoiceType mode, ModeType allocationMode, std::vector<Allocation> *alloc)
{
	// Create variables exclusive to this function here.
	bool loop = true;
	std::string tempValue = "";
	
	// Loop until the signal is given to stop.
	while (loop)
	{
		// If we're in Main Menu mode...
		if (mode == GetChoiceType::MainMenu)
		{
			try
			{
				// Ask the user to make a decision.
				std::cout << "Option: ";

				// Store their choice in a variable.
				std::cin >> tempValue;
				
				// Try and convert their choice to an integer value. If the parsing fails or if they fail to give a number between 
				// 1 and 8, throw to the catch statement and inform the user their input is invalid. Then loop back to the top and
				// try again.
				this->userInput = std::stoi(tempValue) >= 1 && std::stoi(tempValue) <= 8 ? std::stoi(tempValue) : std::stoi("");

				// If parsing succeeds, break from the while loop.
				loop = !loop;
				
				
			}
			catch (const std::exception&)
			{
				// Inform the user their input is invalid.
				std::cerr << "Invalid option. Please pick a number between 1 and 8." << std::endl;
			}
		}
		// If we're in Get File Name mode...
		else if (mode == GetChoiceType::GetFileName)
		{
			// Ask the user to pick a file to move into the simulation.
			std::cout << "Type the name of the file you wish to add to the simulation or type \"quit\" to quit: ";

			// Store their response in a variable.
			std::cin >> tempValue;

			// Return the string "quit" if the user types "quit".
			if (Disk::ToLower(tempValue) == "quit")
			{
				return tempValue;
			}
			// Inform the user their file name is too long.
			else if (tempValue.length() > 8)
			{
				std::cout << "The name of the file you chose is too long. The max character limit for file names is 8. Please try again." << std::endl;
			}
			// If the size of the Allocation vector is 0, it's guaranteed their choice isn't already in the simulation. So just return the string. 
			else if (alloc->size() == 0)
			{
				return tempValue;
			}
			// Check the vector of Allocations for their choice.
			else
			{
				// Iterate through the allocation vector.
				for (unsigned int i = 0; i < alloc->size(); i++)
				{
					// Depending on what allocation mode we're in, check the appropriate object.
					if (allocationMode == ModeType::Chained)
					{
						// If we get an allocation type that has a file name the same as the user's choice,
						// inform the user that the file is in the simulation already and that they should
						// pick a different name.
						if (tempValue == alloc->at(i).ChainInstance->file)
						{
							std::cout << "This file is already in the simulation. Please pick a different filename." << std::endl;
							break;
						}
						// If we reach the end, their file isn't already in the simulation, so return their choice.
						else if (i == alloc->size() - 1)
						{
							return tempValue;
						}
					}
					else if (allocationMode == ModeType::Contiguous)
					{
						// If we get an allocation type that has a file name the same as the user's choice,
						// inform the user that the file is in the simulation already and that they should
						// pick a different name.
						if (tempValue == alloc->at(i).ContigInstance->file)
						{
							std::cout << "This file is already in the simulation. Please pick a different filename." << std::endl;
							break;
						}
						// If we reach the end, their file isn't already in the simulation, so return their choice.
						else if (i == alloc->size() - 1)
						{
							return tempValue;
						}
					}
					else if (allocationMode == ModeType::Indexed)
					{
						// If we get an allocation type that has a file name the same as the user's choice,
						// inform the user that the file is in the simulation already and that they should
						// pick a different name.
						if (tempValue == alloc->at(i).IndexInstance->file)
						{
							std::cout << "This file is already in the simulation. Please pick a different filename." << std::endl;
							break;
						}
						// If we reach the end, their file isn't already in the simulation, so return their choice.
						else if (i == alloc->size() - 1)
						{
							return tempValue;
						}
					}
				}
			}
		}
		else if (mode == GetChoiceType::GetFileNameReal)
		{
			if (alloc->size() == 0)
			{
				std::cout << "The Simulation folder is empty. To move something from the Simulation to the real system, you must first add something to the Simulation." << std::endl;

				return "";
			}

			// Ask the user to pick a file to move into the simulation.
			std::cout << "Type the name of the file you wish to add to the real system or type \"quit\" to quit: ";

			// Store their response in a variable.
			std::cin >> tempValue;
			
			// Return the string "quit" if the user types "quit".
			if (Disk::ToLower(tempValue) == "quit")
			{
				return tempValue;
			}
			// If the size of the Allocation vector is 0, they have nothing to add to the real system.
			// Inform them and return with quit.
			else
			{
				bool match = false;

				// Iterate through the allocation vector.
				for (unsigned int i = 0; i < alloc->size(); i++)
				{
					// Depending on what allocation mode we're in, check the appropriate object.
					if (allocationMode == ModeType::Chained)
					{
						if (tempValue == alloc->at(i).ChainInstance->file)
						{
							match = !match;
							break;
						}
					}
					else if (allocationMode == ModeType::Contiguous)
					{
						if (tempValue == alloc->at(i).ContigInstance->file)
						{
							match = !match;
							break;
						}
					}
					else if (allocationMode == ModeType::Indexed)
					{
						if (tempValue == alloc->at(i).IndexInstance->file)
						{
							match = !match;
							break;
						}
					}
				}

				// Return their value if it was a match.
				if (match)
				{
					return tempValue;
				}
				// Inform them of the problem if there was no match.
				else if (!match)
				{
					std::cout << "The file you chose is not part of the simulation. Please try a different file name." << std::endl;
				}
			}
		}
	}
	
	return tempValue;
}
