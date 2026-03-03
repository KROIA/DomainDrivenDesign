#pragma once

#include "DDD_base.h"

namespace DDD
{
	class IStringifyable
	{
	public:
		IStringifyable() = default;
		IStringifyable(const IStringifyable&) = default;
		IStringifyable(IStringifyable&&) = default;

		bool operator==(const IStringifyable& other) const = default;
		bool operator!=(const IStringifyable& other) const = default;

		IStringifyable& operator=(const IStringifyable& other) = default;
		IStringifyable& operator=(IStringifyable&& other) noexcept = default;

		virtual ~IStringifyable() = default;
		virtual std::string toString() const = 0;
	};
}