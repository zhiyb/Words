#include "mainwindow.h"

#define FONT_NORMAL	24
#define FONT_LARGE	36

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	status.offset = 0;
	QFont f(font());
	f.setPointSize(FONT_NORMAL);
	f.setFamily("Calibri");
	setFont(f);

	QWidget *w = new QWidget(this);
	setCentralWidget(w);
	QVBoxLayout *lay = new QVBoxLayout(w);
	QHBoxLayout *hlay;

	QLabel *lSplit;
	lay->addLayout(hlay = new QHBoxLayout);
	hlay->addWidget(lGroup = new QLabel("Group *"));
	hlay->addWidget(lSplit = new QLabel(tr(" - ")));
	hlay->addWidget(lUnit = new QLabel("Unit *"));
	hlay->addStretch();
	f.setPointSize(FONT_NORMAL / 2);
	lGroup->setFont(f);
	lSplit->setFont(f);
	lUnit->setFont(f);

	lay->addLayout(labelsLayout = new QVBoxLayout);

	lay->addWidget(leInput = new QLineEdit);
	f.setPointSize(FONT_LARGE);
	leInput->setFont(f);

	lay->addLayout(hlay = new QHBoxLayout);
	hlay->addWidget(pbShow = new QPushButton(tr("&Show")));
	hlay->addWidget(pbLevel[0] = new QPushButton(tr("Yes (&Z)")));
	hlay->addWidget(pbLevel[1] = new QPushButton(tr("Skip (&X)")));
	hlay->addWidget(pbLevel[2] = new QPushButton(tr("No (&C)")));
	hlay->addWidget(pbOpen = new QPushButton(tr("&Open")));
	hlay->addWidget(pbClear = new QPushButton(tr("Clea&r")));

	lay->addWidget(drawing = new Drawing(this));
	drawing->setColour(Qt::black);

	connect(pbOpen, SIGNAL(clicked(bool)), this, SLOT(openFile()));
	connect(pbShow, SIGNAL(clicked(bool)), this, SLOT(showWord()));
	connect(pbLevel[0], SIGNAL(clicked(bool)), this, SLOT(wordYes()));
	connect(pbLevel[1], SIGNAL(clicked(bool)), this, SLOT(wordSkip()));
	connect(pbLevel[2], SIGNAL(clicked(bool)), this, SLOT(wordNo()));
	connect(pbClear, SIGNAL(clicked(bool)), drawing, SLOT(clear()));
	connect(drawing, SIGNAL(active()), this, SLOT(drawingMode()));
	connect(leInput, SIGNAL(cursorPositionChanged(int,int)), this, SLOT(keyboardMode()));
}

void MainWindow::openFile()
{
	QString file = QFileDialog::getOpenFileName(this, QString(), QString(), tr("Json document (*.json)"));
	if (file.isEmpty())
		return;

	QFile f(file);
	if (!f.open(QFile::ReadWrite)) {
		QMessageBox::warning(this, tr("Error open file"), tr("Cannot open file for read & write"));
		return;
	}

	QJsonParseError error;
	QJsonDocument json(QJsonDocument::fromJson(f.readAll(), &error));
	if (error.error != QJsonParseError::NoError) {
		QMessageBox::warning(this, tr("Error load JSON"), tr("Error: %1\nOffset: %2").arg(error.errorString()).arg(error.offset));
		return;
	}
	QString err;
	if (!(err = manager.fromJsonDocument(json)).isEmpty()) {
		QMessageBox::warning(this, tr("Error load data"), err);
		return;
	}

	f.close();
	if (docFile.isOpen())
		docFile.close();
	docFile.setFileName(file);
	if (!docFile.open(QFile::ReadWrite)) {
		QMessageBox::warning(this, tr("Error open file"), tr("Cannot open file for write"));
		manager.clear();
		return;
	}

	newWord();
}

void MainWindow::newWord()
{
	Entry entry = manager.randomWord();
	if (!entry.isValid())
		return;
	status.offset = entry.offset.global;

	foreach (QLabel *label, labels)
		label->setVisible(false);

	const Info &info = entry.group->info;
	const QVariantHash &fields = entry.word->fields;
	for (QVariantHash::ConstIterator it = fields.constBegin(); it != fields.constEnd(); it++) {
		if (info.type(it.key()) != QMetaType::QString)
			continue;
		if (!labels.contains(it.key())) {
			QLabel *label = new QLabel;
			label->setWordWrap(true);
			labelsLayout->addWidget(label);
			labels[it.key()] = label;
		}
		QLabel *label = labels[it.key()];
		label->setText(it.value().toString());
		const QVariantHash &param = info.params[it.key()];
		label->setVisible(!param["hidden"].toBool());
		if (param.contains("font")) {
			QFont f(font());
			f.fromString(param.value("font").toString());
			label->setFont(f);
		}
		if (param.contains("index")) {
			int index = param["index"].toInt();
			labelsLayout->takeAt(labelsLayout->indexOf(label));
			labelsLayout->insertWidget(index, label);
		}
	}

	lGroup->setText(entry.group->name);
	lUnit->setText(entry.unit->name);
	pbLevel[0]->setText(tr("Yes/%1 (&Z)").arg(fields["yes"].toInt()));
	pbLevel[1]->setText(tr("Skip/%1 (&X)").arg(fields["skip"].toInt()));
	pbLevel[2]->setText(tr("No/%1 (&C)").arg(fields["no"].toInt()));
	leInput->clear();
	drawing->clear();
	drawing->setColour(Qt::black);
}

void MainWindow::showWord()
{
	Entry entry = manager.entryAt(status.offset);
	if (!entry.isValid())
		return;

	const Info &info = entry.group->info;
	const QVariantHash &fields = entry.word->fields;
	for (QVariantHash::ConstIterator it = fields.constBegin(); it != fields.constEnd(); it++) {
		if (info.type(it.key()) != QMetaType::QString)
			continue;
		QLabel *label = labels[it.key()];
		const QVariantHash &param = info.params[it.key()];
		if (param["hidden"].toBool() && !label->text().isEmpty())
			label->setVisible(!label->isVisible());
	}

	drawing->setColour(drawing->colour() == Qt::black ? Qt::blue : Qt::black);
}

void MainWindow::drawingMode()
{
	QFont f(leInput->font());
	f.setPointSize(FONT_NORMAL / 2);
	leInput->setFont(f);
}

void MainWindow::keyboardMode()
{
	QFont f(leInput->font());
	f.setPointSize(FONT_LARGE);
	leInput->setFont(f);
}

void MainWindow::wordParamInc(QString key, int inc)
{
	manager.incrementWordField(status.offset, key, inc);
	newWord();

	if (!docFile.isOpen())
		return;
	QByteArray json = manager.toJsonDocument().toJson();
	docFile.seek(0);
	docFile.write(json);
	docFile.resize(json.size());
	docFile.flush();
}
