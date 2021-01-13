#include <iostream>
#include <unistd.h>
#include <fstream>
#include <list>
#include <algorithm>
#include <cstdint>
#include <ctime>

#define FIRST_FIT 0
#define BEST_FIT 1
#define WORST_FIT 2

struct Chunk
{
	size_t chunk_size;
	intptr_t memory_address;

	void print() {
		std::cout << "Size: " << chunk_size << std::endl;
		std::cout << "Memory Address: " << (void*)memory_address << std::endl;
	}

	const bool operator==(const struct Chunk& a) const
	{
		bool isEqual = false;
		if (a.chunk_size == chunk_size && a.memory_address == memory_address) {
			isEqual = true;
		}

		return isEqual;
	}
};
#pragma once
