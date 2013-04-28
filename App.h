#pragma once

#include "EnumRange.h"

#include <map>
#include <vector>
#include <set>
#include <mutex>
#include <string>
#include <sstream>
#include <iostream>
#include <cassert>
#include <memory>
#include <algorithm>

//#define LOCK(m) ; //std::lock_guard<std::mutex> lock(m);
#define LOCK(m) std::lock_guard<std::mutex> lock(m);

extern bool _DebugBreak();

#ifdef _DEBUG
	#define ASSERT(f) (bool) ((f) || _DebugBreak())
#else
	#define ASSERT(f) (f)
#endif

extern std::string FormatInt(int n);

extern int Mod(int a, int b);

extern void AssertThrow(bool b = false);
extern void AssertThrow(const std::string& msg, bool b = false);
extern void AssertThrowXML(const std::string& msg, bool b = false);
extern void AssertThrowModel(const std::string& msg, bool b = false);

class Exception : public std::runtime_error
{
public:
	Exception(const std::string& msg) : std::runtime_error(msg) {}
	virtual std::string GetType() const = 0;
};

class XMLException : public Exception
{
public:
	XMLException(const std::string& msg) : Exception(msg) {}
	virtual std::string GetType() const override { return "XML"; }
};

class GenericException : public Exception
{
public:
	GenericException(const std::string& msg = "") : Exception(msg) {}
	virtual std::string GetType() const override { return "Generic"; }
};

class ModelException : public Exception
{
public:
	ModelException(const std::string& msg) : Exception(msg) {}
	virtual std::string GetType() const override { return "Model"; }
};

template <typename T2, typename T1>
T2& CastThrow(const T1& obj)
{
	T2* pObj = dynamic_cast<T2*>(&obj);
	AssertThrow(std::string("CastThrow: Expected ") + typeid(T2).name() + ", got " + typeid(T1).name(), !!pObj);
	return *pObj;
}

template<typename T>
bool InRange(const T& cntr, int n)
{
	return n >= 0 && n < (int)cntr.size();
}