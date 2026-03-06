#include "utilities/Utilities.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>


namespace DDD
{
	std::string jsonToString(const QJsonObject& json, QJsonDocument::JsonFormat format)
	{
		return QString(QJsonDocument(json).toJson(format)).toStdString();
	}
	std::string jsonToString(const QJsonValue& json, QJsonDocument::JsonFormat format)
	{
		if (json.isObject())
			return QString(QJsonDocument(json.toObject()).toJson(format)).toStdString();
		else if (json.isArray())
			return QString(QJsonDocument(json.toArray()).toJson(format)).toStdString();

		// Convert simple values to string
		QString result;
		if (json.isString())
			result = json.toString();
		else if (json.isDouble())
			result = QString::number(json.toDouble());
		else if (json.isBool())
			result = json.toBool() ? "true" : "false";
		else if (json.isNull())
			result = "null";
		else
			result = json.toVariant().toString(); // Fallback for other types
		return result.toStdString();
	}
	QJsonValue stringToJson(const std::string& jsonString)
	{
		return QJsonDocument::fromJson(QString::fromStdString(jsonString).toUtf8()).object();
	}
}