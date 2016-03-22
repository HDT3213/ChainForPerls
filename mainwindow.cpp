#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(AutoPlayerThread *thread, QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow), 
	autoPlayer(thread)
{
    ui->setupUi(this);

    setFixedSize(650,650);
    gap = 40;
    edge = 40;
    stoneSize = 30;
}

MainWindow::~MainWindow()
{
    delete ui;
}

Game MainWindow::getGame() {
    return game;
}

void MainWindow::paintEvent(QPaintEvent *) {
    int Len = game.getLen();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    int i, j;

    QPixmap pixmap(":/background/background.png");
    painter.drawPixmap(0,0,pixmap);

    //Draw board framework
    for (i = 0; i < Len; i++) {
          painter.drawLine(edge, edge + i * gap, edge + gap * (Len - 1), edge + i * gap);
          painter.drawLine(edge + i * gap, edge, edge + i * gap, edge + gap * (Len - 1));
    }

    //Draw mark points
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(Qt::black);
    painter.setBrush(brush);
    //central mark point
    painter.drawEllipse(320 - 5,320 - 5,10,10);
    //north-west mark point
    painter.drawEllipse(160 - 5,160 - 5,10,10);
    //north-east mark point
    painter.drawEllipse(480 - 5,160 - 5,10,10);
    //south-west mark point
    painter.drawEllipse(160 - 5,480 - 5,10,10);
    //south - east mark point
    painter.drawEllipse(480 - 5,480 - 5,10,10);

    //Draw stones
    for (i = 0; i < Len; i++) {
        for (j = 0; j < Len; j++) {
           if (game.getPos(i,j) == BLACK) {
                  brush.setColor(Qt::black);
                  painter.setBrush(brush);
                  painter.drawEllipse(i * gap + edge - 15 , j * gap + edge - 15, stoneSize, stoneSize);
            }
            else if (game.getPos(i,j) == WHITE) {
                  brush.setColor(Qt::white);
                  painter.setBrush(brush);
                  painter.drawEllipse(i * gap + edge - 15, j * gap + edge - 15, stoneSize, stoneSize);
            }
         }
      }

    //Highlight the last step
    Step lastStep = game.getLastStep();
    if (lastStep.color != VACANT) {
        brush.setColor(Qt::red);
        painter.setBrush(brush);
        painter.drawEllipse(edge + lastStep.x * gap - 5, edge + lastStep.y * gap - 5, 10, 10);
    }

}

bool MainWindow::reportWinnner(int x, int y) {
	stoneColor result = game.win(x, y);
	if (result != ILLEGAL) {
		//Terminate the game
		game.gameOver = true;
		QString winner;
		if (result == WHITE) {
			winner = "The winner is the White Player";
		}
		else if (result == BLACK) {
			winner = "The winner is the Black Player";
		}
		else if (result == VACANT) {
			winner = "Drawn Game!";
		}
		//Report the winner
		QMessageBox::information(this, "Game Over", winner, QMessageBox::Ok);
		return true;
	}
	return false;
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
	int i, lower, upper, x, y, Len = game.getLen();
	//If game is in progrss and click on the right Position
	if ( event->x() < edge && event->x() > Len*gap + edge) {
		return;
	}
	else if( event->y() < edge && event->y() > Len*gap + edge ) {
		return;
	}
	if (game.gameOver == true) {
		return;
	}

	//Searching the node of the clicked Position
    for (i = 0; i < Len; i++) {
		lower = edge + (i - 0.5)*gap;
		upper = edge + (i + 0.5)*gap;
		if ((event->x() > lower) && (event->x() < upper)) {
			x = i;
			break;
		}
	}
	for (i = 0; i < Len; i++) {
		lower = edge + (i - 0.5)*gap;
		upper = edge + (i + 0.5)*gap;
		if ((event->y() > lower) && (event->y() < upper)) {
			y = i;
			break;
		}
	}
    if (game.getPos(x,y) != VACANT) {
        return;
    }
    repaint();

	//Make a new step
	game.newStep(x, y);
	if (reportWinnner(x, y) == true) {
		return;
	}
	if (game.getAuto() == true) {
		emit needReact();
	}
    update();
}

void MainWindow::updateReact(Step step) {
	int x, y;
	if (game.getAuto() == true) {
		//Judging whether the game is over
		x = step.x;
		y = step.y;
		game.newStep(x, y);
		if (reportWinnner(x, y) == true) {
			return;
		}
	}
	update();
}

void MainWindow::on_actionRetract_triggered() {
    game.retract();
    update();
}

void MainWindow::on_actionPlayer_Player_triggered() {
    game.newGame();
    update();
}

void MainWindow::on_actionFirst_Hand_triggered()
{
    game.newGame(true,WHITE);
    autoPlayer->setAuto(WHITE,&game);
	autoPlayer->start();
    update();
}


void MainWindow::on_actionSecond_Hand_triggered()
{
    game.newGame(true,BLACK);
    autoPlayer->setAuto(BLACK,&game);
    game.newStep(7,7);
	autoPlayer->start();
    update();
}

void MainWindow::on_actionExit_triggered()
{
    exit(0);
}
