#pragma once
#include "DDD_base.h"
#include <QObject>
#include "utilities/IID.h"
#include "ValueObject.h"

namespace DDD
{
	/**
	 * @brief
	 * Entity class to represent an entity in the domain.
	 * An entity can be attached as a child to an aggregate.
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
	class Entity : public QObject, public IID
	{
		Q_OBJECT
	public:
		Entity(const ID id)
			: IID(id)
			, m_alive(true)
		{

		}
		~Entity() override = default;

		Entity& operator=(const Entity& other) = delete;
		Entity& operator=(Entity&& other) noexcept
		{
			m_alive = other.m_alive;
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

	signals:
		void deleteMarked(ID entityID);
		void dataChanged(ID entityID);

	protected:
		void emitDataChanged()
		{
			emit dataChanged(getID());
		}

	private:
		bool m_alive;
	};
	
}