#pragma once

#include "DDD_base.h"
#include "utilities/IID.h"

namespace DDD
{
	class UniqueIDDomain
	{
	public:
		// reserveFunc: Function that takes the base ID and the amount of IDs to reserve, and returns true if the IDs were successfully reserved
		UniqueIDDomain(const std::function<bool(ID, ID)> &reserveFunc)
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
			ID nextID = ++m_currentID;
			size_t timeoutCounter = 0;
			while (!m_tryReserveNewID(nextID, 1))
			{
				++nextID;
				if (timeoutCounter++ > 100)
				{
					m_currentID = nextID;
					return nextID; // give up after 100 tries and hope that the new id will not collide
				}
			}
			return nextID;
		}
		[[nodiscard]] ID getNextID(ID amount)
		{
			ID nextID = ++m_currentID;
			size_t timeoutCounter = 0;
			while (!m_tryReserveNewID(nextID, amount))
			{
				++nextID;
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
		void setUniqueIDFor(const std::vector<std::shared_ptr<IID>>& objs)
		{
			ID nextID = getNextID(objs.size());
			ID currentID = nextID;
			for (auto& obj : objs)
			{
				if (!obj)
					continue;
				obj->m_id = currentID;
				++currentID;
			}
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
		std::function<bool(ID, ID) > m_tryReserveNewID;
	};
}