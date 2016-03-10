#include "io_utils.h"
#include "Color.h"
#include "Sound.h"
#include <string>

using namespace std;

HANDLE hStdout;

#ifndef WINDOWS
void gotoxy(int x, int y) {}
int _getch(void){ return 0; }
int _kbhit(void){ return 0; }
void Sleep(unsigned long){}
void setTextColor(Color color){}
void hideCursor(){}
void clear_screen(){}
#else
void gotoxy(int x, int y)
{
	HANDLE hConsoleOutput;
	COORD dwCursorPosition;
	cout << flush;
	dwCursorPosition.X = x;
	dwCursorPosition.Y = y;
	hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hConsoleOutput, dwCursorPosition);
}

void setTextColor(Color colorToSet){
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (int)colorToSet);
}

void hideCursor()
{
	HANDLE myconsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO CURSOR;
	CURSOR.dwSize = 1;
	CURSOR.bVisible = FALSE;
	SetConsoleCursorInfo(myconsole, &CURSOR);//second argument need pointer
}

void clear_screen()
{
	system("cls");
}

void PlaySoundGame(SoundFreq Freq, SoundDuration Duration)
{
	if (Freq == SoundFreq::BOMBHITSPACESHIP)
		cout << "\a";
	else
		Beep((unsigned long)Freq, (unsigned long)Duration);
}

void PlayOpeningSound()
{
	//Part Of Mario Sound... Original: http://wiki.mikrotik.com/wiki/Super_Mario_Theme
	Beep(660, 150);
	Sleep(100);
	Beep(660, 150);
	Sleep(200);
	Beep(660, 150);
	Sleep(200);

	Beep(510, 150);
	Sleep(50);
	Beep(660, 150);
	Sleep(170);
	Beep(770, 150);
	Sleep(450);
	Beep(385, 150);
}

//this function scroll the console iRows by direction.
//return number of scrolled rows.
// source: https://msdn.microsoft.com/en-us/library/windows/desktop/ms685118(v=vs.85).aspx
// edited for down also
int ScrollByAbsoluteCoord(int iRows, int direction)
{
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	SMALL_RECT srctWindow;
	
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	// Get the current screen buffer size and window position. 

	if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
	{
		return 0;
	}

	// Set srctWindow to the current window size and location. 

	srctWindow = csbiInfo.srWindow;

	// Check whether the window is too close to the screen buffer top or sceen buffer bottom (up to direction)

	if (direction == KEY_DOWN)
	{
		srctWindow.Top += (SHORT)iRows;     // move top up
		srctWindow.Bottom += (SHORT)iRows;  // move bottom up

		if (!SetConsoleWindowInfo(
			hStdout,          // screen buffer handle 
			TRUE,             // absolute coordinates 
			&srctWindow))     // specifies new location 
		{
			return 0;
		}
		return iRows;
	}
	else if (direction == KEY_UP && srctWindow.Top >= iRows)
	{
		srctWindow.Top -= (SHORT)iRows;     // move top up
		srctWindow.Bottom -= (SHORT)iRows;  // move bottom up

		if (!SetConsoleWindowInfo(
			hStdout,          // screen buffer handle 
			TRUE,             // absolute coordinates 
			&srctWindow))     // specifies new location 
		{
			return 0;
		}
		return iRows;
	}
	else
	{
		return 0;
	}
}

//this function return the path of the current directory.
//source: http://stackoverflow.com/questions/875249/how-to-get-current-directory
string getCurrentDirectoryOnWindows()
{
	char currentDir[MAX_PATH_LENGTH];
	GetCurrentDirectory(MAX_PATH_LENGTH, currentDir);
	return string(currentDir);
}

#endif


