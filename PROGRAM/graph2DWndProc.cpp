#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;								// 当前实例
extern GraphParam EditGraphParam;
extern int edit_regraph_flag;

const MessageProc graph2DMessages[]={
	   	WM_PAINT,                goPaintGraph2D,
		WM_DESTROY,              goDestroyGraph2D		
};

int MyRegisterClassGraph2D(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	INITCOMMONCONTROLSEX icex;
	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)graph2DWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//(LPCTSTR)ID_Menu;//(LPCTSTR)IDC_EX1;
	wcex.lpszClassName	= TEXT("GRAPHWND");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
//注册编辑窗口类
	if(!RegisterClassEx(&wcex)) return 0;
	icex.dwICC=ICC_LISTVIEW_CLASSES;
	icex.dwSize=sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&icex);
	return 1;
}

//编辑窗口过程体
LRESULT CALLBACK graph2DWndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;
	//查找对应的消息ID,并执行对应的消息处理过程
	for(i=0; i<dim(graph2DMessages);i++)
	{
		if(msgCode == graph2DMessages[i].uCode) return(*graph2DMessages[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//对不存在的消息，调用缺省窗口过程
	return DefWindowProc(hWnd, msgCode, wParam, lParam);
}
LRESULT goPaintGraph2D(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	hdc = BeginPaint(hWnd, &ps);
	if(edit_regraph_flag == 1)
	{						
		draw_all(EditGraphParam.hWnd,EditGraphParam.pDataNcGraphMem,EditGraphParam.draw_width,EditGraphParam.draw_length,
			EditGraphParam.mw, EditGraphParam.ml,EditGraphParam.create_num); //画全部图.
		draw_all_tape(EditGraphParam.hWnd,EditGraphParam.pDataNcGraphMem,EditGraphParam.draw_width,EditGraphParam.draw_length,
			EditGraphParam.mw, EditGraphParam.ml,EditGraphParam.create_num); //画tape全部图.
	}
	// TODO: 在此添加任意绘图代码...
	EndPaint(hWnd, &ps);
	return 0;
}

LRESULT goDestroyGraph2D(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	PostQuitMessage(0);
	return 0;
}
