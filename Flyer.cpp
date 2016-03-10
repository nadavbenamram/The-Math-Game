#include "Flyer.h"
#include "TheMathGame.h"


void Flyer::print()const 
{
	setTextColor(color);
	location.draw(ch);
	setTextColor(WHITE);
}

void Flyer::init(bool WithPrinting)
{
	if (WithPrinting) print();
}

bool Flyer::move()
{
	//if the flyer need to cross.
	if (CheckIfNeedToCross(direction, 0))
	{
		return CrossingMovment(direction, 0);
	}
	else { return RegularMovment(direction, 0); }
}

bool Flyer::move(int direction, int LengthToAdd)
{
	//if the flyer need to cross.
	if (CheckIfNeedToCross(direction, LengthToAdd)) 
	{ 
		return CrossingMovment(direction, LengthToAdd);
	}
	else { return RegularMovment(direction, LengthToAdd); }
}

void Flyer::CrossScreen(int direction, int LengthToAdd)
{
	Point dest;

	dest = location;
	MoveDestPoint(dest, direction);
	TheGame->UpdateBoardWhenCrossScreen(location, dest, 1, 1); //update the board.
	location.draw(TheMathGame::GameChars::FREE_PASS); //delete Flyer.

	location = dest;
	setTextColor(color); //set TextColor for draw.
	location.draw(ch);
	setTextColor(WHITE);
}

bool Flyer::RegularMovment(int direction, int LengthToAdd)
{
	char SignificantCh;
	Point PointOfDirection = location;
	
	MovePointByDirection(PointOfDirection, direction);
	SignificantCh = TheGame->GetSignificantChar(direction, location, 1, 1);

	//if the flyer's significant char is a digit, delete the all numbers, update the board and move.
	if (SignificantCh >= '0' && SignificantCh <= '9')
	{
		TheGame->DeleteTheAllNumbers(PointOfDirection,direction,1,1); //delete the number
		TheGame->UpdateBoard(direction, location, 1, 1);
		MoveByDirection(direction, LengthToAdd);
	}

	//if the flyer's significant char is one of the spaceships, bomb the space ship.
	else if (SignificantCh == TheMathGame::GameChars::BIG_SPACE_SHIP_SIGN || SignificantCh == TheMathGame::GameChars::SMALL_SPACE_SHIP_SIGN)
	{
		TheGame->BombSpaceShip(SignificantCh);
	}

	//if the flyer's significant char is one of the spaceships, bomb the space ship.
	else if (SignificantCh == TheMathGame::GameChars::ARTIFACT)
	{
		auto tempArtifact = reinterpret_cast<Artifact*>(TheGame->findCreatureByPoint(PointOfDirection));
		if (tempArtifact->getLeader() != nullptr)
			TheGame->BombSpaceShip(tempArtifact->getLeader()->GetSign());
		else
		{
			TheGame->BombCreature(PointOfDirection);
			TheGame->UpdateBoard(direction, location, 1, 1);
			MoveByDirection(direction, LengthToAdd);
		} 
	}

	//else move 
	else
	{
		TheGame->UpdateBoard(direction, location, 1, 1);
		MoveByDirection(direction, LengthToAdd);
	}

	return true;
}

bool Flyer::CrossingMovment(int direction, int LengthToAdd)
{
	char SignificantCh;
	Point dest;

	dest = location;
	MoveDestPoint(dest, direction);
	SignificantCh = TheGame->GetSignificantCharAfterCrossingScreen(dest, 1, 1);

	//if the flyer's significant char is a digit, delete the all numbers, update the board and move.
	if (SignificantCh >= '0' && SignificantCh <= '9')
	{
		TheGame->DeleteTheAllNumbers(location, direction, 1, 1); //delete the number
		CrossScreen(direction, LengthToAdd);
	}

	//if the flyer's significant char is one of the spaceships, bomb the space ship.
	else if (SignificantCh == TheMathGame::GameChars::BIG_SPACE_SHIP_SIGN || SignificantCh == TheMathGame::GameChars::SMALL_SPACE_SHIP_SIGN)
	{
		TheGame->BombSpaceShip(SignificantCh);
	}

	//if the flyer's significant char is one of the spaceships, bomb the space ship.
	else if (SignificantCh == TheMathGame::GameChars::ARTIFACT)
	{
		Point PointOfArtifact = location;
		MovePointByDirection(PointOfArtifact, direction);
		TheGame->BombCreature(PointOfArtifact);
		CrossScreen(direction, LengthToAdd);
	}

	//else move 
	else
	{
		CrossScreen(direction, LengthToAdd);
	}

	return true;
}

void Flyer::MoveByDirection(int direction, int LengthToAdd)
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

bool Flyer::CheckIfNeedToCross(int direction, int LengthToAdd)const
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

void Flyer::MoveDestPoint(Point& original, int direction)
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

void Flyer::MovePointByDirection(Point& original, int direction)
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

Creature* Flyer::CreateCopyOfMe()
{
	Creature* res;
	try { res = new Flyer(*this); }
	catch (bad_alloc& ba)
	{
		TheGame->DeleteAllMemoryAllocations();
		cerr << "bad_alloc caught: " << ba.what() << '\n';
		exit(1);
	}
	return res;
}

void Flyer::DeleteMe()
{
	location.draw(SPACE);

	TheGame->DeleteFlyerOrNumbersEeaterFromBoard(location);
}