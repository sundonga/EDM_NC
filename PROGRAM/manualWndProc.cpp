#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;								// 当前实例
HANDLE ManualhThread;


extern HANDLE IOPCIhDevice;               // IO_PCI设备句柄 
extern HANDLE NCPCIhDevice;                // NC_PCI设备句柄 


extern Coordata data_m;  //Machine coor
extern Coordata data_w;   //work coor
extern Coorpuls pulse_m; //Machine pulse
extern Coorpuls pulse_w; //work pulse

int ManualSpeed = SPEED_LOW;  //手动运行速度
BOOL ddSorM;

DWORD WINAPI ManualMachAllReturnlCmdThreadProc(LPVOID lpParam);
DWORD WINAPI ManualMachSingleReturnCmdThreadProc(LPVOID lpParam);
DWORD WINAPI ManualWorkAllReturnlCmdThreadProc(LPVOID lpParam);
DWORD WINAPI ManualWorkSingleReturnCmdThreadProc(LPVOID lpParam);
DWORD WINAPI ManualSingleTouchCmdThreadProc(LPVOID lpParam);
DWORD WINAPI ChangeRatioCmdThreadProc(LPVOID lpParam);

int CheckSpeedRadioBtn(HWND hwnd);
int  ManualSpeedProc (HWND hWnd);
int ManualMoveAxisProc(HWND hWnd);
int Manual_keydown_notif_proc(HWND hWnd,int adr_axisvel, int adr_axispul, int orientation);	//orientation为0正转, 1反转
int XPosClr(HWND hWnd);
int YPosClr(HWND hWnd);
int ZPosClr(HWND hWnd);
int BPosClr(HWND hWnd);
int CPosClr(HWND hWnd);
extern int MachinePosPlusWrite(HWND hWnd, double data_mx,double data_my,double data_mz,double data_mb,double data_mc);

HBITMAP graph_X_up;
HBITMAP graph_X_down;
DWORD Stop_single_touch = 1;
DWORD Control_single_touch = 0;


const MessageProc manualMessages[]={
	    WM_CREATE, goCreateManual,
		WM_COMMAND, goCommandManual,
		WM_TIMER,   goTimerManual,
		WM_PAINT,  goPaintManual,
		WM_DRAWITEM, goDrawitemMain,
		WM_DESTROY, goDestroyManual
};


const CommandProc manualCommands[]={
	IDC_MachAllReturn,     goManualMachAllReturnlCmd,
	IDC_MachXReturn,       goManualMachSingleReturnCmd,
	IDC_MachYReturn,       goManualMachSingleReturnCmd,
	IDC_MachZReturn,       goManualMachSingleReturnCmd,
	IDC_MachBReturn,       goManualMachSingleReturnCmd,
	IDC_MachCReturn,       goManualMachSingleReturnCmd,

	IDC_WorkAllReturn,     goManualWorkAllReturnlCmd,
	IDC_WorkXReturn,       goManualWorkSingleReturnCmd,
	IDC_WorkYReturn,       goManualWorkSingleReturnCmd,
	IDC_WorkZReturn,       goManualWorkSingleReturnCmd,   
	IDC_WorkBReturn,       goManualWorkSingleReturnCmd,
	IDC_WorkCReturn,       goManualWorkSingleReturnCmd,

	IDC_XTouchP,           goManualSingleTouchCmd,
	IDC_YTouchP,           goManualSingleTouchCmd,
	IDC_ZTouchP,           goManualSingleTouchCmd,
	IDC_BTouchP,           goManualSingleTouchCmd,
	IDC_CTouchP,           goManualSingleTouchCmd,

	IDC_XTouchM,           goManualSingleTouchCmd,
	IDC_YTouchM,           goManualSingleTouchCmd,
	IDC_ZTouchM,           goManualSingleTouchCmd,
	IDC_BTouchM,           goManualSingleTouchCmd,
	IDC_CTouchM,           goManualSingleTouchCmd,

	IDC_RADIOBTNRAPID,		goChangeRatioCmd,
	IDC_RADIOBTNHIGH,		goChangeRatioCmd,
	IDC_RADIOBTNMIDDLE,		goChangeRatioCmd,
	IDC_RADIOBTNLOW,		goChangeRatioCmd,
	IDC_RADIOBTNX10,		goChangeRatioCmd,
	IDC_RADIOBTNX1,			goChangeRatioCmd
	
};

CtlWndStruct manualMenuBtns[]={
		{WS_EX_CLIENTEDGE,TEXT("STATIC"),IDC_STATICSPEEDMODE,TEXT("Speed Mode"),30,25,140,280,WS_CHILD|BS_GROUPBOX |WS_VISIBLE},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_RADIOBTNRAPID,TEXT("RAPID"),45,45,120,30,WS_CHILD|BS_AUTORADIOBUTTON|WS_VISIBLE|WS_TABSTOP},//|BM_SETCHECK},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_RADIOBTNHIGH,TEXT("HIGH"),45,90,120,30,WS_CHILD|BS_AUTORADIOBUTTON|WS_VISIBLE|WS_TABSTOP},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_RADIOBTNMIDDLE,TEXT("MIDDLE"),45,135,120,30,WS_CHILD|BS_AUTORADIOBUTTON|WS_VISIBLE|WS_TABSTOP},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_RADIOBTNLOW,TEXT("LOW"),45,180,120,30,WS_CHILD|BS_AUTORADIOBUTTON|WS_VISIBLE|WS_TABSTOP},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_RADIOBTNX10,TEXT("X10"),45,225,120,30,WS_CHILD|BS_AUTORADIOBUTTON|WS_VISIBLE|WS_TABSTOP},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_RADIOBTNX1,TEXT("X1"),45,270,120,30,WS_CHILD|BS_AUTORADIOBUTTON|WS_VISIBLE|WS_TABSTOP},		
	
		
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"), IDC_MachAllReturn,TEXT("MachAllRet"),320,35,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_OWNERDRAW},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"), IDC_MachXReturn,TEXT("MachXRet"),420,35,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_OWNERDRAW|WS_GROUP},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"), IDC_MachYReturn,TEXT("MachYRet"),520,35,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_OWNERDRAW|WS_GROUP},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"), IDC_MachZReturn,TEXT("MachZRet"),620,35,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_OWNERDRAW|WS_GROUP},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"), IDC_MachBReturn,TEXT("MachBRet"),720,35,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_OWNERDRAW|WS_GROUP},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"), IDC_MachCReturn,TEXT("MachCRet"),820,35,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_OWNERDRAW|WS_GROUP},
		
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"), IDC_WorkAllReturn,TEXT("WorkAllRet"),320,85,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_OWNERDRAW},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"), IDC_WorkXReturn,TEXT("WorkXRet"),420,85,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_OWNERDRAW|WS_GROUP},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"), IDC_WorkYReturn,TEXT("WorkYRet"),520,85,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_OWNERDRAW|WS_GROUP},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"), IDC_WorkZReturn,TEXT("WorkZRet"),620,85,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_OWNERDRAW|WS_GROUP},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"), IDC_WorkBReturn,TEXT("WorkBRet"),720,85,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_OWNERDRAW|WS_GROUP},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"), IDC_WorkCReturn,TEXT("WorkCRet"),820,85,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_OWNERDRAW|WS_GROUP},

		
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"), IDC_XTouchP,TEXT("X+"),320,155,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_OWNERDRAW},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"), IDC_YTouchP,TEXT("Y+"),420,155,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_OWNERDRAW|WS_GROUP},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"), IDC_ZTouchP,TEXT("Z+"),520,155,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_OWNERDRAW|WS_GROUP},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"), IDC_BTouchP,TEXT("B+"),620,155,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_OWNERDRAW|WS_GROUP},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"), IDC_CTouchP,TEXT("C+"),720,155,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_OWNERDRAW|WS_GROUP},

		{WS_EX_WINDOWEDGE,TEXT("BUTTON"), IDC_XTouchM,TEXT("X-"),320,205,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_OWNERDRAW},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"), IDC_YTouchM,TEXT("Y-"),420,205,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_OWNERDRAW|WS_GROUP},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"), IDC_ZTouchM,TEXT("Z-"),520,205,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_OWNERDRAW|WS_GROUP},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"), IDC_BTouchM,TEXT("B-"),620,205,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_OWNERDRAW|WS_GROUP},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"), IDC_CTouchM,TEXT("C-"),720,205,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_OWNERDRAW|WS_GROUP}

		
};

int MyRegisterClassManual(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)manualWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//(LPCTSTR)IDC_EX1;
	wcex.lpszClassName	= TEXT("MANUALWND");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
//注册手动窗口类
	if(!RegisterClassEx(&wcex)) return 0;

	return 1;
}

//手动窗口过程体
LRESULT CALLBACK manualWndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;
	

	//查找对应的消息ID,并执行对应的消息处理过程
	for(i=0; i<dim(manualMessages);i++)
	{
		if(msgCode == manualMessages[i].uCode) return(*manualMessages[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//对不存在的消息，调用缺省窗口过程
	return DefWindowProc(hWnd, msgCode, wParam, lParam);
}

LRESULT goCreateManual(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		
    int i;
//创建手动窗口菜单
	for(i=0;i<dim(manualMenuBtns);i++){
		CreateWindowEx(manualMenuBtns[i].dwExStyle,
			manualMenuBtns[i].szClass,
			manualMenuBtns[i].szTitle,
			manualMenuBtns[i].lStyle,
			manualMenuBtns[i].x,
			manualMenuBtns[i].y,
			manualMenuBtns[i].cx,
			manualMenuBtns[i].cy,
			hWnd,
			(HMENU)manualMenuBtns[i].nID,
			hInst,
			NULL
			);
	}

//创建手动窗口的子窗口
	/*
	for(i=0;i<dim(manualChildWnds);i++){
		CreateWindowEx( manualChildWnds[i].dwExStyle,
		    manualChildWnds[i].szClass,
			manualChildWnds[i].szTitle,
			manualChildWnds[i].lStyle,
			manualChildWnds[i].x,
			manualChildWnds[i].y,
			manualChildWnds[i].cx,
			manualChildWnds[i].cy,
			hWnd,
			(HMENU)manualChildWnds[i].nID,
			hInst,
			NULL
			);
	}
	//显示其中一个窗口
	ShowWindow(GetDlgItem(hWnd,manualChildWnds[0].nID),SW_SHOW);
*/
     //坐标手动操作板检测时间设定400ms
	SetTimer(hWnd,TIMER_MANUAL,400,NULL);

    

	return 0;

}

LRESULT goCommandManual(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		//定义菜单ID号和通知单
		int menuID, notifyCode;
		
		//定义窗口句柄
		HWND wndCtrl;
		 int i;

		 
		menuID    = LOWORD(wParam); 
		notifyCode = HIWORD(wParam); 
		wndCtrl = (HWND) lParam;
	   

		//显示被选中的子窗口，其他子窗口隐藏
		for(i=0; i<dim(manualCommands);i++)
		{
			if(menuID == manualCommands[i].uCode) return(*manualCommands[i].functionName)(hWnd,wndCtrl,menuID,notifyCode);
		}
			
		return 0;
}


LRESULT goTimerManual(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	DWORD readBuf;
	//以下调用手动面柄检测与操作函数
	
	ManualSpeedProc (hWnd);     //手持面柄速度选择

	readBuf =ioread(hWnd,IOPCIhDevice,IO_FPGA_READ, ADR_MANUAL_BORD_SPEED ); //按下两个

	if(readBuf==249)
	{
		ManualMoveAxisProc(hWnd);
		//readBuf = ioread(hWnd,IOPCIhDevice,IO_FPGA_READ,ADR_INA ); 
		//if(((readBuf&0x08)==0x0)&&(ddSorM==1)) 
		
	} 
	else
	{
	//以下建立停止对刀的处理函数
	  if(Control_single_touch == 0)
		 readBuf = ioread(hWnd,NCPCIhDevice,NC_FPGA_READ,ADR_C_IN_RAS_Z );   //停止对刀检测 ADR_INA   

      if(((readBuf&0x01)==0x0)||(Stop_single_touch == 0))
      {
                
				iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE, ADR_XVEL_L,  0X00);
				iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE, ADR_YVEL_L,  0X00);
				iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE, ADR_ZVEL_L,  0X00);//各轴对刀停止

				iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE, ADR_A_ENC_B,  0x0);
				Stop_single_touch = 1;
				readBuf = 1;
				Control_single_touch = 1;
				              
      }

	  else ManualMoveAxisProc(hWnd);    //手持面柄各轴移动
	}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 



	

	//以下建立软限位超出检测与处理函数
	/*
		if(SoftLimitEnable == TRUE)
		{
			if(data_m.x < 0 || data_m.x > 260 || data_m.y < 0 || data_m.y > 130 || data_m.z < 0 || 
				data_m.z >170 || data_m.b <-5  || data_m.b >75)
			{
				outb(0x0,ADR_SOFTLIM);}//设置软相位;
		}
	*/

	return 0;
}

LRESULT goPaintManual(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		PAINTSTRUCT ps;
		HDC hdc;

		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		return 0;
}

LRESULT goDestroyManual(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		PostQuitMessage(0);
		KillTimer(hWnd,TIMER_MANUAL);
		return 0;
}





DWORD WINAPI ManualMachAllReturnlCmdThreadProc(LPVOID lpParam)
{
		LPCmdThreadParam  pData; 
	    
		
		
		pData = (LPCmdThreadParam)lpParam;

		//以下添加全部轴机械回零操作程序
		MessageBox(pData->hWnd,"get into ManualMachAllReturnlCmdThreadProc",NULL,NULL);

		//Z轴归零
		if(data_m.z >= 110) iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO_DIR, 27);
			
 		else iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO_DIR, 31); //the direction of the axises not return zero must be '1'
		
		iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO, 0);
		
		Sleep(1);
		iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO, 4);//first write '0',then write '1' to insure there is a puls

						
		SuspendThread(ManualhThread); //挂起线程等待归零中断唤醒
		ZPosClr(pData->hWnd);
		


		//B轴归零
		if(data_m.b >= 0) iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO_DIR, 23); 

 		else iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO_DIR, 31);  //the direction of the axises not return zero must be '1'
		
		iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO, 0);
		Sleep(1);
		iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO, 8);//first write '0',then write '1' to insure there is a puls
		
		SuspendThread(ManualhThread); //挂起线程等待归零中断唤醒
		BPosClr(pData->hWnd);
	


		//C轴归零
		iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO_DIR, 31); //the direction of the axises not return zero must be '1'
		
		iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO, 0);
		Sleep(1);
		iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO, 16);//first write '0',then write '1' to insure there is a puls
		
		//X轴归零
		if(data_m.x >= 150) iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO_DIR, 30);

 		else iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO_DIR, 31);  //the direction of the axises not return zero must be '1'
		
		iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO, 0);
		Sleep(1);
		iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO, 1);//first write '0',then write '1' to insure there is a puls

		SuspendThread(ManualhThread); //挂起线程等待归零中断唤醒
		XPosClr(pData->hWnd);
		

		//Y轴归零
		if(data_m.y >= 85) iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO_DIR, 29);

 		else iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO_DIR, 31); //the direction of the axises not return zero must be '1'
		
		iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO, 0);
		Sleep(1);
		iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO, 2);//first write '0',then write '1' to insure there is a puls
		
		SuspendThread(ManualhThread); //挂起线程等待归零中断唤醒
		YPosClr(pData->hWnd);
	



		//取消线程、释放内存
		CloseHandle(ManualhThread);
		if(HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pData) == 0){
			MessageBox(pData->hWnd,"can not free heapmemory in function ManualMachAllReturnlCmdThreadProc",NULL,NULL);
			return 1;
		}
		return 0;
}

LRESULT goManualMachAllReturnlCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	
	DWORD dwThreadID;
	LPCmdThreadParam  pData;  
	

	CloseHandle(ManualhThread);	
	pData = (LPCmdThreadParam)HeapAlloc(GetProcessHeap(),
									HEAP_ZERO_MEMORY,
									sizeof(CmdThreadParam)
									);
	if(pData == NULL)
	{
		MessageBox(hWnd,"can not alloc heapmemory in function goManualMachAllReturnlCmd",NULL,NULL);
		return 1;
	}

	pData->hWnd = hWnd;
	pData->wndCtrl = wndCtrl;
	pData->menuID = menuID;
	pData->notifyCode = notifyCode;
		
   	ManualhThread = CreateThread(
		NULL,
		0,
		ManualMachAllReturnlCmdThreadProc,
		pData,
		CREATE_SUSPENDED,
		&dwThreadID
		);

	if( ManualhThread==NULL)
	{
		MessageBox(hWnd,"can not create Thread in function goManualMachAllReturnlCmd",NULL,NULL);
		return 1;
	}

	return 0;
}



DWORD WINAPI ManualMachSingleReturnCmdThreadProc(LPVOID lpParam)
{
	LPCmdThreadParam  pData; 
   
	pData = (LPCmdThreadParam)lpParam;
	
	//以下添加各轴机械回零操作程序
	MessageBox(pData->hWnd,"get into ManualMachSingleReturnCmdThreadProc",NULL,NULL);
	switch(pData->menuID){
		case IDC_MachXReturn:
				if(data_m.x >= 150) iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO_DIR, 30);

		 		else iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO_DIR, 31); //the direction of the axises not return zero must be '1'
				
				iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO, 0);
				Sleep(1);
				iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO, 1);//first write '0',then write '1' to insure there is a puls

				SuspendThread(ManualhThread); //挂起线程等待归零中断唤醒
				XPosClr(pData->hWnd);
				break;
		case IDC_MachYReturn:
				
				if(data_m.y >= 85) iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO_DIR, 29);

		 		else iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO_DIR, 31); //the direction of the axises not return zero must be '1'
				
				iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO, 0);
				Sleep(1);
				iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO, 2);//first write '0',then write '1' to insure there is a puls
				
				SuspendThread(ManualhThread); //挂起线程等待归零中断唤醒
				YPosClr(pData->hWnd);
				break;
	
		case IDC_MachZReturn:
				
				if(data_m.z >= 110) iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO_DIR, 27);

		 		else iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO_DIR, 31); //the direction of the axises not return zero must be '1'
				
				iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO, 0);
				Sleep(1);
				iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO, 4);//first write '0',then write '1' to insure there is a puls
				
				SuspendThread(ManualhThread); //挂起线程等待归零中断唤醒
				ZPosClr(pData->hWnd);
				break;
		
		case IDC_MachBReturn:
				
				if(data_m.b >= 0) iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO_DIR, 23);

		 		else iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO_DIR, 31); //the direction of the axises not return zero must be '1'
				
				iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO, 0);
				Sleep(1);
				iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO, 8);//first write '0',then write '1' to insure there is a puls
				
				SuspendThread(ManualhThread); //挂起线程等待归零中断唤醒
				BPosClr(pData->hWnd);
				break;

		case IDC_MachCReturn:
				
				if(data_m.b >= 0) iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO_DIR, 31);//the direction of the axises not return zero must be '1'
				
				iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO, 0);
				Sleep(1);
				iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_RETURN_ZERO, 16);//first write '0',then write '1' to insure there is a puls
				
				break;
		default: break;
	}


	
	//取消线程、释放内存
	CloseHandle(ManualhThread);
	if(HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pData) == 0){
		MessageBox(pData->hWnd,"can not free heapmemory in function ManualMachSingleReturnCmdThreadProc",NULL,NULL);
		return 1;
	}
	return 0;
}


LRESULT goManualMachSingleReturnCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	
	DWORD dwThreadID;
	LPCmdThreadParam  pData;  
	

	CloseHandle(ManualhThread);	
	pData = (LPCmdThreadParam)HeapAlloc(GetProcessHeap(),
									HEAP_ZERO_MEMORY,
									sizeof(CmdThreadParam)
									);
	if(pData == NULL)
	{
		MessageBox(hWnd,"can not alloc heapmemory in function goManualMachSingleReturnCmd",NULL,NULL);
		return 1;
	}

	pData->hWnd = hWnd;
	pData->wndCtrl = wndCtrl;
	pData->menuID = menuID;
	pData->notifyCode = notifyCode;
		
   	ManualhThread = CreateThread(
		NULL,
		0,
		ManualMachSingleReturnCmdThreadProc,
		pData,
		CREATE_SUSPENDED,
		&dwThreadID
		);

	if( ManualhThread==NULL)
	{
		MessageBox(hWnd,"can not create Thread in function goManualMachSingleReturnCmd",NULL,NULL);
		return 1;
	}


	return 0;
	
}


DWORD WINAPI ManualWorkAllReturnlCmdThreadProc(LPVOID lpParam)
{
	LPCmdThreadParam  pData; 
 
	pData = (LPCmdThreadParam)lpParam;
	
	//以下添加全部轴工件坐标原点回零操作程序
	MessageBox(pData->hWnd,"get into ManualWorkAllReturnlCmdThreadProc",NULL,NULL);
	
	
	
	
	//取消线程、释放内存
	CloseHandle(ManualhThread);
	if(HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pData) == 0){
		MessageBox(pData->hWnd,"can not free heapmemory in function ManualWorkAllReturnlCmdThreadProc",NULL,NULL);
		return 1;
	}
	return 0;
}

LRESULT goManualWorkAllReturnlCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	
	DWORD dwThreadID;
	LPCmdThreadParam  pData;  
	

	CloseHandle(ManualhThread);	
	pData = (LPCmdThreadParam)HeapAlloc(GetProcessHeap(),
									HEAP_ZERO_MEMORY,
									sizeof(CmdThreadParam)
									);
	if(pData == NULL)
	{
		MessageBox(hWnd,"can not alloc heapmemory in function goManualWorkAllReturnlCmd",NULL,NULL);
		return 1;
	}

	pData->hWnd = hWnd;
	pData->wndCtrl = wndCtrl;
	pData->menuID = menuID;
	pData->notifyCode = notifyCode;
		
   	ManualhThread = CreateThread(
		NULL,
		0,
		ManualWorkAllReturnlCmdThreadProc,
		pData,
		CREATE_SUSPENDED,
		&dwThreadID
		);

	if( ManualhThread==NULL)
	{
		MessageBox(hWnd,"can not create Thread in function goManualWorkAllReturnlCmd",NULL,NULL);
		return 1;
	}

	
	return 0;
}

DWORD WINAPI ManualWorkSingleReturnCmdThreadProc(LPVOID lpParam)
{
	LPCmdThreadParam  pData; 
    
	

	
	pData = (LPCmdThreadParam)lpParam;
	
	//以下添加各轴工件坐标原点回零操作程序
	MessageBox(pData->hWnd,"get into ManualWorkSingleReturnCmdThreadProc",NULL,NULL);
	
	
	
	
	//取消线程、释放内存
	CloseHandle(ManualhThread);
	if(HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pData) == 0){
		MessageBox(pData->hWnd,"can not free heapmemory in function ManualWorkSingleReturnCmdThreadProc",NULL,NULL);
		return 1;
	}
	return 0;
}


LRESULT goManualWorkSingleReturnCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	
	DWORD dwThreadID;
	LPCmdThreadParam  pData;  
	

	CloseHandle(ManualhThread);	
	pData = (LPCmdThreadParam)HeapAlloc(GetProcessHeap(),
									HEAP_ZERO_MEMORY,
									sizeof(CmdThreadParam)
									);
	if(pData == NULL)
	{
		MessageBox(hWnd,"can not alloc heapmemory in function goManualWorkSingleReturnCmd",NULL,NULL);
		return 1;
	}

	pData->hWnd = hWnd;
	pData->wndCtrl = wndCtrl;
	pData->menuID = menuID;
	pData->notifyCode = notifyCode;
		
   	ManualhThread = CreateThread(
		NULL,
		0,
		ManualWorkSingleReturnCmdThreadProc,
		pData,
		CREATE_SUSPENDED,
		&dwThreadID
		);

	if( ManualhThread==NULL)
	{
		MessageBox(hWnd,"can not create Thread in function goManualWorkSingleReturnCmd",NULL,NULL);
		return 1;
	}

	return 0;
}


DWORD WINAPI ManualSingleTouchCmdThreadProc(LPVOID lpParam)
{

	
	LPCmdThreadParam  pData; 
	pData = (LPCmdThreadParam)lpParam;
	PAINTSTRUCT ps;
	HDC hdc;
	hdc=BeginPaint(pData->hWnd,&ps);	
	//以下添加各轴对刀操作程序
	MessageBox(pData->hWnd,"get into ManualSingleTouchCmdThreadProc",NULL,NULL);
	ddSorM=0;
	iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_IQR4_ENABLE, 0);//禁止NC板的IRQ3中断
	 
	iowrite(pData->hWnd,NCPCIhDevice,NC_FPGA_WRITE, ADR_A_ENC_B,  0x01);
	Control_single_touch = 0;

	switch(pData->menuID){
		case IDC_XTouchP:
			//Manual_keydown_notif_proc(pData->hWnd,ADR_XVEL_L, ADR_XPUL_L, 0); //X轴正方向对刀启动
			Manual_keydown_notif_proc(pData->hWnd,ADR_XVEL_L, ADR_XPUL_L, 0); //X轴负方向对刀启动

			break;

		case IDC_XTouchM:
			Manual_keydown_notif_proc(pData->hWnd,ADR_XVEL_L, ADR_XPUL_L, 1); //X轴负方向对刀启动
			break;

		case IDC_YTouchP:
			Manual_keydown_notif_proc(pData->hWnd,ADR_YVEL_L, ADR_YPUL_L, 0);
			break;

		case IDC_YTouchM:
			Manual_keydown_notif_proc(pData->hWnd,ADR_YVEL_L, ADR_YPUL_L, 1);
			break;

		case IDC_ZTouchP:
			Manual_keydown_notif_proc(pData->hWnd,ADR_ZVEL_L, ADR_ZPUL_L, 0);
			break;

		case IDC_ZTouchM:
			Manual_keydown_notif_proc(pData->hWnd,ADR_ZVEL_L, ADR_ZPUL_L, 1);
			break;

		case IDC_BTouchP:
			Manual_keydown_notif_proc(pData->hWnd,ADR_BVEL_L, ADR_BPUL_L, 0);
			break;

		case IDC_BTouchM:
			Manual_keydown_notif_proc(pData->hWnd,ADR_BVEL_L, ADR_BPUL_L, 1);
			break;

		case IDC_CTouchP:
			Manual_keydown_notif_proc(pData->hWnd,ADR_CVEL_L, ADR_CPUL_L, 0);
			break;

		case IDC_CTouchM:
			Manual_keydown_notif_proc(pData->hWnd,ADR_CVEL_L, ADR_CPUL_L, 1);
			break;
		default: break;
	}
	
	
	
	//取消线程、释放内存
	//CloseHandle(ManualhThread);
	if(HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pData) == 0){
		MessageBox(pData->hWnd,"can not free heapmemory in function ManualSingleTouchCmdThreadProc",NULL,NULL);
		return 1;
	}
	return 0;
}

DWORD WINAPI ChangeRatioCmdThreadProc(LPVOID lpParam)
{

	LPCmdThreadParam  pData; 
	pData = (LPCmdThreadParam)lpParam;
	PAINTSTRUCT ps;
	HDC hdc;
	hdc=BeginPaint(pData->hWnd,&ps);
	ddSorM=0;
	//以下添加各轴对刀操作程序
	//MessageBox(pData->hWnd,"get into ChangeRatioCmdThreadProc",NULL,NULL);
 
	switch(pData->menuID){
		case IDC_RADIOBTNRAPID:
			ManualSpeed = SPEED_RAPID;			
			break;

		case IDC_RADIOBTNHIGH:
			ManualSpeed = SPEED_HIGH;
			break;

		case IDC_RADIOBTNMIDDLE:
			ManualSpeed = SPEED_MIDDLE;
			break;

		case IDC_RADIOBTNLOW:
			ManualSpeed = SPEED_LOW;
			break;

		case IDC_RADIOBTNX10:
			ManualSpeed = SPEED_STEP10;
			break;
		case IDC_RADIOBTNX1:
			ManualSpeed = SPEED_STEP1;
			break;

		default: break;
	}
	
	
	
	//取消线程、释放内存
	CloseHandle(ManualhThread);
	if(HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pData) == 0){
		MessageBox(pData->hWnd,"can not free heapmemory in function ManualSingleTouchCmdThreadProc",NULL,NULL);
		return 1;
	}
	return 0;
}

LRESULT goChangeRatioCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	DWORD dwThreadID;
	LPCmdThreadParam  pData;  
	

	CloseHandle(ManualhThread);	
	pData = (LPCmdThreadParam)HeapAlloc(GetProcessHeap(),
									HEAP_ZERO_MEMORY,
									sizeof(CmdThreadParam)
									);
	if(pData == NULL)
	{
		MessageBox(hWnd,"can not alloc heapmemory in function goChangeRatioCmd",NULL,NULL);
		return 1;
	}

	pData->hWnd = hWnd;
	pData->wndCtrl = wndCtrl;
	pData->menuID = menuID;
	pData->notifyCode = notifyCode;
		
   	ManualhThread = CreateThread(
		NULL,
		0,
		ChangeRatioCmdThreadProc,
		pData,
		0,
		&dwThreadID
		);

	if( ManualhThread==NULL)
	{
		MessageBox(hWnd,"can not create Thread in function goChangeRatioCmd",NULL,NULL);
		return 1;
	}
	return 0;
}


LRESULT goManualSingleTouchCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	
	DWORD dwThreadID;
	LPCmdThreadParam  pData;  
	

	CloseHandle(ManualhThread);	
	pData = (LPCmdThreadParam)HeapAlloc(GetProcessHeap(),
									HEAP_ZERO_MEMORY,
									sizeof(CmdThreadParam)
									);
	if(pData == NULL)
	{
		MessageBox(hWnd,"can not alloc heapmemory in function goManualSingleTouchCmd",NULL,NULL);
		return 1;
	}

	pData->hWnd = hWnd;
	pData->wndCtrl = wndCtrl;
	pData->menuID = menuID;
	pData->notifyCode = notifyCode;
		
   	ManualhThread = CreateThread(
		NULL,
		0,
		ManualSingleTouchCmdThreadProc,
		pData,
		CREATE_SUSPENDED,
		&dwThreadID
		);

	if( ManualhThread==NULL)
	{
		MessageBox(hWnd,"can not create Thread in function goManualSingleTouchCmd",NULL,NULL);
		return 1;
	}
	return 0;
}


	
/*设置手动档*/
int CheckSpeedRadioBtn(HWND hWnd)
{
	
		SendMessage( GetDlgItem(hWnd,IDC_RADIOBTNRAPID), BM_SETCHECK, BST_UNCHECKED, 0);
		SendMessage( GetDlgItem(hWnd, IDC_RADIOBTNHIGH), BM_SETCHECK, BST_UNCHECKED, 0);
		SendMessage( GetDlgItem(hWnd, IDC_RADIOBTNMIDDLE), BM_SETCHECK, BST_UNCHECKED, 0);
		SendMessage( GetDlgItem(hWnd, IDC_RADIOBTNLOW), BM_SETCHECK, BST_UNCHECKED, 0);
		SendMessage( GetDlgItem(hWnd, IDC_RADIOBTNX10), BM_SETCHECK, BST_UNCHECKED, 0);
		SendMessage( GetDlgItem(hWnd, IDC_RADIOBTNX1), BM_SETCHECK, BST_UNCHECKED, 0);

	

	if( ManualSpeed == SPEED_RAPID )  SendMessage( GetDlgItem(hWnd,IDC_RADIOBTNRAPID), BM_SETCHECK, BST_CHECKED, 0);
	else if( ManualSpeed == SPEED_HIGH ) SendMessage( GetDlgItem(hWnd, IDC_RADIOBTNHIGH), BM_SETCHECK, BST_CHECKED, 0);
	else if( ManualSpeed == SPEED_MIDDLE ) SendMessage( GetDlgItem(hWnd, IDC_RADIOBTNMIDDLE), BM_SETCHECK, BST_CHECKED, 0);
	else if( ManualSpeed == SPEED_LOW ) SendMessage( GetDlgItem(hWnd, IDC_RADIOBTNLOW), BM_SETCHECK, BST_CHECKED, 0);
	else if( ManualSpeed == SPEED_STEP10 ) SendMessage( GetDlgItem(hWnd, IDC_RADIOBTNX10), BM_SETCHECK, BST_CHECKED, 0);
	else if( ManualSpeed == SPEED_STEP1 ) SendMessage( GetDlgItem(hWnd, IDC_RADIOBTNX1), BM_SETCHECK, BST_CHECKED, 0);
	return 0;
}



int  ManualSpeedProc (HWND hWnd)
{	
	DWORD readBuf;
	readBuf = 0;//   	readBuf =ioread(hWnd,IOPCIhDevice,IO_FPGA_READ, ADR_MANUAL_BORD_SPEED );
//	readBuf =ioread(hWnd,IOPCIhDevice,IO_FPGA_READ, ADR_OUTB );



 	/*wsprintf(chTemp1, " %d", readBuf);

      MessageBox(hWnd,chTemp1,NULL,NULL);*/

	if (readBuf != 0)
	{
		switch(readBuf) {
		case 226:
		   ManualSpeed = SPEED_RAPID; 
		   break;
		case 229:
		   ManualSpeed = SPEED_HIGH; 
		   break;
		case 232:
		   ManualSpeed = SPEED_MIDDLE; 
		   break;
		case 251:
		   ManualSpeed = SPEED_LOW; 
		   break;
		case 225:
		   ManualSpeed = SPEED_STEP10; 
		   break;
		case 228:
		   ManualSpeed = SPEED_STEP1; 
		   break;
		}
	}
	CheckSpeedRadioBtn(hWnd);
	return 0;
}


//通过手持面柄移动各轴函数
int ManualMoveAxisProc(HWND hWnd)
{
	
	DWORD readBuf;
	static int axis = 0;	
	
	readBuf = 0;//	readBuf = ioread(hWnd,IOPCIhDevice,IO_FPGA_READ, ADR_MANUAL_BORD_AXIS);
	
	if((readBuf!=0)&&(axis==1))
	{
		
        	iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_IQR4_ENABLE, 0);//禁止NC板的IRQ3中断            
						
			switch(readBuf){
				case 57:	//X+
					Manual_keydown_notif_proc(hWnd,ADR_XVEL_L, ADR_XPUL_L, 0);
				break;
				case 38:	//X-
					Manual_keydown_notif_proc(hWnd,ADR_XVEL_L, ADR_XPUL_L, 1);
				break;
				case 35://
					Manual_keydown_notif_proc(hWnd,ADR_YVEL_L, ADR_YPUL_L, 0);
				break;
				case 32://
					Manual_keydown_notif_proc(hWnd,ADR_YVEL_L, ADR_YPUL_L, 1);
				break;
				case 42:
					Manual_keydown_notif_proc(hWnd,ADR_ZVEL_L, ADR_ZPUL_L, 0);
				break;
				case 39:
					Manual_keydown_notif_proc(hWnd,ADR_ZVEL_L, ADR_ZPUL_L, 1);
				break;
				case 36:
					Manual_keydown_notif_proc(hWnd,ADR_BVEL_L, ADR_BPUL_L, 0);
				break;
				case 33://
					Manual_keydown_notif_proc(hWnd,ADR_BVEL_L, ADR_BPUL_L, 1);
				break;
				case 59:
					Manual_keydown_notif_proc(hWnd,ADR_CVEL_L, ADR_CPUL_L, 0);
				break;
				case 40:
					Manual_keydown_notif_proc(hWnd,ADR_CVEL_L, ADR_CPUL_L, 1);
				break;
			}
	axis=0;
	}
	else if(readBuf==0 && axis==0)
	{
		iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_XVEL_L, 0);
		iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_YVEL_L, 0);
		iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_ZVEL_L, 0);
		iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_BVEL_L, 0);
		iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_CVEL_L, 0);

		
       		if(ManualSpeed==SPEED_RAPID || ManualSpeed==SPEED_HIGH || ManualSpeed==SPEED_MIDDLE || ManualSpeed==SPEED_LOW )
				iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_DDATOMANUAL, 0);//NC板进入手动调节模式
				
		axis=1;
	}
	
	return 0;
}


int Manual_keydown_notif_proc(HWND hWnd,int adr_axisvel, int adr_axispul, int orientation)	//orientation为0正转, 1反转
{
	
	iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_IQR4_ENABLE, 0);//禁止NC板的IRQ3中断 
	
	
	if(ManualSpeed==SPEED_RAPID || ManualSpeed==SPEED_HIGH || ManualSpeed==SPEED_MIDDLE || ManualSpeed==SPEED_LOW)
		{
			iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_DDATOMANUAL, 0);//NC板进入手动调节模式
			
			iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_DATA_STARTEND, 0);//开始传送数据
				  	
		
		  if(ADR_XVEL_L != adr_axisvel)
		  {
			  iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_XVEL_L, 0);
			  			  
		  }
		  if(ADR_YVEL_L != adr_axisvel) 
		  {
			   iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_YVEL_L, 0);
			  
		  }
		  if(ADR_ZVEL_L != adr_axisvel)
		  {
			   iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_ZVEL_L, 0);
			  
		  }
		  if(ADR_BVEL_L != adr_axisvel)
		  {
			   iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_BVEL_L, 0);
			  
		  }
		  if(ADR_CVEL_L != adr_axisvel) 
		  {
			   iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_CVEL_L, 0);
			  
		  }
		  if(!orientation)
		  {
			  iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  adr_axisvel, ManualSpeed & 0x0FFFF);
			  			 
		  }
		  else 
		  {
			  iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  adr_axisvel, (ManualSpeed + 0x8000) & 0x0FFFF);
			 
		  }
		  iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_DATA_STARTEND, 1);//结束传送数据
		  		
		}
	
	else if(ManualSpeed==SPEED_STEP10)
		{
			 iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_DDATOMANUAL, 1);//NC板进入DDA调节
			 iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_DATA_STARTEND, 0);//开始传送数据
					 
		  
		  if(ADR_XVEL_L != adr_axisvel) 
		  {
			  iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_XVEL_L, 0);
			 
		  }
		   if(ADR_YVEL_L != adr_axisvel) 
		  {
			  iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_YVEL_L, 0);
			  
		  }
		  if(ADR_ZVEL_L != adr_axisvel)
		  {
			 iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_ZVEL_L, 0);
		  }
		  if(ADR_BVEL_L != adr_axisvel)
		  {
			  iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_BVEL_L, 0);
			  
		  }
		  if(ADR_CVEL_L != adr_axisvel) 
		  {
			iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_CVEL_L, 0);
			  
		  }
		   if(!orientation)
		  {
			   iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  adr_axisvel, ManualSpeed & 0x0FFFF);
			 
		  }
		  else 
		  {
			   iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  adr_axisvel, (ManualSpeed + 0x8000) & 0x0FFFF);
			  
		  }
		  
		  
		  if(ADR_XPUL_L != adr_axispul)  iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_XPUL_L, 0);
		  if(ADR_YPUL_L != adr_axispul)  iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_YPUL_L, 0);
		  if(ADR_ZPUL_L != adr_axispul)  iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_ZPUL_L, 0);
		  if(ADR_BPUL_L != adr_axispul)  iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_BPUL_L, 0);
		  if(ADR_CPUL_L != adr_axispul)  iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_CPUL_L, 0);
		 
		  if(!orientation) iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  adr_axispul, 20);
		  else iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  adr_axispul, (0x8000+20)& 0x0FFFF);

		  iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_DATA_STARTEND, 1);//结束传送数据
		  
		}
	else if(ManualSpeed==SPEED_STEP1)
		{
			 iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_DDATOMANUAL, 1);//NC板进入DDA调节
			 iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_DATA_STARTEND, 0);//开始传送数据
		 
		   if(ADR_XVEL_L != adr_axisvel) 
		  {
			   iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_XVEL_L, 0);
		  }
		   if(ADR_YVEL_L != adr_axisvel) 
		  {
			  iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_YVEL_L, 0);
			  
		  }
		  if(ADR_ZVEL_L != adr_axisvel)
		  {
			 iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_ZVEL_L, 0);
		  }
		  if(ADR_BVEL_L != adr_axisvel)
		  {
			iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_BVEL_L, 0);
			  
		  }
		  if(ADR_CVEL_L != adr_axisvel) 
		  {
			 iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_CVEL_L, 0);;
			  
		  }
			 if(!orientation)
		  {
			  iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  adr_axisvel, ManualSpeed & 0x0FFFF);
			 
		  }
		  else 
		  {
			iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  adr_axisvel, (ManualSpeed + 0x8000) & 0x0FFFF);
			  
		  }
			  
		  
		  if(ADR_XPUL_L != adr_axispul)  iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_XPUL_L, 0);
		  if(ADR_YPUL_L != adr_axispul)  iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_YPUL_L, 0);
		  if(ADR_ZPUL_L != adr_axispul)  iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_ZPUL_L, 0);
		  if(ADR_BPUL_L != adr_axispul)  iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_BPUL_L, 0);
		  if(ADR_CPUL_L != adr_axispul)  iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_CPUL_L, 0);
		 
		  if(!orientation) iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  adr_axispul, 2);
		  else iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  adr_axispul, (0x8000+2)& 0x0FFFF);

		  iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_DATA_STARTEND, 1);//结束传送数据
			
		    
	}
	return 0;
}

double XYZPosRepair(double XYZCounter)
{
	double StandardCounter[31] = {9700,9720,9740,9760,9780,9800,9820,9840,9860,9880,9900,9920,9940,9960,9980,10000,10020,10040,10060,10080,10100,10120,10140,10160,10180,10200,10220,10240,10260,10280,10300};
	double RepairCounter[31];
	int i;
	
	double small;
	int small_Item;
	
	for(i = 0;i < 31; i++)
		RepairCounter[i] = abs(StandardCounter[i] - XYZCounter);
	
	small = RepairCounter[0];
	small_Item = 0;
	
	for(i = 1; i < 31; i++)
	{
		if(RepairCounter[i] < RepairCounter[i-1])
		{
			small = RepairCounter[i];
			small_Item = i;
		}
	}
	
	return StandardCounter[small_Item];
		
}


int XPosClr(HWND hWnd)
{
	double Data_Fir;//the data of the first counter;
	double Data_Sec;//the data of the second counter;
	double Data_Thi;//the data of the third counter;
	
	double Data_FirNew;
	double Data_SecNew;//data after being repaired;
	
	double XPos;
	int k1,k2;
	
	
	Data_Fir = (double)ioread(hWnd,NCPCIhDevice,NC_FPGA_READ, ADR_XPOS_CLR_L_FIR);

	Data_Sec = (double)ioread(hWnd,NCPCIhDevice,NC_FPGA_READ, ADR_XPOS_CLR_L_SEC);

	Data_Thi = (double)ioread(hWnd,NCPCIhDevice,NC_FPGA_READ, ADR_XPOS_CLR_L_THI)+1;  //   +1???????


			
	if(Data_Fir>40000)	Data_Fir = 65536 - Data_Fir;
	if(Data_Sec>40000)	Data_Sec = 65536 - Data_Sec;
	if(Data_Thi>40000)	Data_Thi = 65536 - Data_Thi;

	Data_FirNew = XYZPosRepair(Data_Fir);
	Data_SecNew = XYZPosRepair(Data_Sec);
	
	k1=((int)Data_FirNew-10040)/20;
	k2=((int)Data_SecNew-10040)/20;
	
	Data_Fir/=1000;
	Data_FirNew/=1000;
	Data_Sec/=1000;
	Data_SecNew/=1000;
	Data_Thi/=1000;//puls to mm;
	
	
	if(Data_FirNew > 10)
	{
		XPos=(double)(20*k1);
		if(20==Data_FirNew + Data_SecNew)
			{XPos = XPos+20;
			XPos+=(Data_Thi-Data_Sec);}
		else
			{XPos = XPos-Data_SecNew;
			XPos-=(Data_Thi-Data_Sec);}
			
	}
	if(Data_SecNew > 10)
	{
		XPos=(double)(20*k2);
		if(20!= Data_FirNew + Data_SecNew)
			{XPos = XPos+Data_SecNew;
			XPos+=(Data_Thi-Data_Sec);}
		else
			XPos-=(Data_Thi-Data_Sec);
	}

	XPos+=9.98;
	
	
	MachinePosPlusWrite(hWnd,XPos,data_m.y,data_m.z,data_m.b,data_m.c);
	return 0;
}

int YPosClr(HWND hWnd)
{
	double Data_Fir;//the data of the first counter;
	double Data_Sec;//the data of the second counter;
	double Data_Thi;//the data of the third counter;
	
	double Data_FirNew;
	double Data_SecNew;//the data of the counter after being repaired;
	
	double YPos;
	int k1,k2;


	Data_Fir = ioread(hWnd,NCPCIhDevice,NC_FPGA_READ, ADR_YPOS_CLR_L_FIR);

	Data_Sec = ioread(hWnd,NCPCIhDevice,NC_FPGA_READ, ADR_YPOS_CLR_L_SEC);

	Data_Thi = ioread(hWnd,NCPCIhDevice,NC_FPGA_READ, ADR_YPOS_CLR_L_THI)+1;  //   +1???????

	
	if(Data_Fir>40000)	Data_Fir = 65536 - Data_Fir;
	if(Data_Sec>40000)	Data_Sec = 65536 - Data_Sec;
	if(Data_Thi>40000)	Data_Thi = 65536 - Data_Thi;
	
	Data_FirNew = XYZPosRepair(Data_Fir);
	Data_SecNew = XYZPosRepair(Data_Sec);
	
	
		
	k1=((int)Data_FirNew-9860)/20;
	k2=((int)Data_SecNew-9860)/20;

	Data_FirNew/=1000;
	Data_SecNew/=1000;	
	Data_Fir/=1000;
	Data_Sec/=1000;	
	Data_Thi/=1000;//puls to mm;
	
	
	if(Data_FirNew<10)
	{
		YPos=(double)20*k1;
		if(20==Data_FirNew+Data_SecNew)
			{YPos = YPos+20;
			 YPos += (Data_Thi - Data_Sec);}
		else
			{YPos = YPos-Data_SecNew;
			 YPos -= (Data_Thi - Data_Sec);}
	}
	if(Data_SecNew<10)
	{
		YPos=(double)20*k2;
		if(20!=Data_FirNew+Data_SecNew)
		   {YPos = YPos + Data_SecNew;
			YPos += (Data_Thi - Data_Sec);}
		else
			YPos -= (Data_Thi - Data_Sec);
	}

	
	MachinePosPlusWrite(hWnd,data_m.x,YPos,data_m.z,data_m.b,data_m.c);
	return 0;
}
int ZPosClr(HWND hWnd)
{
	double Data_Fir;//the data of the first counter;
	double Data_Sec;//the data of the second counter;
	double Data_Thi;//the data of the third counter;

	double Data_FirNew;
	double Data_SecNew;//the data after being repaired;
	double ZPos;
	int k1,k2;


	Data_Fir = ioread(hWnd,NCPCIhDevice,NC_FPGA_READ, ADR_ZPOS_CLR_L_FIR);

	Data_Sec = ioread(hWnd,NCPCIhDevice,NC_FPGA_READ, ADR_ZPOS_CLR_L_SEC);

	Data_Thi = ioread(hWnd,NCPCIhDevice,NC_FPGA_READ, ADR_ZPOS_CLR_L_THI)+1;  //   +1???????

	
	if(Data_Fir>40000)	Data_Fir = 65536 - Data_Fir;
	if(Data_Sec>40000)	Data_Sec = 65536 - Data_Sec;
	if(Data_Thi>40000)	Data_Thi = 65536 - Data_Thi;

	Data_FirNew = XYZPosRepair(Data_Fir);
	Data_SecNew = XYZPosRepair(Data_Sec);

	
	
	
	k1=((int)Data_FirNew-10040)/20;
	k2=((int)Data_SecNew-10040)/20;
	
	Data_FirNew/=1000;
	Data_SecNew/=1000;
	Data_Fir/=1000;
	Data_Sec/=1000;
	Data_Thi/=1000;//puls to mm;
	
	
	if(Data_FirNew>10)
	{
		ZPos=(double)(20*k1);
		if(20==Data_FirNew + Data_SecNew)
			{ZPos +=20;
		        ZPos+=(Data_Thi-Data_Sec);}
		else
			{ZPos = ZPos-Data_SecNew;
			ZPos-=(Data_Thi-Data_Sec);}
	}
	if(Data_SecNew>10)
	{
		ZPos=(double)(20*k2);
		if(20!=Data_FirNew + Data_SecNew)
			{ZPos = ZPos + Data_SecNew;
		        ZPos+=(Data_Thi-Data_Sec);}
		else
			ZPos-=(Data_Thi-Data_Sec);
	}

	ZPos+=9.98;	

	
	
	MachinePosPlusWrite(hWnd,data_m.x,data_m.y,ZPos,data_m.b,data_m.c);
	return 0;
}

int BPosClr(HWND hWnd)
{
	
	double Data_Fir;//the data of the first counter;
	double Data_Sec;//the data of the second counter;
	double BPos;

	

	Data_Fir = (double)ioread(hWnd,NCPCIhDevice,NC_FPGA_READ, ADR_BPOS_CLR_L_FIR);

	Data_Sec = (double)ioread(hWnd,NCPCIhDevice,NC_FPGA_READ, ADR_BPOS_CLR_L_SEC);

		
	BPos = (Data_Sec - Data_Fir)/5000;
	
	

	MachinePosPlusWrite(hWnd,data_m.x,data_m.y,data_m.z,BPos,data_m.c);
	return 0;
}


