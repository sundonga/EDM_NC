#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;								// 当前实例
extern PNOTEINFO pNoteInfo;
GraphParam EditGraphParam;
int edit_regraph_flag = 0;
LPNCDATA pDataEditNcGraphMem;

//2010-5-6
extern float edit_up_down;
extern float edit_right_left;
extern float edit_zoom;
extern HWND hWndGraph;
extern float edit_turn_up_down;
extern float edit_turn_right_left;

extern float edit_move_step;
extern float edit_turn_step;
extern double ZERO;

extern int calculateNumber;
extern double minLength;
extern double maxLength;
extern void Draw3D();
extern void Draw3DReset();

int SHOW_2D_3D=3;//显示二维或三维图像2表示显示二维图像，3表示显示三维图像

//2010-6-24
bool isEdit;
LPCmdThreadParam  pDataInEdit;
//end 2010-6-24
void EditThreadProc();
int NowSelectID;//标记当前选用的EDIT文本框的ID号
//下面变量分别为按钮所用到的位图句柄
HBITMAP graph_hbm_up;
HBITMAP graph_hbm_up_click;
HBITMAP graph_hbm_right;
HBITMAP graph_hbm_right_click;
HBITMAP graph_hbm_down;
HBITMAP graph_hbm_down_click;
HBITMAP graph_hbm_left;
HBITMAP graph_hbm_left_click;
HBITMAP graph_hbm_zoom_in;
HBITMAP graph_hbm_zoom_in_click;
HBITMAP graph_hbm_zoom_out;
HBITMAP graph_hbm_zoom_out_click;
HBITMAP graph_hbm_turn_up;
HBITMAP graph_hbm_turn_up_click;
HBITMAP graph_hbm_turn_right;
HBITMAP graph_hbm_turn_right_click;
HBITMAP graph_hbm_turn_down;
HBITMAP graph_hbm_turn_down_click;
HBITMAP graph_hbm_turn_left;
HBITMAP graph_hbm_turn_left_click;
//end
void Show2D(HWND);//显示二维绘图
void Show3D(HWND);//显示三维绘图
//2010-5-6

static void InitPathCombo(HWND,char*);
void listviewproc(HWND);
void GetFileAndDirList( HWND,char*);
void instolistview (HWND,PFILEINFO);

const MessageProc editMessages[]={
	    WM_CREATE,               goCreateEdit,
		WM_COMMAND,              goCommandEdit,
		WM_PAINT,                goPaintEdit,
		WM_NOTIFY,               goEditListview,
		WM_DRAWITEM,			 goDrawItemEdit,//2010-5-6
		WM_KEYDOWN,				 goKeydownEdit,//2010-5-13
		WM_DESTROY,              goDestroyEdit
		};
const CommandProc editCommands[]={
	IDC_EPATH,               goEditPathCmd,
	IDC_BTNNEW,              goEditNewCmd,
	IDC_BTNDELET,            goEditDeletCmd,
	IDC_BTNOPEN,             goEditOpenCmd,
	IDC_BTNSAVE,             goEditSaveCmd,
	IDC_MLEDIT,              goEditMleditCmd,
	IDC_BTNEDITCLOSE,        goEditCloseCmd,
	IDC_BTNDRAW,             goEditDrawCmd,
	IDC_BTNGRAPHRESET,       goEditDrawResetCmd
};
CtlWndStruct editMenuBtns[]={
		{WS_EX_CLIENTEDGE,TEXT("COMBOBOX"),IDC_EPATH,TEXT(""),30,10,300,150,WS_CHILD|WS_VISIBLE |WS_TABSTOP |CBS_DROPDOWN},//|CBS_NOTIFY |CBS_READONLY}
		{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_MLEDIT,TEXT(""),10,45,482,480,WS_VISIBLE|WS_BORDER|WS_CHILD|WS_HSCROLL|WS_VSCROLL|ES_UPPERCASE|ES_MULTILINE },
		{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_EFNAME,TEXT(""),400,10,100,25,WS_VISIBLE|WS_CHILD|WS_BORDER|WS_TABSTOP},//|ES_AUTOWRAP|ES_AUTOSELECT}
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNNEW,TEXT("NEW"),10,540,90,30,WS_VISIBLE|WS_BORDER|WS_CHILD},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNOPEN,TEXT("OPEN"),100,540,90,30,WS_VISIBLE|WS_BORDER|WS_CHILD},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNSAVE,TEXT("SAVE"),190,540,90,30,WS_VISIBLE|WS_BORDER|WS_CHILD},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNEDITCLOSE,TEXT("CLOSE"),280,540,90,30,WS_VISIBLE|WS_BORDER|WS_CHILD},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNDELET,TEXT("DELETE"),370,540,90,30,WS_VISIBLE|WS_BORDER|WS_CHILD},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNDRAW,TEXT("DRAW"),480,542,90,30,WS_VISIBLE|WS_BORDER|WS_CHILD},//2010-5-6
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNGRAPHRESET,TEXT("RESET"),480,592,90,30,WS_VISIBLE|WS_BORDER|WS_CHILD},//2010-5-6
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_EDIT_BUP,TEXT(""),705,547,30,18,WS_CHILD|WS_VISIBLE| BS_OWNERDRAW},//2010-5-6
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_EDIT_BLEFT,TEXT(""),687,565,18,30,WS_CHILD|WS_VISIBLE| BS_OWNERDRAW},//2010-5-6
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_EDIT_BDOWN,TEXT(""),705,595,30,18,WS_CHILD|WS_VISIBLE| BS_OWNERDRAW},//2010-5-6
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_EDIT_BRIGHT,TEXT(""),735,565,18,30,WS_CHILD|WS_VISIBLE| BS_OWNERDRAW},//2010-5-6
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_EDIT_BZOOM_IN,TEXT(""),600,535,30,30,WS_CHILD|WS_VISIBLE| BS_OWNERDRAW},//2010-5-6
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_EDIT_BZOOM_OUT,TEXT(""),600,595,30,30,WS_CHILD|WS_VISIBLE| BS_OWNERDRAW},//2010-5-6
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_EDIT_B_TURN_UP,TEXT(""),810,535,30,30,WS_CHILD|WS_VISIBLE| BS_OWNERDRAW},//2010-5-6
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_EDIT_B_TURN_LEFT,TEXT(""),780,565,30,30,WS_CHILD|WS_VISIBLE| BS_OWNERDRAW},//2010-5-6
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_EDIT_B_TURN_DOWN,TEXT(""),810,595,30,30,WS_CHILD|WS_VISIBLE| BS_OWNERDRAW},//2010-5-6
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_EDIT_B_TURN_RIGHT,TEXT(""),840,565,30,30,WS_CHILD|WS_VISIBLE| BS_OWNERDRAW},//2010-5-6
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNYZ,TEXT("Y-Z"),670,565,90,30,WS_VISIBLE|WS_BORDER|WS_CHILD},//2010-5-6
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNXY,TEXT("X-Y"),760,565,90,30,WS_VISIBLE|WS_BORDER|WS_CHILD},//2010-5-6
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNXZ,TEXT("X-Z"),850,565,90,30,WS_VISIBLE|WS_BORDER|WS_CHILD},//2010-5-6
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_EDIT_3D_2D,TEXT("3D"),900,515,100,30,WS_VISIBLE|WS_BORDER|WS_CHILD},//2010-5-6
		{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_EDIT_EZOOM,TEXT(""),590,565,50,30,WS_VISIBLE|WS_BORDER|WS_CHILD|ES_RIGHT|ES_NOHIDESEL},//2010-5-13
		{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_EDIT_E_MOVE,TEXT(""),705,565,30,30,WS_VISIBLE|WS_BORDER|WS_CHILD|ES_RIGHT|ES_NOHIDESEL},//2010-5-13
		{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_EDIT_E_TURN,TEXT(""),810,565,30,30,WS_VISIBLE|WS_BORDER|WS_CHILD|ES_RIGHT|ES_NOHIDESEL},//2010-5-13
		{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_EDIT_E_NUMBER,TEXT(""),350,570,50,20,WS_VISIBLE|WS_BORDER|WS_CHILD|ES_RIGHT|ES_NOHIDESEL},//2010-5-13
		{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_EDIT_E_MIN,TEXT(""),350,590,50,20,WS_VISIBLE|WS_BORDER|WS_CHILD|ES_RIGHT|ES_NOHIDESEL},//2010-5-13
		{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_EDIT_E_MAX,TEXT(""),350,610,50,20,WS_VISIBLE|WS_BORDER|WS_CHILD|ES_RIGHT|ES_NOHIDESEL},//2010-5-13
		{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_EDIT_EDIT,TEXT(""),0,620,250,30,WS_VISIBLE|WS_BORDER|WS_CHILD|ES_UPPERCASE|ES_NUMBER}//2010-5-13
};
//子窗口菜单列表
CtlWndStruct editChildWnds[]={
	{WS_EX_CLIENTEDGE,TEXT("GRAPH"),IDC_GRAPH,TEXT(""),510,10,500,500,WS_CHILD|WS_BORDER|WS_CLIPSIBLINGS},//2010-5-6三维OPENGL对应子窗口
	{WS_EX_CLIENTEDGE,TEXT("GRAPHWND"),IDC_GRAPH_2D,TEXT(""),510,10,500,500,WS_VISIBLE|WS_BORDER|WS_CHILD}//二维对应子窗口
};

int MyRegisterClassEdit(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	INITCOMMONCONTROLSEX icex;
	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)editWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//(LPCTSTR)ID_Menu;//(LPCTSTR)IDC_EX1;
	wcex.lpszClassName	= TEXT("EDITWND");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
//注册编辑窗口类
	if(!RegisterClassEx(&wcex)) return 0;
	if(!MyRegisterClassGraph(hInst)) return 0;
	if(!MyRegisterClassGraph2D(hInst)) return 0;
	icex.dwICC=ICC_LISTVIEW_CLASSES;
	icex.dwSize=sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&icex);
	return 1;
}

//编辑窗口过程体
LRESULT CALLBACK editWndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;

	//查找对应的消息ID,并执行对应的消息处理过程
	for(i=0; i<dim(editMessages);i++)
	{
		if(msgCode == editMessages[i].uCode) return(*editMessages[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//对不存在的消息，调用缺省窗口过程
	return DefWindowProc(hWnd, msgCode, wParam, lParam);
}
LRESULT goCreateEdit(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	
    int i;
	char szBuffer[10];	
//创建编辑窗口菜单
	for(i=0;i<dim(editMenuBtns);i++)
	{
		CreateWindowEx(editMenuBtns[i].dwExStyle,
			editMenuBtns[i].szClass,
			editMenuBtns[i].szTitle,
			editMenuBtns[i].lStyle,
			editMenuBtns[i].x,
			editMenuBtns[i].y,
			editMenuBtns[i].cx,
			editMenuBtns[i].cy,
			hWnd,
			(HMENU)editMenuBtns[i].nID,
			hInst,
			NULL
			);
	}
	CreateWindowEx(WS_EX_CLIENTEDGE,
			WC_LISTVIEW,
			TEXT(""),
			WS_VISIBLE|WS_BORDER|WS_CHILD|WS_HSCROLL|WS_VSCROLL|LVS_REPORT,
			10,45,482,480,
			hWnd,
			(HMENU)IDC_LISTVIEW,
			hInst,
			NULL
			);	
//创建编辑窗口的子窗口	
	for(i=0;i<dim(editChildWnds);i++)
	{
		CreateWindowEx(editChildWnds[i].dwExStyle,
			editChildWnds[i].szClass,
			editChildWnds[i].szTitle,
			editChildWnds[i].lStyle,
			editChildWnds[i].x,
			editChildWnds[i].y,
			editChildWnds[i].cx,
			editChildWnds[i].cy,
			hWnd,
			(HMENU)editChildWnds[i].nID,
			hInst,
			NULL
			);
	}	
	//显示三维或二维窗口，默认是三位窗口
	if(SHOW_2D_3D==3)Show3D(hWnd);
	else Show2D(hWnd);

	listviewproc(hWnd);

	//2010-5-6载入按钮对应的位图
	graph_hbm_up=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP19));
	graph_hbm_up_click=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP20));
	graph_hbm_right=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP7));
	graph_hbm_right_click=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP8));
	graph_hbm_down=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
	graph_hbm_down_click=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP2));
	graph_hbm_left=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP3));
	graph_hbm_left_click=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP4));
	graph_hbm_zoom_in=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP21));
	graph_hbm_zoom_in_click=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP22));
	graph_hbm_zoom_out=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP23));
	graph_hbm_zoom_out_click=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP24));
	graph_hbm_turn_up=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP17));
	graph_hbm_turn_up_click=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP18));
	graph_hbm_turn_right=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP15));
	graph_hbm_turn_right_click=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP16));
	graph_hbm_turn_down=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP11));
	graph_hbm_turn_down_click=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP12));
	graph_hbm_turn_left=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP13));
	graph_hbm_turn_left_click=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP14));
//初始化各EDIT框中内容
	i=int(edit_move_step);
	itoa(i,szBuffer,10);
	SetWindowText (GetDlgItem(hWnd,IDC_EDIT_E_MOVE),szBuffer);
	i=int(edit_turn_step);
	itoa(i,szBuffer,10);
	SetWindowText (GetDlgItem(hWnd,IDC_EDIT_E_TURN),szBuffer);
	i=int(edit_zoom*100);
	itoa(i,szBuffer,10);
	SetWindowText (GetDlgItem(hWnd,IDC_EDIT_EZOOM),szBuffer);
	i=calculateNumber;
	itoa(i,szBuffer,10);
	SetWindowText (GetDlgItem(hWnd,IDC_EDIT_E_NUMBER),szBuffer);
//由于浮点数转为字符串函数方法未找到，故而在本过程中浮点数通过乘一定的倍数直接转换为整数，在重新设置浮点数是也是将得到的整数除对应的倍数得到
	i=int(minLength*1000);
	itoa(i,szBuffer,10);
	SetWindowText (GetDlgItem(hWnd,IDC_EDIT_E_MIN),szBuffer);
	i=int(maxLength*1000);
	itoa(i,szBuffer,10);
	SetWindowText (GetDlgItem(hWnd,IDC_EDIT_E_MAX),szBuffer);
	//2010-5-6
	//2010-6-24
	isEdit=FALSE;
	pDataInEdit = (LPCmdThreadParam)HeapAlloc(GetProcessHeap(),
											HEAP_ZERO_MEMORY,
											sizeof(CmdThreadParam)
											);
	if(pDataInEdit == NULL)
	{
		return 1;
	}
	//end 2010-6-24	
	return 0;
}

LRESULT goCommandEdit(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//定义菜单ID号和通知单
	int menuID, notifyCode;
	//定义窗口句柄
	HWND wndCtrl;
	int i;
	char szBuffer[10]; 
	menuID    = LOWORD(wParam); 
	notifyCode = HIWORD(wParam); 
	wndCtrl = (HWND) lParam;
	//查找对应的控件ID,并执行对应的消息处理过程
	for(i=0; i<dim(editCommands);i++)
	{
		if(menuID == editCommands[i].uCode) return(*editCommands[i].functionName)(hWnd,wndCtrl,menuID,notifyCode);
	}
//	2010-5-6
//旋转、平移、放大缩小对应的参数处理过程
	switch(LOWORD(wParam))
	{
	case IDC_EDIT_B_TURN_UP:
		edit_turn_up_down+=edit_turn_step;
		Draw3D();
		break;
	case IDC_EDIT_B_TURN_DOWN:
		edit_turn_up_down-=edit_turn_step;
		Draw3D();
		break;
	case IDC_EDIT_B_TURN_LEFT:
		edit_turn_right_left-=edit_turn_step;
		Draw3D();
		break;
	case IDC_EDIT_B_TURN_RIGHT:
		edit_turn_right_left+=edit_turn_step;
		Draw3D();
		break;
	case IDC_EDIT_BUP:
		edit_up_down+=edit_move_step;
		Draw3D();
		break;
	case IDC_EDIT_BDOWN:
		edit_up_down-=edit_move_step;
		Draw3D();
		break;
	case IDC_EDIT_BLEFT:
		edit_right_left-=edit_move_step;
		Draw3D();
		break;
	case IDC_EDIT_BRIGHT:
		edit_right_left+=edit_move_step;
		Draw3D();
		break;
	case IDC_EDIT_BZOOM_IN:
		//对于放大缩小按钮每次点击时实时显示当前的放大缩小倍数
		edit_zoom+=0.01;
		i=int(edit_zoom*100);
		itoa(i,szBuffer,10);
		SetWindowText (GetDlgItem(hWnd,IDC_EDIT_EZOOM),szBuffer);
		Draw3D();
		break;
	case IDC_EDIT_BZOOM_OUT:
		edit_zoom-=0.01;
		if(edit_zoom<=0.0)edit_zoom=1.0;
		i=int(edit_zoom*100);
		itoa(i,szBuffer,10);
		SetWindowText (GetDlgItem(hWnd,IDC_EDIT_EZOOM),szBuffer);
		Draw3D();
		break;
	case IDC_EDIT_3D_2D:
		if(SHOW_2D_3D==3)Show2D(hWnd);
		else Show3D(hWnd);
		InvalidateRect(hWnd,NULL,TRUE);
		SendMessage(hWnd,WM_PAINT,NULL,NULL);
	default:
		break;
	}
	//各EDIT文本框对应的处理过程
	if(HIWORD(wParam)==EN_SETFOCUS)
	{
		//去除个文本框高亮
		SendMessage(GetDlgItem(hWnd,IDC_EDIT_EZOOM),EM_SETSEL,-1,0);
		SendMessage(GetDlgItem(hWnd,IDC_EDIT_E_MOVE),EM_SETSEL,-1,0);
		SendMessage(GetDlgItem(hWnd,IDC_EDIT_E_TURN),EM_SETSEL,-1,0);
		SendMessage(GetDlgItem(hWnd,IDC_EDIT_E_NUMBER),EM_SETSEL,-1,0);
		SendMessage(GetDlgItem(hWnd,IDC_EDIT_E_MIN),EM_SETSEL,-1,0);
		SendMessage(GetDlgItem(hWnd,IDC_EDIT_E_MAX),EM_SETSEL,-1,0);
		switch(LOWORD(wParam))
		{
		case IDC_EDIT_EZOOM:
			pDataInEdit->hWnd = hWnd;
			pDataInEdit->wndCtrl = GetDlgItem(hWnd,LOWORD(wParam));
			NowSelectID =IDC_EDIT_EZOOM;
			pDataInEdit->notifyCode = 0;
			SetFocus(GetDlgItem(hWnd,IDC_EDIT_EDIT));
			SendMessage(GetDlgItem(hWnd,LOWORD(wParam)),EM_SETSEL,0,-1);
			isEdit=TRUE;
			break;
		case IDC_EDIT_E_MOVE:
			pDataInEdit->hWnd = hWnd;
			pDataInEdit->wndCtrl = GetDlgItem(hWnd,LOWORD(wParam));
			NowSelectID= IDC_EDIT_E_MOVE;
			pDataInEdit->notifyCode = 0;
			SetFocus(GetDlgItem(hWnd,IDC_EDIT_EDIT));
			SendMessage(GetDlgItem(hWnd,LOWORD(wParam)),EM_SETSEL,0,-1);
			isEdit=TRUE;
			break;
		case IDC_EDIT_E_TURN:
			pDataInEdit->hWnd = hWnd;
			pDataInEdit->wndCtrl = GetDlgItem(hWnd,LOWORD(wParam));
			NowSelectID =IDC_EDIT_E_TURN;
			pDataInEdit->notifyCode = 0;
			SetFocus(GetDlgItem(hWnd,IDC_EDIT_EDIT));
			SendMessage(GetDlgItem(hWnd,LOWORD(wParam)),EM_SETSEL,0,-1);
			isEdit=TRUE;
			break;
		case IDC_EDIT_E_NUMBER:
			pDataInEdit->hWnd = hWnd;
			pDataInEdit->wndCtrl = GetDlgItem(hWnd,LOWORD(wParam));
			NowSelectID =IDC_EDIT_E_NUMBER;
			pDataInEdit->notifyCode = 0;
			SetFocus(GetDlgItem(hWnd,IDC_EDIT_EDIT));
			SendMessage(GetDlgItem(hWnd,LOWORD(wParam)),EM_SETSEL,0,-1);
			isEdit=TRUE;
			break;
		case IDC_EDIT_E_MIN:
			pDataInEdit->hWnd = hWnd;
			pDataInEdit->wndCtrl = GetDlgItem(hWnd,LOWORD(wParam));
			NowSelectID =IDC_EDIT_E_MIN;
			pDataInEdit->notifyCode = 0;
			SetFocus(GetDlgItem(hWnd,IDC_EDIT_EDIT));
			SendMessage(GetDlgItem(hWnd,LOWORD(wParam)),EM_SETSEL,0,-1);
			isEdit=TRUE;
			break;
		case IDC_EDIT_E_MAX:
			pDataInEdit->hWnd = hWnd;
			pDataInEdit->wndCtrl = GetDlgItem(hWnd,LOWORD(wParam));
			NowSelectID =IDC_EDIT_E_MAX;
			pDataInEdit->notifyCode = 0;
			//将当前选定文本框设为高亮
			SetFocus(GetDlgItem(hWnd,IDC_EDIT_EDIT));
			SendMessage(GetDlgItem(hWnd,LOWORD(wParam)),EM_SETSEL,0,-1);
			isEdit=TRUE;
			break;
		default:
			break;
		}
	}
//	2010-5-6
	return 0;
}

LRESULT goPaintEdit(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//定义画板结构体对象
	PAINTSTRUCT ps;
	RECT rect;
	HDC hdc;
	hdc=BeginPaint(hWnd,&ps);;
	SetBkMode(hdc,TRANSPARENT);
	SetTextColor(hdc,RGB(0,0,0));
	SetRect(&rect,0,5,30,35);
	DrawText(hdc,"File",-1,&rect,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,330,5,400,35);
	DrawText(hdc,"FileName",-1,&rect,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	if(SHOW_2D_3D==3)
	{
		SetRect(&rect,640,560,700,595);
		DrawText(hdc,"*0.01",-1,&rect,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		SetRect(&rect,230,570,350,590);
		DrawText(hdc,"POINTNUMBER",-1,&rect,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
		SetRect(&rect,250,590,350,610);
		DrawText(hdc,"MINLENGTH",-1,&rect,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
		SetRect(&rect,250,610,350,630);
		DrawText(hdc,"MAXLENGTH",-1,&rect,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
		SetRect(&rect,400,590,500,610);
		DrawText(hdc,"*0.001",-1,&rect,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		SetRect(&rect,400,610,500,630);
		DrawText(hdc,"*0.001",-1,&rect,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	}
	EndPaint(hWnd,&ps);
	return 0;
}
//2010-5-6
//自绘按钮对应绘制按钮函数
LRESULT goDrawItemEdit(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
    LPDRAWITEMSTRUCT lpdis; 
	lpdis = (LPDRAWITEMSTRUCT) lParam; 
    hdc = CreateCompatibleDC(lpdis->hDC);
	switch(lpdis->CtlID)
	{
	case IDC_EDIT_BUP:
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
	case IDC_EDIT_BLEFT:
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
	case IDC_EDIT_BDOWN:
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

	case IDC_EDIT_BRIGHT:
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
	case IDC_EDIT_BZOOM_IN:
		if (lpdis->itemState & ODS_SELECTED)  // if selected 
			SelectObject(hdc, graph_hbm_zoom_in_click); 
		else 
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
	case IDC_EDIT_BZOOM_OUT:
		if (lpdis->itemState & ODS_SELECTED)  // if selected 
			SelectObject(hdc, graph_hbm_zoom_out_click); 
		else 
			SelectObject(hdc, graph_hbm_zoom_out); 
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
	case IDC_EDIT_B_TURN_UP:
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
	case IDC_EDIT_B_TURN_DOWN:
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
	case IDC_EDIT_B_TURN_LEFT:
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
	case IDC_EDIT_B_TURN_RIGHT:
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

LRESULT goKeydownEdit(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int menuID, notifyCode;		
	//定义窗口句柄
	HWND wndCtrl;
	menuID    = LOWORD(wParam); 
	notifyCode = HIWORD(wParam); 
	wndCtrl = (HWND) lParam;
	if(isEdit)EditThreadProc();
	return 0;
}
void EditThreadProc()
{
	int i;
	char szBuffer[31];
	GetWindowText (GetDlgItem(pDataInEdit->hWnd,IDC_EDIT_EDIT),szBuffer,30);//得到输入文本框内所输入数值
	SetWindowText (GetDlgItem(pDataInEdit->hWnd,IDC_EDIT_EDIT),"");
	i=atoi(szBuffer);;
	SetWindowText (pDataInEdit->wndCtrl,szBuffer);
	SetFocus(pDataInEdit->wndCtrl);
	SendMessage(pDataInEdit->wndCtrl,EM_SETSEL,-1,0);
	switch(NowSelectID)
	{
	case IDC_EDIT_EZOOM:
		edit_zoom=double(i)/100;
		Draw3D();
		break;
	case IDC_EDIT_E_MOVE:
		edit_move_step=i;
		break;
	case IDC_EDIT_E_TURN:
		edit_turn_step=i;
		break;
	case IDC_EDIT_E_MAX:
		maxLength=double(i)/1000;
		break;
	case IDC_EDIT_E_MIN:
		minLength=double(i)/1000;
		break;
	case IDC_EDIT_E_NUMBER:
		calculateNumber=double(i);
		break;
	}
	isEdit=FALSE;
}
//2010-5-6
LRESULT goDestroyEdit(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		PostQuitMessage(0);
		return 0;
}
//Listview的NOFITY消息执行过程
// 双击目录时有问题？？？
LRESULT goEditListview(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//定义列表窗口ID号
	int menuID;
	//定义列表窗口句柄
	HWND wndCtrl;
	//定义消息信息结构体指针
	LPNMHDR pnmh;
    //定义指向包含消息结构和列表信息结构成员的结构体
	
	char chPath[MAX_PATH+1];
	int nSelItem;
	TCHAR szBuffer[_MAX_FNAME+1];
	menuID    = int(wParam); 
	pnmh = (LPNMHDR)lParam;
	wndCtrl = pnmh->hwndFrom;	
	
    memset (chPath, 0, sizeof(chPath));
    GetWindowText (GetDlgItem (hWnd, IDC_EPATH), chPath,MAX_PATH);

	if(menuID==IDC_LISTVIEW)
	{
		if(pnmh->code==NM_DBLCLK)
		{			
			nSelItem =-1;
			nSelItem =  ListView_GetNextItem(wndCtrl,-1,LVNI_SELECTED);				
											
			if (nSelItem >= 0 )
			{					
				ListView_GetItemText(wndCtrl,nSelItem,0,szBuffer,_MAX_FNAME);					
				if(GetFileAttributes(szBuffer)==INVALID_FILE_ATTRIBUTES)
				{
					if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_HIDE);
					MessageBox(hWnd,"can not get FileAttributes in goEditListview funciotn",NULL,NULL);
					if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);
				}
				else if(GetFileAttributes(szBuffer)==FILE_ATTRIBUTE_DIRECTORY)
				{
					if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_HIDE);
					MessageBox(hWnd,"here",NULL,NULL);
					if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);
		       		wsprintf (chPath, "%s/%s", strcmp (chPath, "/") == 0 ? "" : chPath, szBuffer);
                	
            		GetFileAndDirList (GetDlgItem (hWnd, IDC_EPATH), chPath);

					SetWindowText (GetDlgItem (hWnd, IDC_EPATH), chPath);
					if (CB_ERR == SendMessage (GetDlgItem (hWnd, IDC_EPATH), CB_FINDSTRINGEXACT, 0,(LPARAM)chPath))
                		SendMessage (GetDlgItem (hWnd, IDC_EPATH), CB_INSERTSTRING, 0,(LPARAM)chPath);
				}
			}
		}
		else if(pnmh->code==NM_CLICK)
		{
			nSelItem =-1;			
			nSelItem =  ListView_GetNextItem(wndCtrl,-1,LVNI_SELECTED);
			SetFocus(GetParent(wndCtrl));
			ListView_SetItemState(wndCtrl,-1,LVIS_SELECTED, 0XFF);
			if (nSelItem >= 0 )
			{
				ListView_SetItemState(wndCtrl,nSelItem,LVIS_DROPHILITED, 0XFF);
			}
		}
	}
	return 0;
}

LRESULT goEditMleditCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	if(notifyCode == EN_CHANGE)
	{
		pNoteInfo->ischanged = TRUE;
	}
	return 0;
}

LRESULT goEditPathCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	char filename[_MAX_FNAME+1];
	int index;

	if(notifyCode == CBN_SELCHANGE)
	{
		index=SendMessage(wndCtrl,CB_GETCURSEL,0,0);
		if(index>=0)
		{
			SendDlgItemMessage(hWnd,IDC_EPATH,CB_GETLBTEXT,index,(LPARAM)filename);
			GetFileAndDirList (hWnd, filename);
			ShowWindow(wndCtrl,SW_SHOW);
			ShowWindow(GetDlgItem(hWnd,IDC_MLEDIT),SW_HIDE);
			SetWindowText(GetDlgItem(hWnd,IDC_EFNAME),"");
		}
	}
	return 0;
}
//新建文件按扭函数
LRESULT goEditNewCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{

	int choise=0;
	char str[50];
	char filename[_MAX_FNAME+1];

	if(pNoteInfo->ischanged)
	{
		GetWindowText (GetDlgItem(hWnd,IDC_EFNAME), filename, _MAX_FNAME);
		strcpy(str,"do you want save file");
		strcat(str,filename);
		strcat(str,"?");
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_HIDE);
		choise = MessageBox (hWnd,
			str,
			"Save File",
			MB_YESNOCANCEL | MB_ICONQUESTION);
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);
		if(choise=IDYES)
		NBSave(hWnd);
	}

	ShowWindow(GetDlgItem(hWnd,IDC_MLEDIT),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_LISTVIEW),SW_HIDE);
	SetWindowText(GetDlgItem(hWnd,IDC_MLEDIT),"");

	SetWindowText (GetDlgItem(hWnd,IDC_EFNAME),"untitled.txt");							
	
    return 0;
}

//保存文件按扭函数
LRESULT goEditSaveCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	int choise=0;
	char str[50];
	char filename[_MAX_FNAME+1];
	
	
	if(pNoteInfo->ischanged)
    { 
		GetWindowText (GetDlgItem(hWnd,IDC_EFNAME), filename, _MAX_FNAME);
		strcpy(str,"do you want save file ");
		strcat(str,filename);
		strcat(str,"?");
		
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_HIDE);
		choise = MessageBox (hWnd,
               str,
               "Save File",
                MB_YESNOCANCEL | MB_ICONQUESTION); 
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);

		if(choise == IDYES)   NBSave(hWnd);
     }
	return 0;
}
//打开文件按扭函数
LRESULT goEditOpenCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	int nSelItem;
	TCHAR szBuffer[_MAX_FNAME+1]; 	
	char *buffer;
			
	buffer = (char *)HeapAlloc(GetProcessHeap(),
							HEAP_ZERO_MEMORY,
							MAX_FILELEN*sizeof(char)
									);
	if(buffer == NULL)
	{
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_HIDE);
		MessageBox(hWnd,"can not alloc heapmemory in function goEditOpenCmd",NULL,NULL);
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);
		return 1;
	}	

	nSelItem = -1;
	nSelItem =  ListView_GetNextItem(GetDlgItem(hWnd,IDC_LISTVIEW),-1,LVIS_DROPHILITED);
	
    if (nSelItem >= 0 ) 
	{			
		ListView_GetItemText(GetDlgItem(hWnd,IDC_LISTVIEW),nSelItem,0,szBuffer,_MAX_FNAME);
	
		if(!OpenFile(buffer,szBuffer,hWnd))
		{
			if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_HIDE);
			MessageBox(hWnd,"can not open file in goEditOpenCmd function",NULL,NULL);
			if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);
			return 1;
		}
		
		ShowWindow(GetDlgItem(hWnd,IDC_MLEDIT),SW_SHOW);
		ShowWindow(GetDlgItem(hWnd,IDC_LISTVIEW),SW_HIDE);
		SetWindowText (GetDlgItem(hWnd,IDC_MLEDIT), buffer);
		SetWindowText(GetDlgItem(hWnd,IDC_EFNAME),szBuffer);
		
	}

	if(HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,buffer) == 0)
	{
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_HIDE);
		MessageBox(hWnd,"can not free heapmemory in function goEditOpenCmd",NULL,NULL);
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);
		return 1;
	}
		return 0;
}
//删除文件按扭函数
LRESULT goEditDeletCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	
	int  nSelItem;
	int choose=0;
	TCHAR szBuffer[MAX_PATH+1];	

	nSelItem = -1;
	nSelItem =  ListView_GetNextItem(GetDlgItem(hWnd,IDC_LISTVIEW),-1,LVIS_DROPHILITED);
		
    if (nSelItem >= 0 ) 
	{
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_HIDE);
		choose = MessageBox (hWnd,
		"do you want delet file ?",
		"Delete File",
		MB_YESNOCANCEL | MB_ICONQUESTION);
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);
		
		if(choose==IDYES)
		{
			ListView_GetItemText(GetDlgItem(hWnd,IDC_LISTVIEW),nSelItem,0,szBuffer,_MAX_FNAME);
			remove(szBuffer);			

			GetCurrentDirectory(MAX_PATH+1,szBuffer);
			GetFileAndDirList(hWnd,szBuffer);
			ShowWindow(GetDlgItem(hWnd,IDC_LISTVIEW),SW_SHOW);
			ShowWindow(GetDlgItem(hWnd,IDC_MLEDIT),SW_HIDE);
		}
		else if(choose==IDCANCEL) return 0;
	}
	return 0;

}
//关闭文件按扭函数
LRESULT goEditCloseCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	int choise=0;
    char str[50];
    char filename[_MAX_FNAME+1];
	char chPath[MAX_PATH+1];

	if(pNoteInfo->ischanged)
	{
          
		GetWindowText (GetDlgItem(hWnd,IDC_EFNAME), filename, _MAX_FNAME);
		strcpy(str,"do you want save file ");
		strcat(str,filename);
		strcat(str,"?");

		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_HIDE);                                                                                                                       
		choise = MessageBox (hWnd,
				str,
				"Save File",
					MB_YESNOCANCEL | MB_ICONQUESTION);
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);
                    
		if(choise == IDYES)   NBSave(hWnd);
	}
	
	GetWindowText (GetDlgItem(hWnd, IDC_EPATH), chPath, MAX_PATH);
	GetFileAndDirList (hWnd, chPath);
   	ShowWindow(GetDlgItem(hWnd,IDC_LISTVIEW),SW_SHOW);
    ShowWindow(GetDlgItem(hWnd,IDC_MLEDIT),SW_HIDE);
   	SetWindowText(GetDlgItem(hWnd, IDC_EFNAME),"");
	return 0;
}


LRESULT goEditDrawCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	int nSelItem;
	TCHAR szBuffer[_MAX_FNAME+1]; 
	int decodeNum,compasateNum,tapeNum,ComputeNum;
	int all_decode_num,all_creat_num;
	int coordataNum;
	int DcompasateNumup,DcompasateNumlow;//11

	nc_data decodeData[2*DECODE_NUM_ONCE];
	nc_data coordata[2*DECODE_NUM_ONCE];
	nc_data compasateData[2*DECODE_NUM_ONCE];
	nc_data tapeData[2*DECODE_NUM_ONCE];
	nc_data ComputeData[2*DECODE_NUM_ONCE];
	nc_data DcompasateData[2*DECODE_NUM_ONCE];//11
	M_data MChild[2*DECODE_NUM_ONCE];
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
	
	double Dcompasate_Start_point_X,Dcompasate_Start_point_Y;//11
    double Dcompasate_Start_point_B,Dcompasate_Start_point_C;//11
    int Dcompasate_build_c_up,Dcompasate_build_c_low;//11
	nc_data *Dcompasate_cs;	//11
	LPNCCODE lpNcCodeMem;         //自动加工代码内存
	int NcCodeNum; //NC码数

	compasate_Start_point_X=0.;
	compasate_Start_point_Y=0.;
	compasate_build_c=0;
	
	tape_Start_point_X=0;
	tape_Start_point_Y=0;
	tape_Start_point_B=0;
	tape_Start_point_C=0;
	tape_build_c=0;

	Dcompasate_Start_point_X=0;//11
	Dcompasate_Start_point_Y=0;
	Dcompasate_Start_point_B=0;
	Dcompasate_Start_point_C=0;
	Dcompasate_build_c_up=0;
	Dcompasate_build_c_low=0;
	
	memset(decodeData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));
	memset(compasateData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));
	memset(ComputeData,0,2*DECODE_NUM_ONCE*sizeof(nc_data)); 
	memset(MChild,0,2*DECODE_NUM_ONCE*sizeof(M_data)); 
	memset(DcompasateData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));//
	memset(coordata,0,2*DECODE_NUM_ONCE*sizeof(nc_data));//11
	
	compasate_cs = (nc_data *)malloc(sizeof(nc_data));
	memset(compasate_cs,0,sizeof(nc_data));

	tape_cs = (nc_data *)malloc(sizeof(nc_data));
	memset(tape_cs,0,sizeof(nc_data));
	
	Dcompasate_cs = (nc_data *)malloc(sizeof(nc_data));//11
	memset(Dcompasate_cs,0,sizeof(nc_data));
	
	end_decode = 0;
	all_decode_num =0;
	all_creat_num = 0;
	
	//开设译码绘图内存
	pDataEditNcGraphMem = (LPNCDATA)HeapAlloc(GetProcessHeap(),
									HEAP_ZERO_MEMORY,
									MAX_NC_MEM*sizeof(nc_data)
									);
	if(pDataEditNcGraphMem == NULL)
	{
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_HIDE);
		MessageBox(hWnd,"can not alloc heapmemory in function goEditDrawCmd",NULL,NULL);
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);
		return 1;
	}
	lpNcCodeMem = (LPNCCODE)HeapAlloc(GetProcessHeap(),
								HEAP_ZERO_MEMORY,
								MAX_NC_MEM*sizeof(nc_code)
								);
	if(lpNcCodeMem == NULL)
	{
		MessageBox(hWnd,"can not alloc heapmemory in function AutoMachiningBuildThreadProc",NULL,NULL);
		return 1;
	}
	
	nSelItem = -1;
	nSelItem =  ListView_GetNextItem(GetDlgItem(hWnd,IDC_LISTVIEW),-1,LVIS_DROPHILITED);
	
			
    if (nSelItem >= 0 ) 
	{
		ListView_GetItemText(GetDlgItem(hWnd,IDC_LISTVIEW),nSelItem,0,szBuffer,_MAX_FNAME);
	}

	//打开译码文件
	fdEdit = _open(szBuffer, _O_RDWR);
 
	if (fdEdit <= 0)
    {   
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_HIDE);
        MessageBox (hWnd, "can not open file in goEditDrawCmd funciotn","Program", MB_OK | MB_ICONSTOP);
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);
        return 1;
    }
	ReadNcCodeFileToMem(hWnd,fdEdit,lpNcCodeMem,&NcCodeNum);

	do
	{
		if(decode(hWnd,lpNcCodeMem,decodeData,&decodeNum,&all_decode_num,&end_decode,MChild)==1) return 1; // 分段译码
		if(CoordinateSwitch( hWnd, decodeData,decodeNum, coordata,&coordataNum)==1) return 1;  //坐标变换
			
		if(decodeData->G_Dcompensate==152||decodeData->G_Dcompensate==153||decodeData->G_Dcompensate==150)//11
		{ 
			Dcompensate(hWnd,coordata,coordataNum,DcompasateData,&DcompasateNumup,&DcompasateNumlow,&Dcompasate_Start_point_X,&Dcompasate_Start_point_Y,&Dcompasate_Start_point_B,&Dcompasate_Start_point_C,&Dcompasate_build_c_up, &Dcompasate_build_c_low,compasate_cs);
			
		    if(DSP_Compute(hWnd,DcompasateData,DcompasateNumup,ComputeData,&ComputeNum)==1) return 1;
			CopyMemory(pDataEditNcGraphMem+all_creat_num,ComputeData,(ComputeNum*sizeof(nc_data)));	
			
			memset(DcompasateData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));//void *memset( void *dest, int c, size_t count );
			memset(ComputeData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));
			
			all_creat_num=all_creat_num + DcompasateNumup;
			ComputeNum = 0;
			DcompasateNumup=0;
			DcompasateNumlow=0;
		}
		else
		{
			if(compensate(hWnd,coordata,coordataNum,compasateData,&compasateNum,&compasate_Start_point_X,&compasate_Start_point_Y, &compasate_build_c,compasate_cs)==1) return 1;//分段刀具补偿
			if(tape(hWnd,compasateData,compasateNum,tapeData,&tapeNum,&tape_Start_point_X,&tape_Start_point_Y,&tape_build_c,&first5152flag,tape_cs,&tape_Start_point_B,&tape_Start_point_C)==1) return 1; //锥面补偿
			if(DSP_Compute(hWnd,tapeData,tapeNum,ComputeData,&ComputeNum)==1) return 1; 
			
			CopyMemory(pDataEditNcGraphMem+all_creat_num,ComputeData,(ComputeNum*sizeof(nc_data)));	
			memset(compasateData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));//void *memset( void *dest, int c, size_t count );
			memset(tapeData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));//The memset function sets the first count bytes of dest to the character c
			memset(ComputeData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));
			
			all_creat_num=all_creat_num + tapeNum;
			ComputeNum = 0;
			compasateNum=0;
			tapeNum=0;
		}//11
	}while(end_decode != 1);
	_close(fdEdit);	
    find_draw_param(GetDlgItem(hWnd, IDC_GRAPH_2D),pDataEditNcGraphMem,&auto_draw_width,&auto_draw_length,&auto_mw, &auto_ml,all_creat_num);//求取画图参数

	EditGraphParam.hWnd = GetDlgItem(hWnd, IDC_GRAPH_2D);
	EditGraphParam.pDataNcGraphMem = pDataEditNcGraphMem;
	EditGraphParam.draw_width = auto_draw_width;
	EditGraphParam.draw_length = auto_draw_length;
	EditGraphParam.mw = auto_mw;
	EditGraphParam.ml = auto_ml;
	EditGraphParam.create_num = all_creat_num;
	edit_regraph_flag = 1; //创建重画功能
	//二维画图
	if(SHOW_2D_3D==2)
	{
		InvalidateRect(GetDlgItem(hWnd, IDC_GRAPH_2D),NULL,TRUE);
		SendMessage(GetDlgItem(hWnd, IDC_GRAPH_2D),WM_PAINT,NULL,NULL);
	}
	//三维画图
	else Draw3D();
	
	EnableWindow(GetDlgItem(hWnd,IDC_BTNDRAW),FALSE); //屏蔽绘画按钮

    free(tape_cs);
	free(compasate_cs);

	return 0;
}

LRESULT goEditDrawResetCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	int i;
	char szBuffer[10];
	edit_regraph_flag = 0;//取消重画功能	
	if(SHOW_2D_3D==2)
	{
		InvalidateRect(GetDlgItem(hWnd,IDC_GRAPH_2D),NULL,TRUE);		
		 //解放绘图内存
		if(HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pDataEditNcGraphMem) == 0)
		{
			if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_HIDE);
			MessageBox(hWnd,"can not free heapmemory in function goEditDrawResetCmd",NULL,NULL);
			if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);
			return 1;
		}
	}
	else 
	{
		//如果为三维画图就需要重新设定各EDIT控件内容
		edit_zoom=1.0;
		edit_move_step=10.0;
		edit_turn_step=10.0;
		i=int(edit_move_step);
		itoa(i,szBuffer,10);
		SetWindowText (GetDlgItem(hWnd,IDC_EDIT_E_MOVE),szBuffer);
		i=int(edit_turn_step);
		itoa(i,szBuffer,10);
		SetWindowText (GetDlgItem(hWnd,IDC_EDIT_E_TURN),szBuffer);
		i=int(edit_zoom*100);
		itoa(i,szBuffer,10);
		SetWindowText (GetDlgItem(hWnd,IDC_EDIT_EZOOM),szBuffer);
		Draw3DReset();
	}
	EnableWindow(GetDlgItem(hWnd,IDC_BTNDRAW),TRUE); //再起绘画按钮
	return 0;
}
//打开文件函数（打开和写入编辑框）
BOOL OpenFile(char *buffer, char *filename, HWND hParent)
{
    int  fd;
    int reallength=0;	
	if ( _access (filename, 0) < 0)
	{
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_HIDE);
		MessageBox (hParent, "no this file","Program", MB_OK | MB_ICONSTOP);
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);
	}
	else if ( _access (filename, 4) < 0)
	{
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_HIDE);
		MessageBox (hParent, "cann't read file","Program", MB_OK | MB_ICONSTOP);
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);
	}
	else 
	{
		if ( _access (filename, 2) < 0)
		{
			if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);
			MessageBox (hParent, "cann't write file","Program", MB_OK | MB_ICONEXCLAMATION);
			if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);
		}
		fd = _open(filename, _O_RDONLY);
		if (fd <= 0)
		{
			if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_HIDE);
			MessageBox (hParent, "open file fault","Program", MB_OK | MB_ICONSTOP);
			if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);
			return FALSE;
		}
		if ((reallength=_read(fd, buffer, MAX_FILELEN)) >= MAX_FILELEN) 
		{
			if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_HIDE);
			MessageBox (hParent, "file cutted","Program", MB_OK | MB_ICONEXCLAMATION);
			if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);
		}
		_close (fd);
		buffer[reallength]=0; //最后位置 ' \0 '
	    
		return TRUE;
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);
	}
	
    return FALSE;
}
//保存文件函数
BOOL NBSave (HWND hWnd)
{
	char chPath[MAX_PATH+1];
	char chName[_MAX_FNAME+1];
	char chFullName[MAX_PATH+_MAX_FNAME+1];
	int choise=0;
	FILE *file;
	char *buffer;
	long reallength=0;

	buffer = (char *)HeapAlloc(GetProcessHeap(),
									HEAP_ZERO_MEMORY,
									MAX_FILELEN*sizeof(char)
									);
	if(buffer == NULL)
	{
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_HIDE);
		MessageBox(hWnd,"can not alloc heapmemory in function NBSave",NULL,NULL);
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);
		return FALSE;
	}
	
	memset (chFullName, 0, MAX_PATH+_MAX_FNAME + 1);

	GetWindowText (GetDlgItem(hWnd, IDC_EPATH), chPath, MAX_PATH);

	GetWindowText (GetDlgItem(hWnd, IDC_EFNAME), chName, _MAX_FNAME);

	strcpy(chFullName,chPath);
	strcat(chFullName,"\\");
	strcat(chFullName,chName);

	if ((file = fopen (chFullName, "w+")) == NULL)
	{	
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_HIDE);
		MessageBox (GetDlgItem(hWnd, IDC_BTNSAVE),"open file err","Program Input", MB_OK | MB_ICONSTOP);
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);
		
		pNoteInfo->ischanged = FALSE;
		GetFileAndDirList (hWnd, chPath);
		ShowWindow(GetDlgItem(hWnd,IDC_LISTVIEW),SW_SHOW);	
		ShowWindow(GetDlgItem(hWnd,IDC_MLEDIT),SW_HIDE);
	 	SetWindowText(GetDlgItem(hWnd, IDC_EFNAME),"");
		return FALSE;
	}
	else 
	{
		reallength = GetWindowTextLength(GetDlgItem(hWnd,IDC_MLEDIT));
		GetWindowText(GetDlgItem(hWnd,IDC_MLEDIT),buffer,MAX_FILELEN);
		if (fwrite(buffer, 1, reallength, file) < 0)  
		{
			if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_HIDE);
			MessageBox (GetDlgItem(hWnd, IDC_BTNSAVE),"write file err","Program Input", MB_OK | MB_ICONEXCLAMATION);
			if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);
			pNoteInfo->ischanged = FALSE;
			fclose (file);
		}
		pNoteInfo->ischanged = FALSE;
		fclose (file);
	}

	if(HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,buffer) == 0)
	{
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_HIDE);
		MessageBox(hWnd,"can not free heapmemory in function NBSave",NULL,NULL);
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);
		return FALSE;
	}
    return TRUE;
}
//初始化组合框（目录名）函数
static void InitPathCombo(HWND hWnd,char *path)
{
    //HWND hCtrlWnd;
    char chSubPath[MAX_PATH];
    char chPath[MAX_PATH];
    char *pStr;

    if (path == NULL) return;

    strcpy (chPath, path);
    //strtrimall (chPath);
    if ( strlen (chPath) == 0 ) return;

    if (strcmp (chPath, "\\") != 0 &&  chPath[strlen(chPath)-1] == '\\') { //将目录的最后 '/'写为0
        chPath [strlen (chPath) - 1] = 0;
    }    
    SendMessage (hWnd, CB_RESETCONTENT, 0, 0);//清除commbox
    //SendMessage (hWnd, CB_SETITEMHEIGHT, 0, (LPARAM)(HIWORD(GetDialogBaseUnits())+2));//设定commbox高度
   // SendMessage (hWnd, CB_SETITEMHEIGHT, 0, (LPARAM)40);//设定commbox高度
    strcpy(chSubPath, "\\");
    SendMessage (hWnd, CB_ADDSTRING, 0,(LPARAM)chSubPath);
    
    pStr = strchr(chPath + 1, '\\');
    while (pStr != NULL){
        memset (chSubPath, 0, sizeof (chSubPath));
        strncpy (chSubPath, chPath, pStr -chPath);
        SendMessage (hWnd, CB_INSERTSTRING, 0,(LPARAM)chSubPath);
        pStr = strchr (chPath + (pStr -chPath +1), '\\');
    }
    
    if (strcmp (chPath, "\\") != 0 ){
        SendMessage (hWnd, CB_INSERTSTRING, 0,(LPARAM)chPath);
    }
	
    SetWindowText (hWnd, chPath);
	
}

//*****************************************************************************************************************************
char * caption [] =
{
    "Name","Size", "DateModified"
};

//构建listview函数
void listviewproc(HWND hWnd)
{
	int i;
    LVCOLUMN lvcol;
	TCHAR szBuffer[MAX_PATH+1];
	
    
	ZeroMemory(&lvcol, sizeof(LV_COLUMN));

	GetCurrentDirectory(MAX_PATH+1,szBuffer);   //取得当前目录
    
	InitPathCombo (GetDlgItem(hWnd,IDC_EPATH), szBuffer); //初始化目录框
	for (i = 0; i < 3; i++)
	{
        lvcol.iSubItem = 2-i;
        lvcol.pszText = caption[2-i];
        lvcol.cx = 160;
		lvcol.fmt = LVCFMT_CENTER;
        lvcol.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM;
		lvcol.cchTextMax = 60;
        SendMessage (GetDlgItem(hWnd,IDC_LISTVIEW), LVM_INSERTCOLUMN, 0, (LPARAM)&lvcol);
	}
	ListView_SetExtendedListViewStyle(GetDlgItem(hWnd,IDC_LISTVIEW),LVS_EX_GRIDLINES |LVS_EX_HEADERDRAGDROP |LVS_EX_FULLROWSELECT ); 
	GetFileAndDirList(hWnd,szBuffer);	
}
//寻找目录下的文件和目录
 void GetFileAndDirList( HWND hWnd,char* path)
{	
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	char DirSpec[MAX_PATH+1];
	FILEINFO fileinfo;

	ShowWindow(GetDlgItem(hWnd,IDC_MLEDIT),SW_HIDE);
	SendMessage (GetDlgItem(hWnd,IDC_LISTVIEW), LVM_DELETEALLITEMS, 0, 0);

	InvalidateRect(GetDlgItem(hWnd,IDC_LISTVIEW),NULL,TRUE);

	strncpy(DirSpec,path,strlen(path)+1);
	strncat(DirSpec,"\\*",3);
    hFind = FindFirstFile(DirSpec,&FindFileData);
	if(hFind==INVALID_HANDLE_VALUE)
	{
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_HIDE);
		MessageBox(hWnd,"Invalide file handle.Erro1 in GetFileAndDirList function",NULL,NULL);
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);
		return;
	}
	else 
	{
		//如果是文件
		if(FindFileData.dwFileAttributes!=FILE_ATTRIBUTE_DIRECTORY)
		{
			memset (&fileinfo, 0, sizeof (fileinfo));
			fileinfo.IsDir = FALSE;
            fileinfo.filesize = FindFileData.nFileSizeLow;
			strcpy (fileinfo.filename, FindFileData.cFileName);
			FileTimeToSystemTime(&FindFileData.ftLastWriteTime,&fileinfo.modifytime);
			instolistview (hWnd,&fileinfo);
		}
		//如果是目录
		else if(FindFileData.dwFileAttributes==FILE_ATTRIBUTE_DIRECTORY 
			&& strcmp(FindFileData.cFileName,".")!=0
			&& strcmp(FindFileData.cFileName,"..")!=0)
		{
			memset (&fileinfo, 0, sizeof (fileinfo));
			fileinfo.IsDir = TRUE;
            fileinfo.filesize = FindFileData.nFileSizeLow;
			strcpy (fileinfo.filename, FindFileData.cFileName);
			FileTimeToSystemTime(&FindFileData.ftLastWriteTime,&fileinfo.modifytime);
			instolistview (hWnd,&fileinfo);			
		}
		else
		{
			NULL;			
		}		
	}
	
	while(FindNextFile(hFind,&FindFileData)!=0)
	{
		//如果是文件
		if(FindFileData.dwFileAttributes!=FILE_ATTRIBUTE_DIRECTORY)
		{
			memset (&fileinfo, 0, sizeof (fileinfo));
			fileinfo.IsDir = FALSE;
            fileinfo.filesize = FindFileData.nFileSizeLow;
			strcpy (fileinfo.filename, FindFileData.cFileName);
			FileTimeToSystemTime(&FindFileData.ftLastWriteTime,&fileinfo.modifytime);
			instolistview (hWnd,&fileinfo);
			
		}
		//如果是目录
		else if(FindFileData.dwFileAttributes==FILE_ATTRIBUTE_DIRECTORY 
			&& strcmp(FindFileData.cFileName,".")!=0
			&& strcmp(FindFileData.cFileName,"..")!=0)
		{
			memset (&fileinfo, 0, sizeof (fileinfo));
			fileinfo.IsDir = TRUE;
            fileinfo.filesize = FindFileData.nFileSizeLow;
			strcpy (fileinfo.filename, FindFileData.cFileName);
			FileTimeToSystemTime(&FindFileData.ftLastWriteTime,&fileinfo.modifytime);
			instolistview (hWnd,&fileinfo);			
		}
		else
		{
			NULL;
		}		
	}
	FindClose(hFind);
}
//将目录文件放到listview函数
 void instolistview (HWND hWnd,PFILEINFO pfi)
{
    
    int       nItemCount;
    char      chTemp1[100];
	SYSTEMTIME stLocal;
	char lpszString[100];
	LVITEM item;
	
	ZeroMemory(&item, sizeof(LV_ITEM));
	nItemCount = ListView_GetItemCount(GetDlgItem(hWnd,IDC_LISTVIEW));
	
	item.iItem = nItemCount;
	item.mask = LVIF_TEXT;
	item.cchTextMax = MAX_PATH;
	SendMessage(GetDlgItem(hWnd,IDC_LISTVIEW),LVM_INSERTITEM,0,(LPARAM)&item);

	item.iSubItem = 0;
	item.pszText = (char *)malloc(_MAX_FNAME+1);
	if(item.pszText == NULL)
	{
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_HIDE);
		MessageBox(hWnd,"can not malloc item.pszText in listbox",NULL,NULL);
		if(SHOW_2D_3D==3)ShowWindow(hWndGraph,SW_SHOW);
		return;
	}
	strcpy(item.pszText, pfi->filename);
	SendMessage(GetDlgItem(hWnd,IDC_LISTVIEW),LVM_SETITEM,0,(LPARAM)&item);

	item.iSubItem = 1;
	wsprintf(chTemp1, " %d", pfi->filesize);
	strcpy(item.pszText,chTemp1);
	SendMessage(GetDlgItem(hWnd,IDC_LISTVIEW),LVM_SETITEM,0,(LPARAM)&item);

	item.iSubItem = 2;
	SystemTimeToTzSpecificLocalTime(NULL, &pfi->modifytime, &stLocal);
	memset (lpszString, 0, sizeof (lpszString));
	wsprintf(lpszString, "%02d/%02d/%d  %02d:%02d",
	stLocal.wDay, stLocal.wMonth, stLocal.wYear,
	stLocal.wHour, stLocal.wMinute);
	strcpy(item.pszText,lpszString);
	SendMessage(GetDlgItem(hWnd,IDC_LISTVIEW),LVM_SETITEM,0,(LPARAM)&item);

	InvalidateRect(GetDlgItem(hWnd,IDC_LISTVIEW),NULL,TRUE);

	if(item.pszText!=NULL) free(item.pszText);
}
 void Show2D(HWND hWnd)
{
	//隐藏三维显示时用到的控件
	ShowWindow(GetDlgItem(hWnd,IDC_GRAPH),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_BUP),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_BLEFT),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_BDOWN),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_BRIGHT),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_BZOOM_IN),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_BZOOM_OUT),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_B_TURN_UP),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_B_TURN_LEFT),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_B_TURN_DOWN),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_B_TURN_RIGHT),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_EZOOM),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_E_MOVE),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_E_TURN),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_E_NUMBER),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_E_MIN),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_E_MAX),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_EDIT),SW_HIDE);
	//显示二维绘图用到的控件
	ShowWindow(GetDlgItem(hWnd,IDC_GRAPH_2D),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_BTNYZ),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_BTNXY),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_BTNXZ),SW_SHOW);
	SetWindowText (GetDlgItem(hWnd,IDC_EDIT_3D_2D),"2D");
	SHOW_2D_3D=2;
}
void Show3D(HWND hWnd)
{
	ShowWindow(GetDlgItem(hWnd,IDC_GRAPH),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_BUP),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_BLEFT),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_BDOWN),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_BRIGHT),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_BZOOM_IN),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_BZOOM_OUT),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_B_TURN_UP),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_B_TURN_LEFT),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_B_TURN_DOWN),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_B_TURN_RIGHT),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_EZOOM),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_E_MOVE),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_E_TURN),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_E_NUMBER),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_E_MIN),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_E_MAX),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_EDIT_EDIT),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_GRAPH_2D),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_BTNYZ),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_BTNXY),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_BTNXZ),SW_HIDE);
	SetWindowText (GetDlgItem(hWnd,IDC_EDIT_3D_2D),"3D");
	SHOW_2D_3D=3;
}
