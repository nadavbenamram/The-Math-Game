#ifndef _GAME_MANAGER_H_
#define _GAME_MANAGER_H_

#include "ISpecificGame.h"
#include "Point.h"
#include "io_utils.h"
#include <string>
#include <list>


using namespace std;

class GameManager
{
	ISpecificGame& actualGame;
	unsigned int currentLevel;
	unsigned int clockCycleInMillisec;
	unsigned int iterationsPerClockCycle;
	const static unsigned int KEYBOARD_HIT_LOOP = 10;
	const static int FINISHED_ALL_LEVELS_SLEEP_DURATION = 2000;
	const static int GAME_OVER_SLEEP_DURATION = 1000;
	const static char ESC = 27; // the ESC key
public:
	GameManager(ISpecificGame& theSpecificGame, unsigned int clockCycleLengthInMillisec, unsigned int numIterationsPerClockCycle)
		:actualGame(theSpecificGame), currentLevel(0), clockCycleInMillisec(clockCycleLengthInMillisec), iterationsPerClockCycle(numIterationsPerClockCycle) { theSpecificGame.SetSleepTime(clockCycleLengthInMillisec / numIterationsPerClockCycle); }
	void run();
private:
	// helper methods
	void init()
	{
		currentLevel = 0;
	}
	char mainMenu();
	void PrintMainMenu();
	bool playGame(bool isSavedGame = false);
	char playNextLevel(bool isSavedGame = false);
	char doLevelIterations();
	bool doIteration();
	bool doInputIteration();
	void printSubMenu();
	char ExecuteSubMenu();
	string GetFileNameFromUser();
		
public:
	void displayInstructions();

	class MainMenuOptions
	{
	public:
		static bool isValidOption(char option) {
			string validOptions = "123549"; 
			return (validOptions.find(option) != string::npos);
		}
		enum
		{
			PRESENT_INSTRUCTIONS				=	'1',
			PLAY_GAME							=	'2',
			CHECK_BOARD_FROM_FILE				=	'3',
			CONTINUE_SAVED_GAME					=	'4',
			PLAY_GAME_FROM_SELECTED_SCREEN		=	'5',
			EXIT_APPLICATION					=	'9'
		};
	};
	class LevelOptions
	{
	public:
		static bool isValidOption(char option) {
			string validOptions = "123456";
			return (validOptions.find(option) != string::npos);
		}
		enum
		{
			EXIT_APPLICATION			=	'1',
			BACK_TO_MAIN_MENU			=	'2',
			CONTINUE					=	'3',
			REPLAY_LEVEL				=	'4',
			SAVE_GAME                   =   '5',
			PLAY_SOLUTION				=   '6',
			NEXT_LEVEL					=	'9'
		};
	};
};

#endif