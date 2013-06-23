#include "Dynamic.h"

std::map<std::string, std::function<Dynamic*()>> Dynamic::s_map;

Dynamic::~Dynamic() {}
