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
#include <random>

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

extern void Verify(const std::string& msg, bool b = false);
extern void VerifyInput(const std::string& msg, bool b = false);
extern void VerifyModel(const std::string& msg, bool b = false);
extern void VerifySerial(const std::string& msg, bool b = false);

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

class SerialException : public Exception
{
public:
	SerialException(const std::string& msg) : Exception(msg) {}
	virtual std::string GetType() const override { return "Serial"; }
};

template <typename T2, typename T1>
T2& VerifyCastInput(const T1& obj)
{
	T2* pObj = dynamic_cast<T2*>(&obj);
	Verify(std::string("VerifyCastInput: Expected ") + typeid(T2).name() + ", got " + typeid(T1).name(), !!pObj);
	return *pObj;
}

template<typename T>
bool InRange(const T& cntr, int n)
{
	return n >= 0 && n < (int)cntr.size();
}


enum class SquareType;
enum class Resource;

extern Resource SquareTypeToResource(SquareType t);
extern SquareType ResourceToSquareType(Resource t);

extern std::default_random_engine& GetRandom();

#define DEFINE_UNIQUE_PTR(T) typedef std::unique_ptr<T> T##Ptr;