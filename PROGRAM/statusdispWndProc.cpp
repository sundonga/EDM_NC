#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HANDLE IOPCIhDevice;               // IO_PCI设备句柄 
extern HANDLE NCPCIhDevice;                // NC_PCI设备句柄

extern HINSTANCE hInst;								// 当前实例

extern HANDLE AutohThread;
extern HANDLE ManualhThread;
extern HANDLE MdihThread;
extern HANDLE NcSendhThread;
extern HANDLE hMachiningEndEvent;
extern HANDLE hDecodeEvent;
extern int Manual_keydown_notif_proc(HWND hWnd,int adr_axisvel, int adr_axispul, int orientation);	//orientation为0正转, 1反转
extern int Autoreset_flag;
extern HANDLE hM_Code_EventExecutive;
extern HANDLE hM_Code_Event;
extern HANDLE M_Code_hThread;
extern void caculateTapPoint(double&,double&,double&,double&);
extern int G5xFlag;
extern Coordata G54_coordinate,G55_coordinate,G56_coordinate,G57_coordinate,G58_coordinate,G59_coordinate;
extern void readbuffer_to_fc(HWND hWnd);
extern void readContentFc(FILE* fp,char* a,int line,HWND hWnd);
extern void fc_upday(HWND hWnd);
extern void readfc_to_buffer(HWND hWnd);
extern HWND hWndCoor;
extern int M_Code_hTread_Flag;

extern DWORD Stop_single_touch;
extern DWORD Control_single_touch;
HWND hWndstatus;

//2010-6-24
bool isStatusdispWndProc;
LPCmdThreadParam  pDataInStatus;


LRESULT goKeydownCoor(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam);

void CoorThreadProc(void);
HANDLE CoorThread;

int Maching_status = 0;
HBITMAP hBitmap_COOR;

Coordata data_m;  //Machine coor
Coordata data_w;   //work coor
Coordata data_r;   //ralitive coor
Coorpuls pulse_m; //Machine pulse
Coorpuls pulse_w; //work pulse

Coordata data_coorswitch;

const MessageProc statusMessages[]={
	    WM_CREATE,               goCreateStatus,
		WM_TIMER,                goTimerStatus,
		WM_COMMAND,              goCommandStatus,
		WM_PAINT,                goPaintStatus,
		WM_DRAWITEM,             goDrawitemMain,
		WM_DESTROY,              goDestroyStatus,
		WM_KEYDOWN,              goKeydownCoor		
};
int SetStaticsText(HWND hWnd);//????
const CommandProc statusCommands[]={
	    IDC_AXISALLZERO,               goStatusAxisAllzeroCmd,
		IDC_AXISXZERO,				   goStatusAxisSinglezeroCmd,
		IDC_AXISYZERO,				   goStatusAxisSinglezeroCmd,
		IDC_AXISZZERO,				   goStatusAxisSinglezeroCmd,
		IDC_AXISBZERO,				   goStatusAxisSinglezeroCmd,
		IDC_AXISCZERO,				   goStatusAxisSinglezeroCmd,
		IDC_BTNSTARTSTOP,              goStatusStartStopCmd,
		IDC_BTNRESET,                  goStatusResetCmd,
		IDC_BTNSUSPEND,				   goStatusSuspendStopCmd		
};
CtlWndStruct statusMenuBtns[]={
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_AXISALLZERO,TEXT("ALL"),960,273,60,45,WS_CHILD|WS_VISIBLE|WS_BORDER|BS_OWNERDRAW},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_AXISXZERO,TEXT("XZERO"),960,48,60,45,WS_CHILD|WS_VISIBLE|WS_BORDER|BS_OWNERDRAW},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_AXISYZERO,TEXT("YZERO"),960,93,60,45,WS_CHILD|WS_VISIBLE|WS_BORDER|BS_OWNERDRAW},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_AXISZZERO,TEXT("ZZERO"),960,138,60,45,WS_CHILD|WS_VISIBLE|WS_BORDER|BS_OWNERDRAW},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_AXISBZERO,TEXT("BZERO"),960,183,60,45,WS_CHILD|WS_VISIBLE|WS_BORDER|BS_OWNERDRAW},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_AXISCZERO,TEXT("CZERO"),960,228,60,45,WS_CHILD|WS_VISIBLE|WS_BORDER|BS_OWNERDRAW},
		{0,TEXT("STATIC"),IDC_OPENNAME,TEXT("OPEN"),670,50,50,20,WS_CHILD|WS_VISIBLE},
		{0,TEXT("STATIC"),IDC_DISCHARGENNAME,TEXT("SPARK"),670,90,50,20,WS_CHILD|WS_VISIBLE},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNSTARTSTOP,TEXT("START"),0,228,100,45,WS_CHILD|WS_VISIBLE|WS_BORDER|BS_OWNERDRAW},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNRESET,TEXT("RESET"),100,228,100,45,WS_CHILD|WS_VISIBLE|WS_BORDER|BS_OWNERDRAW},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNSUSPEND,TEXT("SUSPEND"),0,273,200,45,WS_CHILD|WS_VISIBLE|WS_BORDER|BS_OWNERDRAW},
		{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_G5xFlagEDIT,TEXT(""),205,50,40,20,WS_VISIBLE|WS_BORDER|WS_CHILD|ES_UPPERCASE},
};//最右一列的内容；具有细线边界的，不具有菜单的子窗口，BS_PUSHBUTTON，每个方块是45的距离


//子窗口菜单列表
CtlWndStruct statusChildWnds[]={
	{WS_EX_CLIENTEDGE,TEXT("STATUSVIEWWND"),IDC_STATUSVIEW,TEXT(""),250,0,416,320,WS_CHILD|SS_NOTIFY|WS_BORDER}	
};//这个没找到哈？？？？
int MyRegisterClassStatus(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	INITCOMMONCONTROLSEX icex;
	
	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)statusWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);//(HBRUSH)GetStockObject(BLACK_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//(LPCTSTR)ID_Menu;//(LPCTSTR)IDC_EX1;
	wcex.lpszClassName	= TEXT("STATUSWND");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
//注册编辑窗口类
	if(!RegisterClassEx(&wcex)) return 0;
	if(!MyRegisterClassCoordview( hInst)) return 0;
	icex.dwICC=ICC_LISTVIEW_CLASSES;
	icex.dwSize=sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&icex);
	return 1;
}//这个不知道是哪个窗口啊？？？？

//编辑窗口过程体
LRESULT CALLBACK statusWndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;

	//查找对应的消息ID,并执行对应的消息处理过程
	for(i=0; i<dim(statusMessages);i++)
	{
		if(msgCode == statusMessages[i].uCode) return(*statusMessages[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//对不存在的消息，调用缺省窗口过程
	return DefWindowProc(hWnd, msgCode, wParam, lParam);
}

LRESULT goCreateStatus(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	
    int i;
	char G5xFlag_string[4];
	
	
//创建Status窗口菜单
	
	for(i=0;i<dim(statusMenuBtns);i++){
		CreateWindowEx(statusMenuBtns[i].dwExStyle,
			statusMenuBtns[i].szClass,
			statusMenuBtns[i].szTitle,
			statusMenuBtns[i].lStyle,
			statusMenuBtns[i].x,
			statusMenuBtns[i].y,
			statusMenuBtns[i].cx,
			statusMenuBtns[i].cy,
			hWnd,
			(HMENU)statusMenuBtns[i].nID,
			hInst,
			NULL
			);
	}
	

	CreateWindowEx(WS_EX_CLIENTEDGE, 
		PROGRESS_CLASS,
		"OPEN STATE",
		WS_CHILD| WS_VISIBLE|WS_BORDER, 
		720,50,220,20,
		hWnd,
		(HMENU)IDC_PROGRESSBAR1,
		hInst,
		NULL
		);
	CreateWindowEx(WS_EX_CLIENTEDGE, 
		PROGRESS_CLASS,
		"DISCHARGE STATE",
		WS_CHILD| WS_VISIBLE|WS_BORDER, 
		720,90,220,20,
		hWnd,
		(HMENU)IDC_PROGRESSBAR2,
		hInst,
		NULL
		); 
	SendMessage(GetDlgItem(hWnd,IDC_PROGRESSBAR1),(UINT)PBM_SETBARCOLOR, 0, (LPARAM) RGB(0,0,255)); 
	SendMessage(GetDlgItem(hWnd,IDC_PROGRESSBAR2),(UINT)PBM_SETBARCOLOR, 0, (LPARAM) RGB(255,0,0));  
                        
	SendMessage(GetDlgItem(hWnd,IDC_PROGRESSBAR1), PBM_SETRANGE, 0, MAKELPARAM(0,10000));
	SendMessage(GetDlgItem(hWnd,IDC_PROGRESSBAR2), PBM_SETRANGE, 0, MAKELPARAM(0,10000)); 

	SendMessage(GetDlgItem(hWnd,IDC_PROGRESSBAR1), PBM_SETPOS, (WPARAM)5000, 0); 
	SendMessage(GetDlgItem(hWnd,IDC_PROGRESSBAR2), PBM_SETPOS, (WPARAM)5000, 0); 

	
//创建编辑窗口的子窗口
	
	for(i=0;i<dim(statusChildWnds);i++){
		CreateWindowEx(statusChildWnds[i].dwExStyle,
			statusChildWnds[i].szClass,
			statusChildWnds[i].szTitle,
			statusChildWnds[i].lStyle,
			statusChildWnds[i].x,
			statusChildWnds[i].y,
			statusChildWnds[i].cx,
			statusChildWnds[i].cy,
			hWnd,
			(HMENU)statusChildWnds[i].nID,
			hInst,
			NULL
			);
	}
	//显示其中一个窗口
	ShowWindow(GetDlgItem(hWnd,statusChildWnds[0].nID),SW_SHOW);
	itoa(G5xFlag,G5xFlag_string,10);
	SetWindowText(GetDlgItem(hWnd,IDC_G5xFlagEDIT),G5xFlag_string);
	hWndstatus = GetDlgItem(hWnd,IDC_G5xFlagEDIT);
	//坐标显示时间设定
	SetTimer(hWnd,TIMER_STATUS,200,NULL);

	//2010-6-24
	isStatusdispWndProc=FALSE;
	pDataInStatus = (LPCmdThreadParam)HeapAlloc(GetProcessHeap(),
											HEAP_ZERO_MEMORY,
											sizeof(CmdThreadParam)
											);
	if(pDataInStatus == NULL)
	{
		return 1;
	}
	//end 2010-6-24	
	return 0;

}

LRESULT goCommandStatus(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//定义菜单ID号和通知单
	int menuID, notifyCode;
	
	//定义窗口句柄
	HWND wndCtrl;
	int i;
	menuID    = LOWORD(wParam); 
	notifyCode = HIWORD(wParam); 
	wndCtrl = (HWND) lParam;
		 
	//查找对应的控件ID,并执行对应的消息处理过程
	for(i=0; i<dim(statusCommands);i++)
	{
		if(menuID == statusCommands[i].uCode) return(*statusCommands[i].functionName)(hWnd,wndCtrl,menuID,notifyCode);
	}
	if(HIWORD(wParam)==EN_SETFOCUS)
	{
		pDataInStatus->hWnd = hWnd;
		pDataInStatus->wndCtrl = GetDlgItem(hWnd,LOWORD(wParam));
		pDataInStatus->notifyCode = 0;
		isStatusdispWndProc=TRUE;
	}
	return 0;
}

void CoorThreadProc()
{
	char szBuffer[31];
	GetWindowText (GetDlgItem(pDataInStatus->hWnd,IDC_G5xFlagEDIT),szBuffer,30);//得到输入文本框内所输入数值
	G5xFlag=atoi(szBuffer);
	SetWindowText (pDataInStatus->wndCtrl,szBuffer);
	isStatusdispWndProc=FALSE;
}
LRESULT goKeydownCoor(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int menuID, notifyCode;		
	//定义窗口句柄
	HWND wndCtrl;
	menuID    = LOWORD(wParam); 
	notifyCode = HIWORD(wParam); 
	wndCtrl = (HWND) lParam;
	if(isStatusdispWndProc)CoorThreadProc();
	return 0;
}
LRESULT goTimerStatus(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	

	if(wParam==TIMER_STATUS)
	{
		 SetStaticsText(hWnd);
	}
	return 0;
}

LRESULT goPaintStatus(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HBRUSH hOldBrush;
	RECT rect;
	BITMAP bmp;
	HDC hdc,hdcMem;
	GetClientRect(hWnd,&rect);
	SetRect(&rect,rect.left,50,400,rect.bottom);
	hdc=BeginPaint(hWnd,&ps);
	hdcMem=CreateCompatibleDC(hdc);
	hOldBrush=(HBRUSH)SelectObject(hdcMem,hBitmap_COOR);
	GetObject(hBitmap_COOR,sizeof(BITMAP),&bmp);
	BitBlt(hdc,rect.left,rect.top,bmp.bmWidth,bmp.bmHeight,hdcMem,0,0,SRCCOPY);
	SelectObject(hdcMem,hOldBrush);
	DeleteDC(hdcMem);
	SetBkMode(hdc,TRANSPARENT);
	SetTextColor(hdc,RGB(255,0,0));

	SetRect(&rect,150,50,230,70);
	DrawText(hdc,"Coor_G",-1,&rect,DT_LEFT|DT_VCENTER|DT_SINGLELINE);

		
	// TODO: 在此添加任意绘图代码...
	EndPaint(hWnd, &ps);
	return 0;
}

LRESULT goDestroyStatus(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		PostQuitMessage(0);
		KillTimer(hWnd,TIMER_STATUS); 
		return 0;
}

LRESULT goStatusAxisAllzeroCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{

	data_coorswitch = data_w;
	fc_upday(hWnd);
	readfc_to_buffer(hWnd);

	switch(G5xFlag)
	{
		case 54:
			G54_coordinate.x = G54_coordinate.x - data_coorswitch.x ;
			G54_coordinate.y = G54_coordinate.y - data_coorswitch.y ;
			G54_coordinate.z = G54_coordinate.z - data_coorswitch.z ;
			G54_coordinate.b = G54_coordinate.b - data_coorswitch.b ;
			G54_coordinate.c = G54_coordinate.c - data_coorswitch.c ;
			readbuffer_to_fc(hWnd);

			break;
		case 55:
			G55_coordinate.x = G55_coordinate.x  - data_coorswitch.x ;
			G55_coordinate.y = G55_coordinate.y - data_coorswitch.y ;
			G55_coordinate.z = G55_coordinate.z - data_coorswitch.z ;
			G55_coordinate.b = G55_coordinate.b - data_coorswitch.b ;
			G55_coordinate.c = G55_coordinate.c - data_coorswitch.c ;
			readbuffer_to_fc(hWnd);
			break;
		case 56:
			G56_coordinate.x  = G56_coordinate.x  - data_coorswitch.x ;
			G56_coordinate.y = G56_coordinate.y - data_coorswitch.y ;
			G56_coordinate.z = G56_coordinate.z - data_coorswitch.z ;
			G56_coordinate.b = G56_coordinate.b - data_coorswitch.b ;
			G56_coordinate.c = G56_coordinate.c - data_coorswitch.c ;
			readbuffer_to_fc(hWnd);
			break;
		case 57:
			G57_coordinate.x  = G57_coordinate.x  - data_coorswitch.x ;
			G57_coordinate.y = G57_coordinate.y - data_coorswitch.y ;
			G57_coordinate.z = G57_coordinate.z - data_coorswitch.z ;
			G57_coordinate.b = G57_coordinate.b - data_coorswitch.b ;
			G57_coordinate.c = G57_coordinate.c - data_coorswitch.c ;
			readbuffer_to_fc(hWnd);
			break;
		case 58:
			G58_coordinate.x  = G58_coordinate.x  - data_coorswitch.x ;
			G58_coordinate.y = G58_coordinate.y - data_coorswitch.y ;
			G58_coordinate.z = G58_coordinate.z - data_coorswitch.z ;
			G58_coordinate.b = G58_coordinate.b - data_coorswitch.b ;
			G58_coordinate.c = G58_coordinate.c - data_coorswitch.c ;
			readbuffer_to_fc(hWnd);
			break;
		case 59:
			G59_coordinate.x  = G59_coordinate.x  - data_coorswitch.x ;
			G59_coordinate.y = G59_coordinate.y - data_coorswitch.y ;
			G59_coordinate.z = G59_coordinate.z - data_coorswitch.z ;
			G59_coordinate.b = G59_coordinate.b - data_coorswitch.b ;
			G59_coordinate.c = G59_coordinate.c - data_coorswitch.c ;
			readbuffer_to_fc(hWnd);
			break;

	}

	fc_upday(hWndCoor);
	data_w.x = 0.;//此处设定为工件坐标系的X原点;
	data_r.x = data_m.x;//机器坐标 - 工件坐标 即为相对坐标;
		
	data_w.y = 0.;//此处设定为工件坐标系的X原点;
	data_r.y = data_m.y;//机器坐标 - 工件坐标 即为相对坐标;
		
	data_w.z = 0.;//此处设定为工件坐标系的X原点;
	data_r.z = data_m.z;//机器坐标 - 工件坐标 即为相对坐标;
		
	data_w.b = 0.;//此处设定为工件坐标系的X原点;
	data_r.b = data_m.b;//机器坐标 - 工件坐标 即为相对坐标;
		
	data_w.c = 0.;//此处设定为工件坐标系的X原点;
	data_r.c = data_m.c;//机器坐标 - 工件坐标 即为相对坐标;
		
	return 0;
}
LRESULT goStatusAxisSinglezeroCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	switch(menuID)
	{
		case IDC_AXISXZERO:
			data_w.x = 0.;//此处设定为工件坐标系的X原点;
			data_r.x = data_m.x;//机器坐标 - 工件坐标 即为相对坐标;
			break;
		case IDC_AXISYZERO:
			data_w.y = 0.;//此处设定为工件坐标系的X原点;
			data_r.y = data_m.y;//机器坐标 - 工件坐标 即为相对坐标;
			break;
		case IDC_AXISZZERO:
			data_w.z = 0.;//此处设定为工件坐标系的X原点;
			data_r.z = data_m.z;//机器坐标 - 工件坐标 即为相对坐标;
			break;
		case IDC_AXISBZERO:
			data_w.b = 0.;//此处设定为工件坐标系的X原点;
			data_r.b = data_m.b;//机器坐标 - 工件坐标 即为相对坐标;
			break;
		case IDC_AXISCZERO:
			data_w.c = 0.;//此处设定为工件坐标系的X原点;
			data_r.c = data_m.c;//机器坐标 - 工件坐标 即为相对坐标;
			break;
		default: break;
	}


	return 0;
}

LRESULT goStatusStartStopCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{

	if(Maching_status == 0)
	{
		ResumeThread(ManualhThread);
		ResumeThread(AutohThread);
		ResumeThread(MdihThread);
			
	}
	else
	{
		iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE, ADR_CONT_Star_Stop,  0x01);
		Maching_status = 0;
	}
return 0;
}
LRESULT goStatusSuspendStopCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	Maching_status = 1;
	iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE, ADR_CONT_Star_Stop,  0x0);   //让fpga停止
	//Manual_keydown_notif_proc(hWnd,0x70, 0x70, 0);             /////地址不是五轴中的一根就可以，选定0x70这个没有用到的
return 0;
}
LRESULT goStatusResetCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	
	
	CloseHandle(ManualhThread);
	Autoreset_flag = 1;
	SetEvent(hDecodeEvent);
	SetEvent(hMachiningEndEvent);
	SetEvent(hM_Code_Event);
	M_Code_hTread_Flag = 1;
	Stop_single_touch = 0;

	

	//CloseHandle(AutohThread);
	//CloseHandle(NcSendhThread);
	//CloseHandle(MdihThread);
	
	return 0;
}

int SetStaticsText(HWND hWnd)
{
	unsigned int XYZBC_Zero=0x80000000;//用于正负判断    
	HDC hdc;   
	int i;	
	HFONT hFont;
	static char coord[12][15]={"","","","","","","","","","","",""};  

	hdc=GetDC(GetDlgItem(hWnd,IDC_STATUSVIEW));
	hFont=CreateFont(20, 0, 0, 0, 900, FALSE, FALSE, 0, 
				GB2312_CHARSET, OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
				DEFAULT_PITCH | FF_MODERN, "Arial" );
	
	SelectObject(hdc,hFont);
	SetTextColor(hdc,RGB(255,255,255));
	SetBkColor(hdc,RGB(0,0,0));	    
	//从NC板的FPGA读机器坐标	
	pulse_m.x = (ioread(hWnd,NCPCIhDevice,NC_FPGA_READ,ADR_XPOS_L)&0X0FFFF) + ((ioread(hWnd,NCPCIhDevice,NC_FPGA_READ,ADR_XPOS_H)&0XFFFF)<<16);

	pulse_m.y = (ioread(hWnd,NCPCIhDevice,NC_FPGA_READ,ADR_YPOS_L)&0X0FFFF) + ((ioread(hWnd,NCPCIhDevice,NC_FPGA_READ,ADR_YPOS_H)&0XFFFF)<<16);

	pulse_m.z = (ioread(hWnd,NCPCIhDevice,NC_FPGA_READ,ADR_ZPOS_L)&0X0FFFF) + ((ioread(hWnd,NCPCIhDevice,NC_FPGA_READ,ADR_ZPOS_H)&0XFFFF)<<16);

	pulse_m.b = (ioread(hWnd,NCPCIhDevice,NC_FPGA_READ,ADR_BPOS_L)&0X0FFFF) + ((ioread(hWnd,NCPCIhDevice,NC_FPGA_READ,ADR_BPOS_H)&0XFFFF)<<16);

	pulse_m.c = (ioread(hWnd,NCPCIhDevice,NC_FPGA_READ,ADR_CPOS_L)&0X0FFFF) + ((ioread(hWnd,NCPCIhDevice,NC_FPGA_READ,ADR_CPOS_H)&0XFFFF)<<16);
	
	//脉冲转化为毫米，求取工件坐标  test
	if(pulse_m.x >=XYZBC_Zero)//判断data_m.x最高位为1,是负数;
	data_m.x=-(double)(pulse_m.x-XYZBC_Zero)/2000;//将脉冲转换为毫米;可能对应内核程序;
	else 
	data_m.x=(double)pulse_m.x/2000;           //仿真将1000改为2000
	data_w.x = data_m.x - data_r.x;
    
	if(pulse_m.y >=XYZBC_Zero)
	data_m.y=-(double)(pulse_m.y-XYZBC_Zero)/2000; //仿真将1000改为2000
	else 
	data_m.y = (double)pulse_m.y/2000;
	data_w.y = data_m.y - data_r.y;
    
	if(pulse_m.z >=XYZBC_Zero)
	data_m.z=-(double)(pulse_m.z-XYZBC_Zero)/2000; //仿真将1000改为2000
	else 
	data_m.z=(double)pulse_m.z/2000;
	data_w.z = data_m.z - data_r.z;
    
	if(pulse_m.b >=XYZBC_Zero)
	data_m.b=-(double)(pulse_m.b-XYZBC_Zero)/2000;//仿真将5000改为2000
	else 
	data_m.b=(double)pulse_m.b/2000;
	data_w.b = data_m.b -data_r.b;


	if(pulse_m.c >=XYZBC_Zero)
	data_m.c=-(double)(pulse_m.c-XYZBC_Zero)/2000;//***************见Returnzero.c中94行; //仿真将1000改为2000
	else                             //c轴转一圈产生1800组信号,4倍频后为7200个脉冲信号,360/50 度,所以7200*50/360=1000;
//			data_m.c=(double)pulse_m.c/4000;
	data_m.c=(double)pulse_m.c/2000;
	data_w.c = data_m.c - data_r.c;
    

    
	sprintf_s( coord[0], "%12.3f", data_w.x );//将浮点形转化为字符串;
	sprintf_s( coord[1], "%12.3f", data_w.y );
	sprintf_s( coord[2], "%12.3f", data_w.z );
	sprintf_s( coord[3], "%12.3f", data_w.b );
	sprintf_s( coord[4], "%12.3f", data_w.c );
	sprintf_s( coord[5], "%12.3f", data_m.x );
	sprintf_s( coord[6], "%12.3f", data_m.y );
	sprintf_s( coord[7], "%12.3f", data_m.z );
	sprintf_s( coord[8], "%12.3f", data_m.b );
	sprintf_s( coord[9], "%12.3f", data_m.c );


	for(i=0;i<5;i++)
	{		        
		TextOut(hdc,260,50+50*i,coord[i],15);
	}
	for(i=0;i<5;i++)
	{
		TextOut(hdc,50,50+50*i,coord[i+5],15);
	}
 
	DeleteObject(hFont);
	ReleaseDC(GetDlgItem(hWnd,IDC_STATUSVIEW),hdc);	
	return 0;
}