#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <chrono>
#include <random>
#include <functional>

#include <QMainWindow>
#include <QtWidgets>
#include "drawing.h"
#include "structures.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);

private slots:
	void openFile();
	void newWord();
	void showWord();
	void wordYes() {wordParamInc("yes", 1);}
	void wordSkip() {wordParamInc("skip", 1);}
	void wordNo() {wordParamInc("no", 1);}
	void drawingMode();
	void keyboardMode();

private:
	void wordParamInc(QString key, int inc);
	//QJsonDocument toJsonDocument();

	QLabel *lGroup, *lUnit, *lKana, *lKanji, *lEnglish;
	QLineEdit *leInput;
	QPushButton *pbShow, *pbLevel[3];
	QPushButton *pbOpen, *pbSave, *pbClear;
	Drawing *drawing;

	QFile docFile;
	Manager manager;

	struct status_t {
		int offset;
	} status;
};

#endif // MAINWINDOW_H
