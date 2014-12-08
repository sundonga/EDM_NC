#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

//�ӹ�����i/o, ����ĵ�ַ
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

extern HINSTANCE hInst;								// ��ǰʵ��
extern int AutoInput;
extern LPCmdThreadParam  pDataInAutoInput;
//start//2010-4-5
//����������������ӹ�����ʱ��Ҫʹ�ã�������ʱ����Ҫʹ��
int NOWSELECTLINE;//2010-4-5 ��ǵ�ǰѡ����ListView
int NOWSELECTROW;//2010-4-5 ��ǵ�ǰѡ����ListView��
int VPosAuto;//2010-4-5 ��ǵ�ǰ������λ��
int NOWITEMNUMBER;//��ǵ�ǰ��Ŀ��������Ӧ�ù�����ʯ��Ҫʹ��
HWND hAUTOLV;//��ǰ���Ӵ��ڵľ��
void fn_auto_setLV(HWND);//����ӹ�����ʱ�ú��������ڹ�����λ�øı�ʱ�����޸Ķ�Ӧ��Listview��ʾ����
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
//ע���Զ�������
	if(!RegisterClassEx(&wcex)) return 0;
	icex.dwICC=ICC_LISTVIEW_CLASSES;//??????
	icex.dwSize=sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&icex);
	return 1;
}
//�Զ��ӹ��������ڹ�����
LRESULT CALLBACK autoParamWndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;

	//���Ҷ�Ӧ����ϢID,��ִ�ж�Ӧ����Ϣ�������
	for(i=0; i<dim(autoParamMessages);i++)
	{
		if(msgCode == autoParamMessages[i].uCode) return(*autoParamMessages[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//�Բ����ڵ���Ϣ������ȱʡ���ڹ���
	return DefWindowProc(hWnd, msgCode, wParam, lParam);
}

//start 2010-4-5
LRESULT goCreateAutoParam(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{		
    int i,j;
	LVCOLUMN lvcol;
    char * caption [] ={"Name","value"};
	HIMAGELIST hSmall;//��չListView�и���Ҫ�õ��Ĳ���
	HICON hIcon;//��չListView�и���Ҫ�õ��Ĳ���
	SCROLLINFO si;//����������
	LPCREATESTRUCT lpcs;
	//���ù�����
	lpcs=(LPCREATESTRUCT)lParam;
	si.cbSize=sizeof(si);
	si.nMin=0;
	si.nMax=lpcs->cy+0;//lpcs->cx=1024
	si.nPage=lpcs->cy;
	si.nPos=0;
	si.fMask=SIF_ALL;
	SetScrollInfo(hWnd,SB_VERT,&si,TRUE);
	//end ���ù�����

	//��ʼ��NOWSELECTLINE��NOWSELECTROW��VPosAuto��hAUTOLV
	NOWSELECTLINE=0;
	NOWSELECTROW=0;
	VPosAuto=0;	
	NOWITEMNUMBER=11;
	hAUTOLV=hWnd;
	//end ��ʼ��
	//�����Զ��ӹ��������ڲ˵�
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
//�����Զ��ӹ���������Ŀ��
//ֻ����ListView�ı���
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
//��չListView�õ��Ĵ��룬�����и���ImageList_Create(1,30,ILC_MASK, 1, 1);�еڶ�������30����
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
//end ��չListView�и�
	fn_auto_setLV(hWnd);//��fn_auto_setLV�������ListView����
	return 0;

}
//end 2010-4-5
LRESULT goCommandAutoParam(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//����˵�ID�ź�֪ͨ��
	int menuID, notifyCode;
	
	//���崰�ھ��
	HWND wndCtrl;
	menuID    = LOWORD(wParam); 
	notifyCode = HIWORD(wParam); 
	wndCtrl = (HWND) lParam;
	return 0;
}
LRESULT goListAutoParam(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//�����б���ID��
	int menuID;
	//�����б��ھ��
	HWND wndCtrl;
	//������Ϣ��Ϣ�ṹ��ָ��
	LPNMHDR pnmh;
    //����ָ�������Ϣ�ṹ���б���Ϣ�ṹ��Ա�Ľṹ��
	int nSelItem;
	menuID    = int(wParam); 
	pnmh = (LPNMHDR)lParam;
	wndCtrl = pnmh->hwndFrom;
	if(menuID==IDC_AUTOPARAMLIST2||menuID==IDC_AUTOPARAMLIST4)//ֻ��Ӧ��2��4�з�������Ϣ
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
	else if(menuID==IDC_AUTOPARAMLIST1||menuID==IDC_AUTOPARAMLIST3)//��1��3�з�������Ϣֻ�ǲ���Ӧ��
	{
		if(pnmh->code==NM_CLICK)
		{
			SetFocus(hWnd);//������ת�Ƶ���ǰֻ���ڣ��������������ͻὫ��ǰ����ListView�ĸ����������
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
		//iowrite(hWnd,Machiningparam.item[nSelItem].hDevice,Machiningparam.item[nSelItem].ctrWord, Machiningparam.item[nSelItem].addr,  //���ӹ�����д���豸
		//	Machiningparam.item[nSelItem].value, 1);
	}
	nSelItem =  ListView_GetNextItem(GetDlgItem(pDataInAutoInput->hWnd,IDC_AUTOPARAMLIST4),-1,LVNI_DROPHILITED);
	if (nSelItem >= 0 ) 
	{
		GetWindowText (GetDlgItem(GetParent(pDataInAutoInput->hWnd),IDC_AUTOPARAMEDIT),szBuffer, 30);
		ListView_SetItemText(GetDlgItem(pDataInAutoInput->hWnd,IDC_AUTOPARAMLIST4),nSelItem,0,szBuffer);
		SetWindowText (GetDlgItem(GetParent(pDataInAutoInput->hWnd),IDC_AUTOPARAMEDIT),"");
		Machiningparam.item[10+nSelItem].value = atoi(szBuffer);
		//iowrite(hWnd,Machiningparam.item[10+nSelItem].hDevice,Machiningparam.item[10+nSelItem].ctrWord, Machiningparam.item[10+nSelItem].addr,  //���ӹ�����д���豸
		//	Machiningparam.item[10+nSelItem].value, 1);
	}	
	//��ѡ������������ɫȥ��
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
	// TODO: �ڴ���������ͼ����...
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
	//�������ListView����
	for(j=0;j<4;j++)
	{
		SendMessage (GetDlgItem(hWnd,autoParamMenuBtns[j].nID), LVM_DELETEALLITEMS, 0, 0);
	}	
	//end�������ListView����
//�����Զ��ӹ�����������Ŀ�Ͷ�Ӧ����	
	for(j=0;j<2;j++)
	{
		for(i=0;i<8;i++)
		{
			if(j*8+i==NOWITEMNUMBER)break;//���õ�������Ŀ�����ϾͲ������
			ZeroMemory(&item, sizeof(LV_ITEM));
			//���1��3��
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
			//���2��4��
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
			//iowrite(hWnd,Machiningparam.item[10*j+i].hDevice,Machiningparam.item[10*j+i].ctrWord, Machiningparam.item[10*j+i].addr,  //���ӹ�����д���豸
			//	Machiningparam.item[10*j+i].value, 1);
		}
	}			 
	if(item.pszText!=NULL) free(item.pszText);
}