#include "SpaceShip.h"
#include "TheMathGame.h"

void SpaceShip::InitSpaceShip(bool WithPrinting)
{
	direction = Direction::STAY;
	TheGame->CalculateWidthAndHeightOfSpaceShip(this);
	Dragged = nullptr; //because CheckForArtifacts is using Dragged, so we don't want garbage in this pointer.
	Dragged = TheGame->CheckForArtifacts(Start, width, height, this);
	if (WithPrinting) _print();
}

//print the SpaceShip.
void SpaceShip::_print()
{
	setTextColor(color);
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			Point(Start.getX()+j,Start.getY()+i).draw(ch);
		}
	}
	setTextColor(WHITE);
}

//setArrowKeys
void SpaceShip::setArrowKeys(const char* keys)
{
	for (int i = 0; i < KEYS_NUM; i++)
		arrowKeys[i] = keys[i];
}

//input: key (char)
//return: if the key represents a direction return his value, else reurn -1.
int SpaceShip::GetDirection(const list<char>& keyHits)
{
	for (char key : keyHits) {
		for (int i = 0; i < KEYS_NUM; ++i) {
			if (key == arrowKeys[i]) {
				return i;
			}
		}
	}
	return -1;
}

//this function moving the spaceship.
void SpaceShip::move()
{	
	Dragged = TheGame->CheckForArtifacts(Start, width, height, this); //always check before movment if you can take an artifact.

	(Dragged != nullptr) ? MoveWithArtifact() : MoveWithoutArtifact();
}

//This function move the spaceship with her artifact by direction.
void SpaceShip::MoveWithArtifact()
{
	switch (direction) {
	case Direction::UP:
		MoveUpWithArtifact();
		break;
	case Direction::DOWN:
		MoveDownWithArtifact();
		break;
	case Direction::LEFT:
		MoveLeftWithArtifact();
		break;
	case Direction::RIGHT:
		MoveRightWithArtifact();
		break;
	default: break;
	}
}

/////////////////////move functions//////////////////////

void SpaceShip::MoveUpWithArtifact()
{
	Point SpaceShipDest; //for new destination if needed.
	int originalWidth = Dragged->getWidth(), originalHeight = Dragged->getHeight();

	//if the spaceship can push this artifact by her own volume.
	if (TheGame->CheckIfCanIPushArtifact(this, Dragged, direction))
	{
		//if the movment is crossing the resolution
		if (Dragged->CheckIfNeedToCross(direction))
		{
			//we need to set the height and size of the part above the leader.
			Dragged->SetHeight(Dragged->getHeightAboveLeader());
			Dragged->SetWidth(Dragged->getWidthAboveLeader());

			//insert to destination points the right points and cross the screen.
			SpaceShipDest.set(Start.getX(), Resolution::HEIGHT_END - height + 1);
			CrossingMovmentWithArtifact(SpaceShipDest, height);

			//return to original width and height.
			Dragged->SetHeight(originalHeight);
			Dragged->SetWidth(originalWidth);
		}
		else //if this is regular movment
		{
			RegularMovmentWithArtifact();
		}
	}
}

void SpaceShip::MoveDownWithArtifact()
{
	Point SpaceShipDest; //for new destination if needed.
	int originalWidth = Dragged->getWidth(), originalHeight = Dragged->getHeight(); //for up and down need to cross cases.

	//if the spaceship movment is crossing the resolution
	if (Start.getY() + height > Resolution::HEIGHT_END)
	{
		//we need to set the height and size of the part above the leader.
		Dragged->SetHeight(Dragged->getHeightAboveLeader());
		Dragged->SetWidth(Dragged->getWidthAboveLeader());

		//insert to destination points the right points and cross the screen.
		SpaceShipDest.set(Start.getX(), Resolution::HEIGHT_BEGINNIG + Dragged->getHeight());
		CrossingMovmentWithArtifact(SpaceShipDest, height);

		//return for original width and height.
		Dragged->SetHeight(originalHeight);
		Dragged->SetWidth(originalWidth);
	}
	//if the artifact movment is crossing the resolution
	else if (Dragged->CheckIfNeedToCross(direction))
	{
		SpaceShipDest.set(Start.getX(), Resolution::HEIGHT_BEGINNIG + (Resolution::HEIGHT_END - (Start.getY() + height - 1)));
		CrossingMovmentWithArtifact(SpaceShipDest, 0);
	}
	else //if this is regular movment
	{
		RegularMovmentWithArtifact();
	}
}

void SpaceShip::MoveLeftWithArtifact()
{
	Point SpaceShipDest; //for new destination if needed.
	int diffForSpaceShip, diffForArtifact; //for the diffrences for the cross (if needed).

	//if the spaceship need to cross.
	if ((Start.getX() - 1) < Resolution::WIDTH_BEGINNIG)
	{
		//calculate the diffrences and zero them if they are negatives.
		diffForSpaceShip = PosOfDragged + Dragged->getWidth() - width;
		diffForArtifact = PosOfDragged;
		if (diffForSpaceShip < 0) diffForSpaceShip = 0;
		if (diffForArtifact < 0) diffForArtifact = 0;

		//insert to destination points the right points and cross the screen.
		SpaceShipDest.set(Resolution::WIDTH_END - width - diffForSpaceShip + 1, Start.getY());
		CrossingMovmentWithArtifact(SpaceShipDest, diffForArtifact);
	}
	else if (Dragged->CheckIfNeedToCross(direction)) //if the articat need to cross.
	{
		//calculate the diffrences and zero them if they are negatives.
		diffForSpaceShip = (Dragged->getWidth() - 1) - (Start.getX() + width - 1);
		diffForArtifact = (Start.getX() + width - 1) - (Start.getX() + PosOfDragged + Dragged->getWidth() - 1);
		if (diffForSpaceShip < 0) diffForSpaceShip = 0;
		if (diffForArtifact < 0) diffForArtifact = 0;

		//insert to destination points the right points and cross the screen.
		SpaceShipDest.set(Resolution::WIDTH_END - width - diffForSpaceShip + 1, Start.getY());
		CrossingMovmentWithArtifact(SpaceShipDest, diffForArtifact);
	}
	else //if this is regular movment.
	{
		RegularMovmentWithArtifact();
	}
}

void SpaceShip::MoveRightWithArtifact()
{
	Point SpaceShipDest; //for new destination if needed.
	int diffForSpaceShip, diffForArtifact; //for the diffrences for the cross (if needed).

	//if the movment is crossing the resolution
	if ((Start.getX() + width) > Resolution::WIDTH_END)
	{
		//calculate the diffrences and zero them if they are negatives.
		diffForSpaceShip = Start.getX() - (Start.getX() + PosOfDragged);
		diffForArtifact = PosOfDragged;
		if (diffForSpaceShip < 0) diffForSpaceShip = 0;
		if (diffForArtifact < 0) diffForArtifact *= -1;

		//insert to destination points the right points and cross the screen.
		SpaceShipDest.set(Resolution::WIDTH_BEGINNIG + diffForSpaceShip, Start.getY());
		CrossingMovmentWithArtifact(SpaceShipDest, diffForArtifact);
	}
	else if (Dragged->CheckIfNeedToCross(direction))
	{
		//calculate the diffrences and zero them if they are negatives.
		diffForSpaceShip = Start.getX() - (Start.getX() + PosOfDragged);
		diffForArtifact = PosOfDragged;
		if (diffForSpaceShip < 0) diffForSpaceShip = 0;
		if (diffForArtifact < 0) diffForArtifact = 0;

		//insert to destination points the right points and cross the screen.
		SpaceShipDest.set(Resolution::WIDTH_BEGINNIG + diffForSpaceShip, Start.getY());
		CrossingMovmentWithArtifact(SpaceShipDest, diffForArtifact);
	}
	else //if this is regular movment
	{
		RegularMovmentWithArtifact();
	}
}

/////////////////////move functions//////////////////////

//This function move the spaceship by direction.
void SpaceShip::MoveWithoutArtifact()
{
	Point dest; //for new destination if needed.

	switch (direction) {
	case Direction::UP:
		if (((Start.getY() + height - 1) - height) < Resolution::HEIGHT_BEGINNIG) //if the movment is crossing the resolution
		{
			dest.set(Start.getX(), Resolution::HEIGHT_END + 1 - height); //insert to dest the right point.
			CrossingMovment(dest);
		}
		else //if this is regular movment
		{
			dest.set(Start.getX(), Start.getY() - 1); //for getting Num if needed.
			RegularMovment(dest);
		}
		break;
	case Direction::DOWN:
		if ((Start.getY() + height) > Resolution::HEIGHT_END) //if the movment is illegal
		{
			dest.set(Start.getX(), Resolution::HEIGHT_BEGINNIG); //insert to dest the right point.
			CrossingMovment(dest);
		}
		else //if this is regular movment
		{
			dest.set(Start.getX(), Start.getY() + height); //for getting Num if needed.
			RegularMovment(dest);
		}
		break;
	case Direction::LEFT:
		if (((Start.getX() + width - 1) - width) < Resolution::WIDTH_BEGINNIG) //if the movment is illegal
		{
			dest.set(Resolution::WIDTH_END - width + 1, Start.getY()); //insert to dest the right point.
			CrossingMovment(dest);
		}
		else //if this is regular movment.
		{
			dest.set(Start.getX() - 1, Start.getY()); //for getting Num if needed.
			RegularMovment(dest);
		}
		break;
	case Direction::RIGHT:
		if ((Start.getX() + width) > Resolution::WIDTH_END) //if the movment is illegal
		{
			dest.set(Resolution::WIDTH_BEGINNIG, Start.getY()); //insert to dest the right point.
			CrossingMovment(dest);
		}
		else //if this is regular movment
		{
			dest.set(Start.getX() + width, Start.getY()); //for getting Num if needed.
			RegularMovment(dest);
		}
		break;
	default: break;
	}
}

//input: destination point.
//this function get the significant char at dest point and crossing the screen accordingly.
void SpaceShip::CrossingMovment(Point dest)
{
	char SignificantCh; //for the significant char of the place that the spaceship will move to.

	SignificantCh = TheGame->GetSignificantCharAfterCrossingScreen(dest, width, height); //get the significat char at dest point
	if (SignificantCh == TheMathGame::GameChars::FREE_PASS)
	{
		CrossScreen(dest); //Change position of spaceship on screen and update board
	}
	// if the char is a digit, take the all numbers, try to solve the equation and cross the screen.
	else if ((SignificantCh >= '0' && SignificantCh <= '9'))
	{
		TheGame->getNumAfterRotateOrCrossScreen(dest, width, height); //get the number
		CrossScreen(dest); //Change position of spaceship on screen and update board
	}
	else if (SignificantCh == TheMathGame::GameChars::ARTIFACT && TheGame->PushArtifactAfterCrossing(this,direction,width,height,0))
	{
		CrossScreen(dest);
	}
	else if (SignificantCh == TheMathGame::GameChars::COL_FLYER)
	{
		TheGame->BombSpaceShip(ch);
	}
	else if (SignificantCh == TheMathGame::GameChars::NUMBERS_EATER)
	{
		TheGame->BombAllNumbersEeaterByDirectionAfterCrossing(this, dest, 0, direction);
	}
}

//input: destination points of spaceship and artifact.
//this function get the significant chars at dest points and crossing the screen accordingly.
void SpaceShip::CrossingMovmentWithArtifact(Point spaceshipDest, int LengthToAdd)
{
	char SpaceShipCH, ArtifactCH; //for the significant char of the place that the spaceship and artifact will move to.

	//get significants chars.
	SpaceShipCH = TheGame->GetSignificantCharAfterCrossingScreen(spaceshipDest, width, height);
	ArtifactCH = TheGame->GetSignificantCharAfterCrossingScreen(direction, *Dragged, Dragged->getWidth(), Dragged->getHeight(), LengthToAdd);

	if (SpaceShipCH == TheMathGame::GameChars::FREE_PASS && ArtifactCH == TheMathGame::GameChars::FREE_PASS)
	{
		Dragged->CrossScreen(direction, LengthToAdd); //Change position of artifact on screen and update board
		CrossScreen(spaceshipDest); //Change position of spaceship on screen and update board
	}
	//if the spaceship significat char is another artifact, and the artifact can move.
	else if (SpaceShipCH == TheMathGame::GameChars::ARTIFACT && (ArtifactCH == TheMathGame::GameChars::FREE_PASS || (ArtifactCH >= '0' && ArtifactCH <= '9')))
	{
		int PosDiff, difForSpaceShip, diffForArtifact; //for the diffrences for the pushing.
		//calculate the diffrences and send the positive one.
		difForSpaceShip = (Dragged->getWidth() - 1) - (Start.getX() + width - 1);
		diffForArtifact = (Start.getX() + width - 1) - (Start.getX() + PosOfDragged + Dragged->getWidth() - 1);
		(difForSpaceShip < 0)  ? PosDiff = diffForArtifact : PosDiff = difForSpaceShip;

		//if the spaceship can push the artifact.
		if (TheGame->PushArtifactAfterCrossing(this, direction, width, height, PosDiff))
		{
			if (ArtifactCH == TheMathGame::GameChars::FREE_PASS)
			{
				Dragged->CrossScreen(direction, LengthToAdd); //Change position of artifact on screen and update board
				CrossScreen(spaceshipDest); //Change position of spaceship on screen and update board
			}
			else
			{
				if (getNumWithArtifactAfterCrossing(ArtifactCH, SpaceShipCH, spaceshipDest, LengthToAdd))
				{
					Dragged->CrossScreen(direction, LengthToAdd); //Change position of artifact on screen and update board
					CrossScreen(spaceshipDest); //Change position of spaceship on screen and update board
				}
			}
		}
	}
	// if one of the chars is a digit, take the all numbers, try to solve the equation and cross the screen.
	else if ((ArtifactCH >= '0' && ArtifactCH <= '9') || (SpaceShipCH >= '0' && SpaceShipCH <= '9'))
	{
		if (getNumWithArtifactAfterCrossing(ArtifactCH, SpaceShipCH, spaceshipDest, LengthToAdd))
		{
			Dragged->CrossScreen(direction, LengthToAdd); //Change position of artifact on screen and update board
			CrossScreen(spaceshipDest); //Change position of spaceship on screen and update board
		}
	}
	else if (SpaceShipCH == TheMathGame::GameChars::COL_FLYER || ArtifactCH == TheMathGame::GameChars::COL_FLYER)
	{
		TheGame->BombSpaceShip(ch);
	}
	else if (SpaceShipCH == TheMathGame::GameChars::NUMBERS_EATER || ArtifactCH == TheMathGame::GameChars::NUMBERS_EATER)
	{
		TheGame->BombAllNumbersEeaterByDirectionAfterCrossing(this, spaceshipDest, LengthToAdd, direction);
	}
}

//input: destination point.
//this function get the significant char at direction and moving accordingly.
void SpaceShip::RegularMovment(Point dest)
{
	char SignificantCh; //for the significant char of the place that the spaceship will move to.

	SignificantCh = TheGame->GetSignificantChar(direction, Start, width, height);
	if (SignificantCh == TheMathGame::GameChars::FREE_PASS)
	{
		TheGame->UpdateBoard(direction, Start, width, height);
		MoveByDirection();
	}
	else if (SignificantCh >= '0' && SignificantCh <= '9') //if the char is a digit, take the all numbers, update the board and move.
	{
		TheGame->getNum(dest, direction, width, height);
		TheGame->UpdateBoard(direction, Start, width, height);
		MoveByDirection();
	}
	//if the char is an artifact, and the spaceship can push it.
	else if (SignificantCh == TheMathGame::GameChars::ARTIFACT && TheGame->PushArtifact(this, direction))
	{
		if (TheGame->CheckIfThereIsNumber(direction,Start,width,height)) TheGame->getNum(dest, direction, width, height);
		TheGame->UpdateBoard(direction, Start, width, height);
		MoveByDirection();
	}
	else if (SignificantCh == TheMathGame::GameChars::COL_FLYER)
	{
		TheGame->BombSpaceShip(ch);
	}
	else if (SignificantCh == TheMathGame::GameChars::NUMBERS_EATER)
	{
		TheGame->BombAllNumbersEeaterByDirection(this, direction);
	}
}

//input: destination points.
//this function get the significant chars at direction and moving accordingly.
void SpaceShip::RegularMovmentWithArtifact()
{
	char SpaceShipCH, ArtifactCH; //for the significant char of the place that the spaceship or artifact will move to.

	//get significant chars
	SpaceShipCH = TheGame->GetSignificantChar(direction, Start, width, height);
	ArtifactCH = TheGame->GetSignificantChar(direction, *(Dragged));

	//if the significant char if the spaceship is artifact, check if this is another artifact.
	if (SpaceShipCH == TheMathGame::GameChars::ARTIFACT)
	{
		Artifact tempAr = TheGame->getNearestArtifact(*this);
		//if this is the same artifact, change to free_pass.
		if (tempAr == *Dragged) SpaceShipCH = TheMathGame::GameChars::FREE_PASS;
	}
	
	//if the artifact siginifcant char is the spaceship sign, ignore it.
	if (ArtifactCH == ch) ArtifactCH = TheMathGame::GameChars::FREE_PASS;

	if (SpaceShipCH == TheMathGame::GameChars::FREE_PASS && ArtifactCH == TheMathGame::GameChars::FREE_PASS)
	{
		UpdateBoardAndMoveWithArtifact();
	}
	//if the spaceship significat char is another artifact, and the artifact can move.
	else if (SpaceShipCH == TheMathGame::GameChars::ARTIFACT && (ArtifactCH == TheMathGame::GameChars::FREE_PASS || (ArtifactCH >= '0' && ArtifactCH <= '9')))
	{
		//if the spaceship can push the artifact.
		if (TheGame->PushArtifact(this, direction))
		{
			if (ArtifactCH == TheMathGame::GameChars::FREE_PASS)
			{
				UpdateBoardAndMoveWithArtifact();
			}
			//there is number, update board and move just if we didn't start the level again.
			else if (getNumWithArtifact(ArtifactCH, SpaceShipCH))
			{
				UpdateBoardAndMoveWithArtifact();
			}
		}
	}
	//if one of the chars is a digit, take all the numbers, try to solve the equation, update the board and move.
	else if ((ArtifactCH >= '0' && ArtifactCH <= '9') || (SpaceShipCH >= '0' && SpaceShipCH <= '9'))
	{
		//update board and move just if we didn't start the level again.
		if (getNumWithArtifact(ArtifactCH, SpaceShipCH))
		{
			UpdateBoardAndMoveWithArtifact();
		}
	}
	else if (SpaceShipCH == TheMathGame::GameChars::COL_FLYER || ArtifactCH == TheMathGame::GameChars::COL_FLYER)
	{
		TheGame->BombSpaceShip(ch);
	}
	else if (SpaceShipCH == TheMathGame::GameChars::NUMBERS_EATER || ArtifactCH == TheMathGame::GameChars::NUMBERS_EATER)
	{
		TheGame->BombAllNumbersEeaterByDirection(this,direction);
	}
}

//this function run the currently movment function by dircetion.
void SpaceShip::MoveByDirection()
{
	switch (direction)
	{
	case Direction::UP:
		moveup();
		break;
	case Direction::DOWN:
		movedown();
		break;
	case Direction::LEFT:
		moveleft();
		break;
	case Direction::RIGHT:
		moveright();
		break;
	default: break;
	}
}

//this function run the currently movment of the spaceship and the artifact functions by dircetion.
void SpaceShip::MoveByDirectionWithArtifact(bool SpaceShipMoveFirst)
{
	switch (direction)
	{
	case Direction::UP: //in this case the artifact has to move first.
		Dragged->moveup();
		moveup();
		break;
	case Direction::DOWN:
		if (SpaceShipMoveFirst)
		{
			movedown();
			Dragged->movedown();
		}
		else
		{
			Dragged->movedown();
			movedown();
		}
		break;
	case Direction::LEFT:
		if (SpaceShipMoveFirst)
		{
			moveleft();
			Dragged->moveleft();
		}
		else
		{
			Dragged->moveleft();
			moveleft();
		}
		break;
	case Direction::RIGHT:
		if (SpaceShipMoveFirst)
		{
			moveright();
			Dragged->moveright();
		}
		else
		{
			Dragged->moveright();
			moveright();
		}
		break;
	default: break;
	}
}

void SpaceShip::moveup()
{
	for (int i = 0; i < width; i++) //delete the last row.
	{
		Point(Start.getX() + i , Start.getY() + height -1 ).draw(SPACE);
	}
	setTextColor(color); //set color for draw.
	Start.move(Direction::UP); //move the start point up.
	for (int i = 0; i < width; i++) //draw ch in the first row.
	{
		Point(Start.getX() + i, Start.getY() ).draw(ch);
	}
	setTextColor(WHITE); //return the color to white.
}
void SpaceShip::movedown()
{
	for (int i = 0; i < width; i++) //delete the first row.
	{
		Point(Start.getX() + i, Start.getY()).draw(SPACE);
	}
	setTextColor(color); //set color for draw.
	Start.move(Direction::DOWN); //move the start point down
	for (int i = 0; i < width; i++) //draw ch in the last row.
	{
		Point(Start.getX() + i, Start.getY() + height -1).draw(ch);
	}
	setTextColor(WHITE); //return color to white.
}
void SpaceShip::moveleft()
{
	for (int i = 0; i < height; i++) //delete the last column.
	{
		Point(Start.getX() + width - 1, Start.getY() + i).draw(SPACE);
	}
	setTextColor(color); //set color for draw.
	Start.move(Direction::LEFT); //move the start point left.
	for (int i = 0; i < height; i++) //draw ch in the first column.
	{
		Point(Start.getX(), Start.getY()+i).draw(ch);
	}
	setTextColor(WHITE); //return color to white.
}
void SpaceShip::moveright()
{
	for (int i = 0; i < height; i++) //delete the first column.
	{
		Point(Start.getX(), Start.getY()+i).draw(SPACE);
	}
	setTextColor(color); //set color for draw.
	Start.move(Direction::RIGHT);//move the start point right.
	for (int i = 0; i < height; i++) //draw ch in the last column.
	{
		Point(Start.getX() + width - 1, Start.getY() + i).draw(ch);
	}
	setTextColor(WHITE); //return column to white.
}

//input: destination point.
//this function put the spaceship at the destination point.
void SpaceShip::CrossScreen(Point dest)
{
	TheGame->UpdateBoardWhenCrossScreen(Start, dest, width, height); //update the board.
	DeleteSpaceShip(); //delete spaceship.
	CopySpaceShipToNewPoint(dest); //copy the spaceship.
}

//this function delete the spaceship from the current destination (from screen).
void SpaceShip::DeleteSpaceShip()
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			Point(Start.getX()+j, Start.getY()+i).draw(TheMathGame::GameChars::FREE_PASS);
		}
	}
}

//input: new destination for the spaceship.
//this function delete the spaceship from her current destination, and copy her to the new destination.
void SpaceShip::CopySpaceShipToNewPoint(Point dest)
{
	setTextColor(color); //set TextColor for draw.
	for (int i = 0; i < height; i++) //set each Point of the spaceship at the new destination, and draw it.
	{
		for (int j = 0; j < width; j++)
		{
			Point(dest.getX() + j, dest.getY() + i).draw(ch);
		}
	}
	SetStartPoint(dest);
	setTextColor(WHITE);
}

//input: destination point.
//this function rotate the spaceship to the destination point.
void SpaceShip::RotateHelper(SoundFreq SoundFreqRotate)
{
	if (Dragged == nullptr) //if there isn't dragged artifact.
	{
		char SignificantCh;
		// if width > height and the rotate is leagl.
		if (width > height && ((Start.getY() + (width / 2)) <= Resolution::HEIGHT_END) && ((Start.getY() - (width / 2)) >= Resolution::HEIGHT_BEGINNIG))
		{
			SignificantCh = TheGame->GetSignificantChar(Direction::ROTATE, Start, width, height);
			if (SignificantCh == TheMathGame::GameChars::FREE_PASS)
			{
				TheGame->UpdateBoard(Direction::ROTATE, Start, width, height);
				Rotate(Point(Start.getX() + 1, (Start.getY() - 1)), SoundFreqRotate);
			}
			else if (SignificantCh >= '0' && SignificantCh <= '9') //if it's a digit.
			{
				//the height and the width are swaped for checking the number of the rotate.
				TheGame->getNumAfterRotateOrCrossScreen(Point(Start.getX() + 1, (Start.getY() - 1)), height, width);
				TheGame->UpdateBoard(Direction::ROTATE, Start, width, height);
				Rotate(Point(Start.getX() + 1, (Start.getY() - 1)), SoundFreqRotate);
			}
		}
		//else if width < height and the rotate is legal.
		else if (width < height && ((Start.getX() + (height / 2)) <= Resolution::WIDTH_END) && ((Start.getX() - (height / 2)) >= Resolution::WIDTH_BEGINNIG))
		{
			SignificantCh = TheGame->GetSignificantChar(Direction::ROTATE, Start, width, height);
			if (SignificantCh == TheMathGame::GameChars::FREE_PASS)
			{
				TheGame->UpdateBoard(Direction::ROTATE, Start, width, height);
				Rotate(Point(Start.getX() - 1, (Start.getY() + 1)), SoundFreqRotate);
			}
			else if (SignificantCh >= '0' && SignificantCh <= '9') //if it's digit.
			{
				//the height and the width are swaped for checking the number of the rotate.
				TheGame->getNumAfterRotateOrCrossScreen(Point(Start.getX() - 1, (Start.getY() + 1)), height, width);
				TheGame->UpdateBoard(Direction::ROTATE, Start, width, height);
				Rotate(Point(Start.getX() - 1, (Start.getY() + 1)), SoundFreqRotate);
			}
		}
	}
}

//input: Destination point.
//this function rotate the spaceship to the dest point.
void SpaceShip::Rotate(Point dest, SoundFreq SoundFreqRotate)
{
	DeleteSpaceShip();
	swap(width, height);
	CopySpaceShipToNewPoint(dest);
	Sleep(TheGame->GetSleepTime());
}

//input: The significants char of spaceship and artifact.
//this function get the numbers at the destionation points of the tow objects.
bool SpaceShip::getNumWithArtifact(char ArtifactCH, char SpaceShipCH)
{
	bool res = true;

	//for efficiency we are calculate the destination points and getting the numbers just in case that there is a number at the object destinition point.
	Point SpaceShipDest;
	if ((ArtifactCH >= '0' && ArtifactCH <= '9') && (SpaceShipCH >= '0' && SpaceShipCH <= '9'))
	{
		SpaceShipDest = TheGame->CalculateDestByDirection(Start, direction, width, height);
		res &= TheGame->getNum(direction, Dragged->getPointList());
		res &= TheGame->getNum(SpaceShipDest, direction, width, height);
	}
	else if ((ArtifactCH >= '0' && ArtifactCH <= '9'))
	{
		res &= TheGame->getNum(direction, Dragged->getPointList()); //get the number
	}
	else
	{
		SpaceShipDest = TheGame->CalculateDestByDirection(Start, direction, width, height);
		res &= TheGame->getNum(SpaceShipDest, direction, width, height);
	}
	return res;
}

//input: The significants char of spaceship and artifact and their destination points.
//this function get the numbers at the destionation points of the tow objects.
bool SpaceShip::getNumWithArtifactAfterCrossing(char ArtifactCH, char SpaceShipCH, Point spaceshipDest, int LengthToAdd)
{

	bool res = true;
	//for efficiency we are getting the numbers just in case that there is a number at the object destinition point.

	if ((ArtifactCH >= '0' && ArtifactCH <= '9') && (SpaceShipCH >= '0' && SpaceShipCH <= '9'))
	{
		res &= TheGame->getNumAfterRotateOrCrossScreen(direction, Dragged->getPointList(), LengthToAdd, Dragged->getWidth(), Dragged->getHeight());
		res &= TheGame->getNumAfterRotateOrCrossScreen(spaceshipDest, width, height);
	}
	else if ((ArtifactCH >= '0' && ArtifactCH <= '9'))
	{
		res &= TheGame->getNumAfterRotateOrCrossScreen(direction, Dragged->getPointList(), LengthToAdd, Dragged->getWidth(), Dragged->getHeight());
	}
	else
	{
		res &= TheGame->getNumAfterRotateOrCrossScreen(spaceshipDest, width, height);
	}
	return res;
}

void SpaceShip::swap(int& a, int& b)
{
	int temp;
	temp = a;
	a = b;
	b = temp;
}

//this function update the board and make the moves of the spaceship and the dragged artifact.
void SpaceShip::UpdateBoardAndMoveWithArtifact()
{
	bool SpaceShipMoveFirst = true; //we need to know if the spaceship need to move first.

	//if the significant char of the space ship is artifact, we know that this is the dragged (because we can move if we are here).
	//we are in one of these situations:
	//             *****       *****
	//             ##***   or  ***##
	//             ##**         **##
	//             ##*      	 *##
	//direction:   ->             <-
	// so the artifact need to be update and move first.
	if (TheGame->GetSignificantChar(direction,Start,width,height) == TheMathGame::GameChars::ARTIFACT)
		SpaceShipMoveFirst = false;

	if (direction == Direction::UP) //for this case the update of the artifact need to be first.
	{
		TheGame->UpdateBoard(direction, Dragged->getPointList());
		TheGame->UpdateBoard(direction, Start, width, height);
	}
	else //in this case is by the case
	{
		if (SpaceShipMoveFirst)
		{
			TheGame->UpdateBoard(direction, Start, width, height);
			TheGame->UpdateBoard(direction, Dragged->getPointList());
		}
		else
		{
			TheGame->UpdateBoard(direction, Dragged->getPointList());
			TheGame->UpdateBoard(direction, Start, width, height);
		}
	}
	MoveByDirectionWithArtifact(SpaceShipMoveFirst);
	
}
