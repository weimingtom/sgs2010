#include "config.h"
#include "logic.h"



int main(int argc, char** argv)
{
	RESULT ret = R_SUCC;
	int test_mode = 0;
	if(argc >= 2 && !strcasecmp(argv[1], "-t"))
	{
		test_mode = 1;
	}


	ret = logic_begin(test_mode);

	if(ret != R_SUCC)
	{
		return 1;
	}


	// cmd loop
	// TODO:

	while(logic_isrunning())
	{
		sleep_us(1000);
	}


	logic_finish();

	return 0;
}




