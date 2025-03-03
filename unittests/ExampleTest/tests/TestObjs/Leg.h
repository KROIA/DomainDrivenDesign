#pragma once

#include "DDD.h"
#include "IInfo.h"

class Leg : public DDD::Entity, IInfo
{
public:
	Leg(DDD::ID id)
		: Entity(id)
	{}
	std::string getInfo() const override
	{
		return "Leg";
	}
private:
};