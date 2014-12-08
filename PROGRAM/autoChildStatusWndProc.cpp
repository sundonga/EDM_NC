#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;								// 当前实例


const MessageProc autoMessages[]={
	    WM_CREATE, goCreateAuto,
		WM_COMMAND, goCommandAuto,
		WM_PAINT,  goPaintAuto,
		WM_DESTROY, goDestroyAuto
};



CtlWndStruct autoMenuBtns[]={
		{TEXT("STATIC"),IDC_ATITLE,TEXT("Auto Running"),0,0,100,20,WS_CHILD|WS_VISIBLE},
	//	{TEXT("STATIC"),IDC_CHOSE,TEXT("Choose program:"),10,70,120,30,WS_VISIBLE | WS_CHILD | SS_CENTER},
	//	{TEXT("BUTTON"),IDC_CLIK,TEXT(">>"),80,100,60,35,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP | BS_PUSHBUTTON|SS_CENTER},
	//	{TEXT("STATIC"),IDC_RTGRAPT,TEXT("RTgraphic"),590,15,100,25,WS_VISIBLE|WS_CHILD},
	//	{TEXT("STATIC"),IDC_RTGRAP,TEXT(""),590,45,420,275,SS_CENTER|WS_VISIBLE|WS_CHILD|WS_BORDER},
	//	{TEXT("LISTBOX"),IDC_CONDITION,TEXT(""),160,45,380,200,WS_VISIBLE|WS_BORDER|WS_CHILD|WS_HSCROLL|WS_VSCROLL},
	//	{TEXT("EDIT"),IDC_INVALUE,TEXT(""),270,250,120,35,WS_VISIBLE|WS_CHILD|WS_BORDER|WS_TABSTOP},//|ES_AUTOWRAP|ES_AUTOSELECT},
	//	{TEXT("STATIC"),IDC_VALUNAME,TEXT(""),200,250,60,35,WS_CHILD|WS_VISIBLE|WS_BORDER},
	//	{TEXT("BUTTON"),IDC_BTNSURE,TEXT("SURE"),400,250,50,35,WS_VISIBLE|WS_BORDER|WS_CHILD},
	//	{TEXT("BUTTON"),IDC_AAStart,TEXT("START"),285,290,65,35,WS_CHILD|WS_BORDER|WS_VISIBLE|BS_PUSHBUTTON},
	//	{TEXT("BUTTON"),IDC_AAStop,TEXT("STOP"),385,290,65,35,WS_CHILD|WS_BORDER|WS_VISIBLE|BS_PUSHBUTTON},
	//	{TEXT("STATIC"),IDC_CONDT,TEXT("Manufacture Condition"),WS_VISIBLE|WS_CHILD}

		
};

//子窗口菜单列表
CtlWndStruct autoChildWnds[]={
	{TEXT("AUTOPARAMWND"),IDC_MANUAL,TEXT(""),200,0,824,334,WS_CHILD|SS_NOTIFY|WS_BORDER},
	{TEXT("AUTOGRAPHWND"),IDC_AUTO,TEXT(""),200,0,824,334,WS_CHILD|SS_NOTIFY|WS_BORDER}
	
};

int MyRegisterClassAuto(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)autoWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//(LPCTSTR)IDC_EX1;
	wcex.lpszClassName	= TEXT("AUTOWND");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
//注册自动窗口类
	if(!RegisterClassEx(&wcex)) return 0;

	return 1;
}

//自动窗口过程体
LRESULT CALLBACK autoWndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;

	//查找对应的消息ID,并执行对应的消息处理过程
	for(i=0; i<dim(autoMessages);i++)
	{
		if(msgCode == autoMessages[i].uCode) return(*autoMessages[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//对不存在的消息，调用缺省窗口过程
	return DefWindowProc(hWnd, msgCode, wParam, lParam);
}

LRESULT goCreateAuto(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		
    int i;
//创建自动窗口菜单
	for(i=0;i<dim(autoMenuBtns);i++){
		CreateWindow( autoMenuBtns[i].szClass,
			autoMenuBtns[i].szTitle,
			autoMenuBtns[i].lStyle,
			autoMenuBtns[i].x,
			autoMenuBtns[i].y,
			autoMenuBtns[i].cx,
			autoMenuBtns[i].cy,
			hWnd,
			(HMENU)autoMenuBtns[i].nID,
			hInst,
			NULL
			);
	}

//创建自动窗口的子窗口
	
	for(i=0;i<dim(autoChildWnds);i++){
		CreateWindow(autoChildWnds[i].szClass,
			autoChildWnds[i].szTitle,
			autoChildWnds[i].lStyle,
			autoChildWnds[i].x,
			autoChildWnds[i].y,
			autoChildWnds[i].cx,
			autoChildWnds[i].cy,
			hWnd,
			(HMENU)autoChildWnds[i].nID,
			hInst,
			NULL
			);
	}
	//显示其中一个窗口
	ShowWindow(GetDlgItem(hWnd,autoChildWnds[0].nID),SW_SHOW);
*/
	return 0;

}

LRESULT goCommandAuto(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		int wmId, wmEvent;
		int i;
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		//显示被选中的字窗口，其他子窗口隐藏
		/*
		for(i=0;i<dim(autoBtnWnds);i++){
				if(wmId == autoBtnWnds[i].nID) ShowWindow(GetDlgItem(hWnd,autoChildWnds[i].nID),SW_SHOW);
				else ShowWindow(GetDlgItem(hWnd,autoChildWnds[i].nID),SW_HIDE);
		}
*/
		return 0;
}

LRESULT goPaintAuto(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		PAINTSTRUCT ps;
		HDC hdc;

		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		return 0;
}

LRESULT goDestroyAuto(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		PostQuitMessage(0);
		return 0;
}

