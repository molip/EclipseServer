#include "Input.h"
#include "App.h"

namespace Input 
{

bool Base::Parse(const std::string& xml)
{
	XmlDoc doc;
	doc.Parse(xml.c_str());
	if (!doc.Error())
		return Process(doc);
	
	std::cerr << "[InMessage::Parse] XML parsing failed." << std::endl;
	return false;
}

}