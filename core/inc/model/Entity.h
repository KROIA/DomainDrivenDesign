#pragma once
#include "DDD_base.h"
#include <string>
#include <QObject>
#include "utilities/IID.h"
#include "utilities/Utilities.h"
#include "utilities/IJsonSerializable.h"
#include "ValueObject.h"

namespace DDD
{
	/**
	 * @brief
	 * Entity class to represent an entity in the domain.
	 * An entity can be attached as a child to an aggregate.
	 * The entity ID is not unique in a global scope, but it is unique in the scope of the aggregate.
	 *
	 * @details
	 * Signals:
	 *   deleteMarked(ID entityID) - emitted when the entity is marked for deletion
	 *   dataChanged(ID entityID) - emitted when the entity data is changed
	 *
	 * Implementation info:
	 *   When implementing a new entity, when a variable changes, the emitDataChanged() function should be called to emit the dataChanged signal.
	 *   When the object is not alive, the entity should not be used anymore.
	 *
	 *   When the entity is marked for deletion, the entity gets removed automatically from the aggregate.
	 */
	class DDD_EXPORT Entity : public QObject, public IID, public IJsonSerializable
	{
		friend class Aggregate; // Allow Aggregate to access private members
		Q_OBJECT
	public:
		Entity(const ID id)
			: IID(id)
			, m_alive(true)
			, m_parent(nullptr) // Initialize parent to nullptr
		{

		}
		~Entity() override = default;

		Entity& operator=(const Entity& other) = delete;
		Entity& operator=(Entity&& other) noexcept
		{
			m_alive = other.m_alive;
			m_parent = other.m_parent; // Move parent pointer
			IID::operator=(std::move(other));
			return *this;
		}

		void markDeleted()
		{
			m_alive = false;
			emit deleteMarked(getID());
		}
		bool isAlive() const
		{
			return m_alive;
		}

		/**
		 * @brief
		 * Converts the entity to a JSON object.
		 * This function should be overridden in derived classes to include additional data.
		 * 
		 * @return QJsonObject containing the entity data
		 */
		QJsonObject toJson() const override
		{
			QJsonObject entityData;
			entityData["id"] = QString::fromStdString(getIDString());
			entityData["alive"] = m_alive;
			return QJsonObject{
				{"Entity", entityData},
			};
		}
		virtual std::string toString()
		{
			return jsonToString(toJson());
		}

	signals:
		void deleteMarked(ID entityID);
		void dataChanged(ID entityID);

	protected:
		void emitDataChanged()
		{
			emit dataChanged(getID());
		}

		void setEntityParent(const Entity* parent)
		{
			m_parent = parent;
		}
		const Entity* getEntityParent() const {
			return m_parent;
		}

	private:
		bool m_alive;
		const Entity* m_parent;
		
	};
	
}