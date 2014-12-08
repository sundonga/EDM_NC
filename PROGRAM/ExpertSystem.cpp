#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;								// 当前实例
extern LPCmdThreadParam  pDataInEXPERT;

//2010-6-24

const MessageProc EXPERTMessage[]=
{
	WM_CREATE,goCreateEXPERTWnd,
	WM_PAINT,goPaintEXPERTWnd,
	WM_KEYDOWN,goKeydownEXPERTWnd,
	WM_COMMAND,goCommandEXPERTWnd,
	WM_DESTROY,goDestroyEXPERTWnd
};
//定义七个控件窗口的创建信息	
CtlWndStruct EXPERTWnd[]={
	{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_EXPERTEDIT,TEXT("EXPERT"),260,620,750,30,WS_VISIBLE|WS_BORDER|WS_CHILD}
};
//CtlWndStruct EXPERTChildWnds[]={
//	{WS_EX_CLIENTEDGE,TEXT("EXPERTChildWND"),IDC_EXPERTChild,TEXT(""),0,0,1024,620,WS_CHILD|SS_NOTIFY|WS_BORDER}
//
//};


int MyRegisterClassEXPERT(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			=CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)EXPERTWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);//
	wcex.lpszMenuName	= NULL;//(LPCTSTR)IDC_EX1;
	wcex.lpszClassName	= TEXT("EXPERT_SYSTEM");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
//注册自动窗口类
	if(!RegisterClassEx(&wcex)) return 0;
	//if(!MyRegisterClassExpertSystemChild(hInstance))return 0;
	return 1;
}
//子窗口过程体，必须为回调函数，当窗口收到消息后，会告知Windows CE系统执行对应的消息处理过程
LRESULT CALLBACK EXPERTWndProc(HWND hWnd,UINT msgCode,WPARAM wParam,LPARAM lParam)
{
	int i;
	//查找对应的消息ID号，并执行相应的消息处理过程
	for(i=0;i<dim(EXPERTMessage);i++)
	{
		if(msgCode==EXPERTMessage[i].uCode)
			return (*EXPERTMessage[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//对于不再消息查找表中的消息，调用Windows CE的缺省窗口过程
	return DefWindowProc(hWnd,msgCode,wParam,lParam);
}
//子窗口收到WM_CREATE消息后的处理过程体
LRESULT  goCreateEXPERTWnd(HWND hWnd,UINT msgCode,WPARAM wParam,LPARAM lParam)
{
	int i;
	//2010-3-30
	//创建自动加工参数窗口菜单
	for(i=0;i<dim(EXPERTWnd);i++)
	{
		CreateWindowEx(EXPERTWnd[i].dwExStyle,
			EXPERTWnd[i].szClass,
			EXPERTWnd[i].szTitle,
			EXPERTWnd[i].lStyle,
			EXPERTWnd[i].x,
			EXPERTWnd[i].y,
			EXPERTWnd[i].cx,
			EXPERTWnd[i].cy,
			hWnd,
			(HMENU)EXPERTWnd[i].nID,
			hInst,
			NULL
			);
	}
	//for(i=0;i<dim(EXPERTChildWnds);i++){
	//	CreateWindowEx(EXPERTChildWnds[i].dwExStyle,
	//	    EXPERTChildWnds[i].szClass,
	//		EXPERTChildWnds[i].szTitle,
	//		EXPERTChildWnds[i].lStyle,
	//		EXPERTChildWnds[i].x,
	//		EXPERTChildWnds[i].y,
	//		EXPERTChildWnds[i].cx,
	//	    EXPERTChildWnds[i].cy,
	//		hWnd,
	//		(HMENU)EXPERTChildWnds[i].nID,
	//		hInst,
	//		NULL
	//		);
	//}
	//ShowWindow(GetDlgItem(hWnd,IDC_EXPERTChild),SW_SHOW);

	return 0;
}
LRESULT goCommandEXPERTWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT goPaintEXPERTWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//定义画板结构体对象
	PAINTSTRUCT ps;
	HDC hdc;
	hdc=BeginPaint(hWnd,&ps);
	EndPaint(hWnd,&ps);
	return 0;
}

LRESULT goDestroyEXPERTWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	PostQuitMessage(0);
	return 0;
}
LRESULT goKeydownEXPERTWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	TCHAR szBuffer[501];
	int menuID, notifyCode;
	HWND wndCtrl;
	int nSelItem;
	int i;

	nSelItem = -1;
	menuID    = LOWORD(wParam); 
	notifyCode = HIWORD(wParam); 
	wndCtrl = (HWND) lParam;

	nSelItem =  ListView_GetNextItem(pDataInEXPERT->wndCtrl,-1,LVNI_DROPHILITED);
	GetWindowText (GetDlgItem(GetParent(pDataInEXPERT->hWnd),IDC_EXPERTEDIT), szBuffer, 30);//得到输入文本框内所输入数值

	SetWindowText (GetDlgItem(GetParent(pDataInEXPERT->hWnd),IDC_EXPERTEDIT),"");
	ListView_SetItemText(pDataInEXPERT->wndCtrl,nSelItem,0,szBuffer);              //??
	//将选择项蓝背景颜色去掉
	for(i=0;i<2;i++)ListView_SetItemState(GetDlgItem(pDataInEXPERT->hWnd,IDC_SIMPLE+i),-1,LVIS_SELECTED, 0XFF);

	return 0;
}