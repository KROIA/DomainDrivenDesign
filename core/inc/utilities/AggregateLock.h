#pragma once
#include "DDD_base.h"
#include <string>

namespace DDD
{
	class DDD_EXPORT AggregateLock
	{
	public:
		virtual ~AggregateLock() = default;

		virtual std::string toString() = 0;
	};
}