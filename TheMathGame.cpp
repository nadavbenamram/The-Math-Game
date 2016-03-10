#include "TheMathGame.h"
#include "io_utils.h"

using namespace std;

//this function send the name of the current level Board's file name to setBoard.
void TheMathGame::setCurrentLevelBoard()
{
	int i = 0; //for index of the list.
	list<TheMathGame::NamesByScreenID>::iterator it = BoardFilesName.begin();
	while ((i < currentLevel) && it != BoardFilesName.end())
	{
		it++;
		i++;
	}
	setBoard(it->name);
}

//input: file name.
//this function set the board from this file.
void TheMathGame::setBoard(string fileName)
{
	bool isSPPfile = false;
	//if this board is valid, read it.
	if (TheBoardChecker(fileName, isSPPfile, false))
	{
		ifstream fileReader;
		fileReader.open(fileName, ios::in);
		readBoardFromFile(fileReader, isSPPfile);
		fileReader.close();
		SetCreaturesList(); //set Artifacts list.
	}
	//else notice and move to the next level.
	else
	{
		clear_screen();
		cout << "THIS BOARD IS INVALID, YOU ARE REDIRECTING TO THE NEXT ONE\n";
		SleepAndPrintCounterSeconds(MESSAGE_SLEEP_DURATION);
		
		// for moving to the next level.
		currentLevel++; 

	//	if (!hasNextLevel()) //if there isn't any more levels
			//currentLevel = 0;

		setCurrentLevelBoard();
	}
}

//input: file to read from, is spp file.
//this function read the board from this file.
void TheMathGame::readBoardFromFile(ifstream &fileToRead, bool isSPPfile)
{
	string lineRead;

	//Get from the file and set the screen id
	getline(fileToRead, lineRead);
	size_t ScreenIDNumber;
	sscanf_s(lineRead.c_str(), "ScreenID=%d", &ScreenIDNumber);
	Gs.setScreenNumber(ScreenIDNumber);

	//if sppfile set clock iterations:
	if (isSPPfile)
	{
		getline(fileToRead, lineRead);
		size_t cl;
		sscanf_s(lineRead.c_str(), "ClockIterations=%d", &cl);
		Gs.setClockCycles(cl);
	}

	//Get from the file and set the exercise
	getline(fileToRead, lineRead);
	string Exercise(MAX_LINE_LEN,' ');
	
	sscanf_s(lineRead.c_str(), "Exercise=%s", &Exercise[0], MAX_LINE_LEN);
	Ex.setExercise(Exercise.c_str());
	
	//if sppfile set lives:
	if (isSPPfile)
	{
		getline(fileToRead, lineRead);
		size_t lives;
		sscanf_s(lineRead.c_str(), "Life=%d", &lives);
		Gs.setLives(lives);
	}

	//Get board from file and set it
	GetBoardFromFile(fileToRead, originalBoard);
	
	//if sppfile getKeyList
	if (isSPPfile)
	{
		getKeysListFromFile(fileToRead);
	}

}

// copy original board to actual board and build all the relevant objects
// this shall be called any time we Start the level
// print the board only if withPrinting is true.
void TheMathGame::initBoard(bool WithPrinting)
{
	clear_screen();
	setTextColor(Color::WHITE);
	bool isBigSpaceShipSet = 0, isSmallSpaceShipSet = 0;

	for (int i = 0; i < ROWS; ++i) {
		for (int j = 0; j < COLS; ++j) {
			//copy the char to board.
			board[i][j] = originalBoard[i][j];
			//handle Objects.
			handleObjectCreationFromBoard(i, j, isBigSpaceShipSet, isSmallSpaceShipSet, WithPrinting);
		}
		board[i][COLS] = '\0';
	}

	if (WithPrinting) Ex.PrintExercise();
	if (WithPrinting) Gs.Print();
	initCreaturesList(WithPrinting);
	bigSpaceShip.InitSpaceShip(WithPrinting);
	smallSpaceShip.InitSpaceShip(WithPrinting);
}

//input: dest board.
//this function copy the original board to dest board.
void TheMathGame::copyBoard(char destBoard[][COLS + 1])
{
	for (int i = 0; i < ROWS; ++i) {
		for (int j = 0; j < COLS; ++j) {
			//copy the char to board.
			destBoard[i][j] = originalBoard[i][j];
		}
		destBoard[i][COLS] = '\0';
	}
}

void TheMathGame::handleObjectCreationFromBoard(int row, int col, bool& isBigSpaceShipSet, bool& isSmallSpaceShipSet, bool WithPrinting)
{
	char ch = board[row][col];
	if (ch == GameChars::BIG_SPACE_SHIP_SIGN)	handleBigSpaceShip(isBigSpaceShipSet, col, row);
	else if (ch == GameChars::SMALL_SPACE_SHIP_SIGN)		handleSmallSpaceShip(isSmallSpaceShipSet, col, row);
	else if (ch == GameChars::EXERCISE)		handleExercise(col, row);
	else if (ch == GameChars::GAME_STATE)	handleGameState(col, row);
	else if (ch >= '0' && ch <= '9' && WithPrinting)	handleDigit(col, row);
	else if (WithPrinting)	handleAllTheRest(col, row);
}
void TheMathGame::handleBigSpaceShip(bool& isBigSpaceShipSet, int col, int row)
{
	if (!isBigSpaceShipSet)
	{
		bigSpaceShip.SetSpaceShip(Point(col, row), '#', this, Color::LIGHTGREEN);
		bigSpaceShip.setArrowKeys("wxadzs");

		isBigSpaceShipSet = true;
	}
}
void TheMathGame::handleSmallSpaceShip(bool& isSmallSpaceShipSet, int col, int row)
{
	if (!isSmallSpaceShipSet)
	{
		smallSpaceShip.SetSpaceShip(Point(col, row), '@', this, Color::LIGHTCYAN);
		smallSpaceShip.setArrowKeys("imjlnk");

		isSmallSpaceShipSet = true;
	}
}
void TheMathGame::handleExercise(int col, int row)
{
	Ex.setStart(Point(col, row));
	Ex.setGame(this);
}
void TheMathGame::handleGameState(int col, int row)
{
	Gs.Set(Point(col, row), this);
}
void TheMathGame::handleDigit(int col, int row)
{
	setTextColor(Color::LIGHTMAGENTA);
	
	digitsList.push_back(Point(col, row));

	//print the digit
	gotoxy(col, row);
	cout << originalBoard[row][col];
	cout.flush();

	setTextColor(Color::WHITE);
}
void TheMathGame::handleAllTheRest(int col, int row)
{
	if (originalBoard[row][col] != GameChars::ARTIFACT && originalBoard[row][col] != GameChars::ROW_FLYER && originalBoard[row][col] != GameChars::COL_FLYER && originalBoard[row][col] != GameChars::NUMBERS_EATER)
	{
		//print the char.
		gotoxy(col, row);
		cout << originalBoard[row][col];
		cout.flush();
	}
}

//this function start new level.
void TheMathGame::startLevel(bool isReplay) {
	//resrt game state and exercise.
	Gs.Restart();
	Ex.RestartExercise();
	
	//clear lists
	keysList.clear();
	digitsList.clear();
	ClearCreaturesList(Creatures);
	ClearCreaturesList(CreaturesToFree);

	//set and init current level board.
	if (isReplay) currentLevel--; //if its replay we need to set the same level, so we need to minus 1 currentLevel.
	setCurrentLevelBoard();
	initBoard();
	
	// add 1 to current level and put false at IsEquationSolved.
	++currentLevel;
	IsEquationSolved = false;

	//Play Opening Sound.
	PlayOpeningSound();
}

void TheMathGame::doIteration(const list<char>& keyHits)
{
	int Bdir, Sdir;

	//if the user pressed on key of the big spaceship and small spaceship
	if (((Bdir = bigSpaceShip.GetDirection(keyHits)) != -1) && ((Sdir = smallSpaceShip.GetDirection(keyHits)) != -1))
	{
		(Bdir == Direction::ROTATE) ? bigSpaceShip.RotateHelper(SoundFreq::ROTATE_BIG) : bigSpaceShip.setDirection(Bdir);
		(Sdir == Direction::ROTATE) ? smallSpaceShip.RotateHelper(SoundFreq::ROTATE_SMALL) : smallSpaceShip.setDirection(Sdir);
		
		//if these are different keys from last time insert the keys to the keysList.
		if (keysList.empty())
			keysList.push_back(KeysByClockIteration(Gs.getClockCycle(), bigSpaceShip.GetArrowKey(Bdir), smallSpaceShip.GetArrowKey(Sdir)));
		else if (!keysList.back().CheckIfSameKeys(bigSpaceShip.GetArrowKey(Bdir), smallSpaceShip.GetArrowKey(Sdir)))
			keysList.push_back(KeysByClockIteration(Gs.getClockCycle(), bigSpaceShip.GetArrowKey(Bdir), smallSpaceShip.GetArrowKey(Sdir)));
		
		bigSpaceShip.move();
		smallSpaceShip.move();
	}
	//if the user pressed on key of the big spaceship
	else if ((Bdir = bigSpaceShip.GetDirection(keyHits)) != -1)
	{
		(Bdir == Direction::ROTATE) ? bigSpaceShip.RotateHelper(SoundFreq::ROTATE_BIG) : bigSpaceShip.setDirection(Bdir);
		
		//if this is different key from last time insert the key to the keysList.
		if (keysList.empty())
			keysList.push_back(KeysByClockIteration(Gs.getClockCycle(), bigSpaceShip.GetArrowKey(Bdir)));
		else if (!keysList.back().CheckIfSameKeys(bigSpaceShip.GetArrowKey(Bdir)))
			keysList.push_back(KeysByClockIteration(Gs.getClockCycle(), bigSpaceShip.GetArrowKey(Bdir)));

		bigSpaceShip.move();
	}
	//if the user pressed on key of the small spaceship
	else if ((Sdir = smallSpaceShip.GetDirection(keyHits)) != -1)
	{
		(Sdir == Direction::ROTATE) ? smallSpaceShip.RotateHelper(SoundFreq::ROTATE_SMALL) : smallSpaceShip.setDirection(Sdir);
		
		//if this is different key from last time insert the key to the keysList.
		if (keysList.empty())
			keysList.push_back(KeysByClockIteration(Gs.getClockCycle(), smallSpaceShip.GetArrowKey(Sdir)));
		else if (!keysList.back().CheckIfSameKeys(smallSpaceShip.GetArrowKey(Sdir)))
			keysList.push_back(KeysByClockIteration(Gs.getClockCycle(), smallSpaceShip.GetArrowKey(Sdir)));

		smallSpaceShip.move();
	}
	else //for canceling "fast run" of the spaceships
		//(in case that the spaceship is going to some direction and the user press again this direction).
	{
		bigSpaceShip.move();
		smallSpaceShip.move();
	}
	MoveCreatures();
	Gs.AddClockCycle(); //add clock cycle
	Gs.Print(); //print the game stats (for updating the clock cycles).
}

//this doIteration is for Solution play.
void TheMathGame::doIteration()
{
	Sleep(sleepTime); //sleep the sleep time every iteration.

	list<char> tempKeys;
	tempKeys.clear();

	//if this is the same clock iteration of the first var in keysList:
	//insert these keys (1 or 2) to the new tempKeys list.
	if (Gs.getClockCycle() == keysList.front().clock_iteration) 
	{
		for (size_t i = 0; i < keysList.front().NumOfKeys; i++)
			tempKeys.push_back(keysList.front().Keys[i]);
		keysList.pop_front();
	}

	//send tempKeys list to the original doIteratin 
	//(when this isn't the same clock iteration empty list will be sent)
	doIteration(tempKeys);
}

//input: direction, Start point of the object, width and height of the object.
//return: the significant char of the place that the object will go to.
char TheMathGame::GetSignificantChar(int direction, Point ObjStart, int ObjWidth, int ObjHeight)
{
	char SignificantCh;
	char FirstDigitSaver, FirstSpaceShipSaver; //to ensure that if there is a digit in this place, and after free pass, that digit will return.
	bool ThereIsDigit = false, ThereIsSpaceShip = false, ThereIsArtifact = false; //to know if there was a digit or spaceship already.
	bool ThereIsFlyer = false, ThereIsNumbersEeater = false;
	switch (direction)
	{
	case Direction::UP:
		for (int i = 0; i < ObjWidth; i++) //check all the chars at the line above the object.
		{
			SignificantCh = board[ObjStart.getY() - 1][ObjStart.getX()];
			ObjStart.set(ObjStart.getX() + 1, ObjStart.getY());
			//if the char is wall return wall.
			if (SignificantCh == TheMathGame::GameChars::WALL)
			{
				return SignificantCh;
			}
			else CheckSignificantChar(SignificantCh, FirstSpaceShipSaver, FirstDigitSaver, ThereIsSpaceShip, ThereIsDigit, ThereIsArtifact,ThereIsFlyer ,ThereIsNumbersEeater);
		}
		break;
	case Direction::DOWN:
		ObjStart.set(ObjStart.getX(), ObjStart.getY() + (ObjHeight - 1));
		for (int i = 0; i < ObjWidth; i++) //check all the chars at the line under the object.
		{
			SignificantCh = board[ObjStart.getY() + 1][ObjStart.getX()];
			ObjStart.set(ObjStart.getX() + 1, ObjStart.getY());
			//if the char is wall return wall.
			if (SignificantCh == TheMathGame::GameChars::WALL)
			{
				return SignificantCh;
			}
			else CheckSignificantChar(SignificantCh, FirstSpaceShipSaver, FirstDigitSaver, ThereIsSpaceShip, ThereIsDigit, ThereIsArtifact, ThereIsFlyer, ThereIsNumbersEeater);
		}
		break;
	case Direction::LEFT:
		for (int i = 0; i < ObjHeight; i++) //check all the chars at the column that left to the object.
		{
			SignificantCh = board[ObjStart.getY()][ObjStart.getX() - 1];
			ObjStart.set(ObjStart.getX(), ObjStart.getY() + 1);
			//if the char is wall return wall.
			if (SignificantCh == TheMathGame::GameChars::WALL)
			{
				return SignificantCh;
			}
			else CheckSignificantChar(SignificantCh, FirstSpaceShipSaver, FirstDigitSaver, ThereIsSpaceShip, ThereIsDigit, ThereIsArtifact, ThereIsFlyer, ThereIsNumbersEeater);
		}
		break;
	case Direction::RIGHT:
		ObjStart.set(ObjStart.getX() + (ObjWidth - 1), ObjStart.getY());
		for (int i = 0; i < ObjHeight; i++) //check all the chars at the column that right to the object
		{
			SignificantCh = board[ObjStart.getY()][ObjStart.getX() + 1];
			ObjStart.set(ObjStart.getX(), ObjStart.getY() + 1);
			//if the char is wall return wall.
			if (SignificantCh == TheMathGame::GameChars::WALL)
			{
				return SignificantCh;
			}
			else CheckSignificantChar(SignificantCh, FirstSpaceShipSaver, FirstDigitSaver, ThereIsSpaceShip, ThereIsDigit, ThereIsArtifact, ThereIsFlyer, ThereIsNumbersEeater);
		}
		break;
	case Direction::ROTATE:
	{
		bool flag = true; //The flag use to break the double 'for' when SignificantCh is wall or a SpaceShip
		char SpaceShipChar = board[ObjStart.getY()][ObjStart.getX()];
		if (ObjWidth < ObjHeight)
		{
			ObjStart.set(ObjStart.getX() - 1, ObjStart.getY() + 1);
			for (int i = 0; i < ObjHeight && flag; i++) //The width and height are swaped because we want to check if can to rotate
			{
				for (int j = 0; j < ObjWidth && flag; j++)
				{
					SignificantCh = board[ObjStart.getY() + j][ObjStart.getX() + i];
					if (SignificantCh == SpaceShipChar) //for not stopping the check if this is the same spaceship. 
						SignificantCh = TheMathGame::GameChars::FREE_PASS;
					//if the char is wall return wall.
					if (SignificantCh == TheMathGame::GameChars::WALL)
					{
						return SignificantCh;
					}
					else CheckSignificantChar(SignificantCh, FirstSpaceShipSaver, FirstDigitSaver, ThereIsSpaceShip, ThereIsDigit, ThereIsArtifact, ThereIsFlyer, ThereIsNumbersEeater);
				}
			 }
		 }
	     else					  
		 {
			  ObjStart.set(ObjStart.getX() + 1, ObjStart.getY() - 1);
			  for (int i = 0; i < ObjWidth && flag; i++) //The width and height are swaped because we want to check if can to rotate
			  {
				  for (int j = 0; j < ObjHeight && flag; j++)
				  {
					  SignificantCh = board[ObjStart.getY() + i][ObjStart.getX() + j];
					  if (SignificantCh == SpaceShipChar) //for not stopping in the check if there is any spaceship. 
						  SignificantCh = TheMathGame::GameChars::FREE_PASS;
					  //if the char is wall return wall.
					  if (SignificantCh == TheMathGame::GameChars::WALL)
					  {
						  return SignificantCh;
					  }
					  else CheckSignificantChar(SignificantCh, FirstSpaceShipSaver, FirstDigitSaver, ThereIsSpaceShip, ThereIsDigit, ThereIsArtifact, ThereIsFlyer, ThereIsNumbersEeater);
				  }
			  }
		  }
		}
	}
	//if we are here there isn't a wall.
	if (ThereIsSpaceShip)	return FirstSpaceShipSaver;
	else if (ThereIsArtifact)	return GameChars::ARTIFACT;
	else if (ThereIsFlyer) return GameChars::COL_FLYER;
	else if (ThereIsNumbersEeater) return GameChars::NUMBERS_EATER;
	else if (ThereIsDigit)	return FirstDigitSaver;
	else return GameChars::FREE_PASS; //any different char is a free pass.
}

//this function update the board after movment.
void TheMathGame::UpdateBoard(int direction, Point ObjStart, int ObjWidth, int ObjHeight)
{
	switch (direction)
	{
	case Direction::UP:
		for (int i = 0; i < ObjWidth; i++) //check all the chars at the line above the spaceship.
		{
			board[ObjStart.getY() - 1][ObjStart.getX() + i] = board[ObjStart.getY()][ObjStart.getX() + i];
			board[ObjStart.getY() + (ObjHeight - 1)][ObjStart.getX() + i] = TheMathGame::GameChars::FREE_PASS;
		}
		break;
	case Direction::DOWN:
		for (int i = 0; i < ObjWidth; i++) //check all the chars at the line above the spaceship.
		{
			board[ObjStart.getY() + ObjHeight][ObjStart.getX() + i] = board[ObjStart.getY() + (ObjHeight - 1)][ObjStart.getX() + i];
			board[ObjStart.getY()][ObjStart.getX() + i] = TheMathGame::GameChars::FREE_PASS;
		}
		break;
	case Direction::LEFT:
		for (int i = 0; i < ObjHeight; i++) //check all the chars at the line above the spaceship.
		{
			board[ObjStart.getY() + i][ObjStart.getX() - 1] = board[ObjStart.getY() + i][ObjStart.getX()];
			board[ObjStart.getY() + i][ObjStart.getX() + (ObjWidth - 1)] = TheMathGame::GameChars::FREE_PASS;
		}
		break;
	case Direction::RIGHT:
		for (int i = 0; i < ObjHeight; i++) //check all the chars at the line above the spaceship.
		{
			board[ObjStart.getY() + i][ObjStart.getX() + ObjWidth] = board[ObjStart.getY() + i][ObjStart.getX() + (ObjWidth - 1)];
			board[ObjStart.getY() + i][ObjStart.getX()] = TheMathGame::GameChars::FREE_PASS;
		}
		break;
	case Direction::ROTATE:
	{
		char c = board[ObjStart.getY()][ObjStart.getX()];
		for (int i = 0; i < ObjHeight; i++) //put spaces
			for (int j = 0; j < ObjWidth; j++)
					board[ObjStart.getY() + i][ObjStart.getX() + j] = TheMathGame::GameChars::FREE_PASS;
		(ObjWidth > ObjHeight) ? ObjStart.set(ObjStart.getX() + 1, ObjStart.getY() - 1) : ObjStart.set(ObjStart.getX() - 1, ObjStart.getY() + 1);
		for (int i = 0; i < ObjWidth; i++) //put spaceship chars
			for (int j = 0; j < ObjHeight; j++)
				board[ObjStart.getY() + i][ObjStart.getX() + j] = c;
		break;
	}
	default: break;
	}
}

//input: direction, point list of an artifact, LengthToAdd (default is 0).
//this function update the board for artifact movment.
void TheMathGame::UpdateBoard(int direction, list<Point> points, int LengthToAdd)
{
	char ch = board[points.front().getY()][points.front().getX()]; //get the first sign of the object.
	
	for (Point p : points) //put free pass at all the points of the artifact.
	{
		board[p.getY()][p.getX()] = TheMathGame::GameChars::FREE_PASS;
	}
	
	//set all the points at the right dest, and insert the object's sign to this place at board.
	for (Point p : points)
	{
		p = CalculateDestByDirection(p, direction, LengthToAdd);
		board[p.getY()][p.getX()] = ch;
	}
}

//this function update the board after crossing the screen.
void TheMathGame::UpdateBoardWhenCrossScreen(Point StartPoint, Point DestPoint, int ObjWidth, int ObjHeight)
{
	char c = board[StartPoint.getY()][StartPoint.getX()]; //for saving the spaceship char.
	for (int i = 0; i < ObjHeight; i++) //put spaces
		for (int j = 0; j < ObjWidth; j++)
			board[StartPoint.getY() + i][StartPoint.getX() + j] = TheMathGame::GameChars::FREE_PASS;
	for (int i = 0; i < ObjHeight; i++) //put spaceship chars
		for (int j = 0; j < ObjWidth; j++)
			board[DestPoint.getY() + i][DestPoint.getX() + j] = c;
}

//this function update the board after crossing the screen.
void TheMathGame::UpdateBoardWhenCrossScreen(int direction, list<Point> points, int ObjWidth, int ObjHeight, int LengthToAdd)
{
	char ch = board[points.front().getY()][points.front().getX()]; //get the first sign of the object.
	//for all the points of the object, put space, change to the right position, and put ch.
	for (Point p : points) 
	{
		board[p.getY()][p.getX()] = TheMathGame::GameChars::FREE_PASS; 
		switch (direction) {
			case Direction::UP:
					//set y to the destination y.
					// resolution height end - ( the height of the artifact - 1 ) + the difference from the height beginning - LengthToAdd.
					p.set(p.getX(), Resolution::HEIGHT_END - (ObjHeight - 1) + (p.getY() - Resolution::HEIGHT_BEGINNIG) - LengthToAdd);
				break;
			case Direction::DOWN:
				//set y to the destination y.
					// resolution height beginning + ( the height of the artifact - 1 ) - the diffrenece from the end + length to add.                     
					p.set(p.getX(), Resolution::HEIGHT_BEGINNIG + (ObjHeight - 1) - (Resolution::HEIGHT_END - p.getY()) + LengthToAdd);
				break;
			case Direction::LEFT:
					//set x to the destination y.
					// resolution width end - ( the width of the artifact - 1 ) + the diffrenece from the beginning - length to add.  
					p.set(Resolution::WIDTH_END - (ObjWidth - 1) + (p.getX() - Resolution::WIDTH_BEGINNIG) - LengthToAdd, p.getY());
				break;
			case Direction::RIGHT:
					//set x to the destination y.
					// resolution width beginning + ( the width of the artifact - 1 ) - the diffrenece from the end + length to add. 
					p.set(Resolution::WIDTH_BEGINNIG + (ObjWidth - 1) - (Resolution::WIDTH_END - p.getX()) + LengthToAdd, p.getY());
				break;
			default: break;
		}
		board[p.getY()][p.getX()] = ch;
	}
}

//this function get the significant char of the place that the object will go to after crossing the screen.
char TheMathGame::GetSignificantCharAfterCrossingScreen(Point dest, int ObjWidth, int ObjHeight)
{
	char SignificantCh;
	char FirstDigitSaver, FirstSpaceShipSaver; //to ensure that if there is a digit in this place, and after free pass, that digit will return.
	bool ThereIsDigit = false, ThereIsSpaceShip = false, ThereIsArtifact = false; //to know if there was a digit or spaceship already.
	bool flag = true; //for stopping the double loops if needed.
	
	for (int i = 0; i < ObjHeight && flag; i++) //check all the chars in this place.
	{
		for (int j = 0; j < ObjWidth && flag; j++)
		{
			SignificantCh = board[dest.getY() + i][dest.getX() + j];
			if (SignificantCh == TheMathGame::GameChars::WALL)
			{
				return SignificantCh;
			}
			else if (SignificantCh == TheMathGame::GameChars::BIG_SPACE_SHIP_SIGN || SignificantCh == TheMathGame::GameChars::SMALL_SPACE_SHIP_SIGN)
			{
				ThereIsSpaceShip = true;
				FirstSpaceShipSaver = SignificantCh;
			}
			else if (!ThereIsDigit && SignificantCh >= '0' && SignificantCh <= '9') //if the char is the first digit, save it.
			{
				ThereIsDigit = true;
				FirstDigitSaver = SignificantCh;
			}
			else if (SignificantCh == TheMathGame::GameChars::ARTIFACT)
			{
				ThereIsArtifact = true;
			}
		}
	}
	//if we are here there isn't a wall.
	if (ThereIsSpaceShip)	return FirstSpaceShipSaver;
	else if (ThereIsArtifact)	return GameChars::ARTIFACT;
	else if (ThereIsDigit)	return FirstDigitSaver;
	else return GameChars::FREE_PASS;
	//else return SignificantCh;
}

//this function get the all the numbers at the place that the object will go to.
//return: TryToSolveEquation.
bool TheMathGame::getNum(Point place, int direction, int ObjWidth, int ObjHeight)
{
	PlaySoundGame(SoundFreq::TAKINGNUMBER, SoundDuration::GAMESOUNDDURATION);

	bool res = true;

	if (direction == Direction::UP || direction == Direction::DOWN) 
	{
		for (int i = 0; i < ObjWidth; i++) //for all this line.
		{
			//if this char is a digit, get the full number and try to solve the equation.
			//go to the next char of this line.
			if ((board[place.getY()][place.getX()] >= '0' && board[place.getY()][place.getX()] <= '9') || board[place.getY()][place.getX()]=='-')
			{
				res &= TryToSolveEquation(getFullNumAndCalc(place));
				place.set(place.getX(), place.getY() + 1);
			}
			else //go to the next char of this line.
				place.set(place.getX() + 1, place.getY());
		}
	}
	else if (direction == Direction::LEFT || direction == Direction::RIGHT)
	{
		for (int i = 0; i < ObjHeight; i++) //for all this column.
		{
			if ((board[place.getY()][place.getX()] >= '0' && board[place.getY()][place.getX()] <= '9') || board[place.getY()][place.getX()] == '-')
			{
				//go to the next char of this line.
				res &= TryToSolveEquation(getFullNumAndCalc(place));
				place.set(place.getX(), place.getY() + 1);
			}
			else //go to the next char of this line.
				place.set(place.getX(), place.getY() + 1);
		}
	}
	return res;
}

//this function get the all the numbers at the place that the artifact will go to.
//return: TryToSolveEquation.
bool TheMathGame::getNum(int direction, list<Point> points)
{
	bool res = true;
	
	PlaySoundGame(SoundFreq::TAKINGNUMBER, SoundDuration::GAMESOUNDDURATION);

	for (Point p : points) //for all the points.
	{
		p = CalculateDestByDirection(p, direction); //calculate their dest by the direction.
		//if there is number there, take it and try to solve the equation.
		if ((board[p.getY()][p.getX()] >= '0' && board[p.getY()][p.getX()] <= '9') || board[p.getY()][p.getX()] == '-')
			res &= TryToSolveEquation(getFullNumAndCalc(p));
	}
	return res;
}

//this function get the all numbers of the place that the object will cross to.
bool TheMathGame::getNumAfterRotateOrCrossScreen(Point Start, int ObjWidth, int ObjHeight)
{
	bool res = true;

	PlaySoundGame(SoundFreq::TAKINGNUMBER, SoundDuration::GAMESOUNDDURATION);

	for (int i = 0; i < ObjHeight; i++) //check all the chars.
	{
		for (int j = 0; j < ObjWidth; j++)
		{
			if (board[Start.getY() + i][Start.getX() + j] >= '0' && board[Start.getY() + i][Start.getX() + j] <= '9')
			{ //if the char is a digit, get the full number, and try to solve the equation.
				res &= TryToSolveEquation(getFullNumAndCalc(Point(Start.getX() + j, Start.getY() + i)));
			}
		}
	}
	return res;
}

//this function get the all numbers of the place that the object will cross to.
bool TheMathGame::getNumAfterRotateOrCrossScreen(int direction, list<Point> points, int LengthToAdd, int ObjWidth, int ObjHeight)
{
	bool res = true;

	PlaySoundGame(SoundFreq::TAKINGNUMBER, SoundDuration::GAMESOUNDDURATION);

	switch (direction) {
	case Direction::UP:
		for (Point& p : points)
		{
			//p.set(p.getX(), Resolution::HEIGHT_END - p.getY() - LengthToAdd - ObjHeight);
			p.set(p.getX(), Resolution::HEIGHT_END - (ObjHeight - 1) + (p.getY() - Resolution::HEIGHT_BEGINNIG) - LengthToAdd);
			if (board[p.getY()][p.getX()] >= '0' && board[p.getY()][p.getX()] <= '9' && res)
			{ //if the char is a digit, get the full number, and try to solve the equation.
				res &= TryToSolveEquation(getFullNumAndCalc(p));
			}
		}
		break;
	case Direction::DOWN:
		for (Point& p : points)
		{
			//p.set(p.getX(), Resolution::HEIGHT_BEGINNIG + (Resolution::HEIGHT_END - p.getY()) + LengthToAdd + ObjHeight);
			p.set(p.getX(), Resolution::HEIGHT_BEGINNIG + (ObjHeight - 1) - (Resolution::HEIGHT_END - p.getY()) + LengthToAdd);
			if (board[p.getY()][p.getX()] >= '0' && board[p.getY()][p.getX()] <= '9' && res)
			{ //if the char is a digit, get the full number, and try to solve the equation.
				res &= TryToSolveEquation(getFullNumAndCalc(p));
			}
		}
		break;
	case Direction::LEFT:
		for (Point p : points)
		{
			//p.set(Resolution::WIDTH_END - p.getX() - LengthToAdd, p.getY() - ObjWidth);
			p.set(Resolution::WIDTH_END - (ObjWidth - 1) + (p.getX() - Resolution::WIDTH_BEGINNIG) - LengthToAdd, p.getY());
			if (board[p.getY()][p.getX()] >= '0' && board[p.getY()][p.getX()] <= '9' && res)
			{ //if the char is a digit, get the full number, and try to solve the equation.
				res &= TryToSolveEquation(getFullNumAndCalc(p));
			}
		}
		break;
	case Direction::RIGHT:
		for (Point& p : points)
		{
			//p.set(Resolution::WIDTH_BEGINNIG + (Resolution::WIDTH_END - p.getX()) - LengthToAdd, p.getY() + ObjWidth);
			p.set(Resolution::WIDTH_BEGINNIG + (ObjWidth - 1) - (Resolution::WIDTH_END - p.getX()) + LengthToAdd, p.getY());
			if (board[p.getY()][p.getX()] >= '0' && board[p.getY()][p.getX()] <= '9' && res)
			{ //if the char is a digit, get the full number, and try to solve the equation.
				res &= TryToSolveEquation(getFullNumAndCalc(p));
			}
		}
		break;
	default: break;
	}
	return res;
}

//this function get the full number at place and calculate it to an integer.
int TheMathGame::getFullNumAndCalc(Point place)
{
	int res = 0;

	//go to the previous place at this row if the char is a digit.
	while (board[place.getY()][place.getX()] >= '0' && board[place.getY()][place.getX()] <= '9')
	{
		place.set(place.getX() - 1, place.getY());
	}
	
	place.set(place.getX() + 1, place.getY()); //back to the first digit of the number.

	//now calculate the number and delete him from the board.
	//for all the digits of this number
	while (board[place.getY()][place.getX()] >= '0' && board[place.getY()][place.getX()] <= '9') 
	{
		res *= 10; //multiply res in 10 (at the first iteration it's will not influencing because res is 0).
		res += board[place.getY()][place.getX()] - '0'; //add the digit to res.
		place.draw(TheMathGame::GameChars::FREE_PASS); //delete the digit from the screen.
		board[place.getY()][place.getX()] = TheMathGame::GameChars::FREE_PASS; //delete the digit from board.
		digitsList.remove((const Point)place); //delete this point from digits list
		place.set(place.getX() + 1, place.getY()); //move to the next place in this row.
	}

	return res;
}

//this function delete all the numbers from the direction of the object (for spacehship).
void TheMathGame::DeleteTheAllNumbers(Point place, int direction, int ObjWidth, int ObjHeight)
{
	if (direction == Direction::UP || direction == Direction::DOWN)
	{
		for (int i = 0; i < ObjWidth; i++) //for all the points of object width.
		{
			//if the char is a digit, delete the number that the digit is own to.
			if (board[place.getY()][place.getX()] >= '0' && board[place.getY()][place.getX()] <= '9')
			{
				DeleteNumber(place);
				place.set(place.getX() + 1, place.getY());
			}
			else
				place.set(place.getX() + 1, place.getY());
		}
	}
	else if (direction == Direction::LEFT || direction == Direction::RIGHT)
	{
		for (int i = 0; i < ObjHeight; i++) //for all the points of object height.
		{
			//if the char is a digit, delete the number that the digit is own to.
			if (board[place.getY()][place.getX()] >= '0' && board[place.getY()][place.getX()] <= '9')
			{
				DeleteNumber(place);
				place.set(place.getX(), place.getY() + 1);
			}
			else
				place.set(place.getX(), place.getY() + 1);
		}
	}
}

//this function delete all the numbers from the direction of the object (for artifact).
void TheMathGame::DeleteTheAllNumbers(int direction, list<Point> points, int LengthToAdd)
{
	for (Point p : points) //for all the points.
	{
		p = CalculateDestByDirection(p, direction, LengthToAdd); //calc thier destination by direction.
		if (board[p.getY()][p.getX()] >= '0' && board[p.getY()][p.getX()] <= '9') //if this is a digit, delete the number.
			DeleteNumber(p);
	}
}

//input: place of an digit.
//this function delete the whole number of this digit.
void TheMathGame::DeleteNumber(Point place)
{
	//go to the previous place at this row if the char is a digit.
	while (board[place.getY()][place.getX()] >= '0' && board[place.getY()][place.getX()] <= '9')
	{
		place.set(place.getX() - 1, place.getY());
	}
	place.set(place.getX() + 1, place.getY()); //back to the first digit of the number.

	//for all the digits of this number
	while (board[place.getY()][place.getX()] >= '0' && board[place.getY()][place.getX()] <= '9') 
	{
		place.draw(TheMathGame::GameChars::FREE_PASS); //delete the digit from the screen.
		board[place.getY()][place.getX()] = TheMathGame::GameChars::FREE_PASS; //delete the digit from the board.
		digitsList.remove((const Point)place); //delete this point from digits list
		place.set(place.getX() + 1, place.getY()); //move to the next place in this row.
	}
}

//input: number.
//this function is trying to solve the equation with this number.
//return: false if the equation cant be solved, true otherwise.
bool TheMathGame::TryToSolveEquation(int num)
{
	unsigned int check = Ex.CheckAns(num); //check the ansewer.
	if (check == SOLVED) //if the equation resolved.
	{
		IsEquationSolved = true;
	}
	else if (check == CAN_BE_SOLVED) //if the equation can be solved, print the new exercise.
	{
		Ex.PrintExercise();
	}
	else if (check == CANT_BE_SOLVED)//if the equation can't be solved.
	{
		if (Gs.GetLives() == 1)
			Gs.LessLife();
		else startLevelAfterEquationCantBeSolved();
		return false;
	}
	else if (check == WRONG) //if the equation is wrong
	{
		Gs.LessLife();
	}
	return true;
}

//input: artifact's start point, width, height, isStatic (by ref).
//return: if there is spaceship under the artifact and the artifact is static, return the spaceship's address.
SpaceShip* TheMathGame::StaticsCheckArtifact(Artifact& ar, bool& isStatic)
{
	SpaceShip* res = nullptr;
	//get the significant char.
	char SignificantChar = GetSignificantChar(Direction::DOWN, ar);
	//check if there is spaceship under this artifact, if there is tow return the sign of the first.
	char SpaceShipChar = ThereIsSpaceShipUnderMe(ar.getPointList());

	if (SignificantChar == TheMathGame::GameChars::WALL)
	{
		res = nullptr;
		isStatic = true;
	}
	if ((SpaceShipChar == TheMathGame::GameChars::BIG_SPACE_SHIP_SIGN) && (isStatic))
	{
		res = &bigSpaceShip;
		ar.CalculateWidthAndHeight(&bigSpaceShip);
		isStatic = true;
	}
	if ((SpaceShipChar == TheMathGame::GameChars::SMALL_SPACE_SHIP_SIGN) && (isStatic))
	{
		res = &smallSpaceShip;
		ar.CalculateWidthAndHeight(&smallSpaceShip);
		isStatic = true;
	}
	if ((SignificantChar >= '0' && SignificantChar <= '9') && (isStatic))
	{
		res = nullptr;
		isStatic = true;
	}
	if (SignificantChar == GameChars::FREE_PASS)
	{
		res = nullptr;
		isStatic = false;
	}
	return res;
}

//input: start point, width and height.
//return: pointer to artifact or nullptr.
//this function check if there is an artifact above the spaceship, if yes return pointer of him.
Artifact* TheMathGame::CheckForArtifacts(Point start, int width, int height, SpaceShip* spaceShip)
{
	Artifact* res = spaceShip->GetDragged();
	char c = GetSignificantChar(Direction::UP, start, width, height); //we need to check the artifacts above the spaceship.
	
	//if the significant char is artifact and the artifact is static.
	//check if there is a leader, if there is not take this artifact.
	//if there is check if it's me (for the case that the artifact take the spaceship as leader before this function).

	if (c == TheMathGame::GameChars::ARTIFACT)
	{
		Artifact& tempAr = getAboveArtifact(*spaceShip);
		if ((tempAr.IsStatic()) && (tempAr.getLeader() != nullptr && tempAr.getLeader() == spaceShip))
		{
			if (spaceShip->GetDragged() == nullptr) //if there is already dragged artifact, unificate these artifacts.
				res = &tempAr;
			else if (!(*spaceShip->GetDragged() == tempAr))
				ArtifactsUnification(*spaceShip->GetDragged(), tempAr);

			spaceShip->SetPosOfDragged(GetPosOfDraggedAtrifact(spaceShip));
			tempAr.SetLeader(spaceShip);
		}
	}
	return res;
}

//this function print the board.
void TheMathGame::printBoard()
{
	for (int i = 0; i<ROWS; ++i) {
		for (int j = 0; j<COLS; ++j) {
			gotoxy(j, i);
			//if this char is a special char, don't print it.
			if (board[i][j] != GameChars::BIG_SPACE_SHIP_SIGN && board[i][j] != GameChars::SMALL_SPACE_SHIP_SIGN && board[i][j] != GameChars::EXERCISE && board[i][j] != GameChars::GAME_STATE && board[i][j] != GameChars::ARTIFACT && !CheckIfDigit(board[i][j]))
				cout << board[i][j];
			else if (CheckIfDigit(board[i][j])) //if the char is an digit, print in color.
			{
				setTextColor(Color::LIGHTMAGENTA);
				cout << board[i][j];
				setTextColor(Color::WHITE);
			}
			cout.flush();
		}
	}
	//print the specials
	smallSpaceShip._print();
	bigSpaceShip._print();
	Ex.PrintExercise();
	Gs.Print();
	_printCreatures();
}

//input: the spaceship sign.
//this function will bomb the spaceship, and start the level again with one less live,
//if this is the last live, game over.
void TheMathGame::BombSpaceShip(char SpaceShipSign)
{
	PlaySoundGame(SoundFreq::BOMBHITSPACESHIP, SoundDuration::GAMESOUNDDURATION_LONG);
	for (int i = 0; i < 3; i++) //flicker the injured spaceship 3 times.
	{
		Sleep(BOMB_SLEEP_DURATION);
		(SpaceShipSign == GameChars::BIG_SPACE_SHIP_SIGN) ? bigSpaceShip.DeleteSpaceShip() : smallSpaceShip.DeleteSpaceShip();
		Sleep(BOMB_SLEEP_DURATION);
		(SpaceShipSign == GameChars::BIG_SPACE_SHIP_SIGN) ? bigSpaceShip._print() : smallSpaceShip._print();
	}

	if (Gs.GetLives() == 1) //if this is the last live, game over.
	{
		Gs.LessLife(); //less one life.
	}
	else startLevelAfterBomb(); //else start the level again.
}

//input: list of points
//return: if there is spaceship under this object, return the sign of it (if there is tow return the first one),
//else return wall sign.
char TheMathGame::ThereIsSpaceShipUnderMe(list <Point> points)
{
	char ch;

	for (Point p : points) //check all the chars under the object and stop if there is spaceship.
	{
		p = CalculateDestByDirection(p, Direction::DOWN);
		ch = board[p.getY()][p.getX()];
		//if the char is any spaceship return the spaceship's sign.
		if (ch == TheMathGame::GameChars::BIG_SPACE_SHIP_SIGN || ch == TheMathGame::GameChars::SMALL_SPACE_SHIP_SIGN)
		{
			return ch;
		}
	}
	return GameChars::WALL;
}

//input: direction, obj volume.
//this funcion push the artifact to this direction.
//return: true if the push succeded, false otherwise.
bool TheMathGame::PushArtifact(SpaceShip* spaceShip, int direction)
{
	Artifact& tempAr = getNearestArtifact(*spaceShip);

	//if the object can to push this artifact, and the artifact move succeded.
	if (CheckIfCanPushArtifact(spaceShip, &tempAr, direction) && tempAr.move(direction))
	{
		//explanation: Because the smallSpaceShip is moving first if there isn't spaceship's key from user (we did it at doIteration as default)
		//and if the spaceship that pushing the artifact is small spaceship and she cant do it alone (so the bigspace ship is pushing with her)
		//move the big space ship (for staying at same line)
		if (spaceShip == &smallSpaceShip && !CheckIfCanIPushArtifact(spaceShip, &tempAr, direction))
			bigSpaceShip.move();
		return true;
	}
	else return false;
}

//input: direction, object width and height.
//return: true if the push succeded, false otherwise.
bool TheMathGame::PushArtifactAfterCrossing(SpaceShip* spaceShip, int direction, int ObjWidth, int ObjHeight, int LengthToAdd)
{
	bool res = true;
	Artifact& tempAr = getCroosedArtifact(*spaceShip);
	if (CheckIfCanPushArtifact(spaceShip, &tempAr, direction))
	{
		tempAr.setDirection(direction);

		//if the direction is up or down, return if the artifact can move with the spaceship height minus 1.
		//minus 1 because there is one step of the artifact also.
		if (direction == Direction::UP || direction == Direction::DOWN)
		{
			return tempAr.move(ObjWidth - 1);
		}
		//if the direction is right or left, return if the artifact can move with the spaceship width minus 1.
		//minus 1 because there is one step of the artifact also.
		else return tempAr.move(ObjWidth - 1 + LengthToAdd);
	}
	else return false;
}

//this function check the shignificant char and update the variables.
void TheMathGame::CheckSignificantChar(char SignificantCh, char& FirstSpaceShipSaver, char& FirstDigitSaver, bool& ThereIsSpaceShip, bool& ThereIsDigit, bool& ThereIsArtifact, bool& ThereIsFlyer, bool& ThereIsNumbersEeater)
{
	if (SignificantCh == TheMathGame::GameChars::BIG_SPACE_SHIP_SIGN || SignificantCh == TheMathGame::GameChars::SMALL_SPACE_SHIP_SIGN)
	{
		ThereIsSpaceShip = true;
		FirstSpaceShipSaver = SignificantCh;
	}
	else if (!ThereIsDigit &&((SignificantCh >= '0' && SignificantCh <= '9')))
	{
		ThereIsDigit = true;
		FirstDigitSaver = SignificantCh;
	}
	else if (SignificantCh == TheMathGame::GameChars::ARTIFACT)
	{
		ThereIsArtifact = true;
	}
	else if (!ThereIsFlyer && (SignificantCh == TheMathGame::GameChars::ROW_FLYER || SignificantCh == TheMathGame::GameChars::COL_FLYER))
		ThereIsFlyer = true;
	else if (!ThereIsNumbersEeater && (SignificantCh == TheMathGame::GameChars::NUMBERS_EATER))
		ThereIsNumbersEeater = true;
}

//input: start point of an object, direction, width and height.
//return: the start point after the movment to the setted direction.
Point TheMathGame::CalculateDestByDirection(Point start, int direction, int width, int height)
{
	switch (direction){
	case Direction::UP:
		return Point(start.getX(), start.getY() - 1);
		break;
	case Direction::DOWN:
		return Point(start.getX(), start.getY() + height);
		break;
	case Direction::LEFT:
		return Point(start.getX() - 1, start.getY());
		break;
	case Direction::RIGHT:
		return Point(start.getX() + width, start.getY());
		break;
	default:
		return Point(1, 1); //just for all paths will return point.
		break;
	}
}

//input: point, direction, LengthToAdd (default is 0)
//return: the point after the movment to the setted direction.
Point TheMathGame::CalculateDestByDirection(Point p, int direction, int LengthToAdd)
{
	switch (direction) {
	case Direction::UP:
		return Point(p.getX(), p.getY() - 1 - LengthToAdd);
		break;
	case Direction::DOWN:
		return Point(p.getX(), p.getY() + 1 + LengthToAdd);
		break;
	case Direction::LEFT:
		return Point(p.getX() - 1 - LengthToAdd, p.getY());
		break;
	case Direction::RIGHT:
		return Point(p.getX() + 1 + LengthToAdd, p.getY());
		break;
	default:
		return Point(1, 1); //just for all paths will return point.
		break;
	}
}

//input: point, direction, obj width , obj height, length to add.
//return: the crossing point of this point.
Point TheMathGame::CalculateCrossingDestByDirection(Point p, int direction, int ObjWidth, int ObjHeight, int LengthToAdd)
{
	switch (direction) //put the point at the right dest.
	{
	case Direction::UP:
		p.set(p.getX(), Resolution::HEIGHT_END - (ObjHeight - 1) + (p.getY() - Resolution::HEIGHT_BEGINNIG) - LengthToAdd);
		break;
	case Direction::DOWN:
		p.set(p.getX(), Resolution::HEIGHT_BEGINNIG + (ObjHeight - 1) - (Resolution::HEIGHT_END - p.getY()) + LengthToAdd);
		break;
	case Direction::LEFT:
		p.set(Resolution::WIDTH_END - (ObjWidth - 1) + (p.getX() - Resolution::WIDTH_BEGINNIG) - LengthToAdd, p.getY());
		break;
	case Direction::RIGHT:
		p.set(Resolution::WIDTH_BEGINNIG + (ObjWidth - 1) - (Resolution::WIDTH_END - p.getX()) + LengthToAdd, p.getY());
		break;
	default: return Point(1, 1);
	}
	return p;
}

//input: point, direction, LengthToAdd.
//return: the point after adding length to add by the direction.
//(the difference from calculate dest by direction is that here we don't adding the 1 step, only the length to add,
//its critical for getsignificant chat)
Point TheMathGame::AddLengthByDirection(Point p, int direction, int LengthToAdd)
{
	switch (direction) {
	case Direction::UP:
		return Point(p.getX(), p.getY() - LengthToAdd);
		break;
	case Direction::DOWN:
		return Point(p.getX(), p.getY() + LengthToAdd);
		break;
	case Direction::LEFT:
		return Point(p.getX() - LengthToAdd, p.getY());
		break;
	case Direction::RIGHT:
		return Point(p.getX() + LengthToAdd, p.getY());
		break;
	default:
		return Point(1, 1); //just for all paths will return point.
		break;
	}
}

//input: object direction, start point, width and height.
//return: true if there is number at his direction.
bool TheMathGame::CheckIfThereIsNumber(int _direction, Point ObjStart, int ObjWidth, int ObjHeight)
{
	char SignificantCh;

	switch (_direction)
	{
		case Direction::UP:
			for (int i = 0; i < ObjWidth; i++) //check all the chars at the line above the object.
			{
				SignificantCh = board[ObjStart.getY() - 1][ObjStart.getX()];
				ObjStart.set(ObjStart.getX() + 1, ObjStart.getY());
				//if the char is a digit return true.
				if (SignificantCh >= '0' && SignificantCh <= '9')	return true;
			}
			break;
		case Direction::DOWN:
			ObjStart.set(ObjStart.getX(), ObjStart.getY() + (ObjHeight - 1));
			for (int i = 0; i < ObjWidth; i++) //check all the chars at the line under the object.
			{
				SignificantCh = board[ObjStart.getY() + 1][ObjStart.getX()];
				ObjStart.set(ObjStart.getX() + 1, ObjStart.getY());
				//if the char is a digit return true.
				if (SignificantCh >= '0' && SignificantCh <= '9')	return true;
			}
			break;
		case Direction::LEFT:
			for (int i = 0; i < ObjHeight; i++) //check all the chars at the column that left to the object.
			{
				SignificantCh = board[ObjStart.getY()][ObjStart.getX() - 1];
				ObjStart.set(ObjStart.getX(), ObjStart.getY() + 1);
				//if the char is a digit return true.
				if (SignificantCh >= '0' && SignificantCh <= '9')	return true;
			}
			break;
		case Direction::RIGHT:
			ObjStart.set(ObjStart.getX() + (ObjWidth - 1), ObjStart.getY());
			for (int i = 0; i < ObjHeight; i++) //check all the chars at the column that right to the object
			{
				SignificantCh = board[ObjStart.getY()][ObjStart.getX() + 1];
				ObjStart.set(ObjStart.getX(), ObjStart.getY() + 1);
				//if the char is a digit return true.
				if (SignificantCh >= '0' && SignificantCh <= '9')	return true;
			}
			break;
		}
	return false;
}

//This function check if the spaceship can push an artifact by his own (by the game rules),
//if the spaceship can't, the function calculate the volume of the other spaceship and checks
//if she push the same artifact too. If the spaceships together can push the artifact together they all
//(the artifact and the spaceships) move.
bool TheMathGame::CheckIfCanPushArtifact(SpaceShip* ThisSpaceShip, Artifact* pushedArtifact, int direction)
{
	SpaceShip* TheOtherSpaceShip = &bigSpaceShip;
	(TheOtherSpaceShip == ThisSpaceShip) ? TheOtherSpaceShip = &smallSpaceShip : TheOtherSpaceShip = &bigSpaceShip; //Check and init the other spaceship
	double spaceShipVolumeAfterClac, otherSpaceShipVolumeAfterClac, ArtifactVolume; //The volumes of the spaceships and the artifact

	ArtifactVolume = pushedArtifact->GetArtifactVolume(); //get the volume of the artifact
	spaceShipVolumeAfterClac = (CalculateObjectVolume(ThisSpaceShip->getWidth(), ThisSpaceShip->getHeight()) * 2) / 3; //calc the volume of the spaceship by the game rules

	if ((ArtifactVolume) <= (spaceShipVolumeAfterClac)) return true; //if the space ship can push the artifact by his own return true
	else
	{
		if (TheOtherSpaceShip->GetDirectionOfSpaceship() == ThisSpaceShip->GetDirectionOfSpaceship()) //check if the two spaceships have the same direction
		{
			//swap the direction because we are looking for char at the opposite direction of the spaceships.
			direction = SwapTheDirection(direction); 
			if (SearchForChar(direction, pushedArtifact->getPointList(), TheOtherSpaceShip->GetSign())) //check if the other space is near the artifact
			{
				//calc the volume of the other spaceship by the game rules
				otherSpaceShipVolumeAfterClac = (CalculateObjectVolume(TheOtherSpaceShip->getWidth(), TheOtherSpaceShip->getHeight()) * 2) / 3; 
				//if the two spaceships can push the artifact together, return true
				if ((ArtifactVolume) <= (otherSpaceShipVolumeAfterClac + spaceShipVolumeAfterClac)) return true; 
			}
		}
	}
	return false;
}

//This function check if the spaceship can push the artifact by her own volume.
bool TheMathGame::CheckIfCanIPushArtifact(SpaceShip* ThisSpaceShip, Artifact* pushedArtifact, int direction)
{
	double spaceShipVolumeAfterClac, ArtifactVolume; //The volumes of the spaceships and the artifact

	ArtifactVolume = pushedArtifact->GetArtifactVolume(); //get the volume of the artifact
	spaceShipVolumeAfterClac = (CalculateObjectVolume(ThisSpaceShip->getWidth(), ThisSpaceShip->getHeight()) * 2) / 3; //calc the volume of the spaceship by the game rules

	if ((ArtifactVolume) <= (spaceShipVolumeAfterClac)) return true; //if the space ship can push the artifact by his own return true
	else return false;
}

//input: direction, list of points, length to add.
//return: significant char of an atrifact.
char TheMathGame::GetSignificantChar(int direction, Artifact& ar, int LengthToAdd)
{
	char SignificantChar = TheMathGame::GameChars::FREE_PASS, temp;
	for (Point p : ar.getPointList()) //for all the points of the artifact.
	{
		p = AddLengthByDirection(p, direction, LengthToAdd);
		temp = GetSignificantChar(direction, p, 1, 1);
		if (temp == TheMathGame::GameChars::ARTIFACT)
		{
			p = CalculateDestByDirection(p, direction);
			if (!ar.findPoint(p)) //if this is not the same artifact.
			{
				if (findArtifactByPoint(p).getLeader() == nullptr) //if this artifact hasn't leader, unuficate these artifacts.
				{
					ArtifactsUnification(ar, findArtifactByPoint(p));
					temp = TheMathGame::GameChars::WALL;
				}
				else //if this artifact has leader, return his sign for bomb it at artifact move.
				{
					temp = findArtifactByPoint(p).getLeader()->GetSign();
				}
			}
			else
				temp = TheMathGame::GameChars::FREE_PASS;
		}
		//if the artifact have a leader, and this is his sign, ignore it.
		if (ar.getLeader() != nullptr && ar.getLeader()->GetSign() == temp)
			temp = GameChars::FREE_PASS;

		SignificantChar = WhoIsMoreSignificant(SignificantChar, temp); //insert to SignificantChar the more significant char.
	}
	return SignificantChar;
}

//input: two chars.
//return: the more significant char.
char TheMathGame::WhoIsMoreSignificant(char ch1, char ch2)
{
	//The Significant order: Wall, BigSpaceShip, SmallSpaceShip, Artifact, Digit, FreePass.
	if (ch1 == TheMathGame::GameChars::WALL || ch2 == TheMathGame::GameChars::WALL)
		return TheMathGame::GameChars::WALL;
	else if (ch1 == TheMathGame::GameChars::BIG_SPACE_SHIP_SIGN || ch2 == TheMathGame::GameChars::BIG_SPACE_SHIP_SIGN)
		return TheMathGame::GameChars::BIG_SPACE_SHIP_SIGN;
	else if (ch1 == TheMathGame::GameChars::SMALL_SPACE_SHIP_SIGN || ch2 == TheMathGame::GameChars::SMALL_SPACE_SHIP_SIGN)
		return TheMathGame::GameChars::SMALL_SPACE_SHIP_SIGN;
	else if (ch1 == TheMathGame::GameChars::ARTIFACT || ch2 == TheMathGame::GameChars::ARTIFACT)
		return TheMathGame::GameChars::ARTIFACT;
	else if (ch1 == TheMathGame::GameChars::COL_FLYER || ch2 == TheMathGame::GameChars::COL_FLYER)
		return TheMathGame::GameChars::COL_FLYER;
	else if (ch1 == TheMathGame::GameChars::NUMBERS_EATER || ch2 == TheMathGame::GameChars::NUMBERS_EATER)
		return TheMathGame::GameChars::NUMBERS_EATER;
	else if (ch1 >= '0' && ch1 <= '9')
		return ch1;
	else if (ch2 >= '0' && ch2 <= '9')
		return ch2;
	else return TheMathGame::GameChars::FREE_PASS;
}

//this function get the significant char of the place that the artifact will go to after crossing the screen.
char TheMathGame::GetSignificantCharAfterCrossingScreen(int direction, Artifact& ar, int ObjWidth, int ObjHeight, int LengthToAdd)
{
	char SignificantChar = TheMathGame::GameChars::FREE_PASS, temp;
	for (Point p : ar.getPointList()) //for all the points of the artifact.
	{
		p = CalculateCrossingDestByDirection(p, direction, ObjWidth, ObjHeight, LengthToAdd);
		temp = board[p.getY()][p.getX()];  //insert to temp the char of this place.
		if (temp == TheMathGame::GameChars::ARTIFACT && ar.findPoint(p))
		{
			temp = TheMathGame::GameChars::FREE_PASS;
		}
		SignificantChar = WhoIsMoreSignificant(SignificantChar, temp); //insert to SignificantChar the more significant char.
	}
	return SignificantChar;
}

//input: pointer of the current spaceship.
//return: the position of the dragged arifact in ratio to the spaceship's start point.
int TheMathGame::GetPosOfDraggedAtrifact(SpaceShip* currentSpaceShip)
{
	//set p to the point above the start point.
	Point p(currentSpaceShip->GetStartPoint().getX(), currentSpaceShip->GetStartPoint().getY() - 1);

	//while this is an artifact, set p to one point left.
	while (board[p.getY()][p.getX()] == GameChars::ARTIFACT)
	{
		p.set(p.getX() - 1, p.getY());
	}

	//while this isn't an artifact, set p to one point right.
	while (board[p.getY()][p.getX()] != GameChars::ARTIFACT)
	{
		p.set(p.getX() + 1, p.getY());
	}

	//return the x of p minus the x of the spaceship's start point.
	return (p.getX() - currentSpaceShip->GetStartPoint().getX());
}

//Input: direction, points list and char for search.
//return: ture if the char is at the direction of this object, false otherwise.
bool TheMathGame::SearchForChar(int direction, list<Point> points, char searchFor)
{
	for (Point p : points) //for all the points.
	{
		p = CalculateDestByDirection(p, direction); //calculate their dest by the direction.
		if (board[p.getY()][p.getX()] == searchFor)
			return true;
	}
	return false;
}

//input: direction.
//return: the opposite direction.
int TheMathGame::SwapTheDirection(int direction)
{
	if (direction == Direction::UP) direction = Direction::DOWN;
	else if (direction == Direction::DOWN) direction = Direction::UP;
	else if (direction == Direction::RIGHT) direction = Direction::LEFT;
	else direction = Direction::RIGHT;

	return direction;
}

//input: pointer of spaceship.
//this function calculate the width and the height of this spaceship, and set them.
void TheMathGame::CalculateWidthAndHeightOfSpaceShip(SpaceShip* curr)
{
	int width = 0, height = 0;
	//while the char at (y,x+width) is the spaceship's sign, add 1 to width.
	while (board[curr->GetStartPoint().getY()][curr->GetStartPoint().getX() + width] == curr->GetSign())
	{
		width++;
	}
	//while the char at (y + height,x) is the spaceship's sign, add 1 to height.
	while (board[curr->GetStartPoint().getY() + height][curr->GetStartPoint().getX()] == curr->GetSign())
	{
		height++;
	}

	//set them.
	curr->setWidth(width);
	curr->setHeight(height);
}

//this function get name that not exist (at the saving directory) from user, and return the name as string.
string TheMathGame::getNameFromUser()
{
	string res;
	bool FirstTryFailed = false;
	
	cout << "Please enter the name of the file: \n";
	
	do	{
		if (FirstTryFailed) //if its not the first try notice and clear res.
		{
			cout << "This file name is exist, please choose another one\n";
			cout << "for back to sub menu please insert \"back\"" << endl;
			res.clear(); 
		}
		//make from the name that user insert the whole path.
		cin.clear();
		getline(cin, res); 
		res.append(".spp"); 
		res.insert(0, "\\"); 
		res.insert(0, getCurrentDirectoryOnWindows());
		if (!FirstTryFailed) FirstTryFailed = true; //if its the first time, insert true to FristTryFailed.
	}	while (ifstream(res, ios::out | ios::_Nocreate));
	
	return res;
}

//this function get solver name from user.
string TheMathGame::getSolverNameFromUser()
{
	string res;

	cout << "Please enter your name: \n";

	getline(cin, res);

	return res;
}

//this function is saving the game by name the user insert at the saving folder.
void TheMathGame::SaveGame()
{
	string NameOfSave;
	clear_screen();
	NameOfSave = getNameFromUser();

	if (NameOfSave.compare(CreateBackPath())) //when return "back" path from getNameFromUser the user cancel the saving.
	{
		ofstream outfile(NameOfSave, ios::out); //open out file.

		//write to the file the screen id
		outfile << "ScreenID=" << Gs.getScreenNumber() << endl;
		//write to the file the clock iterations
		outfile << "ClockIterations=" << Gs.getClockCycle() << endl;
		//write to the file the exercise.
		copyExerciseToFile(outfile);
		//write to the file the life
		outfile << "Life=" << Gs.getLives() << endl;
		//write to the file the board.
		copyBoardToFile(outfile);
		//write to the file the keys list
		copyKeysListToFile(outfile);
		
		outfile.close(); //close out file.
	}
}

//this function is for saving temp file of this game (fileName will be back.spp and we know that there isn't this file name)
//this function will use us for saving the game before user watch screen solution, for continue his game.
void TheMathGame::SaveGame(string fileName)
{
	ofstream outfile(fileName, ios::out); //open out file.

	//write to the file the screen id
	outfile << "ScreenID=" << Gs.getScreenNumber() << endl;
	//write to the file the clock iterations
	outfile << "ClockIterations=" << Gs.getClockCycle() << endl;
	//write to the file the exercise.
	copyExerciseToFile(outfile);
	//write to the file the life
	outfile << "Life=" << Gs.getLives() << endl;
	//write to the file the board.
	copyBoardToFile(outfile);
	//write to the file the keys list
	copyKeysListToFile(outfile);

	outfile.close(); //close out file.
}

//this function save solution.
void TheMathGame::SaveSolution()
{
	if (needToSaveSolution())
	{
		string NameOfSolver;
		clear_screen();
		NameOfSolver = getSolverNameFromUser();

		ofstream outfile(getCurrentDirectoryOnWindows() + "\\" + Gs.getStrScreenNumber() + ".spd", ios::trunc | ios::out); //open out file.
		
		//write to the file the screen id
		outfile << "ScreenID=" << Gs.getScreenNumber() << endl;
		//write to the file the clock iterations
		outfile << "ClockIterations=" << Gs.getClockCycle() << endl;
		//write to the file the clock iterations
		outfile << "NameOfSolver=" << NameOfSolver << endl;
		//write to the file the keys list
		copyKeysListToFile(outfile);

		outfile.close(); //close out file.
	}
}

//this function check if we need to save this solution.
bool TheMathGame::needToSaveSolution()
{
	list<TheMathGame::NamesByScreenID>::iterator iter = SolutionFilesName.begin();

	while (iter != SolutionFilesName.end())
	{
		if (iter->getScreenID() == Gs.getScreenNumber()) //if there is solution of this screen, return true if the new is better, false otherwise.
			return (iter->getClockCycle(iter->name) > Gs.getClockCycle());
		iter++;
	}

	//if thereisn't solution of this screen, return true.
	return true;
}

//input: referance of out file.
//this function copy board to this file.
void TheMathGame::copyBoardToFile(ofstream& outfile) {
	for (int i = 0; i<ROWS; ++i) {
		for (int j = 0; j<COLS; ++j) {
			outfile.put(board[i][j]);
			outfile.flush();
		}
		outfile << endl;
		outfile.flush();
	}
}

//input: referance of out file.
//this function copy the keys list to this file.
void TheMathGame::copyKeysListToFile(ofstream& outfile)
{
	for (KeysByClockIteration k : keysList)
	{
		k._OutToFile(outfile); 
	}
}

//input: reference to out file.
//this function copy the exercise to this file.
void TheMathGame::copyExerciseToFile(ofstream& outfile)
{
	outfile << "Exercise=";
	Ex.CopyExerciseToFile(outfile);
	outfile << endl;
}

//this function create and return the "back" file path.
string TheMathGame::CreateBackPath()
{
	string res;
	res.append(getCurrentDirectoryOnWindows());
	res.append("\\");
	res.append("back.spp");
	return res;
}

//input: board file name.
//return: true if the file is vaild, false otherwise.
bool TheMathGame::TheBoardChecker(string boardFileName, bool& isSppFile, bool printExceptions)
{
	set<string> Exceptions; //set of all the exceptions.
	ifstream inFile(boardFileName, ios::in);
	bool res = true;
	string lineBuffer;
	isSppFile = false;

	if (!inFile.is_open()) //if the file isn't exist, add to exceptions and return false.
	{
		Exceptions.insert("FILE NOT EXIST!");
		res = false;
	}
	else if (res = CheckEndingOfFileName(boardFileName.c_str(), isSppFile, Exceptions)) //if the ending of the file is valid
	{
		//check the ScreenID.
		res &= CheckIfScreenIDExist(inFile, Exceptions, boardFileName.c_str(), isSppFile);
			
		if(isSppFile) //id the file is spp file, we have to skip over the Clock Iterations title
			getline(inFile, lineBuffer);

		//if the screen id is valid, check exercise.
		res &= CheckExercise(inFile, Exceptions);
			
		if (isSppFile) //id the file is spp file, we have to skip over the Life title
			getline(inFile, lineBuffer);

		//if the exercise is valid, get board from file and check if is valid.
	
		char tempBoard[ROWS][COLS + 1];
		res &= GetBoardFromFile(inFile, tempBoard, Exceptions);
		res &= BoardChecker(tempBoard, Exceptions);
	}

	if (printExceptions) PrintExceptions(Exceptions, res);

	inFile.close();

	return res;
}

//input: board and set of exceptions.
//return: true if this board is valid, false otherwise.
bool TheMathGame::BoardChecker(char boardToCheck[][COLS + 1], set<string> &Exceptions)
{
	bool ValidBoard = true, ThereIsInvalidNumber=false;
	bool isSmallShipChecked = false, isBigShipChecked = false;
	bool isExerciseExist = false, isGameStateExist = false;
	int DigitCounter = 0;
	char ch;

	//for all the chars at board.
	for (int i = 0; i < ROWS; i++) 
		for (int j = 0; j <= COLS; j++)
		{
			ch = boardToCheck[i][j];
			switch(ch)
			{
				case GameChars::BIG_SPACE_SHIP_SIGN: 
					//if this char is the big spaceship's char, check big spaceship validation.
					if (!isBigShipChecked)
					{
						bool tempValidBoard = CheckBigSpaceShip(boardToCheck, i, j);
						ValidBoard &= tempValidBoard;

						if(!tempValidBoard)
							Exceptions.insert("BIG SPACE SHIP ERROR!");

						isBigShipChecked = true;
					}
					else
					//if there is another big spaceship's char this isn't legal.
					// (because we are deleting the chars at the first time).
					{  
						ValidBoard = false;
						Exceptions.insert("BIG SPACE SHIP ERROR!");
					}
					break;

				case GameChars::SMALL_SPACE_SHIP_SIGN:
					//if this char is the small spaceship's char, check big spaceship validation.
					if (!isSmallShipChecked)
					{
						bool tempValidBoard = CheckSmallSpaceShip(boardToCheck, i, j);
						ValidBoard &= tempValidBoard;

						if (!tempValidBoard)
							Exceptions.insert("SMALL SPACE SHIP ERROR!");

						isSmallShipChecked = true;
					}
					//if there is another small spaceship's char this isn't legal.
					// (because we are deleting the chars at the first time).
					else
					{
						ValidBoard = false;
						Exceptions.insert("SMALL SPACE SHIP ERROR!");
					}
					break;

				case 'X':
					//if there is more than one Exercise sign this is an error.
					if (!isExerciseExist)
						isExerciseExist = true;
					else
					{
						ValidBoard = false;
						Exceptions.insert("EXERCISE ERROR!");
					}
					break;

				case 'S':
					//if there is more than one Game Stats sign this is an error.
					if (!isGameStateExist)
						isGameStateExist = true;
					else
					{
						ValidBoard = false;
						Exceptions.insert("GAME STATE ERROR!");
					}
					break;
			}
			if (!ThereIsInvalidNumber) //if we didn't find invalid number yet.
			{
				CheckInvalidNumber(ch, DigitCounter, ThereIsInvalidNumber);
			}
		}
	//if we didn't checked spaceship, add error and put false in validboard
	if (!isBigShipChecked)
	{
		Exceptions.insert("BIG SPACE SHIP ERROR!");
		ValidBoard = false;
	}
	if (!isSmallShipChecked)
	{
		Exceptions.insert("SMALL SPACE SHIP ERROR!");
		ValidBoard = false;
	}
	if (!isExerciseExist)
	{
		Exceptions.insert("EXERCISE ERROR!");
		ValidBoard = false;
	}
	if (!isGameStateExist)
	{
		Exceptions.insert("GAME STATE ERROR!");
		ValidBoard = false;
	}
	if (ThereIsInvalidNumber)
	{
		Exceptions.insert("THERE IS NUMBER AT BOARD THAT HAS MORE THAN 5 DIGITS!");
		ValidBoard = false;
	}
	return ValidBoard;
}

//input: board, and the position of the small spaceship's sign.
//return: true if the big spaceship's is in the right sizes.
bool TheMathGame::CheckBigSpaceShip(char boardToCheck[][COLS + 1], int row, int col)
{
	bool ValidSpaceShip = true;
	bool ValidSpaceShipVertical = true, ValidSpaceShipHorizontal = true;

	char ch = GameChars::BIG_SPACE_SHIP_SIGN;
	for (int i = 0; i < 3; i++) //checking if the spaceship is horizontal or vertical
		for (int j = 0; j < 2; j++)
		{
			if (boardToCheck[row + i][col + j] != ch)
				ValidSpaceShipVertical = false;
				
			if (boardToCheck[row + j][col + i] != ch)
				ValidSpaceShipHorizontal = false;
		}
	//if the spaceship is valid after this up writen check, put true in the ValidSpaceShip bool
	ValidSpaceShip = ValidSpaceShipVertical || ValidSpaceShipHorizontal; 

	//in case of valid space ship, delete the spaceship from the board to check by put space in the right places
	if (ValidSpaceShip) 
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 2; j++)
			{
				if (ValidSpaceShipVertical)
					boardToCheck[row + i][col + j] = GameChars::FREE_PASS;
				else if (ValidSpaceShipHorizontal)
					boardToCheck[row + j][col + i] = GameChars::FREE_PASS;
			}
	}

	return ValidSpaceShip;
}

//input: board, and the position of the small spaceship's sign.
//return: true if the small spaceship is in the right sizes.
bool TheMathGame::CheckSmallSpaceShip(char boardToCheck[][COLS + 1], int row, int col)
{
	bool ValidSpaceShip = true;
	bool ValidSpaceShipVertical = true, ValidSpaceShipHorizontal = true;

	char ch = GameChars::SMALL_SPACE_SHIP_SIGN;
	for (int i = 0; i < 3; i++) //checking in the space ship is horizontal or vertical
		for (int j = 0; j < 1; j++)
		{
			if (boardToCheck[row + i][col + j] != ch)
				ValidSpaceShipVertical = false;
			if (boardToCheck[row + j][col + i] != ch)
				ValidSpaceShipHorizontal = false;
		}

	//if the spaceship is valid after this up writen check, put true in the ValidSpaceShip bool
	ValidSpaceShip = ValidSpaceShipVertical || ValidSpaceShipHorizontal; 

	//in case of valid space ship, delete the spaceship from the board to check by put space in the right places
	if (ValidSpaceShip) 
	{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 1; j++)
		{
			if (ValidSpaceShipVertical)
				boardToCheck[row + i][col + j] = GameChars::FREE_PASS;
			else if (ValidSpaceShipHorizontal)
				boardToCheck[row + j][col + i] = GameChars::FREE_PASS;
		}
	}

	return ValidSpaceShip;
}

//input: file name, bool by ref isSppFile, set of exceptions.
//return: ture if the ending file name is spp or spg, false otherwise.
bool TheMathGame::CheckEndingOfFileName(const char* FileName, bool &isSppFile, set<string>& Exceptions)
{
	int strLength = strlen(FileName);
	if (strLength >= 5) //Check the ending only if there is enough chars in the string
	{
		char temp[5];
		int j = 0;
		for (int i = strLength - 4; i < strLength; i++) //get the last 4 chars of the string
			temp[j++] = FileName[i];
		temp[4] = '\0';
		if (!strcmp(temp, ".spp") || !strcmp(temp, ".spg")) //if the format is spp or spg return true
		{
			if (!strcmp(temp, ".spp")) //if the format is spp, isSppFile is true.
				isSppFile = true;
			return true;
		}
			
	}
	Exceptions.insert("ENDING DONT MATCH!");
	return false; //return false if the format isnt spp or spg
}

//input: reading file, set of exceptions, file name and preintExceptions.
//return: true if the screen id exist, false otherwise.

bool TheMathGame::CheckIfScreenIDExist(ifstream& inFile, set<string>& Exceptions, const char* FileName, bool isSppFile)
{
	string lineRead, theScreenID(MAX_LINE_LEN, ' ');
	size_t ScreenIDNumber;

	//Get from the file and set the screen id
	getline(inFile, lineRead);

	//Get the sub strings and the numbers
	sscanf_s(lineRead.c_str(), "%9s%d", &theScreenID[0], MAX_LINE_LEN, &ScreenIDNumber); 

	//Checking if the screenid line is valid
	if (strcmp(theScreenID.c_str(), "ScreenID=") || ScreenIDNumber <= 0)
	{
		Exceptions.insert("SCREEN ID ERROR!");
		return false; //if the screenid isnt ok, return false
	}
	//if the screenid is ok and we need to print exceptions, check if it unique and return true
	else if (!isSppFile) //if the file name isn't saved game, check if the screenID is unique.
	{
		return CheckIfScreenIDisUnique(ScreenIDNumber, Exceptions, FileName);
	}

	return true; //if the screenid is ok return true
}

//input: reading file, set of exceptions.
//return: true if the exercise is valid, false otherwise.
bool TheMathGame::CheckExercise(ifstream& inFile, set<string>& Exceptions)
{
	string lineRead, exerciseTitle(MAX_LINE_LEN, ' '), equation(MAX_LINE_LEN, ' ');

	//Get from the filethe exercise line
	getline(inFile, lineRead);
	sscanf_s(lineRead.c_str(), "%9s%s", &exerciseTitle[0], MAX_LINE_LEN, &equation[0], MAX_LINE_LEN); //Get the sub strings

	if (strcmp(exerciseTitle.c_str(), "Exercise=")) //Checking if the exercise line title is valid
	{
		Exceptions.insert("EXERCISE ERROR!");
		return false; //if the exercise isnt ok, return false
	}
	else
	{
		//if the exercise title if valid, check if the equation is valid
		bool res = CheckEquationValidation(equation, Exceptions);
		if (!res) Exceptions.insert("EXERCISE ERROR!");
		return res;
	}
}

//input: char.
//return: true if this char is an digit, false otherwise.
bool TheMathGame::CheckIfDigit(char ch)
{
	if (ch >= '0' && ch <= '9')
		return true;
	else
		return false;
}

//input: line, index, isValid.
//this function check if the index at this line is a number or an unkown parameter sign.
void TheMathGame::CheckIfNumberOrUnkown(const string& line, size_t& index, bool& isValid, set<string>& Exceptions)
{
	char ch = line[index++];
	int DigitsCounter = 0;
	//if this char is a digit or an under score and this equalition is valid so far
	if ((CheckIfDigit(ch) || ch == '_') && isValid)
	{
		if (ch != '_') //if this isn't unkown paramter's sign.
		{
			DigitsCounter = 0;
			//while this is a digit, check the next char.
			while (ch != '\0' && CheckIfDigit(ch))
			{
				ch = line[index++];
				DigitsCounter++;
			}
			if (DigitsCounter > 5)
			{
				isValid = false;
				Exceptions.insert("THERE IS NUMBER THAT HAS MORE THAN 5 DIGITS AT EQUATION!");
			}
			index--; //back to the first index after this number.
		}
		
	}
	else
		isValid = false;
}

//input: equation string.
//return: ture if this eqution is valid, false otherwise.
bool TheMathGame::CheckEquationValidation(const string& equation, set<string>& Exceptions)
{
	bool ValidExercise = true;
	size_t index = 0;
	char ch;

	//First group of exercise check: if this is a number or an unkown parameter.
	CheckIfNumberOrUnkown(equation, index, ValidExercise, Exceptions);

	//Second group of exeecise check: if this is an operator.
	ch = equation[index++];
	if (ch != '+' && ch != '-' && ch != '/' && ch != '*')
		ValidExercise = false;

	//Third group of exeecise check: if this is a number or an unkown parameter.
	if (ValidExercise) CheckIfNumberOrUnkown(equation, index, ValidExercise, Exceptions);

	//Fourth group of exercise check: if this is an equal sign.
	ch = equation[index++];
	if (ch != '=' && ValidExercise)
		ValidExercise = false;

	//Fifth group of exeecise check: if this is a number or an unkown parameter.
	if (ValidExercise) CheckIfNumberOrUnkown(equation, index, ValidExercise, Exceptions);

	return ValidExercise;
}

//input: file, board.
//this function get board from this file.
void TheMathGame::GetBoardFromFile(ifstream& file, char board[][COLS + 1])
{
	char ch;
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j <= COLS; j++) {
			if(!file.eof())
				ch = file.get();
			else break; //don't need to continue this for if there isn't more chars.
			if (ch != '\n') //if this is not the end of this line.
			{
				board[i][j] = ch; //add this char to board
			}
			else
			{
				while (j < COLS) //in this case we don't have enough chars in this line.
				{
					board[i][j] = GameChars::FREE_PASS; //add free pass.
					j++;
				}
			}
		}
		board[i][COLS] = '\0';
	}
}

//input: file, board.
//this function get board from this file and add exception if there is a resolution error.
bool TheMathGame::GetBoardFromFile(ifstream& file, char board[][COLS + 1], set<string>& Exceptions)
{
	char ch;

	// for all the chars we need to take.
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j <= COLS; j++) {
			//if this isn't the end of the file get the first char.
			if (!file.eof())
				ch = file.get();
			else //in this case we don't have enough lines.
			{
				Exceptions.insert("BOARD HEIGHT ERROR!");
				return false;
			}

			if (ch != '\n') //if this is not the end of this line.
			{
				board[i][j] = ch; //add this char to board
			}
			else
			{
				while (j < COLS) //in this case we don't have enough chars in this line.
				{
					board[i][j] = GameChars::FREE_PASS; //add free pass.
					j++;
				}
			}
		}
		board[i][COLS] = '\0';
	}
	return true;
}

//input: set of exceptions, bool there isn't exceptions.
void TheMathGame::PrintExceptions(const set<string>& Exceptions, bool ThereIsntExceptions)
{
	if (!ThereIsntExceptions) //if there is exceptions, print them.
	{
		setTextColor(Color::LIGHTRED);
		cout << "The exceptions are:" << endl;

		setTextColor(Color::WHITE);
		for (const string& exception : Exceptions)
			cout << exception << endl;
	}

	else //else note that this file is valid.
	{
		setTextColor(Color::LIGHTGREEN);
		cout << "The File is Valid\n";
	}

	setTextColor(Color::LIGHTRED);
	cout << endl << "Press any key to continue..." << endl;

	_getch();
	clear_screen();
	setTextColor(Color::WHITE);
}

//this function update all the files list.
void TheMathGame::UpdateFilesLists()
{
	BoardFilesName = UpdateFilesList(getCurrentDirectoryOnWindows(), "spg");
	SavedFilesName = UpdateFilesList(getCurrentDirectoryOnWindows(), "spp");
	SolutionFilesName = UpdateFilesList(getCurrentDirectoryOnWindows(), "spd");
}

//input: folder name and Ending of files.
//return: list of all the names of the files with these ending in this folder.
//source: http://stackoverflow.com/a/20847429/4279201
list<TheMathGame::NamesByScreenID> TheMathGame::UpdateFilesList(string folder, char* Ending)
{
	list<TheMathGame::NamesByScreenID> res;
	char search_path[MAX_NAME_LEN];
	snprintf(search_path, MAX_NAME_LEN, "%s/*.%s", folder.c_str(), Ending);
	res.clear();
	
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path, &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				//push this file and his screenID to the list.
				res.push_back((string)fd.cFileName);
				res.back().setScreenID(GetScreenIDFromFile((string)fd.cFileName));
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}

	//sort the list by the screenIDs
	res.sort([](const TheMathGame::NamesByScreenID & a, const TheMathGame::NamesByScreenID & b) { return a.getScreenID() < b.getScreenID(); });
	return res;
}

//input: file name.
//return: the screen_id of this number.
size_t TheMathGame::GetScreenIDFromFile(const string& fileName)
{
	ifstream inFile(fileName, ios::in);
	string lineRead, theScreenID(MAX_LINE_LEN, ' ');
	size_t ScreenIDNumber;

	//Get from the file the screenID line.
	getline(inFile, lineRead);

	//Get the sub string and the number
	sscanf_s(lineRead.c_str(), "%9s%d", &theScreenID[0], MAX_LINE_LEN, &ScreenIDNumber);

	inFile.close();

	return ScreenIDNumber;
}

//input: screenID, set of exceptions.
//this function check if this screenID is unique.
bool TheMathGame::CheckIfScreenIDisUnique(const size_t& ScreenIDNumber, set<string>& Exceptions, const char* FileName)
{
	for (const TheMathGame::NamesByScreenID& file_name : BoardFilesName)
	{
		//if the files have the same screen id and different name
		if (file_name.screenID == ScreenIDNumber && strcmp(file_name.name.c_str(), FileName))
		{
			//create the error line and print it.
			char temp[MAX_LINE_LEN] = "THIS SCREEN_ID ISN'T UNIQUE, ";
			strcat_s(temp, MAX_NAME_LEN, file_name.name.c_str());
			strcat_s(temp, MAX_LINE_LEN, " HAS THE SAME ONE");
			Exceptions.insert(temp);
			
			return false; //1 ununique file is enough.
		}
	}
	return true;
}

//this functionn upload saved game by user selection.
size_t TheMathGame::UploadSavedGame()
{
	list<TheMathGame::NamesByScreenID>::iterator it = SavedFilesName.begin(); //for running on the list.
 	size_t user; //for user choice.
	if (SavedFilesName.size() > 0) //if there is saved games.
	{
		//print saved games options and get the user choice.
		user = PrintSavedGamesList();
	
		if (user > 0) //if the user chose any saved game.
		{
			//go to this saved level.
			it = SavedFilesName.begin();
			for (size_t i = 0; i < (user - 1); i++)
				it++;

			//get current level.
			currentLevel = 0;
			list<TheMathGame::NamesByScreenID>::iterator it2 = BoardFilesName.begin();
			//find the first original level with the same screenID.
			for (int i = 0; it2->screenID != it->screenID; i++)
			{
				it2++;
				currentLevel++;
			}

			//set board and total levels.
			setBoard(it->name);
			setTotalLevels(BoardFilesName.size());
			initBoard();
			currentLevel++;
			PlayOpeningSound();
		}
	}
	else //if there isn't saved game, notice and back to main menu.
	{
		clear_screen();
		cout << "THERE ISN'T SAVED GAMES\nplease press any key for back to the main menu...";
		_getch();
		user = 0;
	}
	return user;
}

//this function print list of the saved game and return the choice of user.
size_t TheMathGame::PrintSavedGamesList()
{
	clear_screen();
	list<TheMathGame::NamesByScreenID>::iterator it = SavedFilesName.begin();
	size_t user;
	bool isFirstTime = true;
	//print options:
	for (int i = 0; it != SavedFilesName.end(); i++)
	{
		cout << (i + 1) << ". " << it->name << endl;
		it++;
	}
	do
	{
		if (isFirstTime) cout << "Please Insert the number of the saved game: \n";
		else cout << "Please Insert Valid number or 0 for return to main menu: \n";
		cin >> user;
		isFirstTime = false;
	} while ((user) < 0 || (user) > SavedFilesName.size()); //while this isn't valid choose.
	cin.clear();	cin.ignore(INT_MAX, '\n'); //clean the cin buffer.
	return user;
}

//this function print list of the screens and return the choice of user.
size_t TheMathGame::PrintScreensList()
{
	clear_screen();
	list<TheMathGame::NamesByScreenID>::iterator it = BoardFilesName.begin(); //iterator for screens list
	list<TheMathGame::NamesByScreenID>::iterator it2 = SolutionFilesName.begin(); //iterator of solutions list.
	size_t user;
	size_t SolutionFilesNameIndex = 0; //for index of the solution files name.
	bool isFirstTime = true;
	
	//print options:
	for (int i = 0; it != BoardFilesName.end(); i++)
	{
		cout << (i + 1) << ". " << it->name;
		if (SolutionFilesNameIndex < SolutionFilesName.size()) //if there is still solutions
		{
			if ((it->screenID == it2->screenID)) //if the screenID of this board and the next solution is equal
			{
				cout << "\t\t there is solution of this screen";
				it2++;
				SolutionFilesNameIndex++;
			}
		}
		cout << endl;
		it++;
	}
	do
	{
		if (isFirstTime) cout << "Please Insert the number of the screen: \n";
		else cout << "Please Insert Valid number or 0 for return to main menu: \n";
		cin >> user;
		isFirstTime = false;
	} while ((user) < 0 || (user) > BoardFilesName.size()); //while this isn't valid choose.
	
	cin.clear();	cin.ignore(INT_MAX, '\n'); //clean the cin buffer.
	return user;
}

//input: file name
//return: the clockcycle of this solution file (spd).
size_t TheMathGame::NamesByScreenID::getClockCycle(string fileName)
{
	size_t res;
	ifstream inFile(fileName, ios::in);
	string lineBuffer;

	//skip the first tow lines.
	getline(inFile, lineBuffer); //get the first line of file
	getline(inFile, lineBuffer); //get the second line of file

	sscanf_s(lineBuffer.c_str(), "ClockIterations=%u", &res);

	return res;
}

//this function play solution of this current level if exist and return to main menu when finish.
void TheMathGame::PlaySolution()
{
	if(CheckIfThereIsSolutionOfThisScreen()) //if there isn't any key, there isn't solution.
	{
		//we know that there isn't file with this name (because we don't allow it)
		//so we will not run over existing save file
		SaveGame("back.spp");

		int tempSleepTime = sleepTime; //save the temp sleep time.
		sleepTime *= SLOW_MOTION_FACTOR; //update sleep time for slowing motion. 

		startLevel(true); //start level
		getKeysListFromSolutionFile(); //get the Keys list of this solution file.

		//while the equation isn't solved and the user doesn't stopped the solution(when sleepTime is zero), do iteration.
		while (!IsEquationSolved && sleepTime > 0) 
		{
			ManageKeyboardHitsAtSolutionPlay(sleepTime);
			doIteration();
		}

		bool NoteBackingToSubMenu; //if the user stopped the solutoin don't note for backing to sub menu.
		(sleepTime > 0) ? NoteBackingToSubMenu = true : NoteBackingToSubMenu = false;

		//return all the settings to the situation before solutin played
		sleepTime = tempSleepTime; //return sleep time to the original value.
		IsEquationSolved = false;
		setBoard("back.spp"); //set the temp board.
		remove("back.spp"); //remove the temp board file.
		initBoard(false); //init board (send false for init without printing)
		
		if (NoteBackingToSubMenu) //note for backing to sub menu if the solution play is over
		{
			clear_screen();
			cout << "THE SOLUTION PLAY IS OVER..." << endl;
			cout << "YOU ARE REDIRECTING TO THE SUB MENU..." << endl;
			cout << "YOU CAN COUNTINUE YOUR GAME FROM THE SAME SITUATION YOU STOPPED IT!" << endl;
			SleepAndPrintCounterSeconds(AFTER_SOLUTION_SLEEP_DURATION);
		}
	}
	else //print note and return to the main menu.
	{
		clear_screen();
		cout << "THERE ISN'T SOLUTION OF THIS LEVEL..." << endl;
		cout << "YOU ARE REDIRECTING TO THE SUB MENU..." << endl;
		cout << "YOU CAN COUNTINUE YOUR GAME FROM THE SAME SITUATION YOU STOPPED IT!" << endl;
		SleepAndPrintCounterSeconds(AFTER_SOLUTION_SLEEP_DURATION);
	}
}

//this function get key list from solution file.
void TheMathGame::getKeysListFromSolutionFile()
{
	string lineRead;

	ifstream infile(getCurrentDirectoryOnWindows() + "\\" + Gs.getStrScreenNumber() + ".spd", ios::in); //open file to read
	
	//skip first 3 lines.
	getline(infile, lineRead); //get the first line from the file
	getline(infile, lineRead); //get the second line from the file
	getline(infile, lineRead); //get the third line from the file

	getKeysListFromFile(infile); //get keysList from this file.
}

//this function update the current level of the selected screen that the user want to start play
//return: true if the user want to play some screen, false otherwise.
bool TheMathGame::StartGameFromSelectedScreen()
{
	currentLevel = (int)(PrintScreensList() - 1); //get the current level of the screen.
	
	//if current level is -1, the user want back to the main menu, so current Level is 0 and return false.
	if (currentLevel == -1) 
	{
		currentLevel = 0;
		return false;
	}
	else
		return true;
}

//this funcion check if there is solution of this current level screen.
//return: true if there is, false otherwise.
bool TheMathGame::CheckIfThereIsSolutionOfThisScreen()
{
	list<TheMathGame::NamesByScreenID>::iterator it = BoardFilesName.begin(); //iterator for screens list.
	list<TheMathGame::NamesByScreenID>::iterator it2 = SolutionFilesName.begin(); //iterator for solutions list.

	for (int i = 0; i < (currentLevel-1); i++) //put the iterator on the current level screen.
	{
		it++;
	}

	for (size_t i = 0; i < SolutionFilesName.size(); i++) //check if there is solution of this screen
	{
		if (it->screenID == it2->screenID) //if there is return true.
			return true;
		it2++;
	}
	return false;
}

//input: infile at the first line of keys list.
//this function get this file's keysList.
void TheMathGame::getKeysListFromFile(ifstream& infile)
{
	string lineRead;
	size_t clockCycle;
	char ch1, ch2;

	keysList.clear(); //clear the keysList.

	while (getline(infile, lineRead)) //while there is lines
	{
		//scanning from line the clock cycle and keys.
		sscanf_s(lineRead.c_str(), "%d: %c%c", &clockCycle, &ch1, 1, &ch2, 1);

		if (ch2 >= 'a' && ch2 <= 'z') //for case that there is just 1 key.
			keysList.push_back(KeysByClockIteration(clockCycle, ch1, ch2));
		else
			keysList.push_back(KeysByClockIteration(clockCycle, ch1));
		
		//zero ch1 and ch2 for the next loop
		ch1 = '\0';
		ch2 = '\0';
	}
}

void TheMathGame::startLevelAfterBomb() {
	Gs.LessLife(); //less one life.
	initBoard(); //init board for starting the level again.
	StartedAfterBomb = true;
}

void TheMathGame::startLevelAfterEquationCantBeSolved() {
	Gs.LessLife(); //less one life.
	Ex.RestartExercise();
	initBoard(); //init board for starting the level again.
}

bool TheMathGame::isGameOver()
{
	if (Gs.GetLives() <= 0 || Gs.GetLives() >= 4) // >=4 for if somehow there is lives problem.
		return true;
	else
		return false;
}

bool TheMathGame::hasNextLevel()const
{
	if (currentLevel < TotalLevels)
		return true;
	else
	{
		return false;
	}
}

//input: sleep time pre mil seconds.
//this function is printing the seconds counter with the Sleep of the program. 
void TheMathGame::SleepAndPrintCounterSeconds(int SleepTimePerMilSeconds)
{
	cout << "MORE: ";
	for (int i = SleepTimePerMilSeconds / MIL_SECONDS_PER_SECOND; i > 0; i--)
	{
		cout << "\b" << i;
		Sleep(MIL_SECONDS_PER_SECOND);
	}
}

//input: char, DigitCounter by ref, ThereIsInvalidNumber by ref.
//this function check:
//if this char is digit: check if this digit is own to invalid number (more than 5 digits).
//if this char isn't digit: zero DigitCounter.
void TheMathGame::CheckInvalidNumber(char ch, int& DigitCounter, bool& ThereIsInvalidNumber)
{
	//if this char isn't digit and the digit count is more than zero, zero the counter.
	if (!CheckIfDigit(ch) && DigitCounter > 0)
		DigitCounter = 0;
	//if this char is a digit, count it.
	if (CheckIfDigit(ch))
		DigitCounter++;
	//if there is more than 5 digits at row change ThereIsInvaliddNumber to true.
	if (DigitCounter > 5)
	{
		ThereIsInvalidNumber = true;
	}
}

//input: need to print or not.
//this function init the artifacts list.
void TheMathGame::initCreaturesList(bool WithPrinting)
{
	//clear the list and copy the original list.
	CopyCreaturesList();

	//init all the artifacts.
	for (Creature* cr : Creatures)
	{
		cr->init(WithPrinting);
	}
}

//this function print all the Creatures.
void TheMathGame::_printCreatures()
{
	for (Creature* tempCr : Creatures)
		tempCr->print();
}

//this function move all the creatures
void TheMathGame::MoveCreatures()
{
	list<Creature*>::iterator NextIt;
	NumbersEater* temp;

	for (list<Creature*>::iterator it = Creatures.begin(); it != Creatures.end();)
		{
			//we have to save temp next iterator for case that this creature is Flyer and it died at this loop.
			NextIt = next(it, 1);
			
			//to know if this creature is a numbersEater.
			temp = dynamic_cast <NumbersEater*> (*it);

			if ((*it)->move())
			{
				//if the movment succeeded (the creature didn't die)
				//if this creature was an artifact and it bombed spaceship, break this loop
				//because we have new Creature list now.
				if (StartedAfterBomb)
				{
					StartedAfterBomb = false;
					break;
				}
				else it++;
			}
			else
			{
				//if this creature is a numbersEater and the movment isn't succeded, this numbersEater died, use NextIt.
				if (temp)
				{
					it = NextIt;
				}
				else it++;
			}
		}
}

//input: point, artifact by ref, temp Board.
//this function add to the artifact's list all the artifact's point around the main point.
void TheMathGame::SetArtifact(const Point &point, Artifact &artifact, char tempBoard[][COLS + 1])
{
	if (tempBoard[point.getY()][point.getX()] != GameChars::ARTIFACT)
		return;

	artifact.AddPoint(point);
	tempBoard[point.getY()][point.getX()] = GameChars::FREE_PASS;

	SetArtifact(Point(point.getX(), point.getY() - 1), artifact, tempBoard);
	SetArtifact(Point(point.getX(), point.getY() + 1), artifact, tempBoard);
	SetArtifact(Point(point.getX() - 1, point.getY()), artifact, tempBoard);
	SetArtifact(Point(point.getX() + 1, point.getY()), artifact, tempBoard);
}

//input: point.
//return: the artifact of this point.
Artifact& TheMathGame::findArtifactByPoint(const Point& point)
{
	//for return the last Artifact if we didn't find any artifact that has this point.
	//(sholdn't happen beacuse we know that we have one artifact at least if we are in this function).
	Artifact* lastAr = nullptr;
	Artifact* tempAr;
	for (Creature* tempCreature : Creatures) //for all the creatures
	{
		tempAr = dynamic_cast<Artifact*> (tempCreature);
		if (tempAr) //if this creature is an artifact
		{
			lastAr = tempAr;
			if ((*tempAr).findPoint(point)) //if this point is of this artifact, return it.
				return *tempAr;
		}
	}
	return *lastAr;
}

//input: 2 artifacts by ref.
//this function unification these artifacts.
void TheMathGame::ArtifactsUnification(Artifact& first, Artifact& second)
{
	//if at least one of them hasn't leader, or they have the same leader.
	if ((first.getLeader() == nullptr || second.getLeader() == nullptr || (first.getLeader() != nullptr && (first.getLeader() == second.getLeader()))))// ))
	{
		//insert all the points of second to first
		first.getPointList().insert(first.getPointList().end(), second.getPointList().begin(), second.getPointList().end());
		first.CalculateWidthAndHeight();

		//remove seconds from the artifacts list
		CreaturesToFree.push_back(&second);
		Creatures.remove(&second);
	}
}

//input: spaceship.
//return: the nearest artifact.
Artifact& TheMathGame::getNearestArtifact(const SpaceShip& s)
{
	Point ObjStart = s.GetStartPoint();
	switch (s.GetDirectionOfSpaceship())
	{
	case Direction::UP:
		for (int i = 0; i < s.getWidth(); i++) //check all the chars at the line above the object.
		{
			if (board[ObjStart.getY() - 1][ObjStart.getX()] == GameChars::ARTIFACT)
				return findArtifactByPoint(Point(ObjStart.getX(), ObjStart.getY() - 1));
			else ObjStart.set(ObjStart.getX() + 1, ObjStart.getY());
		}
		break;
	case Direction::DOWN:
		ObjStart.set(ObjStart.getX(), ObjStart.getY() + (s.getHeight() - 1));
		for (int i = 0; i < s.getWidth(); i++) //check all the chars at the line under the object.
		{
			if (board[ObjStart.getY() + 1][ObjStart.getX()] == GameChars::ARTIFACT)
				return findArtifactByPoint(Point(ObjStart.getX(), ObjStart.getY() + 1));
			else ObjStart.set(ObjStart.getX() + 1, ObjStart.getY());
		}
		break;
	case Direction::LEFT:
		for (int i = 0; i < s.getHeight(); i++) //check all the chars at the column that left to the object.
		{
			if (board[ObjStart.getY()][ObjStart.getX() - 1] == GameChars::ARTIFACT)
				return findArtifactByPoint(Point(ObjStart.getX() - 1, ObjStart.getY()));
			else ObjStart.set(ObjStart.getX(), ObjStart.getY() + 1);
		}
		break;
	case Direction::RIGHT:
		ObjStart.set(ObjStart.getX() + (s.getWidth() - 1), ObjStart.getY());
		for (int i = 0; i < s.getHeight(); i++) //check all the chars at the column that right to the object
		{
			if (board[ObjStart.getY()][ObjStart.getX() + 1] == GameChars::ARTIFACT)
				return findArtifactByPoint(Point(ObjStart.getX() + 1, ObjStart.getY()));
			else ObjStart.set(ObjStart.getX(), ObjStart.getY() + 1);
		}
		break;
	}
	return getLastArtifact(); //we shouldn't get here just for all control path return a value.
}

//input: spaceship.
//return: the artifact from the other side of the screen.
Artifact& TheMathGame::getCroosedArtifact(const SpaceShip& s)
{
	Point ObjStart = s.GetStartPoint();
	//change the local start point of this spaceship to the other side of the board by direction.
	switch (s.GetDirectionOfSpaceship())
	{
	case Direction::UP:
		ObjStart.set(ObjStart.getX(), Resolution::HEIGHT_END - s.getHeight() + 1);
		break;
	case Direction::DOWN:
		ObjStart.set(ObjStart.getX(), Resolution::HEIGHT_BEGINNIG);
		break;
	case Direction::RIGHT:
		ObjStart.set(Resolution::WIDTH_BEGINNIG, ObjStart.getY());
		break;
	case Direction::LEFT:
		ObjStart.set(Resolution::WIDTH_END - s.getWidth() + 1, ObjStart.getY());
		break;
	}

	//check all the chars of space ship at this place and return the artifact.
	for (int i = 0; i < s.getHeight(); i++) 
	{
		for (int j = 0; j < s.getWidth(); j++)
		{
			if (board[ObjStart.getY() + i][ObjStart.getX() + j] == GameChars::ARTIFACT)
				return findArtifactByPoint(Point(ObjStart.getX() + j, ObjStart.getY() + i));
		}
	}
	return getLastArtifact(); //we shouldn't get here just for all control path return a value.
}

//input: spaceship.
//return: the artifact that above this spaceship (we know that there is one at least)
Artifact& TheMathGame::getAboveArtifact(const SpaceShip& s)
{
	Point ObjStart = s.GetStartPoint();
	
	for (int i = 0; i < s.getWidth(); i++) //check all the chars at the line above the object.
	{
		//if this char is an artifact.
		if (board[ObjStart.getY() - 1][ObjStart.getX()] == GameChars::ARTIFACT)
		{
			//if thereisn't dragged artifact already, return this artifact.
			if (s.GetDragged() == nullptr)
				return findArtifactByPoint(Point(ObjStart.getX(), ObjStart.getY() - 1));
	
			//if there is already dragged artifact, and this isn't the same one, return this artifact.
			else if (!(findArtifactByPoint(Point(ObjStart.getX(), ObjStart.getY() - 1)) == *(s.GetDragged())))
				return findArtifactByPoint(Point(ObjStart.getX(), ObjStart.getY() - 1));
			
			//else this is the same one, move to the next char.
			else ObjStart.set(ObjStart.getX() + 1, ObjStart.getY());
		}
		else ObjStart.set(ObjStart.getX() + 1, ObjStart.getY());
	}
	
	return *(s.GetDragged()); //if we are here we already have dragged and there is'nt another artifact above me
}

//this function set the artifacts list.
void TheMathGame::SetCreaturesList()
{
	Creature* tempCreature;

	ClearCreaturesList(OriginalCreatures);

	//we need to use tempBoard because we are changing the board at SetArtifact.
	char tempBoard[ROWS][COLS + 1];
	copyBoard(tempBoard); 

	for (int i = 0; i < ROWS; ++i) {
		for (int j = 0; j < COLS; ++j) {
			//if this char is an artifact, push to original list and set it by this point.
			if (tempBoard[i][j] == GameChars::ARTIFACT)
			{
				try { tempCreature = new Artifact(this, GameChars::ARTIFACT, Color::LIGHTRED);}
				catch (bad_alloc& ba)
				{
					DeleteAllMemoryAllocations();
					cerr << "bad_alloc caught: " << ba.what() << '\n';
					exit(1);
				}
				OriginalCreatures.push_back(tempCreature);
				SetArtifact(Point(j, i), static_cast<Artifact&>(*tempCreature), tempBoard);
			}
			else if (tempBoard[i][j] == GameChars::ROW_FLYER)
			{
				try { tempCreature = new Flyer(Point(j, i), GameChars::ROW_FLYER, Direction::RIGHT, Color::LIGHTBLUE, this); }
				catch (bad_alloc& ba)
				{
					DeleteAllMemoryAllocations();
					cerr << "bad_alloc caught: " << ba.what() << '\n';
					exit(1);
				}
				OriginalCreatures.push_back(tempCreature);
			}
			else if (tempBoard[i][j] == GameChars::COL_FLYER)
			{
				try { tempCreature = new Flyer(Point(j, i), GameChars::COL_FLYER, Direction::UP, Color::LIGHTBLUE, this); }
				catch (bad_alloc& ba)
				{
					DeleteAllMemoryAllocations();
					cerr << "bad_alloc caught: " << ba.what() << '\n';
					exit(1);
				}
				OriginalCreatures.push_back(tempCreature);
			}
			else if (tempBoard[i][j] == GameChars::NUMBERS_EATER)
			{
				try { tempCreature = new NumbersEater(Point(j, i), GameChars::NUMBERS_EATER, Direction::DOWN, Color::LIGHTGREY, this); }
				catch (bad_alloc& ba)
				{
					DeleteAllMemoryAllocations();
					cerr << "bad_alloc caught: " << ba.what() << '\n';
					exit(1);
				}
				OriginalCreatures.push_back(tempCreature);
			}
		}
	}
}

//input: direction, artifact by ref, artifact's width, artifact's height, length to add.
//return: the artifact that at the oposite side of the screen.
Artifact& TheMathGame::getCrossedArtifact(int direction, Artifact& ar, int ObjWidth, int ObjHeight, int LengthToAdd)
{
	char temp;
	for (Point p : ar.getPointList()) //for all the points of the artifact.
	{
		p = CalculateCrossingDestByDirection(p, direction, ObjWidth, ObjHeight, LengthToAdd); //change p to dest point.
		temp = board[p.getY()][p.getX()];  //insert to temp the char of this place.
		//if this is an artifact, and it's not the same artifact, return this artifact.
		if (temp == TheMathGame::GameChars::ARTIFACT)
		{
			if (!ar.findPoint(p))
				return findArtifactByPoint(p);
		}
	}
	return getLastArtifact(); //we shouldn't get here just for all control path return a value.
}

//input: sleepTime by ref.
//this function manage the user's keyboard hits when solution is playing.
void TheMathGame::ManageKeyboardHitsAtSolutionPlay(int& SleepTime)
{
	char ch;

	//go to the line under the board and print the options
	gotoxy(Resolution::WIDTH_BEGINNIG, Resolution::HEIGHT_END + 1);
	setTextColor(Color::YELLOW);
	cout << "'+' PLAYING FASTER, '-' PLAYING SLOWER, 'ESC' return to sub menu";
	setTextColor(Color::WHITE);
	
	//if the user pressed any key
	if (_kbhit()) 
	{
		ch = _getch();

		//if the key is '+' divide sleepTime by 2.
		if (ch == '+' && sleepTime >= MIN_SLEEP_TIME)
			sleepTime /= 2;
		
		//if the key is '-' multiply sleepTime by 2.
		else if (ch == '-' && sleepTime <= MAX_SLEEP_TIME)
			sleepTime *= 2;

		//if the key is 'ESC' make sleepTime  0 for breaking the playing solution loops.
		else if (ch == ESC)
			sleepTime = 0;

		cin.clear(); //clean cin buffer for case that there is multiple keyboard hits (for new keyboard hits to be treated).
	}
}

TheMathGame::~TheMathGame()
{
	DeleteAllMemoryAllocations();
}

void TheMathGame::BombCreature(const Point& PointOfCreature)
{
	Creature* crToRemove = findCreatureByPoint(PointOfCreature);

	crToRemove->DeleteMe(); //delete creature from board
	CreaturesToFree.push_back(crToRemove);
	Creatures.remove(crToRemove);
}

//deep copy of original creatures to creatures.
void TheMathGame::CopyCreaturesList()
{
	ClearCreaturesList(Creatures);
	for (Creature* cr : OriginalCreatures)
	{
		Creatures.push_back(cr->CreateNewCreature());
	}
}

Creature* TheMathGame::findCreatureByPoint(const Point& PointOfCreature)
{
	for (Creature* cr : Creatures)
	{
		if (cr->CheckIfThisPointBelongToMe(PointOfCreature))
			return cr;
	}
	return Creatures.back(); //if we didn't find, return the last one.
}

void TheMathGame::DeleteArtrifactFromBoard(const Artifact& ar)
{
	for (Point p : ar.getPointList())
	{
		board[p.getY()][p.getX()] = GameChars::FREE_PASS;
	}
}

void TheMathGame::DeleteFlyerOrNumbersEeaterFromBoard(const Point& p)
{
	board[p.getY()][p.getX()] = GameChars::FREE_PASS;
}

void TheMathGame::BombAllNumbersEeaterByDirection(SpaceShip* s, int direction)
{
	//first delete all the Numbers Eater near the space ship.
	BombAllNumbersEatersNearSpaceShip(s, direction);
	//if there is dragged artifact, delete all the numbers eater next to him also.
	if (s->GetDragged() != nullptr)
	{
		BombAllNumbersEatersNearArtifact(s->GetDragged(), direction);
	}
}

void TheMathGame::BombAllNumbersEatersNearSpaceShip(SpaceShip* s, int direction)
{
	Point ObjStart = s->GetStartPoint();
	switch (direction){
	case Direction::UP:
		for (int i = 0; i < s->getWidth(); i++) //check all the chars at the line above the object.
		{
			if (board[ObjStart.getY() - 1][ObjStart.getX()] == GameChars::NUMBERS_EATER)
				BombCreature(Point (ObjStart.getX(), ObjStart.getY()-1));
			ObjStart.set(ObjStart.getX() + 1, ObjStart.getY());
		}
			break;
	case Direction::DOWN:
		ObjStart.set(ObjStart.getX(), ObjStart.getY() + (s->getHeight() - 1));
		for (int i = 0; i < s->getWidth(); i++) //check all the chars at the line under the object.
		{
			if (board[ObjStart.getY() + 1][ObjStart.getX()] == GameChars::NUMBERS_EATER)
				BombCreature(Point(ObjStart.getX(), ObjStart.getY() + 1));
			ObjStart.set(ObjStart.getX() + 1, ObjStart.getY());
		}
		break;
	case Direction::LEFT:
		for (int i = 0; i < s->getHeight(); i++) //check all the chars at the column that left to the object.
		{
			if (board[ObjStart.getY()][ObjStart.getX() - 1] == GameChars::NUMBERS_EATER)
				BombCreature(Point(ObjStart.getX() - 1, ObjStart.getY()));
			ObjStart.set(ObjStart.getX(), ObjStart.getY() + 1);
		}
		break;
	case Direction::RIGHT:
		ObjStart.set(ObjStart.getX() + (s->getWidth() - 1), ObjStart.getY());
		for (int i = 0; i < s->getHeight(); i++) //check all the chars at the column that right to the object
		{
			if (board[ObjStart.getY()][ObjStart.getX() + 1] == GameChars::NUMBERS_EATER)
				BombCreature(Point(ObjStart.getX() + 1, ObjStart.getY()));
			ObjStart.set(ObjStart.getX(), ObjStart.getY() + 1);
			break;
		}
	}
}

void TheMathGame::BombAllNumbersEatersNearArtifact(Artifact* ar, int direction)
{
	for (Point p : ar->getPointList()) //check all the chars at the line above the object.
	{
		switch (direction)
		{
		case Direction::UP:
			p.set(p.getX(), p.getY() - 1);
			break;
		case Direction::DOWN:
			p.set(p.getX(), p.getY() + 1);
			break;
		case Direction::LEFT:
			p.set(p.getX() - 1, p.getY());
			break;
		case Direction::RIGHT:
			p.set(p.getX() + 1, p.getY());
			break;
		}
		if (board[p.getY()][p.getX()] == GameChars::NUMBERS_EATER)
			BombCreature(p);
	}
}

void TheMathGame::BombAllNumbersEeaterByDirectionAfterCrossing(SpaceShip* s, Point dest, int LengthToAdd, int direction)
{
	//first delete all the Numbers Eater near the space ship.
	BombAllNumbersEatersNearSpaceShipAfterCrossing(s, dest, direction);
	//if there is dragged artifact, delete all the numbers eater next to him also.
	if (s->GetDragged() != nullptr)
	{
		BombAllNumbersEatersNearArtifactAfterCrossing(s, LengthToAdd, direction);
	}
}

void TheMathGame::BombAllNumbersEatersNearSpaceShipAfterCrossing(SpaceShip* s, Point dest, int direction)
{
	for (int i = 0; i < s->getHeight(); i++) //check all the chars in this place.
	{
		for (int j = 0; j < s->getWidth(); j++)
		{
			if (board[dest.getY() + i][dest.getX() + j] == GameChars::NUMBERS_EATER)
				BombCreature(Point (dest.getX() + j, dest.getY() + i));
		}
	}
}

void TheMathGame::BombAllNumbersEatersNearArtifactAfterCrossing(SpaceShip* s, int LengthToAdd, int direction)
{
	for (Point p : s->GetDragged()->getPointList()) //for all the points of the artifact.
	{
		p = CalculateCrossingDestByDirection(p, direction, s->getWidth(), s->getHeight(), LengthToAdd);

		if (board[p.getY()][p.getX()] == TheMathGame::GameChars::NUMBERS_EATER)
		{
			BombCreature(p);
		}
	}
}

void TheMathGame::BombAllNumbersEatersNearArtifactAfterCrossing(Artifact* ar, int direction)
{
	for (Point p : ar->getPointList()) //for all the points of the artifact.
	{
		p = CalculateCrossingDestByDirection(p, direction, 0, 0, 0);

		if (board[p.getY()][p.getX()] == TheMathGame::GameChars::NUMBERS_EATER)
		{
			BombCreature(p);
		}
	}
}

void TheMathGame::ClearCreaturesList(list<Creature*> &l)
{
	if (!l.empty())
	{
		for (Creature* cr : l)
			delete cr;
		l.clear();
	}
}

Point TheMathGame::getClosestPoint(const Point& p)
{
	Point res;
	double minDis = Point(Resolution::WIDTH_BEGINNIG, Resolution::HEIGHT_BEGINNIG).calcDistance(Point(Resolution::WIDTH_END, Resolution::HEIGHT_END));
	for (Point& tempPoint : digitsList)
	{
		if (tempPoint.calcDistance(p) < minDis) //if the distance between the two points is lower then the min distance, return the point
		{
			minDis = tempPoint.calcDistance(p);
			res = tempPoint;
		}
	}
	return res; //if there is not numbers on the board, the res will return with values x = 1, y = 1
}

bool TheMathGame::checkIfNumberIsStillAlive(const Point& p)
{
	for (Point& tempPoint : digitsList)
	{
		if (tempPoint == p)
			return true;
	}
	return false;
}

void TheMathGame::doSubIteration()
{
	list<Creature*>::iterator NextIt;
	NumbersEater* tempNE;

	for (list<Creature*>::iterator it = Creatures.begin(); it != Creatures.end();)
	{
		tempNE = dynamic_cast <NumbersEater*> (*it);
		if (tempNE) //if this creature is a numbers eater.
		{
			//we have to save temp next iterator for case that this creature died at this loop.
			NextIt = next(it, 1);
			if ((*it)->move()) //if the movment succeeded (the creature didn't die)
				it++;
			else
				it = NextIt;
		}
		else it++;
	}
}

Artifact& TheMathGame::getLastArtifact()
{
	Artifact* lastAr = nullptr;
	Artifact* tempAr;
	for (Creature* tempCreature : Creatures) //for all the creatures
	{
		tempAr = dynamic_cast<Artifact*> (tempCreature);
		if (tempAr) //if this creature is an artifact
		{
			lastAr = tempAr;
		}
	}
	return *lastAr;
}

void TheMathGame::DeleteAllMemoryAllocations()
{
	ClearCreaturesList(OriginalCreatures);
	ClearCreaturesList(Creatures);
	ClearCreaturesList(CreaturesToFree);
}