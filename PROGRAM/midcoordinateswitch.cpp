#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"
extern HINSTANCE hInst;								// 当前实例

int Coordina;
void CoordinatehreadProc();
LPCmdThreadParam  pDataInCoor;

void fc_upday(HWND hWnd);
void fc_countLineNumber(FILE* fp,int& count);
void fc_changeFile(int lineNo,int rowNo,char* inLine,HWND hWnd);
void readContentFc(FILE* fp,char* a,int line,HWND hWnd);
void readfc_to_buffer(HWND hWnd);
void readbuffer_to_fc(HWND hWnd);
extern FILE* fpcoor;
int CoorCount=0;
extern Coordata G54_coordinate,G55_coordinate,G56_coordinate,G57_coordinate,G58_coordinate,G59_coordinate;
HWND hWndCoor;

//定义全局消息ID号与子窗口消息处理过程指针查找表
const MessageProc coordinateMessage[]={
	WM_CREATE,goCreateCoordinateWnd,
		WM_PAINT,goPaintCoordinateWnd,
		WM_COMMAND,goCommandCoordinateWnd,
		WM_NOTIFY,goListCoordinateWnd,
		WM_DESTROY,goDestroyCoordinateWnd
};	  
//定义控件窗口的创建信息
//每一个ListView代表一列信息
//加工参数建立过程如下：每一个ListView代表一列的信息，ListView内的行数量为一定值，在拖动滚动条过程中改变的只是ListView的内容。即ListView本身没有
//滚动条，10个ListView公用一个滚动条
CtlWndStruct coordinateWnd[]={
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_COORL0,TEXT(""),0,0,100,308,WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_ICON},
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_COORL1,TEXT(""),100,0,100,308,WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_ICON},
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_COORL2,TEXT(""),200,0,100,308,WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_ICON},
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_COORL3,TEXT(""),300,0,100,308,WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_ICON},
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_COORL4,TEXT(""),400,0,100,308,WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_ICON},
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_COORL5,TEXT(""),500,0,100,308,WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_ICON},
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_COORL6,TEXT(""),600,0,100,308,WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_ICON}
};
//注册子窗口类
int MyRegisterClassCoordinate(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)CoordinateWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH) GetSysColorBrush(COLOR_WINDOW);//(HBRUSH)(COLOR_WINDOW+1);//
	wcex.lpszMenuName	= NULL;//(LPCTSTR)IDC_EX1;
	wcex.lpszClassName	= TEXT("CoordinateWND");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
//注册自动窗口类
	if(!RegisterClassEx(&wcex)) return 0;

	return 1;
}
//子窗口过程体，必须为回调函数，当窗口收到消息后，会告知Windows CE系统执行对应的消息处理过程
LRESULT CALLBACK CoordinateWndProc(HWND hWnd,UINT msgCode,WPARAM wParam,LPARAM lParam)
{
	int i;
	//查找对应的消息ID号，并执行相应的消息处理过程
	for(i=0;i<dim(coordinateMessage);i++)
	{
		if(msgCode==coordinateMessage[i].uCode)
			return (*coordinateMessage[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//对于不再消息查找表中的消息，调用Windows CE的缺省窗口过程
	return DefWindowProc(hWnd,msgCode,wParam,lParam);
}
//子窗口收到WM_CREATE消息后的处理过程体
LRESULT  goCreateCoordinateWnd(HWND hWnd,UINT msgCode,WPARAM wParam,LPARAM lParam)
{
	HIMAGELIST hSmall;
	HICON hIcon;
	int i;
	LVCOLUMN lvcol;
	LVITEM item;
	int nItemCount;
	char * caption [] ={"CONTENT","G54","G55","G56","G57","G58","G59"};
	char * text[]={"X","Y","Z","B","C"};
	int k;

	hWndCoor = hWnd;
	//创建自动加工参数窗口菜单
	for(i=0;i<7;i++)
	{
		CreateWindowEx(WS_EX_WINDOWEDGE,
			WC_LISTVIEW,
			coordinateWnd[i].szTitle,
			coordinateWnd[i].lStyle,
			coordinateWnd[i].x,
			coordinateWnd[i].y,
			coordinateWnd[i].cx,
			coordinateWnd[i].cy,
			hWnd,
			(HMENU)coordinateWnd[i].nID,
			hInst,
			NULL
			);
	}
//创建参数的名目栏
	for(k=0;k<7;k++)
	{
		ZeroMemory(&lvcol, sizeof(LV_COLUMN));
		ListView_SetTextBkColor(GetDlgItem(hWnd,coordinateWnd[k].nID),RGB(255,255,255));
		lvcol.cx =100;//ListView宽100
		lvcol.iSubItem =0;
		lvcol.fmt = LVCFMT_CENTER;
		lvcol.pszText =caption[k];
		lvcol.cchTextMax = 60;
		lvcol.mask =LVCF_SUBITEM|LVCF_WIDTH|LVCF_TEXT|LVCF_FMT;
		//由于ListView的特定如果只参入第0列的话那么默认就是左对齐，为了实现居中对齐需要分别插入0,1列然后将0列删除
		SendMessage (GetDlgItem(hWnd,coordinateWnd[k].nID), LVM_INSERTCOLUMN,0,(LPARAM)&lvcol);
		SendMessage (GetDlgItem(hWnd,coordinateWnd[k].nID), LVM_INSERTCOLUMN,1,(LPARAM)&lvcol);
		SendMessage (GetDlgItem(hWnd,coordinateWnd[k].nID),LVM_DELETECOLUMN ,0,0);	
		//end
		ListView_SetExtendedListViewStyle(GetDlgItem(hWnd,coordinateWnd[k].nID),LVS_EX_FULLROWSELECT );
		SendMessage (GetDlgItem(hWnd,coordinateWnd[k].nID), LVM_DELETEALLITEMS, 0, 0);
		//插入行
		for(i=0;i<5;i++)
		{
			ZeroMemory(&item, sizeof(LV_ITEM));
			nItemCount = ListView_GetItemCount(GetDlgItem(hWnd,coordinateWnd[k].nID));
			item.iItem = nItemCount;
			SendMessage(GetDlgItem(hWnd,coordinateWnd[k].nID),LVM_INSERTITEM,0,(LPARAM)&item);
		}
	}
	for(i=0;i<5;i++)
	{
		item.iItem = i;
		item.mask = LVIF_TEXT;
		item.cchTextMax =10;
		item.pszText = text[i];
		item.iSubItem = 0;
		SendMessage(GetDlgItem(hWnd,coordinateWnd[0].nID),LVM_SETITEM,0,(LPARAM)&item);
	}
	//设置ListView的行高为50
	// Create the ImageList
	hSmall = ImageList_Create(1,50,ILC_MASK, 1, 1);
// Add two icons to ImageList
	hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	ImageList_AddIcon(hSmall, hIcon);
	DestroyIcon(hIcon);
// Assign ImageList to List View
	ListView_SetImageList(GetDlgItem(hWnd,IDC_COORL0), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_COORL1), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_COORL2), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_COORL3), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_COORL4), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_COORL5), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_COORL6), hSmall, LVSIL_STATE);
	//2010-6-24
	Coordina=0;
	pDataInCoor = (LPCmdThreadParam)HeapAlloc(GetProcessHeap(),
											HEAP_ZERO_MEMORY,
											sizeof(CmdThreadParam)
											);
	if(pDataInCoor == NULL)
	{
		return 1;
	}
	//end 2010-6-24
	return 0;
}
LRESULT goCommandCoordinateWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	return 0;
}
//鼠标点击过程的函数相应
//当鼠标点击到某一参数项目时刷新屏幕以将对应的项目背景设为蓝色，同时将焦点设到输入文本框中以修改该参数数值
LRESULT goListCoordinateWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
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
	int line;
	menuID    = int(wParam); 
	pnmh = (LPNMHDR)lParam;
	wndCtrl = pnmh->hwndFrom;
	if(menuID==IDC_COORL1||menuID==IDC_COORL2||menuID==IDC_COORL3||menuID==IDC_COORL4||menuID==IDC_COORL5||menuID==IDC_COORL6)
	{		
		if(pnmh->code==NM_CLICK)
		{
			nSelItem =-1;
			nSelItem =  ListView_GetNextItem(wndCtrl,-1,LVNI_SELECTED);
			line=menuID-IDC_COORL0;
			//去除高亮
			for(i=0;i<6;i++)
			ListView_SetItemState(GetDlgItem(hWnd,IDC_COORL1+i),-1,LVIS_SELECTED, 0XFF);
			if (nSelItem >= 0 )
			{
				ListView_SetItemState(wndCtrl,nSelItem,LVIS_DROPHILITED, 0XFF);
				//启动另一个线程用于修改参数数值
				SetFocus(GetDlgItem(GetParent(hWnd),IDC_MDIEDIT));
				pDataInCoor->hWnd = hWnd;
				pDataInCoor->wndCtrl = wndCtrl;
				pDataInCoor->menuID = line;
				pDataInCoor->notifyCode = 0;
				Coordina=1;
			}
		}
	}
	else if(menuID==IDC_COORL0)//如果点击项目名称则将其他高亮的ListView高亮消除
	{		
		if(pnmh->code==NM_CLICK)
		{
			SetFocus(hWnd);
			for(i=0;i<6;i++)
			ListView_SetItemState(GetDlgItem(hWnd,IDC_COORL1+i),-1,LVIS_SELECTED, 0XFF);
		}
	}
	return 0;
}
//重画屏幕时所调用的函数
LRESULT goPaintCoordinateWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//定义画板结构体对象
	PAINTSTRUCT ps;
	HDC hdc;
	hdc=BeginPaint(hWnd,&ps);
	EndPaint(hWnd,&ps);
	return 0;
}
LRESULT goDestroyCoordinateWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	PostQuitMessage(0);
	return 0;
}
//修改参数数值线程
void CoordinatehreadProc()
{
	int i;
	char szBuffer[31];
	int nSelItem;
	nSelItem = -1;
	nSelItem =  ListView_GetNextItem(pDataInCoor->wndCtrl,-1,LVNI_DROPHILITED);
	GetWindowText (GetDlgItem(GetParent(pDataInCoor->hWnd),IDC_MDIEDIT), szBuffer, 30);//得到输入文本框内所输入数值
	fc_changeFile(nSelItem+1,pDataInCoor->menuID,szBuffer,pDataInCoor->hWnd);
	readfc_to_buffer(pDataInCoor->hWnd);
	SetWindowText (GetDlgItem(GetParent(pDataInCoor->hWnd),IDC_MDIEDIT),"");
	ListView_SetItemText(pDataInCoor->wndCtrl,nSelItem,0,szBuffer);              //??
	//将选择项蓝背景颜色去掉
	for(i=0;i<6;i++)ListView_SetItemState(GetDlgItem(pDataInCoor->hWnd,IDC_COORL1+i),-1,LVIS_SELECTED, 0XFF);
	Coordina=0;
}

void fc_countLineNumber(FILE* fp,int& count)
{
	char c;
	rewind(fp);
	while((c=char(fgetc(fp)))!=EOF)
	{
		if(c=='\n')count++;
	}
	count++;
}
void readContentFc(FILE* fp,char* a,int line,HWND hWnd)
{
	int i;
	char b;
	rewind(fp);
	for(i=0;i<line-1;i++)
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
void fc_changeFile(int lineNo,int rowNo,char* inLine,HWND hWnd)
{
	int i,j;
	FILE* temp;
	char c;
	int signIsEnd;

	signIsEnd=0;

	i = lineNo;
	j = rowNo;

	temp=fopen("CoorTemp.txt","w");
	rewind(fpcoor);
	for(i=1;i< (lineNo-1) *6 + rowNo;i++)
	{
		while((c=char(fgetc(fpcoor)))!='\n')fputc(int(c),temp); //找到所在行
		fputc(int(c),temp);
	}

	for(i=0;i<20;i++)
	{
		if(inLine[i]=='\0')break;							//写入替换值
		fputc(int(inLine[i]),temp);
	}
	c='\n';
	fputc(int(c),temp);

	while((c=(char(fgetc(fpcoor))))!='\n')
	{
		if(c==EOF)
		{
			signIsEnd=1;
			break;
		}
	};					//跳过该行
	while((signIsEnd==0)&&((c=(char(fgetc(fpcoor))))!=EOF))fputc(int(c),temp); //写剩余文件
	fclose(temp);
	temp=fopen("CoorTemp.txt","r");
	rewind(temp);
	rewind(fpcoor);
	while((c=(char(fgetc(temp))))!=EOF)fputc(int(c),fpcoor); //写回
	fclose(temp);	
}
void readbuffer_to_fc(HWND hWnd)
{
	char content[100];

	sprintf_s(content, "%12.3f", G54_coordinate.x);
	fc_changeFile(1,1,content,hWnd);
	sprintf_s(content, "%12.3f", G54_coordinate.y);
	fc_changeFile(2,1,content,hWnd);
	sprintf_s(content, "%12.3f",G54_coordinate.z);
	fc_changeFile(3,1,content,hWnd);
	sprintf_s(content, "%12.3f", G54_coordinate.b);
	fc_changeFile(4,1,content,hWnd);
	sprintf_s(content, "%12.3f",G54_coordinate.c);
	fc_changeFile(5,1,content,hWnd);

	sprintf_s(content, "%12.3f", G55_coordinate.x);
	fc_changeFile(1,2,content,hWnd);
	sprintf_s(content, "%12.3f",G55_coordinate.y);
	fc_changeFile(2,2,content,hWnd);
	sprintf_s(content, "%12.3f", G55_coordinate.z);
	fc_changeFile(3,2,content,hWnd);
	sprintf_s(content, "%12.3f", G55_coordinate.b);
	fc_changeFile(4,2,content,hWnd);
	sprintf_s(content, "%12.3f", G55_coordinate.c);
	fc_changeFile(5,2,content,hWnd);

	sprintf_s(content, "%12.3f", G56_coordinate.x);
	fc_changeFile(1,3,content,hWnd);
	sprintf_s(content, "%12.3f", G56_coordinate.y);
	fc_changeFile(2,3,content,hWnd);
	sprintf_s(content, "%12.3f", G56_coordinate.z);
	fc_changeFile(3,3,content,hWnd);
	sprintf_s(content, "%12.3f", G56_coordinate.b);
	fc_changeFile(4,3,content,hWnd);
	sprintf_s(content, "%12.3f", G56_coordinate.c);
	fc_changeFile(5,3,content,hWnd);

	sprintf_s(content, "%12.3f", G57_coordinate.x);
	fc_changeFile(1,4,content,hWnd);
	sprintf_s(content, "%12.3f", G57_coordinate.y);
	fc_changeFile(2,4,content,hWnd);
	sprintf_s(content, "%12.3f", G57_coordinate.z);
	fc_changeFile(3,4,content,hWnd);
	sprintf_s(content, "%12.3f", G57_coordinate.b);
	fc_changeFile(4,4,content,hWnd);
	sprintf_s(content, "%12.3f", G57_coordinate.c);
	fc_changeFile(5,4,content,hWnd);

	sprintf_s(content, "%12.3f", G58_coordinate.x);
	fc_changeFile(1,5,content,hWnd);
	sprintf_s(content, "%12.3f", G58_coordinate.y);
	fc_changeFile(2,5,content,hWnd);
	sprintf_s(content, "%12.3f", G58_coordinate.z);
	fc_changeFile(3,5,content,hWnd);
	sprintf_s(content, "%12.3f", G58_coordinate.b);
	fc_changeFile(4,5,content,hWnd);
	sprintf_s(content, "%12.3f", G58_coordinate.c);
	fc_changeFile(5,5,content,hWnd);

	sprintf_s(content, "%12.3f", G59_coordinate.x);
	fc_changeFile(1,6,content,hWnd);
	sprintf_s(content, "%12.3f", G59_coordinate.y);
	fc_changeFile(2,6,content,hWnd);
	sprintf_s(content, "%12.3f", G59_coordinate.z);
	fc_changeFile(3,6,content,hWnd);
	sprintf_s(content, "%12.3f", G59_coordinate.b);
	fc_changeFile(4,6,content,hWnd);
	sprintf_s(content, "%12.3f", G59_coordinate.c);
	fc_changeFile(5,6,content,hWnd);

}
void readfc_to_buffer(HWND hWnd)
{
		char content[100];

	rewind(fpcoor);
	readContentFc(fpcoor,content,1,hWnd);
	G54_coordinate.x = atof(content);
	readContentFc(fpcoor,content,2,hWnd);
	G55_coordinate.x = atof(content);
	readContentFc(fpcoor,content,3,hWnd);
	G56_coordinate.x = atof(content);
	readContentFc(fpcoor,content,4,hWnd);
	G57_coordinate.x = atof(content);
	readContentFc(fpcoor,content,5,hWnd);
	G58_coordinate.x = atof(content);
	readContentFc(fpcoor,content,6,hWnd);
	G59_coordinate.x = atof(content);

	readContentFc(fpcoor,content,7,hWnd);
	G54_coordinate.y = atof(content);
	readContentFc(fpcoor,content,8,hWnd);
	G55_coordinate.y = atof(content);
	readContentFc(fpcoor,content,9,hWnd);
	G56_coordinate.y = atof(content);
	readContentFc(fpcoor,content,10,hWnd);
	G57_coordinate.y = atof(content);
	readContentFc(fpcoor,content,11,hWnd);
	G58_coordinate.y = atof(content);
	readContentFc(fpcoor,content,12,hWnd);
	G59_coordinate.y = atof(content);

	readContentFc(fpcoor,content,13,hWnd);
	G54_coordinate.z=atof(content);
	readContentFc(fpcoor,content,14,hWnd);
	G55_coordinate.z=atof(content);
	readContentFc(fpcoor,content,15,hWnd);
	G56_coordinate.z=atof(content);
	readContentFc(fpcoor,content,16,hWnd);
	G57_coordinate.z=atof(content);
	readContentFc(fpcoor,content,17,hWnd);
	G58_coordinate.z=atof(content);
	readContentFc(fpcoor,content,18,hWnd);
	G59_coordinate.z=atof(content);

	readContentFc(fpcoor,content,19,hWnd);
	G54_coordinate.b=atof(content);
	readContentFc(fpcoor,content,20,hWnd);
	G55_coordinate.b=atof(content);
	readContentFc(fpcoor,content,21,hWnd);
	G56_coordinate.b=atof(content);
	readContentFc(fpcoor,content,22,hWnd);
	G57_coordinate.b=atof(content);
	readContentFc(fpcoor,content,23,hWnd);
	G58_coordinate.b=atof(content);
	readContentFc(fpcoor,content,24,hWnd);
	G59_coordinate.b=atof(content);

	readContentFc(fpcoor,content,25,hWnd);
	G54_coordinate.c=atof(content);
	readContentFc(fpcoor,content,26,hWnd);
	G55_coordinate.c=atof(content);
	readContentFc(fpcoor,content,27,hWnd);
	G56_coordinate.c=atof(content);
	readContentFc(fpcoor,content,28,hWnd);
	G57_coordinate.c=atof(content);
	readContentFc(fpcoor,content,29,hWnd);
	G58_coordinate.c=atof(content);
	readContentFc(fpcoor,content,30,hWnd);
	G59_coordinate.c=atof(content);
	
}
void fc_upday(HWND hWnd)
{
	int i,k;
	LVITEM item;
	int   nItemCount;
	char content[100];
	int Count=0;
	//fc_countLineNumber(fpcoor,Count);
	CoorCount=7;	
	for(i=1;i<CoorCount;i++)
	{
		SendMessage (GetDlgItem(hWnd,coordinateWnd[i].nID), LVM_DELETEALLITEMS, 0, 0);	
	}

	for(k=0;k<5;k++)
	{
		for(i=1;i<CoorCount;i++)
		{
			ZeroMemory(&item, sizeof(LV_ITEM));
			nItemCount = ListView_GetItemCount(GetDlgItem(hWnd,coordinateWnd[i].nID));
			item.iItem = k;
			item.mask = LVIF_TEXT;
			item.cchTextMax =30;
			item.pszText = (char *)malloc(31);
			if(item.pszText == NULL)
			{
				MessageBox(hWnd,"can not malloc item.pszText goCreateAutoParam functiotn",NULL,NULL);
			}
			SendMessage(GetDlgItem(hWnd,coordinateWnd[i].nID),LVM_INSERTITEM,0,(LPARAM)&item);

			item.iSubItem = 0;
			rewind(fpcoor);
			readContentFc(fpcoor,content,k*6+i,hWnd);//111
			strcpy(item.pszText,content);
			
			SendMessage(GetDlgItem(hWnd,coordinateWnd[i].nID),LVM_SETITEM,0,(LPARAM)&item);
			ListView_SetItemText(GetDlgItem(hWnd,coordinateWnd[i].nID),k,0,content); 
			
		}
	
	}

}