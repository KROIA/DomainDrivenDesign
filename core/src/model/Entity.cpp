#include "model/Entity.h"


namespace DDD
{
	Entity::Entity(const ID id)
		: IID(id)
		, m_alive(true)
		, m_parent(nullptr) // Initialize parent to nullptr
	{

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