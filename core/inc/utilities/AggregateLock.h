#pragma once
#include "DDD_base.h"
#include <string>

namespace DDD
{
	class AggregateLock
	{
	public:
		AggregateLock() = default;
		AggregateLock(const AggregateLock& other)
			: m_lockedAggregateID(other.m_lockedAggregateID)
		{
			
		}
		AggregateLock(AggregateLock&& other) noexcept
			: m_lockedAggregateID(other.m_lockedAggregateID)
		{
		}
		virtual ~AggregateLock() = default;

		AggregateLock& operator=(const AggregateLock&) = default;
		AggregateLock& operator=(AggregateLock&&) noexcept = default;

		virtual std::string toString() = 0;

		virtual void setAggregateID(const DDD::ID& ref) { m_lockedAggregateID = ref; }
		const DDD::ID& getAggregateID() const { return m_lockedAggregateID; }


	private:
		DDD::ID m_lockedAggregateID;
	};
}