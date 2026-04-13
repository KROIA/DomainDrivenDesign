#include "model/Entity.h"


namespace DDD
{
	Entity::Entity(const ID id)
		: IID(id)
		, m_alive(true)
		, m_parent(nullptr) // Initialize parent to nullptr
	{

	}
	bool Entity::operator==(const Entity& other) const
	{
		// Two entities are considered equal if they have the same ID and alive status
		return getID() == other.getID() &&
			m_alive == other.m_alive;
	}
	bool Entity::operator!=(const Entity& other) const
	{
		return !(*this == other);
	}

	QJsonObject Entity::toDebugJsonObject() const
	{
		QJsonObject entityData;
		entityData["id"] = QString::fromStdString(getIDString());
		entityData["alive"] = m_alive;
		return QJsonObject{
			{"Entity", entityData},
		};
	}
	std::string Entity::toString() const
	{
		return jsonToString(toDebugJsonObject());
	}
}