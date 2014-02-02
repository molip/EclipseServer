#pragma once

#include <map>
#include <functional>
#include <string>

namespace Serial { class SaveNode; class LoadNode; }

class LoadException {};

extern bool _DebugBreak();

class Dynamic
{
	typedef std::function<Dynamic*()> Func;
	typedef std::map<std::string, Func> ClassMap;
public:
	virtual ~Dynamic();

	virtual void Save(Serial::SaveNode& node) const {}
	virtual void Load(const Serial::LoadNode& node) {}

	static void RegisterObject(const std::string& id, Func fn);

	template <typename T> static T* CreateObject(const std::string& id)
	{
		auto it = GetClassMap().find(id);
		if (it == GetClassMap().end())
		{
			_DebugBreak();
			throw LoadException();
		}
		T* pObj = dynamic_cast<T*>(it->second());
		if (pObj == nullptr)
		{
			_DebugBreak();
			throw LoadException();
		}
		return pObj;
	}

private:
	static ClassMap& GetClassMap();
};

#define REGISTER_DYNAMIC(T) \
	namespace { \
		struct _Register_##T {\
			_Register_##T() { \
				Dynamic::RegisterObject(typeid(T).name(), [] { return new T; } ); \
			} \
		} _register_##T; \
	}
