#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;								// ��ǰʵ��

extern int length;//�涨ÿ�����ݶ�д�볤��Ϊ6���ֽڣ����С=�ӹ������ļ����λ��+2
extern int conditionLength;//�趨�ӹ������ļ������ĿΪ9999��
extern FILE* fpTemp;//��λtemp.txt�ļ���ָ��
extern FILE* fpSeek;//��λseek.txt�ļ���ָ��
extern FILE* fpLock;//��λlock.txt�ļ���ָ��
extern int startR;//���ListView���һ�ж�Ӧ�ļӹ������ļ���
extern int NOWSELECTLISTID;//��ǰѡ��ListView��ID
extern int NOWSELECTLISTITEM;
extern int ConditionNumber;//
extern HANDLE ConditionThread;
extern HWND hScreenEdit;//����ҳ�����������Ҫ�ڻ����ڼ�����ʾ������Ҫ�ľ��
extern int HPos;//ˮƽ������λ��
extern int HPosIsCONDB;//�������Ƿ���û���϶�����±��޸� 1�ǣ�0��
extern void setColume(HWND,int,int);//�趨ĳһ������
extern void writeF(FILE*,int);
extern HWND hWndConT;
extern void ConditionThreadProc();
extern bool isCondition;

//������ť��Ӧ�ĺ��� 1.1
char* fn_getString();//��ȡ�����������
void fn_actionAdd(HWND);//���Ӽӹ������ļ�
void fn_actionDele(HWND);//ɾ���ӹ������ļ�
void fn_actionSearch(HWND,int);//�����ӹ������ļ�
void fn_actionLock(HWND);//������������
void setItem(int*,int);//��ȡ��Ӧ�ļӹ������ļ���Ϣ�������Ᵽ����itemIn[0]�У�����λ�ñ����Ӧ�Ĳ�����Ϣ
int getInNumber(HWND);//�ú�������Ϊ�õ���������������Ϣ
int fnSearch(HWND,int);//�ú�������Ϊ�ж�������ļӹ������ļ�����Ƿ����
int getCond(int,int);//��ȡ�ӹ�������Ϣ������lineΪ�ļ����ƣ�rowΪ��Ҫ���ҵ���0��ʾ���⣬1��ʾ��һ��Ժ�����
int seekNext(int);//�ú�������Ϊ�õ���ǰ�ӹ������ļ�����һ���ļ����У�ͨ����ȡseek.txt�ļ��õ�
int seekPre(int);//�ú�������Ϊ�õ���ǰ�ӹ������ļ���ǰһ���ļ����У�ͨ����ȡseek.txt�ļ��õ�
int getLockCondition(int);//�ú�������Ϊ�õ���Ӧ�ļӹ������ļ��Ƿ�Ϊ����

const MessageProc ChildMessage[]=
{
	WM_CREATE,goCreateConditionBWnd,
	WM_PAINT,goPaintConditionBWnd,
	WM_COMMAND,goCommandConditionBWnd,
	WM_KEYDOWN, goKeydownConditionBWnd,
	WM_DESTROY,goDestroyConditionBWnd
};
//�����߸��ؼ����ڵĴ�����Ϣ	
CtlWndStruct controlWnd[]={
	{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BADD,TEXT("ADD"),12,90,250,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_PUSHBUTTON|SS_CENTER},
	{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BDELE,TEXT("DELE"),262,90,250,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP | BS_PUSHBUTTON|SS_CENTER},
	{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BSEARCH,TEXT("SEARCH"),512,90,250,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP | BS_PUSHBUTTON|SS_CENTER},
	{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BLOCK,TEXT("LOCK"),762,90,250,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP | BS_PUSHBUTTON|SS_CENTER},
	{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_TIN,TEXT(""),50,35,974,30,WS_VISIBLE|WS_BORDER|WS_CHILD|ES_UPPERCASE|ES_NUMBER},
	{0,TEXT("STATIC"),IDC_TOUT,TEXT("Edit"),0,35,50,30,WS_VISIBLE | WS_CHILD | SS_CENTER},
};
int MyRegisterClassConditionB(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			=CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)conditionBWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);//
	wcex.lpszMenuName	= NULL;//(LPCTSTR)IDC_EX1;
	wcex.lpszClassName	= TEXT("CONDITIONBWND");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
//ע���Զ�������
	if(!RegisterClassEx(&wcex)) return 0;
	return 1;
}
//�Ӵ��ڹ����壬����Ϊ�ص��������������յ���Ϣ�󣬻��֪Windows CEϵͳִ�ж�Ӧ����Ϣ�������
LRESULT CALLBACK conditionBWndProc(HWND hWnd,UINT msgCode,WPARAM wParam,LPARAM lParam)
{
	int i;
	//���Ҷ�Ӧ����ϢID�ţ���ִ����Ӧ����Ϣ�������
	for(i=0;i<dim(ChildMessage);i++)
	{
		if(msgCode==ChildMessage[i].uCode)
			return (*ChildMessage[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//���ڲ�����Ϣ���ұ��е���Ϣ������Windows CE��ȱʡ���ڹ���
	return DefWindowProc(hWnd,msgCode,wParam,lParam);
}
//�Ӵ����յ�WM_CREATE��Ϣ��Ĵ��������
LRESULT  goCreateConditionBWnd(HWND hWnd,UINT msgCode,WPARAM wParam,LPARAM lParam)
{
	int i;
	//2010-3-30
	//�����Զ��ӹ��������ڲ˵�
	for(i=0;i<dim(controlWnd);i++)
	{
		CreateWindowEx(controlWnd[i].dwExStyle,
			controlWnd[i].szClass,
			controlWnd[i].szTitle,
			controlWnd[i].lStyle,
			controlWnd[i].x,
			controlWnd[i].y,
			controlWnd[i].cx,
			controlWnd[i].cy,
			hWnd,
			(HMENU)controlWnd[i].nID,
			hInst,
			NULL
			);
	}
	return 0;
}
LRESULT goCommandConditionBWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDC_BADD://���ADD��ť
		SetWindowText (GetDlgItem(GetParent(hWnd),IDC_STATICALARM),"");//���alarm����Ϣ
		fn_actionAdd(hWnd);
		break;
	case IDC_BDELE://���DELE��ť
		SetWindowText (GetDlgItem(GetParent(hWnd),IDC_STATICALARM),"");
		fn_actionDele(hWnd);
		break;
	case IDC_BSEARCH://���SEARCH��ť
		SetWindowText (GetDlgItem(GetParent(hWnd),IDC_STATICALARM),"");
		fn_actionSearch(hWnd,-1);
		break;
	case IDC_BLOCK://���LOCK��ť
		SetWindowText (GetDlgItem(GetParent(hWnd),IDC_STATICALARM),"");
		fn_actionLock(hWnd);
		break;
	default:
		break;
	}
	return 0;
}

LRESULT goPaintConditionBWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//���廭��ṹ�����
	PAINTSTRUCT ps;
	HDC hdc;
	hdc=BeginPaint(hWnd,&ps);
	EndPaint(hWnd,&ps);
	return 0;
}

LRESULT goKeydownConditionBWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int menuID, notifyCode;		
	//���崰�ھ��
	HWND wndCtrl;
	menuID    = LOWORD(wParam); 
	notifyCode = HIWORD(wParam); 
	wndCtrl = (HWND) lParam;
	if(isCondition)ConditionThreadProc();
	 return 0;
}
LRESULT goDestroyConditionBWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	PostQuitMessage(0);
	return 0;
}

//��ȡ��Ӧ�ļӹ������ļ���Ϣ�������Ᵽ����itemIn[0]�У�����λ�ñ����Ӧ�Ĳ�����Ϣ
void setItem(int* itemIn,int number)
{
	int i;
	for(i=0;i<12;i++)
	{
		itemIn[i]=getCond(number,i);
	}
}
//��ȡ�ӹ�������Ϣ������lineΪ�ļ����ƣ�rowΪ��Ҫ���ҵ���0��ʾ���⣬1��ʾ��һ��Ժ�����
int getCond(int line,int row)
{
	int i;//��ʱ����
	char temp[10];//��ʱ����
	fseek(fpTemp,long((((conditionLength+1)*length+2)*(row)+(line)*length)*sizeof(char)),SEEK_SET);//��λtemp.txt�ļ�ָ������Ӧ��λ��
	(char)fgetc(fpTemp);//���","��ֱ�Ӷ���
	if(row==0)fgetc(fpTemp);//���Eֱ�Ӷ���
	for(i=0;i<10;i++)
	{
		temp[i]=(char)fgetc(fpTemp);
		if(temp[i]==',')
		{
			temp[i]='\0';
			break;
		}
		else if(temp[i]=='\n'||temp[i]=='\r')
		{
			temp[i]='\0';
			break;
		}
		else if(temp[i]==' ')
		{
			temp[i]='\0';
			break;
		}
	}
	return atoi(temp);
}

//�ú�������Ϊ�õ���ǰ�ӹ������ļ�����һ���ļ����У�ͨ����ȡseek.txt�ļ��õ�
int seekNext(int now)
{
	int i;
	char a[6];
	fseek(fpSeek,long((now*12+6)*sizeof(char)),SEEK_SET);//��λseek.txt�ļ�ָ��
	a[0]=(char)fgetc(fpSeek);
	if(a[0]==' ')return 0;
	for(i=1;i<6;i++)a[i]=(char)fgetc(fpSeek);
	for(i=0;i<6;i++)
	{
		if(a[i]==' ')
		{
			a[i]='\0';
			break;
		}
	}
	return atoi(a);
}

//�ú�������Ϊ�õ���ǰ�ӹ������ļ���ǰһ���ļ����У�ͨ����ȡseek.txt�ļ��õ�
int seekPre(int now)
{
	int i;
	char a[6];
	fseek(fpSeek,long((now*12)*sizeof(char)),SEEK_SET);//��λseek.txt�ļ�ָ��
	a[0]=(char)fgetc(fpSeek);
	if(a[0]==' ')return 0;
	for(i=1;i<6;i++)a[i]=(char)fgetc(fpSeek);
	for(i=0;i<6;i++)
	{
		if(a[i]==' ')
		{
			a[i]='\0';
			break;
		}
	}
	return atoi(a);
}
//�ú�������Ϊ�õ���Ӧ�ļӹ������ļ��Ƿ�Ϊ����
int getLockCondition(int ENO)
{
	int lockCondition;
	fseek(fpLock,long((ENO-1)*sizeof(char)),SEEK_SET);//��λlock.txtָ��
	lockCondition=getc(fpLock);
	if(lockCondition==int( '0'))return 0;
	else if(lockCondition==int('1'))return 1;
	return 0;
}
//SEARCH��ť��Ӧ�ķ�����ͬʱ�ú�����Ϊ�������������ù�����otherENO����
//���otherENO����Ϊ-1��ʾΪSEARCH�����÷���Ϊ��������������
void fn_actionSearch(HWND hWnd,int otherENO)//�����ӹ������ļ�
{
	int i,j;
	int ENO;
	if(otherENO==-1)ENO=getInNumber(hWnd);//�õ��������������ļ�����
	else ENO=otherENO;
	//����NOWSELECTLISTID��NOWSELECTLISTITEM
	NOWSELECTLISTID=0;
	NOWSELECTLISTITEM=0;
	if(ENO!=-1)
	{
		if(fnSearch(hWnd,ENO)==1)//����Ѱ�ļ����ڣ�����ʾ���ļ��������ļ���ʾ�ڵ�һ��
		{
			startR=seekPre(ENO);
			i=ENO;
			j=0;
			while(1)
			{
				if((i=seekPre(i))==0)break;
				j++;
			}
			//�趨ˮƽ������λ��
			HPos=j;
			if(HPos>ConditionNumber-9)
			{
				HPos=ConditionNumber-9;
				startR=seekPre(0);
				for(i=0;i<9;i++)
				{
					startR=seekPre(startR);
					if(startR==0)startR=seekPre(startR);
				}				
			}
			HPosIsCONDB=1;//��ʾ������λ���Ѹı䣬�������ָı䲢����ͨ���϶����������������
			SendMessage(GetDlgItem(GetParent(hWnd),IDW_CONDT),WM_HSCROLL,SB_LINELEFT,NULL);
		}
		else SetWindowText (GetDlgItem(GetParent(hWnd),IDC_STATICALARM),"NO FOUND");//��ʾû�з��ָ��ļ��������κ������Ķ���
	}
	else SetWindowText (GetDlgItem(GetParent(hWnd),IDC_STATICALARM),"NO FOUND");	
}

//�ú�������Ϊ�õ���������������Ϣ
int getInNumber(HWND hWnd)
{
	int i;
	char szBuffer[31];
	GetWindowText (GetDlgItem(hWnd,IDC_TIN), szBuffer, 30);
	SetWindowText (GetDlgItem(hWnd,IDC_TIN),"");
	i=atoi(szBuffer);
	return i;
}
//�ú�������Ϊ�ж�������ļӹ������ļ�����Ƿ����
int fnSearch(HWND hWnd,int ENO)
{
	char a[6];
	fseek(fpSeek,long((ENO*12+6)*sizeof(char)),SEEK_SET);
	a[0]=(char)fgetc(fpSeek);
	if(a[0]==' ') 
	{
		return 0;
	}
	return 1;
}
//��ӦLOCK��ť
void fn_actionLock(HWND hWnd)//������������
{
	int lockCondition;
	int ENO;
	int time;
	int i;
	//NOWSELECTLISIID��¼�ŵ�ǰ�����е��б���λ�á�
	//startR��¼�ŵ�ǰ����ʾ���б����һ�е��ļ����
	//time����Ϊ�˵õ���ǰ�������������ʾ�����һ��֮��Ĳ�ֵ��Ȼ��ͨ��seekPre�����õ���ǰ��������еĲ����ļ����
	if(NOWSELECTLISTID!=0)
	{
		time=9-NOWSELECTLISTID;
		ENO=startR;
		for(i=0;i<time;i++)
		{
			ENO=seekPre(ENO);
			if(ENO==0)ENO=seekPre(ENO);
		}
		//�õ���ѡ�е��ļ����е�����״̬��Ϣ�����û�������ͽ�����Ϊ������������Ϊ��������ͬʱ����״̬�޸�LOCK BUTTON����ʾ���ı�
		lockCondition=getLockCondition(ENO);
		fseek(fpLock,long((ENO-1)*sizeof(char)),SEEK_SET);
		if(lockCondition==0)
		{
			putc(int('1'),fpLock);
			SetWindowText (GetDlgItem(hWnd,IDC_BLOCK),"UNLOCK");
		}
		else
		{
			putc(int('0'),fpLock);
			SetWindowText (GetDlgItem(hWnd,IDC_BLOCK),"LOCK");
		}
		setColume(hWndConT,NOWSELECTLISTID,ENO);
		NOWSELECTLISTID=0;
		NOWSELECTLISTITEM=0;
		SetFocus(hWndConT);
		for(i=-1;i<9;i++)
		ListView_SetItemState(GetDlgItem(hWndConT,IDC_CONTL1+i),-1,LVIS_SELECTED, 0XFF);
	}
}
//���ADD��ťʱ����Ӧ
void fn_actionAdd(HWND hWnd)
{
	int i,j;
	char a[6];
	int ENO;
	int pre,next,now;
	char b;
	ENO=-1;
	ENO=getInNumber(hWnd);//�õ��ı���������Ĳ����ļ����
	if(ENO>=0&&ENO<=9999)
	{
		//���������Ĳ����ļ��Ѿ�������˵���Ѿ�����ͬʱ��ʾ�ò����ļ���ͨ������fn_antionSearch����ʾ
		if(fnSearch(hWnd,ENO)==1)
		{
			SetWindowText (GetDlgItem(GetParent(hWnd),IDC_STATICALARM),"ALREADY EXIST");
			startR=seekPre(ENO);
			HPosIsCONDB=1;
			fn_actionSearch(hWnd,ENO);
		}
		else 
		{
			//����������������ļ����д��temp.txt��ͬʱ�޸�seek.txt�ļ��ڵ���Ϣ������������������
			writeF(fpTemp,ENO);
			i=1;
			while(i<12)
			{
				fseek(fpTemp,long((((conditionLength+1)*length+2)*i+ENO*length)*sizeof(char)),SEEK_SET);
				fputc(int(','),fpTemp);
				fputc(int('0'),fpTemp);
				i++;
			}
			now=0;
			next=seekNext(0);
			pre=seekPre(0);
			//��������ļ�����������ͷ
			if(ENO<next)
			{
				fseek(fpSeek,long((next*12)*sizeof(char)),SEEK_SET);
				for(j=0;j<6;j++)fputc(int(' '),fpSeek);
				fseek(fpSeek,long((next*12)*sizeof(char)),SEEK_SET);
				itoa(ENO,a,10);
				for(i=0;i<6;i++)if(a[i]=='\0')break;
				for(j=0;j<i;j++)
				{
					b=a[j];
					fputc(int(b),fpSeek);
				}
				fseek(fpSeek,long((ENO*12)*sizeof(char)),SEEK_SET);
				for(j=0;j<6;j++)fputc(int(' '),fpSeek);
				fseek(fpSeek,long((ENO*12)*sizeof(char)),SEEK_SET);
				fputc(int('0'),fpSeek);
				fseek(fpSeek,long((ENO*12+6)*sizeof(char)),SEEK_SET);
				for(j=0;j<6;j++)fputc(int(' '),fpSeek);
				fseek(fpSeek,long((ENO*12+6)*sizeof(char)),SEEK_SET);
				itoa(next,a,10);
				for(i=0;i<6;i++)if(a[i]=='\0')break;
				for(j=0;j<i;j++)
				{
					b=a[j];
					fputc(int(b),fpSeek);
				}
				fseek(fpSeek,long((6)*sizeof(char)),SEEK_SET);
				for(j=0;j<6;j++)fputc(int(' '),fpSeek);
				fseek(fpSeek,long((6)*sizeof(char)),SEEK_SET);
				itoa(ENO,a,10);
				for(i=0;i<6;i++)if(a[i]=='\0')break;
				for(j=0;j<i;j++)
				{
					b=a[j];
					fputc(int(b),fpSeek);
				}
			}
			//��������ļ��������β
			if(ENO>pre)
			{
				fseek(fpSeek,long((pre*12+6)*sizeof(char)),SEEK_SET);
				for(j=0;j<6;j++)fputc(int(' '),fpSeek);
				fseek(fpSeek,long((pre*12+6)*sizeof(char)),SEEK_SET);
				itoa(ENO,a,10);
				for(i=0;i<6;i++)if(a[i]=='\0')break;
				for(j=0;j<i;j++)
				{
					b=a[j];
					fputc(int(b),fpSeek);
				}
				fseek(fpSeek,long((ENO*12+6)*sizeof(char)),SEEK_SET);
				for(j=0;j<6;j++)fputc(int(' '),fpSeek);
				fseek(fpSeek,long((ENO*12+6)*sizeof(char)),SEEK_SET);
				fputc(int('0'),fpSeek);
				fseek(fpSeek,long((ENO*12)*sizeof(char)),SEEK_SET);
				for(j=0;j<6;j++)fputc(int(' '),fpSeek);
				fseek(fpSeek,long((ENO*12)*sizeof(char)),SEEK_SET);
				itoa(pre,a,10);
				for(i=0;i<6;i++)if(a[i]=='\0')break;
				for(j=0;j<i;j++)
				{
					b=a[j];
					fputc(int(b),fpSeek);
				}
				fseek(fpSeek,long((0)*sizeof(char)),SEEK_SET);
				for(j=0;j<6;j++)fputc(int(' '),fpSeek);
				fseek(fpSeek,long((0)*sizeof(char)),SEEK_SET);
				itoa(ENO,a,10);
				for(i=0;i<6;i++)if(a[i]=='\0')break;
				for(j=0;j<i;j++)
				{
					b=a[j];
					fputc(int(b),fpSeek);
				}
			}
			//��������ļ��������м�
			if(ENO<pre&&ENO>next)
			{
				while(next<ENO)
				{
					now=next;
					next=seekNext(now);
				}
				//�ı�ǰһ���ļ���next��Ϣ
				fseek(fpSeek,long((now*12+6)*sizeof(char)),SEEK_SET);
				for(j=0;j<6;j++)fputc(int(' '),fpSeek);
				fseek(fpSeek,long((now*12+6)*sizeof(char)),SEEK_SET);
				itoa(ENO,a,10);
				for(i=0;i<6;i++)if(a[i]=='\0')break;
				for(j=0;j<i;j++)
				{
					b=a[j];
					fputc(int(b),fpSeek);
				}
				//�ı䵱ǰ�ļ���next��Ϣ
				fseek(fpSeek,long((ENO*12+6)*sizeof(char)),SEEK_SET);
				for(j=0;j<6;j++)fputc(int(' '),fpSeek);
				fseek(fpSeek,long((ENO*12+6)*sizeof(char)),SEEK_SET);
				itoa(next,a,10);
				for(i=0;i<6;i++)if(a[i]=='\0')break;
				for(j=0;j<i;j++)
				{
					b=a[j];
					fputc(int(b),fpSeek);
				}

				//�ı䵱ǰ�ļ���pre��Ϣ
				fseek(fpSeek,long((ENO*12)*sizeof(char)),SEEK_SET);
				for(j=0;j<6;j++)fputc(int(' '),fpSeek);
				fseek(fpSeek,long((ENO*12)*sizeof(char)),SEEK_SET);
				itoa(now,a,10);
				for(i=0;i<6;i++)if(a[i]=='\0')break;
				for(j=0;j<i;j++)
				{
					b=a[j];
					fputc(int(b),fpSeek);
				}
				//�ı��һ���ļ���pre��Ϣ
				fseek(fpSeek,long((next)*12*sizeof(char)),SEEK_SET);
				for(j=0;j<6;j++)fputc(int(' '),fpSeek);
				fseek(fpSeek,long((next)*12*sizeof(char)),SEEK_SET);
				itoa(ENO,a,10);
				for(i=0;i<6;i++)if(a[i]=='\0')break;
				for(j=0;j<i;j++)
				{
					b=a[j];
					fputc(int(b),fpSeek);
				}
			}
			//����fn_actionSearchˢ����Ļ
			ConditionNumber++;
			fn_actionSearch(hWnd,ENO);
		}
	}
	else
	{
		SetWindowText (GetDlgItem(GetParent(hWnd),IDC_STATICALARM),"Must Be >=0 and <10000");
	}
	SetWindowText (GetDlgItem((hWnd),IDC_TIN),"");
}
//���DELE��ť����Ӧ
void fn_actionDele(HWND hWnd)
{
	int i,j;
	char a[6];
	int ENO;
	int pre,next,now;
	char b;
	int time;
	//�õ���ǰ��ѡ������Ҫɾ�����ļ������
	if(NOWSELECTLISTID!=0)
	{
		time=9-NOWSELECTLISTID;
		ENO=startR;
		for(i=0;i<time;i++)
		{
			ENO=seekPre(ENO);
			if(ENO==0)ENO=seekPre(ENO);
		}
		//ˢ��temp.txt�ļ���Ϣ��������ǰ��Ҫɾ�������λ����Ϊ�ո�
		fseek(fpLock,long((ENO-1)*sizeof(int)),SEEK_SET);
		fputc(int('0'),fpLock);
		i=0;
		while(i<12)
		{
			fseek(fpTemp,long((((conditionLength+1)*length+2)*i+ENO*length)*sizeof(char)),SEEK_SET);
			for(j=0;j<6;j++)fputc(int(' '),fpTemp);
			i++;
		}
		//�޸�seek.txt�ļ���Ϣ
		now=ENO;
		next=seekNext(ENO);
		pre=seekPre(ENO);
		//�޸�ǰһ���ļ���next��Ϣ
		fseek(fpSeek,long((pre*12+6)*sizeof(char)),SEEK_SET);
		for(j=0;j<6;j++)fputc(int(' '),fpSeek);
		fseek(fpSeek,long((pre*12+6)*sizeof(char)),SEEK_SET);
		itoa(next,a,10);
		for(i=0;i<6;i++)if(a[i]=='\0')break;
		for(j=0;j<i;j++)
		{
			b=a[j];
			fputc(int(b),fpSeek);
		}
		//�޸ĺ�һ���ļ���next��Ϣ
		fseek(fpSeek,long((next*12)*sizeof(char)),SEEK_SET);
		for(j=0;j<6;j++)fputc(int(' '),fpSeek);
		fseek(fpSeek,long((next*12)*sizeof(char)),SEEK_SET);
		itoa(pre,a,10);
		for(i=0;i<6;i++)if(a[i]=='\0')break;
		for(j=0;j<i;j++)
		{
			b=a[j];
			fputc(int(b),fpSeek);
		}
		//����ǰ�ļ�λ��ˢΪ�ո�
		fseek(fpSeek,long((ENO*12)*sizeof(char)),SEEK_SET);
		for(i=0;i<12;i++)fputc(int(' '),fpSeek);
		//ˢ����Ļ����һ����ʾ��ɾ�����ļ���ǰһ���ļ�
		ConditionNumber--;
		if(pre==0)
		{
			startR=seekPre(next);
			fn_actionSearch(hWnd,next);
		}
		else
		{
			startR=seekPre(pre);
			fn_actionSearch(hWnd,pre);
		}
	}
}