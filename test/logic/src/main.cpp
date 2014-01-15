#include "config.h"
#include "logic.h"
#include "cmd.h"
#include "comm.h"


static char* get_line(const char* prompt, char* buf, int size)
{
#ifdef WIN32
	int n,c;

	//fflush(stdin);

	MSG_OUT("%s", prompt);


	for(n = 0; n < size-1; n++)
	{
		c = getchar();
		if(c == EOF)
		{
			if(n == 0)
				return NULL;
			break;
		}
		else if(c=='\n')
		{
			break;
		}
		else
		{
			buf[n] = (char)c;
		}
	}
	buf[n] = 0;

#elif defined(LINUX)
	char    utf8[1024];
	char*   sl;


	A2UTF8( prompt, utf8, sizeof(utf8));

	log_text(utf8);
	sl = readline(utf8);

	if(strlen(sl) > 0)
		add_history(sl);

	strncpy(buf, sl, size);

	free(sl);

#endif

	log_text("%s\n", buf);

	return buf;
}



int main(int argc, char** argv)
{
	RESULT ret = R_SUCC;
	int test_mode = 0;
	char  promptstr[1024];
	char  inputstr[4096];


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
		if(logic_check_input(promptstr, sizeof(promptstr)))
		{
			logic_send_cmd(get_line(promptstr, inputstr, sizeof(inputstr)));
		}
		else
		{
			sleep_us(1000);
		}
	}


	logic_finish();

	return 0;
}




