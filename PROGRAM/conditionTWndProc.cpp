#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"
extern HINSTANCE hInst;								// 当前实例

//2010-3-30
extern int length;//规定每个数据段写入长度为6个字节，其大小=加工参数文件最大位数+2
extern int conditionLength;//设定加工参数文件最大数目为9999个
extern FILE* fpTemp;//定位temp.txt文件的指针
extern FILE* fpSeek;//定位seek.txt文件的指针
extern FILE* fpLock;//定位lock.txt文件的指针
int NOWSELECTLISTID;//记录当前所点击的列位置
int NOWSELECTLISTITEM;//记录当前所点击的行位置
int startR;//记录当前所显示信息最后一列的文件序号
int VPos;//竖直滚动条位置
int HPos;//水平滚动条位置
extern int ConditionNumber;//当前参数文件数量
extern HWND hScreenEdit;//启动页面句柄，如果需要在画面内加入提示语句就需要改句柄
int HPosIsCONDB;//滚动条是否在没有拖动情况下被修改 1是，0否
//2010-4-1
extern void setItem(int*,int);//读取对应的加工参数文件信息，将标题保存在itemIn[0]中，其余位置保存对应的参数信息
extern int seekNext(int);
extern int seekPre(int);
extern int getLockCondition(int);
void conditionUpdate(HWND);//刷新ListView内容
void ConditionThreadProc();
extern int fnSearch(HWND,int);//该函数功能为判断所输入的加工参数文件序号是否存在
void setColume(HWND,int,int);//设定某一列内容
HWND hWndConT;
bool isCondition;
LPCmdThreadParam  pDataCondition;
//
//定义全局消息ID号与子窗口消息处理过程指针查找表
const MessageProc tableChildMessage[]={
	WM_CREATE,goCreateConditionTWnd,
		WM_PAINT,goPaintConditionTWnd,
		WM_COMMAND,goCommandConditionTWnd,
		WM_NOTIFY,goListConditionTWnd,
		WM_VSCROLL,goVscrollConditionTWnd,
		WM_HSCROLL,goHscrollConditionTWnd,
		WM_DESTROY,goDestroyConditionTWnd
};	  
//定义控件窗口的创建信息
//每一个ListView代表一列信息
//加工参数建立过程如下：每一个ListView代表一列的信息，ListView内的行数量为一定值，在拖动滚动条过程中改变的只是ListView的内容。即ListView本身没有
//滚动条，10个ListView公用一个滚动条
CtlWndStruct controlTWnd[]={
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_CONTL0,TEXT(""),0,0,100,514,WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_ICON},
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_CONTL1,TEXT(""),100,0,100,514,WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_ICON},
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_CONTL2,TEXT(""),200,0,100,514,WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_ICON},
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_CONTL3,TEXT(""),300,0,100,514,WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_ICON},
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_CONTL4,TEXT(""),400,0,100,514,WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_ICON},
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_CONTL5,TEXT(""),500,0,100,514,WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_ICON},
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_CONTL6,TEXT(""),600,0,100,514,WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_ICON},
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_CONTL7,TEXT(""),700,0,100,514,WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_ICON},
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_CONTL8,TEXT(""),800,0,100,514,WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_ICON},
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_CONTL9,TEXT(""),900,0,100,514,WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_ICON}
};
//注册子窗口类
int MyRegisterClassConditionT(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)conditionTWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH) GetSysColorBrush(COLOR_WINDOW);//(HBRUSH)(COLOR_WINDOW+1);//
	wcex.lpszMenuName	= NULL;//(LPCTSTR)IDC_EX1;
	wcex.lpszClassName	= TEXT("CONDITIONTWND");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
//注册自动窗口类
	if(!RegisterClassEx(&wcex)) return 0;

	return 1;
}
//子窗口过程体，必须为回调函数，当窗口收到消息后，会告知Windows CE系统执行对应的消息处理过程
LRESULT CALLBACK conditionTWndProc(HWND hWnd,UINT msgCode,WPARAM wParam,LPARAM lParam)
{
	int i;
	//查找对应的消息ID号，并执行相应的消息处理过程
	for(i=0;i<dim(tableChildMessage);i++)
	{
		if(msgCode==tableChildMessage[i].uCode)
			return (*tableChildMessage[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//对于不再消息查找表中的消息，调用Windows CE的缺省窗口过程
	return DefWindowProc(hWnd,msgCode,wParam,lParam);
}
//子窗口收到WM_CREATE消息后的处理过程体
LRESULT  goCreateConditionTWnd(HWND hWnd,UINT msgCode,WPARAM wParam,LPARAM lParam)
{
	HIMAGELIST hSmall;
	HICON hIcon;
	int i;
	LVCOLUMN lvcol;
	LVITEM item;
	int nItemCount;
	char * caption [] ={"CONTENT"};
	SCROLLINFO si;//滚动条变量
	LPCREATESTRUCT lpcs;
	int k;
	//创建自动加工参数窗口菜单
	for(i=0;i<10;i++)
	{
		CreateWindowEx(WS_EX_WINDOWEDGE,
			WC_LISTVIEW,
			controlTWnd[i].szTitle,
			controlTWnd[i].lStyle,
			controlTWnd[i].x,
			controlTWnd[i].y,
			controlTWnd[i].cx,
			controlTWnd[i].cy,
			hWnd,
			(HMENU)controlTWnd[i].nID,
			hInst,
			NULL
			);
	}
//创建参数的名目栏
	for(k=0;k<10;k++)
	{
		ZeroMemory(&lvcol, sizeof(LV_COLUMN));
		ListView_SetTextBkColor(GetDlgItem(hWnd,controlTWnd[k].nID),RGB(255,255,255));
		lvcol.cx =100;//ListView宽100
		lvcol.iSubItem =0;
		lvcol.fmt = LVCFMT_CENTER;
		lvcol.pszText =caption[0];
		lvcol.cchTextMax = 60;
		lvcol.mask =LVCF_SUBITEM|LVCF_WIDTH|LVCF_TEXT|LVCF_FMT;
		//由于ListView的特定如果只参入第0列的话那么默认就是左对齐，为了实现居中对齐需要分别插入0,1列然后将0列删除
		SendMessage (GetDlgItem(hWnd,controlTWnd[k].nID), LVM_INSERTCOLUMN,0,(LPARAM)&lvcol);
		SendMessage (GetDlgItem(hWnd,controlTWnd[k].nID), LVM_INSERTCOLUMN,1,(LPARAM)&lvcol);
		SendMessage (GetDlgItem(hWnd,controlTWnd[k].nID),LVM_DELETECOLUMN ,0,0);	
		//end
		ListView_SetExtendedListViewStyle(GetDlgItem(hWnd,controlTWnd[k].nID),LVS_EX_FULLROWSELECT );
		SendMessage (GetDlgItem(hWnd,controlTWnd[k].nID), LVM_DELETEALLITEMS, 0, 0);
		//插入行
		for(i=0;i<9;i++)
		{
			ZeroMemory(&item, sizeof(LV_ITEM));
			nItemCount = ListView_GetItemCount(GetDlgItem(hWnd,controlTWnd[k].nID));
			item.iItem = nItemCount;
			SendMessage(GetDlgItem(hWnd,controlTWnd[k].nID),LVM_INSERTITEM,0,(LPARAM)&item);
		}
	}
	//设置滚动条
	lpcs=(LPCREATESTRUCT)lParam;
	si.cbSize=sizeof(si);
	si.nMin=0;
//	设定水平滚动条
	si.nMax=lpcs->cx+(ConditionNumber-9)*100;//(ConditionNumber-9)*100为滚动条最大滚动量。
	//通过查询得到lpcs->cx=1024，在需要修改滚动条时在设定si.nMax时将lpcs->cx用1024代替
	si.nPage=lpcs->cx;
	si.nPos=0;
	si.fMask=SIF_ALL;
	SetScrollInfo(hWnd,SB_HORZ,&si,TRUE);
	//设定数值滚动条
	si.nMax=lpcs->cy+100;//162为50*3得到
	si.nPage=lpcs->cy;
	si.nPos=0;
	SetScrollInfo(hWnd,SB_VERT,&si,TRUE);
	if(fnSearch(hWnd,startR)!=1)startR=0;
	NOWSELECTLISTID=0;
	NOWSELECTLISTITEM=0;
	VPos=0;
	HPos=0;
	HPosIsCONDB=0;
	//设置ListView的行高为50
	// Create the ImageList
	hSmall = ImageList_Create(1,50,ILC_MASK, 1, 1);
// Add two icons to ImageList
	hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	ImageList_AddIcon(hSmall, hIcon);
	DestroyIcon(hIcon);
// Assign ImageList to List View
	ListView_SetImageList(GetDlgItem(hWnd,IDC_CONTL0), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_CONTL1), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_CONTL2), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_CONTL3), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_CONTL4), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_CONTL5), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_CONTL6), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_CONTL7), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_CONTL8), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_CONTL9), hSmall, LVSIL_STATE);
	//刷新ListView
	conditionUpdate(hWnd);
	hWndConT=hWnd;

	//
	isCondition=FALSE;
	pDataCondition = (LPCmdThreadParam)HeapAlloc(GetProcessHeap(),
											HEAP_ZERO_MEMORY,
											sizeof(CmdThreadParam)
											);
	if(pDataCondition == NULL)
	{
		return 1;
	}

	return 0;
}
LRESULT goCommandConditionTWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	return 0;
}
//鼠标点击过程的函数相应
//当鼠标点击到某一参数项目时刷新屏幕以将对应的项目背景设为蓝色，同时将焦点设到输入文本框中以修改该参数数值
LRESULT goListConditionTWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//定义列表窗口ID号
	int menuID;
	//定义列表窗口句柄
	HWND wndCtrl;
	//定义消息信息结构体指针
	LPNMHDR pnmh;
    //定义指向包含消息结构和列表信息结构成员的结构体
	int nSelItem;
	int i;
    //定义指向包含消息结构和列表信息结构成员的结构体
	int line,row;
	int time,ENO,lockCondition;
	menuID    = int(wParam); 
	pnmh = (LPNMHDR)lParam;
	wndCtrl = pnmh->hwndFrom;
	if(menuID==IDC_CONTL1||menuID==IDC_CONTL2||menuID==IDC_CONTL3||menuID==IDC_CONTL4||menuID==IDC_CONTL5||menuID==IDC_CONTL6||menuID==IDC_CONTL7||menuID==IDC_CONTL8||menuID==IDC_CONTL9)
	{		
		if(pnmh->code==NM_CLICK)
		{
			nSelItem =-1;
			nSelItem =  ListView_GetNextItem(wndCtrl,-1,LVNI_SELECTED);
			line=menuID-IDC_CONTL0;//记录当前所选定的ListView序号
			row=nSelItem+1;//记录当前所记录的行序号,是屏幕内显示的行序号
			//去除高亮
			for(i=0;i<9;i++)
			ListView_SetItemState(GetDlgItem(hWnd,IDC_CONTL1+i),-1,LVIS_SELECTED, 0XFF);
			if (nSelItem >= 0 )
			{
				if(line==NOWSELECTLISTID&&row==NOWSELECTLISTITEM)return 0; ///??????????????????????????为什么要退出
				NOWSELECTLISTID=line;
				NOWSELECTLISTITEM=row;
				//得到当前所点击的列对应的参数文件序号
				time=9-NOWSELECTLISTID;
				ENO=startR;
				for(i=0;i<time;i++)
				{
					ENO=seekPre(ENO);
					if(ENO==0)ENO=seekPre(ENO);
				}
				ListView_SetItemState(wndCtrl,nSelItem,LVIS_DROPHILITED, 0XFF);
				//得到当前所选择参数文件是否为锁定，如果锁定修改LOCK BUTTON的输出文本内容
				lockCondition=getLockCondition(ENO);
				if(lockCondition==0)SetWindowText (GetDlgItem(GetDlgItem(GetParent(hWnd),IDW_CONDB),IDC_BLOCK),"LOCK");
				else SetWindowText (GetDlgItem(GetDlgItem(GetParent(hWnd),IDW_CONDB),IDC_BLOCK),"UNLOCK");
				//启动另一个线程用于修改参数数值
				SetFocus(GetDlgItem(GetDlgItem(GetParent(hWnd),IDW_CONDB),IDC_TIN));
				pDataCondition->hWnd = hWnd;
				pDataCondition->wndCtrl = wndCtrl;
				pDataCondition->menuID = menuID;
				pDataCondition->notifyCode = 0;
				isCondition=TRUE;
			}
		}
	}
	else if(menuID==IDC_CONTL0)//如果点击项目名称则将其他高亮的ListView高亮消除
	{		
		if(pnmh->code==NM_CLICK)
		{
			SetFocus(hWnd);
			for(i=-1;i<9;i++)
			ListView_SetItemState(GetDlgItem(hWnd,IDC_CONTL1+i),-1,LVIS_SELECTED, 0XFF);
		}
	}
	return 0;
}
//重画屏幕时所调用的函数
LRESULT goPaintConditionTWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//定义画板结构体对象
	PAINTSTRUCT ps;
	HDC hdc;
	hdc=BeginPaint(hWnd,&ps);
	EndPaint(hWnd,&ps);
	return 0;
}
LRESULT goDestroyConditionTWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	PostQuitMessage(0);
	return 0;
}
//拖动竖直滚动条时的函数相应
LRESULT goVscrollConditionTWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;
	int oldPos;
	SCROLLINFO si;
	oldPos=VPos;
	switch(LOWORD(wParam))
	{
	case SB_BOTTOM://滚动条移动到底部
		VPos=2;
		break;
	case SB_TOP://顶部
		VPos=0;
		break;
	case SB_LINEUP:
		VPos-=1;
		break;
	case SB_LINEDOWN:
		VPos+=1;
		break;
	case SB_PAGEUP:
		VPos-=1;
		break;
	case SB_PAGEDOWN:
		VPos+=1;
		break;
	case SB_THUMBPOSITION:
		VPos=HIWORD(wParam)/50;
		break;
	}
	if(VPos<0)
		VPos=0;
	if(VPos>2)
		VPos=2;
	if(VPos!=oldPos)
	{
		si.cbSize=sizeof(si);
		si.nPos=VPos*50;
		SetScrollInfo(hWnd,SB_VERT,&si,TRUE);
		for(i=9;i>=1;i--)
		{
			startR=seekPre(startR);
			if(startR==0)startR=seekPre(startR);
		}
		NOWSELECTLISTID=0;
		NOWSELECTLISTITEM=0;
		conditionUpdate(hWnd);
		//清除高亮
		SetFocus(hWnd);
		for(i=-1;i<9;i++)
		ListView_SetItemState(GetDlgItem(hWnd,IDC_CONTL1+i),-1,LVIS_SELECTED, 0XFF);
	}
	return 0;
}

//拖动水平滚动条时的函数相应
LRESULT goHscrollConditionTWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;
	int oldPos;
	SCROLLINFO si;
	oldPos=HPos;
	switch(LOWORD(wParam))
	{
	case SB_LINELEFT:
		HPos-=1;
		break;
	case SB_LINERIGHT:
		HPos+=1;
		break;
	case SB_PAGELEFT:
		HPos-=9;
		break;
	case SB_PAGERIGHT:
		HPos+=9;
		break;
	case SB_THUMBPOSITION:
		HPos=HIWORD(wParam)/100;
		break;
	case SB_LEFT:
		HPos=0;
		break;
	case SB_RIGHT:
		HPos=ConditionNumber-9;
		break;
	}
	//滚动条大小需要改变
	if(HPosIsCONDB==1)
	{
		HPos++;
		si.cbSize=sizeof(si);
		si.nPos=HPos*100;
		si.nMin=0;
		si.nPage=1024;
		si.fMask=SIF_ALL;
		si.nMax=1024+(ConditionNumber-9)*100;
		SetScrollInfo(hWnd,SB_HORZ,&si,TRUE);
		NOWSELECTLISTID=0;
		NOWSELECTLISTITEM=0;
		conditionUpdate(hWnd);
	}
	//滚动条大小不需要改变
	else 
	{
		if(HPos<0)HPos=0;
		if(HPos>ConditionNumber-9)
			HPos=ConditionNumber-9;
		if(HPos!=oldPos)
		{
			si.cbSize=sizeof(si);
			si.nPos=HPos*100;
			SetScrollInfo(hWnd,SB_HORZ,&si,TRUE);
			if(HPos-oldPos>0)
			{
				for(i=0;i<(HPos-oldPos);i++)startR=seekNext(startR);
			}
			else if(HPos-oldPos<0)
			{
				for(i=0;i<(oldPos-HPos);i++)startR=seekPre(startR);
			}
			for(i=9;i>=1;i--)
			{
				startR=seekPre(startR);
				if(startR==0)startR=seekPre(startR);
			}
			NOWSELECTLISTID=0;
			NOWSELECTLISTITEM=0;
			conditionUpdate(hWnd);
		}
	}
	SetFocus(hWnd);
	for(i=-1;i<9;i++)
	ListView_SetItemState(GetDlgItem(hWnd,IDC_CONTL1+i),-1,LVIS_SELECTED, 0XFF);
	HPosIsCONDB=0;
	return 0;
}
//修改参数数值线程
void ConditionThreadProc(void)
{ 
	int i,j;
	char a[6];
	int ENO;
	int time;
	int inCondition;
	char szBuffer[31];
	int nSelItem;
	ENO=-1;
	nSelItem = -1;
	nSelItem =  ListView_GetNextItem(pDataCondition->wndCtrl,-1,LVNI_DROPHILITED);
	GetWindowText (GetDlgItem(GetDlgItem(GetParent(pDataCondition->hWnd),IDW_CONDB),IDC_TIN), szBuffer, 30);//得到输入文本框内所输入数值
	SetWindowText (GetDlgItem(GetDlgItem(GetParent(pDataCondition->hWnd),IDW_CONDB),IDC_TIN),"");
	inCondition=atoi(szBuffer);
	if(inCondition>-1&&inCondition<100000)
	{
		if(inCondition!=-1)
		{
			time=9-NOWSELECTLISTID;
			ENO=startR;
			for(i=0;i<time;i++)
			{
				ENO=seekPre(ENO);
				if(ENO==0)ENO=seekPre(ENO);
			}
			//如果所要修改的参数被锁定显示该参数已被锁定
			if(getLockCondition(ENO)==1)
			{
				SetWindowText (GetDlgItem(GetParent(pDataCondition->hWnd),IDC_STATICALARM),"THIS ITEM IS LOCKED");
			}
			else
			{
				fseek(fpTemp,long((((conditionLength+1)*length+2)*(NOWSELECTLISTITEM+VPos)+(ENO)*length+1)*sizeof(char)),SEEK_SET);//定位fpTemp指针
				for(i=0;i<5;i++)fputc(int(' '),fpTemp);
				fseek(fpTemp,long((((conditionLength+1)*length+2)*(NOWSELECTLISTITEM+VPos)+(ENO)*length+1)*sizeof(char)),SEEK_SET);//定位fpTemp指针
				itoa(inCondition,a,10);
				for(i=0;i<6;i++)if(a[i]=='\0')break;
				for(j=0;j<i;j++)fwrite(&a[j],sizeof(char),1,fpTemp);
				ListView_SetItemText(pDataCondition->wndCtrl,nSelItem,0,szBuffer);
			}
		}
	}
	else
	{
		SetWindowText (GetDlgItem(GetParent(pDataCondition->hWnd),IDC_STATICALARM),"Must Be >=0 and <100000");
	}
	//将选择项蓝背景颜色去掉
	for(i=0;i<9;i++)ListView_SetItemState(GetDlgItem(pDataCondition->hWnd,IDC_CONTL1+i),-1,LVIS_SELECTED, 0XFF);
	isCondition=FALSE;
}
void conditionUpdate(HWND hWnd)
{
	int i;
	int j;
    char * caption [] ={"CONTENT","ON","OFF","SV","RV","IP","IPM","VEL","TENSION","FREEVEL","VOBJ","PID"};
	LVCOLUMN lvcol;
	LVITEM item;
	ZeroMemory(&lvcol, sizeof(LV_COLUMN));
	lvcol.iSubItem =0;
	lvcol.pszText =caption[0];
	lvcol.fmt = LVCFMT_CENTER;
	lvcol.mask =LVCF_TEXT|LVCF_SUBITEM;
	lvcol.cchTextMax = 60;
	SendMessage (GetDlgItem(hWnd,controlTWnd[0].nID), LVM_SETCOLUMN, 0, (LPARAM)&lvcol);
	//设置标题列
	for(j=0;j<9;j++)
	{
		ZeroMemory(&item, sizeof(LV_ITEM));
		item.iItem = j;
		item.mask = LVIF_TEXT;
		item.cchTextMax =30;
		item.iSubItem = 0;
//		SendMessage(GetDlgItem(hWnd,controlTWnd[0].nID),LVM_GETITEM,0,(LPARAM)&item);
		item.pszText =caption[j+VPos+1];
		SendMessage(GetDlgItem(hWnd,controlTWnd[0].nID),LVM_SETITEM,0,(LPARAM)&item);
	}
	//设置参数列
	for(i=1;i<=9;i++)
	{
		startR=seekNext(startR);
		if(startR==0)startR=seekNext(startR);
		setColume(hWnd,i,startR);
	}
	SetFocus(hWnd);
	for(i=-1;i<9;i++)
	ListView_SetItemState(GetDlgItem(hWnd,IDC_CONTL1+i),-1,LVIS_SELECTED, 0XFF);
}
//该函数功能为绘制某一列
void setColume(HWND hWnd,int columeNo,int ENO)
{
	int j;
	LVCOLUMN lvcol;
	LVITEM item;
	char chTemp1[100];
	char chTemp2[12];
	int inItem[12];
	setItem(inItem,ENO);
	itoa(inItem[0],chTemp1,10);
	for(j=1;j<=10;j++)chTemp2[j]=chTemp1[j-1];
	chTemp2[0]='E';
	ZeroMemory(&lvcol, sizeof(LV_COLUMN));
	lvcol.iSubItem =0;
	lvcol.pszText =chTemp2;
	lvcol.fmt = LVCFMT_CENTER;
	lvcol.mask = LVCF_TEXT|LVCF_SUBITEM;
	lvcol.cchTextMax = 60;
	SendMessage (GetDlgItem(hWnd,controlTWnd[columeNo].nID), LVM_SETCOLUMN, 0, (LPARAM)&lvcol);
	for(j=0;j<9;j++)
	{
		ZeroMemory(&item, sizeof(LV_ITEM));
		item.iItem = j;
		item.mask = LVIF_TEXT;
		item.cchTextMax =30;
		item.iSubItem = 0;
//		SendMessage(GetDlgItem(hWnd,controlTWnd[columeNo].nID),LVM_GETITEM,0,(LPARAM)&item);
		itoa(inItem[j+1+VPos],chTemp2,10);
		item.pszText =chTemp2;
		SendMessage(GetDlgItem(hWnd,controlTWnd[columeNo].nID),LVM_SETITEM,0,(LPARAM)&item);
		//如果对应的列为锁定则将其背景显示为COLOR_SCROLLBAR，否则显示为COLOR_WINDOW
		if(getLockCondition(ENO)==1)SendMessage(GetDlgItem(hWnd,controlTWnd[columeNo].nID),LVM_SETTEXTBKCOLOR,0,GetSysColor(COLOR_SCROLLBAR));
		else if(getLockCondition(ENO)!=1)SendMessage(GetDlgItem(hWnd,controlTWnd[columeNo].nID),LVM_SETTEXTBKCOLOR,0,GetSysColor(COLOR_WINDOW));
	}
}