#include "Point.h"

void Point::move(int dir) {
	switch (dir) {
	case Direction::UP :
		--y;
		if (y < Resolution::HEIGHT_BEGINNIG) { //if y exceeds the resulotuin of the console.
			y = Resolution::HEIGHT_END;
		}
		break;
	case Direction::DOWN :
		++y;
		if (y > Resolution::HEIGHT_END) { //if y exceeds the resulotuin of the console.
			y = Resolution::HEIGHT_BEGINNIG;
		}
		break;
	case Direction::LEFT :
		--x;
		if (x < Resolution::WIDTH_BEGINNIG) { //if x exceeds the resulotuin of the console.
			x = Resolution::WIDTH_END;
		}
		break;
	case Direction::RIGHT :
		++x;
		if (x > Resolution::WIDTH_END) { //if x exceeds the resulotuin of the console.
			x = Resolution::WIDTH_BEGINNIG;
		}
		break;
	}
}

void Point::draw(char ch)const {
	gotoxy(x, y);
	cout << ch;
	cout.flush();
}

bool Point::operator== (const Point& comp)const
{
	return (((const int)x == comp.getX()) && ((const int)y == comp.getY()));
}