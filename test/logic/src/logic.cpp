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

typedef HANDLE event_t;




#elif defined (LINUX)
#include <pthread.h>

typedef pthread_t    thread_t;
typedef void* (*ThreadFunc)(void*);

typedef struct tagEvent
{
	pthread_mutex_t  _mutex;
	pthread_cond_t   _cond;
	int              _signaled;
} event_t;

#endif



static int create_thread_i(thread_t* pth, ThreadFunc  func, void*  param)
{
	int ret;
	thread_t  th = -1;
#ifdef WIN32
	 th = _beginthread(func, 0, param);
	 if(th == 0)
	 {
		 ret = errno;
	 }
	 else
	 {
		 ret = 0;
		 *pth = th;
	 }

#elif defined (LINUX)
	ret = pthread_create(pth, NULL, func,  param);
#endif
	return ret;	
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


int event_create_i(event_t* ev)
{
#ifdef WIN32
	*ev = CreateEvent(NULL, FALSE, FALSE, NULL);
	return *ev == NULL ? -1 : 0;
#elif defined (LINUX)
	pthread_mutex_init(&ev->_mutex, NULL);
	pthread_cond_init(&ev->_cond, NULL);
	ev->_signaled = 0;
	return 0;
#endif
}


void event_destroy_i(event_t* ev)
{
#ifdef WIN32
	CloseHandle(*ev);
#elif defined (LINUX)
	pthread_cond_destroy(&ev->_cond);
	pthread_mutex_destroy(&ev->_mutex);
	ev->_signaled = 0;
#endif
}


void event_set_i(event_t* ev)
{
#ifdef WIN32
	SetEvent(*ev);
#elif defined (LINUX)
	pthread_mutex_lock(&ev->_mutex);
	ev->_signaled = 1;
	pthread_cond_broadcast(&ev->_cond);
	pthread_mutex_unlock(&ev->_mutex);
#endif
}


void event_wait_i(event_t* ev)
{
#ifdef WIN32
	WaitForSingleObject(*ev, INFINITE);
#elif defined (LINUX)
	pthread_mutex_lock(&ev->_mutex);
	while(!ev->_signaled)
	{
		pthread_cond_wait(&ev->_cond, &ev->_mutex);
	}
	ev->_signaled = 0;
	pthread_mutex_unlock(&ev->_mutex);
#endif
}


int event_wait_np(event_t* ev, int msec)
{
#ifdef WIN32
	if(WAIT_OBJECT_0 == WaitForSingleObject(*ev, msec))
	{
		return 0;
	}
	return -1;
#elif defined (LINUX)
	struct timespec  to;
	to.tv_sec = msec / 1000;
	to.tv_nsec = msec % 1000 * 1000 * 1000;

	pthread_mutex_lock(&ev->_mutex);
	while(!ev->_signaled)
	{
		if(ETIMEDOUT == pthread_cond_timedwait(&ev->_cond, &ev->_mutex, &to))
		{
			pthread_mutex_unlock(&ev->_mutex);
			return -1;
		}
	}
	ev->_signaled = 0;
	pthread_mutex_unlock(&ev->_mutex);
#endif

}


// 
static thread_t g_logic_thread_id = 0;


RESULT logic_begin(int test_mode)
{
	int ret;
	thread_t thid;
	LogicThreadParam  param;
	param.test_mode = test_mode;

	ret = create_thread_i(&thid, thread_proc, (void*)&param);

	if((int)thid < 0)
	{
		MSG_OUT("create logic thread failed (%d) %s\n", ret, strerror(ret));
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



enum { in_init, in_fini, in_wait, in_check, in_send, };

static int logic_input_swap(int mode, const char* instr, char* outstr, int len)
{
	static int inited = 0;
	static event_t event_input_wait;
	static event_t event_input_complete;
	static char prompt_str[1024];
	static char input_str[4096];

	if(inited == 0)
	{
		if (0 != event_create_i(&event_input_wait) ||
			0 != event_create_i(&event_input_complete) )
		{
			return -1;
		}
		inited = 1;
	}

	switch(mode)
	{
	case in_wait:
		strncpy(prompt_str, instr, sizeof(prompt_str));
		prompt_str[sizeof(prompt_str) - 1] = 0;
		event_set_i(&event_input_wait);
		event_wait_i(&event_input_complete);
		strncpy(outstr, input_str, len);
		outstr[len - 1] = 0;
		return 0;
	case in_check:
		if(0 != event_wait_np(&event_input_wait, 0))
		{
			return -1;
		}
		strncpy(outstr, prompt_str, len);
		outstr[len - 1] = 0;
		return 0;

	case in_send:
		//event_wait_i(&event_input_wait);
		strncpy(input_str, instr, sizeof(input_str));
		input_str[sizeof(input_str) - 1] = 0;
		event_set_i(&event_input_complete);
		return 0;
	case in_fini:
		if(inited  == 1)
		{
			event_destroy_i(&event_input_wait);
			event_destroy_i(&event_input_complete);
			inited = 0;
		}
		break;
	}	
	return 0;
}


char* logic_wait_cmd(const char* prompt, char* buf, int size)
{
	if(0 != logic_input_swap(in_wait, prompt, buf, size))
		return NULL;

	return buf;
}

int  logic_check_input(char* prompt, int size)
{
	return logic_input_swap(in_check,  NULL, prompt, size) == 0 ? 1 : 0;
}


RESULT logic_send_cmd(const char* cmd_str)
{
	logic_input_swap(in_send,  cmd_str, NULL, 0);
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

		logic_input_swap(in_fini, NULL, NULL, 0);
	}
}


