// EDM_NC_PROGRAM.cpp : 定义应用程序的入口点。
//
#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"



// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名


HANDLE IOPCIhDevice;               // IO_PCI设备句柄 
HANDLE NCPCIhDevice;                // NC_PCI设备句柄 
HANDLE PowerPCIhDevice;                // NC_PCI设备句柄 
HANDLE hDecodeEvent;  //启动向下位机传送译码数据线程事件的句柄 


extern Coordata data_m;  //Machine coor
extern Coordata data_w;   //work coor
extern Coordata data_r;   //ralitive coor
extern Coorpuls pulse_m; //Machine pulse
extern Coorpuls pulse_w; //work pulse
extern ConditionStru Machiningparam;
extern char AutoMachineFileName[_MAX_FNAME];
extern int DspProgramLoad(HWND hWnd);
extern int DspProgramLoad1(HWND hWnd);
int message(HWND hWnd,int u);
int MachinePosPlusWrite(HWND hWnd, double data_mx,double data_my,double data_mz,double data_mb,double data_mc);
int DspMemRead(HWND hWnd,DWORD *HpiA,DWORD DspMem[],int Length );
HANDLE GetDeviceViaInterface(HWND hWnd,GUID* pGuid, DWORD instance);
int IinitProgram(HWND hWnd);
int MachineRead(HWND hWnd);//Read the machine position before ReturnZero progarm starts
//2010-3-30
int readCondition();//本函数功能为初始化加工参数库。
int length=6;//规定每个数据段写入长度为6个字节，其大小=加工参数文件最大位数+2
int conditionLength=9999;//设定加工参数文件最大数目为9999个
int countNumber=11;//项目有11个项目
void startW(FILE*,FILE*,int);
void copyF(FILE*,FILE*,FILE*,FILE*);
void firstLine(FILE*,FILE*,FILE*,int&);
void otherLine(FILE*,FILE*,FILE*,int&);
int getFILE(FILE*,int&);
void copyF(FILE*,FILE*,FILE*);
void writeF(FILE*,int);
int getC(FILE*);

extern int startR;
int ConditionNumber;

FILE* fpTemp;//定位temp.txt文件的指针
FILE* fpSeek;//定位seek.txt文件的指
FILE* fpLock;//定位lock.txt文件的指针
//2010-3-30
//2010-4-20
extern FILE* fpSet;
extern FILE* fpMac;
extern FILE* fpSys;
extern FILE* fpEIO;
extern FILE* fpEIO_Address;
extern FILE* fpcoor;
//2010-4-20
extern int MILL_ESPARK;
int main(HWND hWnd);


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;
    
	//HWND hWnd;
	
	MachineRead(0);  //读关机保存的情报
	//2010-3-30
	if(readCondition()==1)return 3;//读取加工参数文件失败返回3 
//以读写模式打开各文件，其中temp.txt为condition.csv文件的副本
//seek.txt用于标记各加工参数在temp.txt中的位置，以提供快速查找搜寻操作
//lock.txt用于标记各参数文件是否锁定
	fpTemp=fopen("temp.txt","r+");
	if(fpTemp==NULL)
	{
		return 1;
	}
	fpSeek=fopen("seek.txt","r+");
	if(fpSeek==NULL)
	{
		return 1;
	}
	fpLock=fopen("lock.txt","r+");
	if(fpLock==NULL)
	{
		return 1;
	}
	fpSet=fopen("NCSetSet.txt","r+");
	if(fpSet==NULL)
	{
		return 1;
	}
	fpMac=fopen("NCSetMac.txt","r+");
	if(fpMac==NULL)
	{
		return 1;
	}
	fpSys=fopen("NCSetSys.txt","r+");
	if(fpSys==NULL)
	{
		return 1;
	}
	fpEIO=fopen("EIO.txt","r");
	if(fpEIO==NULL)
	{
		return 1;
	}
	fpEIO_Address=fopen("EIO_Address.txt","r");
	if(fpSys==NULL)
	{
		return 1;
	}
	fpcoor=fopen("fpcoor.txt","r+");
	if(fpcoor==NULL)
	{
		return 1;
	}



	//2010-3-30
//	MILL_ESPARK=1;//2010-5-25 1表示系统初始默认为铣削系统，0表示线切割系统
    IinitProgram(0);  //整个数控程序软硬件的初始化		
	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_EDM_NC_PROGRAM, szWindowClass, MAX_LOADSTRING);

	

	// 判断注册窗口成功否，不成功返回1并退出
	if(!MyRegisterClassMain(hInstance)) return 1;

	//判断创建主窗口并显示主窗口成功否，失败，返回值2并退出
	if(!showMainWnd(hInstance,lpCmdLine,nCmdShow)) return 2;

	
	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_EDM_NC_PROGRAM);
	
	
	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{

		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			if(msg.message==WM_KEYDOWN && msg.wParam==VK_RETURN && (GetDlgCtrlID(GetFocus())!= IDC_MLEDIT ||GetDlgCtrlID(GetFocus())!=IDC_MDIEDIT)) {
				SendMessage(GetParent(GetFocus()),WM_KEYDOWN,msg.wParam,msg.lParam);
			}
			else
			DispatchMessage(&msg);
		}		
	}
	return (int) msg.wParam;
}

int MachineRead(HWND hWnd)//Read the machine position before ReturnZero progarm starts
{

	char data[51][_MAX_FNAME]={"","","","",""};
	FILE *MachineFile;//Machine Coor File;	
	int i,j;
	char ch;
	/*Read Machine Pos*/
	MachineFile=fopen("MachinePos.txt","rt");	
	if(MachineFile==NULL)
	{
		//MessageBox(hWnd,"MachinePos.txt open error when reading file in fuction MachinePosRead ",NULL,NULL);
		return 1;
	}
            
	i=0;
	j=0;
	ch=fgetc(MachineFile);
	while(ch!=EOF)
	 {
		if(ch !='\n')
		{
			data[i][j]=ch;
				j=j+1;
		}
		else
        {
			i++;
			j=0;
		}	
		ch=fgetc(MachineFile);
	 }
	
	data_m.x=atof(data[0]);
	data_m.y=atof(data[1]);
	data_m.z=atof(data[2]);
	data_m.b=atof(data[3]);
	data_m.c=atof(data[4]);//string to float;

	data_r.x=atof(data[5]);
	data_r.y=atof(data[6]);
	data_r.z=atof(data[7]);
	data_r.b=atof(data[8]);
	data_r.c=atof(data[9]);//string to float;

	strcpy(AutoMachineFileName,data[10]);
	Machiningparam.eNo = atoi(data[11]);
	
	for(i=0;i<30;i++)
	{
        Machiningparam.item[i].value= atoi(data[12+i]);
	}
	startR=atoi(data[12+i]);//读取关机时加工参数界面状态 by lsw 2010-4-6
	if(fclose(MachineFile))
	{
		//MessageBox(hWnd,"MachinePos.txt close error when reading file in fuction MachinePosRead ",NULL,NULL);
		return 1;
	}		
	return 0;	
}
int PowerOn(HWND hWnd)
{
	 //iowrite(hWnd,IOPCIhDevice,IO_FPGA_WRITE, ADR_OUTB,  0x13);
	 //Sleep(1000);
	 //iowrite(hWnd,IOPCIhDevice,IO_FPGA_WRITE, ADR_OUTB,  0x17);
	 //Sleep(1000);
	 //iowrite(hWnd,IOPCIhDevice,IO_FPGA_WRITE, ADR_OUTB,  0x1f);

	 return 0;
}
int PowerOff(HWND hWnd)
{
	 //iowrite(hWnd,IOPCIhDevice,IO_FPGA_WRITE, ADR_OUTB,  0x17);
	 //Sleep(1000);
	 //iowrite(hWnd,IOPCIhDevice,IO_FPGA_WRITE, ADR_OUTB,  0x03);

	 return 0;
}
int message(HWND hWnd,int u) 
{ 
	char chTemp1[50];
		wsprintf(chTemp1, " %d",u);
		MessageBox(hWnd,chTemp1,NULL,NULL);
	return 0; 
}
int DspMemRead(HWND hWnd,DWORD *HpiA,DWORD DspMem[],int Length )
{
	BOOL bResult;
	DWORD junk; 
	DWORD MemRead[100];	

		bResult = DeviceIoControl(NCPCIhDevice,  // device to be queried
					DSP_MEMORY_READ,  // operation to perform
                    (LPVOID)HpiA, 4, // no input buffer
                    &MemRead,sizeof(MemRead),     // output buffer
                     &junk,                 // # bytes returned
                     (LPOVERLAPPED) NULL);  // synchronous I/O
		
		if(bResult==FALSE)
		{
				MessageBox(hWnd,"can not get i/o data in fuction HpiLoad", NULL,NULL);
				int c = GetLastError();
				return 1;
		}
		memcpy(DspMem,&MemRead,Length);
	
		return 0;
}
int IinitProgram(HWND hWnd)  //整个数控程序软硬件的初始化
{
    unsigned int limit;
	int readtest;
	//DWORD *HpiAadr;
	//DWORD DSPMem[36];	
	//int DSPLength;
	//DSPLength = sizeof(DSPMem);
	//HpiAadr = (DWORD *) malloc(sizeof(DWORD)); 
	//*HpiAadr = 0x0;
 //   
	//char string[8];


	//建立PCI_NC板设备

	NCPCIhDevice = GetDeviceViaInterface(hWnd,(LPGUID)&NCCtrlDevice,0);

	if (NCPCIhDevice == INVALID_HANDLE_VALUE) // cannot open the drive
	{
		MessageBox(hWnd,"can not create  NCPCIhDevice in IinitProgram function",NULL,NULL);
		return 1;
	}
	
	//建立PCI_IO板设备
	
	IOPCIhDevice = GetDeviceViaInterface(hWnd,(LPGUID)&IOCtrlDevice,0);

	if (IOPCIhDevice == INVALID_HANDLE_VALUE) // cannot open the drive
	{
		MessageBox(hWnd,"can not create  IOPCIhDevice in IinitProgram function",NULL,NULL);
		return 1;
	}


	PowerPCIhDevice = GetDeviceViaInterface(hWnd,(LPGUID)&PowerCtrlDevice,0);

	if (PowerPCIhDevice == INVALID_HANDLE_VALUE) // cannot open the drive
	{
		MessageBox(hWnd,"can not create  PowerPCIhDevice in IinitProgram function",NULL,NULL);
		return 1;
	}
	DspProgramLoad(hWnd);  //通过HPI接口向DSP下载程序	
//	DspProgramLoad1(hWnd);  //通过HPI接口向PowerDSP下载程序	
	//将设备号写入加工参数结构体中，要写30个
	//Machiningparam.item[0].hDevice = IOPCIhDevice;
	//Machiningparam.item[1].hDevice = IOPCIhDevice;
/************I/O的输出***************************/



  //iowrite(hWnd,IOPCIhDevice,IO_FPGA_WRITE, ADR_OUTB,  0x0);
 // iowrite(hWnd,IOPCIhDevice,IO_FPGA_WRITE, ADR_OUTB,  0x13);
  iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE, ADR_CONT_Star_Stop,  0x01);//下传reset信号


	
	//iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE, ADR_A_ENC_B,  0x0);

	limit = ioread(hWnd,NCPCIhDevice,NC_FPGA_READ,ADR_C_IN_RAS_Z );


	iowrite(hWnd,IOPCIhDevice,IO_FPGA_WRITE, ADR_test,  2);

	iowrite(hWnd,IOPCIhDevice,IO_FPGA_WRITE, ADR_ab_con, 63 );
	iowrite(hWnd,IOPCIhDevice,IO_FPGA_WRITE, ADR_ab_stop,  255);

	iowrite(hWnd,IOPCIhDevice,IO_FPGA_WRITE, ADR_nor_con,  255);
	iowrite(hWnd,IOPCIhDevice,IO_FPGA_WRITE, ADR_nor_stop,  255);

	iowrite(hWnd,IOPCIhDevice,IO_FPGA_WRITE, ADR_gap,  10);
	iowrite(hWnd,IOPCIhDevice,IO_FPGA_WRITE, ADR_I,  8);

  	iowrite(hWnd,IOPCIhDevice,IO_FPGA_WRITE, ADR_start,  0x01);
	//iowrite(hWnd,IOPCIhDevice,IO_FPGA_WRITE, ADR_start,  0x0);
	//DspMemRead(hWnd,HpiAadr,DSPMem,DSPLength);
	//ultoa(DSPMem[0],string,16); 


/*********开机限位检查，有问题显示****************/	
	
	limit = ioread(hWnd,NCPCIhDevice,NC_FPGA_READ,ADR_LIMIT );
	//if(3==limit)  {SetWindowText(GetDlgItem(hMainWnd,130),"OK,no limit!");}  

	if(7 ==limit) 	MessageBox(hWnd,"Waring: Z- limit !", NULL,NULL);

	if(11==limit)   MessageBox(hWnd,"Waring: Z+ limit !", NULL,NULL);
	if(19==limit) MessageBox(hWnd,"Waring: Y- limit !", NULL,NULL);
	if(35==limit) MessageBox(hWnd,"Waring: Y+ limit !", NULL,NULL);
	if(67==limit) MessageBox(hWnd,"Waring: X- limit !", NULL,NULL);
	if(131==limit) MessageBox(hWnd,"Waring: X+ limit !", NULL,NULL); //开机限位检查;	

	/***************写入电机运转的必须信号*************************/
	 iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE, ADR_SRV_ON,  0x0);
	 iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE, ADR_SRV_ON,  0x01F);
	 iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE, ADR_SRV_INH,  0x01F);
	 iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE, ADR_SOFTLIM,  0x01);  //软限位信号
	// iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE, ADR_SRV_CWL,  0x01F);   //FPGA内部接了限位信号，不需要上位机控制
	// iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE, ADR_SRV_CCWL,  0x01F);	

	/******************加工参数输出******************/

		//将加工参数读入全局变量加工参数结构体并将结构体值输出的设备

	/******************初始化位置寄存器的值********************/
	 
	 MachinePosPlusWrite(hWnd,data_m.x,data_m.y,data_m.z,data_m.b,data_m.c);	//写机器坐标到NC板

	 hDecodeEvent = CreateEvent(NULL,FALSE,FALSE,NULL); 
	if(hDecodeEvent == NULL)
	{
			MessageBox(hWnd,"can not create  hDecodeEvent in IinitProgram function",NULL,NULL);
			return 1;
	}

	SendNCDriverUserDecodeEvent(hWnd,hDecodeEvent); //向驱动程序下传启动传送译码数据线程的事件句柄
	
	return 0;
}



HANDLE GetDeviceViaInterface(HWND hWnd,GUID* pGuid, DWORD instance)
{
	// Get handle to relevant device information set
	HDEVINFO info = SetupDiGetClassDevs(pGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
	if(info==INVALID_HANDLE_VALUE)
	{
		MessageBox(hWnd, "No HDEVINFO available for this GUID",NULL,NULL);
		return NULL;
	}

	// Get interface data for the requested instance
	SP_INTERFACE_DEVICE_DATA ifdata;
	ifdata.cbSize = sizeof(ifdata);
	if(!SetupDiEnumDeviceInterfaces(info, NULL, pGuid, instance, &ifdata))
	{
		MessageBox(hWnd, "No SP_INTERFACE_DEVICE_DATA available for this GUID instance",NULL,NULL);//printf("No SP_INTERFACE_DEVICE_DATA available for this GUID instance\n");
		SetupDiDestroyDeviceInfoList(info);
		return NULL;
	}

	// Get size of symbolic link name
	DWORD ReqLen;
	SetupDiGetDeviceInterfaceDetail(info, &ifdata, NULL, 0, &ReqLen, NULL);
	PSP_INTERFACE_DEVICE_DETAIL_DATA ifDetail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)(new char[ReqLen]);
	if( ifDetail==NULL)
	{
		
		SetupDiDestroyDeviceInfoList(info);
		return NULL;
	}

	// Get symbolic link name
	ifDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
	if( !SetupDiGetDeviceInterfaceDetail(info, &ifdata, ifDetail, ReqLen, NULL, NULL))
	{
		SetupDiDestroyDeviceInfoList(info);
		delete ifDetail;
		return NULL;
	}

	//printf("Symbolic link is %s\n",ifDetail->DevicePath);
	// Open file
	HANDLE rv = CreateFile( ifDetail->DevicePath, 
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if( rv==INVALID_HANDLE_VALUE) rv = NULL;

	delete ifDetail;
	SetupDiDestroyDeviceInfoList(info);
	return rv;
}

int MachinePosPlusWrite(HWND hWnd, double data_mx,double data_my,double data_mz,double data_mb,double data_mc)
	//Write the puls of position to the registers of the NC broad
{
		
	int m_tempx,m_tempy,m_tempz,m_tempb,m_tempc;
	int m_tempxL, m_tempxH;
	int m_tempyL, m_tempyH;
	int m_tempzL, m_tempzH;
	int m_tempbL, m_tempbH;
	int m_tempcL, m_tempcH;
	
	m_tempx=(int)(1000*data_mx);
	m_tempy=(int)(1000*data_my);
	m_tempz=(int)(1000*data_mz);
//	m_tempb=(int)(5000*data_mb);2010-5-25
	m_tempb=(int)(1000*data_mb);//2010-5-25
	m_tempc=(int)(1000 * data_mc);//float to puls then to int;
	
	
	
	
	m_tempxL = m_tempx & 0xFFFF;
	m_tempx=(m_tempx >> 16);
	m_tempxH = m_tempx & 0xFFFF;
	

	m_tempyL = m_tempy & 0xFFFF;
	m_tempy=(m_tempy >> 16);
	m_tempyH = m_tempy & 0xFFFF;
	

   m_tempzL = m_tempz & 0xFFFF;
   m_tempz=(m_tempz >> 16);
   m_tempzH = m_tempz & 0xFFFF;

   m_tempbL = m_tempb & 0xFFFF;
	m_tempb=(m_tempb >> 16);
	m_tempbH = m_tempb & 0xFFFF;

	m_tempcL = m_tempc & 0xFFFF;
	m_tempc=(m_tempc >> 16);
	m_tempcH = m_tempc & 0xFFFF;

   /*
	if(m_tempy>=0)
	 {
		m_tempyL = m_tempy & 0xFF;
	        m_tempy=(m_tempy >> 8);
		m_tempyM = m_tempy & 0xFF;
	        m_tempy=(m_tempy >> 8);
		m_tempyH = m_tempy & 0xFF;
	 }
	if(m_tempy < 0)
	 {
		m_tempy= abs(m_tempy);
		m_tempy=(~(m_tempy-1));
		m_tempyL = m_tempy & 0xFF;
	        m_tempy=(m_tempy >> 8);
		m_tempyM = m_tempy & 0xFF;
	        m_tempy=(m_tempy >> 8);
		m_tempyH = m_tempy & 0xFF;
	 }

	
	if(m_tempz>=0)
	 {
		m_tempzL = m_tempz & 0xFF;
	        m_tempz=(m_tempz >> 8);
		m_tempzM = m_tempz & 0xFF;
	        m_tempz=(m_tempz >> 8);
		m_tempzH = m_tempz & 0xFF;
	 }
	if(m_tempz < 0)
	 {
		m_tempz= abs(m_tempz);
		m_tempz=(~(m_tempz-1));
		m_tempzL = m_tempz & 0xFF;
	        m_tempz=(m_tempz >> 8);
		m_tempzM = m_tempz & 0xFF;
	        m_tempz=(m_tempz >> 8);
		m_tempzH = m_tempz & 0xFF;
	 }
	

	if(m_tempb>=0)
	 {
		m_tempbL = m_tempb & 0xFF;
	        m_tempb=(m_tempb >> 8);
		m_tempbM = m_tempb & 0xFF;
	        m_tempb=(m_tempb >> 8);
		m_tempbH = m_tempb & 0xFF;
	 }
	if(m_tempb < 0)
	 {
		m_tempb= abs(m_tempb);
		m_tempb=(~(m_tempb-1));
		m_tempbL = m_tempb & 0xFF;
	        m_tempb=(m_tempb >> 8);
		m_tempbM = m_tempb & 0xFF;
	        m_tempb=(m_tempb >> 8);
		m_tempbH = m_tempb & 0xFF;
	 }
	
	if(m_tempc>=0)
	 {
		m_tempcL = m_tempc & 0xFF;
	        m_tempc=(m_tempc >> 8);
		m_tempcM = m_tempc & 0xFF;
	        m_tempc=(m_tempc >> 8);
		m_tempcH = m_tempc & 0xFF;
	 }
	if(m_tempc < 0)
	 {
		m_tempc= abs(m_tempc);
		m_tempc=(~(m_tempc-1));
		m_tempcL = m_tempc & 0xFF;
	        m_tempc=(m_tempc >> 8);
		m_tempcM = m_tempc & 0xFF;
	        m_tempc=(m_tempc >> 8);
		m_tempcH = m_tempc & 0xFF;
	 }



   	if(m_tempxL>128) m_tempxL=m_tempxL-256;	
   	if(m_tempxM>128) m_tempxM=m_tempxM-256;	
   	if(m_tempxH>128) m_tempxH=m_tempxH-256;	

   	if(m_tempyL>128) m_tempyL=m_tempyL-256;	
   	if(m_tempyM>128) m_tempyM=m_tempyM-256;	
   	if(m_tempyH>128) m_tempyH=m_tempyH-256;	

   	if(m_tempzL>128) m_tempzL=m_tempzL-256;	
   	if(m_tempzM>128) m_tempzM=m_tempzM-256;	
   	if(m_tempzH>128) m_tempzH=m_tempzH-256;	

   	if(m_tempbL>128) m_tempbL=m_tempbL-256;	
   	if(m_tempbM>128) m_tempbM=m_tempbM-256;	
   	if(m_tempbH>128) m_tempbH=m_tempbH-256;	

   	if(m_tempcL>128) m_tempcL=m_tempcL-256;	
   	if(m_tempcM>128) m_tempcM=m_tempcM-256;	
   	if(m_tempcH>128) m_tempcH=m_tempcH-256;//to assoiate with RTLinux
*/

	iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_XPOS_INIT_L,  m_tempxL);
	iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_XPOS_INIT_H,  m_tempxH);

	iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_YPOS_INIT_L,  m_tempyL);
	iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_YPOS_INIT_H,  m_tempyH);

	iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_ZPOS_INIT_L,  m_tempzL);
	iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_ZPOS_INIT_H,  m_tempzH);

	iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_BPOS_INIT_L,  m_tempbL);
	iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_BPOS_INIT_H,  m_tempbH);

	iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_CPOS_INIT_L,  m_tempcL);
	iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE,  ADR_CPOS_INIT_H,  m_tempcH);	        
	return 0;
 }
//2010-3-30
//本函数功能为初始化加工参数库。
int readCondition()
{
	//加工参数文件保存在condition.csv文件中
	FILE* fpLock,*fpCondition,*fpTemp,*fpTempa,*fpSeek;
	int c;
	ConditionNumber=0;//计算condition.csv文件中加工参数文件数目
	int k=0;
	fpLock=fopen("lock.txt","r");//以读模式打开lock.txt，如果打开失败，说明lock.txt文件不存在，则重新建立lock.txt文件。
	//如果要重新设置所有加工参数文件的锁定状态则只需要在运行程序前将lock.txt文件在、删除即可。
	//如果lock.txt文件不存在，重新建立lock.txt，各加工参数状态标记为非锁定即0,1表示锁定
	if(fpLock==NULL)
	{
		fpLock=fopen("lock.txt","w");
		if(fpLock==NULL)return 1;
		for(c=0;c<conditionLength;c++)putc(int('0'),fpLock);
		fclose(fpLock);
	}
	else fclose(fpLock);
	fpCondition=fopen("condition.csv","r");
	if(fpCondition==NULL)return 1;//读取condition.csv失败返回1
	fpTemp=fopen("temp.txt","w");//temp.txt为加工参数文件一副本，用于在程序运行时修改加工参数，在程序退出时将该文件内容复制至condition.csv文件中
	if(fpTemp==NULL)return 1;//读取temp.txt失败返回1
	fpTempa=fopen("tempa.txt","w");//tempa.txt为加工参数文件另一副本，用于在readCondition操作中协助读取文件
	if(fpTempa==NULL)return 1;//读取tampa.txt失败返回1
	fpSeek=fopen("seek.txt","w");
	if(fpSeek==NULL)return 1;
	c=getc(fpCondition);
	while((c!=int('\n'))&&(c!=int('\r')))
	{
		putc(c,fpTempa);//复制condition.csv第一行至tampa.txt
		c=getc(fpCondition);
	}
	fclose(fpTempa);
	rewind(fpCondition);//将fp指针重置至文件头
	fpTempa=fopen("tempa.txt","r");//重新打开tampa.txt，以读格式
	if(fpTempa==NULL)return 1;//读取tempa.txt失败返回1
	startW(fpTemp,fpSeek,conditionLength);//预设temp.txt和seek.txt文件内容
	rewind(fpCondition);
	rewind(fpTemp);
	rewind(fpSeek);
	rewind(fpTempa);
	copyF(fpCondition,fpTemp,fpTempa,fpSeek);//将condition.csv文件内容复制至temp.txt中
	//关闭各文件
	fclose(fpCondition);
	fclose(fpTemp);
	fclose(fpTempa);
	fclose(fpSeek);
	return 0;
}
//startW操作为预设temp.txt文件内容，写入足以容纳所有加工参数信息的空格，以便后面的读写操作
void startW(FILE* fpTemp,FILE* fpSeek,int startL)
{
	int i,j;
	char a;
	a=' ';
	for(i=0;i<12;i++)
	{
		for(j=1;j<=(startL+1)*length;j++)
		{
			fputc(int(' '),fpTemp);
		}
		fputc(int('\n'),fpTemp);
	}
	for(i=0;i<(conditionLength+1)*12;i++)
	{
		fputc(int(' '),fpSeek);
	}
}

//本函数功能为初始化temp.txt,seek.txt文件内容
void copyF(FILE* fpCondition,FILE* fpTemp,FILE* fpTempa,FILE* fpSeek)
{
	int sign=0;
	while(((char)getc(fpCondition))!=',');//读取第一行第一列的无用信息
	firstLine(fpCondition,fpTemp,fpSeek,sign);//读取第一行加工参数文件标题信息
	otherLine(fpCondition,fpTemp,fpTempa,sign);//读取加工参数信息
}
void firstLine(FILE* fpCondition,FILE* fpTemp,FILE* fpSeek,int& sign)
{
	int now;
	int next;
	int pre;
	char temp[6];
	int i,j;
	sign=0;
	now=0;
	pre=0;
	next=0;
	while(sign!=1)
	{		
		next=getFILE(fpCondition,sign);//得到加工参数文件名序号即E****的****；
		//写入seek.txt用于索引
		//seek.txt框架如下：对于E100文件在seek.txt文件第101*12位置中前6位记录E100前一个文件文件的名称，后6为记录E100后一个文件的名称。如果
		//在condition.csv文件中文件顺序如下E50 E100 E200,则在seek.txt文件中在第101*12至101*12+6位置记录着50，在第101*12+6至102*12位置记录着
		//200。这样即可快速得到所要搜寻的加工参数文件的位置。而不必从1-9999完全搜索。
		//pre当前文件的前一个文件位置
		for(i=0;i<6;i++)temp[i]=NULL;
		itoa(pre,temp,10);
		for(i=0;i<6;i++)
		{
			if(temp[i]=='\0') break;
		}
		fseek(fpSeek,long((now*12)*sizeof(char)),SEEK_SET);
		for(j=0;j<i;j++)fputc(int(temp[j]),fpSeek);
		for(i=0;i<6;i++)temp[i]=NULL;
		itoa(next,temp,10);
		//next、当前文件的后一个文件位置
		for(i=0;i<6;i++)
		{
			if(temp[i]=='\0') break;
		}
		fseek(fpSeek,long((now*12+6)*sizeof(char)),SEEK_SET);
		for(j=0;j<i;j++)fputc(int(temp[j]),fpSeek);
		//将标题信息写入temp.txt文件中
		writeF(fpTemp,next);
		pre=now;
		now=next;
		//sign标记文件标题信息是否读取完毕，读取完毕则为3，退出循环；
		if(sign==3)sign=1;
		ConditionNumber++;//记录conditon.csv文件中的加工参数文件数目，用于设定水平滚动的状态
	}
	//设置seek.txt文件头尾信息。0~6记录最后一个文件位置，6~12记录第一个文件位置
	for(i=0;i<6;i++)temp[i]=NULL;
	itoa(now,temp,10);
	for(i=0;i<6;i++)
	{
		if(temp[i]=='\0') break;
	}
	fseek(fpSeek,long((0*12)*sizeof(char)),SEEK_SET);
	for(j=0;j<i;j++)fputc(int(temp[j]),fpSeek);
	for(i=0;i<6;i++)temp[i]=NULL;
	itoa(pre,temp,10);
	for(i=0;i<6;i++)
	{
		if(temp[i]=='\0') break;
	}
	fseek(fpSeek,long((now*12)*sizeof(char)),SEEK_SET);
	for(j=0;j<i;j++)fputc(int(temp[j]),fpSeek);
	for(i=0;i<6;i++)temp[i]=NULL;
	itoa(0,temp,10);
	for(i=0;i<6;i++)
	{
		if(temp[i]=='\0') break;
	}
	fseek(fpSeek,long((now*12+6)*sizeof(char)),SEEK_SET);
	for(j=0;j<i;j++)fputc(int(temp[j]),fpSeek);
}
//得到加工参数文件标题信息即E****中的****
int getFILE(FILE* fpCondition,int&s)
{
	int i;//临时变量
	char temp[10];//临时变量
	//获得首字母E，直接丢弃
	while((char)fgetc(fpCondition)!='E');
	for(i=0;i<10;i++)
	{
		temp[i]=(char)fgetc(fpCondition);
		if(temp[i]==',')
		{
			temp[i]='\0';
			s=2;
			break;
		}
		//得到'\n'或'\r'表示第一行读取完毕
		else if(temp[i]=='\n'||temp[i]=='\r')
		{
			if(temp[i]=='\r')fgetc(fpCondition);
			temp[i]='\0';
			s=3;
			break;
		}
		//得到'\n'或'\r'表示第一行读取完毕
		else if(temp[i]==EOF)
		{
			temp[i]='\0';
			s=3;
			break;
		}
	}
	return atoi(temp);
}

//本函数功能为将标题信息读入temp.txt文件中
void writeF(FILE* fpTemp,int length)
{
	char temp[5];
	int i,j;
	for(i=0;i<5;i++)temp[i]=NULL;
	//定位指针至该文件序列位置
	fseek(fpTemp,long((length*6)*sizeof(char)),SEEK_SET);
	itoa(length,temp,10);
	for(i=0;i<5;i++)
	{
		if(temp[i]=='\0') break;
	}
	fputc(int(','),fpTemp);
	fputc(int('E'),fpTemp);
	for(j=0;j<i;j++)fputc(int(temp[j]),fpTemp);
}
//本函数功能为将加工参数信息写入temp.txt文件中
void otherLine(FILE* fpCondition,FILE* fpTemp,FILE* fpTempa,int&sign)
{
	int count;
	int i,j;//临时变量
	int temp;
	char charT[10];
	char a;
	count=1;
	while(count<countNumber+1)
	{
		rewind(fpTempa);
		fseek(fpTemp,long((((conditionLength+1)*length+2)*count)*sizeof(char)),SEEK_SET);//定位temp.txt指针
		sign=0;
		//写入加工参数文件项目名称
		while((temp=fgetc(fpCondition))!=int(','))
		{
			a=char(temp);
			fputc(int(temp),fpTemp);
		}
		while(((char)fgetc(fpTempa))!=',');
		//写入加工参数
		while(sign==0)
		{
			temp=getFILE(fpTempa,sign);//得到该参数所对应的文件名称
			if(sign==2)sign=0;
			else if(sign==3)sign=1;
			for(i=0;i<5;i++)charT[i]=NULL;
			i=getC(fpCondition);//得到参数
			itoa(i,charT,10);
			for(i=0;i<5;i++)
			{
				if(charT[i]=='\0') break;
			}
			fseek(fpTemp,long((((conditionLength+1)*length+2)*count+temp*length)*sizeof(char)),SEEK_SET);//定位temp.txt指针
			fputc(int(','),fpTemp);
			for(j=0;j<i;j++)fputc(int(charT[j]),fpTemp);
		}
		count++;
	}
}
//读取参数
int getC(FILE* fpCondition)
{
	int i;//临时变量
	char temp[10];//临时变量
	for(i=0;i<10;i++)
	{
		temp[i]=(char)fgetc(fpCondition);
		if(temp[i]==',')
		{
			temp[i]='\0';
			break;
		}
		else if((temp[i]=='\n')||(temp[i]=='\r'))
		{
			if(temp[i]=='\r')fgetc(fpCondition);
			temp[i]='\0';
			break;
		}
		else if(temp[i]==EOF)
		{
			temp[i]='\0';
			break;
		}
	}
	return atoi(temp);//atoi()将字符串转化为整数
}
//2010-3-30