#pragma once

#include <memory>
#include <string>

class TiXmlDocument;
class TiXmlElement;

namespace Xml
{

class Element
{
	friend class Document;
public:
	Element() : m_pElem(nullptr) {}

	Element AddElement(const std::string& name);

	Element FindFirstChild(const std::string& name) const;

	void SetAttribute(const std::string& name, const std::string& val);
	void SetAttribute(const std::string& name, int val);

	bool GetAttribute(const std::string& name, std::string& val) const;
	bool GetAttribute(const std::string& name, int& val) const;
	bool GetAttribute(const std::string& name, bool& val) const;

	const std::string& GetAttributeStr(const std::string& name) const;
	int GetAttributeInt(const std::string& name) const;
	bool GetAttributeBool(const std::string& name) const;

private:
	Element(TiXmlElement* pElem);

	TiXmlElement* m_pElem; 
};

class Document
{
public:
	Document();
	~Document();

	std::string SaveToString() const;
	bool LoadFromString(const std::string str) const;

	Element AddElement(const std::string& name);

	Element GetRoot();

private:
	std::unique_ptr<TiXmlDocument> m_pDoc; 
};

}
