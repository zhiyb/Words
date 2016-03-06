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

Word::Word(const QJsonObject &obj)
{
	fields = obj.toVariantHash();
}

double Word::weight(const Info &i) const
{
	double w = 0.f;
	for (QVariantHash::const_iterator it = fields.constBegin(); it != fields.constEnd(); it++) {
		switch (i.types[it.key()]) {
		case QMetaType::Int:
		case QMetaType::Double:
			w += it.value().toDouble() * i.weights[it.key()];
			break;
		case QMetaType::QDateTime:
			QVariant v = it.value();
			w += (double)QDateTime::currentDateTime().secsTo(v.toDateTime()) * \
					i.weights[it.key()];
			break;
		}
	}
	return w;
}

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

	for (unsigned int i = 0; i != sizeof(defaults) / sizeof(*ptr); i++, ptr++) {
		if (!info.weights.contains(ptr->name))
			info.weights[ptr->name] = ptr->weight;
		info.types[ptr->name] = ptr->type;
	}

	return info;
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
