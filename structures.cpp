#include <chrono>
#include <random>
#include <functional>

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include <QDebug>
#include "structures.h"

using std::chrono::system_clock;

static const struct {
	const char *name;
	const float weight;
	const int type;
} defaults[] = {
	{"yes", -0.75f, QMetaType::Int},
	{"no", 1.f, QMetaType::Int},
	{"skip", 0.2f, QMetaType::Int},
	{"time", -(1.f / 24.f / 60.f / 60.f), QMetaType::QDateTime},
}, *ptr;

QJsonObject Info::toJsonObject() const
{
	QJsonObject object;

	QJsonObject weight;
	for (Weight::const_iterator it = weights.constBegin(); it != weights.constEnd(); it++)
		weight.insert(it.key(), it.value());
	object.insert("weight", weight);

	QJsonObject type;
	for (Type::const_iterator it = types.constBegin(); it != types.constEnd(); it++)
		type.insert(it.key(), QMetaType::typeName(it.value()));
	object.insert("type", type);

	object.insert("time", lastTime.toString(Qt::ISODate));
	object.insert("groupWeight", groupWeight);

	return object;
}

Info Info::fromJsonObject(const QJsonObject &object)
{
	Info info;

	const QJsonObject weight = object.value("weight").toObject();
	for (QJsonObject::const_iterator it = weight.begin(); it != weight.end(); it++)
		info.weights[it.key()] = it.value().toDouble();

	const QJsonObject type = object.value("type").toObject();
	for (QJsonObject::const_iterator it = type.begin(); it != type.end(); it++)
		info.types[it.key()] = QMetaType::type(it.value().toString().toLocal8Bit());

	if (object.contains("time"))
		info.lastTime = QDateTime::fromString(object.value("time").toString(), Qt::ISODate);
	else
		info.lastTime = QDateTime::currentDateTime();

	info.groupWeight = object.value("groupWeight").toDouble(1.f);

	ptr = defaults;
	for (unsigned int i = 0; i != sizeof(defaults) / sizeof(*ptr); i++, ptr++) {
		if (!info.weights.contains(ptr->name))
			info.weights[ptr->name] = ptr->weight;
		info.types[ptr->name] = ptr->type;
	}

	return info;
}

double Word::weight(const Info &info) const
{
	double w = 0.f;
	for (QVariantHash::const_iterator it = fields.constBegin(); it != fields.constEnd(); it++) {
		switch (info.types[it.key()]) {
		case QMetaType::Int:
		case QMetaType::Double:
			w += it.value().toDouble() * info.weights[it.key()];
			break;
		case QMetaType::QDateTime:
			QVariant v = it.value();
			w += (double)info.lastTime.secsTo(v.toDateTime()) * \
					info.weights[it.key()];
			break;
		}
	}
	return w;
}

const QJsonValue Word::toJsonValue(const QString &key, const Info &i) const
{
	switch (i.types[key]) {
	case QMetaType::Int:
		return QJsonValue(fields[key].toInt());
	//case QMetaType::QDateTime:
	//	return QJsonValue(fields[key].toDateTime().toString(Qt::ISODate));
	}
	return QJsonValue(fields[key].toString());
}

Unit::Unit(const QString &name, const QJsonArray &array)
{
	this->name = name;
	foreach (QJsonValue val, array) {
		Word w(val.toObject());
		words.append(w);
	}
}

QJsonArray Unit::toJsonArray() const
{
	QJsonArray array;
	foreach (const Word &word, words)
		array.append(QJsonObject::fromVariantHash(word.fields));
	return array;
}

Group::Group(const QString &name, const QJsonObject &object)
{
	this->name = name;
	info = Info::fromJsonObject(object.value("info").toObject());

	int count = 0;
	QJsonObject payload = object.value("payload").toObject();
	for (QJsonObject::const_iterator it = payload.begin(); it != payload.end(); it++) {
		Unit unit(it.key(), it.value().toArray());
		units.append(unit);
		count += unit.words.count();
	}
	wordCount = count;
}

const QJsonObject Group::toJsonObject() const
{
	QJsonObject object;
	object.insert("info", info.toJsonObject());

	QJsonObject payload;
	foreach (const Unit &unit, units)
		payload.insert(unit.name, unit.toJsonArray());
	object.insert("payload", payload);

	return object;
}

void Group::updateEntry(Entry &entry, int offset)
{
	if (offset >= wordCount)
		return;
	int unitOffset = 0;
	for (Unit &unit: units) {
		if (offset < unit.words.count()) {
			entry.unit = &unit;
			entry.word = &unit.words[offset];
			entry.offset.unit = unitOffset;
			entry.offset.word = offset;
			break;
		}
		offset -= unit.words.count();
		unitOffset++;
	}
}

void Group::incrementWordField(const Entry &entry, const QString &key, int inc)
{
	QDateTime current = QDateTime::currentDateTime();
	info.lastTime = current;
	Word &word = *entry.word;
	word[key] = word[key].toInt() + inc;
	word["time"] = current.toString(Qt::ISODate);
}

Manager::Manager()
{
	unsigned seed = system_clock::now().time_since_epoch().count();
	generator = new std::default_random_engine(seed);
}

QString Manager::fromJsonDocument(QJsonDocument json)
{
	if (!json.isObject())
		return QObject::tr("Invalid data format");
	clear();

	QString key;
	foreach (key, json.object().keys()) {
		QJsonObject object(json.object().value(key).toObject());
		Group group(key, object);
		if (!group.wordCount)
			continue;
		groups.push_back(group);
		wordCount += group.wordCount;
	}
	if (!wordCount)
		return QObject::tr("Did not find any words");

	updateDistribution();
	return QString();
}

QJsonDocument Manager::toJsonDocument()
{
	QJsonObject root;
	for (const Group &group: groups)
		root.insert(group.name, group.toJsonObject());
	return QJsonDocument(root);
}

Entry Manager::entryAt(int offset)
{
	Entry entry;
	entry.offset.global = offset;
	int groupOffset = 0;
	for (Group &group: groups) {
		if (offset < group.wordCount) {
			entry.group = &group;
			entry.offset.group = groupOffset;
			group.updateEntry(entry, offset);
			break;
		}
		offset -= group.wordCount;
		groupOffset++;
	}
	return entry;
}

void Manager::updateDistribution()
{
	probabilities.clear();
	for (const Group &group: groups)
		for (const Unit &unit: group.units)
			for (const Word &word: unit.words)
				probabilities.push_back(word.weight(group.info));
	distribution = std::discrete_distribution<int>(probabilities.begin(), probabilities.end());
	//debugProb();
}

Entry Manager::randomWord()
{
	if (!wordCount)
		return Entry();
	int offset = distribution(*generator);
	return entryAt(offset);
}

void Manager::incrementWordField(int offset, const QString &key, int inc)
{
	if (!wordCount)
		return;
	Entry entry = entryAt(offset);
	if (!entry.isValid())
		return;

	entry.group->incrementWordField(entry, key, inc);
	updateDistribution(entry);
}

void Manager::updateDistribution(const Entry &entry)
{
	probabilities[entry.offset.global] = \
			entry.word->weight(entry.group->info) * \
			entry.group->info.groupWeight;
	distribution = std::discrete_distribution<int>(probabilities.begin(), probabilities.end());
}

void Manager::debugProb() const
{
	QMap<double, int> map;
	for (double p: probabilities)
		map[p]++;
	qDebug() << "Probabilities:" << map;
}
