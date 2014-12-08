#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;								// 当前实例
extern HANDLE NcSendhThread;
HANDLE MdihThread;
extern LPNCCODE lpNcCodeMem;         //自动加工代码内存
extern int NcCodeNum; //NC码数
extern int Coordina;
extern void CoordinatehreadProc();
int Coordinate_Flag = 0;
int ExpertSystem_Flag = 0;
extern DWORD WINAPI NcSendThreadProc(LPVOID lpParam);
DWORD WINAPI MdiMachiningBuildThreadProc(LPVOID lpParam);

FILE* fpcoor;
extern void fc_upday(HWND hWnd);
extern void readContentFc(FILE* fp,char* a,int line,HWND hWnd);
extern void readfc_to_buffer(HWND hWnd);
extern void readbuffer_to_fc(HWND hWnd);
extern inline double Gcode2d(char* s, const char* gcode); /*将字符串s中字符串gcode后面的实型数字返回, gcode必须是一个字符!*/
extern int SetStaticsText(HWND hWnd);

extern Coordata data_coorswitch;
extern HWND hWndCoor;
extern Coordata G54_coordinate,G55_coordinate,G56_coordinate,G57_coordinate,G58_coordinate,G59_coordinate;
extern Coordata data_w;   //work coor
extern Coordata data_m;  //Machine coor
extern Coordata data_r;   //ralitive coor
extern HWND hWndstatus;
extern LPCmdThreadParam  pDataInEXPERT;
extern HWND hWnd1;
extern DSP_Para_data DSP_data;


const MessageProc mdiMessages[]={
	    WM_CREATE, goCreateMdi,
		WM_COMMAND, goCommandMdi,
		WM_KEYDOWN, goKeydownMdi,
		WM_PAINT,  goPaintMdi,
		WM_DESTROY, goDestroyMdi
};

const CommandProc mdiCommands[]={
	    IDC_MDIBTNCLR,       goMdiClrCmd,
		IDC_MDIBTNBUILD,     goMdiMachiningBuildCmd, 
		IDC_MDIBTCOOR,		 goMdiCoordinateSwitch,
		IDC_MDIBTEXPERT,	 goMdiExpertSystem
};

CtlWndStruct mdiMenuBtns[]={
		{WS_EX_CLIENTEDGE,TEXT("EDIT"),IDC_MDIEDIT,TEXT(""),0,629,1024,25,WS_VISIBLE|WS_BORDER|WS_CHILD|ES_UPPERCASE},
		{WS_EX_CLIENTEDGE,TEXT("LISTBOX"),IDC_MDILIST,TEXT(""),0,320,620,312,WS_VISIBLE|WS_BORDER|WS_CHILD},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_MDIBTNBUILD,TEXT("LOADING"),910,529,100,50,WS_VISIBLE|WS_BORDER|WS_CHILD|BS_PUSHBUTTON},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_MDIBTNCLR,TEXT("CLRMDI"),910,579,100,50,WS_VISIBLE|WS_BORDER|WS_CHILD|BS_PUSHBUTTON},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_MDIBTCOOR,TEXT("COORDINATE"),910,479,100,50,WS_VISIBLE|WS_BORDER|WS_CHILD|BS_PUSHBUTTON},
		{WS_EX_WINDOWEDGE,TEXT("BUTTON"),IDC_MDIBTEXPERT,TEXT("EXPERT"),910,429,100,50,WS_VISIBLE|WS_BORDER|WS_CHILD|BS_PUSHBUTTON}
};
//子窗口菜单列表
CtlWndStruct mdiChildWnds[]={
	{WS_EX_CLIENTEDGE,TEXT("CoordinateWND"),IDW_COORL,TEXT(""),0,320,705,308,WS_CHILD|SS_NOTIFY|WS_BORDER},//by lsw 2010-4-19
	{WS_EX_CLIENTEDGE,TEXT("EXPERTChildWND"),IDC_EXPERTChild,TEXT(""),0,0,1024,629,WS_CHILD|SS_NOTIFY|WS_BORDER}
};//子窗口，第一列距左端距离，第二列距顶端距离，第三列长度，第四列高度

int MyRegisterClassMdi(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)mdiWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//(LPCTSTR)IDC_EX1;
	wcex.lpszClassName	= TEXT("MDIWND");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
//注册mdi窗口类
	if(!RegisterClassEx(&wcex)) return 0;
	if(!MyRegisterClassCoordinate(hInstance))return 0;
	if(!MyRegisterClassExpertSystemChild(hInstance))return 0;
	return 1;
}

//mdi窗口过程体
LRESULT CALLBACK mdiWndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;

	//查找对应的消息ID,并执行对应的消息处理过程
	for(i=0; i<dim(mdiMessages);i++)
	{
		if(msgCode == mdiMessages[i].uCode) return(*mdiMessages[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//对不存在的消息，调用缺省窗口过程
	return DefWindowProc(hWnd, msgCode, wParam, lParam);
}

LRESULT goCreateMdi(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		
    int i;
//创建mdi窗口菜单
	for(i=0;i<dim(mdiMenuBtns);i++){
		CreateWindowEx( mdiMenuBtns[i].dwExStyle,
			mdiMenuBtns[i].szClass,
			mdiMenuBtns[i].szTitle,
			mdiMenuBtns[i].lStyle,
			mdiMenuBtns[i].x,
			mdiMenuBtns[i].y,
			mdiMenuBtns[i].cx,
			mdiMenuBtns[i].cy,
			hWnd,
			(HMENU)mdiMenuBtns[i].nID,
			hInst,
			NULL
			);
	}

//创建mdi窗口的子窗口
	
	for(i=0;i<dim(mdiChildWnds);i++){
		CreateWindowEx(mdiChildWnds[i].dwExStyle,
		    mdiChildWnds[i].szClass,
			mdiChildWnds[i].szTitle,
			mdiChildWnds[i].lStyle,
			mdiChildWnds[i].x,
			mdiChildWnds[i].y,
			mdiChildWnds[i].cx,
		    mdiChildWnds[i].cy,
			hWnd,
			(HMENU)mdiChildWnds[i].nID,
			hInst,
			NULL
			);
	}
	//显示其中一个窗口

	ShowWindow(GetDlgItem(hWnd,mdiChildWnds[0].nID),SW_SHOW);
	ShowWindow(GetDlgItem(hWnd,IDW_COORL),SW_HIDE);
	ShowWindow(GetDlgItem(hWnd,IDC_EXPERTChild),SW_HIDE);
	fc_upday(GetDlgItem(hWnd,IDW_COORL));
	readfc_to_buffer(hWnd);
	return 0;
}

LRESULT goCommandMdi(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		//定义菜单ID号和通知单
		int menuID, notifyCode;
		
		//定义窗口句柄
		HWND wndCtrl;
		 int i;

		 
		menuID    = LOWORD(wParam); 
		notifyCode = HIWORD(wParam); 
		wndCtrl = (HWND) lParam;
	   

		
		for(i=0; i<dim(mdiCommands);i++)
		{
			if(menuID == mdiCommands[i].uCode) return(*mdiCommands[i].functionName)(hWnd,wndCtrl,menuID,notifyCode);
		}
			
		return 0;
}


LRESULT goKeydownMdi(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	
	TCHAR szBuffer[501];
	int menuID, notifyCode;
		
	//定义窗口句柄
	HWND wndCtrl;
	int i;
	int nSelItem;
	

	menuID    = LOWORD(wParam); 
	notifyCode = HIWORD(wParam); 
	wndCtrl = (HWND) lParam;
	if(ExpertSystem_Flag == 0)
	{
		if(Coordinate_Flag == 1)
		{
			switch(Coordina)
			{
			case 1:
				CoordinatehreadProc();
				break;
			}
		}
		else
		{
			GetWindowText(GetDlgItem(hWnd,IDC_MDIEDIT), szBuffer, 500);
			SendMessage(GetDlgItem(hWnd,IDC_MDILIST),LB_INSERTSTRING,-1,(LPARAM)szBuffer);
			SetWindowText (GetDlgItem(hWnd,IDC_MDIEDIT), "");
		}	
	}
	else
	{
		nSelItem =  ListView_GetNextItem(pDataInEXPERT->wndCtrl,-1,LVNI_DROPHILITED);
		GetWindowText (GetDlgItem(GetParent(pDataInEXPERT->hWnd),IDC_MDIEDIT), szBuffer, 30);//得到输入文本框内所输入数值

		SetWindowText (GetDlgItem(GetParent(pDataInEXPERT->hWnd),IDC_MDIEDIT),"");
		ListView_SetItemText(pDataInEXPERT->wndCtrl,nSelItem,0,szBuffer);              //??
		//将选择项蓝背景颜色去掉
		for(i=0;i<5;i++)ListView_SetItemState(GetDlgItem(pDataInEXPERT->hWnd,IDC_SIMPLE+i),-1,LVIS_SELECTED, 0XFF);	

		switch(nSelItem)
		{
		case 0:
			DSP_data.PARA_X = atoi(szBuffer);
			break;
		case 1:
			DSP_data.PARA_Y = atoi(szBuffer);
			break;
		case 2:
			DSP_data.PARA_Z = atoi(szBuffer);
			break;
		case 3:
			DSP_data.PARA_Sway_Switch_Flag = atoi(szBuffer);
			break;
		case 4:
			DSP_data.PARA_Sway_R = atoi(szBuffer);
			break;
		case 5:
			DSP_data.PARA_Sway_FreeOr3D = atoi(szBuffer);
			break;
		case 6:
			DSP_data.PARA_Sway_Polygon_N = atoi(szBuffer);
			break;
		case 7:
			DSP_data.PARA_Jump_Select_Center = atoi(szBuffer);
			break;
		case 8:
			DSP_data.PARA_Jump_Height = atoi(szBuffer);
			break;
		case 9:
			DSP_data.PARA_Jump_Average_Speed = atoi(szBuffer);
			break;
		case 10:
			DSP_data.PARA_DownTime = atoi(szBuffer);
			break;
		case 11:
			DSP_data.PARA_Offset  = atoi(szBuffer);
			break;
		case 12:
			DSP_data.PARA_Sway_A  = atoi(szBuffer);
			break;
		default:
			break;
		}
	}

	return 0;
}

LRESULT goPaintMdi(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		PAINTSTRUCT ps;
		HDC hdc;

		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		return 0;
}

LRESULT goDestroyMdi(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
		PostQuitMessage(0);
		return 0;
}
//坐标系选择
LRESULT goMdiCoordinateSwitch(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
/////////////////////////
	if(Coordinate_Flag == 0)
	{
		ShowWindow(GetDlgItem(hWnd,IDW_COORL),SW_SHOW);
		ShowWindow(GetDlgItem(hWnd,IDC_MDILIST),SW_HIDE);
		ShowWindow(GetDlgItem(hWnd,IDC_EXPERTChild),SW_HIDE);
		fc_upday(hWndCoor);
		readfc_to_buffer(hWnd);
		Coordinate_Flag = 1;
	}
	else
	{
		ShowWindow(GetDlgItem(hWnd,IDW_COORL),SW_HIDE);
		ShowWindow(GetDlgItem(hWnd,IDC_MDILIST),SW_SHOW);
		ShowWindow(GetDlgItem(hWnd,IDC_EXPERTChild),SW_HIDE);
		fc_upday(hWndCoor);
		readfc_to_buffer(hWnd);
		Coordinate_Flag = 0;
	}

	return 0;
}

LRESULT goMdiExpertSystem(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
/////////////////////////
	if(ExpertSystem_Flag == 0)
	{
		ShowWindow(GetDlgItem(GetParent(hWnd),IDC_STATUSDISP),SW_HIDE);
		ShowWindow(GetDlgItem(hWnd,IDC_EXPERTChild),SW_SHOW);
		ShowWindow(GetDlgItem(hWnd,IDW_COORL),SW_HIDE);
		ShowWindow(GetDlgItem(hWnd,IDC_MDILIST),SW_HIDE);
		ExpertSystem_Flag = 1;
		ShowWindow(GetDlgItem(hWnd,IDC_MDIBTNBUILD),SW_HIDE);
		ShowWindow(GetDlgItem(hWnd,IDC_MDIBTNCLR),SW_HIDE);
		ShowWindow(GetDlgItem(hWnd,IDC_MDIBTCOOR),SW_HIDE);
		ShowWindow(GetDlgItem(hWnd,IDC_MDIBTEXPERT),SW_HIDE);

	}
	//else
	//{
	//	
	//	ShowWindow(hWnd1,SW_SHOW);
	//	ShowWindow(GetDlgItem(hWnd,IDC_EXPERTChild),SW_HIDE);
	//	ShowWindow(GetDlgItem(hWnd,IDW_COORL),SW_HIDE);
	//	ShowWindow(GetDlgItem(hWnd,IDC_MDILIST),SW_SHOW);
	//	Coordinate_Flag = 0;
	//	ExpertSystem_Flag = 0;
	//}

	return 0;
}
LRESULT goMdiClrCmd(HWND hWnd,HWND wndCtrl,int menuID, int notifyCode)
{
	int i;
	int nItem;
	

	nItem = SendMessage(GetDlgItem(hWnd,IDC_MDILIST),LB_GETCOUNT,0,0);
	for(i=0;i<nItem;i++){
        SendMessage(GetDlgItem(hWnd,IDC_MDILIST),LB_DELETESTRING,(WPARAM)0,0);
	}
    return 0;
}


LRESULT goMdiMachiningBuildCmd(HWND hWnd, HWND wndCtrl,int menuID,int notifyCode)
{
	DWORD dwThreadID;
	LPCmdThreadParam  pData;
	
	
		
	pData = (LPCmdThreadParam)HeapAlloc(GetProcessHeap(),
									HEAP_ZERO_MEMORY,
									sizeof(CmdThreadParam)
									);
	if(pData == NULL)
	{
		MessageBox(hWnd,"can not alloc heapmemory in function MdiMachiningBuild",NULL,NULL);
		return 1;
	}

	pData->hWnd = hWnd;
	pData->wndCtrl = wndCtrl;
	pData->menuID = menuID;
	pData->notifyCode = notifyCode;
		
   	MdihThread = CreateThread(
		NULL,
		0,
		MdiMachiningBuildThreadProc,
		pData,
		0,
		&dwThreadID
		);

	if( MdihThread==NULL)
	{
		MessageBox(hWnd,"can not create Thread in function MdiMachiningBuild",NULL,NULL);
		return 1;
	}

	return 0;
}


DWORD WINAPI MdiMachiningBuildThreadProc(LPVOID lpParam)
{
	LPCmdThreadParam  pData; 
  	
	LPCmdThreadParam  pData_nc;
	DWORD dwThreadID;
	LPNCDATA pDataNcGraphMem;
	FILE *file;
	int nItemCount;
	int reallength;

	TCHAR szBuffer[501]; 
	
	
	int decodeNum,compasateNum,tapeNum,ComputeNum;
	int all_decode_num,all_creat_num;

	nc_data decodeData[2*DECODE_NUM_ONCE];
	nc_data compasateData[2*DECODE_NUM_ONCE];
	nc_data tapeData[2*DECODE_NUM_ONCE];
	nc_data ComputeData[2*DECODE_NUM_ONCE];
	M_data MChild[2*DECODE_NUM_ONCE];
	

	
	
	int  fdEdit;  //译码文件句柄定义
	int end_decode;
	

	double compasate_Start_point_X,compasate_Start_point_Y;
    int compasate_build_c;
	nc_data *compasate_cs;

	double tape_Start_point_X,tape_Start_point_Y;
	double tape_Start_point_B,tape_Start_point_C;
    int tape_build_c,first5152flag;
	nc_data *tape_cs;
	


	int nc_start_flag;

	int i;
	int j = 0,k = 0;
    char ptext[100];
	
	

	
			//以下添加mdi加工程序
			//SuspendThread(MdihThread);
			
			compasate_Start_point_X=0.;
			compasate_Start_point_Y=0.;
			compasate_build_c=0;
			
			tape_Start_point_X=0.;
			tape_Start_point_Y=0.;
			tape_build_c=0;
			
			memset(decodeData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));
			memset(compasateData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));
			memset(ComputeData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));
			memset(MChild,0,2*DECODE_NUM_ONCE*sizeof(M_data)); 
		   
			
			compasate_cs = (nc_data *)malloc(sizeof(nc_data));
			memset(compasate_cs,0,sizeof(nc_data));

			tape_cs = (nc_data *)malloc(sizeof(nc_data));
			memset(tape_cs,0,sizeof(nc_data));
			
			


			pData = (LPCmdThreadParam)lpParam;

			end_decode = 0;
			all_decode_num =0;
			all_creat_num = 0;

			nc_start_flag=1;


			
			//将mdi的nc码放到临时文件中
			
			
			
			if ((file = fopen("MdiTemp.txt", "w+")) == NULL)
			{	
				MessageBox (pData->hWnd,"can not create MdiTemp file in function MdiMachiningBuildThreadProc",NULL,NULL);
				
				return 1;
			}


			nItemCount = SendMessage(GetDlgItem(pData->hWnd,IDC_MDILIST),LB_GETCOUNT,0,0);
			
			if(nItemCount==0)
			{
				MessageBox (pData->hWnd,"there is no NCCODE in function MdiMachiningBuildThreadProc",NULL,NULL);
				
				return 1;
			}
			
			for(i=0;i<nItemCount;i++)
			{
				SendMessage(GetDlgItem(pData->hWnd,IDC_MDILIST),LB_GETTEXT,i,(LPARAM)szBuffer);
				reallength = SendMessage(GetDlgItem(pData->hWnd,IDC_MDILIST),LB_GETTEXTLEN,i,0);
				
				if (fwrite(szBuffer, 1, reallength, file) < 0)
				{
        				MessageBox (pData->hWnd,"write file err in function MdiMachiningBuildThreadProc" ,NULL,NULL);
						return 1;
						
				}
			}
			fclose (file);
			fdEdit = _open("MdiTemp.txt", O_RDONLY);

			do{

				do{
					reallength = _read(fdEdit, ptext+k, 1 );
					if(reallength == 0)
					{
						return 0;
					}
					k++;
				}while(ptext[k-1] != ';');
				ptext[k] = 0;


					if(strstr(ptext+j,"X")) 
					{
						data_coorswitch.x=Gcode2d(ptext+j,"X");
					}
					else 
					{
						data_coorswitch.x = 0;
					}
					if(strstr(ptext+j,"Y")) 
					{
						data_coorswitch.y=Gcode2d(ptext+j,"Y");
					}
					else 
					{
						data_coorswitch.y = 0;
					}
					if(strstr(ptext+j,"Z")) 
					{
						data_coorswitch.z=Gcode2d(ptext+j,"Z");
					}
					else 
					{
						data_coorswitch.z = 0;
					}
					if(strstr(ptext,"B")) 
					{
						data_coorswitch.b=Gcode2d(ptext,"B");
					}
					else 
					{
						data_coorswitch.b = 0;
					}
					if(strstr(ptext+j,"C")) 
					{
						data_coorswitch.c=Gcode2d(ptext+j,"C");
					}
					else 
					{
						data_coorswitch.c = 0;
					}

					if(strstr(ptext+j,"G54:"))
					{
						G54_coordinate = data_coorswitch;

					}
					if(strstr(ptext+j,"G55:"))
					{
						G55_coordinate = data_coorswitch;

					}
					if(strstr(ptext+j,"G56:"))
					{
						G56_coordinate = data_coorswitch;

					}
					if(strstr(ptext+j,"G57:"))
					{
						G57_coordinate = data_coorswitch;

					}
					if(strstr(ptext+j,"G58:"))
					{
						G58_coordinate = data_coorswitch;

					}
					if(strstr(ptext+j,"G59:"))
					{
						G59_coordinate = data_coorswitch;

					}
					if(strstr(ptext+j,"G92:"))
					{
						data_w = data_coorswitch;
						data_r.x = data_m.x - data_w.x;
						data_r.y = data_m.y - data_w.y;
						data_r.z = data_m.z - data_w.z;
						data_r.b = data_m.b - data_w.b;
						data_r.c = data_m.c - data_w.c;



					}
					j = k;
					readbuffer_to_fc(pData->hWnd);
					fc_upday(hWndCoor);
			}while(reallength !=0);
			




			SuspendThread(MdihThread);
			//开设译码绘图内存
			pDataNcGraphMem = (LPNCDATA)HeapAlloc(GetProcessHeap(),
											HEAP_ZERO_MEMORY,
											MAX_NC_MEM*sizeof(nc_data)
											);
			if(pDataNcGraphMem == NULL)
			{
				MessageBox(pData->hWnd,"can not alloc heapmemory in function MdiMachiningBuildThreadProc",NULL,NULL);
				return 1;
			}

			//开设NC代码内存
			lpNcCodeMem = (LPNCCODE)HeapAlloc(GetProcessHeap(),
											HEAP_ZERO_MEMORY,
											MAX_NC_MEM*sizeof(nc_code)
											);
			if(pDataNcGraphMem == NULL)
			{
				MessageBox(pData->hWnd,"can not alloc heapmemory in function AutoMachiningBuildThreadProc",NULL,NULL);
				return 1;
			}

			
			 //为读取文件到内存打开文件
			fdEdit = _open(szBuffer, O_RDONLY);
		 
			if (fdEdit <= 0) 
			{		                        
				MessageBox (pData->hWnd, "can not open file in AutoMachiningBuildThreadProc","Program", MB_OK | MB_ICONSTOP);
				return 1;
			}

			ReadNcCodeFileToMem(pData->hWnd,fdEdit,lpNcCodeMem,&NcCodeNum);
			_close(fdEdit);


			//打开译码文件
			fdEdit = _open("MdiTemp.txt", O_RDONLY);
		 
			if (fdEdit <= 0)
			{		                        
				MessageBox (pData->hWnd, "can not open file in MdiMachiningBuildThreadProc","Program", MB_OK | MB_ICONSTOP);
				return 1;
			}
			ReadNcCodeFileToMem(pData->hWnd,fdEdit,lpNcCodeMem,&NcCodeNum);
			do{
					if(decode(pData->hWnd,lpNcCodeMem,decodeData,&decodeNum,&all_decode_num,&end_decode,MChild)==1) return 1; // 分段译码
				
					if(compensate(pData->hWnd,decodeData,decodeNum,compasateData,&compasateNum,&compasate_Start_point_X,&compasate_Start_point_Y, &compasate_build_c,compasate_cs)==1) return 1;//分段刀具补偿
				  
				  	
					if(tape(pData->hWnd,compasateData,compasateNum,tapeData,&tapeNum,&tape_Start_point_X,&tape_Start_point_Y,&tape_build_c,&first5152flag,tape_cs,&tape_Start_point_B,&tape_Start_point_C)==1,&tape_Start_point_B,&tape_Start_point_C) return 1; //锥面补偿
					if(DSP_Compute(pData->hWnd,tapeData,tapeNum,ComputeData,&ComputeNum)==1) return 1; 			
					
							
					CopyMemory(pDataNcGraphMem+all_creat_num,ComputeData,(ComputeNum*sizeof(nc_data)));
						
				

					memset(compasateData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));
					memset(tapeData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));
					memset(ComputeData,0,2*DECODE_NUM_ONCE*sizeof(nc_data));


					all_creat_num=all_creat_num + ComputeNum;
					ComputeNum = 0;
					compasateNum=0;
					tapeNum=0;
					
					//开设向下位机传送数据线程
					if(nc_start_flag==1)
					{
						pData_nc = (LPCmdThreadParam)HeapAlloc(GetProcessHeap(),
											HEAP_ZERO_MEMORY,
											sizeof(CmdThreadParam)
											);

						if(pData_nc == NULL)
						{
							MessageBox(pData->hWnd,"can not alloc heapmemory in function MdiMachiningBuildThreadProc",NULL,NULL);
							return 1;
						}


						pData_nc->hWnd = pData->hWnd;
						pData_nc->wndCtrl = pData->wndCtrl;
						pData_nc->menuID = pData->menuID;
						pData_nc->notifyCode =pData-> notifyCode;
						pData_nc->ncMem = pDataNcGraphMem;

						NcSendhThread = CreateThread(
								NULL,
								0,
								NcSendThreadProc,
								pData_nc,
								0,
								&dwThreadID
								);

						if( NcSendhThread==NULL)
						{
							MessageBox(pData->hWnd,"can not create Thread in function MdiMachiningBuildThreadProc",NULL,NULL);
							return 1;
						}

						SendNCDriverUserDecodeEvent(pData->hWnd,NcSendhThread);  //向驱动程序下传传送数据线程的句柄
						nc_start_flag = 0;
					}


			}while(end_decode != 1);

			_close(fdEdit);



			
			//find_draw_param(GetDlgItem (pData->hWnd, IDC_AUTOGRAPH),pDataNcGraphMem,&auto_draw_width,&auto_draw_length,&auto_mw, &auto_ml,all_creat_num);//求取画图参数
			

			//draw_all(GetDlgItem (pData->hWnd, IDC_AUTOGRAPH),pDataNcGraphMem,auto_draw_width,auto_draw_length,auto_mw, auto_ml,all_creat_num); //画全部图
			
					
			free(tape_cs);
		    free(compasate_cs);
	

	//取消线程、释放内存
	CloseHandle(MdihThread);
	if(HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,pData) == 0){
		MessageBox(pData->hWnd,"can not free heapmemory in function MdiMachiningBuildThreadProc",NULL,NULL);
		return 1;
	}
	return 0;
}




