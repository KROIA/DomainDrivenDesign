#pragma once
#include "DDD_base.h"
#include "Aggregate.h"
#include "utilities/UniqueIDDomain.h"
#include <unordered_map>
#include <functional>
#include <memory>
#include <QObject>



namespace DDD
{
	class DDD_EXPORT IRepository : public QObject
	{
		Q_OBJECT
	public:
		IRepository(std::string_view name)
			: m_name(name)
		{}
		virtual ~IRepository() = default;

		const std::string_view& getName() const
		{
			return m_name;
		}
	protected:
		virtual void onAggregateMarketForDelete(Aggregate* agg) = 0;

		void unclaimAggregate(std::shared_ptr<Aggregate> agg)
		{
			agg->m_isInRepository = false;
		}
		void claimAggregate(std::shared_ptr<Aggregate> agg)
		{
			agg->m_isInRepository = true;
		}
	public slots:
		void onAggregateMarketForDeleteSlot()
		{
			Aggregate* agg = qobject_cast<Aggregate*>(QObject::sender());
			onAggregateMarketForDelete(agg);
		}

	private:
		std::string_view m_name;
	};

	template <DerivedFromAggregate AGG>
	class Repository : public IRepository
	{
	public:
		typedef AGG AggregateType;
		Repository(UniqueIDDomain &domain)
			: IRepository(typeid(AGG).name())
			, m_idDomain(domain)
		{}
		Repository(const Repository& other) = delete;
		Repository(Repository&& other) noexcept
			: IRepository(typeid(AGG).name())
			, m_storage(std::move(other.m_storage))
			, m_idDomain(other.m_idDomain)
		{
			
		}

		Repository& operator=(const Repository& other) = delete;
		Repository& operator=(Repository&& other) noexcept
		{
			if (this != &other)
			{
				m_storage = std::move(other.m_storage);
				m_idDomain = std::move(other.m_idDomain);
			}
			return *this;
		}

#if LOGGER_LIBRARY_AVAILABLE == 1
		void attachLogger(Log::LogObject* logger)
		{
			m_logger = logger;
		}
#endif

		bool add(const std::shared_ptr<AGG>& aggregate);
		bool remove(ID id);
		//bool replace(const std::shared_ptr<AGG>& aggregate);
		[[nodiscard]] std::shared_ptr<AGG> get(ID id);
		[[nodiscard]] std::shared_ptr<const AGG> get(ID id) const;
		
		[[nodiscard]] std::vector<std::shared_ptr<AGG>> getAll();
		[[nodiscard]] std::vector<std::shared_ptr<const AGG>> getAll() const;
		[[nodiscard]] std::vector<ID> getIDs() const;
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
			return m_storage.contains(id);
		}
		[[nodiscard]] bool contains(const AGG& aggregate) const
		{
			return contains(aggregate.getID());
		}
		[[nodiscard]] bool contains(const std::shared_ptr<AGG>& aggregate) const
		{
			return contains(aggregate->getID());
		}
		[[nodiscard]] bool contains(const AGG* aggregate) const
		{
			return contains(aggregate->getID());
		}

		[[nodiscard]] std::vector<std::shared_ptr<AGG>> getDeleted()
		{
			std::vector<std::shared_ptr<AGG>> result;
			for (auto& pair : m_deleted)
				result.push_back(pair.second);
			return result;
		}
		[[nodiscard]] std::vector<std::shared_ptr<const AGG>> getDeleted() const
		{
			std::vector<std::shared_ptr<const AGG>> result;
			for (auto& pair : m_deleted)
				result.push_back(pair.second);
			return result;
		}
		void clearDeletedCache()
		{
			m_deleted.clear();
		}

		UniqueIDDomain& getIDDomain() const
		{
			return m_idDomain;
		}
		
	protected:
		


		void onAggregateMarketForDelete(Aggregate *agg) override
		{
			if (agg)
			{
#if LOGGER_LIBRARY_AVAILABLE == 1
				if (m_logger) m_logger->info("Object: " + std::to_string(agg->getID()) + " marked for delete");
#endif
				remove(agg->getID());
			}
		}
		std::unordered_map<ID, std::shared_ptr<AGG>> m_storage;
		std::unordered_map<ID, std::shared_ptr<AGG>> m_deleted;
		UniqueIDDomain& m_idDomain;

#if LOGGER_LIBRARY_AVAILABLE == 1
		Log::LogObject* m_logger = nullptr;
#endif
	};


	template <DerivedFromAggregate AGG>
	bool Repository<AGG>::add(const std::shared_ptr<AGG>& aggregate)
	{
		const ID id = aggregate->getID();

		if (contains(id)) {
			// Remove existing object
			remove(id);
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger) m_logger->warning("Repository<" + std::string(typeid(AGG).name()) + ">::add(): Aggregate with ID " + IID::getIDString(id) + " already exists in the repository, it will be replaced by the new instance.");
#endif
			/*
#if LOGGER_LIBRARY_AVAILABLE == 1
			Logger::logError("Repository<" + std::string(typeid(AGG).name()) + ">::add(): Aggregate with ID " + IID::getIDString(id) + " already exists in the repository.");
#endif
			return false;*/
		}
		if (!aggregate->isAlive())
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			if (m_logger) m_logger->error("Repository<" + std::string(typeid(AGG).name()) + ">::add(): Aggregate with ID " + IID::getIDString(id) + " is not alive.");
#endif
			return false;
		}
		if (id == INVALID_ID)
			m_idDomain.setUniqueIDFor(aggregate);
		else
		{
			m_idDomain.setCurrentIDIFLarger(id);
		}
		if (!m_storage.contains(aggregate->getID()))
		{
			claimAggregate(aggregate);
			m_storage.insert({ aggregate->getID(), aggregate });
			QObject::connect(aggregate.get(), &Aggregate::deleteMarked, this, &IRepository::onAggregateMarketForDeleteSlot);
			return true;
		}
#if LOGGER_LIBRARY_AVAILABLE == 1
		if (m_logger) m_logger->error("Repository<" + std::string(typeid(AGG).name()) + ">::add(): Aggregate with ID " + aggregate->getIDString() + " already exists in the repository.");
#endif
		return false;
	}
	template <DerivedFromAggregate AGG>
	bool Repository<AGG>::remove(ID id)
	{
		auto it = m_storage.find(id);
		if (it != m_storage.end())
		{
			QObject::disconnect(it->second.get(), &Aggregate::deleteMarked, this, &IRepository::onAggregateMarketForDeleteSlot);
			unclaimAggregate(it->second);
			m_deleted.insert({ it->first, it->second });
			m_storage.erase(it);
			return true;
		}
#if LOGGER_LIBRARY_AVAILABLE == 1
		if (m_logger) m_logger->warning("Repository<" + std::string(typeid(AGG).name()) + ">::remove(): Aggregate with ID " + IID::getIDString(id) + " does not exists in the repository.");
#endif
		return false;
	}
	/*
	template <DerivedFromAggregate AGG>
	bool Repository<AGG>::replace(const std::shared_ptr<AGG>& aggregate)
	{
		if (!aggregate->isAlive())
		{
#if LOGGER_LIBRARY_AVAILABLE == 1
			Logger::logError("Repository<" + std::string(typeid(AGG).name()) + ">::add(): Aggregate with ID " + aggregate->getIDString() + " is not alive.");
#endif
			return false;
		}
		auto it = m_storage.find(aggregate->getID());
		if (it != m_storage.end())
		{
			QObject::disconnect(it->second.get(), &Aggregate::deleteMarked, this, &IRepository::onAggregateMarketForDeleteSlot);
			unclaimAggregate(it->second);
			claimAggregate(aggregate);
			it->second = aggregate;
			QObject::connect(aggregate.get(), &Aggregate::deleteMarked, this, &IRepository::onAggregateMarketForDeleteSlot);
			return true;
		}
#if LOGGER_LIBRARY_AVAILABLE == 1
		Logger::logWarning("Repository<" + std::string(typeid(AGG).name()) + ">::replace(): Aggregate with ID " + aggregate->getIDString() + " does not exists in the repository.");
#endif
		return false;
	}*/
	template <DerivedFromAggregate AGG>
	[[nodiscard]] std::shared_ptr<AGG> Repository<AGG>::get(ID id)
	{
		auto it = m_storage.find(id);
		if (it != m_storage.end())
		{
			return it->second;
		}
#if LOGGER_LIBRARY_AVAILABLE == 1
		if (m_logger) m_logger->error("Repository<" + std::string(typeid(AGG).name()) + ">::get(): Aggregate with ID " + IID::getIDString(id) + " does not exists in the repository.");
#endif
		return nullptr;
	}

	template <DerivedFromAggregate AGG>
	[[nodiscard]] std::shared_ptr<const AGG> Repository<AGG>::get(ID id) const
	{
		auto it = m_storage.find(id);
		if (it != m_storage.end())
		{
			return it->second;
		}
#if LOGGER_LIBRARY_AVAILABLE == 1
		if (m_logger) m_logger->error("Repository<" + std::string(typeid(AGG).name()) + ">::get(): Aggregate with ID " + IID::getIDString(id) + " does not exists in the repository.");
#endif
		return nullptr;
	}


	template <DerivedFromAggregate AGG>
	[[nodiscard]] std::vector<std::shared_ptr<AGG>> Repository<AGG>::getAll()
	{
		std::vector<std::shared_ptr<AGG>> result;
		for (auto& pair : m_storage)
		{
			result.push_back(pair.second);
		}
		return result;
	}

	template <DerivedFromAggregate AGG>
	[[nodiscard]] std::vector<std::shared_ptr<const AGG>> Repository<AGG>::getAll() const
	{
		std::vector<std::shared_ptr<const AGG>> result;
		for (auto& pair : m_storage)
		{
			result.push_back(pair.second);
		}
		return result;
	}

	template <DerivedFromAggregate AGG>
	[[nodiscard]] std::vector<ID> Repository<AGG>::getIDs() const
	{
		std::vector<ID> ids;
		ids.reserve(m_storage.size());
		for (const auto& el : m_storage)
		{
			ids.emplace_back(el.first);
		}
		return ids;
	}

}


