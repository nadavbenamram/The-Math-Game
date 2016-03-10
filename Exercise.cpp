#include "Exercise.h"
#include <string.h>
#include <fstream>

using namespace std;

//input: string of the exercise, start point of the exersize, pointer of the game.
void Exercise::setExercise(const char* ex)
{
	unsigned int ind = 0; //for the index of Equation array.
	NOUP = 0;
	for (unsigned int i = 0; i < strlen(ex); i++) //for all the chars in the string.
	{
		if (ex[i] >= '0' && ex[i] <= '9') //if the char is a digit, insert this number to Equation.
		{
			OriginalEquation[ind++] = Equation[ind] = getNumFromString(ex, i);
		}
		else if (ex[i] == '+' || ex[i] == '-' || ex[i] == '/' || ex[i] == '*') //if the char is a operator
		{
			op = ex[i];
		}
		else if (ex[i] == UKNOWN_PARAMETER) //if the char is an unknown parameter.
			Originalplaces[NOUP++] = places[NOUP] = ind++;
	}
	OriginalNOUP = NOUP;
}

//this function print the exercise.
void Exercise::PrintExercise()
{
	gotoxy(Start.getX(), Start.getY());
	int PInd = 0; //for the index of the unkown parameters places array
	for (int i = 0; i < NumOfParameters; i++)
	{
		if (i == 1) cout << op << (char)SPACE_; //if we are after the first number / first unkown parameter print op.
		if (i == 2) cout << "=" << (char)SPACE_; //if we are after the first tow numbers / unkown parameters, print = .
		if (i == places[PInd]) //if this is place of unkown parameter, print unkown parameter sign.
		{
			cout << (char)UKNOWN_PARAMETER << (char)SPACE_;
			PInd++;
		}
		else cout << Equation[i] << (char)SPACE_; //else this is a number so print it.
	}
}

//input: reference of ofstream file.
//this function copy the exercise to this file.
void Exercise::CopyExerciseToFile(ofstream& fout)
{
	int PInd = 0; //for the index of the unkown parameters places array
	for (int i = 0; i < NumOfParameters; i++)
	{
		if (i == 1) fout << op; //if we are after the first number / first unkown parameter print op.
		if (i == 2) fout << "="; //if we are after the first two numbers / unkown parameters, print = .
		if (i == places[PInd]) //if this is place of unkown parameter, print unkown parameter sign.
		{
			fout << (char)UKNOWN_PARAMETER;
			PInd++;
		}
		else fout << Equation[i]; //else this is a number so print it.
	}
}

//input: string, and an index in the string (by ref).
//return: the number in this index.
int Exercise::getNumFromString(const char* ex, unsigned int& i)
{
	int res = 0; //for the result.
	while (ex[i] >= '0' && ex[i] <= '9') //if this char is a digit add it to res.
	{
		res *= 10;
		res += ex[i] - '0';
		i++;
	}
	i--; //for backing from this function to the next char after the last digit (because we have i++ in the loop of PrintExercise).
	return res;
}

//input: an answer.
//return: SOLVED if the equation was solved, CAN_BE_SOLVED if there is more uknown parameters and the equation can be solved, 
//CANT_BE_SOLVED if there is no chance to solve the equation after this answer.
unsigned int Exercise::CheckAns(int ans)
{
	unsigned int res;
	//place ans in equation array.
	Equation[places[0]] = ans;
	//if there is no more unkown parameters, check the ans.
	//if the answer is right, return solved, else return cant be solved.
	if (NOUP <= 1)
		(CheckEquation()) ? res = SOLVED : res = WRONG;
	//else if there is unkown parameters, remove this unkownn place and check if this exercise can be solved.
	//if yes return can be solved, else return cant be solved.
	else
	{
		RemoveUknown();
		CheckIfCanBeSolved() ? res = CAN_BE_SOLVED : res = CANT_BE_SOLVED;
	}
	return res;
}

//this function check if the equation is true or false, and return the answer.
bool Exercise::CheckEquation()
{
	bool res;
	switch (op)
	{
	case '+': ((Equation[0] + Equation[1]) == Equation[2]) ? res = true : res = false;
		break;
	case '-': ((Equation[0] - Equation[1]) == Equation[2]) ? res = true : res = false;
		break;
	case '/': ((Equation[0] / Equation[1]) == Equation[2]) ? res = true : res = false;
		break;
	case '*': ((Equation[0] * Equation[1]) == Equation[2]) ? res = true : res = false;
		break;
	}
	return res;
}

//this function remove the first unkown parameter place from places, and update NOUP (num of uknown parameters).
void Exercise::RemoveUknown()
{
	for (unsigned int i = 0; i < NOUP - 1; i++)
		places[i] = places[i + 1];
	NOUP--;
}

//this function check if the equation can bo solved with these known parameters.
bool Exercise::CheckIfCanBeSolved()
{
	if (ThereIsASolution()) //if there is a solution (the right side of the equation) check if can be solved.
	{
		switch (op)
		{
		case'+': return CheckIfCanBeSolvedPlusOperator(); break;
		case'-': return CheckIfCanBeSolvedMinusOperator(); break;
		case'/': return CheckIfCanBeSolvedDivisionOperator(); break;
		case'*': return CheckIfCanBeSolvedMultiplicationOperator(); break;
		default: return false; break;
		}

		return true;
	}
	return true; //return true if there is not a solution.
}

//this function check if the exercise can be solved under plus operator.
bool Exercise::CheckIfCanBeSolvedPlusOperator()
{
	int sum = 0; //for the summary of the left side.
	unsigned int PInd = 0; //for the index of places.
	for (unsigned int i = 0; i < NumOfParameters - 1; i++) //for all the left side of the equation. (the last number in the array is the solution).
	{
		//if there is still unknown parameters and i is the place of it, add 1 to PInd, else add this number to sum.
		(PInd < NOUP && i == places[PInd]) ? PInd++ : sum += Equation[i];
	}
	if (sum <= Equation[NumOfParameters - 1]) //if sum is less or equal to the solution return true, else return false.
		return true;
	else
		return false;
}

//this function check if the exercise can be solved under minus operator.
bool Exercise::CheckIfCanBeSolvedMinusOperator()
{
	bool res = true;
	if (places[0] == 1) //if the first unkown parameter is the second number. 
	//if the first number is less than the solution return false, else return true.
		(Equation[0] < Equation[2]) ? res = false : res = true;
	return res;
}

//this function check if the exercise can be solved under division operator.
bool Exercise::CheckIfCanBeSolvedDivisionOperator()
{
	bool res = true;
	if (places[0] == 1) //if the first unkown parameter is the second number. 
		//if the first number is less than the solution return false, else return true.
		(Equation[0] < Equation[2]) ? res = false : res = true;
	return res;
}

//this function check if the exercise can be solved under multiplication operator.
bool Exercise::CheckIfCanBeSolvedMultiplicationOperator()
{
	int mul = 1; //for the multiplication of the left side.
	unsigned int PInd = 0; //for the index of places.
	for (unsigned int i = 0; i < NumOfParameters - 1; i++) //for all the left side of the equation. (the last number in the array is the solution).
	{
		//if there is still unknown parameters and i is the place of it, add 1 to PInd, else add this number to sum.
		(PInd < NOUP && i == places[PInd]) ? PInd++ : mul *= Equation[i];
	}
	if (mul <= Equation[NumOfParameters - 1]) //if sum is less or equal to the solution return true, else return false.
		return true;
	else
		return false;
}

//this function check if there is a solution (the right side of the equation).
bool Exercise::ThereIsASolution()
{
	for (unsigned int i = 0; i < NOUP; i++)
	{
		if (places[i] == (NumOfParameters - 1))
			return false;
	}
	return true;
}

void Exercise::RestartExercise()
{
	for (unsigned int i = 0; i < NumOfParameters; i++)
	{
		Equation[i] = OriginalEquation[i];
	}
	for (unsigned int i = 0; i < OriginalNOUP; i++)
	{
		places[i] = Originalplaces[i];
	}
	NOUP = OriginalNOUP;
}