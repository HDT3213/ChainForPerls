#include "autoplayerthread.h"

AutoPlayerThread::AutoPlayerThread() {

}

void AutoPlayerThread::setAuto(stoneColor color, Game *game) {
	autoPlayer.setAuto(color, game);
}

void AutoPlayerThread::run() {
	this->exec();
}

void AutoPlayerThread::react() {
	Step step;
	step = autoPlayer.react();
	emit reacted(step);
}