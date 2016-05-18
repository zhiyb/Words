#ifndef DRAWING_H
#define DRAWING_H

#include <QtWidgets>

class Drawing : public QWidget
{
	Q_OBJECT
public:
	explicit Drawing(QWidget *parent = 0);
	void setColour(QColor clr) {this->clr = clr;}
	QColor colour() const {return clr;}

signals:
	void active();

public slots:
	void clear();

protected:
	void resizeEvent(QResizeEvent *);
	void paintEvent(QPaintEvent *);
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	//void mouseReleaseEvent(QMouseEvent *e);

private:
	QPixmap pixmap;
	QPoint pos;
	QColor clr;
};

#endif // DRAWING_H
