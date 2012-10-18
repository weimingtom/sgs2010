#include "stdafx.h"
#include "../include/resource.h"


static BOOL CALLBACK MainDialogCallback(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

int CALLBACK WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, MainDialogCallback);

	return 0;

}


static BOOL CALLBACK MainDialogCallback(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{

	case WM_COMMAND:
		if(LOWORD(wParam) == IDOK)
		{		
			EndDialog(hDlg, IDOK);
			return TRUE;
		}
		else if(LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
	
		break;
	}
	return FALSE;
}
