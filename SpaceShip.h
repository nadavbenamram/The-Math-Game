#ifndef _SPACESHIP_H_
#define _SPACESHIP_H_

#include <iostream>
#include <list>
#include "Color.h"
#include "Sound.h"
#include "Point.h"
#include "io_utils.h"
#include "Resolution.h"

using namespace std;

enum { KEYS_NUM = 6 , BEGINNIG_DIRECTION = Direction::STAY};
enum { BUFFER_SIZE = 5 };
#define SPACE ' '

class TheMathGame;
class Artifact;

class SpaceShip{
	int width, height; //the size of the spaceship
	Point Start;
	char ch; //the sign of the spaceship.
	TheMathGame* TheGame;
	char arrowKeys[KEYS_NUM];
	int direction = BEGINNIG_DIRECTION;
	Color color = WHITE;
	Artifact* Dragged = nullptr;
	int PosOfDragged = -1; //for the first point of the artifact on the spaceship.

public:
	//set functions:
	void SetSpaceShip(Point _Start, char c, TheMathGame* game, Color col = WHITE)	{ Start = _Start; ch = c; TheGame = game; color = col; }
	void SetStartPoint(Point dest) { Start = dest; }
	void SetPosOfDragged(int _pos) { PosOfDragged = _pos; }
	void setArrowKeys(const char* keys);
	void SetColor(Color c) { color = c; }
	void setHeight(int h) { height = h; }
	void setWidth(int w) { width = w; }
	void setDirection(int dir) { direction = dir; }

	//get functions:
	char GetSign() { return ch; }
	Point GetStartPoint()const { return Start; }
	int GetPosOfDragged()const { return PosOfDragged; }
	char GetArrowKey(int index) { return arrowKeys[index]; }
	int getWidth()const { return width; }
	int getHeight()const { return height; }
	Artifact* GetDragged()const { return Dragged; }
	int GetDirectionOfSpaceship()const { return direction; }
	int GetDirection(const list<char>& keyHits);
	
	void InitSpaceShip(bool WithPrinting = true);
	void _print();
	void move();
	void RotateHelper(SoundFreq SoundFreqRotate);
	void DeleteSpaceShip();

private:
	void MoveWithArtifact();
	void MoveUpWithArtifact();
	void MoveDownWithArtifact();
	void MoveLeftWithArtifact();
	void MoveRightWithArtifact();
	void MoveWithoutArtifact();
	void CrossingMovment(Point dest);
	void CrossingMovmentWithArtifact(Point spaceshipDest, int LengthToAdd);
	void RegularMovment(Point dest);
	void RegularMovmentWithArtifact();
	void MoveByDirection();
	void MoveByDirectionWithArtifact(bool);
	void moveup();
	void movedown();
	void moveleft();
	void moveright();
	void CrossScreen(Point dest);
	void CopySpaceShipToNewPoint(Point dest);
	void swap(int& a, int& b);
	void Rotate(Point dest, SoundFreq SoundFreqRotate);
	bool getNumWithArtifact(char ArtifactCH, char SpaceShipCH);
	bool getNumWithArtifactAfterCrossing(char ArtifactCH, char SpaceShipCH, Point spaceshipDest, int LengthToAdd);
	void UpdateBoardAndMoveWithArtifact();
};

#endif