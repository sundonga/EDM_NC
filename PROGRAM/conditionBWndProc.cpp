#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;								// 当前实例

extern int length;//规定每个数据段写入长度为6个字节，其大小=加工参数文件最大位数+2
extern int conditionLength;//设定加工参数文件最大数目为9999个
extern FILE* fpTemp;//定位temp.txt文件的指针
extern FILE* fpSeek;//定位seek.txt文件的指针
extern FILE* fpLock;//定位lock.txt文件的指针
extern int startR;//标记ListView最后一列对应的加工参数文件号
extern int NOWSELECTLISTID;//当前选中ListView的ID
extern int NOWSELECTLISTITEM;
extern int ConditionNumber;//
extern HANDLE ConditionThread;
extern HWND hScreenEdit;//启动页面句柄，如果需要在画面内加入提示语句就需要改句柄
extern int HPos;//水平滚动条位置
extern int HPosIsCONDB;//滚动条是否在没有拖动情况下被修改 1是，0否
extern void setColume(HWND,int,int);//设定某一列内容
extern void writeF(FILE*,int);
extern HWND hWndConT;
extern void ConditionThreadProc();
extern bool isCondition;

//各个按钮对应的函数 1.1
char* fn_getString();//读取输入框内数据
void fn_actionAdd(HWND);//增加加工参数文件
void fn_actionDele(HWND);//删除加工参数文件
void fn_actionSearch(HWND,int);//搜索加工参数文件
void fn_actionLock(HWND);//锁定解锁参数
void setItem(int*,int);//读取对应的加工参数文件信息，将标题保存在itemIn[0]中，其余位置保存对应的参数信息
int getInNumber(HWND);//该函数功能为得到输入框内输入的信息
int fnSearch(HWND,int);//该函数功能为判断所输入的加工参数文件序号是否存在
int getCond(int,int);//读取加工参数信息，其中line为文件名称，row为所要查找的象，0表示标题，1表示第一项，以后类推
int seekNext(int);//该函数功能为得到当前加工参数文件的下一个文件序列，通过读取seek.txt文件得到
int seekPre(int);//该函数功能为得到当前加工参数文件的前一个文件序列，通过读取seek.txt文件得到
int getLockCondition(int);//该函数功能为得到对应的加工参数文件是否为锁定

const MessageProc ChildMessage[]=
{
	WM_CREATE,goCreateConditionBWnd,
	WM_PAINT,goPaintConditionBWnd,
	WM_COMMAND,goCommandConditionBWnd,
	WM_KEYDOWN, goKeydownConditionBWnd,
	WM_DESTROY,goDestroyConditionBWnd
};
//定义七个控件窗口的创建信息	
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
//注册自动窗口类
	if(!RegisterClassEx(&wcex)) return 0;
	return 1;
}
//子窗口过程体，必须为回调函数，当窗口收到消息后，会告知Windows CE系统执行对应的消息处理过程
LRESULT CALLBACK conditionBWndProc(HWND hWnd,UINT msgCode,WPARAM wParam,LPARAM lParam)
{
	int i;
	//查找对应的消息ID号，并执行相应的消息处理过程
	for(i=0;i<dim(ChildMessage);i++)
	{
		if(msgCode==ChildMessage[i].uCode)
			return (*ChildMessage[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//对于不再消息查找表中的消息，调用Windows CE的缺省窗口过程
	return DefWindowProc(hWnd,msgCode,wParam,lParam);
}
//子窗口收到WM_CREATE消息后的处理过程体
LRESULT  goCreateConditionBWnd(HWND hWnd,UINT msgCode,WPARAM wParam,LPARAM lParam)
{
	int i;
	//2010-3-30
	//创建自动加工参数窗口菜单
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
	case IDC_BADD://点击ADD按钮
		SetWindowText (GetDlgItem(GetParent(hWnd),IDC_STATICALARM),"");//清空alarm内信息
		fn_actionAdd(hWnd);
		break;
	case IDC_BDELE://点击DELE按钮
		SetWindowText (GetDlgItem(GetParent(hWnd),IDC_STATICALARM),"");
		fn_actionDele(hWnd);
		break;
	case IDC_BSEARCH://电机SEARCH按钮
		SetWindowText (GetDlgItem(GetParent(hWnd),IDC_STATICALARM),"");
		fn_actionSearch(hWnd,-1);
		break;
	case IDC_BLOCK://电机LOCK按钮
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
	//定义画板结构体对象
	PAINTSTRUCT ps;
	HDC hdc;
	hdc=BeginPaint(hWnd,&ps);
	EndPaint(hWnd,&ps);
	return 0;
}

LRESULT goKeydownConditionBWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int menuID, notifyCode;		
	//定义窗口句柄
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

//读取对应的加工参数文件信息，将标题保存在itemIn[0]中，其余位置保存对应的参数信息
void setItem(int* itemIn,int number)
{
	int i;
	for(i=0;i<12;i++)
	{
		itemIn[i]=getCond(number,i);
	}
}
//读取加工参数信息，其中line为文件名称，row为所要查找的象，0表示标题，1表示第一项，以后类推
int getCond(int line,int row)
{
	int i;//临时变量
	char temp[10];//临时变量
	fseek(fpTemp,long((((conditionLength+1)*length+2)*(row)+(line)*length)*sizeof(char)),SEEK_SET);//定位temp.txt文件指针至对应的位置
	(char)fgetc(fpTemp);//获得","，直接丢弃
	if(row==0)fgetc(fpTemp);//获得E直接丢弃
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

//该函数功能为得到当前加工参数文件的下一个文件序列，通过读取seek.txt文件得到
int seekNext(int now)
{
	int i;
	char a[6];
	fseek(fpSeek,long((now*12+6)*sizeof(char)),SEEK_SET);//定位seek.txt文件指针
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

//该函数功能为得到当前加工参数文件的前一个文件序列，通过读取seek.txt文件得到
int seekPre(int now)
{
	int i;
	char a[6];
	fseek(fpSeek,long((now*12)*sizeof(char)),SEEK_SET);//定位seek.txt文件指针
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
//该函数功能为得到对应的加工参数文件是否为锁定
int getLockCondition(int ENO)
{
	int lockCondition;
	fseek(fpLock,long((ENO-1)*sizeof(char)),SEEK_SET);//定位lock.txt指针
	lockCondition=getc(fpLock);
	if(lockCondition==int( '0'))return 0;
	else if(lockCondition==int('1'))return 1;
	return 0;
}
//SEARCH按钮对应的方法，同时该函数还为其他函数所引用故增加otherENO参数
//如果otherENO参数为-1表示为SEARCH所引用否则为其他函数所引用
void fn_actionSearch(HWND hWnd,int otherENO)//搜索加工参数文件
{
	int i,j;
	int ENO;
	if(otherENO==-1)ENO=getInNumber(hWnd);//得到输入框内输入的文件名称
	else ENO=otherENO;
	//重置NOWSELECTLISTID、NOWSELECTLISTITEM
	NOWSELECTLISTID=0;
	NOWSELECTLISTITEM=0;
	if(ENO!=-1)
	{
		if(fnSearch(hWnd,ENO)==1)//所搜寻文件存在，则显示该文件并将该文件显示在第一列
		{
			startR=seekPre(ENO);
			i=ENO;
			j=0;
			while(1)
			{
				if((i=seekPre(i))==0)break;
				j++;
			}
			//设定水平滚动条位置
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
			HPosIsCONDB=1;//表示滚动条位置已改变，并且这种改变并不是通过拖动滚动条操作引起的
			SendMessage(GetDlgItem(GetParent(hWnd),IDW_CONDT),WM_HSCROLL,SB_LINELEFT,NULL);
		}
		else SetWindowText (GetDlgItem(GetParent(hWnd),IDC_STATICALARM),"NO FOUND");//显示没有发现该文件，不做任何其他的动作
	}
	else SetWindowText (GetDlgItem(GetParent(hWnd),IDC_STATICALARM),"NO FOUND");	
}

//该函数功能为得到输入框内输入的信息
int getInNumber(HWND hWnd)
{
	int i;
	char szBuffer[31];
	GetWindowText (GetDlgItem(hWnd,IDC_TIN), szBuffer, 30);
	SetWindowText (GetDlgItem(hWnd,IDC_TIN),"");
	i=atoi(szBuffer);
	return i;
}
//该函数功能为判断所输入的加工参数文件序号是否存在
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
//相应LOCK按钮
void fn_actionLock(HWND hWnd)//锁定解锁参数
{
	int lockCondition;
	int ENO;
	int time;
	int i;
	//NOWSELECTLISIID记录着当前鼠标点中的列表列位置。
	//startR记录着当前所显示的列表最后一列的文件序号
	//time变量为了得到当前所点击的列于显示的最后一列之间的差值，然后通过seekPre函数得到当前所点击的列的参数文件序号
	if(NOWSELECTLISTID!=0)
	{
		time=9-NOWSELECTLISTID;
		ENO=startR;
		for(i=0;i<time;i++)
		{
			ENO=seekPre(ENO);
			if(ENO==0)ENO=seekPre(ENO);
		}
		//得到所选中的文件序列的锁定状态信息，如果没有锁定就将其设为锁定，否则设为非锁定，同时根据状态修改LOCK BUTTON所显示的文本
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
//点击ADD按钮时的响应
void fn_actionAdd(HWND hWnd)
{
	int i,j;
	char a[6];
	int ENO;
	int pre,next,now;
	char b;
	ENO=-1;
	ENO=getInNumber(hWnd);//得到文本框内输入的参数文件序号
	if(ENO>=0&&ENO<=9999)
	{
		//如果所输入的参数文件已经存在则说明已经存在同时显示该参数文件。通过调用fn_antionSearch来显示
		if(fnSearch(hWnd,ENO)==1)
		{
			SetWindowText (GetDlgItem(GetParent(hWnd),IDC_STATICALARM),"ALREADY EXIST");
			startR=seekPre(ENO);
			HPosIsCONDB=1;
			fn_actionSearch(hWnd,ENO);
		}
		else 
		{
			//不存在则将所输入的文件序号写入temp.txt中同时修改seek.txt文件内的信息即进行链表插入操作。
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
			//所输入的文件序号在链表表头
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
			//所输入的文件在链表表尾
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
			//所输入的文件在链表中间
			if(ENO<pre&&ENO>next)
			{
				while(next<ENO)
				{
					now=next;
					next=seekNext(now);
				}
				//改变前一个文件的next信息
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
				//改变当前文件的next信息
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

				//改变当前文件的pre信息
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
				//改变后一个文件的pre信息
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
			//调用fn_actionSearch刷新屏幕
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
//点击DELE按钮的响应
void fn_actionDele(HWND hWnd)
{
	int i,j;
	char a[6];
	int ENO;
	int pre,next,now;
	char b;
	int time;
	//得到当前所选定的需要删除的文件的序号
	if(NOWSELECTLISTID!=0)
	{
		time=9-NOWSELECTLISTID;
		ENO=startR;
		for(i=0;i<time;i++)
		{
			ENO=seekPre(ENO);
			if(ENO==0)ENO=seekPre(ENO);
		}
		//刷新temp.txt文件信息，即将当前所要删除的序号位置设为空格
		fseek(fpLock,long((ENO-1)*sizeof(int)),SEEK_SET);
		fputc(int('0'),fpLock);
		i=0;
		while(i<12)
		{
			fseek(fpTemp,long((((conditionLength+1)*length+2)*i+ENO*length)*sizeof(char)),SEEK_SET);
			for(j=0;j<6;j++)fputc(int(' '),fpTemp);
			i++;
		}
		//修改seek.txt文件信息
		now=ENO;
		next=seekNext(ENO);
		pre=seekPre(ENO);
		//修改前一个文件的next信息
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
		//修改后一个文件的next信息
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
		//将当前文件位置刷为空格
		fseek(fpSeek,long((ENO*12)*sizeof(char)),SEEK_SET);
		for(i=0;i<12;i++)fputc(int(' '),fpSeek);
		//刷新屏幕。第一列显示所删除的文件的前一个文件
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