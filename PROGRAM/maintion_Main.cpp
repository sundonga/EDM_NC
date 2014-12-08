#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;								// 当前实例
HANDLE MaintionInputhThread;
FILE *fpEIO;
FILE *fpEIO_Address;
HWND hEIOMain;
extern HANDLE DSPMemReadThread;
int nowActivateChild[4];
extern int Ram_Flag;
//2010-6-24
int MaintionInput;
LPCmdThreadParam  pDataInMaintion;
extern void EIO_1_Edit_ThreadProc();//edit输入对应的线程
extern void EIO_1_LV_ThreadProc();//ListView输入对应的线程
extern void EIO_2_Edit_ThreadProc();
extern void EIO_2_LV_ThreadProc();
extern void DSPMemReadThreadProc();
//end 2010-6-24

const MessageProc maintionMessages[]={
	    WM_CREATE, goCreateMaintion,
		WM_COMMAND, goCommandMaintion,
		WM_KEYDOWN, goKeydownMaintion,
		WM_PAINT,  goPaintMaintion,
		WM_DESTROY, goDestroyMaintion
};
CtlWndStruct maintionMenuBtns[]={		
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_MAINTION_MAIN_EIO,TEXT("EIO"),0,50,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP | BS_PUSHBUTTON|SS_CENTER},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_MAINTION_MAIN_RAM,TEXT("RAM"),0,100,100,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP | BS_PUSHBUTTON|SS_CENTER},
		{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_MAINTION_MAIN_EDIT,TEXT(""),100,565,900,30,WS_VISIBLE|WS_BORDER|WS_CHILD|ES_UPPERCASE}				
};

//子窗口菜单列表
CtlWndStruct maintionChildWnds[]={
	{WS_EX_CLIENTEDGE,TEXT("MAINTION_EIO_1"),IDW_MAINTION_EIO_1,TEXT(""),100,0,462,560,WS_CHILD|SS_NOTIFY|WS_BORDER|WS_VISIBLE},
	{WS_EX_CLIENTEDGE,TEXT("MAINTION_EIO_2"),IDW_MAINTION_EIO_2,TEXT(""),562,0,462,560,WS_CHILD|SS_NOTIFY|WS_BORDER|WS_VISIBLE},
	{WS_EX_CLIENTEDGE,TEXT("MAINTION_RAM"),IDW_MAINTION_RAM,TEXT(""),100,0,924,560,WS_CHILD|SS_NOTIFY|WS_BORDER|WS_VISIBLE}	
};

int MyRegisterClassMaintion(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)maintionWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);//
	wcex.lpszMenuName	= NULL;//(LPCTSTR)IDC_EX1;
	wcex.lpszClassName	= TEXT("MAINTION");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
//注册自动窗口类
	if(!RegisterClassEx(&wcex)) return 0;
	if(!MyRegisterClassMaintion_EIO_1(hInstance))  return 0;
	if(!MyRegisterClassMaintion_EIO_2(hInstance))  return 0;
	if(!MyRegisterClassMaintion_RAM(hInstance))  return 0;
	return 1;
}

//自动窗口过程体
LRESULT CALLBACK maintionWndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;	

	//查找对应的消息ID,并执行对应的消息处理过程
	for(i=0; i<dim(maintionMessages);i++)
	{
		if(msgCode == maintionMessages[i].uCode) return(*maintionMessages[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//对不存在的消息，调用缺省窗口过程
	return DefWindowProc(hWnd, msgCode, wParam, lParam);
}

LRESULT goCreateMaintion(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		
    int i;
//创建自动窗口菜单
	for(i=0;i<dim(maintionMenuBtns);i++){
		CreateWindowEx(maintionMenuBtns[i].dwExStyle,
			maintionMenuBtns[i].szClass,
			maintionMenuBtns[i].szTitle,
			maintionMenuBtns[i].lStyle,
			maintionMenuBtns[i].x,
			maintionMenuBtns[i].y,
			maintionMenuBtns[i].cx,
			maintionMenuBtns[i].cy,
			hWnd,
			(HMENU)maintionMenuBtns[i].nID,
			hInst,
			NULL
			);
	}
//创建自动窗口的子窗口
	
	for(i=0;i<dim(maintionChildWnds);i++){
		CreateWindowEx(maintionChildWnds[i].dwExStyle,
			maintionChildWnds[i].szClass,
			maintionChildWnds[i].szTitle,
			maintionChildWnds[i].lStyle,
			maintionChildWnds[i].x,
			maintionChildWnds[i].y,
			maintionChildWnds[i].cx,
			maintionChildWnds[i].cy,
			hWnd,
			(HMENU)maintionChildWnds[i].nID,
			hInst,
			NULL
			);
	}
	nowActivateChild[0]=nowActivateChild[1]=nowActivateChild[2]=nowActivateChild[3]=0;
	//显示其中一个窗口
	ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_EIO_1),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_EIO_2),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_RAM),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_MAINTION_MAIN_SEG_SET),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_MAINTION_MAIN_RAM_SET),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_MAINTION_MAIN_OFSET_SET),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_MAINTION_MAIN_ABSOLUTE_SET),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_MAINTION_MAIN_LENGTH_SET),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_MAINTION_MAIN_CLEAR),SW_HIDE);
	hEIOMain=hWnd;
	//2010-6-24
	MaintionInput=0;
	pDataInMaintion = (LPCmdThreadParam)HeapAlloc(GetProcessHeap(),
											HEAP_ZERO_MEMORY,
											sizeof(CmdThreadParam)
											);
	if(pDataInMaintion == NULL)
	{
		return 1;
	}
	//end 2010-6-24
	return 0;
}
LRESULT goCommandMaintion(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDC_MAINTION_MAIN_EIO:
		ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_EIO_1),SW_SHOW);
		ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_EIO_2),SW_SHOW);
		ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_RAM),SW_HIDE);
		ShowWindow(GetDlgItem(hWnd,IDC_MAINTION_MAIN_SEG_SET),SW_HIDE);
		ShowWindow(GetDlgItem(hWnd,IDC_MAINTION_MAIN_RAM_SET),SW_HIDE);
		ShowWindow(GetDlgItem(hWnd,IDC_MAINTION_MAIN_OFSET_SET),SW_HIDE);
		ShowWindow(GetDlgItem(hWnd,IDC_MAINTION_MAIN_ABSOLUTE_SET),SW_HIDE);
		ShowWindow(GetDlgItem(hWnd,IDC_MAINTION_MAIN_LENGTH_SET),SW_HIDE);
		ShowWindow(GetDlgItem(hWnd,IDC_MAINTION_MAIN_CLEAR),SW_HIDE);
		UpdateWindow(GetDlgItem(hWnd,IDW_MAINTION_EIO_1));
		UpdateWindow(GetDlgItem(hWnd,IDW_MAINTION_EIO_2));
		break;
	case IDC_MAINTION_MAIN_RAM:
		ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_EIO_1),SW_HIDE);
		ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_EIO_2),SW_HIDE);
		ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_RAM),SW_SHOW);
		UpdateWindow(GetDlgItem(hWnd,IDW_MAINTION_RAM));
		break;
	case IDC_MAINTION_MAIN_RAM_SET:
		break;
	default:
		break;
	}
		return 0;
}
LRESULT goKeydownMaintion(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{	
	int menuID, notifyCode;		
	//定义窗口句柄
	HWND wndCtrl;
	menuID    = LOWORD(wParam); 
	notifyCode = HIWORD(wParam); 
	wndCtrl = (HWND) lParam;
	switch(MaintionInput)
	{
	case 1:
		EIO_1_Edit_ThreadProc();
		break;
	case 2:
		EIO_1_LV_ThreadProc();
		break;
	case 3:
		EIO_2_Edit_ThreadProc();
		break;
	case 4:
		EIO_2_LV_ThreadProc();
		break;
	case 5:
		 DSPMemReadThreadProc();
		 break;
	}
	 return 0;
}

LRESULT goPaintMaintion(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码..		
		EndPaint(hWnd, &ps);
		return 0;
}
LRESULT goDestroyMaintion(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		PostQuitMessage(0);
		return 0;
}
void readEIO_Content(FILE* fpFile,int address)
{
	int i,j;
	for(i=1;i<address;i++)
	{
		for(j=0;j<17;j++)while(char(fgetc(fpFile))!='\n');
	}
}
void readNext(FILE* fpFile,char* content)
{
	int i;
	i=0;
	while(TRUE)
	{
		content[i]=char(fgetc(fpFile));
		if(content[i]=='\n'||content[i]==EOF)break;
		i++;
	}
	content[i]='\0';
}
void setListView(EIO& inEIO,HWND hLV)
{
	char content[30];
	int i;
	int search;
	LVITEM item;
	rewind(fpEIO_Address);
	rewind(fpEIO);
	search=getAddress(fpEIO_Address,inEIO.EIO);
	if(search!=0)
	{
		readEIO_Content(fpEIO,search);
		readNext(fpEIO,inEIO.Address);
		ZeroMemory(&item, sizeof(LV_ITEM));
		for(i=0;i<16;i++)
		{
			item.iItem = i;
			item.mask = LVIF_TEXT;
			item.pszText = (char *)malloc(31);
			item.iSubItem = 2;
			readNext(fpEIO,content);
			strcpy(item.pszText,content);
			SendMessage(hLV,LVM_SETITEM,0,(LPARAM)&item);
		}
	}
	else
	{
		SetWindowText (GetDlgItem(GetParent(GetParent(hLV)),IDC_STATICALARM),"THE EIO NO FOUND");//显示没有发现该文件，不做任何其他的动作
	}
}
int getAddress(FILE* fpFile,int address)
{
	int i;
	int search;
	char content[5];
	int count;
	count=1;
	while(TRUE)
	{
		
		for(i=0;i<5;i++)
		{
			content[i]=(char)fgetc(fpFile);
			if(content[i]==EOF)
			{
				content[i]='\0';
				search=atoi(content);
				if(address==search)return count;
				else return 0;
			}
			else if(content[i]==',')
			{
				content[i]='\0';
				search=atoi(content);
				if(address==search)return count;
				else break;
			}
		}
		count++;
	}
	return 0;
}
void setListViewEIO(EIO inEIO,HWND hLV)
{
	int i;
	for(i=0;i<16;i++)setListViewEIO_Line(inEIO,i,hLV);
}
void setListViewEIO_Line(EIO inEIO,int line,HWND hLV)
{
	char content[2];
	LVITEM item;
	ZeroMemory(&item, sizeof(LV_ITEM));
	item.iItem =line;
	item.mask = LVIF_TEXT;
	item.pszText = (char *)malloc(2);
	item.iSubItem = 0;
	itoa(inEIO.Bit_Address[line],content,10);
	strcpy(item.pszText,content);
	SendMessage(hLV,LVM_SETITEM,0,(LPARAM)&item);
}
void EIO_Change_Activate(HWND hChild)
{
	SetFocus(hChild);
}
void EIOclearEdit()
{
	SendMessage(GetDlgItem(GetDlgItem(hEIOMain,IDW_MAINTION_EIO_1),IDW_MAINTION_EIO_1_NUM),EM_SETSEL,-1,0);
	SendMessage(GetDlgItem(GetDlgItem(hEIOMain,IDW_MAINTION_EIO_2),IDW_MAINTION_EIO_2_NUM),EM_SETSEL,-1,0);
}
void EIOclearLV()
{
	ListView_SetItemState(GetDlgItem(GetDlgItem(hEIOMain,IDW_MAINTION_EIO_1),IDW_MAINTION_EIO_1_LV),-1,LVIS_SELECTED, 0XFF);
	ListView_SetItemState(GetDlgItem(GetDlgItem(hEIOMain,IDW_MAINTION_EIO_2),IDW_MAINTION_EIO_2_LV),-1,LVIS_SELECTED, 0XFF);
}