#ifndef _ISPECIFIC_GAME_H_
#define _ISPECIFIC_GAME_H_

#include <list>

using namespace std;

// this is a base class for actual games
// actual games will inherit from this class and implement the required methods
class ISpecificGame
{
public:
	// virtual fucnction with =0 is called an "abstract method"
	// abstract method must be implemented by non-abstract inherited class
	virtual bool isLevelDone()const=0;
	virtual bool isGameOver() = 0;
	virtual bool hasNextLevel()const=0;
	virtual void startLevel(bool isReplay = false)=0;
	virtual void printBoard() = 0;
	virtual void ZeroCurrentLevel() = 0;
	virtual void ZeroIsLevelDone() = 0;
	virtual void SetTotalLevels() = 0;
	// get a list with keyHits and returns a list with the keys that were used
	virtual void doIteration(const list<char>& keyHits)=0;
	virtual void doIteration() = 0;
	virtual void doSubIteration()=0;
	virtual void SetSleepTime(int SleepTime) = 0;
	virtual void SaveGame() = 0;
	virtual void SaveSolution() = 0;
	virtual bool TheBoardChecker(string boardFileName, bool& isSppFile, bool printExceptions) = 0;
	virtual void UpdateFilesLists() = 0;
	virtual size_t UploadSavedGame() = 0;
	virtual void PlaySolution() = 0;
	virtual bool StartGameFromSelectedScreen() = 0;
};

#endif