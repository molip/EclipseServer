#pragma once

#include <cassert>

template <typename T> class Singleton
{
public:
	static T* Get() { return s_pInstance; }

protected:
	Singleton() { assert(!s_pInstance); s_pInstance = static_cast<T*>(this); }
	virtual ~Singleton() { s_pInstance = nullptr; }

private:
	static T* s_pInstance;
};

#define SINGLETON(t) t* Singleton<t>::s_pInstance;