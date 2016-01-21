#include "drawing.h"

Drawing::Drawing(QWidget *parent) : QWidget(parent)
{
	resize(0, 0);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setFocusPolicy(Qt::ClickFocus);
}

void Drawing::clear()
{
	pixmap.fill();
	update();
}

void Drawing::resizeEvent(QResizeEvent *)
{
	QPixmap pix(width(), height());
	pix.fill();
	QPainter painter(&pix);
	painter.drawPixmap(0, 0, pixmap);
	painter.end();
	pixmap = pix;
}

void Drawing::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.drawPixmap(0, 0, pixmap);
}

void Drawing::mousePressEvent(QMouseEvent *e)
{
	e->accept();
	pos = e->pos();
	emit active();
}

void Drawing::mouseMoveEvent(QMouseEvent *e)
{
	e->accept();
	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(QPen(QBrush(clr), 2));
	painter.drawLine(pos, e->pos());
	pos = e->pos();
	update();
	//emit active();
}
