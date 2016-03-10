#include <list>
#include <iostream>
#include <fstream>
#include "GameManager.h"
#include "ISpecificGame.h"
#include "io_utils.h"

using namespace std;

char GameManager::mainMenu()
{
	actualGame.UpdateFilesLists(); //update the files lists any time we are in the main menu.
	PrintMainMenu();
	char selection = 0;
	do {
		selection = _getch();
	} while(!GameManager::MainMenuOptions::isValidOption(selection));
	return selection;
}

void GameManager::PrintMainMenu()
{
	setTextColor(Color::YELLOW);

	gotoxy(26, 6);
	cout << "//////////////////////////" << endl;
	gotoxy(26, 7);
	cout << "      Main Menu:       " << endl;
	gotoxy(26, 8);
	cout << "//////////////////////////" << endl;
	gotoxy(32, 10);
	cout << "1. Instructions" << endl;
	gotoxy(32, 11);
	cout << "2. Play Game" << endl;
	gotoxy(32, 12);
	cout << "3. Check Board From File" << endl;
	gotoxy(32, 13);
	cout << "4. Continue Saved Game" << endl;
	gotoxy(32, 14);
	cout << "5. Play Game From Selected Screen" << endl;
	gotoxy(32, 15);
	cout << "9. Exit" << endl;

	setTextColor(Color::WHITE);
}

void GameManager::printSubMenu()
{
	clear_screen();
	setTextColor(Color::YELLOW);
	gotoxy(31, 6);
	cout << "//////////////////////////" << endl;
	gotoxy(31, 7);
	cout << "      Sub Menu:       " << endl;
	gotoxy(31, 8);
	cout << "//////////////////////////" << endl;
	gotoxy(32, 10);
	cout << "1. Exit the application" << endl;
	gotoxy(32, 11);
	cout << "2. Back to main menu" << endl;
	gotoxy(32, 12);
	cout << "3. Continue level" << endl;
	gotoxy(32, 13);
	cout << "4. Replay level" << endl;
	gotoxy(32, 14);
	cout << "5. Save the game" << endl;
	gotoxy(32, 15);
	cout << "6. Play solution of this level" << endl;
	setTextColor(Color::WHITE);
}

//this function is printing the sub menu and return the chose of the user.
char GameManager::ExecuteSubMenu()
{
	char res;

	//print the sub menu and take valid option.
	clear_screen();
	printSubMenu();
	do {
		res = _getch();
	} while (!GameManager::LevelOptions::isValidOption(res));
	return res;
}

void GameManager::run()
{
	bool userWantsToPlay = true;
	// we run as long as the user wants
	while(userWantsToPlay) {
		char menuSelection = mainMenu();
		// TODO: handle here all the different menu options
		switch(menuSelection)
		{
		case GameManager::MainMenuOptions::PLAY_GAME:
			init();
			actualGame.ZeroCurrentLevel();
			actualGame.ZeroIsLevelDone();
			actualGame.SetTotalLevels();
			userWantsToPlay = playGame();
			break;
		case GameManager::MainMenuOptions::PRESENT_INSTRUCTIONS:
			clear_screen();
			displayInstructions();
			break;
		case GameManager::MainMenuOptions::CHECK_BOARD_FROM_FILE:
			bool temp; //temp need to do overloaded function.
			actualGame.TheBoardChecker(GetFileNameFromUser(), temp, true);
			break;
		case GameManager::MainMenuOptions::CONTINUE_SAVED_GAME:
			if (actualGame.UploadSavedGame())
				userWantsToPlay = playGame(true);
			else
			{
				clear_screen();
				PrintMainMenu();
			}
			break;
		case GameManager::MainMenuOptions::PLAY_GAME_FROM_SELECTED_SCREEN:
			actualGame.SetTotalLevels(); //set total levels.
			if (actualGame.StartGameFromSelectedScreen()) //if the user chose screen play game.
				userWantsToPlay = playGame();
			else //else clear the screen and return to the main menu (userWantsToPlay is true so we shouldn't do nothing)
			{
				clear_screen();
			}
			break;
		case GameManager::MainMenuOptions::EXIT_APPLICATION:
			userWantsToPlay = false;
			break;
		default: // normally we shouldn't get to here...
			userWantsToPlay = false;
		};
	}
}

bool GameManager::playGame(bool isSavedGame)
{
	// we assume that we have multiple levels so there is a need to loop through levels
	// till all levels were played or till user quits - if user quits we do not continue to next level
	//-------------------------------------------------------------
	// this is the game LEVELS loop
	//-------------------------------------------------------------
	char action = GameManager::LevelOptions::NEXT_LEVEL;
	while((isSavedGame || actualGame.hasNextLevel()) && action == GameManager::LevelOptions::NEXT_LEVEL) {
		action = playNextLevel(isSavedGame);
		isSavedGame = false;
	}
	//-------------------------------------------------------------
	// END of game LEVELS loop
	//-------------------------------------------------------------
	
	//Note if the user won (check if level done for case that the user is exiting from the sub menu).
	if (!actualGame.hasNextLevel() && actualGame.isLevelDone())
	{
		clear_screen();
		cout << "Congratulation!! you finished all the Levels!\n";
		Sleep(FINISHED_ALL_LEVELS_SLEEP_DURATION);
	}
	clear_screen();
	
	// return true if the user wants to keep playing
	return (action != GameManager::LevelOptions::EXIT_APPLICATION);
}

// return action to take in case of ESC
char GameManager::playNextLevel(bool isSavedGame)
{
	if (!isSavedGame)
	{
		++currentLevel;
		actualGame.startLevel();
	}
	
	//------------------------------------------------------------------------------
	// here we control the ESC menu
	//------------------------------------------------------------------------------
	bool keepRunning = true;
	char action = 0;
	while(keepRunning)
	{
		//=============================================================================================
		// this is the actual call to play game iterations
		if (action == 0) action = doLevelIterations(); //the if is for case that we returned from save game.
		//=============================================================================================
		
		// check action based on game ended (action==BACK_TO_MAIN_MENU) or input from user on ESC menu
		switch(action) {
		case GameManager::LevelOptions::CONTINUE:
			// keepRunning is true, so we only need to set thing right and then - keepRunning!
			//--------------------------------------------------------------------------------
			clear_screen();
			actualGame.printBoard();
			action = 0; //for enter to the if (action == 0) for doLevelIteration
			break;
		case GameManager::LevelOptions::REPLAY_LEVEL:
			// keepRunning is true, so we only need to set thing right and then - keepRunning!
			//--------------------------------------------------------------------------------
			actualGame.startLevel(true);
			action = 0; //for enter to the if (action == 0) for doLevelIteration
			break;
		case GameManager::LevelOptions::BACK_TO_MAIN_MENU:
		case GameManager::LevelOptions::EXIT_APPLICATION:
			// get out from the loop
			clear_screen();
			keepRunning = false;
			break;
		case GameManager::LevelOptions::PLAY_SOLUTION:
			actualGame.PlaySolution();

			//print the sub menu and take valid option.
			action = ExecuteSubMenu();
			break;
		case GameManager::LevelOptions::SAVE_GAME: 
			//save the game
			actualGame.SaveGame();
			
			//print the sub menu and take valid option.
			action = ExecuteSubMenu();

			break;
		case GameManager::LevelOptions::NEXT_LEVEL:
			// get out from the loop so we get to the next level
			keepRunning = false;
			break;
		}
		//---------------------------------------
		// END of sub menu action switch
		//---------------------------------------
	}
	//------------------------------------------
	// END of Level keepRunning loop (sub menu)
	//------------------------------------------

	// the actions that may get out from this method are: BACK_TO_MAIN_MENU and EXIT_APPLICATION
	return action;
}

// return action to take in case of ESC
char GameManager::doLevelIterations()
{
	char action;
	bool escapePressed = false; 
	//---------------------------------------
	// this is the game iterations loop
	//---------------------------------------
	while(!actualGame.isLevelDone() && !escapePressed && !actualGame.isGameOver()) {
		escapePressed = !doIteration();
	}
	//-----------------------------------------------------
	// END of game iterations loop or exit on ESC
	//-----------------------------------------------------

	// check why we are here
	if(actualGame.isLevelDone()) {
		clear_screen();
		cout << endl << "WELL DONE" << endl;
		actualGame.SaveSolution();
		PlaySoundGame(SoundFreq::WINNING, SoundDuration::GAMESOUNDDURATION_LONG);
		action = GameManager::LevelOptions::NEXT_LEVEL;
	}
	else if (actualGame.isGameOver())
	{
		clear_screen();
		PlaySoundGame(SoundFreq::GAMEOVER, SoundDuration::GAMESOUNDDURATION_LONG);
		gotoxy(32, 12);
		cout << "Game Over!!!\n" << endl;
		Sleep(GAME_OVER_SLEEP_DURATION);
		init(); //init the current level for new start if needed.
		actualGame.ZeroCurrentLevel(); //do the same in the mathgame.
		actualGame.ZeroIsLevelDone();
		clear_screen();
		action = GameManager::LevelOptions::BACK_TO_MAIN_MENU;
	}
	else if(escapePressed) {
		action = ExecuteSubMenu();
	}
	// end of esc pressed
	// TODO: clear the sub menu options from screen
	return action;
}

bool GameManager::doIteration()
{
	unsigned int sleepLength = clockCycleInMillisec/iterationsPerClockCycle;
	for(unsigned int i=0; i<iterationsPerClockCycle-1; ++i) {
		Sleep(sleepLength);
		actualGame.doSubIteration();
	}
	return doInputIteration();
}

bool GameManager::doInputIteration()
{
	list<char> keyHits(KEYBOARD_HIT_LOOP); // set vector initialize size to the max key strokes that we may collect
	bool shouldContinue = true;
	// get keyboard input in a loop, to get enough keyboard hits
	unsigned int sleepLength = clockCycleInMillisec/iterationsPerClockCycle / KEYBOARD_HIT_LOOP;
	for(unsigned int i=0; i<KEYBOARD_HIT_LOOP; ++i) {
		Sleep(sleepLength);
		if (_kbhit()) {
			char ch = _getch();
			if(ch == ESC) {
				// ESC pressed
				shouldContinue = false;
			}
			else {
				// we got keyHits byref, so we actually set input into the original list sent here
				keyHits.push_front(ch); // we want recent keys to come first, thus we use push_front
			}
		}
	}
	// send the keystrokes to the game
	// (even if ESC was hit, we may still have something in the keystrokes vector and should use it)
	actualGame.doIteration(keyHits);

	return shouldContinue;
}

void GameManager::displayInstructions()
{
	ifstream InstructionsFile("Instructions.txt");
	string str;
	string file_contents = "";
	while (getline(InstructionsFile, str))
	{
		file_contents += str;
		file_contents.push_back('\n');
	}
	setTextColor(Color::LIGHTBLUE);
	cout << "## YOU CAN SCROLL THE PAGE UP AND DOWN BY PRESSING UP AND DOWN ARROWS ##" << endl;
	cout << "For return to the Main Menu Press ESC" << endl;
	setTextColor(Color::WHITE);

	cout << file_contents;

	setTextColor(Color::LIGHTBLUE);
	cout << "For return to the Main Menu Press ESC" << endl;
	setTextColor(Color::WHITE);
	//for backing to the start of the instructions.
	gotoxy(0, 0);
	cout << "";

	//waiting for the user to press ESC (for returning to the main menu).
	char selection = 0;
	do {
		selection = _getch();
		if (selection == KEY_DOWN || selection == KEY_UP) //for scrolling the page up and down.
			ScrollByAbsoluteCoord(1, (int)selection);
	} while (selection != ESC);

	clear_screen(); //clear the screen for the print of the main menu.
}

string GameManager::GetFileNameFromUser()
{
	string res;

	clear_screen();
	setTextColor(Color::LIGHTRED);
	cout << "Please enter the name of the board file: " << endl;
	setTextColor(Color::WHITE);
	cin >> res;
	cout << endl;

	return res;
}