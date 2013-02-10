#pragma once

#include <string>
#include "tinyxml.h"

typedef TiXmlDocument XmlDoc;

namespace Input 
{

class Base
{
public:
	bool Parse(const std::string& xml);

private:
	virtual bool Process(const XmlDoc& doc) = 0;
};

}