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
	GenericException(const std::string& msg) : Exception(msg) {}
	virtual std::string GetType() const override { return "Generic"; }
};

class ModelException : public Exception
{
public:
	ModelException(const std::string& msg) : Exception(msg) {}
	virtual std::string GetType() const override { return "Model"; }
};
