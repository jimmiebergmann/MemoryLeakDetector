#include <iostream>
#include "MemoryLeak.hpp"


int main( )
{
	int * pA = new int;
	int * pB = new int[10];

	delete pA;
	//delete pB;

	return 0;
}