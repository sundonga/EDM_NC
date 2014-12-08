#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

//int IinitProgram(HWND hWnd);  //�������س�����Ӳ���ĳ�ʼ��

int DspProgramLoad(HWND hWnd);     //DSP��������
int DspProgramLoad1(HWND hWnd);     //DSP��������

extern HANDLE IOPCIhDevice;               // IO_PCI�豸��� 
extern HANDLE NCPCIhDevice;                // NC_PCI�豸���
extern HANDLE PowerPCIhDevice;                // NC_PCI�豸��� 
extern HANDLE hDecodeEvent;  //��������λ���������������߳��¼��ľ�� 

extern HINSTANCE hInst;								// ��ǰʵ��
extern TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
extern TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

extern Coordata data_m;  //Machine coor//?????
extern Coordata data_w;   //work coor
extern Coordata data_r;   //ralitive coor
extern Coorpuls pulse_m; //Machine pulse
extern Coorpuls pulse_w; //work pulse
extern ConditionStru Machiningparam;
extern char AutoMachineFileName[_MAX_FNAME];

//2010-4-1 lsw
extern FILE* fpTemp;//��λtemp.txt�ļ���ָ��
extern FILE* fpSeek;//��λseek.txt�ļ���ָ��
extern FILE* fpLock;//��λlock.txt�ļ���ָ��
extern FILE* fpSet;
extern FILE* fpMac;
extern FILE* fpSys;
extern FILE* fpEIO;
extern FILE* fpEIO_Address;
extern FILE* fpcoor;
extern int seekPre(int);
extern int startR;
extern int Mill_ESPARK;//2010-5-25

HWND hWnd1;

HANDLE ScreenThread;
DWORD WINAPI ScreenThreadProc(LPVOID lpParam);


extern HWND hScreenEdit;

extern int isEndScreen;
//2010-4-4 lsw

PNOTEINFO pNoteInfo;//?????

//2010

HBITMAP hbm1;
HBITMAP hbm2;
//
const MessageProc mainMessages[]={
	    WM_CREATE, goCreateMain,
		WM_COMMAND, goCommandMain,
		WM_TIMER,   goTimerMain,
		WM_PAINT,  goPaintMain,
		WM_DRAWITEM, goDrawitemMain,
		WM_DESTROY, goDestroyMain
};
const CommandProc mainCommands[]={
	IDC_BTNMANUEL,     goMainManualCmd,
	IDC_BTNAUTO,       goMainAutoCmd,
	IDC_BTNEDIT,       goMainEditCmd,
	IDC_BTNMDI,        goMainMdiCmd,
	IDC_BTNLIM,        goMainConditionCmd,//by lsw in 2010-3-24 ԭΪ"IDC_BTNLIM,        goMainSLimenCmd"
	IDC_StartStop,     goMainStartStopCmd,//by lsw in 2010-4-7
	IDC_BTNREST,       goMainResetCmd,//by lsw in 2010-4-19
	IDC_BTNCLOSE,	   goExpertSystemCmd	
	//IDC_BTNCLOSE,      goMainCloseCmd
};
//�����ڲ˵������б�	

CtlWndStruct mainMenuBtns[]={
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNMANUEL,TEXT("MANUAL"),0,704,128,64, WS_BORDER| WS_CHILD|BS_OWNERDRAW|WS_VISIBLE},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNAUTO,TEXT("AUTO"),128,704,128,64,WS_BORDER|WS_CHILD|BS_OWNERDRAW|WS_VISIBLE},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNMDI,TEXT("MDI"),256,704,128,64,WS_BORDER|WS_CHILD|BS_OWNERDRAW|WS_VISIBLE},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNEDIT,TEXT("EDIT"),384,704,128,64,WS_BORDER|WS_CHILD|BS_OWNERDRAW|WS_VISIBLE},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNLIM,TEXT("CONDITION"),512,704,128,64,WS_BORDER|WS_CHILD|BS_OWNERDRAW|WS_VISIBLE},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_StartStop,TEXT("NCSET"),640,704,128,64,WS_BORDER|WS_CHILD|BS_OWNERDRAW|WS_VISIBLE},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNREST,TEXT("MAINTAIN"),768,704,128,64,WS_BORDER|WS_CHILD|BS_OWNERDRAW|WS_VISIBLE},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_BTNCLOSE,TEXT("CTLKEY"),896,704,128,64,WS_BORDER|WS_CHILD|BS_OWNERDRAW|WS_VISIBLE},
		{WS_EX_WINDOWEDGE,TEXT("STATIC"),IDC_STATICALARM,TEXT("alarm:"),0,654,1024,50,WS_BORDER|WS_CHILD|WS_BORDER|WS_VISIBLE|WS_TABSTOP}
};//���г��ֵ����һ�У�WEDGEʽШ�����˼����һ�������Ǿ�����˵ľ��룬704�Ǿඥ�˾��룬128��ÿ����ĺ��򳤶ȣ�64�Ǹ߶�

//�Ӵ��ڲ˵��б�
CtlWndStruct mainChildWnds[]={
	{WS_EX_CLIENTEDGE,TEXT("MANUALWND"),IDC_MANUAL,TEXT(""),0,320,1024,334,WS_CHILD|SS_NOTIFY|WS_BORDER},
	{WS_EX_CLIENTEDGE,TEXT("AUTOWND"),IDC_AUTO,TEXT(""),0,320,1024,334,WS_CHILD|SS_NOTIFY|WS_BORDER},
	{WS_EX_CLIENTEDGE,TEXT("EDITWND"),IDC_EDIT,TEXT(""),0,0,1024,654,WS_CHILD|SS_NOTIFY|WS_BORDER},
	{WS_EX_CLIENTEDGE,TEXT("MDIWND"),IDC_MDI,TEXT(""),0,0,1024,654,WS_CHILD|SS_NOTIFY|WS_BORDER},
	{WS_EX_CLIENTEDGE,TEXT("STATUSWND"),IDC_STATUSDISP,TEXT(""),0,0,1024,320,WS_CHILD|SS_NOTIFY|WS_BORDER},
	{WS_EX_CLIENTEDGE,TEXT("CONDITIONTWND"),IDW_CONDT,TEXT(""),0,0,1024,514,WS_CHILD|SS_NOTIFY|WS_BORDER},//by lsw 2010-3-24
	{WS_EX_CLIENTEDGE,TEXT("CONDITIONBWND"),IDW_CONDB,TEXT(""),0,514,1024,140,WS_CHILD|SS_NOTIFY|WS_BORDER},//by lsw 2010-3-24
	{WS_EX_CLIENTEDGE,TEXT("NCSetB"),IDW_NCSETB,TEXT(""),0,0,1024,654,WS_CHILD|SS_NOTIFY|WS_BORDER},//by lsw 2010-4-7
	{WS_EX_CLIENTEDGE,TEXT("MAINTION"),IDW_MAINTION_MAIN,TEXT(""),0,0,1024,654,WS_CHILD|SS_NOTIFY|WS_BORDER},//by lsw 2010-4-19
	{WS_EX_CLIENTEDGE,TEXT("EXPERT_SYSTEM"),IDC_EXPERT_SYSTEM,TEXT(""),0,0,1024,654,WS_CHILD|SS_NOTIFY|WS_BORDER}//by lsw 2010-4-19
};//�Ӵ��ڣ���һ�о���˾��룬�ڶ��оඥ�˾��룬�����г��ȣ������и߶�


//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
//  ע��: 
//
//    ����ϣ��������ӵ� Windows 95 ��
//    ��RegisterClassEx������֮ǰ�˴����� Win32 ϵͳ����ʱ��
//    ����Ҫ�˺��������÷������ô˺���
//    ʮ����Ҫ������Ӧ�ó���Ϳ��Ի�ù�����
//   ����ʽ��ȷ�ġ�Сͼ�ꡣ
//
int MyRegisterClassMain(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;//���崰����ṹ�����
    //���ṹ���и���Ա
	wcex.cbSize = sizeof(WNDCLASSEX); //?????

	wcex.style			= CS_HREDRAW | CS_VREDRAW;//�ͻ����ƶ�������/�߶ȸı�ʱ�ػ��ͻ���
	wcex.lpfnWndProc	= (WNDPROC)mainWndProc;//ָ�򴰿ڹ��̵ĳ�ָ��
	wcex.cbClsExtra		= 0;//ָ��������ṹ��洢��֮����⿪�ٵ��ֽڣ�ϵͳ��ʼ����Щ�ֽڵ�ַΪ0
	wcex.cbWndExtra		= 0;//ָ������ʵ�����⿪�ٵ��ֽ���
	wcex.hInstance		= hInstance;//ָ��ǰ���ڹ������ڵĳ���ʵ�����
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDC_EDM_NC_PROGRAM);//����ͼ��
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);//���
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);//��ɫ������ˢ
	wcex.lpszMenuName	= NULL;//(LPCTSTR)ID_Menu;//�˵���windowsCE��֧��
	wcex.lpszClassName	= szWindowClass;//ָ��һ���ַ���������ָ���Զ��崰���������
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);//��������
//ע����������
	if(!RegisterClassEx(&wcex)) return 0;
//ע���Ӵ�����

	if(!MyRegisterClassManual(hInstance)) return 0;
	if(!MyRegisterClassAuto(hInstance)) return 0;
	if(!MyRegisterClassMdi(hInstance)) return 0;
	if(!MyRegisterClassEdit(hInstance)) return 0;
	if(!MyRegisterClassStatus(hInstance)) return 0;
	if(!MyRegisterClassConditionT(hInstance)) return 0;//by lsw 2010-3-24
	if(!MyRegisterClassConditionB(hInstance)) return 0;//by lsw 2010-3-24
	if(!MyRegisterClassNCSetB(hInstance)) return 0;//by lsw 2010-4-7
	if(!MyRegisterClassMaintion(hInstance))return 0;//by lsw 2010-4-19
	if(!MyRegisterClassEXPERT(hInstance))return 0;
	return 1;
}


//        ��������ʾ�����򴰿ڡ�
//
int showMainWnd(HINSTANCE hInstance,LPTSTR lpCmdLine,int nCmdShow)
{
	HWND hWnd;//���ھ��
	DWORD dwThreadID;		//2010-4-17
	LPCmdThreadParam  pData; //2010-4-17
	hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����
  // hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
   //   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
//2010-4-17
   isEndScreen=0;
   CloseHandle(ScreenThread);	
   pData = (LPCmdThreadParam)HeapAlloc(GetProcessHeap(),
											HEAP_ZERO_MEMORY,
											sizeof(CmdThreadParam)
											);
   if(pData == NULL)
   {
	   return 1;
   }
	pData->notifyCode = 0;
	ScreenThread = CreateThread(
			NULL,
			0,
			ScreenThreadProc,
			pData,
			CREATE_SUSPENDED,
			&dwThreadID
			);
	if( ScreenThread==NULL)
	{
		return 1;
	}
	ResumeThread(ScreenThread);
	Sleep(500);//�ȴ�0.5s�����߳����㹻��ʱ����ɳ���ĳ�ʼ������
//2010-4-17	
	//����������
	hWnd = CreateWindow(szWindowClass,
		"WEDM--NC", 
		WS_VISIBLE | WS_BORDER | WS_CAPTION | WS_SYSMENU,
		0, 0, 1024, 788,
		NULL, 
		NULL,
		hInstance,
		NULL);/*���������������������ڳ�ʼ״̬�ɼ�����������ϸ�߽߱�Ĵ��ڣ����б��⣬�����ڷǿͻ�������close�Ĵ��ڣ�
			   x,y,���ߣ�ָ�򸸴��ڵľ����ָ��ؼ����Ӵ��ڵ�Ψһ����ID�ţ�ָ�����Ӧ��ʵ�����,
			   ��ӦWM_create��Ϣʱ�������ڴ��������У���ʱ��ʹ�ø�ָ�뷵�ص� lparam����ֵ����ֵ������Ҫ�������ڵ�λ�ô�С����Ϣ*/

		 //���������������0
	if (!hWnd)      return  0;//ʡ����ʽ����Iswindow(hWnd)�����жϴ����Ƿ���ڣ�

	//��ʾ���ڣ���ˢ��
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
//2010-4-17
	isEndScreen=1;
	SendMessage(hScreenEdit,EM_SETSEL,-1,-1);
	SendMessage(hScreenEdit,EM_REPLACESEL,TRUE,(LPARAM)"Start\r\n");
//2010-4-17
	return 1;

}

DWORD WINAPI ScreenThreadProc(LPVOID lpParam)
{
	DialogBox(hInst,MAKEINTRESOURCE(IDD_SCREEN),NULL,screenDlgProc);//2010-4-17
	return 0;
}

//
//  ����: WndProc(HWND, unsigned, WORD, LONG)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
//�����ڹ�����
//�����ڹ���ԭ�ͣ����ڹ��̱���Ϊ�ص�����
LRESULT CALLBACK mainWndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;

	//���Ҷ�Ӧ����ϢID,��ִ�ж�Ӧ����Ϣ�������
	for(i=0; i<dim(mainMessages);i++)
	{
		if(msgCode == mainMessages[i].uCode) return(*mainMessages[i].functionName)(hWnd,msgCode,wParam,lParam);
	}

	
	//�Բ����ڵ���Ϣ������ȱʡ���ڹ���
	return DefWindowProc(hWnd, msgCode, wParam, lParam);
}

LRESULT goCreateMain(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		
    int i;
	
//	IinitProgram(hWnd);  //�������س�����Ӳ���ĳ�ʼ��
	
//���������ڲ˵�
	for(i=0;i<dim(mainMenuBtns);i++){
		CreateWindowEx(mainMenuBtns[i].dwExStyle,
			mainMenuBtns[i].szClass,
			mainMenuBtns[i].szTitle,
			mainMenuBtns[i].lStyle,
			mainMenuBtns[i].x,
			mainMenuBtns[i].y,
			mainMenuBtns[i].cx,
			mainMenuBtns[i].cy,
			hWnd,
			(HMENU)mainMenuBtns[i].nID,
			hInst,
			NULL
			);
	}

//�����Ӵ���
	for(i=0;i<dim( mainChildWnds);i++){
		CreateWindowEx(mainChildWnds[i].dwExStyle,
			mainChildWnds[i].szClass,
			mainChildWnds[i].szTitle,
			mainChildWnds[i].lStyle,
			mainChildWnds[i].x,
			mainChildWnds[i].y,
			mainChildWnds[i].cx,
			mainChildWnds[i].cy,
			hWnd,
			(HMENU)mainChildWnds[i].nID,
			hInst,
			NULL
			);
	}
    


	//��ʾ�ֶ�����
	ShowWindow(GetDlgItem(hWnd,IDC_MANUAL),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDC_STATUSDISP),SW_SHOW);
		
	 //��Դ��ť���ʱ���趨400ms
	SetTimer(hWnd,TIMER_MAIN,400,NULL);

	//
	hbm1=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP5)); 	
	hbm2=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP6)); 
	

	//
	return 0;

}

LRESULT goCommandMain(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		//����˵�ID�ź�֪ͨ��
		int menuID, notifyCode;
		
		//���崰�ھ��
		HWND wndCtrl;
		 int i;

		 
		menuID    = LOWORD(wParam); 
		notifyCode = HIWORD(wParam); 
		wndCtrl = (HWND) lParam;
	   

		//��ʾ��ѡ�е��Ӵ��ڣ������Ӵ�������//
		for(i=0; i<dim(mainCommands);i++)
		{
			if(menuID == mainCommands[i].uCode) return(*mainCommands[i].functionName)(hWnd,wndCtrl,menuID,notifyCode);
		}
			
		return 0;

}
LRESULT goTimerMain(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{

	//���½�������Դ��ť��������


     //stoppow=inb(ADR_INA);           //�ػ�
/*	
	if((stoppow&0x04)==0x0)
                                {
                                        ioperm(ADR_OUTB,1,1);
                                        outb(0x0,ADR_OUTB);
                                        ioperm(ADR_OUTB,1,0);

                    /* Machine Poweroff */
                    /* copied from IDC_BTNCLOSE */
              /*      nPowerStat = 0;
                    MachineSave(HWND hWnd);
                    ioperm(ADR_SRV_ON, ADR_SRV_INH-ADR_SRV_ON+1, 1);
                    outb(0x0, ADR_SRV_ON); 
                    outb(0x0, ADR_SRV_CWL);
                    outb(0x0, ADR_SRV_CCWL); 
                    outb(0x0, ADR_SRV_INH);
                    ioperm(ADR_SRV_ON, ADR_SRV_INH-ADR_SRV_ON+1, 0);
    
                    KillTimer(hwnd,IDC_STIMER);
                        KillTimer(hwnd,IDC_TIMER);
                                DestroyMainWindow(hwnd);
                                MainWindowCleanup(hwnd);
                        DestroyMainWindow(hwnd);
                        PostQuitMessage(hwnd);
*/
/*
    unsigned int limit;
	char chTemp1[50];
	iowrite(hWnd,IOPCIhDevice,IO_FPGA_WRITE, ADR_OUTB,  0x0);
   	limit =ioread(hWnd,IOPCIhDevice,IO_FPGA_READ, ADR_OUTB );
	wsprintf(chTemp1, " %d",limit);
    MessageBox(hWnd,chTemp1,NULL,NULL);
*/

	return 0;
}


LRESULT goPaintMain(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		PAINTSTRUCT ps;
		HDC hdc;
		
		hdc = BeginPaint(hWnd, &ps);
		// TODO: �ڴ���������ͼ����...
		EndPaint(hWnd, &ps);
		return 0;
}

LRESULT goDestroyMain(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//2010-4-1   ������������ڹرճ���ʱ��temp.txt�ļ����ݸ��Ƶ�condition.csv�ļ��У��ڸ��ƹ����к��Կո��Լ�Сcondition.csv�ļ���С
	FILE* fp,* fpp;
	int c;
	fclose(fpTemp);
	fclose(fpSeek);
	fclose(fpLock);
	fclose(fpSet);
	fclose(fpMac);
	fclose(fpSys);
	fclose(fpEIO);
	fclose(fpcoor);
	fclose(fpEIO_Address);
	fp=fopen("condition.csv","w");
	fpp=fopen("temp.txt","r");
	while((c=getc(fpp))!=EOF)
	{
		if(c==int(' '))continue;//���Կո�
		putc(c,fp);//����condition.csv��һ����tampa.txt
	}
	putc(c,fp);
	fclose(fpp);
	fclose(fp);
	//2010-4-1
	PostQuitMessage(0);
	return 0;
}


// �����ڡ������Ϣ�������
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG://WM_INITDIALOG��Ϣ��Ӧ�������ڳ�������ʱ������Ի�����ӿؼ�ȫ��������ϣ���Ҫ��ʾ���ݵ�ʱ���͵���Ϣ��
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}


//�ֶ���ťִ�й���
LRESULT goMainManualCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{ 

	 ShowWindow( GetDlgItem(hWnd,IDC_AUTO),SW_HIDE);//��ʾ���ڣ����ػ�ɼ������ط���0���ɼ�����1
     ShowWindow( GetDlgItem(hWnd,IDC_EDIT),SW_HIDE);//GetDlgItem��ȡ�Ի������Ӵ��ڿؼ��ľ��,hWnd�Ի���ľ��	
     ShowWindow( GetDlgItem(hWnd,IDC_MDI),SW_HIDE);
	 ShowWindow(GetDlgItem(hWnd,IDC_MANUAL),SW_SHOW);
	 ShowWindow(GetDlgItem(hWnd,IDC_STATUSDISP),SW_SHOW);
	 ShowWindow(GetDlgItem(hWnd,IDW_CONDT),SW_HIDE);//by lsw 2010-3-24
	 ShowWindow(GetDlgItem(hWnd,IDW_CONDB),SW_HIDE);//by lsw 2010-3-24
	 ShowWindow(GetDlgItem(hWnd,IDW_NCSETB),SW_HIDE);//by lsw 2010-4-7
	 ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_MAIN),SW_HIDE);//by lsw 2010-4-19
	 ShowWindow(GetDlgItem(hWnd,IDC_EXPERT_SYSTEM),SW_HIDE);
	 UpdateWindow(GetDlgItem(hWnd,IDC_MANUAL));
	 
	 return 0;
	 
}

//�Զ���ťִ�й���
LRESULT goMainAutoCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)	 
{
   
	ShowWindow( GetDlgItem(hWnd,IDC_MANUAL),SW_HIDE);
	 ShowWindow( GetDlgItem(hWnd,IDC_EDIT),SW_HIDE);	
     ShowWindow( GetDlgItem(hWnd,IDC_MDI),SW_HIDE);
	 ShowWindow(GetDlgItem(hWnd,IDC_AUTO),SW_SHOW);
	 ShowWindow(GetDlgItem(hWnd,IDC_STATUSDISP),SW_SHOW);
	 ShowWindow(GetDlgItem(hWnd,IDW_CONDT),SW_HIDE);//by lsw 2010-3-24
     ShowWindow(GetDlgItem(hWnd,IDW_CONDB),SW_HIDE);//by lsw 2010-3-24
	 ShowWindow(GetDlgItem(hWnd,IDW_NCSETB),SW_HIDE);//by lsw 2010-4-7
	 ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_MAIN),SW_HIDE);//by lsw 2010-4-19
	 ShowWindow(GetDlgItem(hWnd,IDC_EXPERT_SYSTEM),SW_HIDE);
	 UpdateWindow(GetDlgItem(hWnd,IDC_AUTO));
	 
	
	 return 0;

}
//�༭��ťִ�й���
LRESULT goMainEditCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	 
	 ShowWindow( GetDlgItem(hWnd,IDC_MANUAL),SW_HIDE);
	 ShowWindow( GetDlgItem(hWnd,IDC_AUTO),SW_HIDE);	
     ShowWindow( GetDlgItem(hWnd,IDC_MDI),SW_HIDE);
	 ShowWindow(GetDlgItem(hWnd,IDC_STATUSDISP),SW_HIDE);
	 ShowWindow(GetDlgItem(hWnd,IDC_EDIT),SW_SHOW);
	 ShowWindow(GetDlgItem(hWnd,IDW_CONDT),SW_HIDE);//by lsw 2010-3-24
     ShowWindow(GetDlgItem(hWnd,IDW_CONDB),SW_HIDE);//by lsw 2010-3-24
	 ShowWindow(GetDlgItem(hWnd,IDW_NCSETB),SW_HIDE);//by lsw 2010-4-7
	 ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_MAIN),SW_HIDE);//by lsw 2010-4-19
	 ShowWindow(GetDlgItem(hWnd,IDC_EXPERT_SYSTEM),SW_HIDE);
	 UpdateWindow(GetDlgItem(hWnd,IDC_EDIT));

     InitpNoteInfo(wndCtrl);
	 return 0;
}
//MDI��ťִ�й���
LRESULT goMainMdiCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	 
   	 ShowWindow(  GetDlgItem(hWnd,IDC_MANUAL),SW_HIDE);
	 ShowWindow( GetDlgItem(hWnd,IDC_AUTO),SW_HIDE);	
     ShowWindow( GetDlgItem(hWnd,IDC_EDIT),SW_HIDE);
	 ShowWindow(GetDlgItem(hWnd,IDC_MDI),SW_SHOW);
	 ShowWindow(GetDlgItem(hWnd,IDC_STATUSDISP),SW_SHOW);
	hWnd1 = GetDlgItem(hWnd,IDC_STATUSDISP);
	 ShowWindow(GetDlgItem(hWnd,IDW_CONDT),SW_HIDE);//by lsw 2010-3-24
     ShowWindow(GetDlgItem(hWnd,IDW_CONDB),SW_HIDE);//by lsw 2010-3-24
	 ShowWindow(GetDlgItem(hWnd,IDW_NCSETB),SW_HIDE);//by lsw 2010-4-7
	 ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_MAIN),SW_HIDE);//by lsw 2010-4-19
	 ShowWindow(GetDlgItem(hWnd,IDC_EXPERT_SYSTEM),SW_HIDE);
	 UpdateWindow(GetDlgItem(hWnd,IDC_MDI));

	
	 return 0;
}


//CONDITION��ťִ�й��� by lsw 2010-3-24
LRESULT goMainConditionCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	 
	 ShowWindow(  GetDlgItem(hWnd,IDC_MANUAL),SW_HIDE);
	 ShowWindow( GetDlgItem(hWnd,IDC_AUTO),SW_HIDE);	
     ShowWindow( GetDlgItem(hWnd,IDC_EDIT),SW_HIDE);
	 ShowWindow(GetDlgItem(hWnd,IDC_MDI),SW_HIDE);
	 ShowWindow(GetDlgItem(hWnd,IDC_STATUSDISP),SW_HIDE);
	 ShowWindow(GetDlgItem(hWnd,IDW_CONDT),SW_SHOW);
     ShowWindow(GetDlgItem(hWnd,IDW_CONDB),SW_SHOW);
	 ShowWindow(GetDlgItem(hWnd,IDW_NCSETB),SW_HIDE);//by lsw 2010-4-7
	 ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_MAIN),SW_HIDE);//by lsw 2010-4-19
	 ShowWindow(GetDlgItem(hWnd,IDC_EXPERT_SYSTEM),SW_HIDE);
	 UpdateWindow(GetDlgItem(hWnd,IDW_CONDT));
	 UpdateWindow(GetDlgItem(hWnd,IDW_CONDB));
	
	 return 0;
}
//NCSet��ťִ�й��� by lsw 2010-4-7
LRESULT goMainStartStopCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	 
	 ShowWindow(  GetDlgItem(hWnd,IDC_MANUAL),SW_HIDE);
	 ShowWindow( GetDlgItem(hWnd,IDC_AUTO),SW_HIDE);	
     ShowWindow( GetDlgItem(hWnd,IDC_EDIT),SW_HIDE);
	 ShowWindow(GetDlgItem(hWnd,IDC_MDI),SW_HIDE);
	 ShowWindow(GetDlgItem(hWnd,IDC_STATUSDISP),SW_HIDE);
	 ShowWindow(GetDlgItem(hWnd,IDW_CONDT),SW_HIDE);
     ShowWindow(GetDlgItem(hWnd,IDW_CONDB),SW_HIDE);
	 ShowWindow(GetDlgItem(hWnd,IDW_NCSETB),SW_SHOW);//by lsw 2010-4-7
	 ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_MAIN),SW_HIDE);//by lsw 2010-4-19
	 ShowWindow(GetDlgItem(hWnd,IDC_EXPERT_SYSTEM),SW_HIDE);
	 UpdateWindow(GetDlgItem(hWnd,IDW_NCSETB));// by lsw 2010-4-7
	
	 return 0;
}
//MAINTION��ťִ�й���
LRESULT goMainResetCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	ShowWindow(  GetDlgItem(hWnd,IDC_MANUAL),SW_HIDE);
	ShowWindow( GetDlgItem(hWnd,IDC_AUTO),SW_HIDE);	
    ShowWindow( GetDlgItem(hWnd,IDC_EDIT),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_MDI),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_STATUSDISP),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDW_CONDT),SW_HIDE);
    ShowWindow(GetDlgItem(hWnd,IDW_CONDB),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDW_NCSETB),SW_HIDE);//by lsw 2010-4-7
	ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_MAIN),SW_SHOW);//by lsw 2010-4-19
	ShowWindow(GetDlgItem(hWnd,IDC_EXPERT_SYSTEM),SW_HIDE);
	UpdateWindow(GetDlgItem(hWnd,IDW_MAINTION_MAIN));// by lsw 2010-4-19
	
	 return 0;
}

LRESULT goExpertSystemCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	ShowWindow(GetDlgItem(hWnd,IDC_MANUAL),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_AUTO),SW_HIDE);	
    ShowWindow(GetDlgItem(hWnd,IDC_EDIT),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_MDI),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_STATUSDISP),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDW_CONDT),SW_HIDE);
    ShowWindow(GetDlgItem(hWnd,IDW_CONDB),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDW_NCSETB),SW_HIDE);//by lsw 2010-4-7
	ShowWindow(GetDlgItem(hWnd,IDW_MAINTION_MAIN),SW_HIDE);//by lsw 2010-4-19
	ShowWindow(GetDlgItem(hWnd,IDC_EXPERT_SYSTEM),SW_SHOW);
	UpdateWindow(GetDlgItem(hWnd,IDC_EXPERT_SYSTEM));// by lsw 2010-4-19
	
	 return 0;
}

void InitpNoteInfo(HWND hWnd)
{
	TCHAR currentpath [MAX_PATH + 1];
	
	GetCurrentDirectory(MAX_PATH+1,currentpath); 
	
    
        //if(pNoteInfo == NULL)
	if(!(pNoteInfo = (PNOTEINFO)malloc(sizeof(NOTEINFO))))
	{	MessageBox(hWnd,"can not malloc pNoteInfo in InitpNoteInfo function",NULL,NULL);
		return ; //error!!
	}
        pNoteInfo->ischanged = FALSE;
        strcpy(pNoteInfo->fileName , "untitled.txt");
        strcpy(pNoteInfo->filePath , currentpath);
	if (pNoteInfo->filePath [strlen (pNoteInfo->filePath) - 1] != '\\')
        	strcat (pNoteInfo->filePath, "\\");
        pNoteInfo->fileSize = 0;
        pNoteInfo->Buffer = NULL;
        pNoteInfo->hMLEditWnd = NULL;
} 

int MachineSave(HWND hWnd)    //Save the machine postion before the program is over
{

    char data[51][_MAX_FNAME]={"","","","",""};
	int i,j;
	int MachineFile;

	MachineFile=_open("MachinePos.txt",O_WRONLY);
	
	if(MachineFile==-1)
	{
		MessageBox(hWnd,"MachinePos.txt open error when write the file in fuction MachinePosSave ",NULL,NULL);
		  return 1;
	}

	 //����������� 
	sprintf_s( data[0], "%.3f", data_m.x );
	sprintf_s( data[1], "%.3f", data_m.y );
	sprintf_s( data[2], "%.3f", data_m.z );
	sprintf_s( data[3], "%.3f", data_m.b );
	sprintf_s( data[4], "%.3f", data_m.c );
	
		 
	for(i=0;i<5;i++)
	{
		j=_write(MachineFile,data[i],strlen(data[i]));
	
		if(j != strlen(data[i]))
		{
			 MessageBox(hWnd,"MachineFile write error in fuction MachinePosSave ",NULL,NULL);
			 return 1;
		}	
		_write(MachineFile,"\n",1);
	}	
	
	//���湤������
	sprintf_s( data[0], "%.3f", data_r.x );
	sprintf_s( data[1], "%.3f", data_r.y );
	sprintf_s( data[2], "%.3f", data_r.z );
	sprintf_s( data[3], "%.3f", data_r.b );
	sprintf_s( data[4], "%.3f", data_r.c );	
		 
	for(i=0;i<5;i++)
	{
		j=_write(MachineFile,data[i],strlen(data[i]));
	
		if(j != strlen(data[i]))
		{
			 MessageBox(hWnd,"MachineFile write error in fuction MachinePosSave ",NULL,NULL);
			 return 1;
		}	
		_write(MachineFile,"\n",1);
	}

	//����ӹ��ļ���
	j=_write(MachineFile,AutoMachineFileName,strlen(AutoMachineFileName));
	
	if(j != strlen(AutoMachineFileName))
	{
			 MessageBox(hWnd,"MachineFile write error in fuction MachinePosSave ",NULL,NULL);
			 return 1;
	}
	
	_write(MachineFile,"\n",1);

	//����ӹ�����
	
	sprintf_s( data[0], "%d", Machiningparam.eNo );  //����������
	
	j=_write(MachineFile,data[0],strlen(data[0]));
	
	if(j != strlen(data[0]))
	{
			 MessageBox(hWnd,"MachineFile write error in fuction MachinePosSave ",NULL,NULL);
			 return 1;
	}
	
	_write(MachineFile,"\n",1);
	
	
	for(i=0;i<30;i++)
	{                                                ////������ӹ�����ֵ
        sprintf_s( data[i], "%d", Machiningparam.item[i].value );
		j=_write(MachineFile,data[i],strlen(data[i]));
	
		if(j != strlen(data[i]))
		{
			 MessageBox(hWnd,"MachineFile write error in fuction MachinePosSave ",NULL,NULL);
			 return 1;
		}	
		_write(MachineFile,"\n",1);	
	}
	//����ӹ������⵱ǰ��״̬
	//by lsw 2010-4-6
	for(i=9;i>=1;i--)
	{
		startR=seekPre(startR);
		if(startR==0)startR=seekPre(startR);
	}			
	sprintf_s( data[0], "%d", startR ); 
	j=_write(MachineFile,data[0],strlen(data[0]));
	if(j != strlen(data[0]))
	{
			 MessageBox(hWnd,"MachineFile write error in fuction MachinePosSave ",NULL,NULL);
			 return 1;
	}	
	_write(MachineFile,"\n",1);
	// by lsw 2010-4-6

	if(_close(MachineFile))
	{
		MessageBox(hWnd,"MachinePos.txt close error when whiting the file in fuction MachinePosSave ",NULL,NULL);
		return 1;
	}
	return 0;
}  
int DspProgramLoad(HWND hWnd)
{
	BOOL bResult;
	DWORD junk;       
		// DSP�����´�
		
		bResult = DeviceIoControl(NCPCIhDevice,  // device to be queried
					NC_HPIBOOTLOAD,  // operation to perform
                    (LPVOID)&code, sizeof(code), // no input buffer
                    NULL,0,     // output buffer
                     &junk,                 // # bytes returned
                     (LPOVERLAPPED)NULL);  // synchronous I/O
		
		if(bResult==FALSE)
		{
				MessageBox(hWnd,"can not get i/o data in fuction HpiLoad", NULL,NULL);
				return 1;
		}
		
	
		return 0;
}

int DspProgramLoad1(HWND hWnd)
{
	BOOL bResult;
	DWORD junk;       
		// DSP�����´�
		
		bResult = DeviceIoControl(PowerPCIhDevice,  // device to be queried
					Power_HPIBOOTLOAD,  // operation to perform
                    (LPVOID)&code, sizeof(code), // no input buffer
                    NULL,0,     // output buffer
                     &junk,                 // # bytes returned
                     (LPOVERLAPPED)NULL);  // synchronous I/O
		
		if(bResult==FALSE)
		{
				MessageBox(hWnd,"can not get i/o data in fuction HpiLoad", NULL,NULL);
				return 1;
		}
		
	
		return 0;
}







unsigned int  ioread(HWND hWnd,HANDLE hDvice,DWORD ctlword,  DWORD adr )
{
	BOOL bResult;                 // results flag
	DWORD junk;
	unsigned int returnbuf;                   // discard results
	LPIOstru pData; 


	pData = (LPIOstru)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,	sizeof(IOstru));
	if(pData == NULL)
			{
				//MessageBox(hWnd,"can not alloc heapmemory in function ioread",NULL,NULL);
				return 1;
	}

	pData->adr = adr;
	pData->val = 0;

	bResult = DeviceIoControl(hDvice,  // device to be queried
					ctlword,  // operation to perform
                    pData, sizeof(IOstru), // no input buffer
                    pData, sizeof(IOstru),     // output buffer
                     &junk,                 // # bytes returned
                     (LPOVERLAPPED) NULL);  // synchronous I/O
	if(bResult==FALSE)
	{
			//MessageBox(hWnd,"can not get i/o data in fuction ioread", NULL,NULL);
			return 1;
	}

	returnbuf = pData->val;

	if(HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pData) == 0){
		MessageBox(hWnd,"can not free heapmemory in function iowrite",NULL,NULL);
		return 1;
	}
	
	return returnbuf;
}

LRESULT iowrite(HWND hWnd,HANDLE hDvice,DWORD ctlword,  DWORD adr, DWORD buffer)
{
	BOOL bResult;                 // results flag
	DWORD junk;                   // discard results
	LPIOstru pData; 


	pData = (LPIOstru)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,	sizeof(IOstru));
	if(pData == NULL)
			{
				//MessageBox(hWnd,"can not alloc heapmemory in function iowrite",NULL,NULL);
				return 1;
	}
	
	pData->adr = adr;
	pData->val = buffer;
		
	bResult = DeviceIoControl(hDvice,  // device to be queried
					ctlword,  // operation to perform
                    pData,sizeof(IOstru), // no input buffer
                    NULL,0,     // output buffer
                     &junk,                 // # bytes returned
                     (LPOVERLAPPED) NULL);  // synchronous I/O
	
	if(HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pData) == 0){
		MessageBox(hWnd,"can not free heapmemory in function iowrite",NULL,NULL);
		return 1;
	}
	
	
	if(bResult==FALSE)
	{
			//MessageBox(hWnd,"can not get i/o data in fuction iowrite", NULL,NULL);
			return 1;
	}
	return 0;
}

int SendNCDriverUserDecodeEvent(HWND hWnd,HANDLE hEvent)
{
		
		BOOL bResult;
		DWORD junk;
	
		       
		// ��NCDIVER�´������߳��¼�
		
		bResult = DeviceIoControl(NCPCIhDevice,  // device to be queried
                  NC_TRANSMIT_EVENT,  // operation to perform
                    &hEvent, sizeof(hEvent), // no input buffer
                    NULL,0,     // output buffer
                     &junk,                 // # bytes returned
                     (LPOVERLAPPED) NULL);  // synchronous I/O
	
		if(bResult==FALSE)
		{
				MessageBox(hWnd,"can not get i/o data in fuction SendNCDriverUserDecodeEvent", NULL,NULL);
				return 1;
		}
			
		
	
		return 0;
}


//
LRESULT goDrawitemMain(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
    LPDRAWITEMSTRUCT lpdis; 
	lpdis = (LPDRAWITEMSTRUCT) lParam; 
    hdc = CreateCompatibleDC(lpdis->hDC);
	if (lpdis->itemState & ODS_FOCUS)  // if selected 
		SelectObject(hdc, hbm2); 
	else 
		SelectObject(hdc, hbm1); 
	StretchBlt( 
                lpdis->hDC,         // destination DC 
                lpdis->rcItem.left, // x upper left 
                lpdis->rcItem.top,  // y upper left  
                // The next two lines specify the width and 
                // height. 
                lpdis->rcItem.right - lpdis->rcItem.left, 
                lpdis->rcItem.bottom - lpdis->rcItem.top, 
                hdc,    // source device context 
                0, 0,      // x and y upper left 
                57,        // source bitmap width 
                30,        // source bitmap height 
                SRCCOPY);  // raster operation 
	SetBkMode(lpdis->hDC,TRANSPARENT);	
	switch(lpdis->CtlID)
	{
	case IDC_BTNMANUEL:
		DrawText(lpdis->hDC,"MANUAL",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNAUTO:
		DrawText(lpdis->hDC,"AUTO",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNEDIT:
		DrawText(lpdis->hDC,"EDIT",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNMDI:
		DrawText(lpdis->hDC,"MDI",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNREST:
		DrawText(lpdis->hDC,"MAINTAIN",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNLIM:
		DrawText(lpdis->hDC,"CONDITION",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNCLOSE:
		DrawText(lpdis->hDC,"CTLKEY",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_StartStop:
		DrawText(lpdis->hDC,"NCSET",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
/////////////////////////////////
	case IDC_SELCON:
		DrawText(lpdis->hDC,"Machining Condition",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_SELSTATE:
		DrawText(lpdis->hDC,"Machining State",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BUILDMACHING:
		DrawText(lpdis->hDC,"Build Machining",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
/////////////////////////////
	case IDC_BTNYZ:
		DrawText(lpdis->hDC,"Y-Z",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNXY:
		DrawText(lpdis->hDC,"X-Y",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNXZ:
		DrawText(lpdis->hDC,"X-Z",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNNEW:
		DrawText(lpdis->hDC,"NEW",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNOPEN:
		DrawText(lpdis->hDC,"OPEN",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNSAVE:
		DrawText(lpdis->hDC,"SAVE",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNEDITCLOSE:
		DrawText(lpdis->hDC,"CLOSE",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNDELET:
		DrawText(lpdis->hDC,"DELET",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNDRAW:
		DrawText(lpdis->hDC,"DRAW",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNGRAPHRESET:
		DrawText(lpdis->hDC,"RESET",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
///////////////////////////////////////
	case IDC_AXISALLZERO:
		DrawText(lpdis->hDC,"ALL",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_AXISXZERO:
		DrawText(lpdis->hDC,"XZERO",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_AXISYZERO:
		DrawText(lpdis->hDC,"YZERO",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_AXISZZERO:
		DrawText(lpdis->hDC,"ZZERO",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_AXISBZERO:
		DrawText(lpdis->hDC,"BZERO",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_AXISCZERO:
		DrawText(lpdis->hDC,"CZERO",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNSTARTSTOP:
		DrawText(lpdis->hDC,"START",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTNRESET:
		DrawText(lpdis->hDC,"RESET",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
		
	case IDC_BTNSUSPEND:
		DrawText(lpdis->hDC,"SUSPEND",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
////////////////////////////////////////
	case IDC_MachAllReturn:
		DrawText(lpdis->hDC,"MachAllRet",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_MachXReturn:
		DrawText(lpdis->hDC,"MachXRet",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_MachYReturn:
		DrawText(lpdis->hDC,"MachYRet",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_MachZReturn:
		DrawText(lpdis->hDC,"MachZRet",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_MachBReturn:
		DrawText(lpdis->hDC,"MachBRet",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_MachCReturn:
		DrawText(lpdis->hDC,"MachCRet",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_WorkAllReturn:
		DrawText(lpdis->hDC,"WorkAllRet",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_WorkXReturn:
		DrawText(lpdis->hDC,"WorkXRet",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_WorkYReturn:
		DrawText(lpdis->hDC,"WorkYRet",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_WorkZReturn:
		DrawText(lpdis->hDC,"WorkZRet",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
///////////////////////////////////////
	case IDC_WorkBReturn:
		DrawText(lpdis->hDC,"WorkBRet",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_WorkCReturn:
		DrawText(lpdis->hDC,"WorkCRet",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_XTouchP:
		DrawText(lpdis->hDC,"X+",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_YTouchP:
		DrawText(lpdis->hDC,"Y+",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_ZTouchP:
		DrawText(lpdis->hDC,"Z+",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTouchP:
		DrawText(lpdis->hDC,"B+",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_CTouchP:
		DrawText(lpdis->hDC,"C+",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_XTouchM:
		DrawText(lpdis->hDC,"X-",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_YTouchM:
		DrawText(lpdis->hDC,"Y-",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_ZTouchM:
		DrawText(lpdis->hDC,"Z-",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_BTouchM:
		DrawText(lpdis->hDC,"B-",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
	case IDC_CTouchM:
		DrawText(lpdis->hDC,"C-",-1,&lpdis->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		break;
		}
	DeleteDC(hdc);
    return TRUE; 
}