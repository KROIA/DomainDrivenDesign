#pragma once
#include "DDD_base.h"
#include "Aggregate.h"
#include <unordered_map>
#include <functional>
#include <memory>



namespace DDD
{
	template <DerivedFromAggregate T>
	class Repository
	{
	public:
		Repository()
		{

		}
		~Repository()
		{

		}

		void add(const std::shared_ptr<T>& aggregate)
		{
			m_storage[aggregate->getID()] = aggregate;
		}
		void remove(ID id)
		{
			m_storage.erase(id);
		}
		std::shared_ptr<T> get(ID id)
		{
			auto it = m_storage.find(id);
			if (it != m_storage.end())
			{
				return it->second;
			}
			return nullptr;
		}
		std::vector<std::shared_ptr<T>> getAll() const
		{
			std::vector<std::shared_ptr<T>> result;
			for (auto& pair : m_storage)
			{
				result.push_back(pair.second);
			}
			return result;
		}
		void clear()
		{
			m_storage.clear();
		}

		size_t size() const
		{
			return m_storage.size();
		}
		bool empty() const
		{
			return m_storage.empty();
		}
		bool contains(ID id) const
		{
			return m_storage.find(id) != m_storage.end();
		}
		bool contains(const T& aggregate) const
		{
			return contains(aggregate.id());
		}
		bool contains(const std::shared_ptr<T>& aggregate) const
		{
			return contains(aggregate->id());
		}
		bool contains(const T* aggregate) const
		{
			return contains(aggregate->id());
		}

		
	protected:
		std::unordered_map<ID, std::shared_ptr<T>> m_storage;
	};
}


