#pragma once
#include "DDD_base.h"
#include <string>

namespace DDD
{
	class AggregateLock
	{
	public:
		AggregateLock() = default;
		AggregateLock(const AggregateLock&) = default;
		AggregateLock(AggregateLock&&) noexcept = default;
		virtual ~AggregateLock() = default;

		virtual std::string toString() = 0;

		virtual void setAggregateID(const DDD::ID& ref) { m_lockedAggregateID = ref; }
		const DDD::ID& getAggregateID() const { return m_lockedAggregateID; }


	private:
		DDD::ID m_lockedAggregateID;
	};
}