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

Element::Element(cJSON* pElem) : m_pElem(pElem)
{
}

std::string Element::GetName() const
{
	VERIFY_SERIAL(!!m_pElem);
	return m_pElem->string;
}

Element Element::AddElement(const std::string& name)
{
	VERIFY_SERIAL(!!m_pElem);
	VERIFY_SERIAL(!HasChild(name));

	return AddObject(cJSON_CreateObject(), name);
}

Element Element::AddArray(const std::string& name)
{
	VERIFY_SERIAL(!!m_pElem);
	VERIFY_SERIAL(!HasChild(name));
	return AddObject(cJSON_CreateArray(), name);
}

Element Element::AddObject(cJSON* pElem, const std::string& name)
{
	cJSON_AddItemToObject(m_pElem, name.c_str(), pElem);
	return Element(pElem);
}

Element Element::AppendElement()
{
	VERIFY_SERIAL(!!m_pElem);
	VERIFY_SERIAL(m_pElem->type == cJSON_Array);
	cJSON* pElem = cJSON_CreateObject();
	cJSON_AddItemToArray(m_pElem, pElem);
	return Element(pElem);
}

Element Element::AppendArray()
{
	VERIFY_SERIAL(!!m_pElem);
	VERIFY_SERIAL(m_pElem->type == cJSON_Array);
	cJSON* pElem = cJSON_CreateArray();
	cJSON_AddItemToArray(m_pElem, pElem);
	return Element(pElem);
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

void Element::Append(const std::string& val)
{
	VERIFY_SERIAL(!!m_pElem);
	VERIFY_SERIAL(m_pElem->type == cJSON_Array);
	cJSON* pElem = cJSON_CreateString(val.c_str());
	cJSON_AddItemToArray(m_pElem, pElem);
}

void Element::Append(int val)
{
	VERIFY_SERIAL(!!m_pElem);
	VERIFY_SERIAL(m_pElem->type == cJSON_Array);
	cJSON* pElem = cJSON_CreateNumber(val);
	cJSON_AddItemToArray(m_pElem, pElem);
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

Element Element::GetChild(const std::string& name) const
{
	VERIFY_SERIAL(!!m_pElem);
	for (cJSON* p = m_pElem->child; p; p = p->next)
		if (name.empty() || p->string == name)
			return Element(p);

	return Element();
}

Element Element::GetNextSibling() const
{
	VERIFY_SERIAL(!!m_pElem);
	return Element(m_pElem->next);
}

bool Element::HasChild(const std::string& name) const
{
	return m_pElem && !!cJSON_GetObjectItem(m_pElem, name.c_str());
}

ElementIter Element::begin() const { return ElementIter(m_pElem ? m_pElem->child : nullptr); }
ElementIter Element::end() const { return ElementIter(); }

}
