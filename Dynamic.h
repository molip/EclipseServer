#pragma once

#include <map>
#include <functional>
#include <string>

namespace Serial { class SaveNode; class LoadNode; }

class Dynamic
{
public:
	virtual ~Dynamic();

	virtual void Save(Serial::SaveNode& node) const {}
	virtual void Load(const Serial::LoadNode& node) {}

	static void RegisterObject(const std::string& id, std::function<Dynamic*()> fn)
	{
		s_map.insert(std::make_pair(id, fn));
	}

	template <typename T> static T* CreateObject(const std::string& id)
	{
		auto it = s_map.find(id);
		T* pObj = dynamic_cast<T*>(it->second());
		if (pObj == nullptr)
			throw 1;
		return pObj;
	}

private:
	static std::map<std::string, std::function<Dynamic*()>> s_map;
};

#define REGISTER_DYNAMIC(T) \
	namespace { \
		struct _Register_##T {\
			_Register_##T() { \
				Dynamic::RegisterObject(typeid(T).name(), [] { return new T; } ); \
			} \
		} _register_##T; \
	}
