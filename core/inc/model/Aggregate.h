#pragma once
#include "DDD_base.h"
#include "model/Entity.h"

namespace DDD
{
	class Aggregate : public Entity
	{
		Q_OBJECT
	public:
		explicit Aggregate(const ID id)
			: Entity(id)
		{}

		virtual bool addEntity(const std::shared_ptr<Entity>& entity)
		{
			if (!entity)
				return false;
			if (!m_entities.contains(entity->getID()))
			{
				m_entities.insert({ entity->getID(), entity });
				emit entityAdded(entity->getID());
				return true;
			}
			return false;
		}
		virtual bool removeEntity(const ID id)
		{
			const auto it = m_entities.find(id);
			if (it != m_entities.end())
			{
				m_entities.erase(it);
				emit entityRemoved(id);
				return true;
			}
			return false;
		}
		[[nodiscard]] virtual std::shared_ptr<Entity> getEntity(const ID id) const
		{
			const auto it = m_entities.find(id);
			if (it != m_entities.end())
			{
				return it->second;
			}
			return nullptr;
		}
		[[nodiscard]] std::vector<std::shared_ptr<Entity>> getEntities() const
		{
			std::vector<std::shared_ptr<Entity>> entities;
			entities.reserve(m_entities.size());
			for (const auto& it : m_entities)
				entities.push_back(it.second);
			return entities;
		}

	signals:
		void entityAdded(ID entityID);
		void entityRemoved(ID entityID);
		void entityDeleted(ID entityID);

	private slots:
		void onEntityDeleteMarketd(ID entityID)
		{
			removeEntity(entityID);
			emit entityDeleted(entityID);
		}


	private:
		std::unordered_map<ID, std::shared_ptr<Entity>> m_entities;
	};
}