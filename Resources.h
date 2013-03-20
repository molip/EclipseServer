#pragma once

#include <vector>

enum class Resource { Money, Science, Materials, _Count };

class Resources : private std::vector<int>
{
public:
	const int& operator[] (Resource r) const { return __super::at((int)r); }
	int& operator[] (Resource r) { return __super::at((int)r); }

protected:
	Resources() {}
	Resources(int money, int sci, int mat) 
	{
		push_back(money);
		push_back(sci);
		push_back(mat);
	}

private:
};

class Population : public Resources
{
public:
	Population() {}
	Population(int money, int sci, int mat) : Resources(money, sci, mat) {}
};

class Storage : public Resources
{
public:
	Storage() {}
	Storage(int money, int sci, int mat) : Resources(money, sci, mat) {}
};
