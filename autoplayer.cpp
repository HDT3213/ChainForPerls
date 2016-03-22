#include "autoplayer.h"

#include <QDebug>

AutoPlayer::AutoPlayer() {
	blockDecent = 2;
	jumpBase = 8;
	scoreBase = 8;

	defend = 5;
	attack = 3;
	rash = 1;
    maxDepth = 24;

	//Random Seed
	QTime time = QTime::currentTime();
	qsrand(time.msec() + time.second() * 1000);
}

void AutoPlayer::setAuto(stoneColor color, Game *game) {
	this->color = color;
	this->game =  game;
}

void AutoPlayer::sortLinear(LinearCondition *arr, int Len) {
	//Descending
	int i, j;
	double score;
	LinearCondition temp;
	for (i = 1; i < Len; i++) {
		temp = arr[i];
		score = temp.score;
		for (j = i - 1; arr[j].score < score && j >= 0; j--) {
			arr[j + 1] = arr[j];
		}
		if (j != i - 1) {
			arr[j + 1] = temp;
		}
	}
}

int AutoPlayer::nearestPos(QList<StepScore> steps, int Len) { //Get the most nearest element from the center
	int i, t = -1, min = INF, dis;
	int arr[MaxLen * MaxLen] = {0}, top = 0;
	
	for (i = 0; i < Len; i++) {
		dis = (steps.at(i).step.x - 7)*(steps.at(i).step.x - 7) + (steps.at(i).step.y - 7)*(steps.at(i).step.y - 7);
		if (dis < min) {
			min = dis;
			t = i;
		}
	}
	arr[top++] = t;
	for (i = 0; i < Len; i++) {
		dis = (steps.at(i).step.x - 7)*(steps.at(i).step.x - 7) + (steps.at(i).step.y - 7)*(steps.at(i).step.y - 7);
		if (dis == min) {
			arr[top++] = i;
		}
	}
	i = qrand() % top;
	return arr[i];
}

inline void AutoPlayer::changeBorder(Border& border, int x, int y) {
	border.west  = (x < border.west)  ? x : border.west;
	border.east  = (x > border.east)  ? x : border.east;
	border.north = (y < border.north) ? y : border.north;
	border.south = (y > border.south) ? y : border.south;
}

LinearCondition AutoPlayer::evaluateLinearCondition(Board board, Step step, int Xoff, int Yoff) { 
	//(Xoff, Yoff) (1,0)-H (0,1)-V (-1,1)-L (1,1)-R
	int i, num, Len = game->getLen(); // num : the number of positions have been judged
	int x, y, tx, ty; //tempx, tempy
	int stone, jump;
	bool leftBlock, rightBlock, breakOut; //breakOut == true exit the loop in advance
	double score;
	Step stepOpp; //Opposite side step
	LinearCondition linear;

	leftBlock  = false;
	rightBlock = false;
	stone = 1;
	jump = 0;
	num = 0;
	stepOpp.color = stoneColor(1 - step.color); // set opposite color
	for (i = 1; i < 5; i++) {
		breakOut = true;
		num++;
		x = step.x - i * Xoff;
		y = step.y - i * Yoff;
		if (x < 0 || y < 0 || x >= Len || y >= Len) {
			leftBlock = true;
			break;
		}
		if (board.pos(x, y) == step.color) {
			stone++;
		}
		else if (board.pos(x, y) == stoneColor(1 - step.color)) {
			leftBlock = true;
			break;
		}
		else if (board.pos(x, y) == VACANT) { //Judge the next
			stepOpp.x = x;
			stepOpp.y = y;
			tx = x - Xoff;
			ty = y - Yoff;
			if (tx < 0 || ty < 0 || x >= Len || y >= Len) {
				leftBlock = false;
				break;
			}
			if (board.pos(tx, ty) == VACANT) {
				leftBlock = false;
				break;
			}
			if (board.pos(tx, ty) == step.color) {
				jump++;
			}
			else if (board.pos(tx, ty) == stoneColor(1 - step.color)) {
				leftBlock = false;
				break;
			}
		}
		breakOut = false;
	}

	//If exit the loop normally, we need to judge the left stone
	if (breakOut == false) {
		tx = x - Xoff;
		ty = y - Yoff;
		if (tx < 0 || ty < 0 || x >= Len || y >= Len) {
			leftBlock = true;
		}
		else  {
			if (board.pos(tx, ty) == VACANT) {
				leftBlock = false;
			}
			else if (board.pos(tx, ty) == stoneColor(1 - step.color)) {
				leftBlock = true;
			}
		}
	}
	num--;

	//Judge the other direction
	for (i = 1; i <= 4 - num; i++) {  //warning : unknown index limitation
		breakOut = true;
		x = step.x + i * Xoff;
		y = step.y + i * Yoff;
		if (x < 0 || y < 0 || x >= Len || y >= Len) {
			rightBlock = true;
			break;
		}
		if (board.pos(x, y) == step.color) {
			stone++;
		}
		else if (board.pos(x, y) == stoneColor(1 - step.color)) {
			rightBlock = true;
			break;
		}
		else if (board.pos(x, y) == VACANT) {
			stepOpp.x = x;
			stepOpp.y = y;
			tx = x + Xoff;
			ty = y + Yoff;
			if (tx < 0 || ty < 0 || x >= Len || y >= Len) {
				rightBlock = false;
				break;
			}
			if (board.pos(tx, ty) == VACANT) {
				rightBlock = false;
				break;
			}
			if (board.pos(tx, ty) == step.color) {
				jump++;
			}
			else if (board.pos(tx, ty) == stoneColor(1 - step.color)) {
				rightBlock = false;
				break;
			}
		}
		breakOut = false;
	}
	if (breakOut == false) {
		tx = x - Xoff;
		ty = y - Yoff;
		if (tx < 0 || ty < 0 || x >= Len || y >= Len) {
			rightBlock = true;
		}
		else  {
			if (board.pos(tx, ty) == VACANT) {
				rightBlock = false;
			}
			else if (board.pos(tx, ty) == stoneColor(1 - step.color)) {
				rightBlock = true;
			}
		}
	}

	//Evaluate the Score
	score = pow(scoreBase, stone);
	if (jump != 0) {
		score -= pow(jumpBase, jump);
	}
	if (leftBlock || rightBlock) {
		score /= blockDecent;
	}
	if (leftBlock && rightBlock) {
		score = 1;
	}

	if (stone == 5) {
		score = INF;
	}

	linear.color = step.color;
	linear.leftBlock  = leftBlock;
	linear.rightBlock = rightBlock;
	linear.score = score;
	linear.stone = stone;
	linear.jump = jump;
	return linear;
}

ConditionScore  AutoPlayer::evaluateJointCondition(Board board, Step step) { //warning : redefine judging order
	ConditionScore conditionScore;
	LinearCondition linear1, linear2, linears[4];

	//Evaluate four directions, get two essential directions
	linears[0] = evaluateLinearCondition(board, step, 1, 0);
	linears[1] = evaluateLinearCondition(board, step, 0, 1);
	linears[2] = evaluateLinearCondition(board, step, 1, 1);
	linears[3] = evaluateLinearCondition(board, step, -1, 1);
	sortLinear(linears, 4);
	linear1 = linears[0];
	linear2 = linears[1];

	//Judge joint condition
	//L4L4, L4D4, D4D4
	if (linear1.stone == 4 && linear2.stone == 4) {
		if (linear1.leftBlock == false && linear1.rightBlock == false && 
			  linear2.leftBlock == false && linear2.rightBlock == false) {

			conditionScore.condition = L4L4;
			conditionScore.score = L4L4s;
		}
		if (linear1.leftBlock == false && linear1.rightBlock == false && (linear2.leftBlock || linear2.rightBlock)) {
			//After sorting, linear1 is more dangerous.The condition ,linear2 is L4 && linear1 is not L4, is impossible.
			conditionScore.condition = L4D4;
			conditionScore.score = L4D4s;
			return conditionScore;
		}
		if (linear1.leftBlock && linear1.rightBlock && linear2.leftBlock && linear2.rightBlock) {
			conditionScore.condition = D4D4;
			conditionScore.score = D4D4s;
			return conditionScore;
		}
	}
	//L4L3
	if (linear1.stone == 4 && linear1.jump == 0 && linear1.leftBlock == false && linear1.rightBlock == false ) {
		if (linear2.stone == 3 && linear2.jump < 2 && linear2.leftBlock == false && linear2.rightBlock == false) {
			conditionScore.condition = L4L3;
			conditionScore.score = L4L3s;
			return conditionScore;
		}
	}
	//SL4
	if (linear1.stone == 4 && linear1.jump == 0 && linear1.leftBlock == false && linear1.rightBlock == false) {
		conditionScore.condition = SL4;
		conditionScore.score = SL4s;
		return conditionScore;
	}

	//SD4
	if (linear1.stone == 4 && linear1.jump == 0  
		&& (linear1.leftBlock && !linear1.rightBlock) && (!linear1.leftBlock && linear1.rightBlock) ) {
		conditionScore.condition = SD4;
		conditionScore.score = SD4s;
		return conditionScore;
	}

	//D4L3
	if (linear1.stone == 4 && linear1.jump == 0 && linear2.stone == 3 && linear2.jump < 2) {
		if (linear2.leftBlock == false && linear2.rightBlock == false) {
			conditionScore.condition = D4L3;
			conditionScore.score = D4L3s;
			return conditionScore;
		}
	}
	//L3L3
	if (linear1.stone == 3 && linear1.jump < 2 && linear1.leftBlock == false && linear1.rightBlock == false) {
		if (linear2.stone == 3 && linear2.jump < 2 && linear2.leftBlock == false && linear2.rightBlock == false) {
			conditionScore.condition = L3L3;
			conditionScore.score = L3L3s;
			return conditionScore;
		}
	}

	//JL4L3
	if (linear1.stone == 4 && linear1.jump != 0 && linear1.leftBlock == false && linear1.rightBlock == false) {
		if (linear2.stone == 3 && linear2.jump < 2 && linear2.leftBlock == false && linear2.rightBlock == false) {
			conditionScore.condition = JL4L3;
			conditionScore.score = JL4L3s;
			return conditionScore;
		}
	}
	//JD4L3
	if (linear1.stone == 4 && linear1.jump != 0 && linear2.stone == 3 && linear2.jump < 2) {
		if (linear1.leftBlock || linear1.rightBlock) {
			conditionScore.condition = JD4L3;
			conditionScore.score = JD4L3s;
			return conditionScore;
		}
	}
	//Others
	conditionScore.condition = SAFE;
	conditionScore.score = linear1.score + linear2.score;
	return conditionScore;
}


StepScore  AutoPlayer::evaluateEssentialStep(Board board, Border border, stoneColor color) {
	int i, j;
	Step step;
	StepScore stepScore;
	Condition condition;
	ConditionScore conditionScore;

	//Deadly step
	for (i = border.west; i <= border.east; i++) {
		for (j = border.north; j <= border.south; j++) {
			if (board.pos(i, j) != VACANT) {
				continue;
			}
			step.x = i;
			step.y = j;
			step.color = color;
			if (board.win(step) == true) {
				stepScore.step = step;
				stepScore.score = INF;
				return stepScore;
			}
		}
	}
	//Oppssite side deadly step
	for (i = border.west; i <= border.east; i++) {
		for (j = border.north; j <= border.south; j++) {
			if (board.pos(i, j) != VACANT) {
				continue;
			}
			step.x = i;
			step.y = j;
			step.color = stoneColor(1-color) ;
			if (i > 9 && j > 4) {
				qDebug() << "";
			}
			if (board.win(step) == true) {
				step.color = color;   //Our deadly  pos. 
				stepScore.step = step;
				stepScore.score = INF;
				return stepScore;
			}
		}
	}
	//L4L4, L4D4, D4D4, L4L3, D4L3 our side
	for (i = border.west; i <= border.east; i++) {
		for (j = border.north; j <= border.south; j++) {
			if (board.pos(i, j) != VACANT) {
				continue;
			}
			step.x = i;
			step.y = j;
			step.color = color;
			conditionScore = evaluateJointCondition(board, step);
			condition = conditionScore.condition;
			if (condition == L4L4 || condition == L4D4 || condition == L4L3 ||
				condition == D4D4 || condition == D4L3) 
			{
				stepScore.step = step;
				stepScore.score = conditionScore.score;
				return stepScore;
			}
		}
	}

	//SL4 our side
	for (i = border.west; i <= border.east; i++) {
		for (j = border.north; j <= border.south; j++) {
			if (board.pos(i, j) != VACANT) {
				continue;
			}
			step.x = i;
			step.y = j;
			step.color = color;
			conditionScore = evaluateJointCondition(board, step);
			condition = conditionScore.condition;
			if (condition == SL4) {
				stepScore.step = step;
				stepScore.score = conditionScore.score;
				return stepScore;
			}
		}
	}

	//JL4L3, JD4L3 our side
	for (i = border.west; i <= border.east; i++) {
		for (j = border.north; j <= border.south; j++) {
			if (board.pos(i, j) != VACANT) {
				continue;
			}
			step.x = i;
			step.y = j;
			step.color = color;
			conditionScore = evaluateJointCondition(board, step);
			condition = conditionScore.condition;
			if (condition == JL4L3 || condition == JD4L3 ) {
				stepScore.step = step;
				stepScore.score = conditionScore.score;
				return stepScore;
			}
		}
	}

	//L4L4, L4D4, D4D4, L4L3, D4L3 opposite side
	for (i = border.west; i <= border.east; i++) {
		for (j = border.north; j <= border.south; j++) {
			if (board.pos(i, j) != VACANT) {
				continue;
			}
			step.x = i;
			step.y = j;
			step.color = stoneColor(1-color);
			conditionScore = evaluateJointCondition(board, step);
			condition = conditionScore.condition;
			if (condition == L4L4 || condition == L4D4 || condition == L4L3 ||
				condition == D4D4 || condition == D4L3)
			{
				step.color = color;
				stepScore.step = step;
				stepScore.score = conditionScore.score;
				return stepScore;
			}
		}
	}

	//SL4 opposite side
	for (i = border.west; i <= border.east; i++) {
		for (j = border.north; j <= border.south; j++) {
			if (board.pos(i, j) != VACANT) {
				continue;
			}
			step.x = i;
			step.y = j;
			step.color = stoneColor(1-color); 
			conditionScore = evaluateJointCondition(board, step);
			condition = conditionScore.condition;
			if (condition == SL4) {
				step.color = color;
				stepScore.step = step;
				stepScore.score = conditionScore.score;
				return stepScore;
			}
		}
	}

	//JL4L3, JD4L3 our side
	for (i = border.west; i <= border.east; i++) {
		for (j = border.north;  j <= border.south; j++) {
			if (board.pos(i, j) != VACANT) {
				continue;
			}
			step.x = i;
			step.y = j;
			step.color = stoneColor(1-color);
			conditionScore = evaluateJointCondition(board, step);
			condition = conditionScore.condition;
			if (condition == JL4L3 || condition == JD4L3) {
				step.color = color;
				stepScore.step = step;
				stepScore.score = conditionScore.score;
				return stepScore;
			}
		}
	}

	//L3L3 our side
	for (i = border.west; i <= border.east; i++) {
		for (j = border.north; j <= border.south; j++) {
			if (board.pos(i, j) != VACANT) {
				continue;
			}
			step.x = i;
			step.y = j;
			step.color = color;
			conditionScore = evaluateJointCondition(board, step);
			condition = conditionScore.condition;
			if (condition == L3L3) {
				stepScore.step = step;
				stepScore.score = conditionScore.score;
				return stepScore;
			}
		}
	}
	//L3L3 opposite side
	for (i = border.west; i <= border.east; i++) {
		for (j = border.north; j <= border.south; j++) {
			if (board.pos(i, j) != VACANT) {
				continue;
			}
			step.x = i;
			step.y = j;
			step.color = stoneColor(1-color);
			conditionScore = evaluateJointCondition(board, step);
			condition = conditionScore.condition;
			if (condition == L3L3) {
				step.color = color;
				stepScore.step = step;
				stepScore.score = conditionScore.score;
				return stepScore;
			}
		}
	}
	stepScore.score = 0;
	return stepScore;
}

StepScore	AutoPlayer::getBestPos(Board board, Border border, stoneColor color) {
	int i, j, size, next;
	double score, ourMax = -INF, oppMax = -INF;
	Step step;
	StepScore stepScore, resultStepScore;
	QList<StepScore> ourStepScores, ourMaxSteps, oppStepScores, oppMaxSteps, crossMaxSteps;
	ConditionScore jointConditionScore;

	stepScore = evaluateEssentialStep(board, border, color);
	if (stepScore.score != 0) {
		return stepScore;
	}
	//Other condition
	for (i = border.west; i <= border.east; i++) {
		for (j = border.north; j <= border.south; j++) {
			if (board.pos(i, j) != VACANT) {
				continue;
			}
			step.x = i;
			step.y = j;
			step.color = color;
			jointConditionScore = evaluateJointCondition(board, step);
			score = jointConditionScore.score;
			stepScore.step  = step;
			stepScore.score = score;
			if (score > ourMax) {
				ourMax = score;
			}
			ourStepScores.append(stepScore);

			step.color = stoneColor(1 - color);
			jointConditionScore = evaluateJointCondition(board, step);
			score = jointConditionScore.score;
			stepScore.step = step;
			stepScore.score = score;
			if (score > oppMax) {
				oppMax = score;
			}
			oppStepScores.append(stepScore);

		}
	}
	for (i = 0; i < ourStepScores.size(); i++) {
		if (ourStepScores.at(i).score == ourMax || ourStepScores.at(i).score > INF - 500) {
			ourMaxSteps.append(ourStepScores.at(i));
		}
	}
	for (i = 0; i < oppStepScores.size(); i++) {
		if (oppStepScores.at(i).score == oppMax || oppStepScores.at(i).score > INF - 500) {
			oppMaxSteps.append(ourStepScores.at(i));
		}
	}
	//Get crossing stepScores
	for (i = 0; i < ourMaxSteps.size(); i++) {
		for (j = 0; j < oppMaxSteps.size(); j++) {
			if (ourMaxSteps.at(i).step.x == oppMaxSteps.at(j).step.x && 
					ourMaxSteps.at(i).step.y == oppMaxSteps.at(j).step.y)
			{
				stepScore = ourMaxSteps.at(i);
				stepScore.score = (ourMaxSteps.at(i).score + oppMaxSteps.at(j).score) / 2;
				crossMaxSteps.append(stepScore);
			}
		}
	}

	//Select the best pos
	size = crossMaxSteps.size();
	if (size >= 1) {
		next = nearestPos(crossMaxSteps, size);
		resultStepScore = crossMaxSteps.at(next);
		resultStepScore.step.color = color;
		return resultStepScore;
	}
	//Our side is about to win
	if (ourMax > 36000) {
		size = ourMaxSteps.size();
		next = qrand() % size;
		resultStepScore = ourMaxSteps.at(next);
		resultStepScore.step.color = color;
		return resultStepScore;
	}
	//Opposite side is about to win
	if (oppMax > 36000) {
		size = oppMaxSteps.size();
		next = qrand() % size;
		resultStepScore = oppMaxSteps.at(next);
		resultStepScore.step.color = color;
		return resultStepScore;
	}

	//White
	if (color == WHITE) {
		if (ourMax * attack > oppMax) {
			size = ourMaxSteps.size();
			next = nearestPos(ourMaxSteps, size);
			resultStepScore = ourMaxSteps.at(next);
			return resultStepScore;
		}
		else {
			size = oppMaxSteps.size();
			next = nearestPos(oppMaxSteps, size);
			resultStepScore = oppMaxSteps.at(next);
			return resultStepScore;
		}
	}
	else if (color == BLACK) {
		if (ourMax > oppMax * attack) {
			size = ourMaxSteps.size();
			next = nearestPos(ourMaxSteps, size);
			resultStepScore = ourMaxSteps.at(next);
			return resultStepScore;
		}
		else {
			size = oppMaxSteps.size();
			next = nearestPos(oppMaxSteps, size);
			resultStepScore = oppMaxSteps.at(next);
			return resultStepScore;
		}
	}
	resultStepScore.step.color = color;
	return resultStepScore;
}

StepScore AutoPlayer::recurBestPos(Board board, Border border, stoneColor color) {
	int i, j, maxIndex, minIndex, depth = maxDepth -1;// max - WHITE, min - BLACK 
	StepScore stepScore;
	Step step;
	QList<StepScore> stepScores;
	ConditionScore jointConditionScore;
	Border  borderDraft, borderTemp;
	Board boardDraft;
	double score, minScore, maxScore;
	maxScore = -maxDepth * INF;
	minScore = -maxScore;
	borderDraft = border;

	stepScore = evaluateEssentialStep(board, borderDraft, color);
	if (stepScore.score != 0) {
		return stepScore;
	}

	for (i = border.west; i <= border.east; i++) {
		for (j = border.north; j <= border.south; j++) {
			step.x = i;
			step.y = j;
			step.color = color;
			if (board.pos(i, j) != VACANT) {
				continue;
			}
			jointConditionScore = evaluateJointCondition(board, step);
			score = jointConditionScore.score;
			if (step.color == BLACK) {
				score *= -1;
			}
			borderTemp = border;
			boardDraft = board;
			boardDraft.board[step.x][step.y] = step.color;
			//change border
            changeBorder(borderTemp, step.x, step.y);
			score += recurEvaluate(board, borderTemp, stoneColor(1 - color), depth,INF,-INF);
			stepScore.step = step;
			stepScore.score = score;
			stepScores.append(stepScore);
			if (score > maxScore) {
				maxScore = score;
				maxIndex = stepScores.size() - 1;
			}
			else if (score < minScore) {
				minScore = score;
				minIndex = stepScores.size() - 1;
			}
		}
	}
	if (color == BLACK) {
		return stepScores.at(minIndex);
	}
	else {
		return stepScores.at(maxIndex);
	}
}

double  AutoPlayer::recurEvaluate(Board board, Border border, stoneColor color, int depth, double alpha, double beta) {
	StepScore stepScore;
	double score, curScore,best = -2*INF;

	if (depth <= 0) {
		return 0;
	}
	if (depth == 1) {
		stepScore = evaluateEssentialStep(board, border, color);
		if (color == WHITE) {
			score = stepScore.score;
		}
		else if (color == BLACK) {
			score = -stepScore.score;
		}
		return score;
	}
	stepScore = getBestPos(board, border, color);
	if (stepScore.score >= INF) {
		if (color == WHITE) {
			score = INF * maxDepth;
		}
		else if (color == BLACK) {
			score = -INF * maxDepth;
		}
		depth = 1; //warning 
		return score;
	}

	board.board[stepScore.step.x][stepScore.step.y] = stepScore.step.color;
	changeBorder(border, stepScore.step.x, stepScore.step.y);
	if (color == WHITE) {
		curScore = stepScore.score;
	}
	else if (color == BLACK) {
		curScore = -stepScore.score;
	}
	score = recurEvaluate(board, border, stoneColor(1 - color), depth - 1,-beta, -alpha) + curScore;
	if (score > best) {
		best = score;
	}
	if (best > alpha) {
		alpha = best;
	}
	if (best >= beta) {
		score = -2 * INF;
	}
	return score;
}

Step AutoPlayer::react() {
	int x, y;
	Board board;
	Border border;
	Step step;
	StepScore stepScore;

	board.update(game->board,game->Len);
	if (game->stepNum < 14) {
		stepScore = getBestPos(board, border, color);
	}
	else {
		stepScore = recurBestPos(board, border, color);
	}
    step = stepScore.step;
	x = step.x;
	y = step.y;
	return step;
}
