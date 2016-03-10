#ifndef _EXERCISE_H_
#define _EXERCISE_H_

#include "Point.h"

class TheMathGame;

enum {NumOfParameters = 3, UKNOWN_PARAMETER = '_', SPACE_ = ' ', SOLVED = 10, CAN_BE_SOLVED = 11, CANT_BE_SOLVED = 12, WRONG = 13}; 

class Exercise{
	int OriginalEquation[NumOfParameters]; //for saving the original equation.
	int Equation[NumOfParameters];
	unsigned int OriginalNOUP = 0; //for saving the original num of unkown parameters.
	unsigned int NOUP = 0; //Num Of Unkown Parameters.
	unsigned int places[NumOfParameters]; // for the places in Equation of the unknown parameters.
	unsigned int Originalplaces[NumOfParameters]; //for saving the original places in equation of unkown parameters.
	char op; //for the sign of the operation.
	Point Start; //for the start point of the exercise.
	TheMathGame *TheGame;
	unsigned int ExSituation = CAN_BE_SOLVED; //for the exercise situatuin : solved, can be solved, can't be solved.
	
	//private functions:
	int getNumFromString(const char* ex, unsigned int& i);
	bool CheckEquation();
	void RemoveUknown();
	bool CheckIfCanBeSolved();
	bool CheckIfCanBeSolvedPlusOperator();
	bool CheckIfCanBeSolvedMinusOperator();
	bool CheckIfCanBeSolvedDivisionOperator();
	bool CheckIfCanBeSolvedMultiplicationOperator();
	bool ThereIsASolution();

public:
	void setExercise(const char* ex);
	void setStart(Point _start) { Start = _start; }
	void setGame(TheMathGame* _thegame) { TheGame = _thegame; }
	void PrintExercise();
	unsigned int CheckAns(int ans);
	void RestartExercise();
	void CopyExerciseToFile(ofstream& fout);
};


#endif