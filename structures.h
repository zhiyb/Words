#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <QMap>
#include <QVector>
#include <QString>
#include <QVariant>
#include <QVariantHash>

typedef QHash<QString, double> Weight;
typedef QHash<QString, int> Type;

struct Info
{
	QJsonObject toJsonObject() const;
	static Info fromJsonObject(const QJsonObject &object);

	Weight weights;
	Type types;
};

struct Word
{
	Word() {}
	Word(const QJsonObject &obj);
	double weight(const Info &i) const;
	QVariant &operator[](const QString key) {return fields[key];}
	const QVariant operator[](const QString key) const {return fields.value(key);}
	const QJsonValue toJsonValue(const QString &key, const Info &i) const;

	QVariantHash fields;
};

struct Unit
{
	QJsonArray toJsonArray() const;

	QString name;
	QVector<Word> words;
	Weight weightList;
};

#endif // STRUCTURES_H
