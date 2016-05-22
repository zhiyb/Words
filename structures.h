#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <QMap>
#include <QVector>
#include <QString>
#include <QVariant>
#include <QVariantHash>
#include <QDateTime>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

typedef QHash<QString, double> Weight;
typedef QHash<QString, int> Type;
typedef QHash<QString, QVariantHash> Param;
typedef QVector<double> Probabilities;

class Info
{
	friend class Group;
public:
	int type(const QString &key) const {return types.value(key, QMetaType::QString);}

	Type types;
	Weight weights;
	Param params;
	double groupWeight;
	QDateTime lastTime;

private:
	QJsonObject toJsonObject() const;
	static Info fromJsonObject(const QJsonObject &object);
};

struct Entry;

class Word
{
	friend class Unit;
public:
	Word() {}
	double weight(const Info &info) const;
	QVariant &operator[](const QString key) {return fields[key];}
	const QVariant operator[](const QString key) const {return fields.value(key);}

	QVariantHash fields;

private:
	Word(const QJsonObject &obj) : fields(obj.toVariantHash()) {}
	const QJsonValue toJsonValue(const QString &key, const Info &info) const;
};

class Unit
{
	friend class Group;
public:
	Unit() {}

	QString name;
	QVector<Word> words;

private:
	Unit(const QString &name, const QJsonArray &array);
	QJsonArray toJsonArray() const;
};

class Group
{
	friend class Manager;
public:
	Group() : wordCount(0) {}

	QString name;
	Info info;
	QVector<Unit> units;
	int wordCount;

private:
	Group(const QString &name, const QJsonObject &object);
	const QJsonObject toJsonObject() const;
	void updateEntry(Entry &entry, int offset);
	void incrementWordField(const Entry &entry, const QString &key, int inc);
};

struct Entry
{
	Entry() : word(0), unit(0), group(0) {}
	bool isValid() const {return word && unit && group;}

	Word *word;
	Unit *unit;
	Group *group;
	struct {
		int global, group, unit, word;
	} offset;
};

class Manager
{
public:
	Manager();
	~Manager() {delete generator;}
	QString fromJsonDocument(QJsonDocument json);
	QJsonDocument toJsonDocument();
	Entry entryAt(int offset);
	void updateDistribution();
	void clear() {groups.clear(); wordCount = 0;}
	Entry randomWord();
	void incrementWordField(int offset, const QString &key, int inc);

	QVector<Group> groups;
	int wordCount;
	std::discrete_distribution<int> distribution;
	std::default_random_engine *generator;

private:
	void updateDistribution(const Entry &entry);
	void debugProb();

	Probabilities probabilities;
};

#endif // STRUCTURES_H
