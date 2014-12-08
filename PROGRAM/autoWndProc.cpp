#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

#define Z_Return_Center 0.02
const double DELT = 0.001; 
const double JUMP_DELT = 0.01;

extern HINSTANCE hInst;								// 当前实例
extern ConditionStru Machiningparam; 
extern CtlWndStruct autoParamMenuBtns[];
extern int AutographFirstEnterFlag;  //自动图形显示函数第一次进入标志
extern int AutonccodeFirstEnterFlag;  //nc码显示函数第一次进入标志
extern int row;
extern int Para_End_Flag;
int M_Code_hTread_Flag = 0;

extern HANDLE IOPCIhDevice;               // IO_PCI设备句柄 
extern HANDLE NCPCIhDevice;                // NC_PCI设备句柄 
extern HANDLE hDecodeEvent;  //启动向下位机传送译码数据线程事件的句柄 
//2010-4-1
extern HWND hAUTOLV;
extern void fn_auto_setLV(HWND);
extern void setItem(int*,int);
extern int fnSearch(HWND,int);
extern int getCond(int,int);
extern int NOWSELECTLINE;
//end 2010-4-1
//2010-5-26锥加工用参数
extern double z_position;
extern double xy_plane;
extern double h_thickness;
double HLOWDISTANCE = 50.0;
//end 2010-5-26
//2010-5-30
extern HBITMAP graph_hbm_up;
extern HBITMAP graph_hbm_up_click;
extern HBITMAP graph_hbm_right;
extern HBITMAP graph_hbm_right_click;
extern HBITMAP graph_hbm_down;
extern HBITMAP graph_hbm_down_click;
extern HBITMAP graph_hbm_left;
extern HBITMAP graph_hbm_left_click;
extern HBITMAP graph_hbm_zoom_in;
extern HBITMAP graph_hbm_zoom_in_click;
extern HBITMAP graph_hbm_zoom_out;
extern HBITMAP graph_hbm_zoom_out_click;
extern HBITMAP graph_hbm_turn_up;
extern HBITMAP graph_hbm_turn_up_click;
extern HBITMAP graph_hbm_turn_right;
extern HBITMAP graph_hbm_turn_right_click;
extern HBITMAP graph_hbm_turn_down;
extern HBITMAP graph_hbm_turn_down_click;
extern HBITMAP graph_hbm_turn_left;
extern HBITMAP graph_hbm_turn_left_click;
//end 2010-5-30
//2010-5-31
int auto_up_down;
int auto_right_left;
double auto_zoom;
int auto_turn_up_down;
int auto_turn_right_left;
int auto_move_step;
int auto_turn_step;
int AutoNowSelectID;
void Auto_Show2D(HWND);
void Auto_Show3D(HWND);
extern void Auto3DDraw();
//end 2010-5-31
int NowAutoGraph2D_3D;//2010-5-27
HANDLE AutohThread;
HANDLE NcSendhThread;
HANDLE hMachiningEndEvent;
HANDLE hM_Code_Event;
HANDLE hM_Code_EventExecutive;
HANDLE M_Code_hThread;

char AutoMachineFileName[_MAX_FNAME];
GraphParam AutoGraphParam;
int auto_regraph_flag=0;
int auto_nccode_flag =0;
LPNCCODE lpNcCodeMem;         //自动加工代码内存
int NcCodeNum = 0; //NC码数
int Autoreset_flag;
int DecodingNum;
int Recent_Row_Num;
nc_data ncdata;
M_data MChild[2*DECODE_NUM_ONCE];
nc_data decodeData[2*DECODE_NUM_ONCE];
nc_data JudgeData[2*DECODE_NUM_ONCE];


DWORD WINAPI AutoMachiningBuildThreadProc(LPVOID lpParam);
void AutoListProgramNameCmdThreadProc();
void AutoListConNameCmdThreadProc();
DWORD WINAPI NcSendThreadProc(LPVOID lpParam);
DWORD WINAPI M_Code_Proc(LPVOID lpParam);
void Show3DInput();//2010-5-31
extern void ListAutoParamThreadProc();
int MachiningParamRead(HWND hWnd,int ConNum);
int Load_NC_TapeParameter_Program(HWND hWnd);
extern inline double Gcode2d(char* s, const char* gcode);
DWORD Jump_Time_Compute(HWND hWnd,int DownTime,int Jump_Speed);
double Jump_max_speed;
double Jump_average_speed;
int Jump_Select_Center = 1;
DSP_Para_data DSP_data;
void DSP_Parameter_Transport(HWND hWnd);
extern int EXPERT;

//2010-6-24
int AutoInput;
LPCmdThreadParam  pDataInAutoInput;
//end
const MessageProc autoMessages[]={
	    WM_CREATE, goCreateAuto,
		WM_COMMAND, goCommandAuto,
		WM_KEYDOWN, goKeydownAuto,
		WM_PAINT,  goPaintAuto,
		WM_DRAWITEM, goDrawItemAuto,
		WM_DESTROY, goDestroyAuto
};

const CommandProc autoCommands[]={
	IDC_SELCON,                goAutoSelectConCmd,
	IDC_SELSTATE,              goAutoSelectStateCmd,
	IDC_NCPRONAMEIN,           goAutoListProgramNameCmd,
	IDC_BUILDMACHING,          goAutoMachiningBuildCmd,
	IDC_CONDNOIN,              goAutoListConNameCmd,
	
};

CtlWndStruct autoMenuBtns[]={
		{WS_EX_CLIENTEDGE,TEXT("LISTBOX"),IDC_NCPRONAMEIN,TEXT(""),100,10,100,30,WS_VISIBLE|WS_CHILD|WS_BORDER|WS_TABSTOP},//|ES_AUTOWRAP|ES_AUTOSELECT},
		{WS_EX_CLIENTEDGE,TEXT("LISTBOX"),IDC_CONDNOIN,TEXT(""),100,40,100,30,WS_VISIBLE|WS_CHILD|WS_BORDER|WS_TABSTOP},//|ES_AUTOWRAP|ES_AUTOSELECT},//????
        {WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_SELCON,TEXT("Machining Condition"),10,70,190,30,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP | BS_PUSHBUTTON|SS_CENTER},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_SELSTATE,TEXT("Machining State"),10,100,190,30,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP | BS_PUSHBUTTON|SS_CENTER},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BUILDMACHING,TEXT("Build Machining"),10,130,190,30,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP | BS_PUSHBUTTON|SS_CENTER},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BAUTO_3D_2D,TEXT(""),10,160,190,30,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP | BS_PUSHBUTTON|SS_CENTER},//2010-5-27
		//2010-5-30
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_AUTO_BUP,TEXT(""),28,212,30,18,WS_CHILD|WS_VISIBLE| BS_OWNERDRAW},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_AUTO_BLEFT,TEXT(""),10,230,18,30,WS_CHILD|WS_VISIBLE| BS_OWNERDRAW},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_AUTO_BDOWN,TEXT(""),28,260,30,18,WS_CHILD|WS_VISIBLE| BS_OWNERDRAW},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_AUTO_BRIGHT,TEXT(""),58,230,18,30,WS_CHILD|WS_VISIBLE| BS_OWNERDRAW},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_AUTO_BZOOM_IN,TEXT(""),20,300,30,30,WS_CHILD|WS_VISIBLE| BS_OWNERDRAW},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_AUTO_B_TURN_UP,TEXT(""),120,200,30,30,WS_CHILD|WS_VISIBLE| BS_OWNERDRAW},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_AUTO_B_TURN_LEFT,TEXT(""),90,230,30,30,WS_CHILD|WS_VISIBLE| BS_OWNERDRAW},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_AUTO_B_TURN_DOWN,TEXT(""),120,260,30,30,WS_CHILD|WS_VISIBLE| BS_OWNERDRAW},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_AUTO_B_TURN_RIGHT,TEXT(""),150,230,30,30,WS_CHILD|WS_VISIBLE| BS_OWNERDRAW},
		{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_AUTO_EZOOM,TEXT(""),50,300,50,30,WS_VISIBLE|WS_BORDER|WS_CHILD|ES_RIGHT|ES_NOHIDESEL},
		{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_AUTO_E_MOVE,TEXT(""),28,230,30,30,WS_VISIBLE|WS_BORDER|WS_CHILD|ES_RIGHT|ES_NOHIDESEL},
		{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_AUTO_E_TURN,TEXT(""),120,230,30,30,WS_VISIBLE|WS_BORDER|WS_CHILD|ES_RIGHT|ES_NOHIDESEL},
		{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_AUTOPARAMEDIT,TEXT(""),200,310,400,25,WS_VISIBLE|WS_BORDER|WS_CHILD|ES_UPPERCASE|ES_NUMBER}
};

//子窗口菜单列表
CtlWndStruct autoChildWnds[]={
	{WS_EX_CLIENTEDGE,TEXT("AUTOPARAMWND"),IDC_AUTOPARAM,TEXT(""),200,0,824,309,WS_CHILD|SS_NOTIFY|WS_BORDER},
	{WS_EX_CLIENTEDGE,TEXT("AUTONCCODEWND"),IDC_AUTONCODE,TEXT(""),200,0,412,309,WS_CHILD|SS_NOTIFY|WS_BORDER},
	{WS_EX_CLIENTEDGE,TEXT("AUTOGRAPHWND"),IDC_AUTOGRAPH,TEXT(""),617,5,395,309,WS_CHILD|SS_NOTIFY|WS_BORDER},
	{WS_EX_CLIENTEDGE,TEXT("AUTOGRAPH3DWND"),IDW_AUTOGRAPH_3D,TEXT(""),617,5,395,309,WS_CHILD|WS_BORDER|WS_CLIPSIBLINGS}//2010-5-27
};

int MyRegisterClassAuto(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)autoWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);//
	wcex.lpszMenuName	= NULL;//(LPCTSTR)IDC_EX1;
	wcex.lpszClassName	= TEXT("AUTOWND");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
//注册自动窗口类
	if(!RegisterClassEx(&wcex)) return 0;
	if(!MyRegisterClassAutoParam(hInstance))  return 0;
	if(!MyRegisterClassAutoNccode(hInstance))  return 0;
	if(!MyRegisterClassAutograph(hInstance))  return 0;
	if(!MyRegisterClassAutoGraph(hInstance)) return 0;//2010-5-27

	return 1;
}

//自动窗口过程体
LRESULT CALLBACK autoWndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;
	//查找对应的消息ID,并执行对应的消息处理过程
	for(i=0; i<dim(autoMessages);i++)
	{
		if(msgCode == autoMessages[i].uCode) return(*autoMessages[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//对不存在的消息，调用缺省窗口过程
	return DefWindowProc(hWnd, msgCode, wParam, lParam);
}

LRESULT goCreateAuto(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		
    int i;
	TCHAR szBuffer1[_MAX_FNAME];
	TCHAR szBuffer2[_MAX_FNAME];
//创建自动窗口菜单
	for(i=0;i<dim(autoMenuBtns);i++){
		CreateWindowEx(autoMenuBtns[i].dwExStyle,
			autoMenuBtns[i].szClass,
			autoMenuBtns[i].szTitle,
			autoMenuBtns[i].lStyle,
			autoMenuBtns[i].x,
			autoMenuBtns[i].y,
			autoMenuBtns[i].cx,
			autoMenuBtns[i].cy,
			hWnd,
			(HMENU)autoMenuBtns[i].nID,
			hInst,
			NULL
			);
	}

//创建自动窗口的子窗口
	
	for(i=0;i<dim(autoChildWnds);i++){
		CreateWindowEx(autoChildWnds[i].dwExStyle,
			autoChildWnds[i].szClass,
			autoChildWnds[i].szTitle,
			autoChildWnds[i].lStyle,
			autoChildWnds[i].x,
			autoChildWnds[i].y,
			autoChildWnds[i].cx,
			autoChildWnds[i].cy,
			hWnd,
			(HMENU)autoChildWnds[i].nID,
			hInst,
			NULL
			);
	}
	//显示其中一个窗口
	ShowWindow(GetDlgItem(hWnd,autoChildWnds[0].nID),SW_SHOW);	
	strcpy(szBuffer1,"E");
	wsprintf(szBuffer2, "%d", Machiningparam.eNo);
	strcat(szBuffer1,szBuffer2);
	SendMessage(GetDlgItem(hWnd,IDC_CONDNOIN),LB_INSERTSTRING, 0,(LPARAM)szBuffer1);//显示加工条件号	
	SendMessage(GetDlgItem(hWnd,IDC_NCPRONAMEIN),LB_INSERTSTRING, 0,(LPARAM)AutoMachineFileName);//显示加工文件名
	//2010-5-31
	auto_up_down=0;
	auto_right_left=0;
	auto_zoom=1.0;
	auto_turn_up_down=20;
	auto_turn_right_left=0;
	auto_move_step=10;
	auto_turn_step=10;
	NowAutoGraph2D_3D=3;
	if(NowAutoGraph2D_3D==3)Auto_Show3D(hWnd);
	else Auto_Show2D(hWnd);
	//初始化各EDIT框中内容
	i=auto_move_step;
	itoa(i,szBuffer1,10);
	SetWindowText (GetDlgItem(hWnd,IDC_AUTO_E_MOVE),szBuffer1);
	i=auto_turn_step;
	itoa(i,szBuffer1,10);
	SetWindowText (GetDlgItem(hWnd,IDC_AUTO_E_TURN),szBuffer1);
	i=int(auto_zoom*100);
	itoa(i,szBuffer1,10);
	SetWindowText (GetDlgItem(hWnd,IDC_AUTO_EZOOM),szBuffer1);
	//end 2010-5-31
	//2010-6-24
	AutoInput=0;
	pDataInAutoInput = (LPCmdThreadParam)HeapAlloc(GetProcessHeap(),
											HEAP_ZERO_MEMORY,
											sizeof(CmdThreadParam)
											);
	if(pDataInAutoInput == NULL)
	{
		return 1;
	}

	memset(&DSP_data,0,sizeof(DSP_data));
	//end 2010-6-24
	return 0;
}

LRESULT goCommandAuto(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
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
	for(i=0; i<dim(autoCommands);i++)
	{
		if(menuID == autoCommands[i].uCode) return(*autoCommands[i].functionName)(hWnd,wndCtrl,menuID,notifyCode);
	}
		//	2010-5-31
//旋转、平移、放大缩小对应的参数处理过程
	switch(LOWORD(wParam))
	{
	case IDC_AUTO_B_TURN_UP:
		auto_turn_up_down+=auto_turn_step;
		Auto3DDraw();
		break;
	case IDC_AUTO_B_TURN_DOWN:
		auto_turn_up_down-=auto_turn_step;
		Auto3DDraw();
		break;
	case IDC_AUTO_B_TURN_LEFT:
		auto_turn_right_left-=auto_turn_step;
		Auto3DDraw();
		break;
	case IDC_AUTO_B_TURN_RIGHT:
		auto_turn_right_left+=auto_turn_step;
		Auto3DDraw();
		break;
	case IDC_AUTO_BUP:
		auto_up_down+=auto_move_step;
		Auto3DDraw();
		break;
	case IDC_AUTO_BDOWN:
		auto_up_down-=auto_move_step;
		Auto3DDraw();
		break;
	case IDC_AUTO_BLEFT:
		auto_right_left-=auto_move_step;
		Auto3DDraw();
		break;
	case IDC_AUTO_BRIGHT:
		auto_right_left+=auto_move_step;
		Auto3DDraw();
		break;
	case IDC_BAUTO_3D_2D:
		if(NowAutoGraph2D_3D==3)Auto_Show2D(hWnd);
		else Auto_Show3D(hWnd);
		InvalidateRect(hWnd,NULL,TRUE);
		SendMessage(hWnd,WM_PAINT,NULL,NULL);
	default:
		break;
	}
	//各EDIT文本框对应的处理过程
	if(HIWORD(wParam)==EN_SETFOCUS)
	{
		//去除个文本框高亮
		SendMessage(GetDlgItem(hWnd,IDC_AUTO_EZOOM),EM_SETSEL,-1,0);
		SendMessage(GetDlgItem(hWnd,IDC_AUTO_E_MOVE),EM_SETSEL,-1,0);
		SendMessage(GetDlgItem(hWnd,IDC_AUTO_E_TURN),EM_SETSEL,-1,0);
		switch(LOWORD(wParam))
		{
		case IDC_AUTO_EZOOM:
			pDataInAutoInput->hWnd = hWnd;
			pDataInAutoInput->wndCtrl = GetDlgItem(hWnd,LOWORD(wParam));
			AutoNowSelectID =IDC_AUTO_EZOOM;
			pDataInAutoInput->notifyCode = 0;
			SetFocus(GetDlgItem(hWnd,IDC_AUTOPARAMEDIT));
			SendMessage(GetDlgItem(hWnd,LOWORD(wParam)),EM_SETSEL,0,-1);
			AutoInput=1;
			break;
		case IDC_AUTO_E_MOVE:
			pDataInAutoInput->hWnd = hWnd;
			pDataInAutoInput->wndCtrl = GetDlgItem(hWnd,LOWORD(wParam));
			AutoNowSelectID= IDC_AUTO_E_MOVE;
			pDataInAutoInput->notifyCode = 0;
			SetFocus(GetDlgItem(hWnd,IDC_AUTOPARAMEDIT));
			SendMessage(GetDlgItem(hWnd,LOWORD(wParam)),EM_SETSEL,0,-1);
			AutoInput=1;
			break;
		case IDC_AUTO_E_TURN:
			pDataInAutoInput->hWnd = hWnd;
			pDataInAutoInput->wndCtrl = GetDlgItem(hWnd,LOWORD(wParam));
			AutoNowSelectID =IDC_AUTO_E_TURN;
			pDataInAutoInput->notifyCode = 0;
			SetFocus(GetDlgItem(hWnd,IDC_AUTOPARAMEDIT));
			SendMessage(GetDlgItem(hWnd,LOWORD(wParam)),EM_SETSEL,0,-1);
			AutoInput=1;
			break;
		default:
			break;
		}
	}
//	2010-5-31
	return 0;
}
//2010-5-30
LRESULT goDrawItemAuto(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
    LPDRAWITEMSTRUCT lpdis; 
	lpdis = (LPDRAWITEMSTRUCT) lParam; 
    hdc = CreateCompatibleDC(lpdis->hDC);
	switch(lpdis->CtlID)
	{
	case IDC_AUTO_BUP:
		if (lpdis->itemState & ODS_SELECTED)  // if selected 
			SelectObject(hdc, graph_hbm_up_click); 
		else 
			SelectObject(hdc, graph_hbm_up); 
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
                30,        // source bitmap width 
                18,        // source bitmap height 
                SRCCOPY);  // raster operation 
		break;
	case IDC_AUTO_BLEFT:
		if (lpdis->itemState & ODS_SELECTED)  // if selected 
			SelectObject(hdc, graph_hbm_left_click); 
		else 
			SelectObject(hdc, graph_hbm_left); 
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
                18,        // source bitmap width 
                30,        // source bitmap height 
                SRCCOPY);  // raster operation 
		break;
	case IDC_AUTO_BDOWN:
		if (lpdis->itemState & ODS_SELECTED)  // if selected 
			SelectObject(hdc, graph_hbm_down_click); 
		else 
			SelectObject(hdc, graph_hbm_down); 
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
                30,        // source bitmap width 
                18,        // source bitmap height 
                SRCCOPY);  // raster operation 
		break;

	case IDC_AUTO_BRIGHT:
		if (lpdis->itemState & ODS_SELECTED)  // if selected 
			SelectObject(hdc, graph_hbm_right_click); 
		else 
			SelectObject(hdc, graph_hbm_right); 
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
                18,        // source bitmap width 
                30,        // source bitmap height 
                SRCCOPY);  // raster operation 
		break;
	case IDC_AUTO_BZOOM_IN:
		//if (lpdis->itemState & ODS_SELECTED)  // if selected 
		//	SelectObject(hdc, graph_hbm_zoom_in_click); 
		//else 
		SelectObject(hdc, graph_hbm_zoom_in); 
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
            30,        // source bitmap width 
            30,        // source bitmap height 
            SRCCOPY);  // raster operation 
		break;
	case IDC_AUTO_B_TURN_UP:
		if (lpdis->itemState & ODS_SELECTED)  // if selected 
			SelectObject(hdc, graph_hbm_turn_up_click); 
		else 
			SelectObject(hdc, graph_hbm_turn_up); 
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
                30,        // source bitmap width 
                30,        // source bitmap height 
                SRCCOPY);  // raster operation 
		break;
	case IDC_AUTO_B_TURN_DOWN:
		if (lpdis->itemState & ODS_SELECTED)  // if selected 
			SelectObject(hdc, graph_hbm_turn_down_click); 
		else 
			SelectObject(hdc, graph_hbm_turn_down); 
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
                30,        // source bitmap width 
                30,        // source bitmap height 
                SRCCOPY);  // raster operation 
		break;
	case IDC_AUTO_B_TURN_LEFT:
		if (lpdis->itemState & ODS_SELECTED)  // if selected 
			SelectObject(hdc, graph_hbm_turn_left_click); 
		else 
			SelectObject(hdc, graph_hbm_turn_left); 
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
                30,        // source bitmap width 
                30,        // source bitmap height 
                SRCCOPY);  // raster operation 
		break;
	case IDC_AUTO_B_TURN_RIGHT:
		if (lpdis->itemState & ODS_SELECTED)  // if selected 
			SelectObject(hdc, graph_hbm_turn_right_click); 
		else 
			SelectObject(hdc, graph_hbm_turn_right); 
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
                30,        // source bitmap width 
                30,        // source bitmap height 
                SRCCOPY);  // raster operation 
		break;
	default:
		break;
	}
	DeleteDC(hdc);
    return TRUE; 
}

//end 2010-5-30
LRESULT goKeydownAuto(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int menuID, notifyCode;		
	//定义窗口句柄
	HWND wndCtrl;
	menuID    = LOWORD(wParam); 
	notifyCode = HIWORD(wParam); 
	wndCtrl = (HWND) lParam;
	switch(AutoInput)
	{
	case 1:
		Show3DInput();
		break;
	case 2:
		AutoListConNameCmdThreadProc();
		break;
	case 3:
		AutoListProgramNameCmdThreadProc();
		break;
	case 4:
		ListAutoParamThreadProc();
		break;
	}
	return 0;
}
LRESULT goPaintAuto(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{	//定义画板结构体对象
	PAINTSTRUCT ps;
	RECT rect;
	HDC hdc;
	hdc=BeginPaint(hWnd,&ps);
	//绘制文本
	SetBkMode(hdc,TRANSPARENT);
	SetTextColor(hdc,RGB(0,0,0));
	SetRect(&rect,0,5,100,40);
	DrawText(hdc,"NC PROGRAM",-1,&rect,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,0,35,100,70);
	DrawText(hdc,"ENO",-1,&rect,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	if(NowAutoGraph2D_3D==3)
	{
		SetRect(&rect,100,305,150,320);
		DrawText(hdc,"*100",-1,&rect,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	}
	//end绘制文本
	EndPaint(hWnd,&ps);
	return 0;
}
LRESULT goDestroyAuto(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	PostQuitMessage(0);
	return 0;
}
LRESULT goAutoSelectConCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	ShowWindow(GetDlgItem(hWnd,IDC_AUTOPARAM),SW_SHOW);
    ShowWindow(GetDlgItem(hWnd,IDC_AUTONCODE),SW_HIDE);
	if(NowAutoGraph2D_3D==2) 
	{
		ShowWindow(GetDlgItem(hWnd,IDC_AUTOGRAPH),SW_SHOW);
		ShowWindow(GetDlgItem(hWnd,IDW_AUTOGRAPH_3D),SW_HIDE);
	}
	else 
	{
		ShowWindow(GetDlgItem(hWnd,IDC_AUTOGRAPH),SW_HIDE);
		ShowWindow(GetDlgItem(hWnd,IDW_AUTOGRAPH_3D),SW_SHOW);
	}
	return 0;
}

LRESULT goAutoSelectStateCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	if(NowAutoGraph2D_3D==2) 
	{
		ShowWindow(GetDlgItem(hWnd,IDC_AUTOGRAPH),SW_SHOW);
		ShowWindow(GetDlgItem(hWnd,IDW_AUTOGRAPH_3D),SW_HIDE);
	}
	else 
	{
		ShowWindow(GetDlgItem(hWnd,IDC_AUTOGRAPH),SW_HIDE);
		ShowWindow(GetDlgItem(hWnd,IDW_AUTOGRAPH_3D),SW_SHOW);
	}
    ShowWindow(GetDlgItem(hWnd,IDC_AUTONCODE),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTOPARAM),SW_HIDE);	
	return 0;
}
LRESULT goAutoListConNameCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{	
	//去掉几个列表的蓝色背景色
	SendMessage(GetDlgItem(hWnd,IDC_NCPRONAMEIN),LB_SETCURSEL,-1,0);
	SendMessage(GetDlgItem(hWnd,IDC_CONDNOIN),LB_SETCURSEL,-1,0);
	pDataInAutoInput->hWnd = hWnd;
	pDataInAutoInput->wndCtrl = wndCtrl;
	pDataInAutoInput->menuID = menuID;
	pDataInAutoInput->notifyCode = notifyCode;
	SendMessage(wndCtrl,LB_SETCURSEL,0,0);
	SetFocus(GetDlgItem(hWnd,IDC_AUTOPARAMEDIT));
	AutoInput=2;
	return 0;
}
void AutoListConNameCmdThreadProc()
{
    TCHAR szBuffer1[_MAX_FNAME];
	TCHAR szBuffer2[_MAX_FNAME];
	int i,j;
	int inItem[12];//记录对应的参数文件信息，包括文件名称，参数
	strcpy(szBuffer1,"E");
    GetWindowText(GetDlgItem(pDataInAutoInput->hWnd,IDC_AUTOPARAMEDIT),szBuffer2, _MAX_FNAME);
	i=atoi(szBuffer2);
	SetWindowText (GetDlgItem(GetParent(pDataInAutoInput->hWnd),IDC_STATICALARM),"");
	//搜索对应的加工参数文件不存在则说明该文件不存在
	if(fnSearch(pDataInAutoInput->hWnd,i)==0)
	{
		SetWindowText (GetDlgItem(GetParent(pDataInAutoInput->hWnd),IDC_STATICALARM),"NO FOUND");
	}
	//end 搜索
	setItem(inItem,i);//将加工参数信息保存到inItem中
	//设置Machiningparam
	strcat(szBuffer1,szBuffer2);
	SendMessage(pDataInAutoInput->wndCtrl,LB_INSERTSTRING, 0,(LPARAM)szBuffer1);
	SetWindowText(GetDlgItem(pDataInAutoInput->hWnd,IDC_AUTOPARAMEDIT),"");
	SendMessage(pDataInAutoInput->wndCtrl,LB_SETCURSEL,-1,0);
	Machiningparam.eNo = atoi(szBuffer2); 
	for(i=1;i<12;i++)
	{
		Machiningparam.item[i].value=inItem[i];
	} 
	//end 设置Machiningparam
	//加工参数的列名目和对应参数	
	for(j=0;j<2;j++)
	{
		for(i=0;i<8;i++)
			{
				//iowrite(GetDlgItem(pData->hWnd,IDC_AUTOPARAM),Machiningparam.item[10*j+i].hDevice,Machiningparam.item[8*j+i].ctrWord, Machiningparam.item[10*j+i].addr,  //将加工参数写入设备
				//	Machiningparam.item[10*j+i].value, 1);							
			}
	}
	NOWSELECTLINE=0;
	fn_auto_setLV(hAUTOLV);//设置ListView内容
	AutoInput=0;
}
LRESULT goAutoListProgramNameCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	//去掉几个列表的蓝色背景色
	SendMessage(GetDlgItem(hWnd,IDC_NCPRONAMEIN),LB_SETCURSEL,-1,0);
	SendMessage(GetDlgItem(hWnd,IDC_CONDNOIN),LB_SETCURSEL,-1,0);
	pDataInAutoInput->hWnd = hWnd;
	pDataInAutoInput->wndCtrl = wndCtrl;
	pDataInAutoInput->menuID = menuID;
	pDataInAutoInput->notifyCode = notifyCode;
	SendMessage(wndCtrl,LB_SETCURSEL,0,0);
	SetFocus(GetDlgItem(hWnd,IDC_AUTOPARAMEDIT));
	AutoInput=3;
	return 0;
}
void AutoListProgramNameCmdThreadProc()
{ 
    TCHAR szBuffer1[_MAX_FNAME];
	TCHAR szBuffer2[_MAX_FNAME];
	TCHAR szBuffer3[_MAX_FNAME];
	int fd;	
	strcpy(szBuffer1,"O");
    GetWindowText(GetDlgItem(pDataInAutoInput->hWnd,IDC_AUTOPARAMEDIT),szBuffer2, _MAX_FNAME);
	strcat(szBuffer1,szBuffer2);
	strcpy(szBuffer3,szBuffer1); 
    strcat(szBuffer1,".txt");
	//GetWindowText(GetDlgItem(hWnd,IDC_NCPRONAMEIN),szBuffer,sizeof(szBuffer)); //取得要加工的数控程序文件名
	 
	fd = _open(szBuffer1, O_RDONLY); //打开数控程序文件确认是否有该文件

    if (fd <= 0)
    {
        strcat(szBuffer1, " is no exist!");        
        MessageBox (pDataInAutoInput->hWnd, szBuffer1,"Program", MB_OK | MB_ICONSTOP);
     }

	_close(fd);
	strcpy(	AutoMachineFileName,szBuffer3);
		
	SendMessage(pDataInAutoInput->wndCtrl,LB_INSERTSTRING, 0,(LPARAM)szBuffer3);
	
	SetWindowText(GetDlgItem(pDataInAutoInput->hWnd,IDC_AUTOPARAMEDIT),"");
	SendMessage(pDataInAutoInput->wndCtrl,LB_SETCURSEL,-1,0);
	AutoInput=0;
}

LRESULT goAutoMachiningBuildCmd(HWND hWnd, HWND wndCtrl,int menuID,int notifyCode)
{
	DWORD dwThreadID;
	LPCmdThreadParam  pData;	
		
	pData = (LPCmdThreadParam)HeapAlloc(GetProcessHeap(),   //分配堆内存
									HEAP_ZERO_MEMORY,
									sizeof(CmdThreadParam)  //CmdThreadParam为各种符号的结构体
									);
	if(pData == NULL)
	{
		MessageBox(hWnd,"can not alloc heapmemory in function goAutoMachiningBuildCmd",NULL,NULL);
		return 1;
	}

	pData->hWnd = hWnd;
	pData->wndCtrl = wndCtrl;
	pData->menuID = menuID;
	pData->notifyCode = notifyCode;
   	AutohThread = CreateThread(                    
		NULL,
		0,
		AutoMachiningBuildThreadProc,
		pData,
		0,
		&dwThreadID
		);
	if( AutohThread==NULL)
	{
		MessageBox(hWnd,"can not create Thread in function goAutoMachiningBuildCmd",NULL,NULL);
		return 1;
	}
	InvalidateRect(GetDlgItem(hWnd,IDC_AUTOGRAPH),NULL,TRUE);
	return 0;
}
DWORD WINAPI AutoMachiningBuildThreadProc(LPVOID lpParam) //线程函数
{
	HANDLE hMutex;
	LPCmdThreadParam  pData; 
  	
	LPCmdThreadParam  pData_nc;
	DWORD dwThreadID;
	LPNCDATA pDataNcGraphMem;
	
	TCHAR szBuffer[_MAX_FNAME+1]; 
	int decodeNum,compasateNum,tapeNum,ComputeNum,coorNum;
	int all_decode_num,all_creat_num;

	nc_data compasateData[2*DECODE_NUM_ONCE];
	nc_data tapeData[2*DECODE_NUM_ONCE];
	nc_data ComputeData[2*DECODE_NUM_ONCE];	
	nc_data coordata[2*DECODE_NUM_ONCE];	
	
	double auto_draw_width;
	double auto_draw_length,auto_mw, auto_ml;	
	
	int  fdEdit;  //译码文件句柄定义
	int end_decode;

	double compasate_Start_point_X,compasate_Start_point_Y;
    int compasate_build_c;
	nc_data *compasate_cs;

	double tape_Start_point_X,tape_Start_point_Y;
	double tape_Start_point_B,tape_Start_point_C;
    int tape_build_c,first5152flag;
	nc_data *tape_cs;
	int nc_start_flag;
	//2010-5-25上下异形面加工增加参量
	int DcompasateNumup,DcompasateNumlow;
	nc_data DcompasateData[2*DECODE_NUM_ONCE];
	double Dcompasate_Start_point_X,Dcompasate_Start_point_Y;
    double Dcompasate_Start_point_B,Dcompasate_Start_point_C;
    int Dcompasate_build_c_up,Dcompasate_build_c_low;
	nc_data *Dcompasate_cs;
	Dcompasate_Start_point_X=0;
	Dcompasate_Start_point_Y=0;
	Dcompasate_Start_point_B=0;
	Dcompasate_Start_point_C=0;
	Dcompasate_build_c_up=0;
	Dcompasate_build_c_low=0;
	//2010-5-25

	
	//以下添加自动加工程序
	SuspendThread(AutohThread);

	compasate_Start_point_X=0.;
	compasate_Start_point_Y=0.;
	compasate_build_c=0;
	
	tape_Start_point_X=0.;
	tape_Start_point_Y=0.;
	tape_Start_point_B=0.;
	tape_Start_point_C=0.;
	tape_build_c=0;
	
	memset(decodeData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));     //清空
	memset(compasateData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));
	memset(ComputeData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));
	memset(MChild,0,2*DECODE_NUM_ONCE*sizeof(M_data)); 
	memset(DcompasateData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));//2010-5-25
	memset(coordata,0,2*DECODE_NUM_ONCE*sizeof(nc_data));//2010-5-25
		
	compasate_cs = (nc_data *)malloc(sizeof(nc_data));           //开辟相应内存
	memset(compasate_cs,0,sizeof(nc_data));						//清空

	tape_cs = (nc_data *)malloc(sizeof(nc_data));
	memset(tape_cs,0,sizeof(nc_data));
	Dcompasate_cs = (nc_data *)malloc(sizeof(nc_data));//2010-5-25
	pData = (LPCmdThreadParam)lpParam;

	end_decode = 0;
	all_decode_num =0;
	all_creat_num = 0;

	nc_start_flag=1;
	Autoreset_flag = 0;	
	
	//开设译码绘图内存
	pDataNcGraphMem = (LPNCDATA)HeapAlloc(GetProcessHeap(),
									HEAP_ZERO_MEMORY,
									MAX_NC_MEM*sizeof(nc_data)
									);
	if(pDataNcGraphMem == NULL)
	{
		MessageBox(pData->hWnd,"can not alloc heapmemory in function AutoMachiningBuildThreadProc",NULL,NULL);
		return 1;
	}

	//开设NC代码内存
	lpNcCodeMem = (LPNCCODE)HeapAlloc(GetProcessHeap(),
									HEAP_ZERO_MEMORY,
									MAX_NC_MEM*sizeof(nc_code)
									);
	if(pDataNcGraphMem == NULL)
	{
		MessageBox(pData->hWnd,"can not alloc heapmemory in function AutoMachiningBuildThreadProc",NULL,NULL);
		return 1;
	}

	SendMessage(GetDlgItem(pData->hWnd,IDC_NCPRONAMEIN),LB_GETTEXT,0,(LPARAM)szBuffer);
	strcat(szBuffer,".txt");
	//MessageBox(pData->hWnd,szBuffer,NULL,NULL);
	
	 //为读取文件到内存打开文件
	fdEdit = _open(szBuffer, O_RDONLY);
 
	if (fdEdit <= 0) 
	{
		MessageBox (pData->hWnd, "can not open file in AutoMachiningBuildThreadProc","Program", MB_OK | MB_ICONSTOP);
		return 1;
	}

	ReadNcCodeFileToMem(pData->hWnd,fdEdit,lpNcCodeMem,&NcCodeNum);
	_close(fdEdit);

	//为译码打开文件
	fdEdit = _open(szBuffer, O_RDONLY);
	
	if (fdEdit <= 0) 
	{
		MessageBox (pData->hWnd, "can not open file in AutoMachiningBuildThreadProc","Program", MB_OK | MB_ICONSTOP);
		return 1;
	}
	
	hMachiningEndEvent = CreateEvent(NULL,FALSE,FALSE,NULL); //建立加工结束事件
	if(hMachiningEndEvent == NULL)
	{
		MessageBox(pData->hWnd,"can not create  hDecodeEvent in AutoMachiningBuildThreadProc function",NULL,NULL);
		return 1;
	}

	hM_Code_Event = CreateEvent(NULL,FALSE,FALSE,NULL); //建立加工结束事件
	if(hM_Code_Event == NULL)
	{
		MessageBox(pData->hWnd,"can not create  hM_Code_Event in AutoMachiningBuildThreadProc function",NULL,NULL);
		return 1;
	}	

	hM_Code_EventExecutive = CreateEvent(NULL,FALSE,FALSE,NULL); //建立加工结束事件
	if(hM_Code_Event == NULL)
	{
		MessageBox(pData->hWnd,"can not create  hM_Code_EventExecutive in AutoMachiningBuildThreadProc function",NULL,NULL);
		return 1;
	}	

	Load_NC_TapeParameter_Program(pData->hWnd);

	do
	{
		if(decode(pData->hWnd,lpNcCodeMem,decodeData,&decodeNum,&all_decode_num,&end_decode,MChild)==1) return 1; // 分段译码	
		if(CoordinateSwitch(pData->hWnd, decodeData,decodeNum, coordata,&coorNum))return 1; 
		if(decodeData->G_Dcompensate==152||decodeData->G_Dcompensate==153||decodeData->G_Dcompensate==150)//11
		{ 
			Dcompensate(pData->hWnd,coordata,coorNum,DcompasateData,&DcompasateNumup,&DcompasateNumlow,&Dcompasate_Start_point_X,&Dcompasate_Start_point_Y,&Dcompasate_Start_point_B,&Dcompasate_Start_point_C,&Dcompasate_build_c_up, &Dcompasate_build_c_low,compasate_cs);
			
		    if(DSP_Compute(pData->hWnd,DcompasateData,DcompasateNumup,ComputeData,&ComputeNum)==1) return 1;

			CopyMemory(pDataNcGraphMem+all_creat_num,ComputeData,(ComputeNum*sizeof(nc_data)));	

			for(int j=0;j<2*DECODE_NUM_ONCE;j++)JudgeData[j] = DcompasateData[j];
			memset(decodeData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));
			memset(coordata,0,2*DECODE_NUM_ONCE*sizeof(nc_data));
			memset(DcompasateData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));//void *memset( void *dest, int c, size_t count );
			memset(ComputeData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));
			
			all_creat_num=all_creat_num + DcompasateNumup;
			ComputeNum = 0;
			DcompasateNumup=0;
			DcompasateNumlow=0;
		}
		else
		{
			if(compensate(pData->hWnd,coordata,coorNum,compasateData,&compasateNum,&compasate_Start_point_X,&compasate_Start_point_Y, &compasate_build_c,compasate_cs)==1) return 1;//分段刀具补偿
			if(tape(pData->hWnd,compasateData,compasateNum,tapeData,&tapeNum,&tape_Start_point_X,&tape_Start_point_Y,&tape_build_c,&first5152flag,tape_cs,&tape_Start_point_B,&tape_Start_point_C)==1) return 1; //锥面补偿
			if(DSP_Compute(pData->hWnd,tapeData,tapeNum,ComputeData,&ComputeNum)==1) return 1; 
			
			CopyMemory(pDataNcGraphMem+all_creat_num,ComputeData,(ComputeNum*sizeof(nc_data)));	
			
			for(int j=0;j<2*DECODE_NUM_ONCE;j++)JudgeData[j] = tapeData[j];
			memset(decodeData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));
			memset(coordata,0,2*DECODE_NUM_ONCE*sizeof(nc_data));
			memset(compasateData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));//void *memset( void *dest, int c, size_t count );
			memset(tapeData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));//The memset function sets the first count bytes of dest to the character c
			memset(ComputeData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));
			
			all_creat_num=all_creat_num + tapeNum;
			ComputeNum = 0;
			compasateNum=0;
			tapeNum=0;
		}									
		//开设向下位机传送数据线程
		if(nc_start_flag==1)
		{		
			pData_nc = (LPCmdThreadParam)HeapAlloc(GetProcessHeap(),
								HEAP_ZERO_MEMORY,
								sizeof(CmdThreadParam)
								);

			if(pData_nc == NULL)
			{
				MessageBox(pData->hWnd,"can not alloc heapmemory in function AutoMachiningBuildThreadProc",NULL,NULL);
				return 1;
			}
			
			hMutex = CreateMutex(NULL,FALSE,NULL);  //建立互斥对象
			if(hMutex == NULL)
			{
				MessageBox(pData->hWnd,"can not make Mutex  in function AutoMachiningBuildThreadProc",NULL,NULL);
				return 1;
			}
				
			pData_nc->hWnd = pData->hWnd;
			pData_nc->wndCtrl = pData->wndCtrl;
			pData_nc->menuID = pData->menuID;
			pData_nc->notifyCode =pData-> notifyCode;
			pData_nc->ncMem = pDataNcGraphMem;
			pData_nc->hMutex = hMutex;  
			
			NcSendhThread = CreateThread(
					NULL,
					0,
					NcSendThreadProc,
					pData_nc,
					0,
					&dwThreadID
					);

			if( NcSendhThread==NULL)
			{
				MessageBox(pData->hWnd,"can not create Thread in function AutoMachiningBuildThreadProc",NULL,NULL);
				return 1;
			}			
			nc_start_flag = 0;
		}

		WaitForSingleObject(hMutex,INFINITE); //通过wait函数获得互斥体独占权
		DecodingNum = all_creat_num;          //保护部分 
		ReleaseMutex(pData->hMutex);		  //释放互斥体
	}while(end_decode != 1);

	_close(fdEdit);	
	find_draw_param(GetDlgItem (pData->hWnd, IDC_AUTOGRAPH),pDataNcGraphMem,&auto_draw_width,&auto_draw_length,&auto_mw, &auto_ml,all_creat_num);//求取画图参数
	AutoGraphParam.hWnd = GetDlgItem (pData->hWnd, IDC_AUTOGRAPH);  //给AUTOPAINT中重画函数的参数结构体赋值
	AutoGraphParam.pDataNcGraphMem = pDataNcGraphMem;
	AutoGraphParam.draw_width = auto_draw_width;
	AutoGraphParam.draw_length = auto_draw_length;
	AutoGraphParam.mw = auto_mw;
	AutoGraphParam.ml = auto_ml;
	AutoGraphParam.create_num = all_creat_num;

	draw_all(GetDlgItem (pData->hWnd, IDC_AUTOGRAPH),pDataNcGraphMem,auto_draw_width,auto_draw_length,auto_mw, auto_ml,all_creat_num); //画全部图
	draw_all_tape(GetDlgItem (pData->hWnd, IDC_AUTOGRAPH),pDataNcGraphMem,auto_draw_width,auto_draw_length,auto_mw, auto_ml,all_creat_num); //画全部tap图
	auto_regraph_flag = 1; //进行图形重画标志
	auto_nccode_flag = 1; //进行nc码显示标志
	AutographFirstEnterFlag = 0; //自动图形显示函数第一次进入恢复
	AutonccodeFirstEnterFlag = 0; // //nc码显示函数第一次进入恢复
		
    free(tape_cs);
    free(compasate_cs);
	free(Dcompasate_cs);
	
	//取消线程、释放内存
	CloseHandle(AutohThread);
	if(HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pData) == 0){
		MessageBox(pData->hWnd,"can not free heapmemory in function AutoMachiningBuildThreadProc",NULL,NULL);
		return 1;
	}
	return 0;
}
DWORD WINAPI NcSendThreadProc(LPVOID lpParam)
{
	LPCmdThreadParam  pData; 
	DWORD dwThreadID;
	int k;
	int Hint_Num;
	int Recent_Row_Num_Temp = 0;
	
	int SendNum,SendNumOnce,SendDoneNum,SendNumOnceAll,SendDoneNumOnce,Gcode_Row_Num;
	Recent_Row_Num = 0;
	Gcode_Row_Num = 0;
	Hint_Num = 0;
	pData = (LPCmdThreadParam)lpParam;
	//以下填写传送程序
	if(pData->hWnd == GetDlgItem(GetParent(pData->hWnd),IDC_MDI)){
		
		EnableWindow(GetDlgItem(GetParent(pData->hWnd),IDC_BTNMANUEL),FALSE); //屏蔽手动按钮
		EnableWindow(GetDlgItem(GetParent(pData->hWnd),IDC_BTNAUTO),FALSE); //屏蔽AUTO按钮

	}
	if(pData->hWnd == GetDlgItem(GetParent(pData->hWnd),IDC_AUTO)){

		EnableWindow(GetDlgItem(GetParent(pData->hWnd),IDC_BTNMANUEL),FALSE); //屏蔽手动按钮
		EnableWindow(GetDlgItem(GetParent(pData->hWnd),IDC_BTNMDI),FALSE); //屏蔽MDI按钮
	}	
	k=0;
	SendDoneNum = 0;

	WaitForSingleObject(pData->hMutex,INFINITE);
	SendNum = DecodingNum;
	ReleaseMutex(pData->hMutex);
//	PowerOn(pData->hWnd);
	do{		
		SendNumOnceAll = 0;
		SendNumOnce = 0;
		SendDoneNumOnce = 0;

		  
		while(MChild[Recent_Row_Num].M ==0)
		{
			if(JudgeData[Gcode_Row_Num].row_id == 2) 
			{
				SendNumOnceAll++;
				Gcode_Row_Num++;

			}
			Recent_Row_Num++;
			SendNumOnceAll++;
			Gcode_Row_Num++;
			
		}

		if(SendNumOnceAll != 0)
		{
		    do
			{
				if(SendNumOnceAll>= SendDoneNumOnce + SENDNCNUM)  SendNumOnce = SENDNCNUM;
				else SendNumOnce = SendNumOnceAll - SendDoneNumOnce;

				iowrite(pData->hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_DECODE_NUM_ADR, SendNumOnce);  //向DSP下传译码结构体数
			
				Load_NC_Decode_Program(pData->hWnd,pData->ncMem,SendDoneNum,SendNumOnce);  //向DSP下传译码结构体数据
				iowrite(pData->hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_Write_Decode_End_Flag_ADR, 0X01);  //向DSP下传Write_Decode_End_Flag
				
			   if( k == 0 )
			   {
					Sleep(1000);
					DSP_Parameter_Transport(pData->hWnd);
					k=1;
			   }
				SendDoneNumOnce = SendDoneNumOnce + SendNumOnce;
				SendDoneNum = SendDoneNum + SendNumOnce;
				ResetEvent(hDecodeEvent);
				WaitForSingleObject(hDecodeEvent,INFINITE); //挂起线程等待下位机唤醒
				Hint_Num++;

				iowrite(pData->hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_Hint_num, Hint_Num); 
				SendNumOnce = SendNumOnceAll - SendDoneNumOnce;

				WaitForSingleObject(pData->hMutex,INFINITE);
				SendNum = DecodingNum - SendDoneNum;
				ReleaseMutex(pData->hMutex);

				if( Autoreset_flag == 1)
				{
					SendNumOnce = 0;
					
				}

			}while(SendNumOnce != 0 );
		}

		

		
		do
		{	
		
			//加入执行m码内容
			iowrite(pData->hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_Sway_End, SendNum);
			if(Recent_Row_Num == 0) Recent_Row_Num_Temp = 0;
			else Recent_Row_Num_Temp = Recent_Row_Num -1;
			Sleep(2000);
			if(((Para_End_Flag == 1)&&(MChild[Recent_Row_Num].M !=0)&&(MChild[row + 1].M !=0))||(MChild[Recent_Row_Num_Temp].M !=0))
			{
 				switch(MChild[Recent_Row_Num].M)
				{
					case 1:
						Sleep(200);
						MessageBox(pData->hWnd,"excuting the M01 code", NULL,NULL);
						Sleep(200);
						break;
					case 17:
						Sleep(200);
						iowrite(pData->hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_Machining_M17_Flag_ADR, 0X01);
						MessageBox(pData->hWnd,"excuting the M17 code", NULL,NULL);
						break;
					case 30:
						Sleep(200);
						MessageBox(pData->hWnd,"excuting the M30 code", NULL,NULL);
						Autoreset_flag = 1;
						break;

					default:
						break;
				}
				iowrite(pData->hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_Machining_ParaEnd_Flag_ADR, 0X01);
				Recent_Row_Num++;
				M_Code_hTread_Flag = 1;

				Sleep(200);
			}
			
		}while(M_Code_hTread_Flag == 0);

		M_Code_hTread_Flag = 0;
		
		if( Autoreset_flag == 1)
		{
			SendNum = 0;
			
		}


		
	}while(SendNum != 0 );


	ResetEvent(hMachiningEndEvent);
	if( Autoreset_flag == 0) WaitForSingleObject(hMachiningEndEvent,INFINITE); //挂起线程等待加工结束唤醒 
	if( Autoreset_flag == 1) 
	{
		
		iowrite(pData->hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_Reset_Flag_ADR, 0X01); //下传reset信号
	}

	PowerOff(pData->hWnd);
	EnableWindow(GetDlgItem(GetParent(pData->hWnd),IDC_BTNMANUEL),TRUE);//使能手动按钮
	EnableWindow(GetDlgItem(GetParent(pData->hWnd),IDC_BTNMDI),TRUE);//使能MDI按钮
	EnableWindow(GetDlgItem(GetParent(pData->hWnd),IDC_BTNAUTO),TRUE);//使能AUTO按钮
	
	auto_regraph_flag = 0; //取消重画功能
	auto_nccode_flag = 0; //取消nc码显示功能	
	
	//取消线程、释放内存
	CloseHandle(NcSendhThread);

			
	if(HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pData) == 0){
		MessageBox(pData->hWnd,"can not free heapmemory in function NcSendThreadProc",NULL,NULL);
		return 1;
	}

	//解放绘图内存
	
	if(HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pData->ncMem) == 0){
		MessageBox(pData->hWnd,"can not free heapmemory in function NcSendThreadProc",NULL,NULL);
		return 1;
	}
	
	//解放NC代码内存

	if(HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,lpNcCodeMem) == 0){
		MessageBox(pData->hWnd,"can not free heapmemory in function NcSendThreadProc",NULL,NULL);
		return 1;
	}
	
	CloseHandle(hMachiningEndEvent);
	return 0;
}
int Load_NC_Decode_Program(HWND hWnd,LPNCDATA pData,int num1, int num2)
{		
	BOOL bResult;
	DWORD junk;

	bResult = DeviceIoControl(NCPCIhDevice,  // device to be queried
				NC_HPI_DECODE_LOAD,  // operation to perform
                (LPVOID)(pData+num1), num2*sizeof(nc_data), // no input buffer
                NULL,0,     // output buffer
                 &junk,                 // # bytes returned
                 (LPOVERLAPPED)NULL);  // synchronous I/O
	
	if(bResult==FALSE)
	{
			MessageBox(hWnd,"can not get i/o data in fuction Load_NC_Decode_Program", NULL,NULL);
			return 1;
	}
	
	return 0;
}

DWORD WINAPI M_Code_Proc(LPVOID lpParam)
{
	int Recent_Row_Num_Temp = 0;
	LPCmdThreadParam  pData; 
	pData = (LPCmdThreadParam)lpParam;      

	while(1)
	{	
	
		//加入执行m码内容
		
		if(Recent_Row_Num == 0) Recent_Row_Num_Temp = 0;
		else Recent_Row_Num_Temp = Recent_Row_Num -1;
		Sleep(2000);
		if(((Para_End_Flag == 1)&&(MChild[Recent_Row_Num].M !=0)&&(MChild[row + 1].M !=0))||(MChild[Recent_Row_Num_Temp].M !=0))
		{
 			switch(MChild[Recent_Row_Num].M)
			{
				case 1:
					Sleep(200);
					MessageBox(pData->hWnd,"excuting the M01 code", NULL,NULL);
					Sleep(200);
					break;
				case 17:
					Sleep(200);
					iowrite(pData->hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_Machining_M17_Flag_ADR, 0X01);
					MessageBox(pData->hWnd,"excuting the M17 code", NULL,NULL);
					break;
				case 30:
					Sleep(200);
					MessageBox(pData->hWnd,"excuting the M30 code", NULL,NULL);
					Autoreset_flag = 1;
					break;

				default:
					break;
			}
			iowrite(pData->hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_Machining_ParaEnd_Flag_ADR, 0X01);
			Recent_Row_Num++;
			SetEvent(hM_Code_Event);
			ResetEvent(hM_Code_EventExecutive);
			WaitForSingleObject(hM_Code_EventExecutive,INFINITE);
			Sleep(200);
		}
		
			
	}
	//WaitForSingleObject(hM_Code_Event,INFINITE); //挂起线程等待上位机执行完m码	
	return 1;
}

int Load_NC_TapeParameter_Program(HWND hWnd)
{		
	BOOL bResult;
	DWORD junk;
	Tape_data TapePara;
	//2010-5-26
	TapePara.Tape_k1 = (HLOWDISTANCE + h_thickness)/ h_thickness;
	TapePara.Tape_k2 = HLOWDISTANCE/ h_thickness;
	TapePara.Tape_k3 = (HLOWDISTANCE - z_position)/ h_thickness;
	TapePara.Tape_k4 =	z_position/ h_thickness ;

	bResult = DeviceIoControl(NCPCIhDevice,  // device to be queried
				DSP_TAPEPARAMETER_WRITE,  // operation to perform
                (LPVOID)(&TapePara), sizeof(Tape_data), // no input buffer
                NULL,0,     // output buffer
                 &junk,                 // # bytes returned
                 (LPOVERLAPPED)NULL);  // synchronous I/O
	
	if(bResult==FALSE)
	{
			MessageBox(hWnd,"can not get i/o data in fuction Load_NC_TapeParameter_Program", NULL,NULL);
			return 1;
	}	
	return 0;
}
int MachiningParamRead(HWND hWnd,int ConNum)
{
	char data[51][_MAX_FNAME]={"","","","",""};
	FILE *ConditionFile;//Machine Coor File;	
	int i,j,k;
	char ch;
	/*Read Machine Pos*/
	ConditionFile=fopen("Condition.txt","rt");
	
	if(ConditionFile==NULL)
	{
		MessageBox(hWnd,"Condition.txt open error when reading file in fuction MachiningParamRead",NULL,NULL);
		return 1;
	}            
	i=0;
	j=0;
	ch=fgetc(ConditionFile);
	do{
		while(ch!=EOF && i!=31 ) 
		{				
			if(ch !='\n')
			{
				data[i][j]=ch;
					j=j+1;
			}
			else
			{
				i++;
				j=0;
			}	
			ch=fgetc(ConditionFile);
		}

		i=0;
		j=0;
		if(atoi(data[0]) == ConNum)
		{ 
			for(k=0;k<30;k++) Machiningparam.item[i].value = atoi(data[i]);
			if(fclose(ConditionFile))
			{
				MessageBox(hWnd,"Condition.txt close error when reading file in fuction MachiningParamRead ",NULL,NULL);
				return 0;
			}
				
			return 0;
		}
	}while(ch!=EOF);

	if(fclose(ConditionFile))
	{
		MessageBox(hWnd,"Condition.txt close error when reading file in fuction MachiningParamRead ",NULL,NULL);
		return 1;
	}	
	return 1;
}
int ReadNcCodeFileToMem(HWND hWnd,int fd,LPNCCODE pData,int *num)
{
	int fdEdit;
	int reallength;
	int i;
	int j=0;
	char ptext[255];
	char ptext2[255];
	int k=0;
	int m=0;
	int g=0;
	int o =0;
	int M98_Flag = 0;
	int M98_offset = 0;
	int M98_j = 0;
	int M98_n = 0;
	int R_num = 0;
	int p=0;
	int Frst = 0;
	TCHAR szBuffer1[_MAX_FNAME];
	TCHAR szBuffer2[_MAX_FNAME];
	M98_data *M98data;
	M98data = (M98_data *)malloc(sizeof(M98_data));	
	memset(M98data,0,sizeof(M98_data));

	while(1)
	{		
		memset(ptext, '0', sizeof(ptext));
		Frst = 0;

		if(g==1)
		{
			i = 1;
			ptext[0] = 'G';
			g=0;
		}
		else if(m==1)
		{
			i = 1;
			ptext[0] = 'M';
			m=0;			
		}
		else if(p==1)
		{
			i = 1;
			ptext[0] = 'P';
			p=0;			
		}
		else if(o==1)
		{
			i = 1;
			ptext[0] = 'O';
			o=0;			
		}
		else i = 0;
		//读一行
		do{
			do
			{
				reallength = _read(fd, ptext+i, 1 );
				if(reallength == 0)
				{
					free(M98data);
					return 0;
				}

			}while((ptext[i] == '\n')||(ptext[i] == '\r'));///////////排除回车
			if(ptext[i] == 'N')                           //在读到子程序时处理
			{
				do{
					i=0;
					do
					{
						reallength = _read(fd, ptext+i, 1 );
						if(reallength == 0)
						{
							
							return 0;
						}
						
					  
					}while((ptext[i] != 'M'));					
					do{	
						reallength = _read(fd, ptext+i+1, 1 );
						if(reallength == 0)
						{
							return 0;
						}
						i++;

						k=1;
						if((ptext[i-1] == ';'))
						{
							if(strstr(ptext,"M")) 
							{
								M98data->M = Gcode2d(ptext,"M");
							}
							k=0;
						}

					}while(k==1);

				}while(M98data->M != 99);
				i=0;
				ptext[0] = ';';             //跳出处理				
			}

			i=i+1;
			k=1;
			if((ptext[i-1] == ';')||(ptext[i-1] == 'M')||(ptext[i-1] == 'G')||(ptext[i-1] == 'P')||(ptext[i-1] == 'O'))k=0;
		}while(k==1);

//////////////////////////////////////判断是否调用子程序///////////////////////////////////////////////
		if(ptext[0] == 'O')
		{
			M98_offset = _tell(fd);

			if(strstr(ptext,"O")) 
			{
				M98data->O = Gcode2d(ptext,"O");
			}
			if(strstr(ptext,"R")) 
			{
				M98data->NUM = Gcode2d(ptext,"R");
			}
			else M98data->NUM = 1;

			if(strstr(ptext,"D")) 
			{
				M98data->D = Gcode2d(ptext,"D");
			}
			if(strstr(ptext,"T")) 
			{
				M98data->T = Gcode2d(ptext,"T");
			}
			itoa(M98data->O,szBuffer2,10);
			strcpy(szBuffer1,"O");
			strcat(szBuffer1,szBuffer2);
			strcat(szBuffer1,".txt");

			fdEdit = _open(szBuffer1, O_RDONLY); //打开数控程序文件确认是否有该文件

			if (fdEdit <= 0)
			{
				strcat(szBuffer1, " is no exist!");        
				MessageBox (pDataInAutoInput->hWnd, szBuffer1,"Program", MB_OK | MB_ICONSTOP);
			}

			M98_n = 0;	
			M98_j = 0;
			do
			{		

				i=0;
				memset(ptext2, '0', sizeof(ptext2));

				if(g==1)
				{
					i = 1;
					ptext2[0] = 'G';
					g=0;
				}
								
				else if(m==1)
				{
					i = 1;
					ptext2[0] = 'M';
					M98_n++;	
					m=0;			
				}
				else i = 0;
				//读一行
				do{
					do
					{
						reallength = _read(fdEdit, ptext2+i, 1 );
						if(reallength == 0)
						{
							return 0;
						}

					}while((ptext2[i] == '\n')||(ptext2[i] == '\r'));///////////排除回车
					
					i=i+1;
					k=1;
					if((ptext2[i-1] == ';')||(ptext2[i-1] == 'M')||(ptext2[i-1] == 'G'))k=0;
				}while(k==1);	
				
				if(ptext2[i-1] == 'M')
				{
					if(i!=1)
					{
						m=1;
					}
					else
					{
						do{
							reallength = _read(fdEdit, ptext2+i, 1 );
							if(reallength == 0)
							{
								return 0;
							}
							i=i+1;
							k=1;
							if((ptext2[i-1] == ';')||(ptext2[i-1] == 'G')||(ptext2[i-1] == 'P'))k=0;
						}while(k==1);

						M98_n++;	
						if((i!=1)&&(ptext2[i-1] == 'G'))
						{
							g=1;

						}

					}
				}

				else if(ptext2[i-1] == 'G')
				{
					if((i!=1)&&(ptext2[0] != 'G'))
					{
						g=1;
					}
					else
					{
						do{
							reallength = _read(fdEdit, ptext2+i, 1 );
							if(reallength == 0)
							{
								return 0;
							}

							i=i+1;
							k=1;
							if((ptext2[i-1] == ';')||(ptext2[i-1] == 'M'))k=0;
						}while(k==1);
						
						//n++;
						if((i!=1)&&(ptext2[i-1] == 'M'))
						{
							m=1;

						}
					}
				}

				ptext2[i-1] = ';';
				ptext2[i] = 0;
				if(Frst == 0)
				{
					memset(szBuffer1, '0', sizeof(szBuffer1));
					memset(szBuffer2, '0', sizeof(szBuffer2));

					ptext2[i-1] = 0;
					sprintf_s(szBuffer2, "%4.3f", M98data->D);
					strcpy(szBuffer1,"D");
					strcat(szBuffer1,szBuffer2);
					strcat(ptext2,szBuffer1);
					i = i+ strlen(szBuffer1);

					ultoa(M98data->T,szBuffer2,10);
					strcpy(szBuffer1,"T");
					strcat(szBuffer1,szBuffer2);
					strcpy(szBuffer2,";");
					strcat(szBuffer1,szBuffer2);
					strcat(ptext2,szBuffer1);
					i = i+ strlen(szBuffer1);

					ptext2[i] = 0;
					Frst = 1;
				}

				pData->row_id = j;
				strcpy((pData+j)->pText,ptext2);
				j++;					
				M98_j++;
				if(ptext2[0] == 'M') 
				{
					if(strstr(ptext2,"M")) 
					{
						M98data->M = Gcode2d(ptext2,"M");
					}
					if(M98data->M == 99)
					{
						M98_offset = _lseek(fd,M98_offset,SEEK_SET);
						M98_Flag = 1;
					}
				}

			}while(M98_Flag == 0);  

			M98_Flag = 0;

////////////////////////////////////////////子程序合并/////////////////////////////////////////////////

			for(R_num = 0;R_num< ((M98data->NUM)-1)*M98_j;R_num++)
			{
				pData->row_id = j;
				strcpy((pData+j)->pText,(pData+j-M98_j)->pText);
				j ++;					
			}

			_close(fdEdit);

		}
		else if(ptext[0] == 'P')
		{
			M98_offset = _tell(fd);

			if(strstr(ptext,"P")) 
			{
				M98data->P = Gcode2d(ptext,"P");
			}
			if(strstr(ptext,"R")) 
			{
				M98data->NUM = Gcode2d(ptext,"R");
			}
			else M98data->NUM = 1;

			if(strstr(ptext,"D")) 
			{
				M98data->D = Gcode2d(ptext,"D");
			}
			if(strstr(ptext,"T")) 
			{
				M98data->T = Gcode2d(ptext,"T");
			}
/////////////////////////////////////////寻找子程序////////////////////////////////////////////////////////			
			do{
				i=0;
				do
				{
					reallength = _read(fd, ptext+i, 1 );
					if(reallength == 0)
					{
						return 0;
					}
					
				  
				}while((ptext[i] != 'N'));	//找'N'
				
				do{	
					reallength = _read(fd, ptext+i+1, 1 );
					if(reallength == 0)
					{
						return 0;
					}
					i++;

					k=1;
					if((ptext[i-1] == ';'))//找到'N'句结束
					{
						if(strstr(ptext,"N")) 
						{
							M98data->N = Gcode2d(ptext,"N");
						}
						k=0;
					}

				}while(k==1);

			}while(M98data->N != M98data->P);//看是否与P匹配

///////////////////////////////////////子程序译码///////////////////////////////////////////////////////////////
			
			M98_j = 0;
			M98_n = 0;
			do
			{

				if(g==1)
				{
					i = 1;
					ptext[0] = 'G';
					g=0;					
				}
				else if(m==1)
				{
					i = 1;
					ptext[0] = 'M';
					m=0;
					M98_n++;					
				}
				else 
				{
					i = 0;
				}

				//读一行
				do{
					do
					{
						reallength = _read(fd, ptext+i, 1 );
						if(reallength == 0)
						{
							return 0;
						}

					}while((ptext[i] == '\n')||(ptext[i] == '\r'));
				            

					i=i+1;
					k=1;
					if((ptext[i-1] == ';')||(ptext[i-1] == 'M')||(ptext[i-1] == 'G'))k=0;
				}while(k==1);

				if(ptext[i-1] == 'M')
				{
					if(i!=1)
					{
						m=1;
					}
					else
					{
						do{
							reallength = _read(fd, ptext+i, 1 );
							if(reallength == 0)
							{
								return 0;
							}
							i=i+1;
							k=1;
							if((ptext[i-1] == ';')||(ptext[i-1] == 'G'))k=0;
						}while(k==1);
						
						M98_n++;

						if((i!=1)&&(ptext[i-1] == 'G'))
						{
							g=1;

						}
					}
				}

				else if(ptext[i-1] == 'G')
				{
					if((i!=1)&&(ptext[0] != 'G'))
					{
						g=1;
					}
					else
					{
						do{
							reallength = _read(fd, ptext+i, 1 );
							if(reallength == 0)
							{
								return 0;
							}

							i=i+1;
							k=1;
							if((ptext[i-1] == ';')||(ptext[i-1] == 'M'))k=0;
						}while(k==1);
						
						//n++;
						if((i!=1)&&(ptext[i-1] == 'M'))
						{
							m=1;
						}
					}
				}

				ptext[i-1] = ';';
				ptext[i] = 0;
				
				if((Frst == 0)&&((M98data->D != 0 )||(M98data->T != 0 )))
				{
					memset(szBuffer1, '0', sizeof(szBuffer1));
					memset(szBuffer2, '0', sizeof(szBuffer2));

					ptext[i-1] = 0;
					sprintf_s(szBuffer2, "%4.3f", M98data->D);
					strcpy(szBuffer1,"D");
					strcat(szBuffer1,szBuffer2);
					strcat(ptext,szBuffer1);
					i = i+ strlen(szBuffer1);

					ultoa(M98data->T,szBuffer2,10);
					strcpy(szBuffer1,"T");
					strcat(szBuffer1,szBuffer2);
					strcpy(szBuffer2,";");
					strcat(szBuffer1,szBuffer2);
					strcat(ptext,szBuffer1);
					i = i+ strlen(szBuffer1);

					ptext[i] = 0;
					Frst = 1;
				}
				pData->row_id = j;
				strcpy((pData+j)->pText,ptext);
				j++;
				M98_j++;
				if(ptext[0] == 'M') 
				{
					if(strstr(ptext,"M")) 
					{
						M98data->M = Gcode2d(ptext,"M");
					}
					if(M98data->M == 99)
					{
						M98_offset = _lseek(fd,M98_offset,SEEK_SET);
						M98_Flag = 1;
					}
				}
			}while(M98_Flag == 0);                                          //遇到子程序结束标志结束
			
			M98_Flag = 0;

////////////////////////////////////////////子程序合并/////////////////////////////////////////////////

			for(R_num = 0;R_num< ((M98data->NUM)-1)*M98_j;R_num++)
			{
				pData->row_id = j;
				strcpy((pData+j)->pText,(pData+j-M98_j)->pText);
				j ++;					
			}
		}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else                       //无子程序正常译码
		{
			if(ptext[i-1] == 'M')
			{
				if(i!=1)
				{
					m=1;
				}
				else
				{
					do{
						reallength = _read(fd, ptext+i, 1 );
						M98_offset++;
						        
						if(reallength == 0)
						{
							return 0;
						}
						i=i+1;
						k=1;
						if((ptext[i-1] == ';')||(ptext[i-1] == 'G')||(ptext[i-1] == 'P')||(ptext[i-1] == 'O'))k=0;
					}while(k==1);

					if((i!=1)&&(ptext[i-1] == 'G'))
					{
						g=1;

					}
					if((i!=1)&&(ptext[i-1] == 'P'))
					{
						p=1;

					}
					if((i!=1)&&(ptext[i-1] == 'O'))
					{
						o=1;

					}
				}
			}

			else if(ptext[i-1] == 'G')
			{
				if((i!=1)&&(ptext[0] != 'G'))
				{
					g=1;
				}
				else
				{
					do{
						reallength = _read(fd, ptext+i, 1 );
						M98_offset++;
						        
						if(reallength == 0)
						{
							return 0;
						}

						i=i+1;
						k=1;
						if((ptext[i-1] == ';')||(ptext[i-1] == 'M'))k=0;
					}while(k==1);
					
					//n++;
					if((i!=1)&&(ptext[i-1] == 'M'))
					{
						m=1;

					}
				}
			}

			ptext[i-1] = ';';
			ptext[i] = 0;

			if(i!=1)
			{
				pData->row_id = j;
				strcpy((pData+j)->pText,ptext);
				j++;
			}
				
		}

		k=1;
		*num = j;

	}
	return 0;
}
//2010-5-31
void Auto_Show2D(HWND hWnd)
{
	//隐藏三维显示时用到的控件
	ShowWindow(GetDlgItem(hWnd,IDW_AUTOGRAPH_3D),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO_BUP),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO_BLEFT),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO_BDOWN),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO_BRIGHT),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO_BZOOM_IN),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO_B_TURN_UP),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO_B_TURN_LEFT),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO_B_TURN_DOWN),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO_B_TURN_RIGHT),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO_EZOOM),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO_E_MOVE),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO_E_TURN),SW_HIDE);
	//显示二维绘图用到的控件
	ShowWindow(GetDlgItem(hWnd,IDC_AUTOGRAPH),SW_SHOW);
	SetWindowText (GetDlgItem(hWnd,IDC_BAUTO_3D_2D),"2D");
	NowAutoGraph2D_3D=2;
}
void Auto_Show3D(HWND hWnd)
{
	//隐藏三维显示时用到的控件
	ShowWindow(GetDlgItem(hWnd,IDW_AUTOGRAPH_3D),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO_BUP),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO_BLEFT),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO_BDOWN),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO_BRIGHT),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO_BZOOM_IN),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO_B_TURN_UP),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO_B_TURN_LEFT),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO_B_TURN_DOWN),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO_B_TURN_RIGHT),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO_EZOOM),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO_E_MOVE),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO_E_TURN),SW_SHOW);
	//显示二维绘图用到的控件
	ShowWindow(GetDlgItem(hWnd,IDC_AUTOGRAPH),SW_HIDE);
	SetWindowText (GetDlgItem(hWnd,IDC_BAUTO_3D_2D),"3D");
	NowAutoGraph2D_3D=3;
}
void Show3DInput()//2010-5-31
{
	int i;
	char szBuffer[31];
	GetWindowText (GetDlgItem(pDataInAutoInput->hWnd,IDC_AUTOPARAMEDIT),szBuffer,30);//得到输入文本框内所输入数值
	SetWindowText (GetDlgItem(pDataInAutoInput->hWnd,IDC_AUTOPARAMEDIT),"");
	i=atoi(szBuffer);;
	SetWindowText (pDataInAutoInput->wndCtrl,szBuffer);
	SetFocus(pDataInAutoInput->wndCtrl);
	SendMessage(pDataInAutoInput->wndCtrl,EM_SETSEL,-1,0);
	switch(AutoNowSelectID)
	{
	case IDC_AUTO_EZOOM:
		auto_zoom=double(i)/100;
		Auto3DDraw();
		break;
	case IDC_AUTO_E_MOVE:
		auto_move_step=i;
		break;
	case IDC_AUTO_E_TURN:
		auto_turn_step=i;
		break;
	}
	AutoInput=0;
}

DWORD Jump_Time_Compute(HWND hWnd,int DownTime,int Jump_Speed)
{
	double Jump_Time ;
	double Jump_Time_Temp_first,Jump_Time_Temp,Jump_Time_Temp0 ,Jump_Time_Temp1,Jump_Time_Temp2;
	
	int Jump_Num_All;
	int Jump_Height;
	int n=1;
	double N = 0.0;
	double Sway_R;
	int Jump_Select_Center;

	Jump_Height = ioread(hWnd,NCPCIhDevice,DSP_PARAMETER_READ, DSP_PARAMETER_Jump_Height );
	Sway_R = (double)ioread(hWnd,NCPCIhDevice,DSP_PARAMETER_READ, DSP_PARAMETER_Sway_R )/1000;
	Jump_Select_Center = ioread(hWnd,NCPCIhDevice,DSP_PARAMETER_READ, DSP_PARAMETER_Jump_Select_Center);

	if(Jump_Select_Center == 1)
	{
		Jump_Num_All = ( Jump_Height - Z_Return_Center * 1000)/(JUMP_DELT * 1000);
		if((Jump_Num_All % 2) != 0) Jump_Num_All++;
	}
	else
	{
		Jump_Num_All = Jump_Height/(JUMP_DELT * 1000);
		if((Jump_Num_All % 2) != 0) Jump_Num_All++;
	}

	N = (double)Jump_Num_All * 2;
	//Jump_Time = 2 * Jump_Height*60/Jump_Speed*1000;
	Jump_Time_Temp = (0.01 * 20000 * 2000 / 50000) * (1.0/500 + (N-1)/1000);
	Jump_average_speed = (double)(2 * 60 * Jump_Height)/(Jump_Time_Temp * 1000);
	DSP_data.PARA_Jump_Vel_Max = 1000;
	Jump_Time_Temp0 = 0.0 ;
	for(n=1;((Jump_average_speed<(Jump_Speed-30) )||(Jump_average_speed > (Jump_Speed+30)))&&( n < 20 );n++)
	{
		Jump_Time_Temp0 = 0.01*20000*2000/50000*4/(n*1000);
		Jump_Time_Temp1 = +0.01*20000*2000/50000*(N-4*n-1)/((n+1)*1000);
		Jump_Time_Temp2 = -0.01*20000*2000/50000*(N-4*(n-1)-1)/(n*1000);

		Jump_Time_Temp = Jump_Time_Temp + Jump_Time_Temp0 + Jump_Time_Temp1 + Jump_Time_Temp2 ;
		Jump_average_speed = (double)(2 * 60 * Jump_Height)/(Jump_Time_Temp * 1000);
		DSP_data.PARA_Jump_Vel_Max = (n+1)*1000;

	}


/*	if(Jump_Speed == 300)
	{
		if( Jump_Select_Center == 1)
		{
			Jump_Time = sqrt( Sway_R * Sway_R + Z_Return_Center * Z_Return_Center)							//向中心移动时间，速度为1mm/s
						+ 0.02 + 0.01 + 0.02/3 + 0.005 + 0.02/6 + 0.02/8 + 0.02/8 + 0.02/6 + 0.005 + 0.01 + 0.001*(Jump_Num_All - 10)    //1mm为0.15833s，慢加速从2000puls/s开始到8000，快加速从8000到20000
						+ (0.01 + 0.005 + 0.02/6 + 0.02/8) * 2 + 0.001*(Jump_Num_All - 9)												//0.1326s，快减速从20000到4000
						+ (double)DownTime/1000;		
		}
		else
		{
			Jump_Time = 0.02 + 0.01 + 0.02/3 + 0.005 + 0.02/6 + 0.02/8 + 0.02/8 + 0.02/6 + 0.005 + 0.01 + 0.001*(Jump_Num_All - 10)    //1mm为0.15833
						+ (0.01 + 0.005 + 0.02/6 + 0.02/8) * 2 + 0.001*(Jump_Num_All - 9)												//0.1326
						+ (double)DownTime/1000;			
		}

	}
	else if(Jump_Speed == 200)
	{
	
	}
*/

	Jump_Time = Jump_Time_Temp * 1000 + DownTime + 2*(sqrt( Sway_R * Sway_R + Z_Return_Center * Z_Return_Center)*20000*2000/50000);
	if(((int)Jump_Time % 2) != 0) Jump_Time++;
	return Jump_Time;

}

void DSP_Parameter_Transport(HWND hWnd)
{
		DSP_data.PARA_DownTime_Z_or_XYZ = 1;
		
		if(DSP_data.PARA_Jump_Select_Center == 0)
			DSP_data.PARA_DownTime_Flag = 0;
		else 
			DSP_data.PARA_DownTime_Flag = 1;

		if(DSP_data.PARA_Sway_A != 0)
			iowrite(hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_Sway_A, DSP_data.PARA_Sway_A);

		if(EXPERT == 0)
		{
			if(DSP_data.PARA_X == 3)
				iowrite(hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_Sway_A_Type, 1);
			else
				iowrite(hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_Sway_A_Type, 1); //将来改成2
		}
		else if((EXPERT == 1)||(EXPERT == 2)||(EXPERT == 3))
			iowrite(hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_Sway_A_Type, 2);
			
		iowrite(hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_Sway_R, DSP_data.PARA_Sway_R);
		iowrite(hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_Sway_Polygon_N, DSP_data.PARA_Sway_Polygon_N);
		iowrite(hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_Sway_Switch_Flag,DSP_data.PARA_Sway_Switch_Flag);
		iowrite(hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_Sway_FreeOr3D,DSP_data.PARA_Sway_FreeOr3D);
		iowrite(hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_Sway_F_Mode,DSP_data.PARA_Sway_F_Mode);

		iowrite(hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_DownTime_Z_or_XYZ, DSP_data.PARA_DownTime_Z_or_XYZ);
		iowrite(hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_Jump_Select_Center,DSP_data.PARA_Jump_Select_Center);
		iowrite(hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_DownTime_Flag,DSP_data.PARA_DownTime_Flag); 
		iowrite(hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_Jump_Height,DSP_data.PARA_Jump_Height); 
		iowrite(hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_DownTime, Jump_Time_Compute(hWnd,DSP_data.PARA_DownTime,DSP_data.PARA_Jump_Average_Speed)); 
		iowrite(hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_Jump_Vel_Max, DSP_data.PARA_Jump_Vel_Max); 

		iowrite(hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_SpeedChk_Flag_ADR, 0X01);  //向DSP下传SpeedChk_Flag
		iowrite(hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_DDA_Vcontrol_ADR, 0X01);  //向DSP下传DDA_Vcontrol
		iowrite(hWnd,NCPCIhDevice,DSP_PARAMETER_WRITE, DSP_PARAMETER_Frst_Start_Decode_Flag_ADR, 0X01);  //向DSP下传Frst_Start_Decode_Flag



}