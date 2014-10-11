#include "Serial.h"

#include <sstream>
#include <memory>

namespace Serial
{

SaveNode::SaveNode(Xml::Element elem) : m_elem(elem) 
{
	VERIFY_SERIAL(!m_elem.IsNull());
}

LoadNode::LoadNode(Xml::Element elem) : m_elem(elem) 
{
	VERIFY_SERIAL(!m_elem.IsNull());
}

template <> std::string FromString<std::string>(const std::string& s)
{
	return s;
}

} // namespace

