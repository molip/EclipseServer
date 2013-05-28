#include "Xml.h"
#include "App.h"
#include "tinyxml.h"

namespace Xml
{

Document::Document()
{
	m_pDoc.reset(new TiXmlDocument);
}

Document::~Document() {}

std::string Document::SaveToString() const
{
	TiXmlPrinter printer;
	m_pDoc->Accept(&printer);
	return printer.Str();
}

bool Document::LoadFromString(const std::string str) const
{
	m_pDoc->Parse(str.c_str());
	return !m_pDoc->Error();
}

Element Document::AddElement(const std::string& name)
{
	TiXmlElement* pElem = new TiXmlElement(name);
	m_pDoc->LinkEndChild(pElem);
	return Element(pElem);
}

Element Document::GetRoot()
{
	auto pRoot = m_pDoc->RootElement();
	AssertThrow("Document::GetRoot", !!pRoot);
	return Element(pRoot);
}

//-----------------------------------------------------------------------------

Element::Element(TiXmlElement* pElem) : m_pElem(pElem)
{
}

Element Element::AddElement(const std::string& name)
{
	AssertThrow("Element::AddElement", !!m_pElem);
	TiXmlElement* pElem = new TiXmlElement(name);
	m_pElem->LinkEndChild(pElem);
	return Element(pElem);
}

void Element::SetAttribute(const std::string& name, const std::string& val)
{
	AssertThrow("Element::SetAttribute", !!m_pElem);
	m_pElem->SetAttribute(name, val);
}

void Element::SetAttribute(const std::string& name, int val)
{
	AssertThrow("Element::SetAttribute", !!m_pElem);
	m_pElem->SetAttribute(name, val);
}

bool Element::GetAttribute(const std::string& name, std::string& val) const
{
	AssertThrow("Element::GetAttribute", !!m_pElem);
	if (auto pStr = m_pElem->Attribute(name))
	{
		val = *pStr;
		return true;
	}
	return false;
}

bool Element::GetAttribute(const std::string& name, int& val) const
{
	AssertThrow("Element::GetAttribute", !!m_pElem);
	return !!m_pElem->Attribute(name, &val);
}

bool Element::GetAttribute(const std::string& name, bool& val) const
{
	AssertThrow("Element::GetAttribute", !!m_pElem);
	return m_pElem->QueryBoolAttribute(name.c_str(), &val) == TIXML_SUCCESS;
}

const std::string& Element::GetAttributeStr(const std::string& name) const
{
	AssertThrow("Element::GetAttributeStr: null element", !!m_pElem);
	auto pStr = m_pElem->Attribute(name);
	AssertThrow("Element::GetAttributeStr: attribute not found", !!pStr);
	return *pStr;
}

int Element::GetAttributeInt(const std::string& name) const
{
	AssertThrow("Element::GetAttributeInt: null element", !!m_pElem);
	int val = 0;
	auto pStr = m_pElem->Attribute(name, &val);
	AssertThrow("Element::GetAttributeInt: attribute not found", !!pStr);
	return val;
}

bool Element::GetAttributeBool(const std::string& name) const
{
	AssertThrow("Element::GetAttributeBool: null element", !!m_pElem);
	bool val = false;
	bool bOK = m_pElem->QueryBoolAttribute(name.c_str(), &val) == TIXML_SUCCESS;
	AssertThrow("Element::GetAttributeBool: attribute not found", bOK);
	return val;
}

Element Element::FindFirstChild(const std::string& name) const
{
	AssertThrow("Element::FindFirstChild: null element", !!m_pElem);
	auto pEl = m_pElem->FirstChildElement(name);
	AssertThrow("Element::FindFirstChild: child not found", !!pEl);
	return Element(pEl);
}

}
