
#include <iostream>
#include "readonly_clau_parser_for_large.h"

#include <ctime>


int main(void)
{
	std::string fileName;

	wiz::Pointer global;

	std::cin >> fileName;

	char* buffer = nullptr;
	std::vector<wiz::MemoryPool*>* pool = nullptr;
	int a = clock();
	if (wiz::LoadData::LoadDataFromFile(fileName, &global, &buffer, pool, 0)) {
		//
	}
	int b = clock();

	std::cout << b - a << "ms" << "\n";

	wiz::LoadData::SaveWizDB(global, buffer, "output.eu4");

	if (buffer) {
		delete[] buffer;
	}
	if (pool->empty() == false) {
		for (auto& x : *pool) {
			x->Clear();
			delete x;
		}
		delete pool;
	}


	return 0;
}

