#ifndef GAME_H
#define GAME_H

#include <QList>
#include <QMutex>
#include <QMutexLocker>

#define MaxLen 17

typedef enum {
    BLACK,WHITE,VACANT,ILLEGAL = -1
}stoneColor;

struct Step {
    int x;
    int y;
    stoneColor color;

    Step() {}

    Step(int x,int y,stoneColor color) {
        this->x = x;
        this->y = y;
        this->color = color;
    }
};

struct Board {

	stoneColor board[MaxLen][MaxLen];
	int Len;

	void update(stoneColor board[MaxLen][MaxLen], int Len) {
		this->Len = Len;
		for (int i = 0; i < Len; i++) {
			for (int j = 0; j < Len; j++) {
				this->board[i][j] = board[i][j];
			}
		}
	}

	stoneColor pos(int i, int j) {
		if (i >= 0 && i < Len && j >= 0 && j < Len) {
			return board[i][j];
		}
		return ILLEGAL;
	}

	bool win(Step step);
};

//Manage the game
class Game {
protected:
	QMutex mutex;
public:
    Game();
	Game(Game& game);
    int getLen();   //return the length of the board
    stoneColor getPos(int x, int y);    //return the stoneColor of (x,y)
    bool getAuto();
    Step getLastStep();

    stoneColor win(int x, int y); //Judge whether there is a winner,input the last step
    bool gameOver;   //flag of game process

    void newGame(bool type = false,stoneColor color = WHITE);    //Set type to enable autoPlayer(Default:off)
    void newStep(int x, int y);     //If autoPlayer == true, this method will auto react.
    void retract();   //If autoPlayer == true, this method will retract two steps

    bool Auto;
    stoneColor AutoColor;

    int Len;
    stoneColor board[MaxLen][MaxLen];

    int stepNum;
    QList<Step> steps;  //Save the history steps
};


#endif // GAME_H
