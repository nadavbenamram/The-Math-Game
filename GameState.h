#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "Point.h"
#include <string>

class TheMathGame;

class GameState{
	Point Start;
	TheMathGame* TheGame;
	size_t ScreenNumber = 1;
	size_t ClockCycles = 0;
	size_t Lives = 3;

public:
	GameState(){}
	GameState(Point _start, TheMathGame* _thegame) : Start(_start), TheGame(_thegame) {}
	void Set(Point _start, TheMathGame* _thegame) { Start = _start; TheGame = _thegame; }
	void setScreenNumber(size_t _ScreenNumber) { ScreenNumber = _ScreenNumber; }
	void setClockCycles(size_t _ClockCycles) { ClockCycles = _ClockCycles; }
	void setLives(size_t _Lives) { Lives = _Lives; }
	void nextLevel(){ ScreenNumber++; Lives = 3; } //for updating the data before the next level
	void LessLife(){ Lives--; }
	unsigned int GetLives(){ return Lives; }
	void AddClockCycle() { ClockCycles++; }
	void Restart() { ClockCycles = 0; Lives = 3; }
	void Print();
	size_t getClockCycle() { return ClockCycles; }
	size_t getScreenNumber() { return ScreenNumber; }
	string getStrScreenNumber();
	size_t getLives() { return Lives; }
};

#endif