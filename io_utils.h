#ifndef _IO_UTILS_H_
#define _IO_UTILS_H_
// you must have a config file, both for windows and for compiling on Linux (MAMA)
// BUT, you need to change the content of the config!
#include "config.h"
#include "Color.h"
#include "Sound.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string>

#define KEY_DOWN 80
#define KEY_UP 72
#define MAX_PATH_LENGTH 260

using namespace std;

void gotoxy(int x, int y);
void setTextColor(Color);
void hideCursor();
void clear_screen();
void PlaySoundGame(SoundFreq Freq, SoundDuration Duration = SoundDuration::GAMESOUNDDURATION);
void PlayOpeningSound();
int ScrollByAbsoluteCoord(int iRows, int direction);
string getCurrentDirectoryOnWindows();

#ifndef WINDOWS	
int _getch(void);
int _kbhit(void);
void Sleep(unsigned long);
void _itoa_s(unsigned int, char s[], unsigned int, int);
#else
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#endif

#endif