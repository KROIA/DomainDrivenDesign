#pragma once
#include "DDD_base.h"
#include "Repository.h"
#include "utilities/UniqueIDDomain.h"

namespace DDD
{
	/*
	* @brief
	* Factory class to create new aggregates.
	* 
	* @details
	* Create a derived class from this factory and implement one ore more create() methodes.
	* The create() methodes should create a new instance of the aggregate and register it in the repository.
	* Example:
	* 
	*	class AnimalFactory : public DDD::AggregateFactory<Animal>
	*	{
	*	public:
	*		AnimalFactory(DDD::Repository<Animal>* repo)
	*			: AggregateFactory(repo)
	*		{}
	*		~AnimalFactory() {}
	*
	*		// Create a new instance of the aggregate and register it in the repository
	*		std::shared_ptr<Animal> createAggregate()
	*		{
	* 			// the registerInstance() method will register the new instance in the model
	*           // When the aggrigate gets instantiated with a id == 0, the model will generate a new unique id, since 0 is not a valid id
	*			// When the id is != 0, and the model contains already a aggregate with the same id, the aggregate gets replaced by the new one.	
	*			// THe aggregate id is globally unique in the model. Different aggregate types can't have the same id.
	*			return registerInstance(std::make_shared<Animal>());
	*		}
	*	protected:
	*
	*	private:
	*
	*	};
	* 
	*  The factory must be registered in the model:
	*  DDD::Model<Animal> model;
	*  std::shared_ptr<AnimalFactory> animalFactory = model.createFactory<AnimalFactory>();
	* 
	*  The factory can be used to create new instances of the aggregate:
	*  animalFactory->createAggregate();
	* 
	* 
	* 
	*/
	template <DerivedFromAggregate AGG>
	class AggregateFactory
	{
	public:
		typedef AGG AggregateType;

		AggregateFactory(Repository<AGG>* repo)
			: m_repository(repo)
		{}
		void unregister()
		{
			m_repository = nullptr;
		}
		static std::string getName()
		{
			return typeid(AGG).name();;
		}
	protected:
		std::shared_ptr<AGG> registerInstance(std::shared_ptr<AGG> agg)
		{
			if (m_repository)
			{
				if (m_repository->add(agg))
					return agg;
#if LOGGER_LIBRARY_AVAILABLE == 1
				Logger::logError("AggregateFactory<AGG>::registerInstance(): Can't register object");
#endif
			}
#if LOGGER_LIBRARY_AVAILABLE == 1
			Logger::logError("AggregateFactory<AGG>::registerInstance(): This factory does not have a repository reference set.");
#endif
			return nullptr;
		}
	private:
		Repository<AGG>* m_repository;
	};
}