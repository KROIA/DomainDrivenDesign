#pragma once
#include "DDD_base.h"
#include "utilities/IID.h"

namespace DDD
{
	class MetadataContainer
	{
		
	public:
		/**
		* @brief The MetaContext class needs to be inherited by any class that wants to provide additional context during metadata loading/saving.
		* This can be used to pass extra information to the loading/saving process, such as why the load/save is being performed, which metadata is needed after loading, etc.
		*/
		class MetaContext
		{
			public:
			MetaContext() = default;
			MetaContext(const MetaContext& other) = default;
			MetaContext(MetaContext&& other) noexcept = default;
			virtual ~MetaContext() = default;
		};

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



		virtual void onLoadBegin(std::shared_ptr<MetaContext> context)
		{

		}
		virtual void onLoadEnd(std::shared_ptr<MetaContext> context)
		{

		}

		virtual void onSaveBegin(std::shared_ptr<MetaContext> context)
		{

		}
		virtual void onSaveEnd(std::shared_ptr<MetaContext> context)
		{

		}

	private:
		ID m_currentHighestID = INVALID_ID;
	};
}