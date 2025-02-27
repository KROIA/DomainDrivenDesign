#pragma once
#include "DDD_base.h"
#include "model/Entity.h"

namespace DDD
{
	class Aggregate : public Entity
	{
	public:
		explicit Aggregate(const ID id)
			: Entity(id)
		{}

		virtual bool addEntity(const std::shared_ptr<Entity>& entity)
		{
			if (m_entities.contains(entity->getID()))
			{
				m_entities.insert({ entity->getID(), entity });
				return true;
			}
			return false;
		}
		virtual bool removeEntity(const ID id)
		{
			auto it = m_entities.find(id);
			if (it != m_entities.end())
			{
				m_entities.erase(it);
				return true;
			}
			return false;
		}
		[[nodiscard]] virtual std::shared_ptr<Entity> getEntity(const ID id)
		{
			auto it = m_entities.find(id);
			if (it != m_entities.end())
			{
				return it->second;
			}
			return nullptr;
		}

	private:
		std::unordered_map<ID, std::shared_ptr<Entity>> m_entities;
	};
}