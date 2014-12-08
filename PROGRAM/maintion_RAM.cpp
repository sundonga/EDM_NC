#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;								// 当前实例
extern HANDLE MaintionInputhThread;
extern int MaintionInput;
extern LPCmdThreadParam  pDataInMaintion;
void DSPMemReadThreadProc();
int upperActivate;
int downActivate;
int RAM_seg[2];
int RAM_ofset[2];
int RAM_length[2];
int RAM_content[16];
int Ram_Flag = 0 ;
void RAM_Dec_Hex(int,char*,int);
void RAM_setLV_title(HWND,int,int,int);
void RAM_setLV_content(HWND,int);
void ncmem_upday(HWND hWnd,DWORD HPIA);
void Add_zero(HWND hWnd,char input[],char output[],int size);
DWORD Str2Int32(char* str);
extern int DspMemRead(HWND hWnd,DWORD *HpiA,DWORD DspMem[],int Length );
const MessageProc maintion_RAMMessages[]={
	    WM_CREATE, goCreateMaintion_RAM,
		WM_COMMAND, goCommandMaintion_RAM,
		WM_PAINT,  goPaintMaintion_RAM,
		WM_NOTIFY, goNotifyMaintion_RAM,
		WM_LBUTTONUP,goButtonMaintion_RAM,
		WM_DESTROY, goDestroyMaintion_RAM
};
CtlWndStruct maintion_RAMMenuBtns[]={
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDW_MAINTION_RAM_LVM_1,TEXT(""),0,40,924,200,WS_VISIBLE|WS_BORDER|WS_CHILD|LVS_REPORT|LVS_ICON|LVS_EDITLABELS},
	{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDW_MAINTION_RAM_LVM_2,TEXT(""),0,320,924,200,WS_VISIBLE|WS_BORDER|WS_CHILD|LVS_REPORT|LVS_ICON|LVS_EDITLABELS}
};
int MyRegisterClassMaintion_RAM(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)maintion_RAMWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	=GetSysColorBrush(COLOR_WINDOW);//(HBRUSH)(COLOR_WINDOW+1);//
	wcex.lpszMenuName	= NULL;//(LPCTSTR)IDC_EX1;
	wcex.lpszClassName	= TEXT("MAINTION_RAM");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
//注册自动窗口类
	if(!RegisterClassEx(&wcex)) return 0;
	return 1;
}

//自动窗口过程体
LRESULT CALLBACK maintion_RAMWndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;	

	//查找对应的消息ID,并执行对应的消息处理过程
	for(i=0; i<dim(maintion_RAMMessages);i++)
	{
		if(msgCode == maintion_RAMMessages[i].uCode) return(*maintion_RAMMessages[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//对不存在的消息，调用缺省窗口过程
	return DefWindowProc(hWnd, msgCode, wParam, lParam);
}

LRESULT goCreateMaintion_RAM(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		
    int i,j;
	LVCOLUMN lvcol;
	char* caption[]={"DSP_MEMORY","0","4","8","C","0","4","8","C"};
//创建自动窗口菜单
	for(i=0;i<dim(maintion_RAMMenuBtns);i++){
		CreateWindowEx(WS_EX_CLIENTEDGE,
			WC_LISTVIEW,
			maintion_RAMMenuBtns[i].szTitle,
			maintion_RAMMenuBtns[i].lStyle,
			maintion_RAMMenuBtns[i].x,
			maintion_RAMMenuBtns[i].y,
			maintion_RAMMenuBtns[i].cx,
			maintion_RAMMenuBtns[i].cy,
			hWnd,
			(HMENU)maintion_RAMMenuBtns[i].nID,
			hInst,
			NULL
			);
	}
	for(i=0;i<2;i++)
	{
		ZeroMemory(&lvcol, sizeof(LV_COLUMN));
		ListView_SetTextBkColor(GetDlgItem(hWnd,maintion_RAMMenuBtns[i].nID),RGB(255,255,255));
		lvcol.iOrder=0;
		lvcol.iSubItem =0;
		lvcol.pszText = caption[0];
		lvcol.cx = 110;
		lvcol.fmt = LVCFMT_CENTER;
		lvcol.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM|LVCF_ORDER;
		lvcol.cchTextMax = 20;
		SendMessage (GetDlgItem(hWnd,maintion_RAMMenuBtns[i].nID), LVM_INSERTCOLUMN, 0, (LPARAM)&lvcol);
		for(j=1;j<9;j++)
		{
			lvcol.iOrder=j;
			lvcol.iSubItem =j;
			lvcol.pszText = caption[j];
			lvcol.cx = 100;
			lvcol.fmt = LVCFMT_CENTER;
			lvcol.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM|LVCF_ORDER;
			lvcol.cchTextMax = 20;
			SendMessage (GetDlgItem(hWnd,maintion_RAMMenuBtns[i].nID), LVM_INSERTCOLUMN, j, (LPARAM)&lvcol);
		}
		ListView_SetExtendedListViewStyle(GetDlgItem(hWnd,maintion_RAMMenuBtns[i].nID),LVS_EX_HEADERDRAGDROP |LVS_EX_FULLROWSELECT );
	}
	upperActivate=downActivate=0;
	RAM_setLV_title(GetDlgItem(hWnd,maintion_RAMMenuBtns[0].nID),0,800,9);
	ncmem_upday(hWnd,0xb0000000);	
	return 0;
}
LRESULT goCommandMaintion_RAM(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	return 0;
}
LRESULT goNotifyMaintion_RAM(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//定义列表窗口ID号
	int menuID;
	//定义列表窗口句柄
	HWND wndCtrl;
	//定义消息信息结构体指针
	LPNMHDR pnmh;
	int nSelItem;
    //定义指向包含消息结构和列表信息结构成员的结构体
	int line;
    //定义指向包含消息结构和列表信息结构成员的结构体	
	menuID    = int(wParam); 
	pnmh = (LPNMHDR)lParam;
	wndCtrl = pnmh->hwndFrom;	
	if((menuID==IDW_MAINTION_RAM_LVM_1)||(menuID==IDW_MAINTION_RAM_LVM_2))
	{
		if(pnmh->code==NM_CLICK)
		{
			Ram_Flag = 1;
			if(menuID==IDW_MAINTION_RAM_LVM_2)
			{
				upperActivate=1;
				downActivate=0;
				InvalidateRect(hWnd,NULL,TRUE);
				UpdateWindow(hWnd);
			}
			else
			{
				upperActivate=0;
				downActivate=1;
				InvalidateRect(hWnd,NULL,TRUE);
				UpdateWindow(hWnd);
			}
			nSelItem =-1;
			nSelItem =  ListView_GetNextItem(wndCtrl,-1,LVNI_SELECTED);
			line=1;
			//去除高亮
			ListView_SetItemState(GetDlgItem(hWnd,IDW_MAINTION_RAM_LVM_1),-1,LVIS_SELECTED, 0XFF);
			ListView_SetItemState(GetDlgItem(hWnd,IDW_MAINTION_RAM_LVM_2),-1,LVIS_SELECTED, 0XFF);
			if (nSelItem >= 0 )
			{
				ListView_SetItemState(wndCtrl,nSelItem,LVIS_DROPHILITED, 0XFF);
				//启动另一个线程用于修改参数数值
				SetFocus(GetDlgItem(GetParent(hWnd),IDC_MAINTION_MAIN_EDIT));
				pDataInMaintion->hWnd = hWnd;
				pDataInMaintion->wndCtrl = wndCtrl;
				pDataInMaintion->menuID = line;
				pDataInMaintion->notifyCode = 0;
				MaintionInput=5;
			}
		}
	}
	
	return 0;
}
void DSPMemReadThreadProc()
{ 
	int i;
	int adr;
	char szBuffer[31];
	int nSelItem;
	nSelItem = -1;
	nSelItem =  ListView_GetNextItem(pDataInMaintion->wndCtrl,-1,LVNI_DROPHILITED);
	GetWindowText (GetDlgItem(GetParent(pDataInMaintion->hWnd),IDC_MAINTION_MAIN_EDIT), szBuffer, 30);//得到输入文本框内所输入数值
	adr = Str2Int32(szBuffer);
	ncmem_upday(pDataInMaintion->hWnd,adr);
	//将选择项蓝背景颜色去掉
	for(i=0;i<2;i++)ListView_SetItemState(GetDlgItem(pDataInMaintion->hWnd,IDW_MAINTION_RAM_LVM_1),-1,LVIS_SELECTED, 0XFF);
	MaintionInput=0;
}
LRESULT goButtonMaintion_RAM(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int y;
	y=HIWORD(lParam);
	if(y<280&&upperActivate==0)
	{
		upperActivate=1;
		downActivate=0;
		InvalidateRect(hWnd,NULL,TRUE);
		UpdateWindow(hWnd);
	}
	else if(y>280&&downActivate==0)
	{
		upperActivate=0;
		downActivate=1;
		InvalidateRect(hWnd,NULL,TRUE);
		UpdateWindow(hWnd);
	}
	SetFocus(GetDlgItem(GetParent(hWnd),IDC_MAINTION_MAIN_EDIT));
	return 0;
}
LRESULT goPaintMaintion_RAM(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HPEN hsysPen,hPen;
	LOGPEN logPen;
	HDC hdc;
	RECT rect;
	char* content;
	hdc = BeginPaint(hWnd, &ps);
	SetBkMode(hdc,TRANSPARENT);	
	logPen.lopnStyle=PS_SOLID;
	logPen.lopnWidth.x=5;
	logPen.lopnWidth.y=5;
	logPen.lopnColor=RGB(0,3,102);
	hPen=CreatePenIndirect(&logPen);
	hsysPen=(HPEN)SelectObject(hdc,hPen);
	Rectangle(hdc,50,5,150,35);
	Rectangle(hdc,50,285,150,315);
	SelectObject(hdc,hsysPen);
	DeleteObject(hPen);
	SetRect(&rect,50,5,150,35);
	if(upperActivate==1)	FillRect(hdc, &rect,GetSysColorBrush(COLOR_HOTLIGHT));
	else FillRect(hdc, &rect,GetSysColorBrush(COLOR_WINDOW));
	content="DSP";
	DrawText(hdc,content,-1,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,50,245,100,275);
	FillRect(hdc, &rect,GetSysColorBrush(COLOR_WINDOW));
	content="SEG =";
	DrawText(hdc,content,-1,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,110,245,210,275);
	FillRect(hdc, &rect,GetSysColorBrush(COLOR_WINDOW));
	content="0000";
	DrawText(hdc,content,-1,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,260,245,360,275);
	FillRect(hdc, &rect,GetSysColorBrush(COLOR_WINDOW));
	content="OFSET =";
	DrawText(hdc,content,-1,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,360,245,460,275);
	FillRect(hdc, &rect,GetSysColorBrush(COLOR_WINDOW));
	content="0000";
	DrawText(hdc,content,-1,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,510,245,610,275);
	FillRect(hdc, &rect,GetSysColorBrush(COLOR_WINDOW));
	content="LENGTH =";
	DrawText(hdc,content,-1,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,610,245,710,275);
	FillRect(hdc, &rect,GetSysColorBrush(COLOR_WINDOW));
	content="00";
	DrawText(hdc,content,-1,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,50,285,150,315);
	if(downActivate==1)	FillRect(hdc, &rect,GetSysColorBrush(COLOR_HOTLIGHT));
	else FillRect(hdc, &rect,GetSysColorBrush(COLOR_WINDOW));
	content="VMC";
	DrawText(hdc,content,-1,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,50,525,100,555);
	FillRect(hdc, &rect,GetSysColorBrush(COLOR_WINDOW));
	content="SEG =";
	DrawText(hdc,content,-1,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,110,525,210,555);
	FillRect(hdc, &rect,GetSysColorBrush(COLOR_WINDOW));
	content="0000";
	DrawText(hdc,content,-1,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,260,525,360,555);
	FillRect(hdc, &rect,GetSysColorBrush(COLOR_WINDOW));
	content="OFSET =";
	DrawText(hdc,content,-1,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,360,525,460,555);
	FillRect(hdc, &rect,GetSysColorBrush(COLOR_WINDOW));
	content="0000";
	DrawText(hdc,content,-1,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,510,525,610,555);
	FillRect(hdc, &rect,GetSysColorBrush(COLOR_WINDOW));
	content="LENGTH =";
	DrawText(hdc,content,-1,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	SetRect(&rect,610,525,710,555);
	FillRect(hdc, &rect,GetSysColorBrush(COLOR_WINDOW));
	content="00";
	DrawText(hdc,content,-1,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	EndPaint(hWnd, &ps);
	return 0;
}
LRESULT goDestroyMaintion_RAM(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		PostQuitMessage(0);
		return 0;
}
void RAM_Dec_Hex(int dec,char* hex,int len)
{
	int i,j;
	char temp[4];
	i=0;
	while(TRUE)
	{
		temp[i]=char(dec%16 + (dec%16< 10 ? '0' : 'A' - 10));
		if(dec/16==0)
		{
			break;	
		}
		else
		{
			dec=dec/16;
		}
		i++;
	}
	if(len==2)
	{
		if(i==0)
		{
			hex[0]='0';
			hex[1]=temp[0];
		}
		else
		{
			hex[0]=temp[1];
			hex[1]=temp[0];
		}
		hex[2]='\0';
	}
	else if(len==4)
	{
		for(j=0;j<3-i;j++)hex[j]='0';
		for(j=3-i;j<4;j++)hex[j]=temp[i--];
		hex[4]='\0';
	}
}
void RAM_setLV_title(HWND hLV,int seg,int ofset,int len)
{
	char inLV[10];
	char hex[5];
	int i,j;
	LVITEM item;
	SendMessage(hLV,LVM_DELETEALLITEMS, 0, 0);
	ZeroMemory(&item, sizeof(LV_ITEM));
	item.mask = LVIF_TEXT;
	item.iSubItem = 0;
	item.pszText = (char *)malloc(10);
	for(i=0;i<len;i++)
	{
		item.iItem = i;
		if(ofset+16>65535)
		{
			seg++;
			ofset-=65536;
		}
		if(seg>65535)seg-=65536;
		ofset=ofset+16;
		RAM_Dec_Hex(seg,hex,4);
		for(j=0;j<4;j++)inLV[j]=hex[j];
		inLV[j]=':';
		RAM_Dec_Hex(ofset,hex,4);
		for(j=0;j<4;j++)inLV[j+5]=hex[j];
		inLV[9]='\0';
		strcpy(item.pszText,inLV);
		SendMessage(hLV,LVM_INSERTITEM,0,(LPARAM)&item);
		SendMessage(hLV,LVM_SETITEM,0,(LPARAM)&item);
	}
}
void RAM_setLV_content(HWND hLV,int line)
{
	char hex[3];
	int i;
	LVITEM item;
	ZeroMemory(&item, sizeof(LV_ITEM));
	item.mask = LVIF_TEXT;
	item.iItem = line;	
	item.pszText = (char *)malloc(3);
	for(i=1;i<17;i++)
	{
		item.iSubItem =i;
		RAM_Dec_Hex(RAM_content[i-1],hex,2);
		strcpy(item.pszText,hex);
		SendMessage(hLV,LVM_SETITEM,0,(LPARAM)&item);
	}
}
void ncmem_upday(HWND hWnd,DWORD HPIA)
{
	int i,k;
	LVITEM item;
	int   nItemCount;
	char str[50];
	char string[50];
	char stringtemp[50];
	DWORD title = 0;
	int Count=0;
	int size=0;

	DWORD *HpiAadr;
	DWORD DSPMem[80];	
	int DSPLength;
	DSPLength = sizeof(DSPMem);
	HpiAadr = (DWORD *) malloc(sizeof(DWORD)); 
	*HpiAadr = HPIA;

	DspMemRead(hWnd,HpiAadr,DSPMem,DSPLength);
	SendMessage (GetDlgItem(hWnd,maintion_RAMMenuBtns[0].nID), LVM_DELETEALLITEMS, 0, 0);	
	
	for(i=0;i<10;i++)
	{
		ZeroMemory(&item, sizeof(LV_ITEM));
		nItemCount = ListView_GetItemCount(GetDlgItem(hWnd,maintion_RAMMenuBtns[0].nID));
		item.iItem = i;
		item.mask = LVIF_TEXT;
		item.cchTextMax =30;
		item.pszText = (char *)malloc(31);
		if(item.pszText == NULL)
		{
			MessageBox(hWnd,"can not malloc item.pszText goCreateAutoParam functiotn",NULL,NULL);
		}
		SendMessage(GetDlgItem(hWnd,maintion_RAMMenuBtns[0].nID),LVM_INSERTITEM,0,(LPARAM)&item);
		
		item.iSubItem = 0;
		title =HPIA+ 32*i;
		ultoa(title,str,16); 
		size = strlen(str);
		Add_zero(hWnd,str,string,size);
		strcpy(item.pszText,string);
		SendMessage(GetDlgItem(hWnd,maintion_RAMMenuBtns[0].nID),LVM_SETITEM,0,(LPARAM)&item);		

		for(k=1;k<9;k++)
		{
			item.iSubItem = k;

			ultoa(DSPMem[i*8+k-1],stringtemp,16); 
			size = strlen(stringtemp);
			Add_zero(hWnd,stringtemp,string,size);
			strcpy(item.pszText,string);
			SendMessage(GetDlgItem(hWnd,maintion_RAMMenuBtns[0].nID),LVM_SETITEM,0,(LPARAM)&item);
			
		}

	}

}
void Add_zero(HWND hWnd,char input[],char output[],int size)
{
	char input_temp[60];
	char output_temp[60];


	size = size *4;
	memcpy(&input_temp,input,size);
	memcpy(&output_temp,input,size);
	switch(size)
	{

	case 32:
		memcpy(output_temp+2,&input_temp,size);
		output_temp[0]='0';
		output_temp[1]='x';
		size=size+8;
		break;
	case 28:
		memcpy(output_temp+3,&input_temp,size);
		output_temp[0]='0';
		output_temp[1]='x';
		output_temp[2]='0';
		size=size+12;
		break;
	case 24:
		memcpy(output_temp+4,&input_temp,size);
		output_temp[0]='0';
		output_temp[1]='x';
		output_temp[2]='0';
		output_temp[3]='0';
		size=size+16;
		break;
	case 20:
		memcpy(output_temp+5,&input_temp,size);
		output_temp[0]='0';
		output_temp[1]='x';
		output_temp[2]='0';
		output_temp[3]='0';
		output_temp[4]='0';
		size=size+20;
		break;
	case 16:
		memcpy(output_temp+6,&input_temp,size);
		output_temp[0]='0';
		output_temp[1]='x';
		output_temp[2]='0';
		output_temp[3]='0';
		output_temp[4]='0';
		output_temp[5]='0';
		size=size+24;
		break;
	case 12:
		memcpy(output_temp+7,&input_temp,size);
		output_temp[0]='0';
		output_temp[1]='x';
		output_temp[2]='0';
		output_temp[3]='0';
		output_temp[4]='0';
		output_temp[5]='0';
		output_temp[6]='0';
		size=size+28;
		break;
	case 8:
		memcpy(output_temp+8,&input_temp,size);
		output_temp[0]='0';
		output_temp[1]='x';
		output_temp[2]='0';
		output_temp[3]='0';
		output_temp[4]='0';
		output_temp[5]='0';
		output_temp[6]='0';
		output_temp[7]='0';
		size=size+32;
		break;
	case 4:
		memcpy(output_temp+9,&input_temp,size);
		output_temp[0]='0';
		output_temp[1]='x';
		output_temp[2]='0';
		output_temp[3]='0';
		output_temp[4]='0';
		output_temp[5]='0';
		output_temp[6]='0';
		output_temp[7]='0';
		output_temp[8]='0';
		size=size+36;
		break;
	default:
		break;
	}
	memcpy(output,&output_temp,size);

}
DWORD Str2Int32(char* str)
{
   if(strstr(str,"0x") || strstr(str,"0X"))
   {
      str += 2;
   }

      DWORD tmp = 0;
      int len = strlen(str);
      for(int i = 0;i < len ;i++)
      {
         int nDecNum;
         switch(str[i])
         {
			case 'a':
			case 'A':
			   nDecNum = 10;
			   break;
			case 'b':
			case 'B':
			   nDecNum = 11;
			   break;
			 case 'c':
			 case 'C':
				nDecNum = 12;
				break;
			case 'd':
			case 'D':
				nDecNum = 13;
				break;
			case 'e':
			case 'E':
				nDecNum = 14;
				break;
			case 'f':
			case 'F':
				nDecNum = 15;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				nDecNum = str[i] - '0';
				break;
			default:
				return 0;

         }
         tmp += nDecNum * pow((double)16,(double)(len-i -1));
     }
     return tmp;

}