#pragma once

#include <string>

class IInfo
{
public:
	virtual std::string getInfo() const = 0;
};