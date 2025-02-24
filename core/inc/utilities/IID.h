#pragma once
#include "DDD_base.h"

namespace DDD
{
	class IID
	{
	public:
		IID(ID id)
			: m_id(id)
		{

		}
		virtual ~IID()
		{

		}

		ID getID() const
		{
			return m_id;
		}
		bool operator==(const IID& other) const
		{
			return m_id == other.m_id;
		}

	private:

		ID m_id;
	};
}