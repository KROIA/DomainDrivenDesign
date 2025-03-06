#pragma once
#include "DDD_base.h"

namespace DDD
{
	/**
	 * @brief
	 * Class to hold a unique ID.
	 */
	class IID
	{
		friend class UniqueIDDomain;
	public:
		explicit IID(const ID id)
			: m_id(id)
		{

		}
		explicit IID()
			: m_id(INVALID_ID)
		{

		}
		IID(const IID& other) = default;
		IID(const IID&& other) noexcept
			: m_id(other.m_id)
		{

		}
		virtual ~IID() = default;

		IID& operator=(const IID& other) = default;
		IID& operator=(IID&& other) noexcept
		{
			if (this != &other)
			{
				m_id = other.m_id;
			}
			return *this;
		}


		[[nodiscard]] ID getID() const
		{
			return m_id;
		}
		[[nodiscard]] std::string getIDString() const
		{
			return getIDString(m_id);
		}
		[[nodiscard]] static std::string getIDString(ID id)
		{
			return std::to_string(id);
		}
		[[nodiscard]] bool operator==(const IID& other) const
		{
			return m_id == other.m_id;
		}

	private:
		// can be set by the UniqueIDDomain class
		ID m_id;
	};
}