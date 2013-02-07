#pragma once

#include <string>
#include <memory>

class Player
{
public:
	Player(const std::string& name);
	~Player();

	const std::string& GetName() const { return m_name; }

private:
	std::string m_name;
};

typedef std::unique_ptr<Player> PlayerPtr;

