#include "config.h"

#include "platform.h"


char* get_path_name(const char* file_path, char* buf, int buflen)
{
	char*  last_sls;

	if(file_path != buf)
	{
		strncpy(buf, file_path, buflen);
	}

	last_sls = strrchr(buf, '/');
	if(last_sls) 
		*last_sls = '\0';

	return buf;
}


char* get_app_path(char*  buf, int buflen)
{
#ifdef WIN32
	GetModuleFileName(NULL, buf, buflen);
#elif defined(LINUX)
	readlink("/proc/self/exe", buf, buflen);
#endif
	REGULAR_PATH(buf);


	return get_path_name(buf, buf, buflen);
}

char* get_cur_path(char*  buf, int buflen)
{
	if(NULL == getcwd(buf, buflen))
		return NULL;

	REGULAR_PATH(buf);

	return buf;
}

int set_cur_path(const char* path)
{
#ifdef WIN32
	char  win_path[MAX_PATH];
	strncpy(win_path, path, sizeof(win_path));
	WIN_PATH(win_path);
	path = win_path;
#endif
	return chdir(path);
}
