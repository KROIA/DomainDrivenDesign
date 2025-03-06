#pragma once
#include "DDD_base.h"
#include "Repository.h"
#include "Service.h"
#include "AggregateFactory.h"
#include "utilities/UniqueIDDomain.h"
#include <variant>
#include <array>

namespace DDD
{
	template <DerivedFromAggregate... Ts>
	class Model
	{
		template<DerivedFromAggregate AGG>
		struct AggregateContainer
		{
			Repository<AGG> repository;
			std::shared_ptr<AggregateFactory<AGG>> factory;
			std::vector<std::shared_ptr<AggregateService<AGG>>> services;

			AggregateContainer(UniqueIDDomain& domain)
				: repository(domain)
			{}
		};

	public:
		static constexpr size_t aggregateTypeCount = sizeof...(Ts);

		// Default constructor initializes each instance
		Model() {

		}

		template <typename FAC> std::shared_ptr<FAC> createFactory();

		template <DerivedFromService SER> std::shared_ptr<SER> createService();
		template <DerivedFromService SER> std::shared_ptr<ServiceExecutionResult> executeService();
		



		template <DerivedFromAggregate AGG> [[nodiscard]] size_t size() const;
		template <DerivedFromAggregate AGG> [[nodiscard]] bool empty() const;
		template <DerivedFromAggregate AGG> [[nodiscard]] bool contains(ID id) const;
		[[nodiscard]] bool contains(ID id) const;
		template <DerivedFromAggregate AGG> [[nodiscard]] bool contains(const AGG& aggregate) const;
		template <DerivedFromAggregate AGG> [[nodiscard]] bool contains(const std::shared_ptr<typename AGG>& aggregate) const;
		template <DerivedFromAggregate AGG>	[[nodiscard]] bool contains(const AGG* aggregate) const;

		template <DerivedFromAggregate AGG> [[nodiscard]] std::shared_ptr<AGG> getAggregate(const ID id) const;
		[[nodiscard]] std::shared_ptr<Aggregate> getAggregate(const ID id) const;
		bool removeAggregate(const ID id);
		template <DerivedFromAggregate AGG> void removeAggregates();
		void clear();
		template <DerivedFromAggregate AGG> [[nodiscard]] std::vector<std::shared_ptr<AGG>> getAggregates(const std::vector<ID>& idList) const;
		[[nodiscard]] std::vector<std::shared_ptr<Aggregate>> getAggregates(const std::vector<ID>& idList) const;
		[[nodiscard]] std::vector<std::shared_ptr<Aggregate>> getAggregates() const;

		template <DerivedFromAggregate AGG> [[nodiscard]] std::vector<ID> getIDs() const;
		[[nodiscard]] std::vector<ID> getIDs() const;

		UniqueIDDomain& getIDDomain()
		{
			return m_idDomain;
		}
	private:
		// Retrieve an instance of a specific type X
		template <typename AGG> [[nodiscard]] AggregateContainer<AGG>& getAggregateContainer();
		template <typename AGG> [[nodiscard]] const AggregateContainer<AGG>& getAggregateContainer() const;

		

		
		using VariantType = std::variant<AggregateContainer<Ts>...>;
		std::array<VariantType, aggregateTypeCount> m_domains{
			VariantType{AggregateContainer<Ts>(m_idDomain)}...
		};

		std::vector<std::shared_ptr<Service>> m_generalServices;
		UniqueIDDomain m_idDomain;
	};

	template <DerivedFromAggregate... Ts>
	template <typename FAC>
	std::shared_ptr<FAC> Model<Ts...>::createFactory()
	{
		static_assert((std::is_base_of_v<AggregateFactory<typename FAC::AggregateType>, FAC>), "FAC must be derived from AggregateFactory");
		AggregateContainer<typename FAC::AggregateType>& domain = getAggregateContainer<typename FAC::AggregateType>();
		if (domain.factory)
			domain.factory->unregister();
		std::shared_ptr<FAC> factory = std::make_shared<FAC>(&domain.repository);
		domain.factory = factory;
		return factory;
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromService SER>
	std::shared_ptr<SER> Model<Ts...>::createService()
	{
		//static_assert((std::is_base_of_v<AggregateService<typename SER::AggregateType>, SER>), "SER must be derived from Service");
		if constexpr (std::is_base_of_v<AggregateService<typename SER::AggregateType>, SER>)
		{
			AggregateContainer<typename SER::AggregateType>& domain = getAggregateContainer<typename SER::AggregateType>();

			// Check if the service already exists
			for (auto& service : domain.services) {
				if (dynamic_cast<SER*>(service.get())) {
					Logger::logError(std::string("Service: ") + typeid(SER).name() + " already exists in the model for the type: " + typeid(Model<Ts...>).name());
					return nullptr;
				}
			}

			std::shared_ptr<SER> service = std::make_shared<SER>(&domain.repository);
			domain.services.push_back(service);
			return service;
		}
		else
		{
			for (auto& service : m_generalServices)
			{
				if (dynamic_cast<SER*>(service.get())) {
					Logger::logError(std::string("Service: ") + typeid(SER).name() + " already exists in the model for the type: " + typeid(Model<Ts...>).name());
					return nullptr;
				}
			}
			std::shared_ptr<SER> service = std::make_shared<SER>();
			m_generalServices.push_back(service);
			return service;
		}
		return nullptr;
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromService SER>
	std::shared_ptr<ServiceExecutionResult> Model<Ts...>::executeService()
	{
		if constexpr ((std::is_same_v<typename SER::AggregateType, Ts> || ...))
		{
			AggregateContainer<typename SER::AggregateType>& domain = getAggregateContainer<typename SER::AggregateType>();
			for (auto& service : domain.services) {
				if (auto* ptr = dynamic_cast<SER*>(service.get())) {
					return ptr->execute();
				}
			}
			return nullptr;
		}
		else
		{
			for (auto& service : m_generalServices)
			{
				if (auto* ptr = dynamic_cast<SER*>(service.get())) {
					return ptr->execute();
				}
			}
		}
		return nullptr;
	}



	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] size_t Model<Ts...>::size() const
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.repository.size();
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] bool Model<Ts...>::empty() const
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.repository.empty();
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] bool Model<Ts...>::contains(ID id) const
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.repository.contains(id);
	}

	template <DerivedFromAggregate... Ts>
	[[nodiscard]] bool Model<Ts...>::contains(ID id) const
	{
		for (auto& agg : m_domains)
		{
			bool found = std::visit([id](auto& obj) {
				return obj.repository.contains(id);
				}, agg);
			if (found)
				return true;
		}
		return false;
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] bool Model<Ts...>::contains(const AGG& aggregate) const
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.repository.contains(aggregate.getID());
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] bool Model<Ts...>::contains(const std::shared_ptr<typename AGG>& aggregate) const
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.repository.contains(aggregate->getID());
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] bool Model<Ts...>::contains(const AGG* aggregate) const
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.repository.contains(aggregate->getID());
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] std::shared_ptr<AGG> Model<Ts...>::getAggregate(const ID id) const
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		return domain.repository.get(id);
	}

	template <DerivedFromAggregate... Ts>
	[[nodiscard]] std::shared_ptr<Aggregate> Model<Ts...>::getAggregate(const ID id) const
	{
		std::shared_ptr<Aggregate> objPtr = nullptr;
		for (auto& agg : m_domains)
		{
			std::visit([&objPtr, id](auto& obj) {
				objPtr = obj.repository.get(id);
				}, agg);
			if (objPtr)
				return objPtr;
		}
		return nullptr;
	}
	template <DerivedFromAggregate... Ts>
	bool Model<Ts...>::removeAggregate(const ID id)
	{
		for (auto& agg : m_domains)
		{
			bool found = std::visit([id](auto& obj) {
				return obj.repository.remove(id);
				}, agg);
			if (found)
				return true;
		}
		return false;
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	void Model<Ts...>::removeAggregates()
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		domain.repository.clear();
	}

	template <DerivedFromAggregate... Ts>
	void Model<Ts...>::clear()
	{
		for (auto& agg : m_domains)
		{
			std::visit([](auto& obj) {
				obj.repository.clear();
				}, agg);
		}
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] std::vector<std::shared_ptr<AGG>> Model<Ts...>::getAggregates(const std::vector<ID>& idList) const
	{
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");
		std::vector<std::shared_ptr<AGG>> objs;
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		objs.reserve(idList.size());
		for (const ID& id : idList)
		{
			std::shared_ptr<AGG> obj = domain.repository.get(id);
			if (obj)
				objs.push_back(obj);
		}
		return objs;
	}

	template <DerivedFromAggregate... Ts>
	[[nodiscard]] std::vector<std::shared_ptr<Aggregate>> Model<Ts...>::getAggregates(const std::vector<ID>& idList) const
	{
		std::vector<std::shared_ptr<Aggregate>> objs;
		for (auto& agg : m_domains)
		{
			std::visit([&objs, &idList](auto& obj) {
				for (const ID& id : idList)
				{
					std::shared_ptr<Aggregate> ins = obj.repository.get(id);
					if (ins)
						objs.push_back(ins);
				}
				}, agg);
		}
		return objs;
	}

	template <DerivedFromAggregate... Ts>
	[[nodiscard]] std::vector<std::shared_ptr<Aggregate>> Model<Ts...>::getAggregates() const
	{
		std::vector<std::shared_ptr<Aggregate>> objs;
		for (auto& agg : m_domains)
		{
			std::visit([&objs](auto& obj) {
				auto subObjs = obj.repository.getAll();
				for (auto& ins : subObjs)
					objs.push_back(ins);
				}, agg);
		}
		return objs;
	}

	template <DerivedFromAggregate... Ts>
	template <DerivedFromAggregate AGG>
	[[nodiscard]] std::vector<ID> Model<Ts...>::getIDs() const
	{
		std::vector<ID> ids;
		const AggregateContainer<AGG>& domain = getAggregateContainer<AGG>();
		std::vector<ID> subIds = domain.repository.getIDs();
		ids.insert(ids.end(), subIds.begin(), subIds.end());
		return ids;
	}

	template <DerivedFromAggregate... Ts>
	[[nodiscard]] std::vector<ID> Model<Ts...>::getIDs() const
	{
		std::vector<ID> ids;
		for (auto& agg : m_domains)
		{
			std::visit([&ids](auto& obj) {
				std::vector<ID> subIds = obj.repository.getIDs();
				ids.insert(ids.end(), subIds.begin(), subIds.end());
				}, agg);
		}
		return ids;
	}


	//
	// PRIVATE
	//

	template <DerivedFromAggregate... Ts>
	template <typename AGG>
	[[nodiscard]] Model<Ts...>::AggregateContainer<AGG>& Model<Ts...>::getAggregateContainer() {
		// Check if D exists in the variant array, otherwise static assert
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");

		for (auto& var : m_domains) {
			if (auto* ptr = std::get_if<AggregateContainer<AGG>>(&var)) {
				return *ptr;
			}
		}
		throw std::runtime_error("Aggregate container not found");
	}

	template <DerivedFromAggregate... Ts>
	template <typename AGG>
	[[nodiscard]] const  Model<Ts...>::AggregateContainer<AGG>& Model<Ts...>::getAggregateContainer() const {
		// Check if D exists in the variant array, otherwise static assert
		static_assert((std::is_same_v<AGG, Ts> || ...), "Aggregate type <AGG> not found in this model");

		for (auto& var : m_domains) {
			if (auto* ptr = std::get_if<AggregateContainer<AGG>>(&var)) {
				return *ptr;
			}
		}
		throw std::runtime_error("Aggregate container not found");
	}
}