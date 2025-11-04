#pragma once
#include "DDD_base.h"
#include "utilities/IID.h"

namespace DDD
{
	class MetadataContainer
	{
		
	public:
		virtual ~MetadataContainer() = default;

		[[nodiscard]] ID getNextObjectID() const
		{
			return m_nextObjectID;
		}
		void setNextObjectID(ID nextID)
		{
			m_nextObjectID = nextID;
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
		ID m_nextObjectID = 1;
	};
}