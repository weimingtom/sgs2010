/**

 logic thread

*/

#include "config.h"

#include "game.h"
#include "cmd.h"
#include "comm.h"
#include "event.h"
#include "script.h"

#ifdef WIN32
#include <process.h>

typedef uintptr_t    thread_t;
typedef void (*ThreadFunc)(void*);

#elif defined (LINUX)
#include <pthread.h>

typedef pthread_t    thread_t;
typedef void* (*ThreadFunc)(void*);

#endif



static thread_t create_thread_i(ThreadFunc  func, void*  param)
{
	thread_t  th = -1;
#ifdef WIN32
	 th = _beginthread(func, 0, param);
#elif defined (LINUX)
	int ret;
	ret = pthread_create(&th, NULL, func,  param);
	if(ret != 0)
	{
		th = -1;
	}
#endif

	return th;
}


static void thread_wait_i(thread_t  thid)
{
#ifdef WIN32
	WaitForSingleObject((HANDLE) thid, INFINITE);
#elif defined (LINUX)
	pthread_join(thid, NULL);
#endif
}


static int thread_wait_np(thread_t  thid)
{
#ifdef WIN32
	return WAIT_TIMEOUT == WaitForSingleObject((HANDLE) thid, 0) ? 0 : 1;
#elif defined (LINUX)
	return EBUSY == pthread_tryjoin_np(thid, NULL) ? 0 : 1;
#endif
}


typedef struct tagLogicThreadParam
{
	int test_mode;
}LogicThreadParam;



static RESULT logic_proc(int test_mode);


#ifdef WIN32
static void thread_proc(void*  param)
#elif defined (LINUX)
static void* thread_proc(void*  param)
#endif
{
	LogicThreadParam* p;
	RESULT ret;


	p = (LogicThreadParam*)param;


#ifdef LINUX
	// set the thread can be canceled.
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

	// set the thread can be canceled at any time.
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
#endif



	ret = logic_proc(p->test_mode);


#ifdef WIN32
	return;
#elif defined (LINUX)
	return 0;
#endif
}

// 
static thread_t g_logic_thread_id = 0;


RESULT logic_begin(int test_mode)
{
	LogicThreadParam  param;
	param.test_mode = test_mode;

	thread_t thid =  create_thread_i(thread_proc, (void*)&param);

	if((int)thid < 0)
	{
		MSG_OUT("create logic thread failed (%d) %s", errno, strerror(errno));
		return R_E_FAIL;
	}

	g_logic_thread_id = thid;

	return R_SUCC;
}


static RESULT logic_proc(int test_mode)
{
	RESULT   ret;
	GameContext   stGameContext;
	GameEventContext  stEvent;

	srand((int)time(NULL));

	MSG_OUT(PROJ_NAME" "VERSION_STR"\n");

	if(R_SUCC != (ret = init_game_script()))
	{
		return ret;
	}

	ST_ZERO(stGameContext);

	if(test_mode != 0)
	{
		if(R_SUCC == script_test_run(&stGameContext, -1))
		{
			set_test_mode();
		}
	}


	GameEventContext* pParentEvent = NULL;

	INIT_EVENT(&stEvent, GameEvent_None, INVALID_PLAYER, INVALID_PLAYER, pParentEvent);

	while (R_EXIT != cmd_loop(&stGameContext, &stEvent, YES, NULL))
	{
		// next game
	}

	close_game_script();

	return R_SUCC;
}


RESULT logic_cmd(const char* cmd_str)
{
	return R_SUCC;
}


YESNO logic_isrunning()
{
	if(g_logic_thread_id == 0)
		return NO;

	return thread_wait_np(g_logic_thread_id) ? NO : YES;
}

void logic_finish()
{
	if(g_logic_thread_id != 0)
	{
		// 1. send finish cmd
		// TODO: 
		// 2. wait thread exit

		thread_wait_i(g_logic_thread_id);
		g_logic_thread_id = 0;
	}
}


