#pragma once
#include "DDD_base.h"
#include "utilities/IID.h"

namespace DDD
{
	class MetadataContainer
	{
		
	public:
		MetadataContainer() = default;
		MetadataContainer(const MetadataContainer& other) 
			: m_currentHighestID(other.m_currentHighestID)
		{
		}
		MetadataContainer(MetadataContainer&& other) noexcept
			: m_currentHighestID(other.m_currentHighestID)
		{
			other.m_currentHighestID = INVALID_ID;
		}
		virtual ~MetadataContainer() = default;

		[[nodiscard]] ID getCurrentHighestID() const
		{
			return m_currentHighestID;
		}
		void setCurrentHighestID(ID id)
		{
			m_currentHighestID = id;
		}



		virtual void onLoadBegin()
		{

		}
		virtual void onLoadEnd()
		{

		}

		virtual void onSaveBegin()
		{

		}
		virtual void onSaveEnd()
		{

		}

	private:
		ID m_currentHighestID = INVALID_ID;
	};
}