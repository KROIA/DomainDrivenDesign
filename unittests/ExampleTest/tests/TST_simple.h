#pragma once

#include <QApplication>

#include "UnitTest.h"
#include "DDD.h"

#include "TestObjs/Animal.h"
#include "TestObjs/Cat.h"


using AnimalModel = DDD::Model<Animal, Cat>;
AnimalModel model;


class GeneralService : public DDD::Service
{
public:
	GeneralService()
		: Service()
	{}
	~GeneralService() {}

	std::shared_ptr<DDD::ServiceExecutionResult> execute() override
	{
		std::cout << "GeneralService::execute " << std::endl;
		std::vector<std::shared_ptr<DDD::Aggregate>> aggs = model.getAggregates();
		for (const auto& agg : aggs)
		{
			std::shared_ptr<Animal> animal = std::dynamic_pointer_cast<Animal>(agg);
			std::cout << animal->getInfo() << "\n";
		}
		return std::make_shared<DDD::ServiceExecutionResult>();
	}
protected:

};

class TST_simple : public UnitTest::Test
{
	TEST_CLASS(TST_simple)
public:
	TST_simple()
		: Test("TST_simple")
	{
		ADD_TEST(TST_simple::createFactories);
		ADD_TEST(TST_simple::createServices);
		ADD_TEST(TST_simple::instantiateAnimal);
		ADD_TEST(TST_simple::runAnimalService);
		ADD_TEST(TST_simple::searchAggregates);
		ADD_TEST(TST_simple::signalsAndSlots);

	}

private:

	
	std::shared_ptr<AnimalFactory> animalFactory;
	std::shared_ptr<CatFactory> catFactory;
	std::shared_ptr<AnimalService> animalService;
	std::shared_ptr<CatService> catService;
	std::shared_ptr<GeneralService> generalService;
	std::vector<DDD::ID> ids;

	// Tests
	TEST_FUNCTION(createFactories)
	{
		TEST_START;
		catFactory = model.createFactory<CatFactory>();
		TEST_ASSERT(catFactory != nullptr);
		animalFactory = model.createFactory<AnimalFactory>();
		TEST_ASSERT(animalFactory != nullptr);

		
	}

	TEST_FUNCTION(createServices)
	{
		TEST_START;
		animalService = model.createService<AnimalService>();
		TEST_ASSERT(animalService != nullptr);

		catService = model.createService<CatService>();
		TEST_ASSERT(catService != nullptr);

		generalService = model.createService<GeneralService>();
		TEST_ASSERT(generalService != nullptr);

	}





	TEST_FUNCTION(instantiateAnimal)
	{
		TEST_START;

		std::shared_ptr<Animal> animal = catFactory->createAggregate();
		TEST_ASSERT(animal != nullptr);
		TEST_ASSERT(animal->getID() > DDD::INVALID_ID);
		animal = catFactory->createAggregate();
		ids.push_back(animal->getID());
		TEST_ASSERT(animal != nullptr);
		TEST_ASSERT(animal->getID() > DDD::INVALID_ID);
		animal = catFactory->createAggregate();
		ids.push_back(animal->getID());
		TEST_ASSERT(animal != nullptr);
		TEST_ASSERT(animal->getID() > DDD::INVALID_ID);
		animal = catFactory->createAggregate();
		ids.push_back(animal->getID());
		TEST_ASSERT(animal != nullptr);
		TEST_ASSERT(animal->getID() > DDD::INVALID_ID);
	}

	TEST_FUNCTION(runAnimalService)
	{
		TEST_START;

		std::shared_ptr<DDD::ServiceExecutionResult> result1 = model.executeService<AnimalService>();
		std::shared_ptr<DDD::ServiceExecutionResult> result2 = model.executeService<CatService>();
		std::shared_ptr<DDD::ServiceExecutionResult> result3 = model.executeService<GeneralService>();
	}

	TEST_FUNCTION(searchAggregates)
	{
		TEST_START;

		// Check if the model returns the same list of ids as the ones we created
		auto modelIDs = model.getIDs();
		bool containsAllIds = true;
		for (const auto& id : ids)
		{
			if (std::find(modelIDs.begin(), modelIDs.end(), id) == modelIDs.end())
			{
				containsAllIds = false;
				break;
			}
		}
		TEST_ASSERT(containsAllIds);

		for (size_t i=0; i<ids.size(); ++i)
		{
			std::shared_ptr<Cat> animal = model.getAggregate<Cat>(ids[i]);
			TEST_ASSERT(animal != nullptr);

			TEST_ASSERT(model.contains<Cat>(ids[i]));
			TEST_ASSERT(model.contains<Cat>(animal));
			TEST_ASSERT(model.contains(animal));
			TEST_ASSERT(model.contains(animal.get()));
			TEST_ASSERT(model.contains(animal->getID()));
			TEST_ASSERT(model.contains(*animal));
		}
		for (size_t i = 0; i < ids.size(); ++i)
		{
			std::shared_ptr<DDD::Aggregate> aggregate = model.getAggregate(ids[i]);
			TEST_ASSERT(aggregate != nullptr);
		}
	}

	TEST_FUNCTION(signalsAndSlots)
	{
		TEST_START;

		std::shared_ptr<Cat> cat = model.getAggregate<Cat>(ids[0]);
		size_t entitySize = cat->getEntities().size();
		bool signalReceived = false;
		DDD::ID signalEntityID = DDD::INVALID_ID;
		// Change signal
		QObject::connect(cat.get(), &DDD::Aggregate::entityChanged, [&signalReceived, &signalEntityID](const DDD::ID entityID)
		{
			signalReceived = true;
			signalEntityID = entityID;
		});
		std::shared_ptr<CatLeg> leg = cat->getEntity<CatLeg>(Cat::LEG1);
		TEST_ASSERT(!signalReceived);
		TEST_ASSERT(leg != nullptr);
		leg->walk();
		QApplication::processEvents();
		TEST_ASSERT(signalReceived);
		TEST_ASSERT(signalEntityID == Cat::LEG1);


		// RemoveSignal
		signalReceived = false;
		QObject::connect(cat.get(), &DDD::Aggregate::entityRemoved, [&signalReceived, &signalEntityID](const DDD::ID entityID)
			{
				signalReceived = true;
				signalEntityID = entityID;
			});
		TEST_ASSERT(!signalReceived);
		TEST_ASSERT(leg != nullptr);
		cat->removeEntity(Cat::LEG1);
		QApplication::processEvents();
		TEST_ASSERT(signalReceived);
		TEST_ASSERT(signalEntityID == Cat::LEG1);
		TEST_ASSERT(cat->getEntities().size() == entitySize-1);

		// AddSignal
		signalReceived = false;
		QObject::connect(cat.get(), &DDD::Aggregate::entityAdded, [&signalReceived, &signalEntityID](const DDD::ID entityID)
			{
				signalReceived = true;
				signalEntityID = entityID;
			});
		TEST_ASSERT(!signalReceived);
		cat->addEntity(leg);
		QApplication::processEvents();
		TEST_ASSERT(signalReceived);
		TEST_ASSERT(signalEntityID == Cat::LEG1);
		TEST_ASSERT(cat->getEntities().size() == entitySize);

		// DeleteSignal
		signalReceived = false;
		QObject::connect(cat.get(), &DDD::Aggregate::deleteMarked, [&signalReceived, &signalEntityID](const DDD::ID entityID)
			{
				signalReceived = true;
				signalEntityID = entityID;
			});
		TEST_ASSERT(!signalReceived);
		cat->getEntity(Cat::LEG1)->markDeleted();
		QApplication::processEvents();
		TEST_ASSERT(signalReceived);
		TEST_ASSERT(signalEntityID == Cat::LEG1);
		TEST_ASSERT(cat->getEntities().size() == entitySize - 1);
	}

};

TEST_INSTANTIATE(TST_simple);