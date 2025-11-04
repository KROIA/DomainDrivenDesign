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
	void walk()
	{
		emitDataChanged();
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
	enum EntityID : DDD::ID
	{
		LEG1 = 1,
		LEG2 = 2,
		LEG3 = 3,
		LEG4 = 4,
		BODY = 5,
		HEAD = 6
	};
	Cat()
		: Animal()
	{
		legs.push_back(std::make_shared<CatLeg>(LEG1));
		legs.push_back(std::make_shared<CatLeg>(LEG2));
		legs.push_back(std::make_shared<CatLeg>(LEG3));
		legs.push_back(std::make_shared<CatLeg>(LEG4));

		body = std::make_shared<CatBody>(BODY);
		head = std::make_shared<CatHead>(HEAD);

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
	CatFactory()
		: AggregateFactory()
	{}
	~CatFactory() {}

	std::shared_ptr<Cat> createAggregate()
	{
		return std::make_shared<Cat>();
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
		std::cout << "CatService::execute " << getRepository()->size() << std::endl;
		std::vector<std::shared_ptr<Cat>> animals = getAggregates();
		for (const auto& animal : animals)
		{
			std::cout << animal->getInfo() << "\n";
		}
		return std::make_shared<DDD::ServiceExecutionResult>();
	}
protected:

};