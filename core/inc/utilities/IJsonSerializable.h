#pragma once
#include "DDD_base.h"
#include <QJsonObject>

namespace DDD
{
	class IJsonSerializable
	{
	public:
		virtual ~IJsonSerializable() = default;

		/**
		 * @brief Converts the object to a JSON object.
		 * @return QJsonObject representing the object.
		 */
		virtual QJsonObject toJson() const = 0;

	};
}