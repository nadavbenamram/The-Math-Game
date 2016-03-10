#include "Artifact.h"
#include "TheMathGame.h"
#include "Creature.h"

//set artifact
void Artifact::setArtifact(Point firstpoint, TheMathGame* _theGame, char _ch, Color _color, int _direction)
{
	pointList.clear(); //for case that this is not the first round of the game.
	pointList.push_back(firstpoint);

	setCreature(_ch, _direction, _color, _theGame);
}

//print the Artifact.
void Artifact::print()const
{
	setTextColor(color);
	for (Point p : pointList)
		p.draw(ch);
	setTextColor(WHITE);
}

//input: print the artifact at init or not.
void Artifact::init(bool WithPrinting)
{
	CalculateWidthAndHeight();
	Leader = TheGame->StaticsCheckArtifact(*this, isStatic);
	isStatic = true;
	if (WithPrinting) print();
}

//return: true if the movment succeeded, false otherwise.
bool Artifact::move()
{
	char ch; //for significant char (in case that the artifact is static).
	bool res = false;

	if (Leader == nullptr) //if there isn't a leader move by diretcion.
	{
		//make static check.
		Leader = TheGame->StaticsCheckArtifact(*this, isStatic);

		//if the artifact need to cross.
		if (CheckIfNeedToCross(direction, 0) && (direction != Direction::DOWN || (!isStatic)))
		{
			res = CrossingMovment(direction);
		}
		else if ((!isStatic) || direction != Direction::DOWN)
		{
			res = RegularMovment(direction, 0);
		}
	}
	else res = false;

	//if the artifact is static and the significant char under him is free pass, the artifact isn't static anymore.
	if (isStatic && (ch = TheGame->GetSignificantChar(Direction::DOWN, *this) == TheMathGame::GameChars::FREE_PASS))
		isStatic = false;

	return res;
}

//input: direction, Length To Add (default is 0, for case that we need to move the artifact more than 1 step)
//return: true if the movment succeeded, false otherwise.
bool Artifact::move(int direction, int LengthToAdd)
{
	char ch; //for significant char (in case that the artifact is static).
	bool res = false;

	if (Leader == nullptr ) //if there isn't a leader move by diretcion.
	{
		//make static check.
		Leader = TheGame->StaticsCheckArtifact(*this, isStatic);
		
		//if the artifact need to cross.
		if (CheckIfNeedToCross(direction, LengthToAdd) && (direction != Direction::DOWN || (!isStatic)))
		{
			res = CrossingMovment(direction);
		}
		else if ((!isStatic) || direction != Direction::DOWN)
		{
			res = RegularMovment(direction, LengthToAdd);
		}
	}
	else res = false;
	
	//if the artifact is static and the significant char under him is free pass, the artifact isn't static anymore.
	if (isStatic && (ch = TheGame->GetSignificantChar(Direction::DOWN, *this) == TheMathGame::GameChars::FREE_PASS))
		isStatic = false;
	
	return res;
}

//////////////////////move functions////////////////////

void Artifact::moveup(int LengthToAdd)
{
	for (Point p : pointList) //delete the points from screen.
	{
		p.draw(SPACE);
	}
	setTextColor(color); //set color for draw.
	for (Point& p : pointList) //move all the points up.
	{
		p.set(p.getX(), p.getY() - 1 - LengthToAdd);
	}
	for (Point p : pointList) //draw ch.
	{
		p.draw(ch);
	}
	setTextColor(WHITE); //return the color to white.
}

void Artifact::movedown(int LengthToAdd)
{
	for (Point p : pointList) //delete the points from screen.
	{
		p.draw(SPACE);
	}
	setTextColor(color); //set color for draw.
	for (Point& p : pointList) //move all the points down.
	{
		p.set(p.getX(), p.getY() + 1 + LengthToAdd);
	}
	for (Point p : pointList) //draw ch.
	{
		p.draw(ch);
	}
	setTextColor(WHITE); //return the color to white.
}

void Artifact::moveleft(int LengthToAdd)
{
	for (Point p : pointList) //delete the points from screen.
	{
		p.draw(SPACE);
	}
	setTextColor(color); //set color for draw.
	for (Point& p : pointList) //move all the points left.
	{
		p.set(p.getX() - 1 - LengthToAdd, p.getY());
	}
	for (Point p : pointList) //draw ch.
	{
		p.draw(ch);
	}
	setTextColor(WHITE); //return the color to white.
}

void Artifact::moveright(int LengthToAdd)
{
	for (Point p : pointList) //delete the points from screen.
	{
		p.draw(SPACE);
	}
	setTextColor(color); //set color for draw.
	for (Point& p : pointList) //move all the points right.
	{
		p.set(p.getX() + 1 + LengthToAdd, p.getY());
	}
	for (Point p : pointList) //draw ch.
	{
		p.draw(ch);
	}
	setTextColor(WHITE); //return the color to white.
}

void Artifact::MoveByDirection(int direction, int LengthToAdd)
{
	switch (direction)
	{
	case Direction::UP:
		moveup(LengthToAdd);
		break;
	case Direction::DOWN:
		movedown(LengthToAdd);
		break;
	case Direction::LEFT:
		moveleft(LengthToAdd);
		break;
	case Direction::RIGHT:
		moveright(LengthToAdd);
		break;
	default: break;
	}
}

/////////////////////////////////////////////////////////

//input: direction, LengthToAdd.
//return: true if one of the points of the artifact is on the edge of the screen at this direction, false otherwise.
bool Artifact::CheckIfNeedToCross(int direction, int LengthToAdd)const 
{
	switch (direction) {
	case Direction::UP:
		for (Point p : pointList) 
		{
			if ((p.getY() - LengthToAdd) == Resolution::HEIGHT_BEGINNIG)
				return true;
		}
		break;
	case Direction::DOWN:
		for (Point p : pointList) 
		{
			if ((p.getY() + LengthToAdd) == Resolution::HEIGHT_END)
				return true;
		}
		break;
	case Direction::LEFT:
		for (Point p : pointList)
		{
			if ((p.getX() - LengthToAdd ) == Resolution::WIDTH_BEGINNIG)
				return true;
		}
		break;
	case Direction::RIGHT:
		for (Point p : pointList)
		{
			if ( (p.getX() + LengthToAdd) == Resolution::WIDTH_END)
				return true;
		}
		break;
	default: return false;
	}
	return false;
}

//input: direction, LengthToAdd.
//return: true if the artifact moved to the new destination, false otherwise,
bool Artifact::RegularMovment(int direction ,int LengthToAdd)
{
	char SignificantCh;
	SignificantCh = TheGame->GetSignificantChar(direction, *this, LengthToAdd);
	
	//if the char is free pass, update and move by direction.
	if (SignificantCh == TheMathGame::GameChars::FREE_PASS)
	{
		TheGame->UpdateBoard(direction, pointList, LengthToAdd);
		MoveByDirection(direction, LengthToAdd);
		return true;
	}

	//if the char is a digit, delete the all numbers, update the board and move.
	else if (SignificantCh >= '0' && SignificantCh <= '9') 
	{
		TheGame->DeleteTheAllNumbers(direction, pointList, LengthToAdd); //delete the number
		TheGame->UpdateBoard(direction, pointList, LengthToAdd);
		MoveByDirection(direction, LengthToAdd);
		return true;
	}

	//if the char is one of the spaceships, bomb the space ship.
	else if (SignificantCh == TheMathGame::GameChars::BIG_SPACE_SHIP_SIGN || SignificantCh == TheMathGame::GameChars::SMALL_SPACE_SHIP_SIGN)
	{
		TheGame->BombSpaceShip(SignificantCh);
		isStatic = false; //after bombing you aren't static.
		return true;
	}

	//if the char is one of the Flyers (COL_FLYER represent both of them), bomb me.
	else if (SignificantCh == TheMathGame::GameChars::COL_FLYER)
	{
		TheGame->BombCreature(pointList.front());
		return false;
	}

	//if the char is Numbers Eater, bomb him.
	else if (SignificantCh == TheMathGame::GameChars::NUMBERS_EATER)
	{
		TheGame->BombAllNumbersEatersNearArtifact(this, direction);
		return true;
	}

	else return false;
}

//this function delete the Artifact from the current destination (from screen).
void Artifact::DeleteMe()
{

	for (Point p : pointList)
	{
		p.draw(TheMathGame::GameChars::FREE_PASS);
	}
	TheGame->DeleteArtrifactFromBoard(*this);
}

//input: direction of the artifact, length to add (for case that the artifact is on spaceship).
//this function change all the points of the artifact to the destination points.
void Artifact::MoveArtifactAfterCrossing(int direction, int LengthToAdd)
{
	setTextColor(color);
	switch (direction) {
	case Direction::UP:
		for (Point& p : pointList)
		{
			//set y to:
			// resolution height end - ( the height of the artifact - 1 ) + the difference from the height beginning - LengthToAdd.
			p.set(p.getX(), Resolution::HEIGHT_END - (height - 1) + (p.getY() - Resolution::HEIGHT_BEGINNIG) - LengthToAdd);
			p.draw(ch);
		}
		break;
	case Direction::DOWN:
		for (Point& p : pointList)
		{
			//set y to:
			// resolution height beginning + ( the height of the artifact - 1 ) - the diffrenece from the end + length to add.                     
			p.set(p.getX(), Resolution::HEIGHT_BEGINNIG + (height - 1) - (Resolution::HEIGHT_END - p.getY()) + LengthToAdd);
			p.draw(ch);
		}
		break;
	case Direction::LEFT:
		for (Point& p : pointList)
		{
			//set x to:
			// resolution width end - ( the width of the artifact - 1 ) + the diffrenece from the beginning - length to add.  
			p.set(Resolution::WIDTH_END - (width-1) + (p.getX() - Resolution::WIDTH_BEGINNIG) - LengthToAdd, p.getY());
			p.draw(ch);
		}
		break;
	case Direction::RIGHT:
		for (Point& p : pointList)
		{
			//set x to:
			// resolution width beginning + ( the width of the artifact - 1 ) - the diffrenece from the end + length to add. 
			p.set(Resolution::WIDTH_BEGINNIG + (width - 1) - (Resolution::WIDTH_END - p.getX()) + LengthToAdd, p.getY());
			p.draw(ch);
		}
		break;
	default: break;
	}
	setTextColor(WHITE);
}

//input: direction and LengthToAdd.
//this function put the Artifact at the destination point.
void Artifact::CrossScreen(int direction, int LengthToAdd)
{
	TheGame->UpdateBoardWhenCrossScreen(direction, pointList, width,height, LengthToAdd); //update the board.
	DeleteMe(); //delete Artifact.
	MoveArtifactAfterCrossing(direction, LengthToAdd); //copy the Artifact.
}

//input: direction and LengthToAdd (default 0 for case that artifacat will cross with spaceship).
//return: true if the artifact moved to the new destination, flase otherwise,
bool Artifact::CrossingMovment(int direction, int LengthToAdd)
{
	char SignificantCh;
	//get the significat char at dest point
	SignificantCh = TheGame->GetSignificantCharAfterCrossingScreen(direction,*this,width,height,LengthToAdd);
	
	
	if (SignificantCh == TheMathGame::GameChars::FREE_PASS)
	{
		CrossScreen(direction, LengthToAdd); //Change position of artifact on screen and update board
		return true;
	}
	//if there is another artifact at destinsation, get this artifact, push him, cross and unificate with it.
	else if (SignificantCh == TheMathGame::GameChars::ARTIFACT)
	{
		Artifact& another = TheGame->getCrossedArtifact(direction, *this, width, height, LengthToAdd);
		PushAnotherAfterCrossing(another, direction);
		CrossScreen(direction, LengthToAdd); 
		TheGame->ArtifactsUnification(*this, another);
		return true;
	}
	//if this is a digit, take the number and cross the screen.
	else if (SignificantCh >= '0' && SignificantCh <= '9') 
	{
		TheGame->DeleteTheAllNumbers(direction, pointList); //delete the number
		CrossScreen(direction, LengthToAdd); 
		return true;
	}

	//if the char is one of the Flyers (COL_FLYER represent both of them), bomb me.
	else if (SignificantCh == TheMathGame::GameChars::COL_FLYER)
	{
		TheGame->BombCreature(pointList.front());
		return false;
	}

	//if the char is Numbers Eater, bomb him.
	else if (SignificantCh == TheMathGame::GameChars::NUMBERS_EATER)
	{
		TheGame->BombAllNumbersEatersNearArtifactAfterCrossing(this, direction);
		return true;
	}
	return false;
}

//this function calculate the width and the height of the artifact.
void Artifact::CalculateWidthAndHeight()
{
	//init the min and max of x and y to the resuolution limits.
	int MinX = Resolution::WIDTH_END, MaxX = Resolution::WIDTH_BEGINNIG, MinY = Resolution::HEIGHT_END, MaxY = Resolution::HEIGHT_BEGINNIG;
	
	//for all the points, update the min/max points.
	for (Point p : pointList)
	{
		if (p.getX() > MaxX) MaxX = p.getX();
		if (p.getX() < MinX) MinX = p.getX();
		if (p.getY() > MaxY) MaxY = p.getY();
		if (p.getY() < MinY) MinY = p.getY();
	}
	
	//calculate the width and the height.
	width = MaxX - MinX + 1;
	height = MaxY - MinY + 1;
}

//input: point
//return: true if this point is of this artifact, false otherwise.
bool Artifact::findPoint(const Point& point)
{
	// find this point in artifact.
	list <Point>::iterator iter = find(pointList.begin(), pointList.end(), point);
	
	//if this isn't the lase point res is true.
	bool res = (iter != pointList.end());

	if (res) return res;
	//if res is false, iter is the last point, return if this point is really the last point at pointList.
	else return (point == pointList.back());
}

bool Artifact::operator== (const Artifact& ar) {
	//if the artifacts has the same number of points
	if (pointList.size() == ar.pointList.size())
	{
		//check if all the points are equal.
		list<Point>::iterator Myitr = pointList.begin();
		list<Point> tempList = ar.getPointList();
		list<Point>::iterator Compitr = tempList.begin();
		for (size_t i = 0; i < pointList.size(); i++)
		{
			if ((Myitr->getX() == Compitr->getX()) && (Myitr->getY() == Compitr->getY()))
			{
				Myitr++;
				Compitr++;
			}
			else
				return false;
		}
		return true;
	}
	else return false;
}

//this function calculate the width and height above Leader.
void Artifact::CalculateWidthAndHeight(SpaceShip* s)
{
	int MinX = Resolution::WIDTH_END, MaxX = Resolution::WIDTH_BEGINNIG, MinY = Resolution::HEIGHT_END, MaxY = (s->GetStartPoint().getY() - 1);
	
	//because we want to calculate the height of the part that above the spaceship only.
	int MaxLegalY = (s->GetStartPoint().getY() - 1);
	for (Point p : pointList)
	{
		if (p.getX() > MaxX) MaxX = p.getX();
		if (p.getX() < MinX) MinX = p.getX();
		if (p.getY() <= MaxLegalY && p.getY() > MaxY) MaxY = p.getY();
		if (p.getY() <= MaxLegalY && (p.getY() < MinY)) MinY = p.getY();
	}
	widthAboveLeader = MaxX - MinX + 1;
	heightAboveLeader = MaxY - MinY + 1;
}

//input: another artifact, direction.
//this function push the other artifact by *this size.
void Artifact::PushAnotherAfterCrossing(Artifact& another, int direction)
{
	//if the direction is up or down, push by the artifact height - 1.
	//(-1 for the already step at the movment of the other artifact).
	another.setDirection(direction);
	if (direction == Direction::UP || direction == Direction::DOWN)
	{
		another.move(height);
	}
	//else the direction is left or right, push by width - 1.
	//(-1 for the already step at the movment of the other artifact).
	else another.move(width-1);
}

bool Artifact::CheckIfThisPointBelongToMe(const Point& p)
{
	return findPoint(p);
}

Creature* Artifact::CreateCopyOfMe()
{
	Creature* res;
	try { res = new Artifact(*this); }
	catch (bad_alloc& ba)
	{
		TheGame->DeleteAllMemoryAllocations();
		cerr << "bad_alloc caught: " << ba.what() << '\n';
		exit(1);
	}
	return res;
}
