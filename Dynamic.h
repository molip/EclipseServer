#pragma once

#include <map>
#include <functional>
#include <string>

namespace Serial { class SaveNode; class LoadNode; }

class Dynamic
{
	typedef std::function<Dynamic*()> Func;
	typedef std::map<std::string, Func> Map;
public:
	virtual ~Dynamic();

	virtual void Save(Serial::SaveNode& node) const {}
	virtual void Load(const Serial::LoadNode& node) {}

	static void RegisterObject(const std::string& id, Func fn);

	template <typename T> static T* CreateObject(const std::string& id)
	{
		auto it = GetMap().find(id);
		T* pObj = dynamic_cast<T*>(it->second());
		if (pObj == nullptr)
			throw 1;
		return pObj;
	}

private:

	static Map& GetMap();
};

#define REGISTER_DYNAMIC(T) \
	namespace { \
		struct _Register_##T {\
			_Register_##T() { \
				Dynamic::RegisterObject(typeid(T).name(), [] { return new T; } ); \
			} \
		} _register_##T; \
	}
