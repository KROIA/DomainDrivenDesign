#pragma once
#include "DDD_base.h"
#include <string_view>
#include <QJsonDocument>

namespace DDD
{
	DDD_API std::string jsonToString(const QJsonObject& json, QJsonDocument::JsonFormat format = QJsonDocument::JsonFormat::Indented);
	DDD_API std::string jsonToString(const QJsonValue& json, QJsonDocument::JsonFormat format = QJsonDocument::JsonFormat::Indented);
	DDD_API QJsonValue stringToJson(const std::string& jsonString);
}