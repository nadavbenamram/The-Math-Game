#include <iostream>
#include <windows.h>
#include "GameManager.h"
#include "TheMathGame.h"
#include "io_utils.h"
#include "SpaceShip.h"
#include "Exercise.h"
#include "Artifact.h"
#include <string>
#include <list>
using namespace std;

int main()
{
	clear_screen();
	hideCursor();

	TheMathGame theMathGame;

	GameManager(theMathGame, 160, 2).run();

	return 0;
}

