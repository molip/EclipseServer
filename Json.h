#pragma once

#include <memory>
#include <string>
#include <vector>

struct cJSON;

namespace Json
{
class ElementIter;
class Array;

class Base
{
public:
	Base() : m_pElem(nullptr) {}

	std::string GetName() const;

	bool IsNull() const { return m_pElem == nullptr; }
	bool operator == (const Base& rhs) const { return m_pElem == rhs.m_pElem; }
	bool operator != (const Base& rhs) const { return !operator ==(rhs); }

protected:
	Base(cJSON* pElem);

	cJSON* AddObject(cJSON* pElem, const std::string& name);
	cJSON* GetChild(const std::string& name = "") const;
	cJSON* GetNextSibling() const;

	cJSON* m_pElem;
};

class Element : public Base
{
	friend class Array;
public:
	Element() : Base(nullptr) {}

	Element AddElement(const std::string& name);
	Array AddArray(const std::string& name);

	Element GetChildElement(const std::string& name = "") const;
	Array GetChildArray(const std::string& name = "") const;

	void SetAttribute(const std::string& name, const std::string& val);
	void SetAttribute(const std::string& name, const char* val);
	void SetAttribute(const std::string& name, int val);
	void SetAttribute(const std::string& name, bool val);

	bool GetAttribute(const std::string& name, std::string& val) const;
	bool GetAttribute(const std::string& name, int& val) const;
	bool GetAttribute(const std::string& name, bool& val) const;

	std::string GetAttributeStr(const std::string& name) const;
	int GetAttributeInt(const std::string& name) const;
	bool GetAttributeBool(const std::string& name) const;

	bool HasChild(const std::string& name) const;
	Element GetNextSibling() const;

protected:
	Element(cJSON* pElem);
};

class Array : public Base
{
	friend class Element;
public:
	Array() : Base(nullptr) {}

	Element AppendElement();
	Array AppendArray();
	void Append(const std::string& val);
	void Append(int val);

	ElementIter begin() const;
	ElementIter end() const;

	std::vector<int> GetAsInts() const;
	std::vector<bool> GetAsBools() const;

protected:
	Array(cJSON* pElem);
};

class Document : public Element
{
public:
	Document();
	~Document();

	//bool SaveToFile(const std::string& path) const;
	//bool LoadFromFile(const std::string& path);

	std::string SaveToString() const;
	bool LoadFromString(const std::string str);
};

class ElementIter
{
public:
	ElementIter(Element elem = Element()) : m_elem(elem) {}
	bool operator !=(const ElementIter& rhs) const { return m_elem != rhs.m_elem; }
	Element operator* () const { return m_elem; }
    void operator++ () { m_elem = m_elem.GetNextSibling(); }
private:
	Element m_elem;
};
 
//class ElementRange
//{
//public:
//	ElementRange(Element parent) : m_begin(parent.GetChild()), m_end(Element()) {}
//	ElementIter begin() const { return ElementIter(m_begin); }
//	ElementIter end() const { return ElementIter(m_end); }
//private:
//	ElementIter m_begin, m_end;
//};

}
