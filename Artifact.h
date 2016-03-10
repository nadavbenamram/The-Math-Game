#ifndef _ARTIFACT_H_
#define _ARTIFACT_H_

#include <iostream>
#include <list>
#include "Color.h"
#include "Point.h"
#include "SpaceShip.h"
#include "io_utils.h"
#include "Resolution.h"
#include "Direction.h"
#include "Creature.h"

using namespace std;

enum { STATIC , MOVEMENT };
enum { BEGINNIG_ARTIFACT = Direction::DOWN };
#define SPACE ' '

class TheMathGame;

class Artifact : public Creature{
	list <Point> pointList;
	int height = 0, width = 0;
	int heightAboveLeader = 0, widthAboveLeader = 0;
	bool isStatic = true;
	SpaceShip* Leader = nullptr; //pointer for the spaceship that the artifact landed on

	//private functions:
	void MoveArtifactAfterCrossing(int direction, int LengthToAdd);

public:
	//constructors:
	Artifact() : Creature() {}
	Artifact(Point firstpoint, TheMathGame* _theGame, char _ch = '*', Color _color = Color::LIGHTRED, int _direction = BEGINNIG_ARTIFACT) : Creature(_ch, _direction, _color, _theGame) { pointList.push_back(firstpoint); }
	Artifact(TheMathGame* _theGame, char _ch = '*', Color _color = Color::LIGHTRED, int _direction = BEGINNIG_ARTIFACT) : Creature(_ch, _direction, _color, _theGame) {}

	//set functions:
	void setArtifact(Point firstpoint, TheMathGame* _theGame, char _ch, Color _color, int _direction = BEGINNIG_ARTIFACT);
	void SetStatic() { isStatic = true; }
	void SetLeader(SpaceShip* _Leader) { Leader = _Leader; }
	void SetWidth(int _width) { width = _width; }
	void SetHeight(int _height) { height = _height; }

	//get functions:
	bool IsStatic() { return isStatic; }
	double GetArtifactVolume() { return (double)pointList.size(); }
	int getHeight() { return height; }
	int getWidth() { return width; }
	int getHeightAboveLeader() { return heightAboveLeader; }
	int getWidthAboveLeader() { return widthAboveLeader; }
	list <Point>& getPointList() { return pointList; }
	list <Point> getPointList()const { return pointList; }
	SpaceShip* getLeader() { return Leader; }

	void AddPoint(Point pointToAdd)	{ pointList.push_back(pointToAdd); }
	bool findPoint(const Point& point);

	virtual void print()const override;
	virtual void init(bool WithPrinting = true) override;
	virtual bool move() override;
	virtual bool move(int direction, int LengthToAdd = 0) override;
	virtual void CrossScreen(int direction, int LengthToAdd) override;
	virtual bool RegularMovment(int direction, int LengthToAdd) override;
	virtual bool CrossingMovment(int direction, int LengthToAdd = 0) override;
	virtual bool CheckIfNeedToCross(int direction, int LengthToAdd=0)const override;
	virtual void MoveByDirection(int direction, int LengthToAdd = 0) override;
	virtual bool CheckIfThisPointBelongToMe(const Point& p) override;
	virtual Creature* CreateCopyOfMe() override;
	virtual void DeleteMe() override;

	void moveup(int LengthToAdd = 0);
	void movedown(int LengthToAdd = 0);
	void moveleft(int LengthToAdd = 0);
	void moveright(int LengthToAdd = 0);
	void CalculateWidthAndHeight();
	void CalculateWidthAndHeight(SpaceShip* s);
	void PushAnotherAfterCrossing(Artifact& another, int direction);

	bool operator== (const Artifact& ar);
};

#endif