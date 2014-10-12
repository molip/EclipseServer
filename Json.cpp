#include "Json.h"
#include "App.h"
#include "cJSON.h"

namespace Json
{

Document::Document()
{
	m_pElem = cJSON_CreateObject();
}

Document::~Document() 
{
	cJSON_Delete(m_pElem);
}

std::string Document::SaveToString() const
{
	char* cstr = cJSON_Print(m_pElem);
	std::string str(cstr);
	free(cstr);
	return str;
}

bool Document::LoadFromString(const std::string str)
{
	cJSON_Delete(m_pElem);
	m_pElem = cJSON_Parse(str.c_str());
	return m_pElem != nullptr;
}

//bool Document::LoadFromFile(const std::string& path)
//{
//	return m_pDoc->LoadFile(path);
//}
//
//bool Document::SaveToFile(const std::string& path) const
//{
//	return m_pDoc->SaveFile(path);
//}

//-----------------------------------------------------------------------------

Base::Base(cJSON* pElem) : m_pElem(pElem)
{
}

std::string Base::GetName() const
{
	VERIFY_SERIAL(!!m_pElem);
	return m_pElem->string;
}

cJSON* Base::AddObject(cJSON* pElem, const std::string& name)
{
	cJSON_AddItemToObject(m_pElem, name.c_str(), pElem);
	return pElem;
}

cJSON* Base::GetChild(const std::string& name) const
{
	VERIFY_SERIAL(!!m_pElem);
	for (cJSON* p = m_pElem->child; p; p = p->next)
		if (name.empty() || p->string == name)
			return p;

	return nullptr;
}

cJSON* Base::GetNextSibling() const
{
	VERIFY_SERIAL(!!m_pElem);
	return m_pElem->next;
}

//-----------------------------------------------------------------------------

Element::Element(cJSON* pElem) : Base(pElem)
{
	VERIFY_SERIAL(!pElem || pElem->type == cJSON_Object);
}

Element Element::AddElement(const std::string& name)
{
	VERIFY_SERIAL(!!m_pElem);
	VERIFY_SERIAL(!HasChild(name));

	return Element(AddObject(cJSON_CreateObject(), name));
}

Array Element::AddArray(const std::string& name)
{
	VERIFY_SERIAL(!!m_pElem);
	VERIFY_SERIAL(!HasChild(name));
	return Array(AddObject(cJSON_CreateArray(), name));
}

void Element::SetAttribute(const std::string& name, const std::string& val)
{
	SetAttribute(name, val.c_str());
}

void Element::SetAttribute(const std::string& name, const char* val)
{
	VERIFY_SERIAL(!!m_pElem);
	VERIFY_SERIAL(!HasChild(name));
	cJSON_AddStringToObject(m_pElem, name.c_str(), val);
}

void Element::SetAttribute(const std::string& name, int val)
{
	VERIFY_SERIAL(!!m_pElem);
	VERIFY_SERIAL(!HasChild(name));
	cJSON_AddNumberToObject(m_pElem, name.c_str(), val);
}

void Element::SetAttribute(const std::string& name, bool val)
{
	VERIFY_SERIAL(!!m_pElem);
	VERIFY_SERIAL(!HasChild(name));
	cJSON_AddBoolToObject(m_pElem, name.c_str(), val);
}

bool Element::GetAttribute(const std::string& name, std::string& val) const
{
	VERIFY_SERIAL(!!m_pElem);
	if (cJSON* pChild = cJSON_GetObjectItem(m_pElem, name.c_str()))
		if (pChild->type == cJSON_String && pChild->valuestring)
		{
			val = pChild->valuestring;
			return true;
		}
	return false;
}

bool Element::GetAttribute(const std::string& name, int& val) const
{
	VERIFY_SERIAL(!!m_pElem);
	if (cJSON* pChild = cJSON_GetObjectItem(m_pElem, name.c_str()))
		if (pChild->type == cJSON_Number)
		{
			val = pChild->valueint;
			return true;
		}
	return false;
}

bool Element::GetAttribute(const std::string& name, bool& val) const
{
	VERIFY_SERIAL(!!m_pElem);
	if (cJSON* pChild = cJSON_GetObjectItem(m_pElem, name.c_str()))
		if (pChild->type == cJSON_True || pChild->type == cJSON_False)
		{
			val = pChild->type == cJSON_True;
			return true;
		}
	return false;
}

std::string Element::GetAttributeStr(const std::string& name) const
{
	std::string str;
	bool bOK = GetAttribute(name, str);
	VERIFY_SERIAL(bOK);
	return str;
}

int Element::GetAttributeInt(const std::string& name) const
{
	int n;
	bool bOK = GetAttribute(name, n);
	VERIFY_SERIAL(bOK);
	return n;
}

bool Element::GetAttributeBool(const std::string& name) const
{
	bool b;
	bool bOK = GetAttribute(name, b);
	VERIFY_SERIAL(bOK);
	return b;
}

Element Element::GetChildElement(const std::string& name) const
{
	return Element(GetChild(name));
}

Array Element::GetChildArray(const std::string& name) const
{
	return Array(GetChild(name));
}

bool Element::HasChild(const std::string& name) const
{
	return m_pElem && !!cJSON_GetObjectItem(m_pElem, name.c_str());
}

Element Element::GetNextSibling() const
{
	return Element(__super::GetNextSibling());
}

//-----------------------------------------------------------------------------

Array::Array(cJSON* pElem) : Base(pElem)
{
	VERIFY_SERIAL(!pElem || pElem->type == cJSON_Array);
}

Element Array::AppendElement()
{
	VERIFY_SERIAL(!!m_pElem);
	cJSON* pElem = cJSON_CreateObject();
	cJSON_AddItemToArray(m_pElem, pElem);
	return Element(pElem);
}

Array Array::AppendArray()
{
	VERIFY_SERIAL(!!m_pElem);
	cJSON* pElem = cJSON_CreateArray();
	cJSON_AddItemToArray(m_pElem, pElem);
	return Array(pElem);
}

void Array::Append(const std::string& val)
{
	VERIFY_SERIAL(!!m_pElem);
	cJSON* pElem = cJSON_CreateString(val.c_str());
	cJSON_AddItemToArray(m_pElem, pElem);
}

void Array::Append(int val)
{
	VERIFY_SERIAL(!!m_pElem);
	cJSON* pElem = cJSON_CreateNumber(val);
	cJSON_AddItemToArray(m_pElem, pElem);
}

ElementIter Array::begin() const { return ElementIter(m_pElem ? m_pElem->child : nullptr); }
ElementIter Array::end() const { return ElementIter(); }

}
