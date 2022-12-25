#pragma once

#include <string>
#include "Utils.cpp"

class Move
{
public:
	Move();
	Move(int from, int to);
	Move(int from, int to, int flag);
	void SetFlag(int flag);
	const std::string ToString();
	const bool IsNotNull();

	int from;
	int to;
	int flag;
};

