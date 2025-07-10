#pragma once
#include "DDD_base.h"
#include <string>

namespace DDD
{
	class DDD_EXPORT User
	{
	public:
		virtual ~User() = default;


		virtual std::string toString() = 0;

	};
}