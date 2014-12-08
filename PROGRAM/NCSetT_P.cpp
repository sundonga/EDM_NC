#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;								// 当前实例

FILE* fpNow;
int NCSetTPCount;
void fn_countLineNumber(FILE*,int&);
void readContent(FILE*,char*,int,HWND);
void fn_changeFile(int,TCHAR*,HWND);
void fn_upday(HWND);
void NCSetT_PThreadProc();
//2010-6-24
extern int NCSet;
LPCmdThreadParam  pDataInNCSetT_P;
//end 2010-6-24
const MessageProc NCSetTPMessage[]=
{
	WM_CREATE,goCreateNCSetT_PWnd,
	WM_PAINT,goPaintNCSetT_PWnd,
	WM_NOTIFY,goListNCSetT_PWnd,	
	WM_COMMAND,goCommandNCSetT_PWnd,
	WM_DESTROY,goDestroyNCSetT_PWnd
};
//定义七个控件窗口的创建信息	
CtlWndStruct NCSetTPWnd[]={	
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_NCSETLV,TEXT(""),0,0,754,620,WS_VISIBLE|WS_BORDER|WS_CHILD|LVS_REPORT|LVS_ICON|LVS_EDITLABELS}	
};
int MyRegisterClassNCSetT_P(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	INITCOMMONCONTROLSEX icex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			=CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)NCSetT_PWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);//
	wcex.lpszMenuName	= NULL;//(LPCTSTR)IDC_EX1;
	wcex.lpszClassName	= TEXT("NCSETT_P");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
//注册自动窗口类
	if(!RegisterClassEx(&wcex)) return 0;
	icex.dwICC=ICC_LISTVIEW_CLASSES;
	icex.dwSize=sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&icex);
	return 1;
}
//子窗口过程体，必须为回调函数，当窗口收到消息后，会告知Windows CE系统执行对应的消息处理过程
LRESULT CALLBACK NCSetT_PWndProc(HWND hWnd,UINT msgCode,WPARAM wParam,LPARAM lParam)
{
	int i;
	//查找对应的消息ID号，并执行相应的消息处理过程
	for(i=0;i<dim(NCSetTPMessage);i++)
	{
		if(msgCode==NCSetTPMessage[i].uCode)
			return (*NCSetTPMessage[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//对于不再消息查找表中的消息，调用Windows CE的缺省窗口过程
	return DefWindowProc(hWnd,msgCode,wParam,lParam);
}
//子窗口收到WM_CREATE消息后的处理过程体
LRESULT  goCreateNCSetT_PWnd(HWND hWnd,UINT msgCode,WPARAM wParam,LPARAM lParam)
{
	int i;
	LVCOLUMN lvcol;
	HIMAGELIST hSmall;
	HICON hIcon;
    char * caption [] ={"NO.","DATA","CONTENT"};
	//创建自动加工参数窗口菜单
	for(i=0;i<1;i++)
	{
		CreateWindowEx(WS_EX_CLIENTEDGE,
			WC_LISTVIEW,
			NCSetTPWnd[i].szTitle,
			NCSetTPWnd[i].lStyle,
			NCSetTPWnd[i].x,
			NCSetTPWnd[i].y,
			NCSetTPWnd[i].cx,
			NCSetTPWnd[i].cy,
			hWnd,
			(HMENU)NCSetTPWnd[i].nID,
			hInst,
			NULL
			);
	}
	ZeroMemory(&lvcol, sizeof(LV_COLUMN));
	ListView_SetTextBkColor(GetDlgItem(hWnd,NCSetTPWnd[0].nID),RGB(255,255,255));
	lvcol.iOrder=2;
	lvcol.iSubItem = 3;
	lvcol.pszText = caption[2];
	lvcol.cx = 450;
	lvcol.fmt = LVCFMT_CENTER;
	lvcol.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM|LVCF_ORDER;
	lvcol.cchTextMax = 60;
	SendMessage (GetDlgItem(hWnd,NCSetTPWnd[0].nID), LVM_INSERTCOLUMN, 0, (LPARAM)&lvcol);
	lvcol.iOrder=0;
	lvcol.iSubItem = 2;
	lvcol.pszText = caption[0];
	lvcol.cx = 100;
	lvcol.fmt = LVCFMT_CENTER;
	lvcol.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM|LVCF_ORDER;
	lvcol.cchTextMax = 60;
	SendMessage (GetDlgItem(hWnd,NCSetTPWnd[0].nID), LVM_INSERTCOLUMN, 0, (LPARAM)&lvcol);
	lvcol.iOrder=1;
	lvcol.iSubItem = 0;
	lvcol.pszText = caption[1];
	lvcol.cx = 200;
	lvcol.fmt = LVCFMT_CENTER;
	lvcol.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM|LVCF_ORDER;
	lvcol.cchTextMax = 60;
	SendMessage (GetDlgItem(hWnd,NCSetTPWnd[0].nID), LVM_INSERTCOLUMN, 0, (LPARAM)&lvcol);
	ListView_SetExtendedListViewStyle(GetDlgItem(hWnd,NCSetTPWnd[0].nID),LVS_EX_GRIDLINES |LVS_EX_HEADERDRAGDROP |LVS_EX_FULLROWSELECT );
	// Create the ImageList
	hSmall = ImageList_Create(1,30,ILC_MASK, 1, 1);
// Add two icons to ImageList
	hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	ImageList_AddIcon(hSmall, hIcon);
	DestroyIcon(hIcon);
// Assign ImageList to List View
	ListView_SetImageList(GetDlgItem(hWnd,IDC_NCSETLV), hSmall, LVSIL_STATE);
	//2010-6-24
	pDataInNCSetT_P = (LPCmdThreadParam)HeapAlloc(GetProcessHeap(),
											HEAP_ZERO_MEMORY,
											sizeof(CmdThreadParam)
											);
	if(pDataInNCSetT_P == NULL)
	{
		return 1;
	}
	//end 2010-6-24
	return 0;
}
LRESULT goCommandNCSetT_PWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT goPaintNCSetT_PWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//定义画板结构体对象
	PAINTSTRUCT ps;
	HDC hdc;
	hdc=BeginPaint(hWnd,&ps);
	EndPaint(hWnd,&ps);
	return 0;	
}
LRESULT goDestroyNCSetT_PWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	PostQuitMessage(0);
	return 0;
}
LRESULT goListNCSetT_PWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//定义列表窗口ID号
	int menuID;
	//定义列表窗口句柄
	HWND wndCtrl;
	//定义消息信息结构体指针
	LPNMHDR pnmh;
    //定义指向包含消息结构和列表信息结构成员的结构体
	int nSelItem;
	
	//menuID    = LOWORD(wParam); 
	menuID    = int(wParam); 
	pnmh = (LPNMHDR)lParam;
	wndCtrl = pnmh->hwndFrom;
	
	if(menuID==IDC_NCSETLV)
	{		
		if(pnmh->code==NM_CLICK)
		{			
			nSelItem =-1;
			nSelItem =  ListView_GetNextItem(wndCtrl,-1,LVNI_SELECTED);			
			ListView_SetItemState(GetDlgItem(hWnd,IDC_NCSETLV),-1,LVIS_SELECTED, 0XFF);				
			if (nSelItem >= 0 )
			{
				SendMessage(GetDlgItem(hWnd,IDC_NCSETLV),LVM_EDITLABEL,nSelItem,0);
				SetFocus(GetDlgItem(GetParent(hWnd),IDC_NCSETPEDIT));
				ListView_SetItemState(wndCtrl,nSelItem,LVIS_DROPHILITED, 0XFF);					
			}
			pDataInNCSetT_P->hWnd = hWnd;
			pDataInNCSetT_P->wndCtrl = wndCtrl;
			pDataInNCSetT_P->menuID = menuID;
			pDataInNCSetT_P->notifyCode = 0;
			NCSet=1;
		}
	}
	return 0;
}
void NCSetT_PThreadProc()
{
    TCHAR szBuffer[31];
	int nSelItem;	
	nSelItem = -1;
	nSelItem =  ListView_GetNextItem(GetDlgItem(pDataInNCSetT_P->hWnd,IDC_NCSETLV),-1,LVNI_DROPHILITED);

	if (nSelItem >= 0 ) 
	{					
		GetWindowText (GetDlgItem(GetParent(pDataInNCSetT_P->hWnd),IDC_NCSETPEDIT), szBuffer, 30);
		ListView_SetItemText(GetDlgItem(pDataInNCSetT_P->hWnd,IDC_NCSETLV),nSelItem,0,szBuffer);
		fn_changeFile(nSelItem+1,szBuffer,pDataInNCSetT_P->hWnd);
		SetWindowText (GetDlgItem(GetParent(pDataInNCSetT_P->hWnd),IDC_NCSETPEDIT),"");
	}	
	//将选择项蓝背景颜色去掉
	ListView_SetItemState(GetDlgItem(pDataInNCSetT_P->hWnd,IDC_NCSETLV),-1,LVIS_SELECTED, 0XFF);
	NCSet=0;
}
void fn_countLineNumber(FILE* fp,int& count)
{
	char c;
	rewind(fp);
	while((c=char(fgetc(fp)))!=EOF)
	{
		if(c=='\n')count++;
	}
	count++;
}
void readContent(FILE* fp,char* a,int line,HWND hWnd)
{
	int i;
	char b;
	rewind(fp);
	for(i=0;i<(line-1);i++)
	{
		while((b=char(fgetc(fp)))!='\n');
	}
		
	i=0;
	while(a[i]=char(fgetc(fp)))
	{
		if(a[i]!='\n'&&a[i]!=EOF)i++;
		
		else break;
	}
	a[i]='\0';
}
void fn_changeFile(int lineNo,char* inLine,HWND hWnd)
{
	FILE* temp;
	int i;
	char c;
	temp=fopen("NCSetTemp.txt","w");
	rewind(fpNow);
	for(i=0;i<(lineNo-1)*3+1;i++)
	{
		while((c=char(fgetc(fpNow)))!='\n')fputc(int(c),temp);
		fputc(int(c),temp);
	}
	for(i=0;i<20;i++)
	{
		if(inLine[i]=='\0')break;
		fputc(int(inLine[i]),temp);
	}
	c='\n';
	fputc(int(c),temp);
	while((c=(char(fgetc(fpNow))))!='\n');
	while((c=TCHAR(fgetc(fpNow)))!=EOF)fputc(int(c),temp);
	fclose(temp);
	temp=fopen("NCSetTemp.txt","r");
	rewind(temp);
	rewind(fpNow);
	while((c=TCHAR(fgetc(temp)))!=EOF)fputc(int(c),fpNow);
	fclose(temp);
}
void fn_upday(HWND hWnd)
{
	int i;
	LVITEM item;
	int   nItemCount;
	char content[30];
	NCSetTPCount=0;
	fn_countLineNumber(fpNow,NCSetTPCount);
	NCSetTPCount=NCSetTPCount/3;	
	SendMessage (GetDlgItem(hWnd,NCSetTPWnd[0].nID), LVM_DELETEALLITEMS, 0, 0);
	for(i=0;i<NCSetTPCount;i++)
	{
		ZeroMemory(&item, sizeof(LV_ITEM));
		nItemCount = ListView_GetItemCount(GetDlgItem(hWnd,NCSetTPWnd[0].nID));
		item.iItem = nItemCount;
		item.mask = LVIF_TEXT;
		item.cchTextMax =30;
		item.pszText = (char *)malloc(31);
		if(item.pszText == NULL)
		{
			MessageBox(hWnd,"can not malloc item.pszText goCreateAutoParam functiotn",NULL,NULL);
		}
		SendMessage(GetDlgItem(hWnd,NCSetTPWnd[0].nID),LVM_INSERTITEM,0,(LPARAM)&item);
		item.iSubItem = 0;
		rewind(fpNow);
		readContent(fpNow,content,i*3+1,hWnd);
		strcpy(item.pszText,content);
		SendMessage(GetDlgItem(hWnd,NCSetTPWnd[0].nID),LVM_SETITEM,0,(LPARAM)&item);
		item.iSubItem = 1;
		rewind(fpNow);
		readContent(fpNow,content,i*3+2,hWnd);
		strcpy(item.pszText,content);
		SendMessage(GetDlgItem(hWnd,NCSetTPWnd[0].nID),LVM_SETITEM,0,(LPARAM)&item);
		item.iSubItem = 2;
		rewind(fpNow);
		readContent(fpNow,content,i*3+3,hWnd);
		strcpy(item.pszText,content);
		SendMessage(GetDlgItem(hWnd,NCSetTPWnd[0].nID),LVM_SETITEM,0,(LPARAM)&item);
	}
}