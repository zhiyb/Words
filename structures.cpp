#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include <QDebug>
#include "structures.h"

static const struct {
	const char *name;
	const float weight;
	const int type;
} defaults[] = {
	{"yes", -0.75f, QMetaType::Int},
	{"no", 1.f, QMetaType::Int},
	{"skip", 0.2f, QMetaType::Int},
	{"time", -0.001f, QMetaType::QDateTime},
}, *ptr = defaults;

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
		info.lastTime = QDateTime::fromString(object.value("time").toString());
	else
		info.lastTime = QDateTime::currentDateTime();

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
		Unit unit;
		unit.name = it.key();
		QJsonArray array(it.value().toArray());

		foreach (QJsonValue val, array) {
			Word w(val.toObject());
			unit.words.append(w);
			probabilities.append(w.weight(info));
		}

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

void Group::debugProb() const
{
	QMap<float, int> map;
	for (float p:probabilities)
		map[p] += 1;
	qDebug() << "Probabilities:" << map;
}
