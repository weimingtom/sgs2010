#include "config.h"
#include "comm.h"


void fill_array_inc_i(int* array, int size, int first, int inc)
{
	int n;
	for(n = 0; n < size; n++)
	{
		array[n] = first;
		first += inc;
	}
}

void rand_array_i(int* array, int size, int rn)
{
	int t, a, b;

	while(rn-->0)
	{
		a = rand() % size;
		b = rand() % size;

		if(a != b)
		{
			t = array[a];
			array[a] = array[b];
			array[b] = t;
		}
	}
}



