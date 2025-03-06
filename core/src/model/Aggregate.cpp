#include "model/Aggregate.h"

namespace DDD
{
	Aggregate& Aggregate::operator=(Aggregate&& other) noexcept
	{
		Entity::operator=(std::move(other));
		m_entities = std::move(other.m_entities);
		return *this;
	}

	bool Aggregate::addEntity(const std::shared_ptr<Entity>& entity)
	{
		if (!entity)
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			Logger::logError("Aggregate::addEntity(): Entity is nullptr");
#endif
			return false;
		}
		if (!m_entities.contains(entity->getID()))
		{
			m_entities.insert({ entity->getID(), entity });
			connect(entity.get(), &Entity::deleteMarked, this, &Aggregate::onEntityDeleteMarketd);
			connect(entity.get(), &Entity::dataChanged, this, &Aggregate::entityChanged);
			emit entityAdded(entity->getID());
			return true;
		}
#if LOGGER_LIBRARY_AVAILABLE == 1
		Logger::logError("Aggregate::addEntity(): Entity with ID " + entity->getIDString() + " already exists in the aggregate.");
#endif
		return false;
	}
	bool Aggregate::removeEntity(const ID id)
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
#if LOGGER_LIBRARY_AVAILABLE == 1
		Logger::logError("Aggregate::removeEntity(): Entity with ID " + IID::getIDString(id) + " does not exist in this aggregate.");
#endif
		return false;
	}
	[[nodiscard]] std::shared_ptr<Entity> Aggregate::getEntity(const ID id) const
	{
		const auto it = m_entities.find(id);
		if (it != m_entities.end())
		{
			return it->second;
		}
#if LOGGER_LIBRARY_AVAILABLE == 1
		Logger::logWarning("Aggregate::removeEntity(): Entity with ID " + IID::getIDString(id) + " does not exist in this aggregate.");
#endif
		return nullptr;
	}
	
	[[nodiscard]] std::vector<std::shared_ptr<Entity>> Aggregate::getEntities() const
	{
		std::vector<std::shared_ptr<Entity>> entities;
		entities.reserve(m_entities.size());
		for (const auto& it : m_entities)
			entities.push_back(it.second);
		return entities;
	}

}