#include "game.h"

#include <QDebug>

Game::Game()  {
    Len = 15;
    stepNum = 0;
    gameOver = true;
    steps.clear();
    for (int i = 0; i< MaxLen; i++) {
        for (int j = 0; j< MaxLen; j++) {
            board[i][j] = VACANT;
        }
    }
}

Game::Game(Game& game) {
	this->Len = game.Len;
	for (int i = 0; i < Len; i++) {
		for (int j = 0; j < Len; j++) {
			this->board[i][j] = game.getPos(i, j);
		}
	}
	this->stepNum = game.stepNum;
	this->steps = game.steps;
}

void Game::newGame(bool type,stoneColor color) {
	QMutexLocker locker(&mutex);
    stepNum = 0;
    steps.clear();
    this->gameOver = false;
    for (int i = 0; i< MaxLen; i++) {
        for (int j = 0; j< MaxLen; j++) {
            board[i][j] = VACANT;
        }
    }
    this->Auto = type;
    this->AutoColor = color;
}

void Game::retract() {
	QMutexLocker locker(&mutex);
    int x,y;
	Step step;
	if (stepNum == 0 || gameOver == true) {
		return;
	}
	step = steps.back();
    x = step.x;
    y = step.y;
    board[x][y] = VACANT;
    stepNum--;
    steps.pop_back();
    if (this->Auto == true) {
        step = steps.back();
        x = step.x;
        y = step.y;
        board[x][y] = VACANT;
        stepNum--;
        steps.pop_back();
    }
}

void Game::newStep(int x,int y){
    int color;
    Step* step;
	QMutexLocker locker(&mutex);
    if (board[x][y] == VACANT )
    {
        color = stepNum++ % 2;
        board[x][y] = stoneColor(color);
        step = new Step(x,y,(stoneColor)color);
        steps.append(*step);
		delete step;
    }
}

stoneColor Game::getPos(int x, int y) {
    if (x < Len && y < Len && x >= 0 && y >= 0) {
        return board[x][y];
    }
    else  {
        return ILLEGAL;
    }
}

int Game::getLen() {
    return Len;
}

bool Game::getAuto() {
    return Auto;
}

Step Game::getLastStep() {
    if (steps.empty() == false) {
        return steps.back();
    }
    Step step(-1,-1,VACANT);
    return step;
}

stoneColor Game::win(int x0, int y0) {
	QMutexLocker locker(&mutex);
    int stones;
    int x;
    int y;
    stoneColor color = steps.back().color;
    //horizontal
    stones = 1;
    y = y0;
    for (x = x0 + 1; x < Len; x++) { //skip the central Pos
        if (stones == 5) {
            return color;
        }
		if (getPos(x, y) == color) {
			stones++;
		}
		else {
			break;
		}
    }
	if (stones == 5) {
		return color;
	}
    for (x = x0 - 1 ; x >= 0; x--) {
        if (stones == 5) {
            return color;
        }
		if (getPos(x, y) == color) {
			stones++;
		}
		else {
			break;
		}
    }
	if (stones == 5) {
		return color;
	}
    //vertical
    stones = 1;
    x = x0;
    for ( y = y0 + 1; y < Len; y++) {
        if (stones == 5) {
            return color;
        }
		if (getPos(x, y) == color) {
			stones++;
		}
		else {
			break;
		}
    }
	if (stones == 5) {
		return color;
	}
    for (y = y0 - 1 ; y >= 0; y--) {
        if (stones == 5) {
            return color;
        }
		if (getPos(x, y) == color) {
			stones++;
		}
		else {
			break;
		}
    }
	if (stones == 5) {
		return color;
	}
    //left slant
    stones = 1;
    for(x = x0 - 1, y = y0 - 1; x >= 0 &&y >= 0; x--,y--) {
        if (stones == 5) {
            return color;
        }
		if (getPos(x, y) == color) {
			stones++;
		}
		else {
			break;
		}
    }
	if (stones == 5) {
		return color;
	}
    for(x = x0 + 1, y = y0 + 1; x < Len && y < Len; x++,y++) {
        if (stones == 5) {
            return color;
        }
		if (getPos(x, y) == color) {
			stones++;
		}
		else {
			break;
		}
    }
	if (stones == 5) {
		return color;
	}
    //right slant
    stones = 1;
    for(x = x0 + 1, y = y0 - 1; x < Len && y >= 0; x++,y--) {
        if (stones == 5) {
            return color;
        }
		if (getPos(x, y) == color) {
			stones++;
		}
		else {
			break;
		}
    }
	if (stones == 5) {
		return color;
	}
    for(x = x0 - 1, y = y0 + 1; x >= 0 && y < Len; x--,y++) {
        if (stones == 5) {
            return color;
        }
		if (getPos(x, y) == color) {
			stones++;
		}
		else {
			break;
		}
    }
	if (stones == 5) {
		return color;
	}
	if (stepNum == 255) {
		return VACANT;
	}
    return ILLEGAL;
}

bool Board::win(Step step) {
	int x, y, stones;
	int x0 = step.x, y0 = step.y;
	stoneColor color = step.color;

	stones = 1;
	y = y0;
	for (x = x0 + 1; x < Len; x++) { //skip the central Pos
		if (stones == 5) {
			return true;
		}
		if (pos(x, y) == color) {
			stones++;
		}
		else {
			break;
		}
	}
	if (stones == 5) {
		return true;
	}
	for (x = x0 - 1; x >= 0; x--) {
		if (stones == 5) {
			return true;
		}
		if (pos(x, y) == color) {
			stones++;
		}
		else {
			break;
		}
	}
	if (stones == 5) {
		return true;
	}
	//vertical
	stones = 1;
	x = x0;
	for (y = y0 + 1; y < Len; y++) {
		if (stones == 5) {
			return true;
		}
		if (pos(x, y) == color) {
			stones++;
		}
		else {
			break;
		}
	}
	if (stones == 5) {
		return true;
	}
	for (y = y0 - 1; y >= 0; y--) {
		if (stones == 5) {
			return true;
		}
		if (pos(x, y) == color) {
			stones++;
		}
		else {
			break;
		}
	}
	if (stones == 5) {
		return true;
	}
	//left slant
	stones = 1;
	for (x = x0 - 1, y = y0 - 1; x >= 0 && y >= 0; x--, y--) {
		if (stones == 5) {
			return true;
		}
		if (pos(x, y) == color) {
			stones++;
		}
		else {
			break;
		}
	}
	if (stones == 5) {
		return true;
	}
	for (x = x0 + 1, y = y0 + 1; x < Len && y < Len; x++, y++) {
		if (stones == 5) {
			return true;
		}
		if (pos(x, y) == color) {
			stones++;
		}
		else {
			break;
		}
	}
	if (stones == 5) {
		return true;
	}
	//right slant
	stones = 1;
	for (x = x0 + 1, y = y0 - 1; x < Len && y >= 0; x++, y--) {
		if (stones == 5) {
			return true;
		}
		if (pos(x, y) == color) {
			stones++;
		}
		else {
			break;
		}
	}
	if (stones == 5) {
		return true;
	}
	for (x = x0 - 1, y = y0 + 1; x >= 0 && y < Len; x--, y++) {
		if (stones == 5) {
			return true;
		}
		if (pos(x, y) == color) {
			stones++;
		}
		else {
			break;
		}
	}
	if (stones == 5) {
		return true;
	}
	return false;
}




