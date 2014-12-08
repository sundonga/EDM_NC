#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

//int IinitProgram(HWND hWnd);  //整个数控程序软硬件的初始化

int DspProgramLoad(HWND hWnd);     //DSP程序下载
int DspProgramLoad1(HWND hWnd);     //DSP程序下载

extern HANDLE IOPCIhDevice;               // IO_PCI设备句柄 
extern HANDLE NCPCIhDevice;                // NC_PCI设备句柄
extern HANDLE PowerPCIhDevice;                // NC_PCI设备句柄 
extern HANDLE hDecodeEvent;  //启动向下位机传送译码数据线程事件的句柄 

extern HINSTANCE hInst;								// 当前实例
extern TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
extern TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

extern Coordata data_m;  //Machine coor//?????
extern Coordata data_w;   //work coor
extern Coordata data_r;   //ralitive coor
extern Coorpuls pulse_m; //Machine pulse
extern Coorpuls pulse_w; //work pulse
extern ConditionStru Machiningparam;
extern char AutoMachineFileName[_MAX_FNAME];

//2010-4-1 lsw
extern FILE* fpTemp;//定位temp.txt文件的指针
extern FILE* fpSeek;//定位seek.txt文件的指针
extern FILE* fpLock;//定位lock.txt文件的指针
extern FILE* fpSet;
extern FILE* fpMac;
extern FILE* fpSys;
extern FILE* fpEIO;
extern FILE* fpEIO_Address;
extern FILE* fpcoor;
extern int seekPre(int);
extern int startR;
extern int Mill_ESPARK;//2010-5-25

HWND hWnd1;

HANDLE ScreenThread;
DWORD WINAPI ScreenThreadProc(LPVOID lpParam);


extern HWND hScreenEdit;

extern int isEndScreen;
//2010-4-4 lsw

PNOTEINFO pNoteInfo;//?????

//2010

HBITMAP hbm1;
HBITMAP hbm2;
//
const MessageProc mainMessages[]={
	    WM_CREATE, goCreateMain,
		WM_COMMAND, goCommandMain,
		WM_TIMER,   goTimerMain,
		WM_PAINT,  goPaintMain,
		WM_DRAWITEM, goDrawitemMain,
		WM_DESTROY, goDestroyMain
};
const CommandProc mainCommands[]={
	IDC_BTNMANUEL,     goMainManualCmd,
	IDC_BTNAUTO,       goMainAutoCmd,
	IDC_BTNEDIT,       goMainEditCmd,
	IDC_BTNMDI,        goMainMdiCmd,
	IDC_BTNLIM,        goMainConditionCmd,//by lsw in 2010-3-24 原为"IDC_BTNLIM,        goMainSLimenCmd"
	IDC_StartStop,     goMainStartStopCmd,//by lsw in 2010-4-7
	IDC_BTNREST,       goMainResetCmd,//by lsw in 2010-4-19
	IDC_BTNCLOSE,	   goExpertSystemCmd	
	//IDC_BTNCLOSE,      goMainCloseCmd
};
//主窗口菜单属性列表	

CtlWndStruct mainMenuBtns[]={
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNMANUEL,TEXT("MANUAL"),0,704,128,64, WS_BORDER| WS_CHILD|BS_OWNERDRAW|WS_VISIBLE},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNAUTO,TEXT("AUTO"),128,704,128,64,WS_BORDER|WS_CHILD|BS_OWNERDRAW|WS_VISIBLE},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNMDI,TEXT("MDI"),256,704,128,64,WS_BORDER|WS_CHILD|BS_OWNERDRAW|WS_VISIBLE},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNEDIT,TEXT("EDIT"),384,704,128,64,WS_BORDER|WS_CHILD|BS_OWNERDRAW|WS_VISIBLE},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNLIM,TEXT("CONDITION"),512,704,128,64,WS_BORDER|WS_CHILD|BS_OWNERDRAW|WS_VISIBLE},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_StartStop,TEXT("NCSET"),640,704,128,64,WS_BORDER|WS_CHILD|BS_OWNERDRAW|WS_VISIBLE},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNREST,TEXT("MAINTAIN"),768,704,128,64,WS_BORDER|WS_CHILD|BS_OWNERDRAW|WS_VISIBLE},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNCLOSE,TEXT("CTLKEY"),896,704,128,64,WS_BORDER|WS_CHILD|BS_OWNERDRAW|WS_VISIBLE},
		{WS_EX_WINDOWEDGE,TEXT("STATIC"),IDC_STATICALARM,TEXT("alarm:"),0,654,1024,50,WS_BORDER|WS_CHILD|WS_BORDER|WS_VISIBLE|WS_TABSTOP}
};//运行出现的最后一行，WEDGE式楔入的意思，第一列数字是距最左端的距离，704是距顶端距离，128是每个块的横向长度，64是高度

//子窗口菜单列表
CtlWndStruct mainChildWnds[]={
	{WS_EX_CLIENTEDGE,TEXT("MANUALWND"),IDC_MANUAL,TEXT(""),0,320,1024,334,WS_CHILD|SS_NOTIFY|WS_BORDER},
	{WS_EX_CLIENTEDGE,TEXT("AUTOWND"),IDC_AUTO,TEXT(""),0,320,1024,334,WS_CHILD|SS_NOTIFY|WS_BORDER},
	{WS_EX_CLIENTEDGE,TEXT("EDITWND"),IDC_EDIT,TEXT(""),0,0,1024,654,WS_CHILD|SS_NOTIFY|WS_BORDER},
	{WS_EX_CLIENTEDGE,TEXT("MDIWND"),IDC_MDI,TEXT(""),0,0,1024,654,WS_CHILD|SS_NOTIFY|WS_BORDER},
	{WS_EX_CLIENTEDGE,TEXT("STATUSWND"),IDC_STATUSDISP,TEXT(""),0,0,1024,320,WS_CHILD|SS_NOTIFY|WS_BORDER},
	{WS_EX_CLIENTEDGE,TEXT("CONDITIONTWND"),IDW_CONDT,TEXT(""),0,0,1024,514,WS_CHILD|SS_NOTIFY|WS_BORDER},//by lsw 2010-3-24
	{WS_EX_CLIENTEDGE,TEXT("CONDITIONBWND"),IDW_CONDB,TEXT(""),0,514,1024,140,WS_CHILD|SS_NOTIFY|WS_BORDER},//by lsw 2010-3-24
	{WS_EX_CLIENTEDGE,TEXT("NCSetB"),IDW_NCSETB,TEXT(""),0,0,1024,654,WS_CHILD|SS_NOTIFY|WS_BORDER},//by lsw 2010-4-7
	{WS_EX_CLIENTEDGE,TEXT("MAINTION"),IDW_MAINTION_MAIN,TEXT(""),0,0,1024,654,WS_CHILD|SS_NOTIFY|WS_BORDER},//by lsw 2010-4-19
	{WS_EX_CLIENTEDGE,TEXT("EXPERT_SYSTEM"),IDC_EXPERT_SYSTEM,TEXT(""),0,0,1024,654,WS_CHILD|SS_NOTIFY|WS_BORDER}//by lsw 2010-4-19
};//子窗口，第一列距左端距离，第二列距顶端距离，第三列长度，第四列高度


//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释: 
//
//    仅当希望在已添加到 Windows 95 的
//    “RegisterClassEx”函数之前此代码与 Win32 系统兼容时，
//    才需要此函数及其用法。调用此函数
//    十分重要，这样应用程序就可以获得关联的
//   “格式正确的”小图标。
//
int MyRegisterClassMain(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;//定义窗口类结构体对象
    //填充结构体中各成员
	wcex.cbSize = sizeof(WNDCLASSEX); //?????

	wcex.style			= CS_HREDRAW | CS_VREDRAW;//客户区移动或其宽度/高度改变时重画客户区
	wcex.lpfnWndProc	= (WNDPROC)mainWndProc;//指向窗口过程的长指针
	wcex.cbClsExtra		= 0;//指定窗口类结构体存储区之后额外开辟的字节，系统初始化这些字节地址为0
	wcex.cbWndExtra		= 0;//指定窗口实例额外开辟的字节数
	wcex.hInstance		= hInstance;//指向当前窗口过程所在的程序实例句柄
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDC_EDM_NC_PROGRAM);//下载图标
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);//鼠标
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);//白色背景画刷
	wcex.lpszMenuName	= NULL;//(LPCTSTR)ID_Menu;//菜单，windowsCE不支持
	wcex.lpszClassName	= szWindowClass;//指向一个字符串常量，指定自定义窗口类的名称
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);//？？？？
//注册主窗口类
	if(!RegisterClassEx(&wcex)) return 0;
//注册子窗口类

	if(!MyRegisterClassManual(hInstance)) return 0;
	if(!MyRegisterClassAuto(hInstance)) return 0;
	if(!MyRegisterClassMdi(hInstance)) return 0;
	if(!MyRegisterClassEdit(hInstance)) return 0;
	if(!MyRegisterClassStatus(hInstance)) return 0;
	if(!MyRegisterClassConditionT(hInstance)) return 0;//by lsw 2010-3-24
	if(!MyRegisterClassConditionB(hInstance)) return 0;//by lsw 2010-3-24
	if(!MyRegisterClassNCSetB(hInstance)) return 0;//by lsw 2010-4-7
	if(!MyRegisterClassMaintion(hInstance))return 0;//by lsw 2010-4-19
	if(!MyRegisterClassEXPERT(hInstance))return 0;
	return 1;
}


//        创建和显示主程序窗口。
//
int showMainWnd(HINSTANCE hInstance,LPTSTR lpCmdLine,int nCmdShow)
{
	HWND hWnd;//窗口句柄
	DWORD dwThreadID;		//2010-4-17
	LPCmdThreadParam  pData; //2010-4-17
	hInst = hInstance; // 将实例句柄存储在全局变量中
  // hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
   //   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
//2010-4-17
   isEndScreen=0;
   CloseHandle(ScreenThread);	
   pData = (LPCmdThreadParam)HeapAlloc(GetProcessHeap(),
											HEAP_ZERO_MEMORY,
											sizeof(CmdThreadParam)
											);
   if(pData == NULL)
   {
	   return 1;
   }
	pData->notifyCode = 0;
	ScreenThread = CreateThread(
			NULL,
			0,
			ScreenThreadProc,
			pData,
			CREATE_SUSPENDED,
			&dwThreadID
			);
	if( ScreenThread==NULL)
	{
		return 1;
	}
	ResumeThread(ScreenThread);
	Sleep(500);//等待0.5s以让线程有足够的时间完成程序的初始化工作
//2010-4-17	
	//创建主窗口
	hWnd = CreateWindow(szWindowClass,
		"WEDM--NC", 
		WS_VISIBLE | WS_BORDER | WS_CAPTION | WS_SYSMENU,
		0, 0, 1024, 788,
		NULL, 
		NULL,
		hInstance,
		NULL);/*窗口类名，窗口名，窗口初始状态可见，创建具有细线边界的窗口，具有标题，创建在非客户区具有close的窗口，
			   x,y,宽，高，指向父窗口的句柄，指向控件或子窗口的唯一整型ID号，指向程序应用实例句柄,
			   响应WM_create消息时窗口正在创建过程中，此时可使用该指针返回的 lparam参数值，该值包含了要创建窗口的位置大小等信息*/

		 //如果不成立，返回0
	if (!hWnd)      return  0;//省略形式，（Iswindow(hWnd)过程判断窗口是否存在）

	//显示窗口，并刷新
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
//2010-4-17
	isEndScreen=1;
	SendMessage(hScreenEdit,EM_SETSEL,-1,-1);
	SendMessage(hScreenEdit,EM_REPLACESEL,TRUE,(LPARAM)"Start\r\n");
//2010-4-17
	return 1;

}

DWORD WINAPI ScreenThreadProc(LPVOID lpParam)
{
	DialogBox(hInst,MAKEINTRESOURCE(IDD_SCREEN),NULL,screenDlgProc);//2010-4-17
	return 0;
}

//
//  函数: WndProc(HWND, unsigned, WORD, LONG)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
//主窗口过程体
//主窗口过程原型，窗口过程必须为回调函数
LRESULT CALLBACK mainWndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;

	//查找对应的消息ID,并执行对应的消息处理过程
	for(i=0; i<dim(mainMessages);i++)
	{
		if(msgCode == mainMessages[i].uCode) return(*mainMessages[i].functionName)(hWnd,msgCode,wParam,lParam);
	}

	
	//对不存在的消息，调用缺省窗口过程
	return DefWindowProc(hWnd, msgCode, wParam, lParam);
}

LRESULT goCreateMain(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		
    int i;
	
//	IinitProgram(hWnd);  //整个数控程序软硬件的初始化
	
//创建主窗口菜单
	for(i=0;i<dim(mainMenuBtns);i++){
		CreateWindowEx(mainMenuBtns[i].dwExStyle,
			mainMenuBtns[i].szClass,
			mainMenuBtns[i].szTitle,
			mainMenuBtns[i].lStyle,
			mainMenuBtns[i].x,
			mainMenuBtns[i].y,
			mainMenuBtns[i].cx,
			mainMenuBtns[i].cy,
			hWnd,
			(HMENU)mainMenuBtns[i].nID,
			hInst,
			NULL
			);
	}

//创建子窗口
	for(i=0;i<dim( mainChildWnds);i++){
		CreateWindowEx(mainChildWnds[i].dwExStyle,
			mainChildWnds[i].szClass,
			mainChildWnds[i].szTitle,
			mainChildWnds[i].lStyle,
			mainChildWnds[i].x,
			mainChildWnds[i].y,
			mainChildWnds[i].cx,
			mainChildWnds[i].cy,
			hWnd,
			(HMENU)mainChildWnds[i].nID,
			hInst,
			NULL
			);
	}
    


	//显示手动窗口
	ShowWindow(GetDlgItem(hWnd,IDC_MANUAL),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_STATUSDISP),SW_SHOW);
		
	 //电源按钮检测时间设定400ms
	SetTimer(hWnd,TIMER_MAIN,400,NULL);

	//
	hbm1=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP5)); 	
	hbm2=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP6)); 
	

	//
	return 0;

}

LRESULT goCommandMain(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		//定义菜单ID号和通知单
		int menuID, notifyCode;
		
		//定义窗口句柄
		HWND wndCtrl;
		 int i;

		 
		menuID    = LOWORD(wParam); 
		notifyCode = HIWORD(wParam); 
		wndCtrl = (HWND) lParam;
	   

		//显示被选中的子窗口，其他子窗口隐藏//
		for(i=0; i<dim(mainCommands);i++)
		{
			if(menuID == mainCommands[i].uCode) return(*mainCommands[i].functionName)(hWnd,wndCtrl,menuID,notifyCode);
		}
			
		return 0;

}
LRESULT goTimerMain(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{

	//以下建立检测电源按钮及处理函数


     //stoppow=inb(ADR_INA);           //关机
/*	
	if((stoppow&0x04)==0x0)
                                {
                                        ioperm(ADR_OUTB,1,1);
                                        outb(0x0,ADR_OUTB);
                                        ioperm(ADR_OUTB,1,0);

                    /* Machine Poweroff */
                    /* copied from IDC_BTNCLOSE */
              /*      nPowerStat = 0;
                    MachineSave(HWND hWnd);
                    ioperm(ADR_SRV_ON, ADR_SRV_INH-ADR_SRV_ON+1, 1);
                    outb(0x0, ADR_SRV_ON); 
                    outb(0x0, ADR_SRV_CWL);
                    outb(0x0, ADR_SRV_CCWL); 
                    outb(0x0, ADR_SRV_INH);
                    ioperm(ADR_SRV_ON, ADR_SRV_INH-ADR_SRV_ON+1, 0);
    
                    KillTimer(hwnd,IDC_STIMER);
                        KillTimer(hwnd,IDC_TIMER);
                                DestroyMainWindow(hwnd);
                                MainWindowCleanup(hwnd);
                        DestroyMainWindow(hwnd);
                        PostQuitMessage(hwnd);
*/
/*
    unsigned int limit;
	char chTemp1[50];
	iowrite(hWnd,IOPCIhDevice,IO_FPGA_WRITE, ADR_OUTB,  0x0);
   	limit =ioread(hWnd,IOPCIhDevice,IO_FPGA_READ, ADR_OUTB );
	wsprintf(chTemp1, " %d",limit);
    MessageBox(hWnd,chTemp1,NULL,NULL);
*/

	return 0;
}


LRESULT goPaintMain(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		PAINTSTRUCT ps;
		HDC hdc;
		
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		return 0;
}

LRESULT goDestroyMain(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//2010-4-1   下面代码用于在关闭程序时候将temp.txt文件内容复制到condition.csv文件中，在复制过程中忽略空格以减小condition.csv文件大小
	FILE* fp,* fpp;
	int c;
	fclose(fpTemp);
	fclose(fpSeek);
	fclose(fpLock);
	fclose(fpSet);
	fclose(fpMac);
	fclose(fpSys);
	fclose(fpEIO);
	fclose(fpcoor);
	fclose(fpEIO_Address);
	fp=fopen("condition.csv","w");
	fpp=fopen("temp.txt","r");
	while((c=getc(fpp))!=EOF)
	{
		if(c==int(' '))continue;//忽略空格
		putc(c,fp);//复制condition.csv第一行至tampa.txt
	}
	putc(c,fp);
	fclose(fpp);
	fclose(fp);
	//2010-4-1
	PostQuitMessage(0);
	return 0;
}


// “关于”框的消息处理程序。
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG://WM_INITDIALOG消息响应函数是在程序运行时，当其对话框和子控件全部创建完毕，将要显示内容的时候发送的消息。
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}


//手动按钮执行过程
LRESULT goMainManualCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{ 

	 ShowWindow( GetDlgItem(hWnd,IDC_AUTO),SW_HIDE);//显示窗口，隐藏或可见，隐藏返回0，可见返回1
     ShowWindow( GetDlgItem(hWnd,IDC_EDIT),SW_HIDE);//GetDlgItem获取对话框中子窗口控件的句柄,hWnd对话框的句柄	
     ShowWindow( GetDlgItem(hWnd,IDC_MDI),SW_HIDE);
	 ShowWindow(GetDlgItem(hWnd,IDC_MANUAL),SW_SHOW);
	 ShowWindow(GetDlgItem(hWnd,IDC_STATUSDISP),SW_SHOW);
	 ShowWindow(GetDlgItem(hWnd,IDW_CONDT),SW_HIDE);//by lsw 2010-3-24
	 ShowWindow(GetDlgItem(hWnd,IDW_CONDB),SW_HIDE);//by lsw 2010-3-24
	 ShowWindow(GetDlgItem(hWnd,IDW_NCSETB),SW_HIDE);//by lsw 2010-4-7
	 ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_MAIN),SW_HIDE);//by lsw 2010-4-19
	 ShowWindow(GetDlgItem(hWnd,IDC_EXPERT_SYSTEM),SW_HIDE);
	 UpdateWindow(GetDlgItem(hWnd,IDC_MANUAL));
	 
	 return 0;
	 
}

//自动按钮执行过程
LRESULT goMainAutoCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)	 
{
   
	ShowWindow( GetDlgItem(hWnd,IDC_MANUAL),SW_HIDE);
	 ShowWindow( GetDlgItem(hWnd,IDC_EDIT),SW_HIDE);	
     ShowWindow( GetDlgItem(hWnd,IDC_MDI),SW_HIDE);
	 ShowWindow(GetDlgItem(hWnd,IDC_AUTO),SW_SHOW);
	 ShowWindow(GetDlgItem(hWnd,IDC_STATUSDISP),SW_SHOW);
	 ShowWindow(GetDlgItem(hWnd,IDW_CONDT),SW_HIDE);//by lsw 2010-3-24
     ShowWindow(GetDlgItem(hWnd,IDW_CONDB),SW_HIDE);//by lsw 2010-3-24
	 ShowWindow(GetDlgItem(hWnd,IDW_NCSETB),SW_HIDE);//by lsw 2010-4-7
	 ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_MAIN),SW_HIDE);//by lsw 2010-4-19
	 ShowWindow(GetDlgItem(hWnd,IDC_EXPERT_SYSTEM),SW_HIDE);
	 UpdateWindow(GetDlgItem(hWnd,IDC_AUTO));
	 
	
	 return 0;

}
//编辑按钮执行过程
LRESULT goMainEditCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	 
	 ShowWindow( GetDlgItem(hWnd,IDC_MANUAL),SW_HIDE);
	 ShowWindow( GetDlgItem(hWnd,IDC_AUTO),SW_HIDE);	
     ShowWindow( GetDlgItem(hWnd,IDC_MDI),SW_HIDE);
	 ShowWindow(GetDlgItem(hWnd,IDC_STATUSDISP),SW_HIDE);
	 ShowWindow(GetDlgItem(hWnd,IDC_EDIT),SW_SHOW);
	 ShowWindow(GetDlgItem(hWnd,IDW_CONDT),SW_HIDE);//by lsw 2010-3-24
     ShowWindow(GetDlgItem(hWnd,IDW_CONDB),SW_HIDE);//by lsw 2010-3-24
	 ShowWindow(GetDlgItem(hWnd,IDW_NCSETB),SW_HIDE);//by lsw 2010-4-7
	 ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_MAIN),SW_HIDE);//by lsw 2010-4-19
	 ShowWindow(GetDlgItem(hWnd,IDC_EXPERT_SYSTEM),SW_HIDE);
	 UpdateWindow(GetDlgItem(hWnd,IDC_EDIT));

     InitpNoteInfo(wndCtrl);
	 return 0;
}
//MDI按钮执行过程
LRESULT goMainMdiCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	 
   	 ShowWindow(  GetDlgItem(hWnd,IDC_MANUAL),SW_HIDE);
	 ShowWindow( GetDlgItem(hWnd,IDC_AUTO),SW_HIDE);	
     ShowWindow( GetDlgItem(hWnd,IDC_EDIT),SW_HIDE);
	 ShowWindow(GetDlgItem(hWnd,IDC_MDI),SW_SHOW);
	 ShowWindow(GetDlgItem(hWnd,IDC_STATUSDISP),SW_SHOW);
	hWnd1 = GetDlgItem(hWnd,IDC_STATUSDISP);
	 ShowWindow(GetDlgItem(hWnd,IDW_CONDT),SW_HIDE);//by lsw 2010-3-24
     ShowWindow(GetDlgItem(hWnd,IDW_CONDB),SW_HIDE);//by lsw 2010-3-24
	 ShowWindow(GetDlgItem(hWnd,IDW_NCSETB),SW_HIDE);//by lsw 2010-4-7
	 ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_MAIN),SW_HIDE);//by lsw 2010-4-19
	 ShowWindow(GetDlgItem(hWnd,IDC_EXPERT_SYSTEM),SW_HIDE);
	 UpdateWindow(GetDlgItem(hWnd,IDC_MDI));

	
	 return 0;
}


//CONDITION按钮执行过程 by lsw 2010-3-24
LRESULT goMainConditionCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	 
	 ShowWindow(  GetDlgItem(hWnd,IDC_MANUAL),SW_HIDE);
	 ShowWindow( GetDlgItem(hWnd,IDC_AUTO),SW_HIDE);	
     ShowWindow( GetDlgItem(hWnd,IDC_EDIT),SW_HIDE);
	 ShowWindow(GetDlgItem(hWnd,IDC_MDI),SW_HIDE);
	 ShowWindow(GetDlgItem(hWnd,IDC_STATUSDISP),SW_HIDE);
	 ShowWindow(GetDlgItem(hWnd,IDW_CONDT),SW_SHOW);
     ShowWindow(GetDlgItem(hWnd,IDW_CONDB),SW_SHOW);
	 ShowWindow(GetDlgItem(hWnd,IDW_NCSETB),SW_HIDE);//by lsw 2010-4-7
	 ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_MAIN),SW_HIDE);//by lsw 2010-4-19
	 ShowWindow(GetDlgItem(hWnd,IDC_EXPERT_SYSTEM),SW_HIDE);
	 UpdateWindow(GetDlgItem(hWnd,IDW_CONDT));
	 UpdateWindow(GetDlgItem(hWnd,IDW_CONDB));
	
	 return 0;
}
//NCSet按钮执行过程 by lsw 2010-4-7
LRESULT goMainStartStopCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	 
	 ShowWindow(  GetDlgItem(hWnd,IDC_MANUAL),SW_HIDE);
	 ShowWindow( GetDlgItem(hWnd,IDC_AUTO),SW_HIDE);	
     ShowWindow( GetDlgItem(hWnd,IDC_EDIT),SW_HIDE);
	 ShowWindow(GetDlgItem(hWnd,IDC_MDI),SW_HIDE);
	 ShowWindow(GetDlgItem(hWnd,IDC_STATUSDISP),SW_HIDE);
	 ShowWindow(GetDlgItem(hWnd,IDW_CONDT),SW_HIDE);
     ShowWindow(GetDlgItem(hWnd,IDW_CONDB),SW_HIDE);
	 ShowWindow(GetDlgItem(hWnd,IDW_NCSETB),SW_SHOW);//by lsw 2010-4-7
	 ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_MAIN),SW_HIDE);//by lsw 2010-4-19
	 ShowWindow(GetDlgItem(hWnd,IDC_EXPERT_SYSTEM),SW_HIDE);
	 UpdateWindow(GetDlgItem(hWnd,IDW_NCSETB));// by lsw 2010-4-7
	
	 return 0;
}
//MAINTION按钮执行过程
LRESULT goMainResetCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	ShowWindow(  GetDlgItem(hWnd,IDC_MANUAL),SW_HIDE);
	ShowWindow( GetDlgItem(hWnd,IDC_AUTO),SW_HIDE);	
    ShowWindow( GetDlgItem(hWnd,IDC_EDIT),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_MDI),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_STATUSDISP),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDW_CONDT),SW_HIDE);
    ShowWindow(GetDlgItem(hWnd,IDW_CONDB),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDW_NCSETB),SW_HIDE);//by lsw 2010-4-7
	ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_MAIN),SW_SHOW);//by lsw 2010-4-19
	ShowWindow(GetDlgItem(hWnd,IDC_EXPERT_SYSTEM),SW_HIDE);
	UpdateWindow(GetDlgItem(hWnd,IDW_MAINTION_MAIN));// by lsw 2010-4-19
	
	 return 0;
}

LRESULT goExpertSystemCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	ShowWindow(GetDlgItem(hWnd,IDC_MANUAL),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO),SW_HIDE);	
    ShowWindow(GetDlgItem(hWnd,IDC_EDIT),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_MDI),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_STATUSDISP),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDW_CONDT),SW_HIDE);
    ShowWindow(GetDlgItem(hWnd,IDW_CONDB),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDW_NCSETB),SW_HIDE);//by lsw 2010-4-7
	ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_MAIN),SW_HIDE);//by lsw 2010-4-19
	ShowWindow(GetDlgItem(hWnd,IDC_EXPERT_SYSTEM),SW_SHOW);
	UpdateWindow(GetDlgItem(hWnd,IDC_EXPERT_SYSTEM));// by lsw 2010-4-19
	
	 return 0;
}

void InitpNoteInfo(HWND hWnd)
{
	TCHAR currentpath [MAX_PATH + 1];
	
	GetCurrentDirectory(MAX_PATH+1,currentpath); 
	
    
        //if(pNoteInfo == NULL)
	if(!(pNoteInfo = (PNOTEINFO)malloc(sizeof(NOTEINFO))))
	{	MessageBox(hWnd,"can not malloc pNoteInfo in InitpNoteInfo function",NULL,NULL);
		return ; //error!!
	}
        pNoteInfo->ischanged = FALSE;
        strcpy(pNoteInfo->fileName , "untitled.txt");
        strcpy(pNoteInfo->filePath , currentpath);
	if (pNoteInfo->filePath [strlen (pNoteInfo->filePath) - 1] != '\\')
        	strcat (pNoteInfo->filePath, "\\");
        pNoteInfo->fileSize = 0;
        pNoteInfo->Buffer = NULL;
        pNoteInfo->hMLEditWnd = NULL;
} 

int MachineSave(HWND hWnd)    //Save the machine postion before the program is over
{

    char data[51][_MAX_FNAME]={"","","","",""};
	int i,j;
	int MachineFile;

	MachineFile=_open("MachinePos.txt",O_WRONLY);
	
	if(MachineFile==-1)
	{
		MessageBox(hWnd,"MachinePos.txt open error when write the file in fuction MachinePosSave ",NULL,NULL);
		  return 1;
	}

	 //保存绝对坐标 
	sprintf_s( data[0], "%.3f", data_m.x );
	sprintf_s( data[1], "%.3f", data_m.y );
	sprintf_s( data[2], "%.3f", data_m.z );
	sprintf_s( data[3], "%.3f", data_m.b );
	sprintf_s( data[4], "%.3f", data_m.c );
	
		 
	for(i=0;i<5;i++)
	{
		j=_write(MachineFile,data[i],strlen(data[i]));
	
		if(j != strlen(data[i]))
		{
			 MessageBox(hWnd,"MachineFile write error in fuction MachinePosSave ",NULL,NULL);
			 return 1;
		}	
		_write(MachineFile,"\n",1);
	}	
	
	//保存工作坐标
	sprintf_s( data[0], "%.3f", data_r.x );
	sprintf_s( data[1], "%.3f", data_r.y );
	sprintf_s( data[2], "%.3f", data_r.z );
	sprintf_s( data[3], "%.3f", data_r.b );
	sprintf_s( data[4], "%.3f", data_r.c );	
		 
	for(i=0;i<5;i++)
	{
		j=_write(MachineFile,data[i],strlen(data[i]));
	
		if(j != strlen(data[i]))
		{
			 MessageBox(hWnd,"MachineFile write error in fuction MachinePosSave ",NULL,NULL);
			 return 1;
		}	
		_write(MachineFile,"\n",1);
	}

	//保存加工文件名
	j=_write(MachineFile,AutoMachineFileName,strlen(AutoMachineFileName));
	
	if(j != strlen(AutoMachineFileName))
	{
			 MessageBox(hWnd,"MachineFile write error in fuction MachinePosSave ",NULL,NULL);
			 return 1;
	}
	
	_write(MachineFile,"\n",1);

	//保存加工参数
	
	sprintf_s( data[0], "%d", Machiningparam.eNo );  //保存条件号
	
	j=_write(MachineFile,data[0],strlen(data[0]));
	
	if(j != strlen(data[0]))
	{
			 MessageBox(hWnd,"MachineFile write error in fuction MachinePosSave ",NULL,NULL);
			 return 1;
	}
	
	_write(MachineFile,"\n",1);
	
	
	for(i=0;i<30;i++)
	{                                                ////保存各加工条件值
        sprintf_s( data[i], "%d", Machiningparam.item[i].value );
		j=_write(MachineFile,data[i],strlen(data[i]));
	
		if(j != strlen(data[i]))
		{
			 MessageBox(hWnd,"MachineFile write error in fuction MachinePosSave ",NULL,NULL);
			 return 1;
		}	
		_write(MachineFile,"\n",1);	
	}
	//保存加工参数库当前的状态
	//by lsw 2010-4-6
	for(i=9;i>=1;i--)
	{
		startR=seekPre(startR);
		if(startR==0)startR=seekPre(startR);
	}			
	sprintf_s( data[0], "%d", startR ); 
	j=_write(MachineFile,data[0],strlen(data[0]));
	if(j != strlen(data[0]))
	{
			 MessageBox(hWnd,"MachineFile write error in fuction MachinePosSave ",NULL,NULL);
			 return 1;
	}	
	_write(MachineFile,"\n",1);
	// by lsw 2010-4-6

	if(_close(MachineFile))
	{
		MessageBox(hWnd,"MachinePos.txt close error when whiting the file in fuction MachinePosSave ",NULL,NULL);
		return 1;
	}
	return 0;
}  
int DspProgramLoad(HWND hWnd)
{
	BOOL bResult;
	DWORD junk;       
		// DSP程序下传
		
		bResult = DeviceIoControl(NCPCIhDevice,  // device to be queried
					NC_HPIBOOTLOAD,  // operation to perform
                    (LPVOID)&code, sizeof(code), // no input buffer
                    NULL,0,     // output buffer
                     &junk,                 // # bytes returned
                     (LPOVERLAPPED)NULL);  // synchronous I/O
		
		if(bResult==FALSE)
		{
				MessageBox(hWnd,"can not get i/o data in fuction HpiLoad", NULL,NULL);
				return 1;
		}
		
	
		return 0;
}

int DspProgramLoad1(HWND hWnd)
{
	BOOL bResult;
	DWORD junk;       
		// DSP程序下传
		
		bResult = DeviceIoControl(PowerPCIhDevice,  // device to be queried
					Power_HPIBOOTLOAD,  // operation to perform
                    (LPVOID)&code, sizeof(code), // no input buffer
                    NULL,0,     // output buffer
                     &junk,                 // # bytes returned
                     (LPOVERLAPPED)NULL);  // synchronous I/O
		
		if(bResult==FALSE)
		{
				MessageBox(hWnd,"can not get i/o data in fuction HpiLoad", NULL,NULL);
				return 1;
		}
		
	
		return 0;
}







unsigned int  ioread(HWND hWnd,HANDLE hDvice,DWORD ctlword,  DWORD adr )
{
	BOOL bResult;                 // results flag
	DWORD junk;
	unsigned int returnbuf;                   // discard results
	LPIOstru pData; 


	pData = (LPIOstru)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,	sizeof(IOstru));
	if(pData == NULL)
			{
				//MessageBox(hWnd,"can not alloc heapmemory in function ioread",NULL,NULL);
				return 1;
	}

	pData->adr = adr;
	pData->val = 0;

	bResult = DeviceIoControl(hDvice,  // device to be queried
					ctlword,  // operation to perform
                    pData, sizeof(IOstru), // no input buffer
                    pData, sizeof(IOstru),     // output buffer
                     &junk,                 // # bytes returned
                     (LPOVERLAPPED) NULL);  // synchronous I/O
	if(bResult==FALSE)
	{
			//MessageBox(hWnd,"can not get i/o data in fuction ioread", NULL,NULL);
			return 1;
	}

	returnbuf = pData->val;

	if(HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pData) == 0){
		MessageBox(hWnd,"can not free heapmemory in function iowrite",NULL,NULL);
		return 1;
	}
	
	return returnbuf;
}

LRESULT iowrite(HWND hWnd,HANDLE hDvice,DWORD ctlword,  DWORD adr, DWORD buffer)
{
	BOOL bResult;                 // results flag
	DWORD junk;                   // discard results
	LPIOstru pData; 


	pData = (LPIOstru)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,	sizeof(IOstru));
	if(pData == NULL)
			{
				//MessageBox(hWnd,"can not alloc heapmemory in function iowrite",NULL,NULL);
				return 1;
	}
	
	pData->adr = adr;
	pData->val = buffer;
		
	bResult = DeviceIoControl(hDvice,  // device to be queried
					ctlword,  // operation to perform
                    pData,sizeof(IOstru), // no input buffer
                    NULL,0,     // output buffer
                     &junk,                 // # bytes returned
                     (LPOVERLAPPED) NULL);  // synchronous I/O
	
	if(HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pData) == 0){
		MessageBox(hWnd,"can not free heapmemory in function iowrite",NULL,NULL);
		return 1;
	}
	
	
	if(bResult==FALSE)
	{
			//MessageBox(hWnd,"can not get i/o data in fuction iowrite", NULL,NULL);
			return 1;
	}
	return 0;
}

int SendNCDriverUserDecodeEvent(HWND hWnd,HANDLE hEvent)
{
		
		BOOL bResult;
		DWORD junk;
	
		       
		// 向NCDIVER下传译码线程事件
		
		bResult = DeviceIoControl(NCPCIhDevice,  // device to be queried
                  NC_TRANSMIT_EVENT,  // operation to perform
                    &hEvent, sizeof(hEvent), // no input buffer
                    NULL,0,     // output buffer
                     &junk,                 // # bytes returned
                     (LPOVERLAPPED) NULL);  // synchronous I/O
	
		if(bResult==FALSE)
		{
				MessageBox(hWnd,"can not get i/o data in fuction SendNCDriverUserDecodeEvent", NULL,NULL);
				return 1;
		}
			
		
	
		return 0;
}


//
LRESULT goDrawitemMain(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
    LPDRAWITEMSTRUCT lpdis; 
	lpdis = (LPDRAWITEMSTRUCT) lParam; 
    hdc = CreateCompatibleDC(lpdis->hDC);
	if (lpdis->itemState & ODS_FOCUS)  // if selected 
		SelectObject(hdc, hbm2); 
	else 
		SelectObject(hdc, hbm1); 
	StretchBlt( 
                lpdis->hDC,         // destination DC 
                lpdis->rcItem.left, // x upper left 
                lpdis->rcItem.top,  // y upper left  
                // The next two lines specify the width and 
                // height. 
                lpdis->rcItem.right - lpdis->rcItem.left, 
                lpdis->rcItem.bottom - lpdis->rcItem.top, 
                hdc,    // source device context 
                0, 0,      // x and y upper left 
                57,        // source bitmap width 
                30,        // source bitmap height 
                SRCCOPY);  // raster operation 
	SetBkMode(lpdis->hDC,TRANSPARENT);	
	switch(lpdis->CtlID)
	{
	case IDC_BTNMANUEL:
		DrawText(lpdis->hDC,"MANUAL",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNAUTO:
		DrawText(lpdis->hDC,"AUTO",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNEDIT:
		DrawText(lpdis->hDC,"EDIT",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNMDI:
		DrawText(lpdis->hDC,"MDI",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNREST:
		DrawText(lpdis->hDC,"MAINTAIN",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNLIM:
		DrawText(lpdis->hDC,"CONDITION",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNCLOSE:
		DrawText(lpdis->hDC,"CTLKEY",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_StartStop:
		DrawText(lpdis->hDC,"NCSET",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
/////////////////////////////////
	case IDC_SELCON:
		DrawText(lpdis->hDC,"Machining Condition",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_SELSTATE:
		DrawText(lpdis->hDC,"Machining State",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BUILDMACHING:
		DrawText(lpdis->hDC,"Build Machining",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
/////////////////////////////
	case IDC_BTNYZ:
		DrawText(lpdis->hDC,"Y-Z",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNXY:
		DrawText(lpdis->hDC,"X-Y",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNXZ:
		DrawText(lpdis->hDC,"X-Z",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNNEW:
		DrawText(lpdis->hDC,"NEW",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNOPEN:
		DrawText(lpdis->hDC,"OPEN",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNSAVE:
		DrawText(lpdis->hDC,"SAVE",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNEDITCLOSE:
		DrawText(lpdis->hDC,"CLOSE",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNDELET:
		DrawText(lpdis->hDC,"DELET",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNDRAW:
		DrawText(lpdis->hDC,"DRAW",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNGRAPHRESET:
		DrawText(lpdis->hDC,"RESET",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
///////////////////////////////////////
	case IDC_AXISALLZERO:
		DrawText(lpdis->hDC,"ALL",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_AXISXZERO:
		DrawText(lpdis->hDC,"XZERO",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_AXISYZERO:
		DrawText(lpdis->hDC,"YZERO",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_AXISZZERO:
		DrawText(lpdis->hDC,"ZZERO",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_AXISBZERO:
		DrawText(lpdis->hDC,"BZERO",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_AXISCZERO:
		DrawText(lpdis->hDC,"CZERO",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNSTARTSTOP:
		DrawText(lpdis->hDC,"START",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNRESET:
		DrawText(lpdis->hDC,"RESET",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
		
	case IDC_BTNSUSPEND:
		DrawText(lpdis->hDC,"SUSPEND",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
////////////////////////////////////////
	case IDC_MachAllReturn:
		DrawText(lpdis->hDC,"MachAllRet",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_MachXReturn:
		DrawText(lpdis->hDC,"MachXRet",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_MachYReturn:
		DrawText(lpdis->hDC,"MachYRet",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_MachZReturn:
		DrawText(lpdis->hDC,"MachZRet",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_MachBReturn:
		DrawText(lpdis->hDC,"MachBRet",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_MachCReturn:
		DrawText(lpdis->hDC,"MachCRet",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_WorkAllReturn:
		DrawText(lpdis->hDC,"WorkAllRet",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_WorkXReturn:
		DrawText(lpdis->hDC,"WorkXRet",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_WorkYReturn:
		DrawText(lpdis->hDC,"WorkYRet",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_WorkZReturn:
		DrawText(lpdis->hDC,"WorkZRet",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
///////////////////////////////////////
	case IDC_WorkBReturn:
		DrawText(lpdis->hDC,"WorkBRet",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_WorkCReturn:
		DrawText(lpdis->hDC,"WorkCRet",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_XTouchP:
		DrawText(lpdis->hDC,"X+",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_YTouchP:
		DrawText(lpdis->hDC,"Y+",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_ZTouchP:
		DrawText(lpdis->hDC,"Z+",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTouchP:
		DrawText(lpdis->hDC,"B+",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_CTouchP:
		DrawText(lpdis->hDC,"C+",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_XTouchM:
		DrawText(lpdis->hDC,"X-",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_YTouchM:
		DrawText(lpdis->hDC,"Y-",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_ZTouchM:
		DrawText(lpdis->hDC,"Z-",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTouchM:
		DrawText(lpdis->hDC,"B-",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_CTouchM:
		DrawText(lpdis->hDC,"C-",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
		}
	DeleteDC(hdc);
    return TRUE; 
}