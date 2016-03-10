#ifndef _FLYER_H_
#define _FLYER_H_

using namespace std;
#include "Color.h"
#include "Creature.h"
#include "Point.h"
class TheMathGame;

class Flyer : public Creature {
	Point location;

	//private functions:
	void MoveDestPoint(Point& original, int direction);
	void MovePointByDirection(Point& original, int direction);

public:
	//constructors:
	Flyer() : Creature() {}
	Flyer(Point _p, char _ch, int _direction, Color _color, TheMathGame* _TheGame) : Creature(_ch, _direction, _color, _TheGame) { location = _p; }

	//set functions:
	void setFlyer(Point _p, char _ch, int _direction, Color _color, TheMathGame* _TheGame) { setCreature(_ch, _direction, _color, _TheGame); location = _p; }
	void setLocation(Point _location) { location = _location; }

	virtual void print()const override;
	virtual void init(bool WithPrinting = true) override;
	virtual bool move() override;
	virtual bool move(int direction, int LengthToAdd = 0) override;
	virtual void CrossScreen(int direction, int LengthToAdd = 0) override;
	virtual bool RegularMovment(int direction, int LengthToAdd) override;
	virtual bool CrossingMovment(int direction, int LengthToAdd = 0) override;
	virtual bool CheckIfNeedToCross(int direction, int LengthToAdd = 0)const override;
	virtual void MoveByDirection(int direction, int LengthToAdd = 0) override;
	virtual bool CheckIfThisPointBelongToMe(const Point& p) override { return (p == location); }
	virtual Creature* CreateCopyOfMe() override;
	virtual void DeleteMe() override;

};

#endif
