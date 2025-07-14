#pragma once
#include "DDD_base.h"
#include <string>

namespace DDD
{
	class AggregateLock
	{
	public:
		virtual ~AggregateLock() = default;

		virtual std::string toString() = 0;
	};
}