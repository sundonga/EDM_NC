#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

//加工条件i/o, 假设的地址
#define ADR_ON 0x000
#define ADR_OFF 0x000
#define ADR_SV 0x000
#define ADR_RV 0x000
#define ADR_IP 0x000
#define ADR_IPM 0x000
#define ADR_VEL 0x000
#define ADR_TENSION 0x000
#define ADR_FREEVEL 0x000
#define ADR_VOBJ 0x000
#define ADR_PID 0x000

void ListAutoParamThreadProc();

extern HINSTANCE hInst;								// 当前实例
extern int AutoInput;
extern LPCmdThreadParam  pDataInAutoInput;
//start//2010-4-5
//以下三个变量在添加滚动条时需要使用，现在暂时不需要使用
int NOWSELECTLINE;//2010-4-5 标记当前选定的ListView
int NOWSELECTROW;//2010-4-5 标记当前选定的ListView行
int VPosAuto;//2010-4-5 标记当前滚动条位置
int NOWITEMNUMBER;//标记当前项目数量，在应用滚动条石需要使用
HWND hAUTOLV;//当前子子窗口的句柄
void fn_auto_setLV(HWND);//在添加滚动条时该函数用于在滚动条位置改变时进行修改对应的Listview显示内容
//end 2010-4-5

ConditionStru Machiningparam = {1000,{	 {"ON",10,0,IO_FPGA_WRITE,ADR_ON},
										 {"OFF",20,0,IO_FPGA_WRITE,ADR_OFF},
	                                     {"SV",30,0,IO_FPGA_WRITE,ADR_SV},
										 {"RV",40,0,IO_FPGA_WRITE,ADR_RV},
										 {"IP",50,0,IO_FPGA_WRITE,ADR_IP},
										 {"IPM",60,0,IO_FPGA_WRITE,ADR_IPM},
										 {"VEL",70,0,IO_FPGA_WRITE,ADR_VEL},
										 {"TENSION",80,0,IO_FPGA_WRITE,ADR_TENSION},
										 {"FREEVEL",90,0,IO_FPGA_WRITE,ADR_FREEVEL},
										 {"VOBJ",100,0,IO_FPGA_WRITE,ADR_VOBJ},
										 {"PID",110,0,IO_FPGA_WRITE,ADR_PID},
										 {"",0,0,IO_FPGA_WRITE,0X00},
										 {"",0,0,IO_FPGA_WRITE,0X00},
										 {"",0,0,IO_FPGA_WRITE,0X00},
										 {"",0,0,IO_FPGA_WRITE,0X00},
										 {"",0,0,IO_FPGA_WRITE,0X00},
										 {"",0,0,IO_FPGA_WRITE,0X00},
										 {"",0,0,IO_FPGA_WRITE,0X00},
										 {"",0,0,IO_FPGA_WRITE,0X00},
										 {"",0,0,IO_FPGA_WRITE,0X00},
										 {"",0,0,IO_FPGA_WRITE,0X00},
										 {"",0,0,IO_FPGA_WRITE,0X00},
										 {"",0,0,IO_FPGA_WRITE,0X00},
										 {"",0,0,IO_FPGA_WRITE,0X00},
										 {"",0,0,IO_FPGA_WRITE,0X00},
										 {"",0,0,IO_FPGA_WRITE,0X00},
										 {"",0,0,IO_FPGA_WRITE,0X00},
										 {"",0,0,IO_FPGA_WRITE,0X00},
										 {"",0,0,IO_FPGA_WRITE,0X00},
										 {"",0,0,IO_FPGA_WRITE,0X00}
									}
	};


const MessageProc autoParamMessages[]={
	    WM_CREATE, goCreateAutoParam,
		WM_COMMAND, goCommandAutoParam,
		WM_NOTIFY,  goListAutoParam,
		WM_PAINT,  goPaintAutoParam,
		WM_DRAWITEM, goDrawitemMain,
		WM_DESTROY, goDestroyAutoParam
};
CtlWndStruct autoParamMenuBtns[]={
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_AUTOPARAMLIST1,TEXT(""),0,0,90,300,WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_ICON},//2010-4-5
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_AUTOPARAMLIST2,TEXT(""),90,0,90,300,WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_ICON},//2010-4-5
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_AUTOPARAMLIST3,TEXT(""),180,0,90,300,WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_ICON},//2010-4-5
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_AUTOPARAMLIST4,TEXT(""),270,0,90,300,WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_ICON}//2010-4-5
};
int MyRegisterClassAutoParam(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	INITCOMMONCONTROLSEX icex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)autoParamWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//(LPCTSTR)IDC_EX1;
	wcex.lpszClassName	= TEXT("AUTOPARAMWND");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
//注册自动窗口类
	if(!RegisterClassEx(&wcex)) return 0;
	icex.dwICC=ICC_LISTVIEW_CLASSES;//??????
	icex.dwSize=sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&icex);
	return 1;
}
//自动加工参数窗口过程体
LRESULT CALLBACK autoParamWndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;

	//查找对应的消息ID,并执行对应的消息处理过程
	for(i=0; i<dim(autoParamMessages);i++)
	{
		if(msgCode == autoParamMessages[i].uCode) return(*autoParamMessages[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//对不存在的消息，调用缺省窗口过程
	return DefWindowProc(hWnd, msgCode, wParam, lParam);
}

//start 2010-4-5
LRESULT goCreateAutoParam(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{		
    int i,j;
	LVCOLUMN lvcol;
    char * caption [] ={"Name","value"};
	HIMAGELIST hSmall;//扩展ListView行高需要用到的参数
	HICON hIcon;//扩展ListView行高需要用到的参数
	SCROLLINFO si;//滚动条变量
	LPCREATESTRUCT lpcs;
	//设置滚动条
	lpcs=(LPCREATESTRUCT)lParam;
	si.cbSize=sizeof(si);
	si.nMin=0;
	si.nMax=lpcs->cy+0;//lpcs->cx=1024
	si.nPage=lpcs->cy;
	si.nPos=0;
	si.fMask=SIF_ALL;
	SetScrollInfo(hWnd,SB_VERT,&si,TRUE);
	//end 设置滚动条

	//初始化NOWSELECTLINE、NOWSELECTROW、VPosAuto、hAUTOLV
	NOWSELECTLINE=0;
	NOWSELECTROW=0;
	VPosAuto=0;	
	NOWITEMNUMBER=11;
	hAUTOLV=hWnd;
	//end 初始化
	//创建自动加工参数窗口菜单
	for(i=0;i<dim(autoParamMenuBtns);i++){
		CreateWindowEx(WS_EX_CLIENTEDGE,
			WC_LISTVIEW,
			autoParamMenuBtns[i].szTitle,
			autoParamMenuBtns[i].lStyle,
			autoParamMenuBtns[i].x,
			autoParamMenuBtns[i].y,
			autoParamMenuBtns[i].cx,
			autoParamMenuBtns[i].cy,
			hWnd,
			(HMENU)autoParamMenuBtns[i].nID,
			hInst,
			NULL
			);
	}
//创建自动加工参数的名目栏
//只创建ListView的标题
    ZeroMemory(&lvcol, sizeof(LV_COLUMN));	
	for(j=0;j<4;j++)
	{
		ListView_SetTextBkColor(GetDlgItem(hWnd,autoParamMenuBtns[j].nID),RGB(255,255,255));
		lvcol.iSubItem =0;
		lvcol.pszText = caption[j%2];
		lvcol.cx = 85;
		lvcol.fmt = LVCFMT_CENTER;
		lvcol.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM;
		lvcol.cchTextMax = 10;
		SendMessage (GetDlgItem(hWnd,autoParamMenuBtns[j].nID), LVM_INSERTCOLUMN, 0, (LPARAM)&lvcol);
		ListView_SetExtendedListViewStyle(GetDlgItem(hWnd,autoParamMenuBtns[j].nID),LVS_EX_FULLROWSELECT  );
	}
//扩展ListView用到的代码，其中行高由ImageList_Create(1,30,ILC_MASK, 1, 1);中第二个参数30决定
	hSmall = ImageList_Create(1,30,ILC_MASK, 1, 1);
// Add two icons to ImageList
	hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	ImageList_AddIcon(hSmall, hIcon);
	DestroyIcon(hIcon);
// Assign ImageList to List View
	ListView_SetImageList(GetDlgItem(hWnd,IDC_AUTOPARAMLIST1), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_AUTOPARAMLIST2), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_AUTOPARAMLIST3), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_AUTOPARAMLIST4), hSmall, LVSIL_STATE);
//end 扩展ListView行高
	fn_auto_setLV(hWnd);//用fn_auto_setLV函数填充ListView内容
	return 0;

}
//end 2010-4-5
LRESULT goCommandAutoParam(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//定义菜单ID号和通知单
	int menuID, notifyCode;
	
	//定义窗口句柄
	HWND wndCtrl;
	menuID    = LOWORD(wParam); 
	notifyCode = HIWORD(wParam); 
	wndCtrl = (HWND) lParam;
	return 0;
}
LRESULT goListAutoParam(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
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
	if(menuID==IDC_AUTOPARAMLIST2||menuID==IDC_AUTOPARAMLIST4)//只相应第2、4列发出的消息
	{		
		if(pnmh->code==NM_CLICK)
		{			
			nSelItem =-1;
			nSelItem =  ListView_GetNextItem(wndCtrl,-1,LVNI_SELECTED);			
			ListView_SetItemState(GetDlgItem(hWnd,IDC_AUTOPARAMLIST1),-1,LVIS_SELECTED, 0XFF);
			ListView_SetItemState(GetDlgItem(hWnd,IDC_AUTOPARAMLIST2),-1,LVIS_SELECTED, 0XFF);
			ListView_SetItemState(GetDlgItem(hWnd,IDC_AUTOPARAMLIST3),-1,LVIS_SELECTED, 0XFF);
			ListView_SetItemState(GetDlgItem(hWnd,IDC_AUTOPARAMLIST4),-1,LVIS_SELECTED, 0XFF);				
			if (nSelItem >= 0 )
			{			
					SetFocus(GetDlgItem(GetParent(hWnd),IDC_AUTOPARAMEDIT));
					ListView_SetItemState(wndCtrl,nSelItem,LVIS_DROPHILITED, 0XFF);
					
			}
			pDataInAutoInput->hWnd = hWnd;
			pDataInAutoInput->wndCtrl = wndCtrl;
			pDataInAutoInput->menuID = menuID;
			pDataInAutoInput->notifyCode = 0;
			AutoInput=4;
		}
	}
	else if(menuID==IDC_AUTOPARAMLIST1||menuID==IDC_AUTOPARAMLIST3)//对1、3列发出的消息只是不响应。
	{
		if(pnmh->code==NM_CLICK)
		{
			SetFocus(hWnd);//将焦点转移到当前只窗口，这样下面的命令就会将当前所有ListView的高亮标记消除
			ListView_SetItemState(GetDlgItem(hWnd,IDC_AUTOPARAMLIST1),-1,LVIS_SELECTED, 0XFF);
			ListView_SetItemState(GetDlgItem(hWnd,IDC_AUTOPARAMLIST2),-1,LVIS_SELECTED, 0XFF);
			ListView_SetItemState(GetDlgItem(hWnd,IDC_AUTOPARAMLIST3),-1,LVIS_SELECTED, 0XFF);
			ListView_SetItemState(GetDlgItem(hWnd,IDC_AUTOPARAMLIST4),-1,LVIS_SELECTED, 0XFF);
		}
	}
	return 0;
}
void ListAutoParamThreadProc()
{ 
    TCHAR szBuffer[31];
	int nSelItem;	
	nSelItem = -1;
	nSelItem =  ListView_GetNextItem(GetDlgItem(pDataInAutoInput->hWnd,IDC_AUTOPARAMLIST2),-1,LVNI_DROPHILITED);
	if (nSelItem >= 0 ) 
	{
		GetWindowText (GetDlgItem(GetParent(pDataInAutoInput->hWnd),IDC_AUTOPARAMEDIT), szBuffer, 30);
		ListView_SetItemText(GetDlgItem(pDataInAutoInput->hWnd,IDC_AUTOPARAMLIST2),nSelItem,0,szBuffer);
		SetWindowText (GetDlgItem(GetParent(pDataInAutoInput->hWnd),IDC_AUTOPARAMEDIT), "");
		Machiningparam.item[nSelItem].value = atoi(szBuffer);
		//iowrite(hWnd,Machiningparam.item[nSelItem].hDevice,Machiningparam.item[nSelItem].ctrWord, Machiningparam.item[nSelItem].addr,  //将加工参数写入设备
		//	Machiningparam.item[nSelItem].value, 1);
	}
	nSelItem =  ListView_GetNextItem(GetDlgItem(pDataInAutoInput->hWnd,IDC_AUTOPARAMLIST4),-1,LVNI_DROPHILITED);
	if (nSelItem >= 0 ) 
	{
		GetWindowText (GetDlgItem(GetParent(pDataInAutoInput->hWnd),IDC_AUTOPARAMEDIT),szBuffer, 30);
		ListView_SetItemText(GetDlgItem(pDataInAutoInput->hWnd,IDC_AUTOPARAMLIST4),nSelItem,0,szBuffer);
		SetWindowText (GetDlgItem(GetParent(pDataInAutoInput->hWnd),IDC_AUTOPARAMEDIT),"");
		Machiningparam.item[10+nSelItem].value = atoi(szBuffer);
		//iowrite(hWnd,Machiningparam.item[10+nSelItem].hDevice,Machiningparam.item[10+nSelItem].ctrWord, Machiningparam.item[10+nSelItem].addr,  //将加工参数写入设备
		//	Machiningparam.item[10+nSelItem].value, 1);
	}	
	//将选择项蓝背景颜色去掉
	ListView_SetItemState(GetDlgItem(pDataInAutoInput->hWnd,IDC_AUTOPARAMLIST1),-1,LVIS_SELECTED, 0XFF);
	ListView_SetItemState(GetDlgItem(pDataInAutoInput->hWnd,IDC_AUTOPARAMLIST2),-1,LVIS_SELECTED, 0XFF);
	ListView_SetItemState(GetDlgItem(pDataInAutoInput->hWnd,IDC_AUTOPARAMLIST3),-1,LVIS_SELECTED, 0XFF);
	ListView_SetItemState(GetDlgItem(pDataInAutoInput->hWnd,IDC_AUTOPARAMLIST4),-1,LVIS_SELECTED, 0XFF);
	AutoInput=0;
}

LRESULT goPaintAutoParam(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(hWnd, &ps);
	// TODO: 在此添加任意绘图代码...
	EndPaint(hWnd, &ps);
	return 0;
}

LRESULT goDestroyAutoParam(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	PostQuitMessage(0);
	return 0;
}
void fn_auto_setLV(HWND hWnd)
{
	int i,j;
	LVITEM item;
	char chTemp1[100];
	int  nItemCount;
	//清空所有ListView内容
	for(j=0;j<4;j++)
	{
		SendMessage (GetDlgItem(hWnd,autoParamMenuBtns[j].nID), LVM_DELETEALLITEMS, 0, 0);
	}	
	//end清空所用ListView内容
//创建自动加工参数的列名目和对应参数	
	for(j=0;j<2;j++)
	{
		for(i=0;i<8;i++)
		{
			if(j*8+i==NOWITEMNUMBER)break;//所用的已有项目填充完毕就不再填充
			ZeroMemory(&item, sizeof(LV_ITEM));
			//填充1、3列
			nItemCount = ListView_GetItemCount(GetDlgItem(hWnd,autoParamMenuBtns[j*2].nID));
			item.iItem = nItemCount;
			item.mask = LVIF_TEXT;
			item.cchTextMax =10;
			item.pszText = (char *)malloc(10);
			SendMessage(GetDlgItem(hWnd,autoParamMenuBtns[j*2].nID),LVM_INSERTITEM,0,(LPARAM)&item);
			item.iSubItem = 0;
			strcpy(item.pszText, Machiningparam.item[8*j+i].name);
			SendMessage(GetDlgItem(hWnd,autoParamMenuBtns[j*2].nID),LVM_SETITEM,0,(LPARAM)&item);
			ZeroMemory(&item, sizeof(LV_ITEM));
			//填充2、4列
			nItemCount = ListView_GetItemCount(GetDlgItem(hWnd,autoParamMenuBtns[j*2+1].nID));
			item.iItem = nItemCount;
			item.mask = LVIF_TEXT;
			item.cchTextMax =10;
			item.pszText = (char *)malloc(10);
			SendMessage(GetDlgItem(hWnd,autoParamMenuBtns[j*2+1].nID),LVM_INSERTITEM,0,(LPARAM)&item);
			item.iSubItem = 0;
			wsprintf(chTemp1, "%d", Machiningparam.item[8*j+i].value);
			strcpy(item.pszText,chTemp1);
			SendMessage(GetDlgItem(hWnd,autoParamMenuBtns[j*2+1].nID),LVM_SETITEM,0,(LPARAM)&item);		
			//iowrite(hWnd,Machiningparam.item[10*j+i].hDevice,Machiningparam.item[10*j+i].ctrWord, Machiningparam.item[10*j+i].addr,  //将加工参数写入设备
			//	Machiningparam.item[10*j+i].value, 1);
		}
	}			 
	if(item.pszText!=NULL) free(item.pszText);
}