
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <tchar.h>

#include <string>
#include <list>
#include <hash_map>
#include <algorithm>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <CommCtrl.h>
#include "resource.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#ifdef __cplusplus
}
#endif

#ifdef _USE_FCL
#define luaL_register(L, name, reg)  luaL_openlib((L), (name), (reg), 0)
#define luaL_Reg   luaL_reg
#endif


static void create_ui();
static void close_ui();


struct ProfStat;

struct FuncInfo {
	const void*  pv;
	std::string  name;
	bool         name_inited;
	std::string  file_name;
	int          line;
};

typedef  stdext::hash_map<const void*, FuncInfo*>  FuncInfoMap;
typedef  std::list<FuncInfo*>   CallStack;
typedef  stdext::hash_map<const void*, ProfStat*>  ProfStatMap;



struct ProfStat {
	struct FuncInfo* func;
	std::string  file_name;
	int          line;
	ProfStatMap  children;
};

static FuncInfoMap  s_funcs;
static ProfStat   s_root_stat;


static void clear_stats(struct ProfStat* stat)
{
	ProfStatMap::iterator it = stat->children.begin(); 
	ProfStatMap::iterator itend = stat->children.end(); 
	for( ; it != itend; it++)
	{
		clear_stats(it->second);
		delete it->second;
	}
	stat->children.clear();
}

static void clear_stats_root()
{
	clear_stats(&s_root_stat);
	s_root_stat.children.clear();
}

static void clear_funcs()
{
	FuncInfoMap::iterator  it = s_funcs.begin();
	FuncInfoMap::iterator  itend = s_funcs.end();

	for( ; it != itend; it++)
	{
		delete it->second;
	}
	s_funcs.clear();
}

static int lprof_init(lua_State* L)
{
	lua_Integer n = luaL_checkint(L, 1);
	printf("lprof.init: %d\n", (int)n);
	return 0;
}

static struct _timeb s_stcb;

static CallStack  s_callst;

static void lprof_hook_callret(lua_State* L, lua_Debug* ar)
{
	struct _timeb tb;
	_ftime(&tb);

	long sec = tb.time - s_stcb.time;
	int msec = tb.millitm - s_stcb.millitm;
	if(msec < 0)
	{
		sec--;
		msec+=1000;
	}

	switch(ar->event)
	{
	case LUA_HOOKCALL:
		lua_getinfo(L, "Snl", ar);
		{
			int n = lua_gettop(L);
			printf("%p:%s\n", lua_tostring(L, -n-1), lua_typename(L,lua_type(L, -n-1)));
			const void* pv = lua_topointer(L, -n-1);
			FuncInfoMap::iterator it = s_funcs.find(pv);
			if(it == s_funcs.end())
			{

			}
		}
		printf("%ld.%03d: lua function `%s' is begin called(at %s: %d).\n", 
			sec, msec, ar->name, ar->short_src, ar->currentline);
		break;
	case LUA_HOOKRET:
		if(s_callst.empty())
			break;
		lua_getinfo(L, "Snl", ar);
		printf("%ld.%03d: lua function `%s' is returned(at line: %d).\n", sec, msec, ar->name, ar->currentline);
		break;
	case LUA_HOOKTAILRET:
		printf("lua function tail call is returned.\n");
		break;
	}
	fflush(stdout);
}


static int lprof_enable(lua_State* L)
{
	if(lua_isnoneornil(L, 1))
		luaL_typerror(L, 1, "boolean");
	lua_Integer b = lua_toboolean(L, 1);
	if(b)
	{
		_ftime(&s_stcb);
		s_callst.clear();
		lua_sethook(L, lprof_hook_callret, LUA_MASKCALL|LUA_MASKRET, 0);
	}
	else
	{
		lua_sethook(L, NULL, 0, 0);
	}
	return 0;
}

static int lprof_showui(lua_State* L)
{
	if(lua_isnoneornil(L, 1))
		luaL_typerror(L, 1, "boolean");
	lua_Integer b = lua_toboolean(L, 1);
	if(b)
	{
		create_ui();
	}
	else
	{
		close_ui();
	}
	return 0;
}

static const luaL_Reg lprof_reg[] = {
	{ "init", lprof_init },
	{ "enable", lprof_enable },
	{ "showui", lprof_showui },
	{ NULL,   NULL },
};


extern "C" __declspec(dllexport) int luaopen_lprof(lua_State* L)
{
	luaL_register(L, "lprof", lprof_reg);
	return 0;
}





HINSTANCE  g_hInst;
HANDLE     g_uiThread;
HWND       g_uiDlg;



BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	g_hInst = hinstDLL;
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		g_uiThread = NULL;
		g_uiDlg = NULL;		
		break;
	case DLL_PROCESS_DETACH:
		//close_ui();
		break;
	}
	return TRUE;
}

static VOID UI_OnSize(HWND hDlg, int cx, int cy, int nSizeType);

static VOID UI_OnInitDialog(HWND hDlg)
{
	RECT  rc;
	GetWindowRect(hDlg, &rc);
	HWND hTab = GetDlgItem(hDlg, IDC_TAB);

	// init tab ctrl
	TCITEM  tcitem;
	ZeroMemory(&tcitem, sizeof(tcitem));

	tcitem.mask = TCIF_TEXT;
	tcitem.pszText = (LPTSTR)_T("CallStack");
	SendMessage(hTab, TCM_INSERTITEM, (WPARAM)0, (LPARAM)&tcitem);
	tcitem.pszText = (LPTSTR)_T("LUA Prof Tree");
	SendMessage(hTab, TCM_INSERTITEM, (WPARAM)1, (LPARAM)&tcitem);
	tcitem.pszText = (LPTSTR)_T("LUA Prof Stat.");
	SendMessage(hTab, TCM_INSERTITEM, (WPARAM)2, (LPARAM)&tcitem);

	SendMessage(hTab, TCM_SETCURSEL, (WPARAM)0, (LPARAM)0);

	HWND hLstCallStack = GetDlgItem(hDlg, IDC_LST_CALLSTACK);
	HWND hTreeProf = GetDlgItem(hDlg, IDC_TREE_PROF);
	HWND hLstStat = GetDlgItem(hDlg, IDC_LST_STAT);

	// init callstack list ctrl
	DWORD  dwlvextstyle = LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP|LVS_EX_INFOTIP;

	LVCOLUMN  lvcol;
	ZeroMemory(&lvcol, sizeof(lvcol));

	lvcol.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT;
	lvcol.fmt = LVCFMT_LEFT;
	lvcol.cx = 32;
	lvcol.pszText = (LPTSTR)_T("");
	::SendMessage(hLstCallStack, LVM_INSERTCOLUMN, (WPARAM)0, (LPARAM)&lvcol);
	lvcol.cx = 320;
	lvcol.pszText = (LPTSTR)_T("Function:");
	::SendMessage(hLstCallStack, LVM_INSERTCOLUMN, (WPARAM)1, (LPARAM)&lvcol);
	lvcol.cx = 64;
	lvcol.pszText = (LPTSTR)_T("Language");
	::SendMessage(hLstCallStack, LVM_INSERTCOLUMN, (WPARAM)2, (LPARAM)&lvcol);
	::SendMessage(hLstCallStack, LVM_SETEXTENDEDLISTVIEWSTYLE, (WPARAM)dwlvextstyle, (LPARAM)dwlvextstyle);

	TVINSERTSTRUCT  tvins;
	ZeroMemory(&tvins, sizeof(tvins));
	tvins.hParent = TVI_ROOT;
	tvins.hInsertAfter = TVI_LAST;
	tvins.item.mask = TVIF_TEXT|TVIF_STATE;	
	tvins.item.state = TVIS_BOLD;
	tvins.item.stateMask = TVIS_BOLD;
	tvins.item.pszText = (LPTSTR)_T("Root");
	::SendMessage(hTreeProf, TVM_INSERTITEM, (WPARAM)0, (LPARAM)&tvins);


	// init stat list ctrl
	//LVCOLUMN  lvcol;
	ZeroMemory(&lvcol, sizeof(lvcol));

	lvcol.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT;
	lvcol.fmt = LVCFMT_LEFT;
	lvcol.cx = 32;
	lvcol.pszText = (LPTSTR)_T("");
	::SendMessage(hLstStat, LVM_INSERTCOLUMN, (WPARAM)0, (LPARAM)&lvcol);
	lvcol.cx = 180;
	lvcol.pszText = (LPTSTR)_T("Function");
	::SendMessage(hLstStat, LVM_INSERTCOLUMN, (WPARAM)1, (LPARAM)&lvcol);
	lvcol.fmt = LVCFMT_RIGHT;
	lvcol.cx = 80;
	lvcol.pszText = (LPTSTR)_T("Count");
	::SendMessage(hLstStat, LVM_INSERTCOLUMN, (WPARAM)2, (LPARAM)&lvcol);
	lvcol.fmt = LVCFMT_RIGHT;
	lvcol.cx = 80;
	lvcol.pszText = (LPTSTR)_T("Time");
	::SendMessage(hLstStat, LVM_INSERTCOLUMN, (WPARAM)3, (LPARAM)&lvcol);
	lvcol.fmt = LVCFMT_RIGHT;
	lvcol.cx = 80;
	lvcol.pszText = (LPTSTR)_T("Average");
	::SendMessage(hLstStat, LVM_INSERTCOLUMN, (WPARAM)4, (LPARAM)&lvcol);
	lvcol.cx = 80;
	lvcol.pszText = (LPTSTR)_T("Max Time");
	::SendMessage(hLstStat, LVM_INSERTCOLUMN, (WPARAM)5, (LPARAM)&lvcol);
	::SendMessage(hLstStat, LVM_SETEXTENDEDLISTVIEWSTYLE, (WPARAM)dwlvextstyle, (LPARAM)dwlvextstyle);



	SetParent(hLstCallStack, hTab);
	ShowWindow(hLstCallStack, SW_SHOW);

	SetParent(hTreeProf, hTab);
	ShowWindow(hTreeProf, SW_HIDE);


	SetParent(hLstStat, hTab);
	ShowWindow(hLstStat, SW_HIDE);

	UI_OnSize(hDlg, rc.right-rc.left, rc.bottom-rc.top, 0);
}


static VOID UI_OnSize(HWND hDlg, int cx, int cy, int nSizeType)
{
	if(IsWindow(hDlg))
	{
		//HDWP hDWP = BeginDeferWindowPos(10);
		RECT  rcDisp;
		GetClientRect(hDlg, &rcDisp);
		HWND hTab = GetDlgItem(hDlg, IDC_TAB);
		//MoveWindow(hTab, rcDisp.left, rcDisp.top, rcDisp.right-rcDisp.left, rcDisp.bottom-rcDisp.top, TRUE);
		SetWindowPos(hTab, NULL, rcDisp.left, rcDisp.top, 
			rcDisp.right-rcDisp.left, rcDisp.bottom-rcDisp.top, SWP_NOOWNERZORDER|SWP_NOACTIVATE);
		SendMessage(hTab, TCM_ADJUSTRECT, (WPARAM)FALSE, (LPARAM)&rcDisp);
		HWND hLstCallStack = GetDlgItem(hTab, IDC_LST_CALLSTACK);
		HWND hTreeProf = GetDlgItem(hTab, IDC_TREE_PROF);
		HWND hLstStat = GetDlgItem(hTab, IDC_LST_STAT);
		SetWindowPos(hLstCallStack, NULL, rcDisp.left, rcDisp.top, 
			rcDisp.right-rcDisp.left, rcDisp.bottom-rcDisp.top, SWP_NOOWNERZORDER|SWP_NOACTIVATE);
		SetWindowPos(hTreeProf, NULL, rcDisp.left, rcDisp.top, 
			rcDisp.right-rcDisp.left, rcDisp.bottom-rcDisp.top, SWP_NOOWNERZORDER|SWP_NOACTIVATE);
		SetWindowPos(hLstStat, NULL, rcDisp.left, rcDisp.top, 
			rcDisp.right-rcDisp.left, rcDisp.bottom-rcDisp.top, SWP_NOOWNERZORDER|SWP_NOACTIVATE);

		//EndDeferWindowPos(hDWP);
	}
}

static VOID UI_TabSelChanged(HWND hDlg, HWND hCtrl)
{
	int nSel = SendMessage(hCtrl, TCM_GETCURSEL, 0, 0);

	HWND hLstCallStack = GetDlgItem(hCtrl, IDC_LST_CALLSTACK);
	ShowWindow(hLstCallStack, nSel == 0 ? SW_SHOW : SW_HIDE);

	HWND hTreeProf = GetDlgItem(hCtrl, IDC_TREE_PROF);
	ShowWindow(hTreeProf, nSel == 1 ? SW_SHOW : SW_HIDE);


	HWND hLstStat = GetDlgItem(hCtrl, IDC_LST_STAT);
	ShowWindow(hLstStat, nSel == 2 ? SW_SHOW : SW_HIDE);

	UpdateWindow(hDlg);

}


static BOOL UI_OnNotify(HWND hDlg, UINT id, LPNMHDR lpNMHDR)
{
	if(lpNMHDR->idFrom == IDC_TAB)
	{
		if(lpNMHDR->code == TCN_SELCHANGE)
		{	
			UI_TabSelChanged(hDlg, lpNMHDR->hwndFrom);
			return TRUE;
		}
	}
	return FALSE;
}

static VOID UI_OnDestroy(HWND hDlg)
{
	PostQuitMessage(0);
}

static INT_PTR CALLBACK UIProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		UI_OnInitDialog(hwndDlg);
		break;
	case WM_SIZE:
		UI_OnSize(hwndDlg, LOWORD(lParam), HIWORD(lParam), wParam);
		break;
	case WM_TIMER:
		break;
	case WM_COMMAND:
		break;
	case WM_NOTIFY:
		return UI_OnNotify(hwndDlg, (UINT)wParam, (LPNMHDR)lParam);
		break;
	case WM_DESTROY:
		UI_OnDestroy(hwndDlg);
		break;
	case WM_CLOSE:
		DestroyWindow(hwndDlg);
		break;
	}
	return FALSE;
}


static HWND  ThCreateUI()
{
	HWND h = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_LPROF_UI), NULL,  UIProc);
	if(h == NULL)
		return NULL;

	g_uiDlg = h;
	ShowWindow(h, SW_SHOW);

	return h;
}

static DWORD WINAPI UIThreadProc(LPVOID lpParameter)
{
	HWND hUI = ThCreateUI();

	if(hUI == NULL)
		return -1;
	MSG  msg;
	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}


static BOOL CreateUIThread()
{
	DWORD  dwID = 0;
	HANDLE hthUI = CreateThread(NULL, 0,  UIThreadProc, NULL, 0, &dwID);
	g_uiThread = hthUI;
	return hthUI != NULL;
}

static void create_ui()
{
	if(g_uiThread == NULL)
	{
		CreateUIThread();
	}
}

static void close_ui()
{
	if(g_uiThread!=NULL)
	{
		SendMessage(g_uiDlg, WM_CLOSE, 0, 0);
		::WaitForSingleObject(g_uiThread, INFINITE);
		CloseHandle(g_uiThread);
		g_uiThread = NULL;
		g_uiDlg = NULL;
	}
}

