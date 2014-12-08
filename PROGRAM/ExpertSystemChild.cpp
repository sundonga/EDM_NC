#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;
extern HANDLE NCPCIhDevice; 
int EXPERT;
void ExpertsystemhreadProc();
LPCmdThreadParam  pDataInEXPERT;
void fc_Expert_upday(HWND hWnd);
int ExpertCount=0;
LRESULT goExpertReturn(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode);
LRESULT goExpertCodeGenerate(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode);
extern HWND hWnd1;
extern int ExpertSystem_Flag;
extern DSP_Para_data DSP_data;
HBITMAP hBitmap_ExpertSystem;
HBITMAP hBitmap_ExpertSystem1;
HBITMAP hBitmap_ExpertSystem2;
HBITMAP hBitmap_ExpertSystem3;
HBITMAP hBitmap_ExpertSystem4;
HBITMAP hBitmap_ExpertSystem5;
HBITMAP hBitmap_ExpertSystem6;

extern DSP_Para_data DSP_data;

const MessageProc ExpertSystemChildMessages[]={
	    WM_CREATE, goCreateExpertSystemChild,
		WM_PAINT,  goPaintExpertSystemChild,
		WM_COMMAND, goCommandExpertSystemChild,
		WM_NOTIFY, goListExpertSystemChild,
		WM_DESTROY, goDestroyExpertSystemChild
};
const CommandProc ExpertCommands[]={
	    IDC_EXPERTRETURN, goExpertReturn,
		IDC_EXPERT_CODE, goExpertCodeGenerate,
};

CtlWndStruct ExpertSystemChildBtns[]={
		//{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_EXPERT0,TEXT("Expert"),0,0,50,50,WS_BORDER|WS_VISIBLE|WS_CHILD|WS_TABSTOP | BS_PUSHBUTTON|SS_CENTER}
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_EXPERTRETURN,TEXT("RETURN"),1,55,98,48,WS_VISIBLE|WS_BORDER|WS_CHILD|BS_PUSHBUTTON},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_EXPERT_CODE,TEXT("GENERATE"),1,104,98,48,WS_VISIBLE|WS_BORDER|WS_CHILD|BS_PUSHBUTTON},
		{WS_EX_CLIENTEDGE,TEXT("LISTBOX"),IDC_EXPERTLISTBOX,TEXT(""),100,1,923,54,WS_VISIBLE|WS_BORDER|WS_CHILD},

		{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_EXPERT0,TEXT(""),100,55,280,575,WS_VISIBLE|WS_BORDER|WS_CHILD|LVS_REPORT},

		{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_SIMPLE,TEXT(""),380,55,343,575,WS_VISIBLE|WS_BORDER|WS_CHILD|LVS_REPORT},
		{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_HAVE_BOTTOM,TEXT(""),380,55,343,575,WS_VISIBLE|WS_BORDER|WS_CHILD|LVS_REPORT},
		{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_NOTHAVE_BOTTOM,TEXT(""),380,55,343,575,WS_VISIBLE|WS_BORDER|WS_CHILD|LVS_REPORT},
		{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_ANGULAR,TEXT(""),380,55,343,575,WS_VISIBLE|WS_BORDER|WS_CHILD|LVS_REPORT},

		{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_LINE_DISTRIBUTE,TEXT(""),380,55,343,575,WS_VISIBLE|WS_BORDER|WS_CHILD|LVS_REPORT},
		{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_ARC_DISTRIBUTE,TEXT(""),380,55,343,575,WS_VISIBLE|WS_BORDER|WS_CHILD|LVS_REPORT},
		{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_DISPERSE_DISTRIBUTE,TEXT(""),380,55,343,575,WS_VISIBLE|WS_BORDER|WS_CHILD|LVS_REPORT},
		{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_COORDINATE_DISTRIBUTE,TEXT(""),380,55,343,575,WS_VISIBLE|WS_BORDER|WS_CHILD|LVS_REPORT},

		{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_MEASURE1,TEXT(""),380,55,343,575,WS_VISIBLE|WS_BORDER|WS_CHILD|LVS_REPORT},
		{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_MEASURE2,TEXT(""),380,55,343,575,WS_VISIBLE|WS_BORDER|WS_CHILD|LVS_REPORT},
		{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_MEASURE3,TEXT(""),380,55,343,575,WS_VISIBLE|WS_BORDER|WS_CHILD|LVS_REPORT},
		{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_MEASURE4,TEXT(""),380,55,343,575,WS_VISIBLE|WS_BORDER|WS_CHILD|LVS_REPORT},

		{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_AUXILIARY_COORDINATE,TEXT(""),380,55,343,575,WS_VISIBLE|WS_BORDER|WS_CHILD|LVS_REPORT},
		{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_AUXILIARY_ATC,TEXT(""),380,55,343,575,WS_VISIBLE|WS_BORDER|WS_CHILD|LVS_REPORT},
		{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_AUXILIARY_GMODE,TEXT(""),380,55,343,575,WS_VISIBLE|WS_BORDER|WS_CHILD|LVS_REPORT},
		{WS_EX_CLIENTEDGE,TEXT("LISTVIEW"),IDC_AUXILIARY_MMODE,TEXT(""),380,55,343,575,WS_VISIBLE|WS_BORDER|WS_CHILD|LVS_REPORT},


};
//�Ӵ��ڣ���һ�о���˾��룬�ڶ��оඥ�˾��룬�����г��ȣ������и߶�

int MyRegisterClassExpertSystemChild(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)ExpertSystemChildWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= 0;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//(LPCTSTR)IDC_EX1;
	wcex.lpszClassName	= TEXT("EXPERTChildWND");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);;
//ע���ֶ�������
	if(!RegisterClassEx(&wcex)) return 0;

	return 1;
}

//�ֶ����ڹ�����
LRESULT CALLBACK ExpertSystemChildWndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;
	

	//���Ҷ�Ӧ����ϢID,��ִ�ж�Ӧ����Ϣ�������
	for(i=0; i<dim(ExpertSystemChildMessages);i++)
	{
		if(msgCode == ExpertSystemChildMessages[i].uCode) return(*ExpertSystemChildMessages[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//�Բ����ڵ���Ϣ������ȱʡ���ڹ���
	return DefWindowProc(hWnd, msgCode, wParam, lParam);
}

LRESULT goCreateExpertSystemChild(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		
	HIMAGELIST hSmall;
	HICON hIcon;
	int i;
	LVCOLUMN lvcol;
	LVITEM item;
	int nItemCount;
	char * caption [] ={"�趨ѡ��","ģʽ�趨","����","����ѡ��","����ֵ"};
	char * text1[]={"----��      ��-----","--------------------","--------------------","--------------------","----��      λ----","--------------------","--------------------","--------------------",
					"----��      ��-----","--------------------","--------------------","--------------------","----��      ��----","--------------------","--------------------","--------------------"};
	char * text2[]={"G100: ��  ��  ��  ��","G101: ��  ��  ��  ��","G102: ֱ  ͨ  ��  ��","G103: ��  ��  ��  ��","G104:   ��        ","G105: ��  /  ��  ��  ��","G106: Բ  ��  ��  ��","G107: ɢ  ��  ��  ��","G108: ��  ��  /  ��  ��","G109: ��  ��  /  ��  ��","G110: ��  ��  /  ��  ��","G111: ��  ��  ��  ��","G112: ��  ��  ��  ��","G113: A  T  C","G114: G  ��  ��","G115: M  ��  ��"};
	char * text[]={	"X/Y/Z��ѡ��",	"�ӹ����                        [um]",		"",	"ҡ����ʽ1","ҡ���뾶","����/����ҡ��","ҡ����������","̧����ʽ","̧���߶�                        [um]","̧���ٶ�                        [mm/min]","�ŵ�ʱ��                        [ms]","Ԥ�����                        [um]","ҡ��ͶӰ�Ƕ�","","","","","",
					"X��ӹ����                  [um]",	"Y��ӹ����                  [um]",	"Z��ӹ����                  [um]",	"ҡ����ʽ2","ҡ���뾶","����/����ҡ��","ҡ����������","̧����ʽ","̧���߶�                        [um]","̧���ٶ�                        [mm/min]","�ŵ�ʱ��                        [ms]","Ԥ�����                        [um]","����б��                        [��]","","","","","",
					"X��ӹ����                  [um]",	"Y��ӹ����                  [um]",	"Z��ӹ����                  [um]",	"ҡ����ʽ3","ҡ���뾶","����/����ҡ��","ҡ����������","̧����ʽ","̧���߶�                        [um]","̧���ٶ�                        [mm/min]","�ŵ�ʱ��                        [ms]","Ԥ�����                        [um]","����б��                        [��]","","","","","",
					"X��ӹ����                  [um]",	"Y��ӹ����                  [um]",	"Z��ӹ����                  [um]",	"ҡ����ʽ4","ҡ���뾶","����/����ҡ��","ҡ����������","̧����ʽ","̧���߶�                        [um]","̧���ٶ�                        [mm/min]","�ŵ�ʱ��                        [ms]","Ԥ�����                        [um]","����б��                        [��]","","","","","",
					
					"ƽ��ָ��",	"�����������",	"��ʼλ������X              [um]","                        Y              [um]","                        Z              [um]","�ھ�I��","�ھ�II��","��ת�Ƕ�","����"," "," "," ","","","","","","",
					"ƽ��ָ��",	"�����������",	"��������X                      [um]",	"                Y                      [um]","                Z                      [um]","                C                      [um]","�뾶                                [um]","�ھ�Ƕ�                        [0.001��]","�ȷָ���","���Ƕ�","�ӹ�λ��","C��ͬ��","","","","","","",
					"ƽ��ָ��",	"�����������",	"��ʼλ������X[um]","                        Y[um]","                        Z[um]","2��λ I��","       II��","3��λ I��","       II��","4��λ I��","       II��","5��λ I��","       II��","6��λ I��","       II��","","","",
					"ƽ��ָ��",	"��������G54",	"��������G55",	"��������G56","��������G57","��������G58","��������G59","�ӹ�λ����X                  [um]","                      Y                  [um]","                      Z                  [um]","�ֶ�C��","�缫��ת����","����ӹ�","","","","","",
	
					"�����г�X[um]","        Y[um]","        Z[um]","�����г�X[um]","        Y[um]",	"        Z[um]","�����ٶ�[mm/min]","��ת�Ƕ�[0.001��]","��������˻���[um]","��׼�����������","�缫�ߴ�/������ֱ��","���������������","�缫ƫ��������","�缫�ߴ�/������ֱ��","Z����������λX[um]","               Y[um]","","",
					"ƽ��ָ��",	"�����г�X[um]",	"        Y[um]",	"        Z[um]","�����г�X[um]","        Y[um]","        Z[um]","�����ٶ�[mm/min]","��ת�Ƕ�[0.001��]","��������˻���[um]","�������̽����[um]","��׼�����������","�缫�ߴ�/������ֱ��","���������������","�缫ƫ��������","�缫�ߴ�/������ֱ��","Z����������λX[um]","               Y[um]",
					"ƽ��ָ��",	"�����г�X[um]",	"        Y[um]",	"        Z[um]","������ֱ��[um]","���������ߴ�Z[um]","�����ٶ�[mm/min]","��ת�Ƕ�[0.001��]","��������˻���[um]","��׼�����������","���������������","�缫ƫ��������","�缫ƫ��������2","","","","","",
					"ƽ��ָ��",	"�����׾�[um]",	"������[um]",	"��1��I��[um]","      II��[um]","��2��I��[um]","     II��[um]","��3��I��[um]","     II��[um]","��4��I��[um]","     II��[um]","�����ٶ�[mm/min]","","","","","","",
	
					"���������趨",	"��������ֵ  X[um]","            Y[um]","            Z[um]","            C[0.001��]","�ֲ�ƫ����  X[um]","            Y[um]","            Z[um]","            C[um]","�缫ƫ��������"," "," ","","","","","","",
					"ԭ���Ϊ˳��",	"�о�λ",	"���缫��ʽ",	"�����ϵ缫����","�����µ缫����","�缫ƫ��������"," "," "," "," "," "," ","","","","","","",
					"ABS/INC",	"�г�����",	"��������  X��ʼ",	"          X����","          Y��ʼ","          Y����","          Z��ʼ","          Z����","��ʱָ��ʱ��","G����ֵ"," "," ","","","","","","",
					"��������",	"���Է�ת",	"�ӹ�Һ",	"ATC������","��Ъ����ʱ����","�ӹ�������","MA/MR C��","YBS","DH","M����ֵ","�ŵ�ʱ��","Ԥ�����","","","","","",""};

	char * text_sign[]={"A",	"L",	"",		"S","R","F","O","J","H","V","T","B","","","","","","",
						"X1",	"Y1",	"Z1",	"S","R","F","O","J","H","V","T","B","A","","","","","",
						"X2",	"Y2",	"Z2",	"S","R","F","O","J","H","V","T","B","","","","","","",
						"X3",	"Y3",	"Z3",	"S","R","F","O","J","H","V","T","B","","","","","","",

						"P","U","V","C","I","J","A","B","Q","W","F","K","D","N","","","","",
						"P","U","V","C","I","J","A","B","Q","W","F","K","D","N","","","","",
						"P","U","V","C","I","J","A","B","Q","W","F","K","D","N","","","","",
						"P","U","V","C","I","J","A","B","Q","W","F","K","D","N","","","","",
	
						"P","U","V","C","I","J","A","B","Q","W","F","K","D","N","","","","",
						"P","U","V","C","I","J","A","B","Q","W","F","K","D","N","","","","",
						"P","U","V","C","I","J","A","B","Q","W","F","K","D","N","","","","",
						"P","U","V","C","I","J","A","B","Q","W","F","K","D","N","","","","",
	
						"P","U","V","C","I","J","A","B","Q","W","F","K","D","N","","","","",
						"P","U","V","C","I","J","A","B","Q","W","F","K","D","N","","","","",
						"P","U","V","C","I","J","A","B","Q","W","F","K","D","N","","","","",
						"P","U","V","C","I","J","A","B","Q","W","F","K","D","N","","","",""};


	int k,j;

	for(i=0;i<3;i++)
	{		
		CreateWindowEx( ExpertSystemChildBtns[i].dwExStyle,
			ExpertSystemChildBtns[i].szClass,
			ExpertSystemChildBtns[i].szTitle,
			ExpertSystemChildBtns[i].lStyle,
			ExpertSystemChildBtns[i].x,
			ExpertSystemChildBtns[i].y,
			ExpertSystemChildBtns[i].cx,
			ExpertSystemChildBtns[i].cy,
			hWnd,
			(HMENU)ExpertSystemChildBtns[i].nID,
			hInst,
			NULL
			);

	}

	for(i=3;i<dim(ExpertSystemChildBtns);i++){
		CreateWindowEx(WS_EX_WINDOWEDGE,
			WC_LISTVIEW,
			ExpertSystemChildBtns[i].szTitle,
			ExpertSystemChildBtns[i].lStyle,
			ExpertSystemChildBtns[i].x,
			ExpertSystemChildBtns[i].y,
			ExpertSystemChildBtns[i].cx,
			ExpertSystemChildBtns[i].cy,
			hWnd,
			(HMENU)ExpertSystemChildBtns[i].nID,
			hInst,
			NULL
			);
	}

		ZeroMemory(&lvcol, sizeof(LV_COLUMN));
		ListView_SetTextBkColor(GetDlgItem(hWnd,ExpertSystemChildBtns[3].nID),RGB(255,255,255));
		lvcol.iOrder=0;
		lvcol.iSubItem = 1;
		lvcol.pszText = caption[0];
		lvcol.cx = 135;
		lvcol.fmt = LVCFMT_CENTER;
		lvcol.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM|LVCF_ORDER|LVCF_FMT;
		lvcol.cchTextMax = 60;
		SendMessage (GetDlgItem(hWnd,ExpertSystemChildBtns[3].nID), LVM_INSERTCOLUMN, 0, (LPARAM)&lvcol);
		lvcol.iOrder=1;
		lvcol.iSubItem = 1;
		lvcol.pszText = caption[1];
		lvcol.cx = 145;
		lvcol.fmt = LVCFMT_CENTER;
		lvcol.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM|LVCF_ORDER|LVCF_FMT;
		lvcol.cchTextMax = 60;
		SendMessage (GetDlgItem(hWnd,ExpertSystemChildBtns[3].nID), LVM_INSERTCOLUMN, 0, (LPARAM)&lvcol);
		ListView_SetExtendedListViewStyle(GetDlgItem(hWnd,ExpertSystemChildBtns[3].nID),LVS_EX_FULLROWSELECT);

		for(i=0;i<16;i++)
		{
			ZeroMemory(&item, sizeof(LV_ITEM));
			nItemCount = ListView_GetItemCount(GetDlgItem(hWnd,ExpertSystemChildBtns[3].nID));
			item.iItem = nItemCount;
			SendMessage(GetDlgItem(hWnd,ExpertSystemChildBtns[3].nID),LVM_INSERTITEM,0,(LPARAM)&item);
		}	

		for(i=0;i<16;i++)
		{
			item.iItem = i;
			item.mask = LVIF_TEXT;
			item.cchTextMax =60;
			item.pszText = text1[i];
			item.iSubItem = 1;
			SendMessage(GetDlgItem(hWnd,ExpertSystemChildBtns[3].nID),LVM_SETITEM,0,(LPARAM)&item);
		}
		for(i=0;i<16;i++)
		{
			item.iItem = i;
			item.mask = LVIF_TEXT;
			item.cchTextMax =60;
			item.pszText = text2[i];
			item.iSubItem = 0;
			SendMessage(GetDlgItem(hWnd,ExpertSystemChildBtns[3].nID),LVM_SETITEM,0,(LPARAM)&item);
		}

	for(i=4;i<dim(ExpertSystemChildBtns);i++)
	{
		ZeroMemory(&lvcol, sizeof(LV_COLUMN));
		lvcol.iOrder=0;
		lvcol.iSubItem = 0;
		lvcol.pszText = caption[2];
		lvcol.cx = 50;
		lvcol.fmt = LVCFMT_CENTER;
		lvcol.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM|LVCF_ORDER|LVCF_FMT;
		lvcol.cchTextMax = 60;
		SendMessage (GetDlgItem(hWnd,ExpertSystemChildBtns[i].nID), LVM_INSERTCOLUMN, 0, (LPARAM)&lvcol);

		ListView_SetTextBkColor(GetDlgItem(hWnd,ExpertSystemChildBtns[i].nID),RGB(255,255,255));
		lvcol.iOrder=1;
		lvcol.iSubItem = 1;
		lvcol.pszText = caption[3];
		lvcol.cx = 180;
		lvcol.fmt = LVCFMT_CENTER;
		lvcol.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM|LVCF_ORDER|LVCF_FMT;
		lvcol.cchTextMax = 60;
		SendMessage (GetDlgItem(hWnd,ExpertSystemChildBtns[i].nID), LVM_INSERTCOLUMN, 0, (LPARAM)&lvcol);

		lvcol.iOrder=2;
		lvcol.iSubItem = 2;
		lvcol.pszText = caption[4];
		lvcol.cx = 113;
		lvcol.fmt = LVCFMT_CENTER;
		lvcol.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM|LVCF_ORDER|LVCF_FMT;
		lvcol.cchTextMax = 60;
		SendMessage (GetDlgItem(hWnd,ExpertSystemChildBtns[i].nID), LVM_INSERTCOLUMN, 0, (LPARAM)&lvcol);
		ListView_SetExtendedListViewStyle(GetDlgItem(hWnd,ExpertSystemChildBtns[i].nID),LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

		for(j=0;j<18;j++)
		{
			ZeroMemory(&item, sizeof(LV_ITEM));
			nItemCount = ListView_GetItemCount(GetDlgItem(hWnd,ExpertSystemChildBtns[i].nID));
			item.iItem = nItemCount;
			SendMessage(GetDlgItem(hWnd,ExpertSystemChildBtns[i].nID),LVM_INSERTITEM,0,(LPARAM)&item);
		}	
		for(j=0;j<18;j++)
		{
			item.iItem = j;
			item.mask = LVIF_TEXT;
			item.cchTextMax =60;
			item.pszText = text_sign[j + (i-4)*18];
			item.iSubItem = 2;
			SendMessage(GetDlgItem(hWnd,ExpertSystemChildBtns[i].nID),LVM_SETITEM,0,(LPARAM)&item);
		}


		for(j=0;j<18;j++)
		{
			item.iItem = j;
			item.mask = LVIF_TEXT;
			item.cchTextMax =60;
			item.pszText = text[j + (i-4)*18];
			item.iSubItem = 1;
			SendMessage(GetDlgItem(hWnd,ExpertSystemChildBtns[i].nID),LVM_SETITEM,0,(LPARAM)&item);
		}

		for(j=0;j<18;j++)
		{
			item.iItem = j;
			item.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM|LVCF_ORDER|LVCF_FMT;
			item.cchTextMax =60;
			item.pszText = 0;
			item.iSubItem = 0;
			SendMessage(GetDlgItem(hWnd,ExpertSystemChildBtns[i].nID),LVM_SETITEM,0,(LPARAM)&item);
		}
	}

	//����ListView���и�Ϊ50
	// Create the ImageList
	hSmall = ImageList_Create(1,29,ILC_MASK, 1, 1);
// Add two icons to ImageList
	hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	ImageList_AddIcon(hSmall, hIcon);
	DestroyIcon(hIcon);
// Assign ImageList to List View
	ListView_SetImageList(GetDlgItem(hWnd,IDC_EXPERT0), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_SIMPLE), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_HAVE_BOTTOM), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_NOTHAVE_BOTTOM), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_ANGULAR), hSmall, LVSIL_STATE);

	ListView_SetImageList(GetDlgItem(hWnd,IDC_LINE_DISTRIBUTE), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_ARC_DISTRIBUTE), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_DISPERSE_DISTRIBUTE), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_COORDINATE_DISTRIBUTE), hSmall, LVSIL_STATE);

	ListView_SetImageList(GetDlgItem(hWnd,IDC_MEASURE1), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_MEASURE2), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_MEASURE3), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_MEASURE4), hSmall, LVSIL_STATE);

	ListView_SetImageList(GetDlgItem(hWnd,IDC_AUXILIARY_COORDINATE), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_AUXILIARY_ATC), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_AUXILIARY_GMODE), hSmall, LVSIL_STATE);
	ListView_SetImageList(GetDlgItem(hWnd,IDC_AUXILIARY_MMODE), hSmall, LVSIL_STATE);


	//2010-6-24
	EXPERT=0;
	pDataInEXPERT = (LPCmdThreadParam)HeapAlloc(GetProcessHeap(),
											HEAP_ZERO_MEMORY,
											sizeof(CmdThreadParam)
											);
	if(pDataInEXPERT == NULL)
	{
		return 1;
	}

	if(hBitmap_ExpertSystem==0)
	{
		hBitmap_ExpertSystem = (HBITMAP)LoadImage(hInst,MAKEINTRESOURCE(IDB_BITMAP25),IMAGE_BITMAP,0,0,0);
	}
	if(hBitmap_ExpertSystem1==0)
	{
		hBitmap_ExpertSystem1 = (HBITMAP)LoadImage(hInst,MAKEINTRESOURCE(IDB_BITMAP26),IMAGE_BITMAP,0,0,0);
	}
	if(hBitmap_ExpertSystem2==0)
	{
		hBitmap_ExpertSystem2 = (HBITMAP)LoadImage(hInst,MAKEINTRESOURCE(IDB_BITMAP27),IMAGE_BITMAP,0,0,0);
	}
	if(hBitmap_ExpertSystem3==0)
	{
		hBitmap_ExpertSystem3 = (HBITMAP)LoadImage(hInst,MAKEINTRESOURCE(IDB_BITMAP28),IMAGE_BITMAP,0,0,0);
	}
	if(hBitmap_ExpertSystem4==0)
	{
		hBitmap_ExpertSystem4 = (HBITMAP)LoadImage(hInst,MAKEINTRESOURCE(IDB_BITMAP29),IMAGE_BITMAP,0,0,0);
	}
	if(hBitmap_ExpertSystem5==0)
	{
		hBitmap_ExpertSystem5 = (HBITMAP)LoadImage(hInst,MAKEINTRESOURCE(IDB_BITMAP30),IMAGE_BITMAP,0,0,0);
	}
	if(hBitmap_ExpertSystem6==0)
	{
		hBitmap_ExpertSystem6 = (HBITMAP)LoadImage(hInst,MAKEINTRESOURCE(IDB_BITMAP31),IMAGE_BITMAP,0,0,0);
	}

	ShowWindow(GetDlgItem(hWnd,IDC_SIMPLE),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_HAVE_BOTTOM),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_NOTHAVE_BOTTOM),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_ANGULAR),SW_HIDE);

	ShowWindow(GetDlgItem(hWnd,IDC_LINE_DISTRIBUTE),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_ARC_DISTRIBUTE),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_DISPERSE_DISTRIBUTE),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_COORDINATE_DISTRIBUTE),SW_HIDE);

	ShowWindow(GetDlgItem(hWnd,IDC_MEASURE1),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_MEASURE2),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_MEASURE3),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_MEASURE4),SW_HIDE);

	ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_COORDINATE),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_ATC),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_GMODE),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_MMODE),SW_HIDE);

	UpdateWindow(GetDlgItem(GetParent(hWnd),IDC_EXPERT_SYSTEM));

	return 0;

}

LRESULT goCommandExpertSystemChild(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		//����˵�ID�ź�֪ͨ��
		int menuID, notifyCode;
		
		//���崰�ھ��
		HWND wndCtrl;
		 int i;

		 
		menuID    = LOWORD(wParam); 
		notifyCode = HIWORD(wParam); 
		wndCtrl = (HWND) lParam;
	   

		
		for(i=0; i<dim(ExpertCommands);i++)
		{
			if(menuID == ExpertCommands[i].uCode) return(*ExpertCommands[i].functionName)(hWnd,wndCtrl,menuID,notifyCode);
		}
			
		return 0;
}

LRESULT goPaintExpertSystemChild(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HBRUSH hOldBrush;
	RECT rect;
	BITMAP bmp;
	HDC hdc,hdcMem;
	GetClientRect(hWnd,&rect);
	SetRect(&rect,724,55,1023,rect.bottom);
	hdc=BeginPaint(hWnd,&ps);
	hdcMem=CreateCompatibleDC(hdc);
	hOldBrush=(HBRUSH)SelectObject(hdcMem,hBitmap_ExpertSystem);
	GetObject(hBitmap_ExpertSystem,sizeof(BITMAP),&bmp);
	BitBlt(hdc,rect.left,rect.top,bmp.bmWidth,bmp.bmHeight,hdcMem,0,0,SRCCOPY);
	SelectObject(hdcMem,hOldBrush);
	DeleteDC(hdcMem);
	SetBkMode(hdc,TRANSPARENT);
	SetTextColor(hdc,RGB(0,0,0));
		
	return 0;

}

LRESULT goDestroyExpertSystemChild(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		PostQuitMessage(0);

		return 0;
}

LRESULT goListExpertSystemChild(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
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
	int line;
	//HDC hdc,mdc,bufdc;
	//HBITMAP hbg,emsmem;
PAINTSTRUCT ps;
	HBRUSH hOldBrush;
	RECT rect;
	BITMAP bmp;
	HDC hdc,hdcMem;

	menuID    = int(wParam); 
	pnmh = (LPNMHDR)lParam;
	wndCtrl = pnmh->hwndFrom;

	if(menuID==IDC_EXPERT0)//��������Ŀ����������������ListView��������
	{		

		if(pnmh->code==NM_CLICK)
		{
			nSelItem = -1;
			nSelItem =  ListView_GetNextItem(wndCtrl,-1,LVNI_SELECTED);
			for(i=0;i<5;i++)
			ListView_SetItemState(GetDlgItem(hWnd,IDC_EXPERT0+i),-1,LVIS_SELECTED, 0XFF);
			if (nSelItem >= 0 )
			{
				ListView_SetItemState(wndCtrl,nSelItem,LVIS_DROPHILITED, 0XFF);
				//������һ���߳������޸Ĳ�����ֵ
				SetFocus(GetDlgItem(GetParent(hWnd),IDC_MDIEDIT));
				DSP_data.Expert_Mode_Flag = nSelItem;
				switch(nSelItem)
				{
				case 0:
						ShowWindow(GetDlgItem(hWnd,IDC_SIMPLE),SW_SHOW);
						ShowWindow(GetDlgItem(hWnd,IDC_HAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_NOTHAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ANGULAR),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_LINE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ARC_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_DISPERSE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_COORDINATE_DISTRIBUTE),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE1),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE2),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE3),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE4),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_COORDINATE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_ATC),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_GMODE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_MMODE),SW_HIDE);

						GetClientRect(hWnd,&rect);
						SetRect(&rect,724,55,1023,rect.bottom);
						hdc=GetDC(hWnd);
						hdcMem=CreateCompatibleDC(hdc);
						hOldBrush=(HBRUSH)SelectObject(hdcMem,hBitmap_ExpertSystem1);
						GetObject(hBitmap_ExpertSystem,sizeof(BITMAP),&bmp);
						BitBlt(hdc,rect.left,rect.top,bmp.bmWidth,bmp.bmHeight,hdcMem,0,0,SRCCOPY);
						SelectObject(hdcMem,hOldBrush);
						DeleteDC(hdcMem);
						break;
				case 1:
						ShowWindow(GetDlgItem(hWnd,IDC_SIMPLE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_HAVE_BOTTOM),SW_SHOW);
						ShowWindow(GetDlgItem(hWnd,IDC_NOTHAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ANGULAR),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_LINE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ARC_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_DISPERSE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_COORDINATE_DISTRIBUTE),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE1),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE2),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE3),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE4),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_COORDINATE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_ATC),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_GMODE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_MMODE),SW_HIDE);

						GetClientRect(hWnd,&rect);
						SetRect(&rect,724,55,1023,rect.bottom);
						hdc=GetDC(hWnd);
						hdcMem=CreateCompatibleDC(hdc);
						hOldBrush=(HBRUSH)SelectObject(hdcMem,hBitmap_ExpertSystem2);
						GetObject(hBitmap_ExpertSystem,sizeof(BITMAP),&bmp);
						BitBlt(hdc,rect.left,rect.top,bmp.bmWidth,bmp.bmHeight,hdcMem,0,0,SRCCOPY);
						SelectObject(hdcMem,hOldBrush);
						DeleteDC(hdcMem);
						break;
				case 2:
						ShowWindow(GetDlgItem(hWnd,IDC_SIMPLE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_HAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_NOTHAVE_BOTTOM),SW_SHOW);
						ShowWindow(GetDlgItem(hWnd,IDC_ANGULAR),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_LINE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ARC_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_DISPERSE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_COORDINATE_DISTRIBUTE),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE1),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE2),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE3),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE4),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_COORDINATE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_ATC),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_GMODE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_MMODE),SW_HIDE);

						GetClientRect(hWnd,&rect);
						SetRect(&rect,724,55,1023,rect.bottom);
						hdc=GetDC(hWnd);
						hdcMem=CreateCompatibleDC(hdc);
						hOldBrush=(HBRUSH)SelectObject(hdcMem,hBitmap_ExpertSystem3);
						GetObject(hBitmap_ExpertSystem,sizeof(BITMAP),&bmp);
						BitBlt(hdc,rect.left,rect.top,bmp.bmWidth,bmp.bmHeight,hdcMem,0,0,SRCCOPY);
						SelectObject(hdcMem,hOldBrush);
						DeleteDC(hdcMem);
						break;
				case 3:
						ShowWindow(GetDlgItem(hWnd,IDC_SIMPLE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_HAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_NOTHAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ANGULAR),SW_SHOW);

						ShowWindow(GetDlgItem(hWnd,IDC_LINE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ARC_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_DISPERSE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_COORDINATE_DISTRIBUTE),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE1),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE2),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE3),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE4),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_COORDINATE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_ATC),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_GMODE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_MMODE),SW_HIDE);

						GetClientRect(hWnd,&rect);
						SetRect(&rect,724,55,1023,rect.bottom);
						hdc=GetDC(hWnd);
						hdcMem=CreateCompatibleDC(hdc);
						hOldBrush=(HBRUSH)SelectObject(hdcMem,hBitmap_ExpertSystem4);
						GetObject(hBitmap_ExpertSystem,sizeof(BITMAP),&bmp);
						BitBlt(hdc,rect.left,rect.top,bmp.bmWidth,bmp.bmHeight,hdcMem,0,0,SRCCOPY);
						SelectObject(hdcMem,hOldBrush);
						DeleteDC(hdcMem);
						break;

				case 4:
						ShowWindow(GetDlgItem(hWnd,IDC_SIMPLE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_HAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_NOTHAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ANGULAR),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_LINE_DISTRIBUTE),SW_SHOW);
						ShowWindow(GetDlgItem(hWnd,IDC_ARC_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_DISPERSE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_COORDINATE_DISTRIBUTE),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE1),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE2),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE3),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE4),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_COORDINATE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_ATC),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_GMODE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_MMODE),SW_HIDE);

						GetClientRect(hWnd,&rect);
						SetRect(&rect,724,55,1023,rect.bottom);
						hdc=GetDC(hWnd);
						hdcMem=CreateCompatibleDC(hdc);
						hOldBrush=(HBRUSH)SelectObject(hdcMem,hBitmap_ExpertSystem5);
						GetObject(hBitmap_ExpertSystem,sizeof(BITMAP),&bmp);
						BitBlt(hdc,rect.left,rect.top,bmp.bmWidth,bmp.bmHeight,hdcMem,0,0,SRCCOPY);
						SelectObject(hdcMem,hOldBrush);
						DeleteDC(hdcMem);
						break;

				case 5:
						ShowWindow(GetDlgItem(hWnd,IDC_SIMPLE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_HAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_NOTHAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ANGULAR),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_LINE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ARC_DISTRIBUTE),SW_SHOW);
						ShowWindow(GetDlgItem(hWnd,IDC_DISPERSE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_COORDINATE_DISTRIBUTE),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE1),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE2),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE3),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE4),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_COORDINATE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_ATC),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_GMODE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_MMODE),SW_HIDE);

						GetClientRect(hWnd,&rect);
						SetRect(&rect,724,55,1023,rect.bottom);
						hdc=GetDC(hWnd);
						hdcMem=CreateCompatibleDC(hdc);
						hOldBrush=(HBRUSH)SelectObject(hdcMem,hBitmap_ExpertSystem6);
						GetObject(hBitmap_ExpertSystem,sizeof(BITMAP),&bmp);
						BitBlt(hdc,rect.left,rect.top,bmp.bmWidth,bmp.bmHeight,hdcMem,0,0,SRCCOPY);
						SelectObject(hdcMem,hOldBrush);
						DeleteDC(hdcMem);
						break;

				case 6:
						ShowWindow(GetDlgItem(hWnd,IDC_SIMPLE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_HAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_NOTHAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ANGULAR),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_LINE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ARC_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_DISPERSE_DISTRIBUTE),SW_SHOW);
						ShowWindow(GetDlgItem(hWnd,IDC_COORDINATE_DISTRIBUTE),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE1),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE2),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE3),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE4),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_COORDINATE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_ATC),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_GMODE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_MMODE),SW_HIDE);

						GetClientRect(hWnd,&rect);
						SetRect(&rect,724,55,1023,rect.bottom);
						hdc=GetDC(hWnd);
						hdcMem=CreateCompatibleDC(hdc);
						hOldBrush=(HBRUSH)SelectObject(hdcMem,hBitmap_ExpertSystem);
						GetObject(hBitmap_ExpertSystem,sizeof(BITMAP),&bmp);
						BitBlt(hdc,rect.left,rect.top,bmp.bmWidth,bmp.bmHeight,hdcMem,0,0,SRCCOPY);
						SelectObject(hdcMem,hOldBrush);
						DeleteDC(hdcMem);
						break;

				case 7:
						ShowWindow(GetDlgItem(hWnd,IDC_SIMPLE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_HAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_NOTHAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ANGULAR),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_LINE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ARC_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_DISPERSE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_COORDINATE_DISTRIBUTE),SW_SHOW);

						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE1),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE2),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE3),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE4),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_COORDINATE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_ATC),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_GMODE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_MMODE),SW_HIDE);

						GetClientRect(hWnd,&rect);
						SetRect(&rect,724,55,1023,rect.bottom);
						hdc=GetDC(hWnd);
						hdcMem=CreateCompatibleDC(hdc);
						hOldBrush=(HBRUSH)SelectObject(hdcMem,hBitmap_ExpertSystem);
						GetObject(hBitmap_ExpertSystem,sizeof(BITMAP),&bmp);
						BitBlt(hdc,rect.left,rect.top,bmp.bmWidth,bmp.bmHeight,hdcMem,0,0,SRCCOPY);
						SelectObject(hdcMem,hOldBrush);
						DeleteDC(hdcMem);
						break;

				case 8:
						ShowWindow(GetDlgItem(hWnd,IDC_SIMPLE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_HAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_NOTHAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ANGULAR),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_LINE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ARC_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_DISPERSE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_COORDINATE_DISTRIBUTE),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE1),SW_SHOW);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE2),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE3),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE4),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_COORDINATE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_ATC),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_GMODE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_MMODE),SW_HIDE);

						GetClientRect(hWnd,&rect);
						SetRect(&rect,724,55,1023,rect.bottom);
						hdc=GetDC(hWnd);
						hdcMem=CreateCompatibleDC(hdc);
						hOldBrush=(HBRUSH)SelectObject(hdcMem,hBitmap_ExpertSystem);
						GetObject(hBitmap_ExpertSystem,sizeof(BITMAP),&bmp);
						BitBlt(hdc,rect.left,rect.top,bmp.bmWidth,bmp.bmHeight,hdcMem,0,0,SRCCOPY);
						SelectObject(hdcMem,hOldBrush);
						DeleteDC(hdcMem);
						break;

				case 9:
						ShowWindow(GetDlgItem(hWnd,IDC_SIMPLE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_HAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_NOTHAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ANGULAR),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_LINE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ARC_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_DISPERSE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_COORDINATE_DISTRIBUTE),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE1),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE2),SW_SHOW);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE3),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE4),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_COORDINATE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_ATC),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_GMODE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_MMODE),SW_HIDE);

						GetClientRect(hWnd,&rect);
						SetRect(&rect,724,55,1023,rect.bottom);
						hdc=GetDC(hWnd);
						hdcMem=CreateCompatibleDC(hdc);
						hOldBrush=(HBRUSH)SelectObject(hdcMem,hBitmap_ExpertSystem);
						GetObject(hBitmap_ExpertSystem,sizeof(BITMAP),&bmp);
						BitBlt(hdc,rect.left,rect.top,bmp.bmWidth,bmp.bmHeight,hdcMem,0,0,SRCCOPY);
						SelectObject(hdcMem,hOldBrush);
						DeleteDC(hdcMem);
						break;

				case 10:
						ShowWindow(GetDlgItem(hWnd,IDC_SIMPLE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_HAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_NOTHAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ANGULAR),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_LINE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ARC_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_DISPERSE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_COORDINATE_DISTRIBUTE),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE1),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE2),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE3),SW_SHOW);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE4),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_COORDINATE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_ATC),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_GMODE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_MMODE),SW_HIDE);

						GetClientRect(hWnd,&rect);
						SetRect(&rect,724,55,1023,rect.bottom);
						hdc=GetDC(hWnd);
						hdcMem=CreateCompatibleDC(hdc);
						hOldBrush=(HBRUSH)SelectObject(hdcMem,hBitmap_ExpertSystem);
						GetObject(hBitmap_ExpertSystem,sizeof(BITMAP),&bmp);
						BitBlt(hdc,rect.left,rect.top,bmp.bmWidth,bmp.bmHeight,hdcMem,0,0,SRCCOPY);
						SelectObject(hdcMem,hOldBrush);
						DeleteDC(hdcMem);
						break;

				case 11:
						ShowWindow(GetDlgItem(hWnd,IDC_SIMPLE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_HAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_NOTHAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ANGULAR),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_LINE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ARC_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_DISPERSE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_COORDINATE_DISTRIBUTE),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE1),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE2),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE3),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE4),SW_SHOW);

						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_COORDINATE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_ATC),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_GMODE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_MMODE),SW_HIDE);

						GetClientRect(hWnd,&rect);
						SetRect(&rect,724,55,1023,rect.bottom);
						hdc=GetDC(hWnd);
						hdcMem=CreateCompatibleDC(hdc);
						hOldBrush=(HBRUSH)SelectObject(hdcMem,hBitmap_ExpertSystem);
						GetObject(hBitmap_ExpertSystem,sizeof(BITMAP),&bmp);
						BitBlt(hdc,rect.left,rect.top,bmp.bmWidth,bmp.bmHeight,hdcMem,0,0,SRCCOPY);
						SelectObject(hdcMem,hOldBrush);
						DeleteDC(hdcMem);
						break;
				case 12:
						ShowWindow(GetDlgItem(hWnd,IDC_SIMPLE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_HAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_NOTHAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ANGULAR),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_LINE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ARC_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_DISPERSE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_COORDINATE_DISTRIBUTE),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE1),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE2),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE3),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE4),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_COORDINATE),SW_SHOW);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_ATC),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_GMODE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_MMODE),SW_HIDE);

						GetClientRect(hWnd,&rect);
						SetRect(&rect,724,55,1023,rect.bottom);
						hdc=GetDC(hWnd);
						hdcMem=CreateCompatibleDC(hdc);
						hOldBrush=(HBRUSH)SelectObject(hdcMem,hBitmap_ExpertSystem);
						GetObject(hBitmap_ExpertSystem,sizeof(BITMAP),&bmp);
						BitBlt(hdc,rect.left,rect.top,bmp.bmWidth,bmp.bmHeight,hdcMem,0,0,SRCCOPY);
						SelectObject(hdcMem,hOldBrush);
						DeleteDC(hdcMem);
						break;

				case 13:
						ShowWindow(GetDlgItem(hWnd,IDC_SIMPLE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_HAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_NOTHAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ANGULAR),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_LINE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ARC_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_DISPERSE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_COORDINATE_DISTRIBUTE),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE1),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE2),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE3),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE4),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_COORDINATE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_ATC),SW_SHOW);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_GMODE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_MMODE),SW_HIDE);

						GetClientRect(hWnd,&rect);
						SetRect(&rect,724,55,1023,rect.bottom);
						hdc=GetDC(hWnd);
						hdcMem=CreateCompatibleDC(hdc);
						hOldBrush=(HBRUSH)SelectObject(hdcMem,hBitmap_ExpertSystem);
						GetObject(hBitmap_ExpertSystem,sizeof(BITMAP),&bmp);
						BitBlt(hdc,rect.left,rect.top,bmp.bmWidth,bmp.bmHeight,hdcMem,0,0,SRCCOPY);
						SelectObject(hdcMem,hOldBrush);
						DeleteDC(hdcMem);
						break;

				case 14:
						ShowWindow(GetDlgItem(hWnd,IDC_SIMPLE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_HAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_NOTHAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ANGULAR),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_LINE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ARC_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_DISPERSE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_COORDINATE_DISTRIBUTE),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE1),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE2),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE3),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE4),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_COORDINATE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_ATC),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_GMODE),SW_SHOW);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_MMODE),SW_HIDE);

						GetClientRect(hWnd,&rect);
						SetRect(&rect,724,55,1023,rect.bottom);
						hdc=GetDC(hWnd);
						hdcMem=CreateCompatibleDC(hdc);
						hOldBrush=(HBRUSH)SelectObject(hdcMem,hBitmap_ExpertSystem);
						GetObject(hBitmap_ExpertSystem,sizeof(BITMAP),&bmp);
						BitBlt(hdc,rect.left,rect.top,bmp.bmWidth,bmp.bmHeight,hdcMem,0,0,SRCCOPY);
						SelectObject(hdcMem,hOldBrush);
						DeleteDC(hdcMem);
						break;

				case 15:
						ShowWindow(GetDlgItem(hWnd,IDC_SIMPLE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_HAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_NOTHAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ANGULAR),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_LINE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ARC_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_DISPERSE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_COORDINATE_DISTRIBUTE),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE1),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE2),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE3),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE4),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_COORDINATE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_ATC),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_GMODE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_MMODE),SW_SHOW);

						GetClientRect(hWnd,&rect);
						SetRect(&rect,724,55,1023,rect.bottom);
						hdc=GetDC(hWnd);
						hdcMem=CreateCompatibleDC(hdc);
						hOldBrush=(HBRUSH)SelectObject(hdcMem,hBitmap_ExpertSystem);
						GetObject(hBitmap_ExpertSystem,sizeof(BITMAP),&bmp);
						BitBlt(hdc,rect.left,rect.top,bmp.bmWidth,bmp.bmHeight,hdcMem,0,0,SRCCOPY);
						SelectObject(hdcMem,hOldBrush);
						DeleteDC(hdcMem);
						break;

				default:
						ShowWindow(GetDlgItem(hWnd,IDC_SIMPLE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_HAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_NOTHAVE_BOTTOM),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ANGULAR),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_LINE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_ARC_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_DISPERSE_DISTRIBUTE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_COORDINATE_DISTRIBUTE),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE1),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE2),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE3),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_MEASURE4),SW_HIDE);

						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_COORDINATE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_ATC),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_GMODE),SW_HIDE);
						ShowWindow(GetDlgItem(hWnd,IDC_AUXILIARY_MMODE),SW_HIDE);
						break;

				}

				EXPERT=1;

			}

		}
	}
	else
	{
		switch(menuID)
		{
		case IDC_SIMPLE:
				if(pnmh->code==NM_CLICK)
				{
					nSelItem =-1;
					nSelItem =  ListView_GetNextItem(wndCtrl,-1,LVNI_SELECTED);
					line=menuID-IDC_EXPERT0;
					//ȥ������
					for(i=0;i<1;i++)
					ListView_SetItemState(GetDlgItem(hWnd,menuID),-1,LVIS_SELECTED, 0XFF);
					if (nSelItem >= 0 )
					{
						ListView_SetItemState(wndCtrl,nSelItem,LVIS_DROPHILITED, 0XFF);
						//������һ���߳������޸Ĳ�����ֵ
						SetFocus(GetDlgItem(GetParent(hWnd),IDC_MDIEDIT));
						pDataInEXPERT->hWnd = hWnd;
						pDataInEXPERT->wndCtrl = wndCtrl;
						pDataInEXPERT->menuID = line;
						pDataInEXPERT->notifyCode = 0;
						EXPERT=0;

					}
				}
				break;

		case IDC_HAVE_BOTTOM:
				if(pnmh->code==NM_CLICK)
				{
					nSelItem =-1;
					nSelItem =  ListView_GetNextItem(wndCtrl,-1,LVNI_SELECTED);
					line=menuID-IDC_EXPERT0;
					//ȥ������
					for(i=0;i<1;i++)
					ListView_SetItemState(GetDlgItem(hWnd,menuID),-1,LVIS_SELECTED, 0XFF);
					if (nSelItem >= 0 )
					{
						ListView_SetItemState(wndCtrl,nSelItem,LVIS_DROPHILITED, 0XFF);
						//������һ���߳������޸Ĳ�����ֵ
						SetFocus(GetDlgItem(GetParent(hWnd),IDC_MDIEDIT));
						pDataInEXPERT->hWnd = hWnd;
						pDataInEXPERT->wndCtrl = wndCtrl;
						pDataInEXPERT->menuID = line;
						pDataInEXPERT->notifyCode = 0;
						EXPERT=1;

					}
				}
				break;

		case IDC_NOTHAVE_BOTTOM:
				if(pnmh->code==NM_CLICK)
				{
					nSelItem =-1;
					nSelItem =  ListView_GetNextItem(wndCtrl,-1,LVNI_SELECTED);
					line=menuID-IDC_EXPERT0;
					//ȥ������
					for(i=0;i<1;i++)
					ListView_SetItemState(GetDlgItem(hWnd,menuID),-1,LVIS_SELECTED, 0XFF);
					if (nSelItem >= 0 )
					{
						ListView_SetItemState(wndCtrl,nSelItem,LVIS_DROPHILITED, 0XFF);
						//������һ���߳������޸Ĳ�����ֵ
						SetFocus(GetDlgItem(GetParent(hWnd),IDC_MDIEDIT));
						pDataInEXPERT->hWnd = hWnd;
						pDataInEXPERT->wndCtrl = wndCtrl;
						pDataInEXPERT->menuID = line;
						pDataInEXPERT->notifyCode = 0;
						EXPERT=2;

					}
				}
				break;

		case IDC_ANGULAR:
				if(pnmh->code==NM_CLICK)
				{
					nSelItem =-1;
					nSelItem =  ListView_GetNextItem(wndCtrl,-1,LVNI_SELECTED);
					line=menuID-IDC_EXPERT0;
					//ȥ������
					for(i=0;i<1;i++)
					ListView_SetItemState(GetDlgItem(hWnd,menuID),-1,LVIS_SELECTED, 0XFF);
					if (nSelItem >= 0 )
					{
						ListView_SetItemState(wndCtrl,nSelItem,LVIS_DROPHILITED, 0XFF);
						//������һ���߳������޸Ĳ�����ֵ
						SetFocus(GetDlgItem(GetParent(hWnd),IDC_MDIEDIT));
						pDataInEXPERT->hWnd = hWnd;
						pDataInEXPERT->wndCtrl = wndCtrl;
						pDataInEXPERT->menuID = line;
						pDataInEXPERT->notifyCode = 0;
						EXPERT=3;

					}
				}
				break;

		default:
				break;
		}

	
	}
	return 0;


}
LRESULT goExpertReturn(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	
	ShowWindow(GetDlgItem(GetParent(GetParent(hWnd)),IDC_STATUSDISP),SW_SHOW);
	ShowWindow(GetDlgItem(GetParent(hWnd),IDC_EXPERTChild),SW_HIDE);
	ShowWindow(GetDlgItem(GetParent(hWnd),IDW_COORL),SW_HIDE);
	ShowWindow(GetDlgItem(GetParent(hWnd),IDC_MDILIST),SW_SHOW);

	ShowWindow(GetDlgItem(GetParent(hWnd),IDC_MDIBTNBUILD),SW_SHOW);
	ShowWindow(GetDlgItem(GetParent(hWnd),IDC_MDIBTNCLR),SW_SHOW);
	ShowWindow(GetDlgItem(GetParent(hWnd),IDC_MDIBTCOOR),SW_SHOW);
	ShowWindow(GetDlgItem(GetParent(hWnd),IDC_MDIBTEXPERT),SW_SHOW);
//	Coordinate_Flag = 0;
	ExpertSystem_Flag = 0;

	return 0;
}

LRESULT goExpertCodeGenerate(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	char ptext[_MAX_FNAME] = "M17;G91G18G01";
	char ptext1[20] = ";M30;";
	char chFullName[_MAX_FNAME] = "O0.txt";
	TCHAR szBuffer1[100];
	TCHAR szBuffer2[40];
	FILE *file;
	long reallength=0;


	if(EXPERT == 0)
	{
			//ultoa(DSP_data.PARA_Y,szBuffer2,10);
			sprintf_s(szBuffer2, "%4.3f", (double)DSP_data.PARA_Y/1000);

			switch(DSP_data.PARA_X)
			{
			case 1:
				strcpy(szBuffer1,"X");
				strcat(szBuffer1,szBuffer2);
				break;

			case 2:
				strcpy(szBuffer1,"Y");
				strcat(szBuffer1,szBuffer2);
				break;

			case 3:
				strcpy(szBuffer1,"Z");
				strcat(szBuffer1,"-");
				strcat(szBuffer1,szBuffer2);
				break;
			}

			strcat(ptext,szBuffer1);
			strcat(ptext,ptext1);

			SendMessage(GetDlgItem(hWnd,IDC_EXPERTLISTBOX),LB_INSERTSTRING,0,(LPARAM)ptext);

			file = fopen (chFullName, "w+");
			reallength = strlen(ptext);
			fwrite(ptext, 1, reallength, file);
			fclose (file);


	}
	else
	{
		if(EXPERT == 0)
		{
		
		}
		sprintf_s(szBuffer2, "%4.3f", (double)DSP_data.PARA_X/1000);
		//ultoa(DSP_data.PARA_X,szBuffer2,10);
		strcpy(szBuffer1,"X");
		strcat(szBuffer1,szBuffer2);

		sprintf_s(szBuffer2, "%4.3f", (double)DSP_data.PARA_Y/1000);
		//ultoa(DSP_data.PARA_Y,szBuffer2,10);
		strcat(szBuffer1,"Y");
		strcat(szBuffer1,szBuffer2);

		sprintf_s(szBuffer2, "%4.3f", (double)DSP_data.PARA_Z/1000);
		//ultoa(DSP_data.PARA_Z,szBuffer2,10);
		strcat(szBuffer1,"Z");
		strcat(szBuffer1,szBuffer2);

		strcat(ptext,szBuffer1);
		strcat(ptext,ptext1);

		SendMessage(GetDlgItem(hWnd,IDC_EXPERTLISTBOX),LB_INSERTSTRING,0,(LPARAM)ptext);
//		UpdateWindow(GetDlgItem(hWnd,IDC_EXPERTLISTBOX));

		file = fopen (chFullName, "w+");
		reallength = strlen(ptext);
		fwrite(ptext, 1, reallength, file);
		fclose (file);
	}


	return 0;
}