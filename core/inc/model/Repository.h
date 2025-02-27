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
		typedef T AggregateType;
		Repository() = default;
		Repository(const Repository& other) = delete;
		Repository(Repository&& other) noexcept
			: m_storage(std::move(other.m_storage))
		{
			
		}
		~Repository() = default;

		bool add(const std::shared_ptr<T>& aggregate)
		{
			if (!m_storage.contains(aggregate->getID()))
			{
				m_storage.insert({ aggregate->getID(), aggregate });
				return true;
			}
#if LOGGER_LIBRARY_AVAILABLE == 1
			Logger::logError("Repository<"+ std::string(typeid(T).name()) +">::add(): Aggregate with ID " + std::to_string(aggregate->getID()) + " already exists in the repository.");
#endif
			return false;
		}
		bool remove(ID id)
		{
			auto it = m_storage.find(id);
			if (it != m_storage.end())
			{
				m_storage.erase(it);
				return true;
			}
#if LOGGER_LIBRARY_AVAILABLE == 1
			Logger::logWarning("Repository<" + std::string(typeid(T).name()) + ">::remove(): Aggregate with ID " + std::to_string(id) + " does not exists in the repository.");
#endif
			return false;
		}
		bool replace(const std::shared_ptr<T>& aggregate)
		{
			auto it = m_storage.find(aggregate->getID());
			if (it != m_storage.end())
			{
				it->second = aggregate;
				return true;
			}
#if LOGGER_LIBRARY_AVAILABLE == 1
			Logger::logWarning("Repository<" + std::string(typeid(T).name()) + ">::replace(): Aggregate with ID " + std::to_string(aggregate->getID()) + " does not exists in the repository.");
#endif
			return false;
		}
		[[nodiscard]] std::shared_ptr<T> get(ID id) const
		{
			auto it = m_storage.find(id);
			if (it != m_storage.end())
			{
				return it->second;
			}
#if LOGGER_LIBRARY_AVAILABLE == 1
			Logger::logError("Repository<" + std::string(typeid(T).name()) + ">::get(): Aggregate with ID " + std::to_string(id) + " does not exists in the repository.");
#endif
			return nullptr;
		}
		
		[[nodiscard]] std::vector<std::shared_ptr<T>> getAll() const
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

		[[nodiscard]] size_t size() const
		{
			return m_storage.size();
		}
		[[nodiscard]] bool empty() const
		{
			return m_storage.empty();
		}
		[[nodiscard]] bool contains(ID id) const
		{
			return m_storage.exists(id);
		}
		[[nodiscard]] bool contains(const T& aggregate) const
		{
			return contains(aggregate.id());
		}
		[[nodiscard]] bool contains(const std::shared_ptr<T>& aggregate) const
		{
			return contains(aggregate->id());
		}
		[[nodiscard]] bool contains(const T* aggregate) const
		{
			return contains(aggregate->id());
		}

		
	protected:
		std::unordered_map<ID, std::shared_ptr<T>> m_storage;
	};
}


