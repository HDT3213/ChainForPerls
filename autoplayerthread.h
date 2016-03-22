#ifndef AUTOPLAYERTHREAD_H
#define AUTOPLAYERTHREAD_H

#include "game.h"
#include "autoplayer.h"

#include <QThread>

class AutoPlayerThread : public QThread {
	Q_OBJECT
signals :
	void reacted(Step step);
public slots:
	void react();
public:
    AutoPlayerThread();
	void setAuto(stoneColor color, Game *game);
	AutoPlayer autoPlayer;
protected:
	void run();
};

#endif // AUTOPLAYERTHREAD_H
