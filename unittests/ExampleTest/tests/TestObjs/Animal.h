#pragma once

#include "DDD.h"
#include "IInfo.h"
#include "Leg.h"
#include "Body.h"
#include "Head.h"
 

class Animal : public DDD::Aggregate, public IInfo
{
public:
	Animal()
		: Aggregate()
	{

	}

	std::string getInfo() const override
	{
		std::string msg;
		std::vector<std::shared_ptr<Entity>> entities = getEntities();
		msg += "ID: " + getIDString() + "\n";
		for (const std::shared_ptr<Entity>& e : entities)
		{
			const IInfo* ie = dynamic_cast<const IInfo*>(e.get());
			if (ie)
			{
				msg += "Entity[" + std::to_string(e->getID()) + "]: " + ie->getInfo() + "\n";
				continue;
			}

			const Leg* leg = dynamic_cast<const Leg*>(e.get());
			if (leg)
			{
				msg += "Entity[" + std::to_string(e->getID()) + "]: " + leg->getInfo() + "\n";
				continue;
			}
			const Body* body = dynamic_cast<const Body*>(e.get());
			if (body)
			{
				msg += "Entity[" + std::to_string(e->getID()) + "]: " + body->getInfo() + "\n";
				continue;
			}
			const Head* head = dynamic_cast<const Head*>(e.get());
			if (head)
			{
				msg += "Entity[" + std::to_string(e->getID()) + "]: " + head->getInfo() + "\n";
				continue;
			}
		}
		return msg;
	}

protected:
	std::vector<std::shared_ptr<Leg>> legs;
	std::shared_ptr<Body> body;
	std::shared_ptr<Head> head;
private:
	


};

class AnimalFactory : public DDD::AggregateFactory<Animal>
{
public:
	AnimalFactory(DDD::Repository<Animal>* repo)
		: AggregateFactory(repo)
	{}
	~AnimalFactory() {}

	std::shared_ptr<Animal> createAggregate()
	{
		return registerInstance(std::make_shared<Animal>());
	}
protected:

private:

};

class AnimalService : public DDD::AggregateService<Animal>
{
public:
	AnimalService(DDD::Repository<Animal>* repository)
		: AggregateService(repository)
	{}
	~AnimalService() {}

	std::shared_ptr<DDD::ServiceExecutionResult> execute() override
	{
		std::cout << "AnimalService::execute " << getRepository()->size() << std::endl;
		std::vector<std::shared_ptr<Animal>> animals = getAggregates();
		for (const auto &animal : animals)
		{
			std::cout << animal->getInfo() << "\n";
		}
		return std::make_shared<DDD::ServiceExecutionResult>();
	}
protected:

};


