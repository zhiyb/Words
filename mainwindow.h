#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
	QJsonDocument toJsonDocument();
	void debugProb() const;

	QLabel *lLesson, *lKana, *lKanji, *lEnglish;
	QLineEdit *leInput;
	QPushButton *pbShow, *pbLevel[3];
	QPushButton *pbOpen, *pbSave, *pbClear;
	Drawing *drawing;

	QFile docFile;
	Info info;
	QVector<Unit> units;

	std::vector<float> probabilities;
	std::discrete_distribution<int> distribution;
	int wordCount;

	std::default_random_engine *generator;

	struct status_t {
		int index;
	} status;
};

#endif // MAINWINDOW_H
