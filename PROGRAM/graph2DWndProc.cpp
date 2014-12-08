#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;								// ��ǰʵ��
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
//ע��༭������
	if(!RegisterClassEx(&wcex)) return 0;
	icex.dwICC=ICC_LISTVIEW_CLASSES;
	icex.dwSize=sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&icex);
	return 1;
}

//�༭���ڹ�����
LRESULT CALLBACK graph2DWndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;
	//���Ҷ�Ӧ����ϢID,��ִ�ж�Ӧ����Ϣ�������
	for(i=0; i<dim(graph2DMessages);i++)
	{
		if(msgCode == graph2DMessages[i].uCode) return(*graph2DMessages[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//�Բ����ڵ���Ϣ������ȱʡ���ڹ���
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
			EditGraphParam.mw, EditGraphParam.ml,EditGraphParam.create_num); //��ȫ��ͼ.
		draw_all_tape(EditGraphParam.hWnd,EditGraphParam.pDataNcGraphMem,EditGraphParam.draw_width,EditGraphParam.draw_length,
			EditGraphParam.mw, EditGraphParam.ml,EditGraphParam.create_num); //��tapeȫ��ͼ.
	}
	// TODO: �ڴ���������ͼ����...
	EndPaint(hWnd, &ps);
	return 0;
}

LRESULT goDestroyGraph2D(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	PostQuitMessage(0);
	return 0;
}
