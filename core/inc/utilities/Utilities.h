#pragma once
#include "DDD_base.h"
#include <string_view>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonDocument>

namespace DDD
{
	static std::string jsonToString(const QJsonObject& json, QJsonDocument::JsonFormat format = QJsonDocument::JsonFormat::Indented)
	{
		return QString(QJsonDocument(json).toJson(format)).toStdString();
	}
	static std::string jsonToString(const QJsonValue& json, QJsonDocument::JsonFormat format = QJsonDocument::JsonFormat::Indented)
	{
		return QString(QJsonDocument(json.toObject()).toJson(format)).toStdString();
	}
	static QJsonValue stringToJson(const std::string& jsonString)
	{
		return QJsonDocument::fromJson(QString::fromStdString(jsonString).toUtf8()).object();
	}

}