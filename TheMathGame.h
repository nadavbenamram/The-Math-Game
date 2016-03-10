#ifndef _THEMATHGAME_H_
#define _THEMATHGAME_H_

#include <list>
#include <string>
#include <set>
#include <fstream>
#include "ISpecificGame.h"
#include "Point.h"
#include "SpaceShip.h"
#include "Exercise.h"
#include "GameState.h"
#include "Artifact.h"
#include "Creature.h"
#include "Flyer.h"
#include "NumbersEater.h"
#include "KeysByClockIteration.h"
#include <cctype>

class GameManager;

using namespace std;

enum {ROWS = 24, COLS = 80};
enum { BOMB_SLEEP_DURATION = 200 };
enum { MAX_LINE_LEN = 256, MAX_NAME_LEN = 200, SLOW_MOTION_FACTOR = 5, MESSAGE_SLEEP_DURATION = 3000 ,
       MIL_SECONDS_PER_SECOND = 1000, AFTER_SOLUTION_SLEEP_DURATION = 8000, ESC = 27,
	   MAX_SLEEP_TIME = 600, MIN_SLEEP_TIME = 60};

class TheMathGame : public ISpecificGame
{
	char originalBoard	[ROWS][COLS+1];	// this is the original board that we got (we need COLS+1 because we handle it as null terminated char*)
	char board		[ROWS][COLS+1];	// this is the actual board we play on, i.e. changes on board are done here
	SpaceShip bigSpaceShip, smallSpaceShip;
	Exercise Ex;
	GameState Gs;
	list <Creature*> OriginalCreatures;
	list <Creature*> Creatures;
	list <Creature*> CreaturesToFree;
	list <Point> digitsList;
	int sleepTime;
	int TotalLevels;
	int currentLevel = 0;
	bool IsEquationSolved = false;
	list <KeysByClockIteration> keysList;
	bool StartedAfterBomb = false; //to know if this level started after bomb (importent for artifact's move).

	////////////////////nested class:///////////////////////////////////////////////
	class NamesByScreenID {
		string name;
		size_t screenID;
	public:
		NamesByScreenID() {}
		NamesByScreenID(string _name) : name(_name) {}
		NamesByScreenID(string _name, size_t _screenID) : name(_name), screenID(_screenID) {}
		void setScreenID(size_t _screenID) { screenID = _screenID; }
		size_t getScreenID()const { return screenID; }
		string getName()const { return name; }
		size_t getClockCycle(string fileName);
		friend TheMathGame;
	};

	////////////////////////////////////////////////////////////////////////////////

	list<TheMathGame::NamesByScreenID> BoardFilesName;
	list<TheMathGame::NamesByScreenID> SavedFilesName;
	list<TheMathGame::NamesByScreenID> SolutionFilesName;

	//private functions:
	void initBoard(bool WithPrinting = true);
	void copyBoard(char destBoard[][COLS + 1]);
	void handleObjectCreationFromBoard(int, int, bool&, bool&, bool);
	void handleBigSpaceShip(bool&, int, int);
	void handleSmallSpaceShip(bool&, int, int);
	void handleExercise(int, int);
	void handleGameState(int, int);
	void handleDigit(int, int);
	void handleAllTheRest(int, int);
	string getNameFromUser();
	string getSolverNameFromUser();
	void copyBoardToFile(ofstream& outfile);
	void copyKeysListToFile(ofstream& outfile);
	void copyExerciseToFile(ofstream& outfile);
	string CreateBackPath();
	void readBoardFromFile(ifstream &fileToRead, bool isSPPfile);
	void GetBoardFromFile(ifstream& file, char board[][COLS + 1]);
	bool GetBoardFromFile(ifstream& file, char board[][COLS + 1], set<string>& Exceptions);
	bool TheBoardChecker(string boardFileName, bool& isSppFile, bool printExceptions);
	bool BoardChecker(char boardToCheck[][COLS + 1], set<string> &Exceptions);
	bool CheckBigSpaceShip(char boardToCheck[][COLS + 1], int row, int col);
	bool CheckSmallSpaceShip(char boardToCheck[][COLS + 1], int row, int col);
	bool CheckEndingOfFileName(const char* FileName, bool &isSppFile, set<string>& Exceptions);
	bool CheckIfScreenIDExist(ifstream& inFile, set<string>& Exceptions, const char* FileName, bool isSppFile);
	bool CheckExercise(ifstream& inFile, set<string>& Exceptions);
	bool CheckIfDigit(char ch);
	void CheckIfNumberOrUnkown(const string& line, size_t& index, bool& isValid, set<string>& Exceptions);
	bool CheckEquationValidation(const string& equation, set<string>& Exceptions);
	void PrintExceptions(const set<string>& Exceptions, bool ThereIsntExceptions);
	size_t GetScreenIDFromFile(const string& fileName);
	bool CheckIfScreenIDisUnique(const size_t& ScreenID, set<string>& Exceptions, const char* FileName);
	list<TheMathGame::NamesByScreenID> UpdateFilesList(string folder, char* Ending);
	size_t PrintSavedGamesList();
	size_t PrintScreensList();
	bool needToSaveSolution();
	void getKeysListFromSolutionFile();
	void getKeysListFromFile(ifstream& infile);
	bool CheckIfThereIsSolutionOfThisScreen();
	void SaveGame(string fileName);
	void SleepAndPrintCounterSeconds(int SleepTimePerMilSeconds);
	void CheckInvalidNumber(char ch, int& DigitCounter, bool& ThereIsInvalidNumber);
	void initCreaturesList(bool WithPrinting);
	void SetCreaturesList();
	void SetArtifact(const Point &point, Artifact &artifact, char tempBoard[][COLS + 1]);
	Artifact& findArtifactByPoint(const Point& point);
	void _printCreatures();
	void MoveCreatures();
	Artifact& getCroosedArtifact(const SpaceShip& s);
	Artifact& getAboveArtifact(const SpaceShip& s);
	int GetPosOfDraggedAtrifact(SpaceShip* currentSpaceShip);
	int getFullNumAndCalc(Point place);
	void CheckSignificantChar(char, char&, char&, bool&, bool&, bool&, bool&, bool&);
	char WhoIsMoreSignificant(char ch1, char ch2);
	char ThereIsSpaceShipUnderMe(list <Point> points);
	Point AddLengthByDirection(Point p, int direction, int LengthToAdd = 0);
	bool TryToSolveEquation(int num);
	void DeleteNumber(Point place);
	double CalculateObjectVolume(int width, int height) { return (double)width * (double)height; }
	bool SearchForChar(int direction, list<Point> points, char searchFor);
	int SwapTheDirection(int direction);
	void startLevelAfterBomb();
	void startLevelAfterEquationCantBeSolved();
	void ManageKeyboardHitsAtSolutionPlay(int& SleepTime);
	void CopyCreaturesList();
	void BombAllNumbersEatersNearSpaceShip(SpaceShip* s, int direction);
	void BombAllNumbersEatersNearSpaceShipAfterCrossing(SpaceShip* s, Point dest, int direction);
	void ClearCreaturesList(list<Creature*>&);
	Artifact& getLastArtifact();

public:
	//set functions:
	void setCurrentLevelBoard();
	void setBoard(string fileName);
	void setTotalLevels(int _totalLevels){ TotalLevels = _totalLevels; }

	//get functoins:
	int GetSleepTime() { return sleepTime; }

	char GetSignificantChar(int direction, Point ObjStart, int ObjWidth, int ObjHeight);
	char GetSignificantChar(int direction, Artifact& ar, int LengthToAdd=0);
	char GetSignificantCharAfterCrossingScreen(Point dest, int ObjWidth, int ObjHeight);
	char GetSignificantCharAfterCrossingScreen(int direction, Artifact& ar, int ObjWidth, int ObjHeight, int LengthToAdd);
	bool getNum(Point place, int direction, int ObjWidth, int ObjHeight);
	bool getNum(int direction, list<Point> points);
	bool getNumAfterRotateOrCrossScreen(Point Start, int ObjWidth, int ObjHeight);
	bool getNumAfterRotateOrCrossScreen(int direction, list<Point> points, int LengthToAdd, int ObjWidth, int ObjHeight);
	void UpdateBoard(int direction, Point ObjStart, int ObjWidth, int ObjHeight);
	void UpdateBoard(int direction, list<Point> points, int LengthToAdd = 0);
	void UpdateBoardWhenCrossScreen(Point StartPoint, Point DestPoint, int ObjWidth, int ObjHeight);
	void UpdateBoardWhenCrossScreen(int direction, list<Point> points, int ObjWidth, int ObjHeight, int LengthToAdd);
	Point CalculateDestByDirection(Point start, int direction, int width, int height);
	Point CalculateDestByDirection(Point p, int direction, int LengthToAdd = 0);
	Point CalculateCrossingDestByDirection(Point p, int direction, int ObjWidth, int ObjHeight, int LengthToAdd = 0);
	void DeleteTheAllNumbers(Point place, int direction, int ObjWidth, int ObjHeight);
	void DeleteTheAllNumbers(int direction, list<Point> points, int LengthToAdd = 0);
	SpaceShip* StaticsCheckArtifact(Artifact& ar, bool& isStatic);
	Artifact* CheckForArtifacts(Point start, int width, int height, SpaceShip* spaceShip);
	void BombSpaceShip(char SpaceShipSign);
	bool PushArtifact(SpaceShip* spaceShip, int direction);
	bool PushArtifactAfterCrossing(SpaceShip* spaceShip, int direction, int ObjWidth, int ObjHeight, int LengthToAdd);
	bool CheckIfCanPushArtifact(SpaceShip* ThisSpaceShip, Artifact* pushedArtifact, int direction);
	bool CheckIfCanIPushArtifact(SpaceShip* ThisSpaceShip, Artifact* pushedArtifact, int direction);
	bool CheckIfThereIsNumber(int direction, Point ObjStart, int ObjWidth, int ObjHeight);
	void CalculateWidthAndHeightOfSpaceShip(SpaceShip* curr);
	Artifact& getNearestArtifact(const SpaceShip& s);
	Artifact& getCrossedArtifact(int direction, Artifact& ar, int ObjWidth, int ObjHeight, int LengthToAdd);
	void ArtifactsUnification(Artifact& first, Artifact& second);
	void BombCreature(const Point& PointOfCreature);
	void DeleteArtrifactFromBoard(const Artifact& ar);
	void DeleteFlyerOrNumbersEeaterFromBoard(const Point& p);
	void BombAllNumbersEeaterByDirection(SpaceShip* s, int direction);
	void BombAllNumbersEeaterByDirectionAfterCrossing(SpaceShip* s, Point dest, int LengthToAdd, int direction);
	Creature* findCreatureByPoint(const Point& PointOfCreature);
	Point getClosestPoint(const Point& p);
	bool checkIfNumberIsStillAlive(const Point& p);
	void BombAllNumbersEatersNearArtifact(Artifact* ar, int direction);
	void BombAllNumbersEatersNearArtifactAfterCrossing(SpaceShip* s, int LengthToAdd, int direction);
	void BombAllNumbersEatersNearArtifactAfterCrossing(Artifact* ar, int direction);
	void DeleteAllMemoryAllocations();
	~TheMathGame();

	/////////////virtual functions://////////////////////////////////
	virtual bool isLevelDone()const{ return IsEquationSolved; }
	virtual bool isGameOver();
	virtual bool hasNextLevel()const;
	virtual void startLevel(bool isReplay = false);
	virtual void doIteration(const list<char>& keyHits);
	virtual void doIteration();
	virtual void doSubIteration();
	virtual void printBoard();
	virtual void ZeroCurrentLevel(){ currentLevel = 0; }
	virtual void ZeroIsLevelDone() { IsEquationSolved = false; }
	virtual void SetTotalLevels(){	TotalLevels = BoardFilesName.size();	}
	virtual void SetSleepTime(int SleepTime) { sleepTime = SleepTime; }
	virtual void SaveGame();
	virtual void SaveSolution();
	virtual void UpdateFilesLists();
	virtual size_t UploadSavedGame();
	virtual void PlaySolution();
	virtual bool StartGameFromSelectedScreen();
	////////////////////////////////////////////////////////////////////

	// inner public class for Game chars
	class GameChars
	{
	public:
		enum {
			WALL = '+',
			EXERCISE = 'X',
			GAME_STATE = 'S',
			FREE_PASS = ' ',
			BIG_SPACE_SHIP_SIGN = '#',
			SMALL_SPACE_SHIP_SIGN = '@',
			ARTIFACT = '*',
			ROW_FLYER = '!',
			COL_FLYER = '$',
			NUMBERS_EATER = '%',
		};
	};
	
};

#endif