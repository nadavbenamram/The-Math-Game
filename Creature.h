#ifndef _CRETURE_H_
#define _CRETURE_H_

using namespace std;
#include "Color.h"
#include "Point.h"

#define SPACE ' '

class TheMathGame;

class Creature {

protected:
	char ch;
	int direction;
	Color color;
	TheMathGame* TheGame;

public:
	//constructors:
	Creature() {}
	Creature(char _ch, int _direction, Color _color, TheMathGame* _TheGame) : ch(_ch), direction(_direction), color(_color), TheGame(_TheGame) {}

	//set functions:
	void setCreature(char _ch, int _direction, Color _color, TheMathGame* _TheGame) { ch = _ch; direction = _direction; color = _color; TheGame = _TheGame; }
	void setDirection(int _direction) { direction = _direction; }
	void SetColor(Color c) { color = c; }

	//get functions:
	int getDirection() { return direction; }
	char getSign() { return ch; }

	virtual void print()const = 0;
	virtual void init(bool WithPrinting = true) = 0;
	virtual bool move() = 0;
	virtual bool move(int direction, int LengthToAdd = 0) = 0;
	virtual void CrossScreen(int direction, int LengthToAdd = 0) = 0;
	virtual bool RegularMovment(int direction, int LengthToAdd = 0) = 0;
	virtual bool CrossingMovment(int direction, int LengthToAdd = 0) = 0;
	virtual bool CheckIfNeedToCross(int direction, int LengthToAdd = 0)const = 0;
	virtual void MoveByDirection(int direction, int LengthToAdd = 0) = 0;
	virtual bool CheckIfThisPointBelongToMe(const Point& p) = 0;
	Creature* CreateNewCreature() { return CreateCopyOfMe(); }
	virtual Creature* CreateCopyOfMe() = 0;
	virtual void DeleteMe() = 0;
};

#endif
