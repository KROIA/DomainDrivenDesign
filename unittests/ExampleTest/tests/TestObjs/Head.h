#pragma once

#include "DDD.h"
#include "IInfo.h"

class Head : public DDD::Entity, IInfo
{
public:
	Head(DDD::ID id)
		: Entity(id)
	{}
	std::string getInfo() const override
	{
		return "Head";
	}
private:
};