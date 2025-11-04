#pragma once

#include "DDD_base.h"
#include "utilities/IID.h"

namespace DDD
{
	class UniqueIDDomain
	{
	public:
		UniqueIDDomain(const std::function<bool(ID)> &reserveFunc)
			: m_tryReserveNewID(reserveFunc)
		{

		}
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
			ID nextID = m_currentID++;
			size_t timeoutCounter = 0;
			while (!m_tryReserveNewID(nextID))
			{
				nextID = m_currentID++;
				if (timeoutCounter++ > 100)
				{
					return nextID; // give up after 100 tries and hope that the new id will not collide
				}
			}
			return nextID;
		}
		[[nodiscard]] ID getCurrentID() const
		{
			return m_currentID;
		}
		void setCurrentID(ID id)
		{
			m_currentID = id;
		}

		void setCurrentIDIFLarger(ID id)
		{
			if (id > m_currentID)
			{
				m_currentID = id;
			}
		}

		void setUniqueIDFor(std::shared_ptr<IID> obj)
		{
			obj->m_id = getNextID();
		}
		bool tryAssignCustomID(std::shared_ptr<IID> obj, ID id)
		{
			if (id > m_currentID)
			{
				m_currentID = id;
				obj->m_id = id;
				return true;
			}
			return false;
		}
		bool canAssignCustomID(ID id) const 
		{
			return id > m_currentID;
		}
		void reset()
		{
			m_currentID = 0;
		}
	private:
		ID m_currentID = 0;
		std::function<bool(ID) > m_tryReserveNewID;
	};
}