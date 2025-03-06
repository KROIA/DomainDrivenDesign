#pragma once

#include "DDD_base.h"
#include "utilities/IID.h"

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
			other.m_currentID = 0;
		}

		~UniqueIDDomain() = default;

		UniqueIDDomain& operator=(const UniqueIDDomain& other) = delete;
		UniqueIDDomain& operator=(UniqueIDDomain&& other) noexcept
		{
			if (this != &other)
			{
				m_currentID = other.m_currentID;
				other.m_currentID = 0;
			}
			return *this;
		}

		[[nodiscard]] ID getNextID()
		{
			return ++m_currentID;
		}
		[[nodiscard]] ID getCurrentID() const
		{
			return m_currentID;
		}
		void setCurrentID(ID id)
		{
			m_currentID = id;
		}

		void setUniqueIDFor(std::shared_ptr<IID> obj)
		{
			obj->m_id = getNextID();
		}
	private:
		ID m_currentID = 0;
	};
}