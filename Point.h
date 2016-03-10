#ifndef _POINT_H_
#define _POINT_H_

#include <iostream>
#include <math.h>
#include "io_utils.h"
#include "Direction.h"
#include "Resolution.h"

using namespace std;

class Point {
	int x = 1, y = 1;
public:
	Point() {}
	Point(int _x, int _y) : x(_x), y(_y) {}
	void set(int _x, int _y){ x = _x; y = _y;}
	int getX()const { return x; } //for getting x value (SpaceShip::InitSpaceShip need this functuin).
	int getY()const { return y; } //for getting y value (SpaceShip::InitSpaceShip need this functuin).
	void draw(char ch)const;
	void move(int direction);
	double calcDistance(const Point& p) { return pow(pow(p.x - x, 2) + pow(p.y - y, 2), 0.5); }
	bool operator== (const Point& comp)const;
	Point& operator= (const Point& other) { x = other.getX(); y = other.getY(); return *this; };
};

#endif