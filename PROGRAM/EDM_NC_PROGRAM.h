#ifndef EDM_NC_PROGRAM_H
#define EDM_NC_PROGRAM_H

#include "resource.h"

//#include "E:\NC_PROGRAM\experiment\new_Dsp_downtime2\Dsp_nc_Beta1.3\nc_address.h"
#include "..\nc_address.h"	//sun改
//#include<math.h>
#include<iostream>
#include "IOGuid.h"
#include "IOIoctls.h"
#include "NCGuid.h"
#include "NCIoctls.h"
#include "PowerGuid.h"
#include "PowerIoctls.h"
#include "code.h"
#include "stdlib.h"
#define MAX_LOADSTRING 100
#define MAX_FILELEN    5000000
#define DECODE_NUM_ONCE 200
#define MAX_NC_MEM 200000
#define SENDNCNUM 6


/*手运行速度值 */
#define SPEED_RAPID 5000    //10mm/s
#define SPEED_HIGH 1000    //5mm/s
#define SPEED_MIDDLE 200    //2.5mm/s
#define SPEED_LOW 3		//1000 0.97mm/s
#define SPEED_STEP10 300    //
#define SPEED_STEP1 30    //


#define dim(x) (sizeof(x)/sizeof(x[0]))



//定义消息ID号与消息处理过程的结构体
typedef struct{
	UINT uCode;
	LRESULT (*functionName)(HWND,UINT,WPARAM,LPARAM);
}MessageProc;

//定义菜单或控件的命令ID号与命令处理过程的结构体
typedef struct{
	UINT uCode;
	LRESULT (*functionName)(HWND,HWND,int,int);
}CommandProc;

typedef struct{
	DWORD dwExStyle;
	TCHAR *szClass;
	int nID;
	TCHAR *szTitle;
	int x;
	int y;
	int cx;
	int cy;
	DWORD lStyle;
} CtlWndStruct;
typedef struct _FILEINFO
{
    char filename[_MAX_FNAME];
    int  filesize;
    int  accessmode;
    SYSTEMTIME modifytime;
    BOOL IsDir;

} FILEINFO; 
typedef FILEINFO* PFILEINFO;

typedef struct _NoteInfo
{
    BOOL ischanged;
    char fileName[_MAX_FNAME+1];
    char filePath[MAX_PATH+1];
    char fileFullName[_MAX_FNAME+MAX_PATH+1];
    long fileSize;
    char* Buffer;
    HWND hMLEditWnd;

    int rows, cols;     // row and column of window.
}NOTEINFO;
typedef NOTEINFO* PNOTEINFO;

struct _nc_data {
    int row_id;     //行号
	int row_id_All;
    int G_pathmode;     //路径插补模式G00(G0)=0 G01(G1)=1 G02(G2)=2 G03(G3)=3
	int G_Coordinate;
	int G_pathmode_upper;
	int G_pathmode_lower;
	int numbc,numxy;
	double Deltl1,Deltl2;
    double X , Y, Z, B, C;  //坐标
    double I, J, K, R;      //下平面圆坐标数据
	double I1,J1,K1,R1;    //上平面圆坐标数据
    
    int M;          //
    int S;          //
    double D;          // offset
	double T;          // 锥面度数
	double A;
    int G_origin;       //G92=1设定坐标原点, 0非设定坐标原点
    int G_coormode_9x;     //G90=90绝对 G91=91相对坐标模式
    int G_plane;        //坐标平面G17=17 G18=18 G19=19
    int G_compensate;       //刀具补偿G40, G41, G42
	int G_tapemode;      //锥面G50, G51, G52
	int G_Dcompensate;  //上下异形面G152,G153    //11
	int G_coormode_92;
	int G_coormode_5x;
	int G_taperCorner;
	} ;
typedef struct _nc_data nc_data, *LPNCDATA;

struct _M_data {

	int row_id_All;
    int	M;     //路径插补模式G00(G0)=0 G01(G1)=1 G02(G2)=2 G03(G3)=3

	} ;
typedef struct _M_data M_data, *LPMDATA;
struct _M98_data {

	int P;
    int	NUM; 
	double D; 
	int	T; 
	int N;
	int M;
	int O;
	} ;
typedef struct _M98_data M98_data, *LPM98DATA;

struct _nc_code{
	int row_id;
	char pText[255];
};
typedef struct _nc_code nc_code, *LPNCCODE;


struct _TapeParam{
	double Hworkthick;
	double Hreference;
	double Hlowdistance;
	double Hupperdistance;
};
typedef struct _TapeParam TapeParam, *LPTAPEPARAM;

typedef struct _Tape_data{
	double Tape_k1;
	double Tape_k2;
	double Tape_k3;
	double Tape_k4;	

} Tape_data;

struct _m_point{
	double X, Y;		//"绝对"坐标 
};
typedef struct _m_point m_point;


//坐标系结构体定义
typedef struct _Coorpuls{
	unsigned int x;
	unsigned int y;
	unsigned int z;
	unsigned int b;
	unsigned int c;
} Coorpuls;

typedef struct _Coordata{
	double x;
	double y;
	double z;
	double b;
	double c;
} Coordata;

typedef struct _DSP_Para_data{
	int PARA_X;
	int PARA_Y;
	int PARA_Z;
	int PARA_DownTime;
	int PARA_Jump_Height;
	int PARA_DownTime_Flag;
	int PARA_Sway_R;
	int PARA_Sway_End;
	int PARA_DownTime_Z_or_XYZ;
	int PARA_Sway_Switch_Flag;
	int PARA_Jump_Select_Center;
	int PARA_Jump_Vel_Max;
	int PARA_Sway_FreeOr3D;
	int PARA_Sway_Polygon_N;
	int PARA_Sway_F_Mode;
	int PARA_Jump_Average_Speed;
	int PARA_Offset;
	int Expert_Mode_Flag;
	int PARA_Sway_A;
} DSP_Para_data;


typedef struct _ConditionItem{
	char *name;
	int value;
	HANDLE hDevice;
	int ctrWord;
	int addr;
} ConditionItem;

typedef struct _ConditionStru{
	int eNo;
	ConditionItem item[31];
} ConditionStru;

typedef struct _CmdThreadParam
{
	HWND hWnd;
	HWND wndCtrl;
	int menuID;
	int notifyCode;
	LPNCDATA ncMem;
	HANDLE hMutex;
}CmdThreadParam,*LPCmdThreadParam;
typedef struct _IOstru{
		DWORD adr;
		DWORD val;
}IOstru, *LPIOstru; 
typedef struct _MEMstru{
		DWORD adr;
		DWORD val[16];
}MEMstru, *LPMEMstru; 
typedef struct _GraphParam{
				HWND hWnd;
				LPNCDATA pDataNcGraphMem;
				double draw_width;
				double draw_length;
				double mw;
				double ml;
				int create_num;
} GraphParam, *LPGraphParam;


const double PI = 3.1415926;
#define ANGLECONST  cos(145*PI/180)
//全局宏定义
#define IDC_BTNMANUEL      100   //主窗口中空间MANUAL的ID;
#define IDC_BTNAUTO    101   // AUTO
#define IDC_BTNEDIT    102   // EDIT
#define IDC_BTNMDI     103   // MDI
#define IDC_BTNREST 104
#define IDC_BTNLIM     105   //软限位使能与解除控件ID

#define IDC_StartStop  106
#define IDC_BTNCLOSE       107   // CLOSE

#define IDC_STATICALARM    130   //主窗口中编辑框的ID

#define IDC_TIMER          240   //定时器操作,定时检查各轴位移量是否超出软限位;
#define IDC_STIMER  110
#define IDC_TTIMER  111
#define IDC_FTIMER  112
#define IDC_MDI     113
#define IDC_MDTITLE 114

/*手动坐标调节对话框初始控件的ID值*/
#define IDC_STATICMI	195
#define IDC_STATICAM    196
#define IDC_STATICReZe    197
#define IDC_MANUAL    199
#define IDC_MTITLE    198


#define IDC_MachAllReturn 270
#define IDC_MachXReturn   271
#define IDC_MachYReturn   272
#define IDC_MachZReturn   273
#define IDC_MachBReturn   274
#define IDC_MachCReturn   275

#define IDC_WorkAllReturn 280
#define IDC_WorkXReturn   281
#define IDC_WorkYReturn   282
#define IDC_WorkZReturn   283
#define IDC_WorkBReturn   284
#define IDC_WorkCReturn   285

#define IDC_XTouchP  200
#define IDC_YTouchP  201
#define IDC_ZTouchP  202
#define IDC_BTouchP  203
#define IDC_CTouchP  204

#define IDC_XTouchM  210
#define IDC_YTouchM  211
#define IDC_ZTouchM  212
#define IDC_BTouchM  213
#define IDC_CTouchM  214





#define IDC_RADIOBTNRAPID     230
#define IDC_RADIOBTNHIGH     231
#define IDC_RADIOBTNMIDDLE   232
#define IDC_RADIOBTNLOW      233
#define IDC_RADIOBTNX10   234
#define IDC_RADIOBTNX1    235

#define IDC_BTNOK          251

#define IDC_AUTO	400
#define IDC_ATITLE	401
#define IDC_CHOSE       402
#define IDC_CLIK	403
#define IDC_RTGRAPT	404
#define IDC_RTGRAP	405
#define IDC_CONDT	406
#define IDC_BTNDRAWAN	407

#define IDC_VOL   411
#define IDC_TOUCH   412
#define IDC_VOLPRO   413
#define IDC_TOUCHPRO   414

#define IDC_CONDITION	415
#define IDC_INVALUE	416
#define IDC_BTNSURE	417
#define IDC_AAStart	418
#define IDC_AAStop	419
#define IDC_VALUNAME	420


#define IDC_MLEDIT         300
#define IDC_GRAPH    301
#define IDC_BTNDELET      302
#define IDC_BTNNEW       303
#define IDC_BTNOPEN      304
#define IDC_BTNSAVE      305
#define IDC_BTNFNAME      306
#define IDC_BTNDRAW      307
#define IDC_BTNEDITCLOSE 308
#define IDC_STATICEDIT         310
#define IDC_STATICEDIT1		316
#define IDC_EPATH	317
#define IDC_EUP		318

#define IDC_EDIT	311
#define IDC_ETITLE	312
#define IDC_TMLEDIT	313
#define IDC_TGRAPH	314
#define IDC_LISTVIEW	315
#define IDC_EFNAME	319
#define IDC_BTNYZ      321
#define IDC_BTNXY      322
#define IDC_BTNXZ      323
#define IDC_BTNGRAPHRESET 324
#define IDC_EDIT_BUP	325//2010-5-6
#define IDC_EDIT_BLEFT	326//2010-5-6
#define IDC_EDIT_BDOWN	327//2010-5-6
#define IDC_EDIT_BRIGHT	328//2010-5-6
#define IDC_EDIT_BZOOM_IN	329//2010-5-6
#define IDC_EDIT_BZOOM_OUT	330//2010-5-6
#define IDC_EDIT_3D_2D			332//2010-5-12
#define IDC_EDIT_B_TURN_UP	333//2010-5-12
#define IDC_EDIT_B_TURN_LEFT	334//2010-5-12
#define IDC_EDIT_B_TURN_DOWN	335//2010-5-12
#define IDC_EDIT_B_TURN_RIGHT	336//2010-5-12
#define IDC_EDIT_EZOOM			337//2010-5-12
#define IDC_EDIT_E_MOVE			338//2010-5-12
#define IDC_EDIT_E_TURN			339//2010-5-12
#define IDC_GRAPH_2D    340//2010-5-12
#define IDC_EDIT_EDIT	341//2010-5-12
#define IDC_EDIT_E_NUMBER	342//2010-5-13
#define IDC_EDIT_E_MIN	343//2010-5-13
#define IDC_EDIT_E_MAX	344//2010-5-13

#define IDC_STATUSDISP  500
#define IDC_AXISX       501
#define IDC_AXISY       502
#define IDC_AXISZ       503
#define IDC_AXISB       504
#define IDC_AXISC       505
#define IDC_MACHCOORD   510
#define IDC_RELATACOORD 511
#define IDC_WORKCOORD   512

#define IDC_AXISALLZERO 540
#define IDC_AXISXZERO   541
#define IDC_AXISYZERO   542
#define IDC_AXISZZERO   543
#define IDC_AXISBZERO   544
#define IDC_AXISCZERO   545
#define IDC_PROGRESSBAR1 546
#define IDC_PROGRESSBAR2 547
#define IDC_OPENNAME  548
#define IDC_DISCHARGENNAME 549
#define IDC_BTNSTARTSTOP 550
#define IDC_BTNRESET 551
#define IDC_BTNSUSPEND 552
#define IDC_G5xFlagEDIT 553

#define IDC_STATUSVIEW  520
#define IDC_LISTCOORDVIEW 530

#define IDC_MDIEDIT  600
#define IDC_MDILIST  601
#define IDC_MDIBTNCLR 602
#define IDC_MDIBTNBUILD 603
#define IDC_MDIBTEXPERT 604
//2010-5-27
#define IDC_COORL0 610
#define IDC_COORL1 611
#define IDC_COORL2 612
#define IDC_COORL3 613
#define IDC_COORL4 614
#define IDC_COORL5 615
#define IDC_COORL6 616
//
#define IDW_COORL 617
#define IDC_MDIBTCOOR 618

#define IDC_AUTOPARAMEDIT 700
#define IDC_AUTOPARAMENO 701
#define IDC_AUTOPARAMENOIN 702
#define IDC_AUTOPARAMLIST1 703
#define IDC_AUTOPARAMLIST2 704
#define IDC_AUTOPARAMLIST3 705
#define IDC_AUTOPARAMLIST4 706//2010-5-6
#define IDC_AUTOPARAM  710
#define IDC_AUTOGRAPH  711
#define IDC_AUTONCODE  712
#define IDW_AUTOGRAPH_3D	713//2010-5-27

#define IDC_AUTONCCODELIST 720

#define		IDC_NCPRONAME    730
#define		IDC_NCPRONAMEIN  731
#define		IDC_CONDNO       732
#define		IDC_CONDNOIN     733
#define		IDC_SELCON       734
#define		IDC_SELSTATE     735
#define     IDC_BUILDMACHING 736
#define		IDC_BAUTO_3D_2D		737//2010-5-27
//2010-5-30
#define IDC_AUTO_BUP	738
#define IDC_AUTO_BLEFT	739
#define IDC_AUTO_BDOWN	740
#define IDC_AUTO_BRIGHT	741
#define IDC_AUTO_BZOOM_IN	742
#define IDC_AUTO_BZOOM_OUT	743
#define IDC_AUTO_B_TURN_UP	744
#define IDC_AUTO_B_TURN_LEFT	745
#define IDC_AUTO_B_TURN_DOWN	746
#define IDC_AUTO_B_TURN_RIGHT	747
#define IDC_AUTO_EZOOM			748
#define IDC_AUTO_E_MOVE			749
#define IDC_AUTO_E_TURN			750
//

#define IDC_STATICRETURNMODE 800
#define IDC_STATICSPEEDMODE 801

#define TIMER_STATUS   1000
#define TIMER_MANUAL   1001
#define TIMER_MAIN     1002
#define TIMER_AUTOGRAPH 1003
#define TIMER_AUTONCCODE 1004

#define IDC_EXPERT_CODE		1100
#define IDC_EXPERT_SYSTEM	1101
#define IDC_EXPERTChild		1102
#define IDC_EXPERTRETURN	1103
#define IDC_EXPERTEDIT		1104

#define IDC_EXPERT0			1105

#define IDC_EXPERTLISTBOX	1115
#define IDC_SIMPLE			1116
#define IDC_HAVE_BOTTOM		1117
#define IDC_NOTHAVE_BOTTOM	1118
#define IDC_ANGULAR			1119

#define IDC_LINE_DISTRIBUTE			1120
#define IDC_ARC_DISTRIBUTE			1121
#define IDC_DISPERSE_DISTRIBUTE			1122
#define IDC_COORDINATE_DISTRIBUTE			1123

#define IDC_MEASURE1			1134
#define IDC_MEASURE2			1135
#define IDC_MEASURE3			1136
#define IDC_MEASURE4			1137

#define IDC_AUXILIARY_COORDINATE			1148
#define IDC_AUXILIARY_ATC			1149
#define IDC_AUXILIARY_GMODE			1140
#define IDC_AUXILIARY_MMODE			1141



///*速度值 */
//#define SPEED_RAPID 20000    //10mm/s
//#define SPEED_HIGH 10000    //5mm/s
//#define SPEED_MIDDLE 4000    //2.5mm/s
//#define SPEED_LOW  1000    //0.97mm/s
//#define SPEED_STEP10 500    //
//#define SPEED_STEP1 200    //


// 此代码模块中包含的函数的前向声明:
int	MyRegisterClassMain(HINSTANCE hInstance);

int	showMainWnd(HINSTANCE hInstance, LPTSTR lpCmdLine,int nCmdShow);

LRESULT CALLBACK mainWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK About(HWND, UINT, WPARAM, LPARAM);
LRESULT goCreateMain(HWND, UINT, WPARAM, LPARAM);
LRESULT goCommandMain(HWND, UINT, WPARAM, LPARAM);
LRESULT goTimerMain(HWND, UINT, WPARAM, LPARAM);
LRESULT goPaintMain(HWND, UINT, WPARAM, LPARAM);
LRESULT goDestroyMain(HWND, UINT, WPARAM, LPARAM);

LRESULT goMainManualCmd(HWND,HWND,int, int);
LRESULT goMainAutoCmd(HWND,HWND,int, int);
LRESULT goMainEditCmd(HWND,HWND,int, int);
LRESULT goMainMdiCmd(HWND,HWND,int, int);
LRESULT goAutoMachiningBuildCmd(HWND,HWND,int, int);

int	MyRegisterClassManual(HINSTANCE hInstance);
LRESULT CALLBACK manualWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT goCreateManual(HWND, UINT, WPARAM, LPARAM);
LRESULT goCommandManual(HWND, UINT, WPARAM, LPARAM);
LRESULT goTimerManual(HWND, UINT, WPARAM, LPARAM);
LRESULT goPaintManual(HWND, UINT, WPARAM, LPARAM);
LRESULT goDestroyManual(HWND, UINT, WPARAM, LPARAM);

LRESULT goManualMachAllReturnlCmd(HWND,HWND,int, int);
LRESULT goManualMachSingleReturnCmd(HWND,HWND,int, int);
LRESULT goManualWorkAllReturnlCmd(HWND,HWND,int, int);
LRESULT goManualWorkSingleReturnCmd(HWND,HWND,int, int);
LRESULT goManualSingleTouchCmd(HWND,HWND,int, int);

LRESULT goChangeRatioCmd(HWND,HWND,int, int);

int	MyRegisterClassAuto(HINSTANCE hInstance);
LRESULT CALLBACK autoWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT goCreateAuto(HWND, UINT, WPARAM, LPARAM);
LRESULT goCommandAuto(HWND, UINT, WPARAM, LPARAM);
LRESULT goKeydownAuto(HWND, UINT, WPARAM, LPARAM);
LRESULT goPaintAuto(HWND, UINT, WPARAM, LPARAM);
LRESULT goDestroyAuto(HWND, UINT, WPARAM, LPARAM);
LRESULT goDrawItemAuto(HWND, UINT, WPARAM, LPARAM);//2010-5-30


LRESULT goAutoSelectConCmd(HWND,HWND,int, int);
LRESULT goAutoSelectStateCmd(HWND,HWND,int, int);
LRESULT goAutoListProgramNameCmd(HWND,HWND,int, int);
LRESULT goAutoListConNameCmd(HWND,HWND,int, int);



int MyRegisterClassAutoParam(HINSTANCE hInstance);
LRESULT CALLBACK autoParamWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT goCreateAutoParam(HWND, UINT, WPARAM, LPARAM);
LRESULT goCommandAutoParam(HWND, UINT, WPARAM, LPARAM);
LRESULT goListAutoParam(HWND, UINT, WPARAM, LPARAM);
LRESULT goPaintAutoParam(HWND, UINT, WPARAM, LPARAM);
LRESULT goDestroyAutoParam(HWND, UINT, WPARAM, LPARAM);

int MyRegisterClassAutograph(HINSTANCE hInstance);
LRESULT CALLBACK autographWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT goCreateAutograph(HWND, UINT, WPARAM, LPARAM);
LRESULT goTimerAutograph(HWND, UINT, WPARAM, LPARAM);
LRESULT goPaintAutograph(HWND, UINT, WPARAM, LPARAM);
LRESULT goDestroyAutograph(HWND, UINT, WPARAM, LPARAM);

//2010-5-28
int MyRegisterClassAutoGraph(HINSTANCE hInstance);
LRESULT CALLBACK AutoGraphWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT goPaintAutoGraph(HWND, UINT, WPARAM, LPARAM);
LRESULT goDestroyAutoGraph(HWND, UINT, WPARAM, LPARAM);
LRESULT goCreateAutoGraph(HWND, UINT, WPARAM, LPARAM);
//2010-5-28

int MyRegisterClassAutoNccode(HINSTANCE hInstance);
LRESULT CALLBACK autoNccodeWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT goCreateAutoNccode(HWND, UINT, WPARAM, LPARAM);
LRESULT goPaintAutoNccode(HWND, UINT, WPARAM, LPARAM);
LRESULT goDestroyAutoNccode(HWND, UINT, WPARAM, LPARAM);
LRESULT goTimerAutoNccode(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam);
LRESULT DispNcCode(HWND hWnd);


int MyRegisterClassMdi(HINSTANCE hInstance);
LRESULT CALLBACK mdiWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT goCreateMdi(HWND, UINT, WPARAM, LPARAM);
LRESULT goCommandMdi(HWND, UINT, WPARAM, LPARAM);
LRESULT goKeydownMdi(HWND, UINT, WPARAM, LPARAM);
LRESULT goPaintMdi(HWND, UINT, WPARAM, LPARAM);
LRESULT goDestroyMdi(HWND, UINT, WPARAM, LPARAM);
LRESULT goMdiExpertSystem(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode);

LRESULT goMdiClrCmd(HWND,HWND,int, int);
LRESULT goMdiMachiningBuildCmd(HWND,HWND,int, int);
LRESULT goMdiCoordinateSwitch(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode);

int	MyRegisterClassCoordinate(HINSTANCE hInstance);
LRESULT CALLBACK CoordinateWndProc(HWND,UINT,WPARAM,LPARAM);
LRESULT goCreateCoordinateWnd(HWND, UINT, WPARAM, LPARAM);
LRESULT goPaintCoordinateWnd(HWND, UINT, WPARAM, LPARAM);
LRESULT goCommandCoordinateWnd(HWND, UINT, WPARAM, LPARAM);
LRESULT goListCoordinateWnd(HWND, UINT, WPARAM, LPARAM);
LRESULT goDestroyCoordinateWnd(HWND, UINT, WPARAM, LPARAM);

int	MyRegisterClassEXPERT(HINSTANCE hInstance);
LRESULT CALLBACK EXPERTWndProc(HWND hWnd,UINT msgCode,WPARAM wParam,LPARAM lParam);
LRESULT goCreateEXPERTWnd(HWND, UINT, WPARAM, LPARAM);
LRESULT goPaintEXPERTWnd(HWND, UINT, WPARAM, LPARAM);
LRESULT goCommandEXPERTWnd(HWND, UINT, WPARAM, LPARAM);
LRESULT goKeydownEXPERTWnd(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam);
LRESULT goDestroyEXPERTWnd(HWND, UINT, WPARAM, LPARAM);
LRESULT goExpertSystemCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode);

int MyRegisterClassExpertSystemChild(HINSTANCE hInstance);
LRESULT CALLBACK ExpertSystemChildWndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam);
LRESULT goCreateExpertSystemChild(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam);
LRESULT goPaintExpertSystemChild(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam);
LRESULT goDestroyExpertSystemChild(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam);
LRESULT goListExpertSystemChild(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam);
LRESULT goCommandExpertSystemChild(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam);

int	MyRegisterClassStatus(HINSTANCE hInstance);
LRESULT CALLBACK statusWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT goCreateStatus(HWND, UINT, WPARAM, LPARAM);
LRESULT goCommandStatus(HWND, UINT, WPARAM, LPARAM);
LRESULT goTimerStatus(HWND, UINT, WPARAM, LPARAM);
LRESULT goPaintStatus(HWND, UINT, WPARAM, LPARAM);
LRESULT goDestroyStatus(HWND, UINT, WPARAM, LPARAM);

LRESULT goStatusAxisAllzeroCmd(HWND,HWND,int, int);
LRESULT goStatusAxisSinglezeroCmd(HWND,HWND,int, int);
LRESULT goStatusStartStopCmd(HWND,HWND,int, int);
LRESULT goStatusResetCmd(HWND,HWND,int, int);
LRESULT goStatusSuspendStopCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode);


int MyRegisterClassCoordview(HINSTANCE hInstance);
LRESULT CALLBACK coordviewWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT goCreateCoordview(HWND, UINT, WPARAM, LPARAM);
LRESULT goPaintCoordview(HWND, UINT, WPARAM, LPARAM);
LRESULT goColorCoordview(HWND, UINT, WPARAM, LPARAM);
LRESULT goDestroyCoordview(HWND, UINT, WPARAM, LPARAM);

int	MyRegisterClassEdit(HINSTANCE hInstance);
LRESULT CALLBACK editWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT goCreateEdit(HWND, UINT, WPARAM, LPARAM);
LRESULT goCommandEdit(HWND, UINT, WPARAM, LPARAM);
LRESULT goPaintEdit(HWND, UINT, WPARAM, LPARAM);
LRESULT goDestroyEdit(HWND, UINT, WPARAM, LPARAM);
LRESULT goDrawItemEdit(HWND, UINT, WPARAM, LPARAM); //2010-5-6
LRESULT goKeydownEdit(HWND, UINT, WPARAM, LPARAM); //2010-5-13
LRESULT goEditListview(HWND, UINT, WPARAM, LPARAM);


LRESULT goEditPathCmd(HWND,HWND,int, int);
LRESULT goEditMleditCmd(HWND,HWND,int, int);
LRESULT goEditNewCmd(HWND,HWND,int, int);
LRESULT goEditSaveCmd(HWND,HWND,int, int);
LRESULT goEditOpenCmd(HWND,HWND,int, int);
LRESULT goEditDeletCmd(HWND,HWND,int, int);
LRESULT goEditCloseCmd(HWND,HWND,int, int);
LRESULT goEditDrawCmd(HWND,HWND,int, int);
LRESULT goEditDrawResetCmd(HWND,HWND,int, int);

int MyRegisterClassGraph(HINSTANCE hInstance);
LRESULT CALLBACK graphWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT goDestroyGraph(HWND, UINT, WPARAM, LPARAM);
LRESULT goCreateGraph(HWND, UINT, WPARAM, LPARAM);//2010-5-7
int MyRegisterClassGraph2D(HINSTANCE hInstance);//2010-5-13
LRESULT CALLBACK graph2DWndProc(HWND, UINT, WPARAM, LPARAM);//2010-5-13
LRESULT goPaintGraph2D(HWND, UINT, WPARAM, LPARAM);//2010-5-13
LRESULT goDestroyGraph2D(HWND, UINT, WPARAM, LPARAM);//2010-5-13


BOOL OpenFile(char *, char *, HWND);
BOOL NBSave (HWND);
void InitpNoteInfo(HWND);

LRESULT  find_draw_param(HWND hWnd,LPNCDATA pData,double *draw_width,double *draw_length,double  *draw_mw, double *draw_ml,int new_num);
LRESULT draw_all(HWND hWnd,LPNCDATA pData,double width,double length,double mw, double ml,int new_num);
LRESULT draw_all_tape(HWND hWnd,LPNCDATA pData,double width,double length,double mw, double ml,int new_num);
LRESULT decode(HWND hWnd,LPNCCODE pData,nc_data outdata[],int *outdatanum,int *Child_decode_num,int *file_End_flag ,M_data MChild[] );
LRESULT compensate(HWND hWnd, nc_data indata[],int indatanum, nc_data outdata[],int *outdatanum,
				   double *Child_Start_point_X,double *Child_Start_point_Y, int *Child_build_c, nc_data *Child_cs);
LRESULT tape(HWND hWnd, nc_data indata[],int indatanum, nc_data outdata[],int *outdatanum,
				   double *Child_Start_point_X,double *Child_Start_point_Y, int *Child_build_c, int *first5152flag,nc_data *Child_cs,double *Child_Start_point_B,double *Child_Start_point_C);//11
LRESULT Dcompensate(HWND hWnd, nc_data indata[],int indatanum, nc_data outdata[],int *outdatanumup,int *outdatanumlow,
				   double *Child_Start_point_X,double *Child_Start_point_Y,double *Child_Start_point_B,double *Child_Start_point_C, int *Child_build_c_up,int *Child_build_c_low, nc_data *Child_cs);//11
LRESULT DSP_Compute(HWND hWnd, nc_data indata[],int indatanum, nc_data outdata[],int *outdatanum);
LRESULT CoordinateSwitch(HWND hWnd, nc_data indata[],int indatanum, nc_data outdata[],int *outdatanum);
void File_Convert(HWND hWnd,int fd);



unsigned int  ioread(HWND hWnd,HANDLE hDvice,DWORD ctlword,  DWORD adr );
LRESULT iowrite(HWND hWnd,HANDLE hDvice,DWORD ctlword,  DWORD adr, DWORD buffer);
int ReadNcCodeFileToMem(HWND hWnd,int fd,LPNCCODE pData,int *num);

int Load_NC_Decode_Program(HWND hWnd,LPNCDATA pData,int num1, int num2);
int SendNCDriverUserDecodeEvent(HWND hWnd,HANDLE hEvent);
int PowerOn(HWND hWnd);
int PowerOff(HWND hWnd);


LRESULT goDrawitemMain(HWND, UINT, WPARAM, LPARAM);
/**************************************************************/

//Condition

//宏定义Condition项目列表子窗口ID号
#define		IDW_CONDT	2012	
//宏定义Condition项目按钮子窗口ID号 
#define		IDW_CONDB	2013
//宏定义Condition项目子窗口上放置的控件ID号
#define IDC_BADD	2032	//新增文件BUTTON
#define	IDC_BDELE	2033	//删除文件BUTTON
#define IDC_BCHANGE	2034	//改变参数BUTTON
#define IDC_BSEARCH	2035	//搜索文件BUTTON
#define	IDC_BLOCK	2036	//锁定解锁参数BUTTON
#define IDC_TIN		2039	//输入文本框
#define IDC_TOUT    2040	//Edit静态文本

#define IDC_CONTL0	2042
#define IDC_CONTL1	2043
#define IDC_CONTL2	2044
#define IDC_CONTL3	2045
#define IDC_CONTL4	2046
#define IDC_CONTL5	2047
#define IDC_CONTL6	2048
#define IDC_CONTL7	2049
#define IDC_CONTL8	2050
#define IDC_CONTL9	2051


LRESULT goMainConditionCmd(HWND,HWND,int, int);

int MyRegisterClassConditionT(HINSTANCE);
LRESULT CALLBACK conditionTWndProc(HWND,UINT,WPARAM,LPARAM);
LRESULT goCreateConditionTWnd(HWND,UINT,WPARAM,LPARAM);
LRESULT goCommandConditionTWnd(HWND,UINT,WPARAM,LPARAM);
LRESULT goListConditionTWnd(HWND,UINT,WPARAM,LPARAM);
LRESULT goPaintConditionTWnd(HWND,UINT,WPARAM,LPARAM);
LRESULT goVscrollConditionTWnd(HWND,UINT,WPARAM,LPARAM);
LRESULT goHscrollConditionTWnd(HWND,UINT,WPARAM,LPARAM);
LRESULT goDestroyConditionTWnd(HWND, UINT, WPARAM, LPARAM);

int MyRegisterClassConditionB(HINSTANCE);
LRESULT CALLBACK conditionBWndProc(HWND,UINT,WPARAM,LPARAM);
LRESULT goCreateConditionBWnd(HWND,UINT,WPARAM,LPARAM);
LRESULT goCommandConditionBWnd(HWND,UINT,WPARAM,LPARAM);
LRESULT goPaintConditionBWnd(HWND,UINT,WPARAM,LPARAM);
LRESULT goDestroyConditionBWnd(HWND, UINT, WPARAM, LPARAM);
LRESULT goKeydownConditionBWnd(HWND, UINT, WPARAM, LPARAM);

//NCSet

#define IDW_NCSETB			3001
#define IDC_NCSETPAR		3002
#define IDC_NCSETTAP		3003
#define IDC_NCSETPSET		3004
#define IDC_NCSETPMAC		3005
#define IDC_NCSETPSYS		3006
#define IDC_NCSETPEDIT		3007

#define IDW_NCSETTP			3008
#define IDC_NCSETLV			3009

#define IDW_NCSETTT			3010
#define IDC_WIRE_BS			3011
#define IDC_Z_POSITION		3012
#define IDC_XY_PLANE		3013
#define IDC_TAPEP_ANGLE		3014
#define IDC_H				3015
#define IDC_A				3016
#define IDC_B				3017
#define IDC_NCSETPMILL		3018//2010-5-25

LRESULT goMainStartStopCmd(HWND,HWND,int,int);

int MyRegisterClassNCSetB(HINSTANCE);
LRESULT CALLBACK NCSetBWndProc(HWND,UINT,WPARAM,LPARAM);
LRESULT goCreateNCSetBWnd(HWND,UINT,WPARAM,LPARAM);
LRESULT goCommandNCSetBWnd(HWND,UINT,WPARAM,LPARAM);
LRESULT goPaintNCSetBWnd(HWND,UINT,WPARAM,LPARAM);
LRESULT goDestroyNCSetBWnd(HWND, UINT, WPARAM, LPARAM);
LRESULT goKeydownNCSetBWnd(HWND, UINT, WPARAM, LPARAM);

int MyRegisterClassNCSetT_P(HINSTANCE);
LRESULT CALLBACK NCSetT_PWndProc(HWND,UINT,WPARAM,LPARAM);
LRESULT goCreateNCSetT_PWnd(HWND,UINT,WPARAM,LPARAM);
LRESULT goCommandNCSetT_PWnd(HWND,UINT,WPARAM,LPARAM);
LRESULT goPaintNCSetT_PWnd(HWND,UINT,WPARAM,LPARAM);
LRESULT goDestroyNCSetT_PWnd(HWND, UINT, WPARAM, LPARAM);
LRESULT goVscrollNCSetT_PWnd(HWND,UINT,WPARAM,LPARAM);
LRESULT goListNCSetT_PWnd(HWND,UINT,WPARAM,LPARAM);

int MyRegisterClassNCSetT_T(HINSTANCE);
LRESULT CALLBACK NCSetT_TWndProc(HWND,UINT,WPARAM,LPARAM);
LRESULT goCreateNCSetT_TWnd(HWND,UINT,WPARAM,LPARAM);
LRESULT goCommandNCSetT_TWnd(HWND,UINT,WPARAM,LPARAM);
LRESULT goPaintNCSetT_TWnd(HWND,UINT,WPARAM,LPARAM);
LRESULT goDestroyNCSetT_TWnd(HWND, UINT, WPARAM, LPARAM);

//Screen
typedef struct
{
	UINT uCode;
	BOOL (*functionName)(HWND,UINT,WPARAM,LPARAM);
}dlgMessageProc;

typedef struct
{
	UINT uCode;
	BOOL (*functionName)(HWND,HWND,WORD,WORD);
}dlgCommandProc;

BOOL CALLBACK screenDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL goCommandScreenWnd(HWND,UINT,WPARAM,LPARAM);
BOOL goPaintScreenWnd(HWND,UINT,WPARAM,LPARAM);

//MAINTION

#define IDW_MAINTION_MAIN			3100
#define IDW_MAINTION_EIO_1			3110
#define IDW_MAINTION_EIO_2			3120
#define IDW_MAINTION_RAM			3150

#define IDC_MAINTION_MAIN_EIO		3101
#define IDC_MAINTION_MAIN_RAM		3102
#define IDC_MAINTION_MAIN_EDIT		3103
#define IDC_MAINTION_MAIN_SEG_SET	3104
#define IDC_MAINTION_MAIN_RAM_SET	3105
#define IDC_MAINTION_MAIN_OFSET_SET	3106
#define IDC_MAINTION_MAIN_ABSOLUTE_SET	3107
#define IDC_MAINTION_MAIN_LENGTH_SET	3108
#define IDC_MAINTION_MAIN_CLEAR		3109

#define IDW_MAINTION_EIO_1_NUM		3111
#define IDW_MAINTION_EIO_1_LV		3112

#define IDW_MAINTION_EIO_2_NUM		3121
#define IDW_MAINTION_EIO_2_LV		3122

#define IDW_MAINTION_RAM_LVM_1		3151
#define IDW_MAINTION_RAM_LVM_2		3152



typedef struct _EIO
{
	int		EIO;
	char	Address[5];
	int		Bit_Address[16];
}EIO;

LRESULT goMainResetCmd(HWND,HWND,int,int);

int MyRegisterClassMaintion(HINSTANCE);
LRESULT CALLBACK maintionWndProc(HWND,UINT,WPARAM,LPARAM);
LRESULT goCreateMaintion(HWND,UINT,WPARAM,LPARAM);
LRESULT goCommandMaintion(HWND,UINT,WPARAM,LPARAM);
LRESULT goPaintMaintion(HWND,UINT,WPARAM,LPARAM);
LRESULT goDestroyMaintion(HWND, UINT, WPARAM, LPARAM);
LRESULT goKeydownMaintion(HWND, UINT, WPARAM, LPARAM);

int MyRegisterClassMaintion_EIO_1(HINSTANCE);
LRESULT CALLBACK maintion_EIO_1WndProc(HWND,UINT,WPARAM,LPARAM);
LRESULT goCreateMaintion_EIO_1(HWND,UINT,WPARAM,LPARAM);
LRESULT goCommandMaintion_EIO_1(HWND,UINT,WPARAM,LPARAM);
LRESULT goPaintMaintion_EIO_1(HWND,UINT,WPARAM,LPARAM);
LRESULT goLButtonDownMaintion_EIO_1(HWND,UINT,WPARAM,LPARAM);
LRESULT goNotifyMaintion_EIO_1(HWND,UINT,WPARAM,LPARAM);
LRESULT goSetFocusMaintion_EIO_1(HWND,UINT,WPARAM,LPARAM);
LRESULT goKillFocusMaintion_EIO_1(HWND,UINT,WPARAM,LPARAM);
LRESULT goDestroyMaintion_EIO_1(HWND, UINT, WPARAM, LPARAM);

int MyRegisterClassMaintion_EIO_2(HINSTANCE);
LRESULT CALLBACK maintion_EIO_2WndProc(HWND,UINT,WPARAM,LPARAM);
LRESULT goCreateMaintion_EIO_2(HWND,UINT,WPARAM,LPARAM);
LRESULT goCommandMaintion_EIO_2(HWND,UINT,WPARAM,LPARAM);
LRESULT goPaintMaintion_EIO_2(HWND,UINT,WPARAM,LPARAM);
LRESULT goLButtonDownMaintion_EIO_2(HWND,UINT,WPARAM,LPARAM);
LRESULT goNotifyMaintion_EIO_2(HWND,UINT,WPARAM,LPARAM);
LRESULT goSetFocusMaintion_EIO_2(HWND,UINT,WPARAM,LPARAM);
LRESULT goKillFocusMaintion_EIO_2(HWND,UINT,WPARAM,LPARAM);
LRESULT goDestroyMaintion_EIO_2(HWND, UINT, WPARAM, LPARAM);

int MyRegisterClassMaintion_RAM(HINSTANCE);
LRESULT CALLBACK maintion_RAMWndProc(HWND,UINT,WPARAM,LPARAM);
LRESULT goCreateMaintion_RAM(HWND,UINT,WPARAM,LPARAM);
LRESULT goCommandMaintion_RAM(HWND,UINT,WPARAM,LPARAM);
LRESULT goPaintMaintion_RAM(HWND,UINT,WPARAM,LPARAM);
LRESULT goButtonMaintion_RAM(HWND,UINT,WPARAM,LPARAM);
LRESULT goNotifyMaintion_RAM(HWND,UINT,WPARAM,LPARAM);
LRESULT goDestroyMaintion_RAM(HWND, UINT, WPARAM, LPARAM);

void readEIO_Content(FILE*,int);
void readNext(FILE*,char*);
void setListView(EIO&,HWND);
int getAddress(FILE*,int);
void setListViewEIO(EIO,HWND);
void setListViewEIO_Line(EIO,int,HWND);
void EIO_Change_Activate(HWND);
void EIOclearEdit();
void EIOclearLV();

/**************************************************************/
#endif

