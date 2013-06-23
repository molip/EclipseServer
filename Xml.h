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

	const std::string& GetName() const;
	
	Element AddElement(const std::string& name);

	Element GetFirstChild(const std::string& name) const;
	Element GetNextSibling(const std::string& name) const;

	void SetAttribute(const std::string& name, const std::string& val);
	void SetAttribute(const std::string& name, int val);

	bool GetAttribute(const std::string& name, std::string& val) const;
	bool GetAttribute(const std::string& name, int& val) const;
	bool GetAttribute(const std::string& name, bool& val) const;

	const std::string& GetAttributeStr(const std::string& name) const;
	int GetAttributeInt(const std::string& name) const;
	bool GetAttributeBool(const std::string& name) const;

	bool IsNull() const { return m_pElem == nullptr; }
	bool operator == (const Element& rhs) const { return m_pElem == rhs.m_pElem; }
	bool operator != (const Element& rhs) const { return !operator ==(rhs); }

private:
	Element(TiXmlElement* pElem);

	TiXmlElement* m_pElem; 
};

class Document
{
public:
	Document();
	~Document();

	bool SaveToFile(const std::string& path) const;
	bool LoadFromFile(const std::string& path);

	std::string SaveToString() const;
	bool LoadFromString(const std::string str) const;

	Element AddElement(const std::string& name);

	Element GetRoot();

private:
	std::unique_ptr<TiXmlDocument> m_pDoc; 
};

class ElementIter
{
public:
    ElementIter(Element elem, const std::string& name) : m_elem(elem), m_name(name) {}
	bool operator !=(const ElementIter& rhs) const { return m_elem != rhs.m_elem; }
	Element operator* () const { return m_elem; }
    void operator++ () { m_elem = m_elem.GetNextSibling(m_name); }
private:
    Element m_elem;
	std::string m_name;
};
 
class ElementRange
{
public:
	ElementRange(Element parent, const std::string& name) : m_begin(parent.GetFirstChild(name), name), m_end(Element(), name) {}
	ElementIter begin() const { return ElementIter(m_begin); }
	ElementIter end() const { return ElementIter(m_end); }
private:
	ElementIter m_begin, m_end;
};

}
