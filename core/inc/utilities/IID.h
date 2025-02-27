#pragma once
#include "DDD_base.h"

namespace DDD
{
	class IID
	{
	public:
		explicit IID(const ID id)
			: m_id(id)
		{

		}
		IID(const IID& other) = default;
		IID(const IID&& other) noexcept
			: m_id(other.m_id)
		{

		}
		virtual ~IID() = default;

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