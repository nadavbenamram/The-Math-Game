#ifndef _KEYS_BY_CLOCK_ITERATION_H_
#define _KEYS_BY_CLOCK_ITERATION_H_

#define MAX_KEYS 2

#include <fstream>

using namespace std;

class TheMathGame;

class KeysByClockIteration {
	size_t clock_iteration;
	char Keys[MAX_KEYS];
	size_t NumOfKeys = 0;

	friend TheMathGame;
public:
	KeysByClockIteration() {};
	KeysByClockIteration(size_t CI, char ch) : clock_iteration(CI), Keys{ ch }, NumOfKeys (1){};
	KeysByClockIteration(size_t CI, char ch1, char ch2) : clock_iteration(CI), Keys{ ch1, ch2 }, NumOfKeys(2) {};
	void set(size_t cl, char ch1)
	{
		clock_iteration = cl;
		Keys[NumOfKeys++] = ch1;
	}
	void set(size_t cl, char ch1, char ch2)
	{
		clock_iteration = cl;
		Keys[NumOfKeys++] = ch1;
		Keys[NumOfKeys++] = ch2;
	}
	void _print();
	void _OutToFile(ofstream& fout);
	bool CheckIfSameKeys(char key);
	bool CheckIfSameKeys(char key1, char key2);
};

#endif