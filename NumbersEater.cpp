#include "NumbersEater.h"
#include "TheMathGame.h"


void NumbersEater::print()const 
{
	setTextColor(color);
	location.draw(ch);
	setTextColor(WHITE);
}

void NumbersEater::init(bool WithPrinting)
{
	if (WithPrinting) print();
}

bool NumbersEater::move()
{
	if (!(TheGame->checkIfNumberIsStillAlive(target)))
		target = TheGame->getClosestPoint(location);

	setDirectionForGetTarget();

	//if the Numbers Eater need to cross.
	if (CheckIfNeedToCross(direction, 0)) { return CrossingMovment(direction, 0); }
	else { return RegularMovment(direction, 0); }

}

bool NumbersEater::move(int direction, int LengthToAdd)
{
	if (!(TheGame->checkIfNumberIsStillAlive(target)))
		target = TheGame->getClosestPoint(location);

	setDirectionForGetTarget();

	//if the Numbers Eater need to cross.
	if (CheckIfNeedToCross(direction, LengthToAdd)) { return CrossingMovment(direction, LengthToAdd); }
	else { return RegularMovment(direction, LengthToAdd); }

}

void NumbersEater::CrossScreen(int direction, int LengthToAdd)
{
	Point dest;

	dest = location;
	MoveDestPoint(dest, direction);
	TheGame->UpdateBoardWhenCrossScreen(location, dest, 1, 1); //update the board.
	location.draw(TheMathGame::GameChars::FREE_PASS); //delete Numbers Eater.

	location = dest;
	setTextColor(color); //set TextColor for draw.
	location.draw(ch);
	setTextColor(WHITE);
}

bool NumbersEater::RegularMovment(int direction, int LengthToAdd)
{
	char SignificantCh;
	Point PointOfDirection = location;

	MovePointByDirection(PointOfDirection, direction);
	SignificantCh = TheGame->GetSignificantChar(direction, location, 1, 1);

	//if the Numbers Eater's significant char is a digit, delete the all numbers, update the board and move.
	if (SignificantCh >= '0' && SignificantCh <= '9')
	{
		TheGame->DeleteTheAllNumbers(PointOfDirection, direction, 1, 1); //delete the number
		TheGame->UpdateBoard(direction, location, 1, 1);
		MoveByDirection(direction, LengthToAdd);
		return true;
	}

	//else if free pass - move 
	else if (SignificantCh == TheMathGame::GameChars::FREE_PASS)
	{
		TheGame->UpdateBoard(direction, location, 1, 1);
		MoveByDirection(direction, LengthToAdd);
		return true;
	}

	else
	{
		TheGame->BombCreature(location);
		return false;
	}

	return false;
}

bool NumbersEater::CrossingMovment(int direction, int LengthToAdd)
{
	char SignificantCh;
	Point dest;
	
	dest = location;
	MoveDestPoint(dest, direction);
	SignificantCh = TheGame->GetSignificantCharAfterCrossingScreen(dest, 1, 1);

	//if the Numbers Eater's significant char is a digit, delete the all numbers, update the board and move.
	if (SignificantCh >= '0' && SignificantCh <= '9')
	{
		TheGame->DeleteTheAllNumbers(location, direction, 1, 1); //delete the number
		CrossScreen(direction, LengthToAdd);
	}

	//else if free pass - move 
	else if (SignificantCh == TheMathGame::GameChars::FREE_PASS)
	{
		CrossScreen(direction, LengthToAdd);
	}

	else
	{
		TheGame->BombCreature(location);
	}

	return true;
}

void NumbersEater::MoveByDirection(int direction, int LengthToAdd)
{
	switch (direction)
	{
	case Direction::UP:
		location.draw(TheMathGame::GameChars::FREE_PASS);
		setTextColor(color); //set color for draw.
		location.set(location.getX(), location.getY() - 1);
		location.draw(ch);
		setTextColor(WHITE); //return the color to white.
		break;
	case Direction::DOWN:
		location.draw(TheMathGame::GameChars::FREE_PASS);
		setTextColor(color); //set color for draw.
		location.set(location.getX(), location.getY() + 1);
		location.draw(ch);
		setTextColor(WHITE); //return the color to white.
		break;
	case Direction::LEFT:
		location.draw(TheMathGame::GameChars::FREE_PASS);
		setTextColor(color); //set color for draw.
		location.set(location.getX() - 1, location.getY());
		location.draw(ch);
		setTextColor(WHITE); //return the color to white.
		break;
	case Direction::RIGHT:
		location.draw(TheMathGame::GameChars::FREE_PASS);
		setTextColor(color); //set color for draw.
		location.set(location.getX() + 1, location.getY());
		location.draw(ch);
		setTextColor(WHITE); //return the color to white.
		break;
	default: break;
	}
}

bool NumbersEater::CheckIfNeedToCross(int direction, int LengthToAdd)const 
{
	switch (direction) {
	case Direction::UP:
		if (location.getY() == Resolution::HEIGHT_BEGINNIG)
			return true;
		break;
	case Direction::DOWN:
		if (location.getY() == Resolution::HEIGHT_END)
			return true;
		break;
	case Direction::LEFT:
		if (location.getX() == Resolution::WIDTH_BEGINNIG)
			return true;
		break;
	case Direction::RIGHT:
		if (location.getX() == Resolution::WIDTH_END)
			return true;
		break;
	default: return false;
	}
	return false;
}

void NumbersEater::MoveDestPoint(Point& original, int direction)
{
	switch (direction)
	{
	case Direction::UP:
		original.set(original.getX(), Resolution::HEIGHT_END);
		break;
	case Direction::DOWN:
		original.set(original.getX(), Resolution::HEIGHT_BEGINNIG);
		break;
	case Direction::LEFT:
		original.set(Resolution::WIDTH_END, original.getY());
		break;
	case Direction::RIGHT:
		original.set(Resolution::WIDTH_BEGINNIG, original.getY());
		break;
	}
}

void NumbersEater::MovePointByDirection(Point& original, int direction)
{
	switch (direction)
	{
	case Direction::UP:
		original.set(original.getX(), original.getY() - 1);
		break;
	case Direction::DOWN:
		original.set(original.getX(), original.getY() + 1);
		break;
	case Direction::LEFT:
		original.set(original.getX() - 1, original.getY());
		break;
	case Direction::RIGHT:
		original.set(original.getX() + 1, original.getY());
		break;
	}
}

Creature* NumbersEater::CreateCopyOfMe()
{
	Creature* res;
	try { res = new NumbersEater(*this); }
	catch (bad_alloc& ba)
	{
		TheGame->DeleteAllMemoryAllocations();
		cerr << "bad_alloc caught: " << ba.what() << '\n';
		exit(1);
	}
	return res;
}

void NumbersEater::DeleteMe() 
{
	location.draw(SPACE);
	TheGame->DeleteFlyerOrNumbersEeaterFromBoard(location);
}

void NumbersEater::setDirectionForGetTarget()
{
	if (location.getY() > target.getY())
		direction = Direction::UP;
	else if (location.getY() < target.getY())
		direction = Direction::DOWN;
	else if (location.getX() > target.getX())
		direction = Direction::LEFT;
	else if (location.getX() < target.getX())
		direction = Direction::RIGHT;
}