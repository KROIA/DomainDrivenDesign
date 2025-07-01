#pragma once
#include "DDD_base.h"
#include <vector>
#include "model/Aggregate.h"
#include "ValidationResult.h"




namespace DDD
{
	class DDD_EXPORT Validator
	{
	public:
		Validator() = default;
		virtual ~Validator() = default;

		virtual ValidationResult validate(std::shared_ptr<Aggregate> aggregate) const = 0;
	private:
	};
}