#include "GameState.h"

void GameState::Print()
{
	gotoxy(Start.getX(), Start.getY());
	cout << "L: " << Lives << " SN: " << ScreenNumber << " T: " << ClockCycles;
}

string GameState::getStrScreenNumber()
{
	string res;
	size_t ScreenID = ScreenNumber;

	while (ScreenID > 0)
	{
		res += (ScreenID % 10) + '0';
		ScreenID /= 10;
	}
	
	return res;
}