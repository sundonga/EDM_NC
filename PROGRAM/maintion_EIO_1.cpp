#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;								// 当前实例
extern int MaintionInput;
extern LPCmdThreadParam  pDataInMaintion;
void EIO_1_Edit_ThreadProc();//edit输入对应的线程
void EIO_1_LV_ThreadProc();//ListView输入对应的线程
int isActivity_EIO_1;//标记当前窗口是否为活动窗口
EIO EIO_1;//存储当前IO口信息
extern void EIO_Change_Activate(HWND);
const MessageProc maintion_EIO_1Messages[]={
	    WM_CREATE, goCreateMaintion_EIO_1,
		WM_COMMAND, goCommandMaintion_EIO_1,
		WM_PAINT,  goPaintMaintion_EIO_1,
		WM_NOTIFY, goNotifyMaintion_EIO_1,
		WM_LBUTTONDOWN,goLButtonDownMaintion_EIO_1,
		WM_SETFOCUS,goSetFocusMaintion_EIO_1,
		WM_KILLFOCUS,goKillFocusMaintion_EIO_1,
		WM_DESTROY, goDestroyMaintion_EIO_1
};
CtlWndStruct maintion_EIO_1MenuBtns[]={
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDW_MAINTION_EIO_1_LV,TEXT(""),0,40,462,520,WS_VISIBLE|WS_BORDER|WS_CHILD|LVS_REPORT|LVS_ICON|LVS_EDITLABELS},
	{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDW_MAINTION_EIO_1_NUM,TEXT(""),50,10,50,20,WS_VISIBLE|WS_BORDER|WS_CHILD|ES_UPPERCASE|ES_NUMBER|ES_NOHIDESEL}
};
int MyRegisterClassMaintion_EIO_1(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			=CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)maintion_EIO_1WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);//
	wcex.lpszMenuName	= NULL;//(LPCTSTR)IDC_EX1;
	wcex.lpszClassName	= TEXT("MAINTION_EIO_1");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
//注册自动窗口类
	if(!RegisterClassEx(&wcex)) return 0;
	return 1;
}

//自动窗口过程体
LRESULT CALLBACK maintion_EIO_1WndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;	

	//查找对应的消息ID,并执行对应的消息处理过程
	for(i=0; i<dim(maintion_EIO_1Messages);i++)
	{
		if(msgCode == maintion_EIO_1Messages[i].uCode) return(*maintion_EIO_1Messages[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//对不存在的消息，调用缺省窗口过程
	return DefWindowProc(hWnd, msgCode, wParam, lParam);
}

LRESULT goCreateMaintion_EIO_1(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{		
    int i;
	HIMAGELIST hSmall;
	HICON hIcon;
	LVCOLUMN lvcol;
	LVITEM item;
	int   nItemCount;
	char* content[]={"0=","1=","2=","3=","4=","5=","6=","7=","8=","9=","10=","11=","12=","13=","14=","15="};
	char * caption [] ={"BIT ADDRESS","DATA","CONTENT"};
	char contentAddress[5];
	//创建自动加工参数窗口菜单
	for(i=0;i<1;i++)
	{
		CreateWindowEx(WS_EX_CLIENTEDGE,
			WC_LISTVIEW,
			maintion_EIO_1MenuBtns[i].szTitle,
			maintion_EIO_1MenuBtns[i].lStyle,
			maintion_EIO_1MenuBtns[i].x,
			maintion_EIO_1MenuBtns[i].y,
			maintion_EIO_1MenuBtns[i].cx,
			maintion_EIO_1MenuBtns[i].cy,
			hWnd,
			(HMENU)maintion_EIO_1MenuBtns[i].nID,
			hInst,
			NULL
			);
	}
//创建自动窗口菜单
	for(i=1;i<dim(maintion_EIO_1MenuBtns);i++){
		CreateWindowEx(maintion_EIO_1MenuBtns[i].dwExStyle,
			maintion_EIO_1MenuBtns[i].szClass,
			maintion_EIO_1MenuBtns[i].szTitle,
			maintion_EIO_1MenuBtns[i].lStyle,
			maintion_EIO_1MenuBtns[i].x,
			maintion_EIO_1MenuBtns[i].y,
			maintion_EIO_1MenuBtns[i].cx,
			maintion_EIO_1MenuBtns[i].cy,
			hWnd,
			(HMENU)maintion_EIO_1MenuBtns[i].nID,
			hInst,
			NULL
			);
	}
	ZeroMemory(&lvcol, sizeof(LV_COLUMN));
	ListView_SetTextBkColor(GetDlgItem(hWnd,maintion_EIO_1MenuBtns[0].nID),RGB(255,255,255));
	lvcol.iOrder=2;
	lvcol.iSubItem = 2;
	lvcol.pszText = caption[2];
	lvcol.cx = 300;
	lvcol.fmt = LVCFMT_CENTER;
	lvcol.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM|LVCF_ORDER;
	lvcol.cchTextMax = 60;
	SendMessage (GetDlgItem(hWnd,maintion_EIO_1MenuBtns[0].nID), LVM_INSERTCOLUMN, 0, (LPARAM)&lvcol);
	lvcol.iOrder=0;
	lvcol.iSubItem = 1;
	lvcol.pszText = caption[0];
	lvcol.cx = 100;
	lvcol.fmt = LVCFMT_CENTER;
	lvcol.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM|LVCF_ORDER;
	lvcol.cchTextMax = 60;
	SendMessage (GetDlgItem(hWnd,maintion_EIO_1MenuBtns[0].nID), LVM_INSERTCOLUMN, 0, (LPARAM)&lvcol);
	lvcol.iOrder=1;
	lvcol.iSubItem = 0;
	lvcol.pszText = caption[1];
	lvcol.cx = 50;
	lvcol.fmt = LVCFMT_CENTER;
	lvcol.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM|LVCF_ORDER;
	lvcol.cchTextMax = 60;
	SendMessage (GetDlgItem(hWnd,maintion_EIO_1MenuBtns[0].nID), LVM_INSERTCOLUMN, 0, (LPARAM)&lvcol);
	ListView_SetExtendedListViewStyle(GetDlgItem(hWnd,maintion_EIO_1MenuBtns[0].nID),LVS_EX_GRIDLINES |LVS_EX_HEADERDRAGDROP |LVS_EX_FULLROWSELECT );
	SendMessage (GetDlgItem(hWnd,maintion_EIO_1MenuBtns[0].nID), LVM_DELETEALLITEMS, 0, 0);
	for(i=0;i<16;i++)
	{
		ZeroMemory(&item, sizeof(LV_ITEM));
		nItemCount = ListView_GetItemCount(GetDlgItem(hWnd,maintion_EIO_1MenuBtns[0].nID));
		item.iItem = nItemCount;
		item.mask = LVIF_TEXT;
		item.cchTextMax =5;
		item.pszText = (char *)malloc(5);
		if(item.pszText == NULL)
		{
			MessageBox(hWnd,"can not malloc item.pszText goCreateAutoParam functiotn",NULL,NULL);
		}
		SendMessage(GetDlgItem(hWnd,maintion_EIO_1MenuBtns[0].nID),LVM_INSERTITEM,0,(LPARAM)&item);
		item.iSubItem = 1;
		strcpy(item.pszText,content[i]);
		SendMessage(GetDlgItem(hWnd,maintion_EIO_1MenuBtns[0].nID),LVM_SETITEM,0,(LPARAM)&item);
	}
	// Create the ImageList
	hSmall = ImageList_Create(1,28,ILC_MASK, 1, 1);
// Add two icons to ImageList
	hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	ImageList_AddIcon(hSmall, hIcon);
	DestroyIcon(hIcon);
// Assign ImageList to List View
	ListView_SetImageList(GetDlgItem(hWnd,IDW_MAINTION_EIO_1_LV), hSmall, LVSIL_STATE);
	isActivity_EIO_1=0;
	EIO_1.EIO=80;
	for(i=0;i<16;i++)EIO_1.Bit_Address[i]=i%2;
	setListViewEIO(EIO_1,GetDlgItem(hWnd,IDW_MAINTION_EIO_1_LV));
	setListView(EIO_1,GetDlgItem(hWnd,IDW_MAINTION_EIO_1_LV));
	itoa(EIO_1.EIO,contentAddress,10);
	SetWindowText(GetDlgItem(hWnd,IDW_MAINTION_EIO_1_NUM),contentAddress);
	return 0;
}
LRESULT goCommandMaintion_EIO_1(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//定义菜单ID号和通知单
	int menuID, notifyCode;
	//定义窗口句柄
	HWND wndCtrl;
	menuID    = LOWORD(wParam); 
	notifyCode = HIWORD(wParam); 
	wndCtrl = (HWND) lParam;
	if(menuID==IDW_MAINTION_EIO_1_NUM)
	{
		if(notifyCode==EN_SETFOCUS)
		{
			EIOclearEdit();
			EIOclearLV();
			SendMessage(GetDlgItem(hWnd,menuID),EM_SETSEL,0,-1);
			pDataInMaintion->hWnd = hWnd;
			pDataInMaintion->wndCtrl = GetDlgItem(hWnd,LOWORD(wParam));
			pDataInMaintion->menuID = LOWORD(wParam);
			pDataInMaintion->notifyCode = 0;
			SetFocus(GetDlgItem(GetParent(hWnd),IDC_MAINTION_MAIN_EDIT));
			MaintionInput=1;
		}
	}
	return 0;
}
void EIO_1_Edit_ThreadProc()
{
    TCHAR szBuffer[_MAX_FNAME];
    GetWindowText(GetDlgItem(GetParent(pDataInMaintion->hWnd),IDC_MAINTION_MAIN_EDIT),szBuffer, _MAX_FNAME);
	SetWindowText (GetDlgItem(GetParent(pDataInMaintion->hWnd),IDC_MAINTION_MAIN_EDIT),"");
	SetWindowText (pDataInMaintion->wndCtrl,szBuffer);
	SetFocus(pDataInMaintion->wndCtrl);
	SendMessage(pDataInMaintion->wndCtrl,EM_SETSEL,-1,0);
	MaintionInput=0;
}
LRESULT goLButtonDownMaintion_EIO_1(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	EIO_Change_Activate(hWnd);
	return 0;
}

LRESULT goNotifyMaintion_EIO_1(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//定义列表窗口ID号
	int menuID;
	//定义列表窗口句柄
	HWND wndCtrl;
	//定义消息信息结构体指针
	LPNMHDR pnmh;
    //定义指向包含消息结构和列表信息结构成员的结构体
	int nSelItem;	
	menuID    = int(wParam); 
	pnmh = (LPNMHDR)lParam;
	wndCtrl = pnmh->hwndFrom;	
	if(menuID==IDW_MAINTION_EIO_1_LV)
	{
		
		if(pnmh->code==NM_CLICK){			
			nSelItem =-1;
			nSelItem =  ListView_GetNextItem(wndCtrl,-1,LVNI_SELECTED);			
			ListView_SetItemState(GetDlgItem(hWnd,IDW_MAINTION_EIO_1_LV),-1,LVIS_SELECTED, 0XFF);
				
			if (nSelItem >= 0 )
			{
				EIOclearEdit();
				EIOclearLV();
				SetFocus(GetDlgItem(GetParent(hWnd),IDC_MAINTION_MAIN_EDIT));
				ListView_SetItemState(wndCtrl,nSelItem,LVIS_DROPHILITED, 0XFF);
				pDataInMaintion->hWnd = hWnd;
				pDataInMaintion->wndCtrl = wndCtrl;
				pDataInMaintion->menuID = menuID;
				pDataInMaintion->notifyCode = 0;
				MaintionInput=2;					
			}			
		}
	}
	return 0;
}
void EIO_1_LV_ThreadProc()
{
	
    TCHAR szBuffer[2];
	int nSelItem;
	nSelItem = -1;
	nSelItem =  ListView_GetNextItem(GetDlgItem(pDataInMaintion->hWnd,IDW_MAINTION_EIO_1_LV),-1,LVNI_DROPHILITED);
	if (nSelItem >= 0 ) 
	{
		GetWindowText (GetDlgItem(GetParent(pDataInMaintion->hWnd),IDC_MAINTION_MAIN_EDIT), szBuffer, 30);
		ListView_SetItemText(GetDlgItem(pDataInMaintion->hWnd,IDW_MAINTION_EIO_1_LV),nSelItem,0,szBuffer);
		SetWindowText (GetDlgItem(GetParent(pDataInMaintion->hWnd),IDC_MAINTION_MAIN_EDIT),"");
	}	
	//将选择项蓝背景颜色去掉
	ListView_SetItemState(GetDlgItem(pDataInMaintion->hWnd,IDW_MAINTION_EIO_1_LV),-1,LVIS_SELECTED, 0XFF);
	MaintionInput=0;
}
LRESULT goSetFocusMaintion_EIO_1(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	EIOclearEdit();
	EIOclearLV();
	isActivity_EIO_1=1;
	InvalidateRect(hWnd,NULL,TRUE);
	UpdateWindow(hWnd);
	return 0;
}
LRESULT goKillFocusMaintion_EIO_1(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	isActivity_EIO_1=0;
	InvalidateRect(hWnd,NULL,TRUE);
	UpdateWindow(hWnd);
	return 0;
}
LRESULT goPaintMaintion_EIO_1(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect;
	hdc = BeginPaint(hWnd, &ps);
	SetBkMode(hdc,TRANSPARENT);
	SetRect(&rect,0,5,50,35);
	if(isActivity_EIO_1==1)	FillRect(hdc, &rect,GetSysColorBrush(COLOR_HOTLIGHT));
	else FillRect(hdc, &rect,GetSysColorBrush(COLOR_INACTIVECAPTION));
	DrawText(hdc,"EIO",-1,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,50,5,100,35);
	if(isActivity_EIO_1==1)	FillRect(hdc, &rect,GetSysColorBrush(COLOR_HOTLIGHT));
	else FillRect(hdc, &rect,GetSysColorBrush(COLOR_INACTIVECAPTION));
	SetRect(&rect,100,5,200,35);
	if(isActivity_EIO_1==1)	FillRect(hdc, &rect,GetSysColorBrush(COLOR_HOTLIGHT));
	else FillRect(hdc, &rect,GetSysColorBrush(COLOR_INACTIVECAPTION));
	DrawText(hdc,"ADDRESS",-1,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,200,5,250,35);
	if(isActivity_EIO_1==1)	FillRect(hdc, &rect,GetSysColorBrush(COLOR_HOTLIGHT));
	else FillRect(hdc, &rect,GetSysColorBrush(COLOR_INACTIVECAPTION));
	DrawText(hdc,EIO_1.Address,-1,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,250,5,320,35);
	if(isActivity_EIO_1==1)	FillRect(hdc, &rect,GetSysColorBrush(COLOR_HOTLIGHT));
	else FillRect(hdc, &rect,GetSysColorBrush(COLOR_INACTIVECAPTION));
	DrawText(hdc,"(OUT)",-1,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	EndPaint(hWnd, &ps);
	return 0;
}
LRESULT goDestroyMaintion_EIO_1(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		PostQuitMessage(0);
		return 0;
}