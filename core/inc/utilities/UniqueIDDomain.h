#pragma once

#include "DDD_base.h"

namespace DDD
{
	class UniqueIDDomain
	{
	public:
		UniqueIDDomain() = default;
		UniqueIDDomain(const UniqueIDDomain& other) = delete;
		UniqueIDDomain(UniqueIDDomain&& other) noexcept
			: m_currentID(other.m_currentID)
		{
			other.m_currentID = 1;
		}

		~UniqueIDDomain() = default;

		[[nodiscard]] ID getNextID()
		{
			return m_currentID++;
		}


	private:
		ID m_currentID = 1;
	};
}