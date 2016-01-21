#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <drawing.h>

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
	void wordNo() {wordParamInc("no", 1);}
	void drawingMode();
	void keyboardMode();

private:
	void wordParamInc(QString key, int inc);
	void debugProb() const;
	static float probability(QJsonObject word);

	QLabel *lLesson, *lKana, *lKanji, *lEnglish;
	QLineEdit *leInput;
	QPushButton *pbShow, *pbLevel[3];
	QPushButton *pbOpen, *pbSave, *pbNext, *pbClear;
	Drawing *drawing;

	QFile docFile;
	QJsonObject root;

	struct lesson_t {
		QString name;
		int count;
	};
	QVector<lesson_t> lessons;
	std::vector<float> probabilities;
	std::discrete_distribution<int> distribution;
	int wordCount;

	std::default_random_engine *generator;

	struct status_t {
		int index;
	} status;
};

#endif // MAINWINDOW_H
