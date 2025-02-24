#pragma once
#include "DDD_base.h"
#include "utilities/IID.h"
#include "ValueObject.h"

namespace DDD
{
	class Entity : public IID
	{
	public:
		Entity(ID id)
			: IID(id)
		{

		}
		~Entity()
		{

		}

		/*virtual bool addValue(const std::shared_ptr<ValueObject>& value)
		{
			if (m_values.find(value->getID()) == m_values.end())
			{
				m_values.insert({ value->getID(), value });
				return true;
			}
			return false;
		}

		virtual bool removeValue(ID id)
		{
			auto it = m_values.find(id);
			if (it != m_values.end())
			{
				m_values.erase(it);
				return true;
			}
			return false;
		}

		virtual std::shared_ptr<ValueObject> getValue(ID id)
		{
			auto it = m_values.find(id);
			if (it != m_values.end())
			{
				return it->second;
			}
			return nullptr;
		}*/



	private:
		//std::unordered_map<ID, std::shared_ptr<ValueObject>> m_values;
	};
	
}