#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;								// 当前实例


double wire_bs;
double z_position;
double xy_plane;
double tapep_angle;
double h_thickness;
double a_uv_plane;
double b_xy_plane;
int NCSetID;
void NCSetT_TThreadProc();
extern int NCSet;
LPCmdThreadParam  pDataInNCSetT_T;

HBITMAP hBitmap_NCSet;


const MessageProc NCSetTTMessage[]=
{
	WM_CREATE,goCreateNCSetT_TWnd,
	WM_PAINT,goPaintNCSetT_TWnd,
	WM_COMMAND,goCommandNCSetT_TWnd,
	WM_DESTROY,goDestroyNCSetT_TWnd
};
//定义七个控件窗口的创建信息	

CtlWndStruct NCSetTTWnd[]={
	{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_WIRE_BS,TEXT("0.000"),570,70,100,30,WS_VISIBLE|WS_BORDER|WS_CHILD|ES_UPPERCASE|ES_NUMBER|ES_READONLY|ES_RIGHT},
	{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_Z_POSITION,TEXT("0.000"),570,120,100,30,WS_VISIBLE|WS_BORDER|WS_CHILD|ES_UPPERCASE|ES_NUMBER|ES_READONLY|ES_RIGHT},
	{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_XY_PLANE,TEXT("0.000"),570,220,100,30,WS_VISIBLE|WS_BORDER|WS_CHILD|ES_NOHIDESEL|ES_RIGHT},
	{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_TAPEP_ANGLE,TEXT("0.000"),570,270,100,30,WS_VISIBLE|WS_BORDER|WS_CHILD|ES_NOHIDESEL|ES_RIGHT},
	{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_H,TEXT("0.000"),570,370,100,30,WS_VISIBLE|WS_BORDER|WS_CHILD|ES_NOHIDESEL|ES_RIGHT},
	{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_A,TEXT("0.000"),570,470,100,30,WS_VISIBLE|WS_BORDER|WS_CHILD|ES_NOHIDESEL|ES_RIGHT},
	{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_B,TEXT("0.000"),570,520,100,30,WS_VISIBLE|WS_BORDER|WS_CHILD|ES_NOHIDESEL|ES_RIGHT}
};

int MyRegisterClassNCSetT_T(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			=CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)NCSetT_TWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);//
	wcex.lpszMenuName	= NULL;//(LPCTSTR)IDC_EX1;
	wcex.lpszClassName	= TEXT("NCSETT_T");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
//注册自动窗口类
	if(!RegisterClassEx(&wcex)) return 0;

	return 1;
}
//子窗口过程体，必须为回调函数，当窗口收到消息后，会告知Windows CE系统执行对应的消息处理过程
LRESULT CALLBACK NCSetT_TWndProc(HWND hWnd,UINT msgCode,WPARAM wParam,LPARAM lParam)
{
	int i;
	//查找对应的消息ID号，并执行相应的消息处理过程
	for(i=0;i<dim(NCSetTTMessage);i++)
	{
		if(msgCode==NCSetTTMessage[i].uCode)
			return (*NCSetTTMessage[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//对于不再消息查找表中的消息，调用Windows CE的缺省窗口过程
	return DefWindowProc(hWnd,msgCode,wParam,lParam);
}
//子窗口收到WM_CREATE消息后的处理过程体
LRESULT  goCreateNCSetT_TWnd(HWND hWnd,UINT msgCode,WPARAM wParam,LPARAM lParam)
{
	int i;
	TCHAR szBuffer[_MAX_FNAME];
	//创建自动加工参数窗口菜单
	for(i=0;i<dim(NCSetTTWnd);i++)
	{
		CreateWindowEx(NCSetTTWnd[i].dwExStyle,
			NCSetTTWnd[i].szClass,
			NCSetTTWnd[i].szTitle,
			NCSetTTWnd[i].lStyle,
			NCSetTTWnd[i].x,
			NCSetTTWnd[i].y,
			NCSetTTWnd[i].cx,
			NCSetTTWnd[i].cy,
			hWnd,
			(HMENU)NCSetTTWnd[i].nID,
			hInst,
			NULL
			);
	}
	if(hBitmap_NCSet==0)
	{
		hBitmap_NCSet=(HBITMAP)LoadImage(hInst,MAKEINTRESOURCE(IDB_BITMAP10),IMAGE_BITMAP,0,0,0);
	}
	wire_bs=0.0;
	z_position=100.0;
	xy_plane=10.0;
	tapep_angle=0.0;
	h_thickness=50.0;
	a_uv_plane=0.0;
	b_xy_plane=0.0;
	sprintf_s(szBuffer,"%f",wire_bs);
	SetWindowText (GetDlgItem(hWnd,IDC_WIRE_BS),szBuffer);
	sprintf_s(szBuffer,"%f",z_position);
	SetWindowText (GetDlgItem(hWnd,IDC_Z_POSITION),szBuffer);
	sprintf_s(szBuffer,"%f",xy_plane);
	SetWindowText (GetDlgItem(hWnd,IDC_XY_PLANE),szBuffer);
	sprintf_s(szBuffer,"%f",tapep_angle);
	SetWindowText (GetDlgItem(hWnd,IDC_TAPEP_ANGLE),szBuffer);
	sprintf_s(szBuffer,"%f",h_thickness);
	SetWindowText (GetDlgItem(hWnd,IDC_H),szBuffer);
	sprintf_s(szBuffer,"%f",a_uv_plane);
	SetWindowText (GetDlgItem(hWnd,IDC_A),szBuffer);
	sprintf_s(szBuffer,"%f",b_xy_plane);
	SetWindowText (GetDlgItem(hWnd,IDC_B),szBuffer);
	//2010-6-24
	pDataInNCSetT_T = (LPCmdThreadParam)HeapAlloc(GetProcessHeap(),
											HEAP_ZERO_MEMORY,
											sizeof(CmdThreadParam)
											);
	if(pDataInNCSetT_T == NULL)
	{
		return 1;
	}
	//end 2010-6-24
	return 0;
}
LRESULT goCommandNCSetT_TWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	if(HIWORD(wParam)==EN_SETFOCUS)
	{
		switch(LOWORD(wParam))
		{		
		case IDC_WIRE_BS:
			SendMessage(GetDlgItem(hWnd,IDC_XY_PLANE),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_TAPEP_ANGLE),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_H),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_A),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_B),EM_SETSEL,-1,0);
			pDataInNCSetT_T->hWnd = hWnd;
			pDataInNCSetT_T->wndCtrl = GetDlgItem(hWnd,LOWORD(wParam));
			NCSetID=IDC_WIRE_BS;
			pDataInNCSetT_T->menuID = LOWORD(wParam);
			pDataInNCSetT_T->notifyCode = 0;
			SendMessage(GetDlgItem(hWnd,LOWORD(wParam)),EM_SETSEL,0,-1);
			SetFocus(GetDlgItem(GetParent(hWnd),IDC_NCSETPEDIT));
			NCSet=2;
			break;
		case IDC_Z_POSITION:
			SendMessage(GetDlgItem(hWnd,IDC_XY_PLANE),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_TAPEP_ANGLE),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_H),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_A),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_B),EM_SETSEL,-1,0);
			pDataInNCSetT_T->hWnd = hWnd;
			pDataInNCSetT_T->wndCtrl = GetDlgItem(hWnd,LOWORD(wParam));
			pDataInNCSetT_T->menuID = LOWORD(wParam);
			NCSetID=IDC_Z_POSITION;
			pDataInNCSetT_T->notifyCode = 0;
			SendMessage(GetDlgItem(hWnd,LOWORD(wParam)),EM_SETSEL,0,-1);
			SetFocus(GetDlgItem(GetParent(hWnd),IDC_NCSETPEDIT));
			NCSet=2;
			break;
		case IDC_XY_PLANE:
			SendMessage(GetDlgItem(hWnd,IDC_XY_PLANE),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_TAPEP_ANGLE),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_H),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_A),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_B),EM_SETSEL,-1,0);
			pDataInNCSetT_T->hWnd = hWnd;
			pDataInNCSetT_T->wndCtrl = GetDlgItem(hWnd,LOWORD(wParam));
			pDataInNCSetT_T->menuID = LOWORD(wParam);
			NCSetID=IDC_XY_PLANE;
			pDataInNCSetT_T->notifyCode = 0;
			SendMessage(GetDlgItem(hWnd,LOWORD(wParam)),EM_SETSEL,0,-1);
			SetFocus(GetDlgItem(GetParent(hWnd),IDC_NCSETPEDIT));
			NCSet=2;
			break;
		case IDC_TAPEP_ANGLE:
			SendMessage(GetDlgItem(hWnd,IDC_XY_PLANE),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_TAPEP_ANGLE),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_H),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_A),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_B),EM_SETSEL,-1,0);
			pDataInNCSetT_T->hWnd = hWnd;
			pDataInNCSetT_T->wndCtrl = GetDlgItem(hWnd,LOWORD(wParam));
			NCSetID=IDC_TAPEP_ANGLE;
			pDataInNCSetT_T->menuID = LOWORD(wParam);
			pDataInNCSetT_T->notifyCode = 0;
			SendMessage(GetDlgItem(hWnd,LOWORD(wParam)),EM_SETSEL,0,-1);
			SetFocus(GetDlgItem(GetParent(hWnd),IDC_NCSETPEDIT));
			NCSet=2;
			break;
		case IDC_H:
			SendMessage(GetDlgItem(hWnd,IDC_XY_PLANE),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_TAPEP_ANGLE),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_H),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_A),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_B),EM_SETSEL,-1,0);
			pDataInNCSetT_T->hWnd = hWnd;
			pDataInNCSetT_T->wndCtrl = GetDlgItem(hWnd,LOWORD(wParam));
			pDataInNCSetT_T->menuID = LOWORD(wParam);
			NCSetID=IDC_H;
			pDataInNCSetT_T->notifyCode = 0;
			SendMessage(GetDlgItem(hWnd,LOWORD(wParam)),EM_SETSEL,0,-1);
			SetFocus(GetDlgItem(GetParent(hWnd),IDC_NCSETPEDIT));
			NCSet=2;
			break;
		case IDC_A:
			SendMessage(GetDlgItem(hWnd,IDC_XY_PLANE),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_TAPEP_ANGLE),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_H),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_A),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_B),EM_SETSEL,-1,0);
			pDataInNCSetT_T->hWnd = hWnd;
			pDataInNCSetT_T->wndCtrl = GetDlgItem(hWnd,LOWORD(wParam));
			pDataInNCSetT_T->menuID = LOWORD(wParam);
			pDataInNCSetT_T->notifyCode = 0;
			NCSetID=IDC_A;
			SendMessage(GetDlgItem(hWnd,LOWORD(wParam)),EM_SETSEL,0,-1);
			SetFocus(GetDlgItem(GetParent(hWnd),IDC_NCSETPEDIT));
			NCSet=2;
			break;
		case IDC_B:	
			SendMessage(GetDlgItem(hWnd,IDC_XY_PLANE),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_TAPEP_ANGLE),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_H),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_A),EM_SETSEL,-1,0);
			SendMessage(GetDlgItem(hWnd,IDC_B),EM_SETSEL,-1,0);
			pDataInNCSetT_T->hWnd = hWnd;
			pDataInNCSetT_T->wndCtrl = GetDlgItem(hWnd,LOWORD(wParam));
			pDataInNCSetT_T->menuID = LOWORD(wParam);
			NCSetID=IDC_B;
			pDataInNCSetT_T->notifyCode = 0;
			SendMessage(GetDlgItem(hWnd,LOWORD(wParam)),EM_SETSEL,0,-1);
			SetFocus(GetDlgItem(GetParent(hWnd),IDC_NCSETPEDIT));
			NCSet=2;
			break;
		default:
			break;
		}
	}
	return 0;
}

void NCSetT_TThreadProc()
{
    TCHAR szBuffer[_MAX_FNAME];
	double i;
    GetWindowText(GetDlgItem(GetParent(pDataInNCSetT_T->hWnd),IDC_NCSETPEDIT),szBuffer, _MAX_FNAME);
	i=atof(szBuffer);
	SetWindowText (GetDlgItem(GetParent(pDataInNCSetT_T->hWnd),IDC_NCSETPEDIT),"");
	if(i<0||i>999.999)
	{
		SetWindowText (GetDlgItem(GetParent(GetParent(pDataInNCSetT_T->hWnd)),IDC_STATICALARM),"ERROR! <0.000 OR >999.999");
	}
	SetWindowText (pDataInNCSetT_T->wndCtrl,szBuffer);
	SetFocus(pDataInNCSetT_T->wndCtrl);
	SendMessage(pDataInNCSetT_T->wndCtrl,EM_SETSEL,-1,0);
	switch(NCSetID)
	{
	case IDC_WIRE_BS:
		wire_bs=i;
		break;
	case IDC_Z_POSITION:
		z_position=i;
		break;
	case IDC_XY_PLANE:
		xy_plane=i;
		break;
	case IDC_TAPEP_ANGLE:
		tapep_angle=i;
		break;
	case IDC_H:
		h_thickness=i;
		break;
	case IDC_A:
		a_uv_plane=i;
		break;
	case IDC_B:	
		b_xy_plane=i;
		break;
	}
	NCSet=0;
}
LRESULT goPaintNCSetT_TWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//定义画板结构体对象
	PAINTSTRUCT ps;
	HBRUSH hOldBrush;
	RECT rect;
	BITMAP bmp;
	HDC hdc,hdcMem;
	GetClientRect(hWnd,&rect);
	SetRect(&rect,rect.left,50,400,rect.bottom);
	hdc=BeginPaint(hWnd,&ps);
	hdcMem=CreateCompatibleDC(hdc);
	hOldBrush=(HBRUSH)SelectObject(hdcMem,hBitmap_NCSet);
	GetObject(hBitmap_NCSet,sizeof(BITMAP),&bmp);
	BitBlt(hdc,rect.left,rect.top,bmp.bmWidth,bmp.bmHeight,hdcMem,0,0,SRCCOPY);
	SelectObject(hdcMem,hOldBrush);
	DeleteDC(hdcMem);
	SetBkMode(hdc,TRANSPARENT);
	SetTextColor(hdc,RGB(0,0,0));

	SetRect(&rect,450,20,754,50);
	DrawText(hdc,"DATA SPECIFIED ON OTHER SCREEN",-1,&rect,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,450,70,550,100);
	DrawText(hdc,"WIRE BS",-1,&rect,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,670,70,754,100);
	DrawText(hdc,"mm",-1,&rect,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,450,120,550,150);
	DrawText(hdc,"Z POSITION",-1,&rect,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,670,120,754,150);
	DrawText(hdc,"mm",-1,&rect,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,450,170,754,200);
	DrawText(hdc,"NC PROGRAM DATA",-1,&rect,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,450,220,550,250);
	DrawText(hdc,"XY PLANE",-1,&rect,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,670,220,754,250);
	DrawText(hdc,"mm",-1,&rect,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,450,270,550,300);
	DrawText(hdc,"TAPE ANGLE",-1,&rect,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,670,270,754,300);
	DrawText(hdc,"deg",-1,&rect,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,450,320,754,350);
	DrawText(hdc,"WORK DATA",-1,&rect,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,450,370,550,400);
	DrawText(hdc,"H:THINCNESS",-1,&rect,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,670,370,754,400);
	DrawText(hdc,"mm",-1,&rect,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,450,420,754,450);
	DrawText(hdc,"RESULT DATA",-1,&rect,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,450,470,550,500);
	DrawText(hdc,"A:UV PLANE",-1,&rect,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,670,470,754,500);
	DrawText(hdc,"mm",-1,&rect,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,450,520,550,550);
	DrawText(hdc,"B:XY PLANE",-1,&rect,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,670,520,754,550);
	DrawText(hdc,"mm",-1,&rect,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,450,570,754,600);
	DrawText(hdc,"UPPER:999.9999   LOWER:0.000",-1,&rect,DT_LEFT|DT_VCENTER|DT_SINGLELINE);

	EndPaint(hWnd,&ps);
	return 0;
}

LRESULT goDestroyNCSetT_TWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	PostQuitMessage(0);
	return 0;
}