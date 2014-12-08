#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;								// 当前实例

FILE* fpSet;
FILE* fpMac;
FILE* fpSys;
extern FILE* fpNow;
extern void fn_upday(HWND);

int MILL_ESPARK=0;

//2010-6-24
int NCSet;
extern void NCSetT_PThreadProc(); 
extern void NCSetT_TThreadProc(); 

const MessageProc NCSetBMessage[]=
{
	WM_CREATE,goCreateNCSetBWnd,
	WM_PAINT,goPaintNCSetBWnd,
	WM_KEYDOWN, goKeydownNCSetBWnd,
	WM_COMMAND,goCommandNCSetBWnd,
	WM_DESTROY,goDestroyNCSetBWnd
};
//定义七个控件窗口的创建信息	
CtlWndStruct NCSetBWnd[]={
	{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_NCSETPAR,TEXT("PARAMETER"),0,0,150,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP | BS_PUSHBUTTON|SS_CENTER},
	{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_NCSETTAP,TEXT("TAPERPRMSET"),0,50,150,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP | BS_PUSHBUTTON|SS_CENTER},
	{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_NCSETPSET,TEXT("SET"),160,0,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP | BS_PUSHBUTTON|SS_CENTER},
	{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_NCSETPMAC,TEXT("MACHINE"),160,50,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP | BS_PUSHBUTTON|SS_CENTER},
	{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_NCSETPSYS,TEXT("SYSTEM"),160,100,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP | BS_PUSHBUTTON|SS_CENTER},
	{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_NCSETPMILL,TEXT(""),0,100,150,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP | BS_PUSHBUTTON|SS_CENTER},//2010-5-25
	{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_NCSETPEDIT,TEXT(""),260,620,750,30,WS_VISIBLE|WS_BORDER|WS_CHILD}
};


CtlWndStruct NCSetChildWnds[]={
	{WS_EX_CLIENTEDGE,TEXT("NCSetT_P"),IDW_NCSETTP,TEXT(""),260,0,754,620,WS_CHILD|SS_NOTIFY|WS_BORDER|WS_VISIBLE},
	{WS_EX_CLIENTEDGE,TEXT("NCSetT_T"),IDW_NCSETTT,TEXT(""),260,0,754,620,WS_CHILD|SS_NOTIFY|WS_BORDER|WS_VISIBLE}
};

int MyRegisterClassNCSetB(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			=CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)NCSetBWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);//
	wcex.lpszMenuName	= NULL;//(LPCTSTR)IDC_EX1;
	wcex.lpszClassName	= TEXT("NCSETB");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
//注册自动窗口类
	if(!RegisterClassEx(&wcex)) return 0;
	if(!MyRegisterClassNCSetT_P(hInstance)) return 0;
	if(!MyRegisterClassNCSetT_T(hInstance)) return 0;

	return 1;
}
//子窗口过程体，必须为回调函数，当窗口收到消息后，会告知Windows CE系统执行对应的消息处理过程
LRESULT CALLBACK NCSetBWndProc(HWND hWnd,UINT msgCode,WPARAM wParam,LPARAM lParam)
{
	int i;
	//查找对应的消息ID号，并执行相应的消息处理过程
	for(i=0;i<dim(NCSetBMessage);i++)
	{
		if(msgCode==NCSetBMessage[i].uCode)
			return (*NCSetBMessage[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//对于不再消息查找表中的消息，调用Windows CE的缺省窗口过程
	return DefWindowProc(hWnd,msgCode,wParam,lParam);
}
//子窗口收到WM_CREATE消息后的处理过程体
LRESULT  goCreateNCSetBWnd(HWND hWnd,UINT msgCode,WPARAM wParam,LPARAM lParam)
{
	int i;
	//2010-3-30
	//创建自动加工参数窗口菜单
	for(i=0;i<dim(NCSetBWnd);i++)
	{
		CreateWindowEx(NCSetBWnd[i].dwExStyle,
			NCSetBWnd[i].szClass,
			NCSetBWnd[i].szTitle,
			NCSetBWnd[i].lStyle,
			NCSetBWnd[i].x,
			NCSetBWnd[i].y,
			NCSetBWnd[i].cx,
			NCSetBWnd[i].cy,
			hWnd,
			(HMENU)NCSetBWnd[i].nID,
			hInst,
			NULL
			);
	}
	for(i=0;i<dim(NCSetChildWnds);i++){
		CreateWindowEx(NCSetChildWnds[i].dwExStyle,
			NCSetChildWnds[i].szClass,
			NCSetChildWnds[i].szTitle,
			NCSetChildWnds[i].lStyle,
			NCSetChildWnds[i].x,
			NCSetChildWnds[i].y,
			NCSetChildWnds[i].cx,
			NCSetChildWnds[i].cy,
			hWnd,
			(HMENU)NCSetChildWnds[i].nID,
			hInst,
			NULL
			);
	}
	ShowWindow(GetDlgItem(hWnd,IDC_NCSETPSET),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_NCSETPMAC),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_NCSETPSYS),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDW_NCSETTT),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDW_NCSETTP),SW_HIDE);
	//2010-5-25
	if(MILL_ESPARK==1)SetWindowText (GetDlgItem(hWnd,IDC_NCSETPMILL),"MILL");
	else if(MILL_ESPARK==0)SetWindowText (GetDlgItem(hWnd,IDC_NCSETPMILL),"ESPARK");
	//end 2010-5-25
	NCSet=0;
	return 0;
}
LRESULT goCommandNCSetBWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDC_NCSETPAR:
		ShowWindow(GetDlgItem(hWnd,IDC_NCSETPSET),SW_SHOW);
		ShowWindow(GetDlgItem(hWnd,IDC_NCSETPMAC),SW_SHOW);
		ShowWindow(GetDlgItem(hWnd,IDC_NCSETPSYS),SW_SHOW);
		ShowWindow(GetDlgItem(hWnd,IDW_NCSETTT),SW_HIDE);
		ShowWindow(GetDlgItem(hWnd,IDW_NCSETTP),SW_HIDE);
		break;
	case IDC_NCSETPSET:
		fpNow=fpSet;
		ShowWindow(GetDlgItem(hWnd,IDW_NCSETTP),SW_SHOW);
		ShowWindow(GetDlgItem(hWnd,IDW_NCSETTT),SW_HIDE);
		fn_upday(GetDlgItem(hWnd,IDW_NCSETTP));
		break;
	case IDC_NCSETPMAC:
		fpNow=fpMac;
		ShowWindow(GetDlgItem(hWnd,IDW_NCSETTP),SW_SHOW);
		ShowWindow(GetDlgItem(hWnd,IDW_NCSETTT),SW_HIDE);
		fn_upday(GetDlgItem(hWnd,IDW_NCSETTP));
		break;
	case IDC_NCSETPSYS:
		fpNow=fpSys;
		ShowWindow(GetDlgItem(hWnd,IDW_NCSETTP),SW_SHOW);
		ShowWindow(GetDlgItem(hWnd,IDW_NCSETTT),SW_HIDE);
		fn_upday(GetDlgItem(hWnd,IDW_NCSETTP));
		break;
	case IDC_NCSETTAP:
		ShowWindow(GetDlgItem(hWnd,IDC_NCSETPSET),SW_HIDE);
		ShowWindow(GetDlgItem(hWnd,IDC_NCSETPMAC),SW_HIDE);
		ShowWindow(GetDlgItem(hWnd,IDC_NCSETPSYS),SW_HIDE);
		ShowWindow(GetDlgItem(hWnd,IDW_NCSETTT),SW_SHOW);
		ShowWindow(GetDlgItem(hWnd,IDW_NCSETTP),SW_HIDE);
		UpdateWindow(GetDlgItem(GetParent(hWnd),IDW_NCSETTT));
		break;
	//2010-5-25
	case IDC_NCSETPMILL:
		if(MILL_ESPARK==1)
		{
			MILL_ESPARK=0;
			SetWindowText (GetDlgItem(hWnd,IDC_NCSETPMILL),"ESPARK");
		}
		else 
		{
			MILL_ESPARK=1;
			SetWindowText (GetDlgItem(hWnd,IDC_NCSETPMILL),"MILL");
		}
		break;
	//end 2010-5-25
	default:
		break;
	}
	return 0;
}

LRESULT goPaintNCSetBWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//定义画板结构体对象
	PAINTSTRUCT ps;
	HDC hdc;
	hdc=BeginPaint(hWnd,&ps);
	EndPaint(hWnd,&ps);
	return 0;
}

LRESULT goDestroyNCSetBWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	PostQuitMessage(0);
	return 0;
}
LRESULT goKeydownNCSetBWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int menuID, notifyCode;		
	//定义窗口句柄
	HWND wndCtrl;
	menuID    = LOWORD(wParam); 
	notifyCode = HIWORD(wParam); 
	wndCtrl = (HWND) lParam;
	switch(NCSet)
	{
	case 1:
		NCSetT_PThreadProc();
		break;
	case 2:
		NCSetT_TThreadProc();
		break;
	}
	return 0;
}