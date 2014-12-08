#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;								// 当前实例
extern int auto_nccode_flag;
extern LPNCCODE lpNcCodeMem;
extern int NcCodeNum; //NC码数
extern HANDLE hMachiningEndEvent;
extern HANDLE NCPCIhDevice;                // NC_PCI设备句柄 
extern nc_data JudgeData[2*DECODE_NUM_ONCE];
int AutonccodeFirstEnterFlag;  //nc码显示函数第一次进入标志
int row_segment;
int row;
int Para_End_Flag = 0;
extern int G5xFlag;
extern HWND hWndstatus;

const MessageProc autoNccodeMessages[]={
	    WM_CREATE, goCreateAutoNccode,
		WM_TIMER,  goTimerAutoNccode,
		WM_PAINT,  goPaintAutoNccode,
		WM_DRAWITEM, goDrawitemMain,
		WM_DESTROY, goDestroyAutoNccode
};//定义全局常量消息ID号与消息处理过程指针查找表

CtlWndStruct autoNccodeMenuBtns[]={
		{WS_EX_CLIENTEDGE,TEXT("LISTBOX"),IDC_AUTONCCODELIST,TEXT(""),0,0,412,350,WS_VISIBLE|WS_BORDER|WS_CHILD|LBS_STANDARD }
			
};//？？？？？

int MyRegisterClassAutoNccode(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	INITCOMMONCONTROLSEX icex;//?????
	wcex.cbSize = sizeof(WNDCLASSEX); //?????

	wcex.style			= CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)autoNccodeWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//(LPCTSTR)IDC_EX1;
	wcex.lpszClassName	= TEXT("AUTONCCODEWND");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);//？？？？
//注册自动窗口类
	if(!RegisterClassEx(&wcex)) return 0;
	icex.dwICC=ICC_LISTVIEW_CLASSES;//???????
	icex.dwSize=sizeof(INITCOMMONCONTROLSEX);//?????
	InitCommonControlsEx(&icex);//?????

	return 1;
}

//自动加工参数窗口过程体
LRESULT CALLBACK autoNccodeWndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;

	//查找对应的消息ID,并执行对应的消息处理过程
	for(i=0; i<dim(autoNccodeMessages);i++)
	{
		if(msgCode == autoNccodeMessages[i].uCode) return(*autoNccodeMessages[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//对不存在的消息，调用缺省窗口过程
	return DefWindowProc(hWnd, msgCode, wParam, lParam);
}

LRESULT goCreateAutoNccode(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{  	
    int i;
	SetTimer(hWnd,TIMER_AUTONCCODE,1000,NULL);
   //创建nc代码显示窗口
	for(i=0;i<dim(autoNccodeMenuBtns);i++)
	{
		CreateWindowEx(autoNccodeMenuBtns[i].dwExStyle, 
			autoNccodeMenuBtns[i].szClass,
			autoNccodeMenuBtns[i].szTitle,
			autoNccodeMenuBtns[i].lStyle,
			autoNccodeMenuBtns[i].x,
			autoNccodeMenuBtns[i].y,
			autoNccodeMenuBtns[i].cx,
			autoNccodeMenuBtns[i].cy,
			hWnd,
			(HMENU)autoNccodeMenuBtns[i].nID,
			hInst,
			NULL
			);
	}
	return 0;
}//????????


LRESULT goTimerAutoNccode(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	
	if(wParam==TIMER_AUTONCCODE)
	{
		if(auto_nccode_flag == 1)
		{
			 DispNcCode(hWnd);
		}
	}
	return 0;
}//?????



LRESULT goPaintAutoNccode(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(hWnd, &ps);
	// TODO: 在此添加任意绘图代码...
	EndPaint(hWnd, &ps);
	return 0;
}

LRESULT goDestroyAutoNccode(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	PostQuitMessage(0);
	KillTimer(hWnd,TIMER_AUTONCCODE);//不需要计时器时？？？？
	return 0;
}

LRESULT DispNcCode(HWND hWnd)
 {
	 int i;	
	 int j = 0;
	char G5xFlag_string[20];


	
	Para_End_Flag = ioread(hWnd,NCPCIhDevice,DSP_PARAMETER_READ,DSP_PARAMETER_Machining_ParaEnd_Flag_ADR ) & 0x0ffff;        
	row = ioread(hWnd,NCPCIhDevice,DSP_PARAMETER_READ,DSP_PARAMETER_Machining_Row_ADR ) & 0x0ffff;

	while(j<row+1)
	{
		if( JudgeData[j].row_id_All == row) 
		{
			G5xFlag = JudgeData[j].G_coormode_5x;
			if(G5xFlag == 0) G5xFlag = 54;
			itoa(G5xFlag,G5xFlag_string,10);
			SetWindowText(hWndstatus,G5xFlag_string);
			break;
		}
		j++;
	}


	if(AutonccodeFirstEnterFlag==0) row_segment = 1;   
	if(row>=NcCodeNum)
	{
		SetEvent(hMachiningEndEvent);
		return 1;
	}	
	if(row_segment != row/17)
	{
		row_segment = row/17;
		SendMessage(GetDlgItem(hWnd,IDC_AUTONCCODELIST),LB_RESETCONTENT,0,0);
	   	for(i=0;i<17;i++)
		{				
			SendMessage(GetDlgItem(hWnd,IDC_AUTONCCODELIST),LB_INSERTSTRING,i,(LPARAM)(lpNcCodeMem+17*row_segment+i)->pText);  //在NC码区显示nc码							
		}
	}
	SendMessage(GetDlgItem(hWnd,IDC_AUTONCCODELIST),LB_SETCURSEL,row%17,0);	
	AutonccodeFirstEnterFlag = 1;		
	return 0;		
 }