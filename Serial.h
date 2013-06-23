#pragma once

#include "App.h"
#include "Xml.h"
#include "Dynamic.h"
#include "EnumTraits.h"

#include <string>

class Dynamic;

namespace Serial
{

///////////////////////////////////////////////////////////////////////////////
// Saving

template <typename T> std::string ToString(const T& t)
{
	std::ostringstream ss;
	ss << t;
	return ss.str();
}

class SaveNode
{
public:
	explicit SaveNode(Xml::Element elem);
	
	template <typename T> void SaveType(const std::string& name, const T& val);
	template <typename T> void SaveEnum(const std::string& name, const T& val);
	template <typename T> void SaveClass(const std::string& name, const T& obj);
	template <typename T> void SaveClassPtr(const std::string& name, const T& pObj);
	template <typename T> void SaveObject(const std::string& name, const T& pObj);
	
	template <typename T, typename S> void SaveCntr(const std::string& name, const T& cntr, S saver);
	template <typename T, typename S> void SaveArray(const std::string& name, const T& cntr, S saver);
	template <typename T, typename SK, typename SV> void SaveMap(const std::string& name, const T& map, SK keySaver, SV valSaver);

private:
	Xml::Element m_elem;
};

struct TypeSaver
{
	template <typename T> void operator ()(Xml::Element& e, const T& val) { SaveNode(e).SaveType("_value", val); }
};

struct EnumSaver
{
	template <typename T> void operator ()(Xml::Element& e, const T& val) { SaveNode(e).SaveEnum("_enum", val); }
};

struct ClassSaver
{
	template <typename T> void operator ()(Xml::Element& e, const T& obj) { obj.Save(SaveNode(e)); }
};

struct ClassPtrSaver
{
	template <typename T> void operator ()(Xml::Element& e, const T& pObj) 
	{
		if (pObj)
			pObj->Save(SaveNode(e)); 
		else
			e.SetAttribute("_null", true);
	}
};

struct ObjectSaver
{
	template <typename T> void operator ()(Xml::Element& e, const T& pObj) 
	{
		e.SetAttribute("_class", typeid(*pObj).name());
		pObj->Save(SaveNode(e));
	}		
};

template <typename T> void SaveNode::SaveType(const std::string& name, const T& val)
{
	m_elem.SetAttribute(name, ToString(val));
}

template <typename T> void SaveNode::SaveEnum(const std::string& name, const T& val)
{
	m_elem.SetAttribute(name, EnumTraits<T>::ToString(val));
}

template <typename T> void SaveNode::SaveClass(const std::string& name, const T& obj)
{
	Xml::Element elem = m_elem.AddElement(name);
	ClassSaver()(elem, obj);
}

template <typename T> void SaveNode::SaveClassPtr(const std::string& name, const T& pObj)
{
	Xml::Element elem = m_elem.AddElement(name);
	ClassPtrSaver()(elem, pObj);
}

template <typename T> void SaveNode::SaveObject(const std::string& name, const T& pObj)
{
	Xml::Element elem = m_elem.AddElement(name);
	ObjectSaver()(elem, pObj);
}

template <typename T, typename S> void SaveNode::SaveCntr(const std::string& name, const T& cntr, S saver)
{
	Xml::Element elem = m_elem.AddElement(name);
	// Can't use range-based for loop here. 
	// auto& doesn't work with vector<bool>, auto doesn't work with vector<unique_ptr>.
	for (auto v = cntr.begin(); v != cntr.end(); ++v) 
	{
		Xml::Element elem2 = elem.AddElement("_item");
		saver(elem2, *v);
	}
}

template <typename T, typename S> void SaveNode::SaveArray(const std::string& name, const T& cntr, S saver)
{
	Xml::Element elem = m_elem.AddElement(name);
	for (auto& v : cntr)
	{
		Xml::Element elem2 = elem.AddElement("_item");
		saver(elem2, v);
	}
}

template <typename T, typename SK, typename SV> void SaveNode::SaveMap(const std::string& name, const T& map, SK keySaver, SV valSaver)
{
	Xml::Element elem = m_elem.AddElement(name);
	for (auto& kv : map)
	{
		Xml::Element item = elem.AddElement("_item");
		keySaver(item.AddElement("_k"), kv.first);
		valSaver(item.AddElement("_v"), kv.second);
	}
}

template <typename T> bool SaveClass(const std::string& path, const T& obj)
{
	Xml::Document doc;
	ClassSaver()(doc.AddElement("class"), obj);

	return doc.SaveToFile(path);
}

template <typename T> bool SaveObject(const std::string& path, const T& pObj)
{
	Xml::Document doc;
	ObjectSaver()(doc.AddElement("object"), pObj);

	return doc.SaveToFile(path);
}


///////////////////////////////////////////////////////////////////////////////
// Loading

template <typename T> T FromString(const std::string& s)
{
	std::istringstream ss(s);
	T t;
	ss >> t;
	return t;
}

template <> std::string FromString<std::string>(const std::string& s);

class LoadNode
{
public:
	explicit LoadNode(Xml::Element elem);
	
	template <typename T> bool LoadType(const std::string& name, T& val) const;
	template <typename T> bool LoadEnum(const std::string& name, T& val) const;
	template <typename T> bool LoadClass(const std::string& name, T& obj) const;
	template <typename T> bool LoadClassPtr(const std::string& name, T& pObj) const;
	template <typename T> bool LoadClassPtr(const std::string& name, std::unique_ptr<T>& pObj) const;
	template <typename T> bool LoadObject(const std::string& name, T& pObj) const;
	template <typename T> bool LoadObject(const std::string& name, std::unique_ptr<T>& pObj) const;

	template <typename T, typename L> bool LoadCntr(const std::string& name, T& cntr, L loader) const;
	template <typename T, typename L> bool LoadArray(const std::string& name, T& cntr, L loader) const;
	template <typename T, typename LK, typename LV> bool LoadMap(const std::string& name, T& map, LK keyLoader, LV valLoader) const;

private:
	const Xml::Element m_elem;
};

struct TypeLoader
{
	template <typename T> void operator ()(const Xml::Element& e, T& val) { LoadNode(e).LoadType("_value", val); }
};

struct EnumLoader
{
	template <typename T> void operator ()(const Xml::Element& e, T& val) { LoadNode(e).LoadEnum("_enum", val); }
};

struct ClassLoader
{
	template <typename T> void operator ()(const Xml::Element& e, T& obj) { obj.Load(LoadNode(e)); }
};

struct ClassPtrLoader
{
	template <typename T> void operator ()(const Xml::Element& e, T*& pObj) 
	{
		delete pObj;
		pObj = nullptr;

		bool bNull = false;
		e.GetAttribute("_null", bNull);
		if (!bNull)
		{
			pObj = new T;
			pObj->Load(LoadNode(e)); 
		}
	}
	template <typename T> void operator ()(const Xml::Element& e, std::unique_ptr<T>& pObj) 
	{ 
		T* p = nullptr;
		operator()(e, p);
		pObj.reset(p);
	}
};

struct ObjectLoader
{
	template <typename T> bool operator ()(const Xml::Element& e, T*& pObj) 
	{ 
		std::string id;
		if (!e.GetAttribute("_class", id))
			AssertThrow("ObjectLoader", false);

		delete pObj;
		pObj = nullptr;

		bool bNull = false;
		e.GetAttribute("_null", bNull);
		if (bNull)
			return true;

		if (pObj = Dynamic::CreateObject<T>(id))
		{
			pObj->Load(LoadNode(e));
			return true;
		}
		return false;
	}
	template <typename T> bool operator ()(const Xml::Element& e, std::unique_ptr<T>& pObj) 
	{ 
		T* p = nullptr;
		bool bOK = operator()(e, p);
		pObj.reset(p);
		return bOK;
	}
};

template <typename T> bool LoadNode::LoadType(const std::string& name, T& val) const
{
	std::string attr;
	if (!m_elem.GetAttribute(name, attr))
		return false;
	val = FromString<T>(attr);
	return true;
}

template <typename T> bool LoadNode::LoadEnum(const std::string& name, T& val) const
{
	std::string s;
	if (LoadType(name, s))
	{
		val = EnumTraits<T>::FromString(s);
		return true;
	}
	return false;
}

template <typename T> bool LoadNode::LoadClass(const std::string& name, T& obj) const
{
	Xml::Element e = m_elem.GetFirstChild(name);
	if (e.IsNull())
		return false;

	ClassLoader()(e, obj);
	return true;
}

template <typename T> bool LoadNode::LoadClassPtr(const std::string& name, T& pObj) const
{
	Xml::Element e = m_elem.GetFirstChild(name);
	if (e.IsNull())
		return false;

	ClassPtrLoader()(e, *pObj);
	return true;
}

template <typename T> bool LoadNode::LoadClassPtr(const std::string& name, std::unique_ptr<T>& pObj) const
{
	T* p = nullptr;
	bool bOK = LoadClassPtr(name, p);
	pObj.reset(p);
	return bOK;
}

template <typename T> bool LoadNode::LoadObject(const std::string& name, T& pObj) const
{
	pObj = nullptr;

	Xml::Element e = m_elem.GetFirstChild(name);
	if (e.IsNull())
		return false;

	return ObjectLoader()(e, pObj);
}

template <typename T, typename L> bool LoadNode::LoadCntr(const std::string& name, T& cntr, L loader) const
{
	ASSERT(cntr.empty());
	
	Xml::Element e = m_elem.GetFirstChild(name);
	if (e.IsNull())
		return false;

	for (auto i : Xml::ElementRange(e, "_item"))
	{
		T::value_type v = T::value_type();
		loader(i, v);
		cntr.insert(cntr.end(), std::move(v));
	}
	return true;
}

template <typename T, typename L> bool LoadNode::LoadArray(const std::string& name, T& cntr, L loader) const
{
	Xml::Element e = m_elem.GetFirstChild(name);
	if (e.IsNull())
		return false;

	int i = 0;
	for (auto item : Xml::ElementRange(e, "_item"))
	{
		loader(item, cntr[i++]);
	}
	return true;
}

template <typename T, typename LK, typename LV> bool LoadNode::LoadMap(const std::string& name, T& map, LK keyLoader, LV valLoader) const
{
	ASSERT(map.empty());
	
	Xml::Element e = m_elem.GetFirstChild(name);
	if (e.IsNull())
		return false;

	for (auto item : Xml::ElementRange(e, "_item"))
	{
		T::key_type key = T::key_type();
		T::mapped_type val = T::mapped_type();

		Xml::Element k = item.GetFirstChild("_k");
		Xml::Element v = item.GetFirstChild("_v");
		AssertThrow("LoadNode::LoadMap", !k.IsNull() && !v.IsNull());

		keyLoader(k, key);
		valLoader(v, val);
		map.insert(std::make_pair(std::move(key), std::move(val)));
	}
	return true;
}

template <typename T> bool LoadClass(const std::string& path, T& obj)
{
	Xml::Document doc;
	if (doc.LoadFromFile(path))
	{
		Xml::Element root = doc.GetRoot();
		if (!root.IsNull() && root.GetName() == "class")
		{
			ClassLoader()(doc.GetRoot(), obj);
			return true;
		}
	}
	return false;
}

template <typename T> bool LoadObject(const std::string& path, T& pObj)
{
	Xml::Document doc;
	if (doc.LoadFromFile(path))
	{
		Xml::Element root = doc.GetRoot();
		if (!root.IsNull() && root.GetName() == "object")
			return ObjectSaver()(doc.GetRoot()pObj);
	}
	return false;
}
}
