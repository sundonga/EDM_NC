#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;								// ��ǰʵ��




const MessageProc coordviewMessages[]={
	    WM_CREATE,               goCreateCoordview,
	   	WM_PAINT,                goPaintCoordview,
		WM_CTLCOLORSTATIC,        goColorCoordview,
		WM_DESTROY,              goDestroyCoordview
		
};

CtlWndStruct coordviewMenuBtns[]={
		{0,TEXT("STATIC"),IDC_AXISX,TEXT("X"),10,50,20,50,WS_CHILD|WS_VISIBLE},
		{0,TEXT("STATIC"),IDC_AXISY,TEXT("Y"),10,100,20,50,WS_CHILD|WS_VISIBLE},
		{0,TEXT("STATIC"),IDC_AXISZ,TEXT("Z"),10,150,20,50,WS_CHILD|WS_VISIBLE},
		{0,TEXT("STATIC"),IDC_AXISB,TEXT("B"),10,200,20,50,WS_CHILD|WS_VISIBLE},
		{0,TEXT("STATIC"),IDC_AXISC,TEXT("C"),10,250,20,50,WS_CHILD|WS_VISIBLE},
		{0,TEXT("STATIC"),IDC_MACHCOORD,TEXT("MACHINE"),100,0,200,50,WS_CHILD|WS_VISIBLE},
		//{TEXT("STATIC"),IDC_RELATACOORD,TEXT("RELATIVE"),280,0,200,50,WS_CHILD|WS_VISIBLE},
		{0,TEXT("STATIC"),IDC_WORKCOORD,TEXT("WORK"),320,0,100,50,WS_CHILD|WS_VISIBLE},
		
};

int MyRegisterClassCoordview(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	INITCOMMONCONTROLSEX icex;
	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)coordviewWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	=(HBRUSH)GetStockObject(BLACK_BRUSH);//(HBRUSH)GetStockObject(BLACK_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//(LPCTSTR)ID_Menu;//(LPCTSTR)IDC_EX1;
	wcex.lpszClassName	= TEXT("STATUSVIEWWND");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
//ע��༭������
	if(!RegisterClassEx(&wcex)) return 0;
	icex.dwICC=ICC_LISTVIEW_CLASSES;//???????
	icex.dwSize=sizeof(INITCOMMONCONTROLSEX);//?????
	InitCommonControlsEx(&icex);//???
	return 1;
}

//�༭���ڹ�����
LRESULT CALLBACK coordviewWndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;

	//���Ҷ�Ӧ����ϢID,��ִ�ж�Ӧ����Ϣ�������
	for(i=0; i<dim(coordviewMessages);i++)
	{
		if(msgCode == coordviewMessages[i].uCode) return(*coordviewMessages[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//�Բ����ڵ���Ϣ������ȱʡ���ڹ���
	return DefWindowProc(hWnd, msgCode, wParam, lParam);
}


LRESULT goCreateCoordview(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
			
    int i;
	HFONT hFont;

	hFont=CreateFont(20, 0, 0, 0, 700, FALSE, FALSE, 0, 
                  GB2312_CHARSET, OUT_DEFAULT_PRECIS,
                  CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
                  DEFAULT_PITCH | FF_MODERN, "Arial" ); //��������
//������ʾ���괰�ڲ˵�
	for(i=0;i<dim(coordviewMenuBtns);i++){
		CreateWindowEx( coordviewMenuBtns[i].dwExStyle,
			coordviewMenuBtns[i].szClass,
			coordviewMenuBtns[i].szTitle,
			coordviewMenuBtns[i].lStyle,
			coordviewMenuBtns[i].x,
			coordviewMenuBtns[i].y,
			coordviewMenuBtns[i].cx,
			coordviewMenuBtns[i].cy,
			hWnd,
			(HMENU)coordviewMenuBtns[i].nID,
			hInst,
			NULL
			);

		SendMessage(GetDlgItem(hWnd,coordviewMenuBtns[i].nID),WM_SETFONT,(WPARAM)hFont,MAKELPARAM(TRUE,0));
	}
	
	//DeleteFont(hFont);

	


		return 0;
}

LRESULT goColorCoordview(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	SetTextColor((HDC)wParam,RGB(255,255,255));
	SetBkColor((HDC)wParam,RGB(0,0,0));
	return (LRESULT)CreateSolidBrush(RGB(0,0,0));
}

LRESULT goPaintCoordview(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		PAINTSTRUCT ps;
		HDC hdc;

		hdc = BeginPaint(hWnd, &ps);

		
		// TODO: �ڴ���������ͼ����...
		EndPaint(hWnd, &ps);
		return 0;
}

LRESULT goDestroyCoordview(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		PostQuitMessage(0);
		return 0;
}
