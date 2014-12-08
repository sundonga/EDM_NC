#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"
									// 当前实例
HBITMAP hBitmap_Screen;

extern HINSTANCE hInst;								// 当前实例

HWND hScreenEdit;


int isEndScreen;


const dlgMessageProc ScreenMessage[]=
{
	WM_PAINT,goPaintScreenWnd,
	WM_COMMAND,goCommandScreenWnd
};

BOOL CALLBACK screenDlgProc(HWND hWnd,UINT msgCode,WPARAM wParam,LPARAM lParam)
{
	int i;
	hScreenEdit=GetDlgItem(hWnd,IDC_SCREEN_EDIT);
	//查找对应的消息ID号，并执行相应的消息处理过程
	for(i=0;i<dim(ScreenMessage);i++)
	{
		if(msgCode==ScreenMessage[i].uCode)
			return (*ScreenMessage[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	
	return FALSE;
}

BOOL goCommandScreenWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDC_SCREEN_EDIT:
		if((HIWORD(wParam)==EN_CHANGE)&&(isEndScreen==1))
		{
			EndDialog(hWnd,0);
			return TRUE;
		}
		break;
	}
	return 0;
}

BOOL goPaintScreenWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HBRUSH hOldBrush;
	RECT rect;
	BITMAP bmp;
	HDC hdc,hdcMem;
	if(hBitmap_Screen==0)
	{
		hBitmap_Screen=(HBITMAP)LoadImage(hInst,MAKEINTRESOURCE(IDB_BITMAP9),IMAGE_BITMAP,0,0,0);
	}
	GetClientRect(hWnd,&rect);	
	hdc=BeginPaint(hWnd,&ps);
	SetRect(&rect,0,0,1024,788);
	FillRect(hdc,&rect,CreateSolidBrush(RGB(255,255,255)));
	hdcMem=CreateCompatibleDC(hdc);
	hOldBrush=(HBRUSH)SelectObject(hdcMem,hBitmap_Screen);
	GetObject(hBitmap_Screen,sizeof(BITMAP),&bmp);
	SetRect(&rect,0,0,1024,500);
	BitBlt(hdc,rect.left,rect.top,1024,500,hdcMem,0,0,SRCCOPY);
	SelectObject(hdcMem,hOldBrush);
	DeleteDC(hdcMem);
	SetBkMode(hdc,TRANSPARENT);
	
	EndPaint(hWnd,&ps);
	return 0;
}
