#pragma once

#include "DDD.h"
#include "IInfo.h"

class Body : public DDD::Entity, IInfo
{
public:
	Body(DDD::ID id)
		: Entity(id)
	{}
	std::string getInfo() const override
	{
		return "Body";
	}
private:
};