#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "game.h"
#include "autoplayerthread.h"

#include <QMainWindow>
#include <QPainter>
#include <QMouseEvent>
#include <QList>
#include <QMessageBox>
#include <QPixmap>
#include <QPalette>

namespace Ui {
    class MainWindow;
}

/*MainWindow will paint/update the window and watch user's action
 *Game save the data of game process and provide API to manage it.
 *When an event is triggered, MainWindow::mouseReleaseEvent will respond the event
    using API provided by object,MainWindow::game, of class Game
 *AutoPlayer runs on its own thread
 *If Autoplayer is on, mouseReleaseEvent will call AutoPlayer::react() to get AutoPlayer's respond
*/

class MainWindow : public QMainWindow
{
    Q_OBJECT
signals:
	void needReact();
public:
    explicit MainWindow(AutoPlayerThread *thread, QWidget *parent = 0);
    ~MainWindow();

    void paintEvent(QPaintEvent *);
    int gap;
    int stoneSize;
    int edge;

    void mouseReleaseEvent(QMouseEvent *event);
	bool reportWinnner(int x, int y);

    Game getGame();
    Game game;//Manage the current game

    AutoPlayerThread *autoPlayer;
public slots:
	void updateReact(Step step);
private slots:
    void on_actionRetract_triggered();

    void on_actionPlayer_Player_triggered();

    void on_actionFirst_Hand_triggered();

    void on_actionSecond_Hand_triggered();

    void on_actionExit_triggered();

private:
    Ui::MainWindow *ui;
};



#endif // MAINWINDOW_H
