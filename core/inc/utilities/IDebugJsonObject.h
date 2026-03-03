#pragma once
#include "DDD_base.h"
#include <QJsonObject>

namespace DDD
{
	class IDebugJsonObject
	{
	public:
		IDebugJsonObject() = default;
		IDebugJsonObject(const IDebugJsonObject&) = default;
		IDebugJsonObject(IDebugJsonObject&&) = default;

		bool operator==(const IDebugJsonObject& other) const = default;
		bool operator!=(const IDebugJsonObject& other) const = default;

		IDebugJsonObject& operator=(const IDebugJsonObject& other) = default;
		IDebugJsonObject& operator=(IDebugJsonObject&& other) noexcept = default;

		virtual QJsonObject toDebugJsonObject() const = 0;
	};
}