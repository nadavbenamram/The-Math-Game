#include "KeysByClockIteration.h"
#include <iostream>
#include <fstream>

using namespace std;

void KeysByClockIteration::_print()
{
	cout << clock_iteration << ": ";
	for (size_t i = 0; i < NumOfKeys; i++)
	{
		cout << Keys[i];
		if ((i + 1) < NumOfKeys)
			cout << ", ";
	}
	cout << endl;
}


void KeysByClockIteration::_OutToFile(ofstream& fout)
{
	fout << clock_iteration << ": ";
	for (size_t i = 0; i < NumOfKeys; i++)
	{
		fout << Keys[i];
		//if ((i + 1) < NumOfKeys)
			//fout << ", ";
	}
	fout << endl;
}

bool KeysByClockIteration::CheckIfSameKeys(char key)
{
	 if (Keys[0] == key)
		 return true; 
	 else
		 return false;
}
bool KeysByClockIteration::CheckIfSameKeys(char key1, char key2)
{
	if (Keys[0] == key1 && Keys[1] == key2)
		return true;
	else
		return false;
}