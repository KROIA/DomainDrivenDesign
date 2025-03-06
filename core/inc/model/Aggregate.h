#pragma once
#include "DDD_base.h"
#include "model/Entity.h"

namespace DDD
{
	class Aggregate : public Entity
	{
		Q_OBJECT
			friend class IRepository;
	public:
		//explicit Aggregate(const ID id)
		//	: Entity(id)
		//{}
		explicit Aggregate()
			: Entity(INVALID_ID)
		{}
		Aggregate& operator=(const Aggregate& other) = delete;
		Aggregate& operator=(Aggregate&& other) noexcept
		{
			Entity::operator=(std::move(other));
			m_entities = std::move(other.m_entities);
			return *this;
		}

		virtual bool addEntity(const std::shared_ptr<Entity>& entity)
		{
			if (!entity)
				return false;
			if (!m_entities.contains(entity->getID()))
			{
				m_entities.insert({ entity->getID(), entity });
				connect(entity.get(), &Entity::deleteMarked, this, &Aggregate::onEntityDeleteMarketd);
				connect(entity.get(), &Entity::dataChanged, this, &Aggregate::entityChanged);
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
				disconnect(it->second.get(), &Entity::deleteMarked, this, &Aggregate::onEntityDeleteMarketd);
				disconnect(it->second.get(), &Entity::dataChanged, this, &Aggregate::entityChanged);
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
		template <DerivedFromEntity ET>
		[[nodiscard]] std::shared_ptr<ET> getEntity(const ID id) const
		{
			return std::dynamic_pointer_cast<ET>(getEntity(id));
		}
		[[nodiscard]] std::vector<std::shared_ptr<Entity>> getEntities() const
		{
			std::vector<std::shared_ptr<Entity>> entities;
			entities.reserve(m_entities.size());
			for (const auto& it : m_entities)
				entities.push_back(it.second);
			return entities;
		}
		template <DerivedFromEntity ET>
		[[nodiscard]] std::vector<std::shared_ptr<ET>> getEntities() const
		{
			std::vector<std::shared_ptr<ET>> entities;
			entities.reserve(m_entities.size());
			for (const auto& it : m_entities)
			{
				std::shared_ptr<ET> entity = std::dynamic_pointer_cast<ET>(it.second);
				if (entity)
					entities.push_back(entity);
			}
			return entities;
		}

		[[nodiscard]] bool isRegistred() const
		{
			return m_isInRepository;
		}

	signals:
		void entityAdded(ID entityID);
		void entityRemoved(ID entityID);
		void entityDeleted(ID entityID);
		void entityChanged(ID entityID);

	private slots:
		void onEntityDeleteMarketd(ID entityID)
		{
			removeEntity(entityID);
			emit entityDeleted(entityID);
		}


	private:
		std::unordered_map<ID, std::shared_ptr<Entity>> m_entities;

		bool m_isInRepository;
	};
}