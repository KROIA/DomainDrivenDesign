#pragma once

#include "Animal.h"


class CatLeg : public Leg
{
public:
	CatLeg(DDD::ID id)
		: Leg(id)
	{}
	std::string getInfo() const override
	{
		return "Cat leg";
	}
};
class CatBody : public Body
{
public:
	CatBody(DDD::ID id)
		: Body(id)
	{}
	std::string getInfo() const override
	{
		return "Cat Body";
	}
};
class CatHead : public Head
{
public:
	CatHead(DDD::ID id)
		: Head(id)
	{}
	std::string getInfo() const override
	{
		return "Cat head";
	}
};

class Cat : public Animal
{
public:
	Cat(DDD::ID id)
		: Animal(id)
	{
		legs.push_back(std::make_shared<CatLeg>(1));
		legs.push_back(std::make_shared<CatLeg>(2));
		legs.push_back(std::make_shared<CatLeg>(3));
		legs.push_back(std::make_shared<CatLeg>(4));

		body = std::make_shared<CatBody>(5);
		head = std::make_shared<CatHead>(6);

		addEntity(legs[0]);
		addEntity(legs[1]);
		addEntity(legs[2]);
		addEntity(legs[3]);
		addEntity(body);
		addEntity(head);
	}


};

class CatFactory : public DDD::AggregateFactory<Cat>
{
public:
	CatFactory(DDD::Repository<Cat>* repo, DDD::UniqueIDDomain* idDomain)
		: AggregateFactory(repo, idDomain)
	{}
	~CatFactory() {}

	std::shared_ptr<Cat> createAggregate()
	{
		return registerInstance(std::make_shared<Cat>(getNextID()));
	}
protected:

private:

};

class CatService : public DDD::AggregateService<Cat>
{
public:
	CatService(DDD::Repository<Cat>* repository)
		: AggregateService(repository)
	{}
	~CatService() {}

	std::shared_ptr<DDD::ServiceExecutionResult> execute() override
	{
		std::cout << "AnimalService::execute " << getRepository()->size() << std::endl;
		std::vector<std::shared_ptr<Cat>> animals = getAggregates();
		for (const auto& animal : animals)
		{
			std::cout << animal->getInfo() << "\n";
		}
		return std::make_shared<DDD::ServiceExecutionResult>();
	}
protected:

};