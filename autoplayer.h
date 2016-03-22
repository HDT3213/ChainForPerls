#ifndef AUTOPLAYER_H
#define AUTOPLAYER_H

#include <QTime>
#include <QObject>

#include "game.h"

typedef enum {
	SAFE,
	L4L4,
	L4D4,
	L4L3,
	D4D4,
	SL4,
	SD4,
	FIVE,
	D4L3,
	JL4L3,
	JD4L3,
	L3L3,
} Condition;

typedef enum {
	INF = 800000,
	L4L4s = 750000,
	L4D4s = 700000,
	L4L3s = 650000,
	D4D4s = 600000,
	SL4s = 550000,
	SD4s = 500000,
	JL4L3s = 500000,
	D4L3s = 450000,
	JD4L3s = 400000,
	L3L3s = 350000,
} Score;

struct LinearCondition {
	stoneColor	color;
	int			stone;
	int			jump;
	bool		leftBlock;
	bool		rightBlock;
	double		score;
};

struct StepScore {
	Step step;
	double score;
};

struct ConditionScore {
	Condition condition;
	double    score;
	
	ConditionScore() {
		condition = SAFE;
		score = 1;
	}
};



struct Border {
	int west;
	int east;
	int north;
	int south; //Including the border

    Border() :west(0), east(14), north(0), south(14) {}
};


class AutoPlayer : public QObject {
     Q_OBJECT
public:
	AutoPlayer();
	void setAuto(stoneColor color, Game *game);
	Step react();
protected:
	stoneColor color;
	Game *game;

	double blockDecent;
	double scoreBase;
	double jumpBase;

	//Style
	double attack;
	double defend;
	double rash;
	int maxDepth;

	//Evaluate and Search
	LinearCondition evaluateLinearCondition(Board board, Step step, int Xoff, int Yoff); 
	ConditionScore  evaluateJointCondition(Board board, Step step);
	StepScore       evaluateEssentialStep(Board board, Border border, stoneColor color);
	//Evaluate steps that evaluateEssentialStep ignores.
	StepScore		getBestPos(Board board, Border border, stoneColor color); 
	StepScore		recurBestPos(Board board, Border border, stoneColor color);
	double			recurEvaluate(Board board, Border border, stoneColor color, int depth, double alpha, double beta);

	//Tools
	int nearestPos(QList<StepScore> steps, int Len); //Get the most nearest element from the center
	void sortLinear(LinearCondition *arr, int Len);
	void changeBorder(Border& border, int x, int y);
};

#endif // AUTOPLAYER_H
