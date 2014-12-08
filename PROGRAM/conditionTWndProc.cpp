#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"
extern HINSTANCE hInst;								// ��ǰʵ��

//2010-3-30
extern int length;//�涨ÿ�����ݶ�д�볤��Ϊ6���ֽڣ����С=�ӹ������ļ����λ��+2
extern int conditionLength;//�趨�ӹ������ļ������ĿΪ9999��
extern FILE* fpTemp;//��λtemp.txt�ļ���ָ��
extern FILE* fpSeek;//��λseek.txt�ļ���ָ��
extern FILE* fpLock;//��λlock.txt�ļ���ָ��
int NOWSELECTLISTID;//��¼��ǰ���������λ��
int NOWSELECTLISTITEM;//��¼��ǰ���������λ��
int startR;//��¼��ǰ����ʾ��Ϣ���һ�е��ļ����
int VPos;//��ֱ������λ��
int HPos;//ˮƽ������λ��
extern int ConditionNumber;//��ǰ�����ļ�����
extern HWND hScreenEdit;//����ҳ�����������Ҫ�ڻ����ڼ�����ʾ������Ҫ�ľ��
int HPosIsCONDB;//�������Ƿ���û���϶�����±��޸� 1�ǣ�0��
//2010-4-1
extern void setItem(int*,int);//��ȡ��Ӧ�ļӹ������ļ���Ϣ�������Ᵽ����itemIn[0]�У�����λ�ñ����Ӧ�Ĳ�����Ϣ
extern int seekNext(int);
extern int seekPre(int);
extern int getLockCondition(int);
void conditionUpdate(HWND);//ˢ��ListView����
void ConditionThreadProc();
extern int fnSearch(HWND,int);//�ú�������Ϊ�ж�������ļӹ������ļ�����Ƿ����
void setColume(HWND,int,int);//�趨ĳһ������
HWND hWndConT;
bool isCondition;
LPCmdThreadParam  pDataCondition;
//
//����ȫ����ϢID�����Ӵ�����Ϣ�������ָ����ұ�
const MessageProc tableChildMessage[]={
	WM_CREATE,goCreateConditionTWnd,
		WM_PAINT,goPaintConditionTWnd,
		WM_COMMAND,goCommandConditionTWnd,
		WM_NOTIFY,goListConditionTWnd,
		WM_VSCROLL,goVscrollConditionTWnd,
		WM_HSCROLL,goHscrollConditionTWnd,
		WM_DESTROY,goDestroyConditionTWnd
};	  
//����ؼ����ڵĴ�����Ϣ
//ÿһ��ListView����һ����Ϣ
//�ӹ����������������£�ÿһ��ListView����һ�е���Ϣ��ListView�ڵ�������Ϊһ��ֵ�����϶������������иı��ֻ��ListView�����ݡ���ListView����û��
//��������10��ListView����һ��������
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
//ע���Ӵ�����
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
//ע���Զ�������
	if(!RegisterClassEx(&wcex)) return 0;

	return 1;
}
//�Ӵ��ڹ����壬����Ϊ�ص��������������յ���Ϣ�󣬻��֪Windows CEϵͳִ�ж�Ӧ����Ϣ�������
LRESULT CALLBACK conditionTWndProc(HWND hWnd,UINT msgCode,WPARAM wParam,LPARAM lParam)
{
	int i;
	//���Ҷ�Ӧ����ϢID�ţ���ִ����Ӧ����Ϣ�������
	for(i=0;i<dim(tableChildMessage);i++)
	{
		if(msgCode==tableChildMessage[i].uCode)
			return (*tableChildMessage[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//���ڲ�����Ϣ���ұ��е���Ϣ������Windows CE��ȱʡ���ڹ���
	return DefWindowProc(hWnd,msgCode,wParam,lParam);
}
//�Ӵ����յ�WM_CREATE��Ϣ��Ĵ��������
LRESULT  goCreateConditionTWnd(HWND hWnd,UINT msgCode,WPARAM wParam,LPARAM lParam)
{
	HIMAGELIST hSmall;
	HICON hIcon;
	int i;
	LVCOLUMN lvcol;
	LVITEM item;
	int nItemCount;
	char * caption [] ={"CONTENT"};
	SCROLLINFO si;//����������
	LPCREATESTRUCT lpcs;
	int k;
	//�����Զ��ӹ��������ڲ˵�
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
//������������Ŀ��
	for(k=0;k<10;k++)
	{
		ZeroMemory(&lvcol, sizeof(LV_COLUMN));
		ListView_SetTextBkColor(GetDlgItem(hWnd,controlTWnd[k].nID),RGB(255,255,255));
		lvcol.cx =100;//ListView��100
		lvcol.iSubItem =0;
		lvcol.fmt = LVCFMT_CENTER;
		lvcol.pszText =caption[0];
		lvcol.cchTextMax = 60;
		lvcol.mask =LVCF_SUBITEM|LVCF_WIDTH|LVCF_TEXT|LVCF_FMT;
		//����ListView���ض����ֻ�����0�еĻ���ôĬ�Ͼ�������룬Ϊ��ʵ�־��ж�����Ҫ�ֱ����0,1��Ȼ��0��ɾ��
		SendMessage (GetDlgItem(hWnd,controlTWnd[k].nID), LVM_INSERTCOLUMN,0,(LPARAM)&lvcol);
		SendMessage (GetDlgItem(hWnd,controlTWnd[k].nID), LVM_INSERTCOLUMN,1,(LPARAM)&lvcol);
		SendMessage (GetDlgItem(hWnd,controlTWnd[k].nID),LVM_DELETECOLUMN ,0,0);	
		//end
		ListView_SetExtendedListViewStyle(GetDlgItem(hWnd,controlTWnd[k].nID),LVS_EX_FULLROWSELECT );
		SendMessage (GetDlgItem(hWnd,controlTWnd[k].nID), LVM_DELETEALLITEMS, 0, 0);
		//������
		for(i=0;i<9;i++)
		{
			ZeroMemory(&item, sizeof(LV_ITEM));
			nItemCount = ListView_GetItemCount(GetDlgItem(hWnd,controlTWnd[k].nID));
			item.iItem = nItemCount;
			SendMessage(GetDlgItem(hWnd,controlTWnd[k].nID),LVM_INSERTITEM,0,(LPARAM)&item);
		}
	}
	//���ù�����
	lpcs=(LPCREATESTRUCT)lParam;
	si.cbSize=sizeof(si);
	si.nMin=0;
//	�趨ˮƽ������
	si.nMax=lpcs->cx+(ConditionNumber-9)*100;//(ConditionNumber-9)*100Ϊ����������������
	//ͨ����ѯ�õ�lpcs->cx=1024������Ҫ�޸Ĺ�����ʱ���趨si.nMaxʱ��lpcs->cx��1024����
	si.nPage=lpcs->cx;
	si.nPos=0;
	si.fMask=SIF_ALL;
	SetScrollInfo(hWnd,SB_HORZ,&si,TRUE);
	//�趨��ֵ������
	si.nMax=lpcs->cy+100;//162Ϊ50*3�õ�
	si.nPage=lpcs->cy;
	si.nPos=0;
	SetScrollInfo(hWnd,SB_VERT,&si,TRUE);
	if(fnSearch(hWnd,startR)!=1)startR=0;
	NOWSELECTLISTID=0;
	NOWSELECTLISTITEM=0;
	VPos=0;
	HPos=0;
	HPosIsCONDB=0;
	//����ListView���и�Ϊ50
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
	//ˢ��ListView
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
//��������̵ĺ�����Ӧ
//���������ĳһ������Ŀʱˢ����Ļ�Խ���Ӧ����Ŀ������Ϊ��ɫ��ͬʱ�������赽�����ı��������޸ĸò�����ֵ
LRESULT goListConditionTWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//�����б���ID��
	int menuID;
	//�����б��ھ��
	HWND wndCtrl;
	//������Ϣ��Ϣ�ṹ��ָ��
	LPNMHDR pnmh;
    //����ָ�������Ϣ�ṹ���б���Ϣ�ṹ��Ա�Ľṹ��
	int nSelItem;
	int i;
    //����ָ�������Ϣ�ṹ���б���Ϣ�ṹ��Ա�Ľṹ��
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
			line=menuID-IDC_CONTL0;//��¼��ǰ��ѡ����ListView���
			row=nSelItem+1;//��¼��ǰ����¼�������,����Ļ����ʾ�������
			//ȥ������
			for(i=0;i<9;i++)
			ListView_SetItemState(GetDlgItem(hWnd,IDC_CONTL1+i),-1,LVIS_SELECTED, 0XFF);
			if (nSelItem >= 0 )
			{
				if(line==NOWSELECTLISTID&&row==NOWSELECTLISTITEM)return 0; ///??????????????????????????ΪʲôҪ�˳�
				NOWSELECTLISTID=line;
				NOWSELECTLISTITEM=row;
				//�õ���ǰ��������ж�Ӧ�Ĳ����ļ����
				time=9-NOWSELECTLISTID;
				ENO=startR;
				for(i=0;i<time;i++)
				{
					ENO=seekPre(ENO);
					if(ENO==0)ENO=seekPre(ENO);
				}
				ListView_SetItemState(wndCtrl,nSelItem,LVIS_DROPHILITED, 0XFF);
				//�õ���ǰ��ѡ������ļ��Ƿ�Ϊ��������������޸�LOCK BUTTON������ı�����
				lockCondition=getLockCondition(ENO);
				if(lockCondition==0)SetWindowText (GetDlgItem(GetDlgItem(GetParent(hWnd),IDW_CONDB),IDC_BLOCK),"LOCK");
				else SetWindowText (GetDlgItem(GetDlgItem(GetParent(hWnd),IDW_CONDB),IDC_BLOCK),"UNLOCK");
				//������һ���߳������޸Ĳ�����ֵ
				SetFocus(GetDlgItem(GetDlgItem(GetParent(hWnd),IDW_CONDB),IDC_TIN));
				pDataCondition->hWnd = hWnd;
				pDataCondition->wndCtrl = wndCtrl;
				pDataCondition->menuID = menuID;
				pDataCondition->notifyCode = 0;
				isCondition=TRUE;
			}
		}
	}
	else if(menuID==IDC_CONTL0)//��������Ŀ����������������ListView��������
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
//�ػ���Ļʱ�����õĺ���
LRESULT goPaintConditionTWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//���廭��ṹ�����
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
//�϶���ֱ������ʱ�ĺ�����Ӧ
LRESULT goVscrollConditionTWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;
	int oldPos;
	SCROLLINFO si;
	oldPos=VPos;
	switch(LOWORD(wParam))
	{
	case SB_BOTTOM://�������ƶ����ײ�
		VPos=2;
		break;
	case SB_TOP://����
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
		//�������
		SetFocus(hWnd);
		for(i=-1;i<9;i++)
		ListView_SetItemState(GetDlgItem(hWnd,IDC_CONTL1+i),-1,LVIS_SELECTED, 0XFF);
	}
	return 0;
}

//�϶�ˮƽ������ʱ�ĺ�����Ӧ
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
	//��������С��Ҫ�ı�
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
	//��������С����Ҫ�ı�
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
//�޸Ĳ�����ֵ�߳�
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
	GetWindowText (GetDlgItem(GetDlgItem(GetParent(pDataCondition->hWnd),IDW_CONDB),IDC_TIN), szBuffer, 30);//�õ������ı�������������ֵ
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
			//�����Ҫ�޸ĵĲ�����������ʾ�ò����ѱ�����
			if(getLockCondition(ENO)==1)
			{
				SetWindowText (GetDlgItem(GetParent(pDataCondition->hWnd),IDC_STATICALARM),"THIS ITEM IS LOCKED");
			}
			else
			{
				fseek(fpTemp,long((((conditionLength+1)*length+2)*(NOWSELECTLISTITEM+VPos)+(ENO)*length+1)*sizeof(char)),SEEK_SET);//��λfpTempָ��
				for(i=0;i<5;i++)fputc(int(' '),fpTemp);
				fseek(fpTemp,long((((conditionLength+1)*length+2)*(NOWSELECTLISTITEM+VPos)+(ENO)*length+1)*sizeof(char)),SEEK_SET);//��λfpTempָ��
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
	//��ѡ������������ɫȥ��
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
	//���ñ�����
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
	//���ò�����
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
//�ú�������Ϊ����ĳһ��
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
		//�����Ӧ����Ϊ�������䱳����ʾΪCOLOR_SCROLLBAR��������ʾΪCOLOR_WINDOW
		if(getLockCondition(ENO)==1)SendMessage(GetDlgItem(hWnd,controlTWnd[columeNo].nID),LVM_SETTEXTBKCOLOR,0,GetSysColor(COLOR_SCROLLBAR));
		else if(getLockCondition(ENO)!=1)SendMessage(GetDlgItem(hWnd,controlTWnd[columeNo].nID),LVM_SETTEXTBKCOLOR,0,GetSysColor(COLOR_WINDOW));
	}
}