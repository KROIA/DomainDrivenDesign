#pragma once

#include "UnitTest.h"
#include "DDD.h"

#include "TestObjs/Animal.h"
#include "TestObjs/Cat.h"


using AnimalModel = DDD::Model<Animal, Cat>;

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

	}

private:

	AnimalModel model;
	std::shared_ptr<AnimalFactory> animalFactory;
	std::shared_ptr<CatFactory> catFactory;
	std::shared_ptr<AnimalService> animalService;
	std::shared_ptr<CatService> catService;

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
	}





	TEST_FUNCTION(instantiateAnimal)
	{
		TEST_START;

		std::shared_ptr<Animal> animal = catFactory->createAggregate();
		TEST_ASSERT(animal != nullptr);
		TEST_ASSERT(animal->getID() > DDD::INVALID_ID);
	}

	TEST_FUNCTION(runAnimalService)
	{
		TEST_START;

		std::shared_ptr<DDD::ServiceExecutionResult> result1 = model.executeService<AnimalService>();
		std::shared_ptr<DDD::ServiceExecutionResult> result2 = model.executeService<CatService>();
	}

};

TEST_INSTANTIATE(TST_simple);