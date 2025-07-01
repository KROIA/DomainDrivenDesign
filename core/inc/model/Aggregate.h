#pragma once
#include "DDD_base.h"
#include "model/Entity.h"

namespace DDD
{
	/**
	 * @brief
	 * An aggregate is the root entity in a list of entities in a model.
	 * It can hold multiple entities as child.
	 * Each aggregate must have a unique ID in the whole model domain space.
	 * Entities can be identified by their ID in the aggregate.
	 * Child entity ID's can be used multiple times but only in different aggregate instances.
	 * 
	 * 
	 */
	class DDD_EXPORT Aggregate : public Entity
	{
		Q_OBJECT
			friend class IRepository;
	public:
		explicit Aggregate()
			: Entity(INVALID_ID)
		{}
		explicit Aggregate(const ID &id)
			: Entity(id)
		{}

		Aggregate& operator=(const Aggregate& other) = delete;
		Aggregate& operator=(Aggregate&& other) noexcept;

		/**
		 * @brief
		 * Add an entity to the aggregate.
		 * The "entityAdded" signal is emitted after the entity is added.
		 * @param entity 
		 * @return true if the entity was added, false otherwise.
		 */
		virtual bool addEntity(const std::shared_ptr<Entity>& entity);

		/**
		 * @brief
		 * Removes a entity from the aggregate.
		 * The "entityRemoved" signal is emitted after the entity is removed.
		 * @param id is the entity id to remove.
		 * @return true if the entity was removed, false otherwise.
		 */
		virtual bool removeEntity(const ID id);

		/**
		 * @brief
		 * Gets an entity by its ID.
		 * @param id 
		 * @return Entity pointer if the entity exists, nullptr otherwise.
		 */
		[[nodiscard]] virtual std::shared_ptr<Entity> getEntity(const ID id) const;

		/**
		 * @brief
		 * Gets all entities in the aggregate.
		 * @return vector of entity pointers.
		 */
		[[nodiscard]] virtual std::vector<std::shared_ptr<Entity>> getEntities() const;

		/**
		 * @brief
		 * Gets and casts the entity with the id to the given type.
		 * @return Entity pointer if the entity exists and is of the given type, nullptr otherwise.
		 */
		template <DerivedFromEntity ET> [[nodiscard]] std::shared_ptr<ET> getEntity(const ID id) const;

		/**
		 * @brief
		 * Gets all entities in the aggregate and casts them to the given type.
		 * @return vector of entity pointers.
		 */
		template <DerivedFromEntity ET> [[nodiscard]] std::vector<std::shared_ptr<ET>> getEntities() const;

		/**
		 * @return true, if the Aggregate is contained in a repository of a model.
		 */
		[[nodiscard]] bool isRegistred() const
		{
			return m_isInRepository;
		}

		QJsonObject toJson() override
		{
			QJsonObject data = Entity::toJson();
			QJsonObject aggregateData;
			aggregateData["isRegistred"] = m_isInRepository;
			// Add childs
			QJsonArray entitiesArray;
			for (const auto& entity : m_entities)
			{
				entitiesArray.append(entity.second->toJson());
			}
			aggregateData["ChildEntities"] = entitiesArray;
			data["Aggregate"] = aggregateData;
			return data;
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


	template <DerivedFromEntity ET>
	[[nodiscard]] std::shared_ptr<ET> Aggregate::getEntity(const ID id) const
	{
		return std::dynamic_pointer_cast<ET>(getEntity(id));
	}
	template <DerivedFromEntity ET>
	[[nodiscard]] std::vector<std::shared_ptr<ET>> Aggregate::getEntities() const
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
}