#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;								// 当前实例
extern int MaintionInput;
extern LPCmdThreadParam  pDataInMaintion;
void EIO_2_Edit_ThreadProc();
void EIO_2_LV_ThreadProc();
int isActivity_EIO_2;
EIO EIO_2;
extern void EIO_Change_Activate(HWND);
const MessageProc maintion_EIO_2Messages[]={
	    WM_CREATE, goCreateMaintion_EIO_2,
		WM_COMMAND, goCommandMaintion_EIO_2,
		WM_PAINT,  goPaintMaintion_EIO_2,
		WM_NOTIFY, goNotifyMaintion_EIO_2,
		WM_LBUTTONDOWN,goLButtonDownMaintion_EIO_2,
		WM_SETFOCUS,goSetFocusMaintion_EIO_2,
		WM_KILLFOCUS,goKillFocusMaintion_EIO_2,
		WM_DESTROY, goDestroyMaintion_EIO_2
};
CtlWndStruct maintion_EIO_2MenuBtns[]={
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDW_MAINTION_EIO_2_LV,TEXT(""),0,40,462,520,WS_VISIBLE|WS_BORDER|WS_CHILD|LVS_REPORT|LVS_ICON|LVS_EDITLABELS},
	{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDW_MAINTION_EIO_2_NUM,TEXT(""),50,10,50,20,WS_VISIBLE|WS_BORDER|WS_CHILD|ES_UPPERCASE|ES_NUMBER|ES_NOHIDESEL}
};
int MyRegisterClassMaintion_EIO_2(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			=CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)maintion_EIO_2WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);//
	wcex.lpszMenuName	= NULL;//(LPCTSTR)IDC_EX1;
	wcex.lpszClassName	= TEXT("MAINTION_EIO_2");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
//注册自动窗口类
	if(!RegisterClassEx(&wcex)) return 0;
	return 1;
}

//自动窗口过程体
LRESULT CALLBACK maintion_EIO_2WndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;	

	//查找对应的消息ID,并执行对应的消息处理过程
	for(i=0; i<dim(maintion_EIO_2Messages);i++)
	{
		if(msgCode == maintion_EIO_2Messages[i].uCode) return(*maintion_EIO_2Messages[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//对不存在的消息，调用缺省窗口过程
	return DefWindowProc(hWnd, msgCode, wParam, lParam);
}

LRESULT goCreateMaintion_EIO_2(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{		
    int i;
	HIMAGELIST hSmall;
	HICON hIcon;
	LVCOLUMN lvcol;
	LVITEM item;
	char contentAddress[5];
	int   nItemCount;
	char* content[]={"0=","1=","2=","3=","4=","5=","6=","7=","8=","9=","10=","11=","12=","13=","14=","15="};
	char * caption [] ={"BIT ADDRESS","DATA","CONTENT"};
	char * temp="0";
	//创建自动加工参数窗口菜单
	for(i=0;i<1;i++)
	{
		CreateWindowEx(WS_EX_CLIENTEDGE,
			WC_LISTVIEW,
			maintion_EIO_2MenuBtns[i].szTitle,
			maintion_EIO_2MenuBtns[i].lStyle,
			maintion_EIO_2MenuBtns[i].x,
			maintion_EIO_2MenuBtns[i].y,
			maintion_EIO_2MenuBtns[i].cx,
			maintion_EIO_2MenuBtns[i].cy,
			hWnd,
			(HMENU)maintion_EIO_2MenuBtns[i].nID,
			hInst,
			NULL
			);
	}
//创建自动窗口菜单
	for(i=1;i<dim(maintion_EIO_2MenuBtns);i++){
		CreateWindowEx(maintion_EIO_2MenuBtns[i].dwExStyle,
			maintion_EIO_2MenuBtns[i].szClass,
			maintion_EIO_2MenuBtns[i].szTitle,
			maintion_EIO_2MenuBtns[i].lStyle,
			maintion_EIO_2MenuBtns[i].x,
			maintion_EIO_2MenuBtns[i].y,
			maintion_EIO_2MenuBtns[i].cx,
			maintion_EIO_2MenuBtns[i].cy,
			hWnd,
			(HMENU)maintion_EIO_2MenuBtns[i].nID,
			hInst,
			NULL
			);
	}
	ZeroMemory(&lvcol, sizeof(LV_COLUMN));
	ListView_SetTextBkColor(GetDlgItem(hWnd,maintion_EIO_2MenuBtns[0].nID),RGB(255,255,255));
	lvcol.iOrder=2;
	lvcol.iSubItem = 2;
	lvcol.pszText = caption[2];
	lvcol.cx = 300;
	lvcol.fmt = LVCFMT_CENTER;
	lvcol.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM|LVCF_ORDER;
	lvcol.cchTextMax = 60;
	SendMessage (GetDlgItem(hWnd,maintion_EIO_2MenuBtns[0].nID), LVM_INSERTCOLUMN, 0, (LPARAM)&lvcol);
	lvcol.iOrder=0;
	lvcol.iSubItem = 1;
	lvcol.pszText = caption[0];
	lvcol.cx = 100;
	lvcol.fmt = LVCFMT_CENTER;
	lvcol.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM|LVCF_ORDER;
	lvcol.cchTextMax = 60;
	SendMessage (GetDlgItem(hWnd,maintion_EIO_2MenuBtns[0].nID), LVM_INSERTCOLUMN, 0, (LPARAM)&lvcol);
	lvcol.iOrder=1;
	lvcol.iSubItem = 0;
	lvcol.pszText = caption[1];
	lvcol.cx = 50;
	lvcol.fmt = LVCFMT_CENTER;
	lvcol.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM|LVCF_ORDER;
	lvcol.cchTextMax = 60;
	SendMessage (GetDlgItem(hWnd,maintion_EIO_2MenuBtns[0].nID), LVM_INSERTCOLUMN, 0, (LPARAM)&lvcol);
	ListView_SetExtendedListViewStyle(GetDlgItem(hWnd,maintion_EIO_2MenuBtns[0].nID),LVS_EX_GRIDLINES |LVS_EX_HEADERDRAGDROP |LVS_EX_FULLROWSELECT );
	SendMessage (GetDlgItem(hWnd,maintion_EIO_2MenuBtns[0].nID), LVM_DELETEALLITEMS, 0, 0);
	for(i=0;i<16;i++)
	{
		ZeroMemory(&item, sizeof(LV_ITEM));
		nItemCount = ListView_GetItemCount(GetDlgItem(hWnd,maintion_EIO_2MenuBtns[0].nID));
		item.iItem = nItemCount;
		item.mask = LVIF_TEXT;
		item.cchTextMax =5;
		item.pszText = (char *)malloc(5);
		if(item.pszText == NULL)
		{
			MessageBox(hWnd,"can not malloc item.pszText goCreateAutoParam functiotn",NULL,NULL);
		}
		SendMessage(GetDlgItem(hWnd,maintion_EIO_2MenuBtns[0].nID),LVM_INSERTITEM,0,(LPARAM)&item);
		item.iSubItem = 1;
		strcpy(item.pszText,content[i]);
		SendMessage(GetDlgItem(hWnd,maintion_EIO_2MenuBtns[0].nID),LVM_SETITEM,0,(LPARAM)&item);
		item.mask = LVIF_TEXT;
		item.cchTextMax =2;
		item.pszText = (char *)malloc(2);
		if(item.pszText == NULL)
		{
			MessageBox(hWnd,"can not malloc item.pszText goCreateAutoParam functiotn",NULL,NULL);
		}
		item.iSubItem = 0;
		strcpy(item.pszText,temp);
		SendMessage(GetDlgItem(hWnd,maintion_EIO_2MenuBtns[0].nID),LVM_SETITEM,0,(LPARAM)&item);
		item.mask = LVIF_TEXT;
		item.cchTextMax =30;
		item.pszText = (char *)malloc(30);
		if(item.pszText == NULL)
		{
			MessageBox(hWnd,"can not malloc item.pszText goCreateAutoParam functiotn",NULL,NULL);
		}
		item.iSubItem = 2;
		strcpy(item.pszText,temp);
		SendMessage(GetDlgItem(hWnd,maintion_EIO_2MenuBtns[0].nID),LVM_SETITEM,0,(LPARAM)&item);
	}
	// Create the ImageList
	hSmall = ImageList_Create(1,28,ILC_MASK, 1, 1);
// Add two icons to ImageList
	hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	ImageList_AddIcon(hSmall, hIcon);
	DestroyIcon(hIcon);
// Assign ImageList to List View
	ListView_SetImageList(GetDlgItem(hWnd,IDW_MAINTION_EIO_2_LV), hSmall, LVSIL_STATE);
	isActivity_EIO_2=0;
	EIO_2.EIO=82;
	for(i=0;i<16;i++)EIO_2.Bit_Address[i]=0;
	setListViewEIO(EIO_2,GetDlgItem(hWnd,IDW_MAINTION_EIO_2_LV));
	setListView(EIO_2,GetDlgItem(hWnd,IDW_MAINTION_EIO_2_LV));
	itoa(EIO_2.EIO,contentAddress,10);
	SetWindowText(GetDlgItem(hWnd,IDW_MAINTION_EIO_2_NUM),contentAddress);
	return 0;
}
LRESULT goCommandMaintion_EIO_2(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//定义菜单ID号和通知单
	int menuID, notifyCode;
	//定义窗口句柄
	HWND wndCtrl;
	menuID    = LOWORD(wParam); 
	notifyCode = HIWORD(wParam); 
	wndCtrl = (HWND) lParam;
	if(menuID==IDW_MAINTION_EIO_2_NUM)
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
			MaintionInput=3;
		}
	}
	return 0;
}
void EIO_2_Edit_ThreadProc()
{
    TCHAR szBuffer[_MAX_FNAME];
    GetWindowText(GetDlgItem(GetParent(pDataInMaintion->hWnd),IDC_MAINTION_MAIN_EDIT),szBuffer, _MAX_FNAME);
	SetWindowText (GetDlgItem(GetParent(pDataInMaintion->hWnd),IDC_MAINTION_MAIN_EDIT),"");
	SetWindowText (pDataInMaintion->wndCtrl,szBuffer);
	SetFocus(pDataInMaintion->wndCtrl);
	SendMessage(pDataInMaintion->wndCtrl,EM_SETSEL,-1,0);
	MaintionInput=0;
}
LRESULT goLButtonDownMaintion_EIO_2(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	EIO_Change_Activate(hWnd);
	return 0;
}

LRESULT goNotifyMaintion_EIO_2(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
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
	if(menuID==IDW_MAINTION_EIO_2_LV)
	{		
		if(pnmh->code==NM_CLICK)
		{			
			nSelItem =-1;
			nSelItem =  ListView_GetNextItem(wndCtrl,-1,LVNI_SELECTED);			
			ListView_SetItemState(GetDlgItem(hWnd,IDW_MAINTION_EIO_2_LV),-1,LVIS_SELECTED, 0XFF);				
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
				MaintionInput=4;					
			}			
		}
	}
	return 0;
}
void EIO_2_LV_ThreadProc()
{
    TCHAR szBuffer[2];
	int nSelItem;	
	nSelItem = -1;
	nSelItem =  ListView_GetNextItem(GetDlgItem(pDataInMaintion->hWnd,IDW_MAINTION_EIO_2_LV),-1,LVNI_DROPHILITED);
	if (nSelItem >= 0 ) 
	{
		GetWindowText (GetDlgItem(GetParent(pDataInMaintion->hWnd),IDC_MAINTION_MAIN_EDIT), szBuffer, 30);
		ListView_SetItemText(GetDlgItem(pDataInMaintion->hWnd,IDW_MAINTION_EIO_2_LV),nSelItem,0,szBuffer);
		SetWindowText (GetDlgItem(GetParent(pDataInMaintion->hWnd),IDC_MAINTION_MAIN_EDIT),"");
	}	
	//将选择项蓝背景颜色去掉
	ListView_SetItemState(GetDlgItem(pDataInMaintion->hWnd,IDW_MAINTION_EIO_2_LV),-1,LVIS_SELECTED, 0XFF);
	MaintionInput=0;
}
LRESULT goSetFocusMaintion_EIO_2(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	EIOclearEdit();
	EIOclearLV();
	isActivity_EIO_2=1;
	InvalidateRect(hWnd,NULL,TRUE);
	UpdateWindow(hWnd);
	return 0;
}
LRESULT goKillFocusMaintion_EIO_2(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	isActivity_EIO_2=0;
	InvalidateRect(hWnd,NULL,TRUE);
	UpdateWindow(hWnd);
	return 0;
}
LRESULT goPaintMaintion_EIO_2(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect;
	hdc = BeginPaint(hWnd, &ps);
	SetBkMode(hdc,TRANSPARENT);
	SetRect(&rect,0,5,50,35);
	if(isActivity_EIO_2==1)	FillRect(hdc, &rect,GetSysColorBrush(COLOR_HOTLIGHT));
	else FillRect(hdc, &rect,GetSysColorBrush(COLOR_INACTIVECAPTION));
	DrawText(hdc,"EIO",-1,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,50,5,100,35);
	if(isActivity_EIO_2==1)	FillRect(hdc, &rect,GetSysColorBrush(COLOR_HOTLIGHT));
	else FillRect(hdc, &rect,GetSysColorBrush(COLOR_INACTIVECAPTION));
	SetRect(&rect,100,5,200,35);
	if(isActivity_EIO_2==1)	FillRect(hdc, &rect,GetSysColorBrush(COLOR_HOTLIGHT));
	else FillRect(hdc, &rect,GetSysColorBrush(COLOR_INACTIVECAPTION));
	DrawText(hdc,"ADDRESS",-1,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,200,5,250,35);
	if(isActivity_EIO_2==1)	FillRect(hdc, &rect,GetSysColorBrush(COLOR_HOTLIGHT));
	else FillRect(hdc, &rect,GetSysColorBrush(COLOR_INACTIVECAPTION));
	DrawText(hdc,EIO_2.Address,-1,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,250,5,320,35);
	if(isActivity_EIO_2==1)	FillRect(hdc, &rect,GetSysColorBrush(COLOR_HOTLIGHT));
	else FillRect(hdc, &rect,GetSysColorBrush(COLOR_INACTIVECAPTION));
	DrawText(hdc,"(IN)",-1,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	EndPaint(hWnd, &ps);
	return 0;
}
LRESULT goDestroyMaintion_EIO_2(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		PostQuitMessage(0);
		return 0;
}