#pragma once
#include "DDD_base.h"
#include <QObject>
#include "utilities/IID.h"
#include "ValueObject.h"

namespace DDD
{
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