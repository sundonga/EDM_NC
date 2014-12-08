// EDM_NC_PROGRAM.cpp : ����Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"



// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������


HANDLE IOPCIhDevice;               // IO_PCI�豸��� 
HANDLE NCPCIhDevice;                // NC_PCI�豸��� 
HANDLE PowerPCIhDevice;                // NC_PCI�豸��� 
HANDLE hDecodeEvent;  //��������λ���������������߳��¼��ľ�� 


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
int readCondition();//����������Ϊ��ʼ���ӹ������⡣
int length=6;//�涨ÿ�����ݶ�д�볤��Ϊ6���ֽڣ����С=�ӹ������ļ����λ��+2
int conditionLength=9999;//�趨�ӹ������ļ������ĿΪ9999��
int countNumber=11;//��Ŀ��11����Ŀ
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

FILE* fpTemp;//��λtemp.txt�ļ���ָ��
FILE* fpSeek;//��λseek.txt�ļ���ָ
FILE* fpLock;//��λlock.txt�ļ���ָ��
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
 	// TODO: �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;
    
	//HWND hWnd;
	
	MachineRead(0);  //���ػ�������鱨
	//2010-3-30
	if(readCondition()==1)return 3;//��ȡ�ӹ������ļ�ʧ�ܷ���3 
//�Զ�дģʽ�򿪸��ļ�������temp.txtΪcondition.csv�ļ��ĸ���
//seek.txt���ڱ�Ǹ��ӹ�������temp.txt�е�λ�ã����ṩ���ٲ�����Ѱ����
//lock.txt���ڱ�Ǹ������ļ��Ƿ�����
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
//	MILL_ESPARK=1;//2010-5-25 1��ʾϵͳ��ʼĬ��Ϊϳ��ϵͳ��0��ʾ���и�ϵͳ
    IinitProgram(0);  //�������س�����Ӳ���ĳ�ʼ��		
	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_EDM_NC_PROGRAM, szWindowClass, MAX_LOADSTRING);

	

	// �ж�ע�ᴰ�ڳɹ��񣬲��ɹ�����1���˳�
	if(!MyRegisterClassMain(hInstance)) return 1;

	//�жϴ��������ڲ���ʾ�����ڳɹ���ʧ�ܣ�����ֵ2���˳�
	if(!showMainWnd(hInstance,lpCmdLine,nCmdShow)) return 2;

	
	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_EDM_NC_PROGRAM);
	
	
	// ����Ϣѭ��:
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
	startR=atoi(data[12+i]);//��ȡ�ػ�ʱ�ӹ���������״̬ by lsw 2010-4-6
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
int IinitProgram(HWND hWnd)  //�������س�����Ӳ���ĳ�ʼ��
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


	//����PCI_NC���豸

	NCPCIhDevice = GetDeviceViaInterface(hWnd,(LPGUID)&NCCtrlDevice,0);

	if (NCPCIhDevice == INVALID_HANDLE_VALUE) // cannot open the drive
	{
		MessageBox(hWnd,"can not create  NCPCIhDevice in IinitProgram function",NULL,NULL);
		return 1;
	}
	
	//����PCI_IO���豸
	
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
	DspProgramLoad(hWnd);  //ͨ��HPI�ӿ���DSP���س���	
//	DspProgramLoad1(hWnd);  //ͨ��HPI�ӿ���PowerDSP���س���	
	//���豸��д��ӹ������ṹ���У�Ҫд30��
	//Machiningparam.item[0].hDevice = IOPCIhDevice;
	//Machiningparam.item[1].hDevice = IOPCIhDevice;
/************I/O�����***************************/



  //iowrite(hWnd,IOPCIhDevice,IO_FPGA_WRITE, ADR_OUTB,  0x0);
 // iowrite(hWnd,IOPCIhDevice,IO_FPGA_WRITE, ADR_OUTB,  0x13);
  iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE, ADR_CONT_Star_Stop,  0x01);//�´�reset�ź�


	
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


/*********������λ��飬��������ʾ****************/	
	
	limit = ioread(hWnd,NCPCIhDevice,NC_FPGA_READ,ADR_LIMIT );
	//if(3==limit)  {SetWindowText(GetDlgItem(hMainWnd,130),"OK,no limit!");}  

	if(7 ==limit) 	MessageBox(hWnd,"Waring: Z- limit !", NULL,NULL);

	if(11==limit)   MessageBox(hWnd,"Waring: Z+ limit !", NULL,NULL);
	if(19==limit) MessageBox(hWnd,"Waring: Y- limit !", NULL,NULL);
	if(35==limit) MessageBox(hWnd,"Waring: Y+ limit !", NULL,NULL);
	if(67==limit) MessageBox(hWnd,"Waring: X- limit !", NULL,NULL);
	if(131==limit) MessageBox(hWnd,"Waring: X+ limit !", NULL,NULL); //������λ���;	

	/***************д������ת�ı����ź�*************************/
	 iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE, ADR_SRV_ON,  0x0);
	 iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE, ADR_SRV_ON,  0x01F);
	 iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE, ADR_SRV_INH,  0x01F);
	 iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE, ADR_SOFTLIM,  0x01);  //����λ�ź�
	// iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE, ADR_SRV_CWL,  0x01F);   //FPGA�ڲ�������λ�źţ�����Ҫ��λ������
	// iowrite(hWnd,NCPCIhDevice,NC_FPGA_WRITE, ADR_SRV_CCWL,  0x01F);	

	/******************�ӹ��������******************/

		//���ӹ���������ȫ�ֱ����ӹ������ṹ�岢���ṹ��ֵ������豸

	/******************��ʼ��λ�üĴ�����ֵ********************/
	 
	 MachinePosPlusWrite(hWnd,data_m.x,data_m.y,data_m.z,data_m.b,data_m.c);	//д�������굽NC��

	 hDecodeEvent = CreateEvent(NULL,FALSE,FALSE,NULL); 
	if(hDecodeEvent == NULL)
	{
			MessageBox(hWnd,"can not create  hDecodeEvent in IinitProgram function",NULL,NULL);
			return 1;
	}

	SendNCDriverUserDecodeEvent(hWnd,hDecodeEvent); //�����������´������������������̵߳��¼����
	
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
//����������Ϊ��ʼ���ӹ������⡣
int readCondition()
{
	//�ӹ������ļ�������condition.csv�ļ���
	FILE* fpLock,*fpCondition,*fpTemp,*fpTempa,*fpSeek;
	int c;
	ConditionNumber=0;//����condition.csv�ļ��мӹ������ļ���Ŀ
	int k=0;
	fpLock=fopen("lock.txt","r");//�Զ�ģʽ��lock.txt�������ʧ�ܣ�˵��lock.txt�ļ������ڣ������½���lock.txt�ļ���
	//���Ҫ�����������мӹ������ļ�������״̬��ֻ��Ҫ�����г���ǰ��lock.txt�ļ��ڡ�ɾ�����ɡ�
	//���lock.txt�ļ������ڣ����½���lock.txt�����ӹ�����״̬���Ϊ��������0,1��ʾ����
	if(fpLock==NULL)
	{
		fpLock=fopen("lock.txt","w");
		if(fpLock==NULL)return 1;
		for(c=0;c<conditionLength;c++)putc(int('0'),fpLock);
		fclose(fpLock);
	}
	else fclose(fpLock);
	fpCondition=fopen("condition.csv","r");
	if(fpCondition==NULL)return 1;//��ȡcondition.csvʧ�ܷ���1
	fpTemp=fopen("temp.txt","w");//temp.txtΪ�ӹ������ļ�һ�����������ڳ�������ʱ�޸ļӹ��������ڳ����˳�ʱ�����ļ����ݸ�����condition.csv�ļ���
	if(fpTemp==NULL)return 1;//��ȡtemp.txtʧ�ܷ���1
	fpTempa=fopen("tempa.txt","w");//tempa.txtΪ�ӹ������ļ���һ������������readCondition������Э����ȡ�ļ�
	if(fpTempa==NULL)return 1;//��ȡtampa.txtʧ�ܷ���1
	fpSeek=fopen("seek.txt","w");
	if(fpSeek==NULL)return 1;
	c=getc(fpCondition);
	while((c!=int('\n'))&&(c!=int('\r')))
	{
		putc(c,fpTempa);//����condition.csv��һ����tampa.txt
		c=getc(fpCondition);
	}
	fclose(fpTempa);
	rewind(fpCondition);//��fpָ���������ļ�ͷ
	fpTempa=fopen("tempa.txt","r");//���´�tampa.txt���Զ���ʽ
	if(fpTempa==NULL)return 1;//��ȡtempa.txtʧ�ܷ���1
	startW(fpTemp,fpSeek,conditionLength);//Ԥ��temp.txt��seek.txt�ļ�����
	rewind(fpCondition);
	rewind(fpTemp);
	rewind(fpSeek);
	rewind(fpTempa);
	copyF(fpCondition,fpTemp,fpTempa,fpSeek);//��condition.csv�ļ����ݸ�����temp.txt��
	//�رո��ļ�
	fclose(fpCondition);
	fclose(fpTemp);
	fclose(fpTempa);
	fclose(fpSeek);
	return 0;
}
//startW����ΪԤ��temp.txt�ļ����ݣ�д�������������мӹ�������Ϣ�Ŀո��Ա����Ķ�д����
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

//����������Ϊ��ʼ��temp.txt,seek.txt�ļ�����
void copyF(FILE* fpCondition,FILE* fpTemp,FILE* fpTempa,FILE* fpSeek)
{
	int sign=0;
	while(((char)getc(fpCondition))!=',');//��ȡ��һ�е�һ�е�������Ϣ
	firstLine(fpCondition,fpTemp,fpSeek,sign);//��ȡ��һ�мӹ������ļ�������Ϣ
	otherLine(fpCondition,fpTemp,fpTempa,sign);//��ȡ�ӹ�������Ϣ
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
		next=getFILE(fpCondition,sign);//�õ��ӹ������ļ�����ż�E****��****��
		//д��seek.txt��������
		//seek.txt������£�����E100�ļ���seek.txt�ļ���101*12λ����ǰ6λ��¼E100ǰһ���ļ��ļ������ƣ���6Ϊ��¼E100��һ���ļ������ơ����
		//��condition.csv�ļ����ļ�˳������E50 E100 E200,����seek.txt�ļ����ڵ�101*12��101*12+6λ�ü�¼��50���ڵ�101*12+6��102*12λ�ü�¼��
		//200���������ɿ��ٵõ���Ҫ��Ѱ�ļӹ������ļ���λ�á������ش�1-9999��ȫ������
		//pre��ǰ�ļ���ǰһ���ļ�λ��
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
		//next����ǰ�ļ��ĺ�һ���ļ�λ��
		for(i=0;i<6;i++)
		{
			if(temp[i]=='\0') break;
		}
		fseek(fpSeek,long((now*12+6)*sizeof(char)),SEEK_SET);
		for(j=0;j<i;j++)fputc(int(temp[j]),fpSeek);
		//��������Ϣд��temp.txt�ļ���
		writeF(fpTemp,next);
		pre=now;
		now=next;
		//sign����ļ�������Ϣ�Ƿ��ȡ��ϣ���ȡ�����Ϊ3���˳�ѭ����
		if(sign==3)sign=1;
		ConditionNumber++;//��¼conditon.csv�ļ��еļӹ������ļ���Ŀ�������趨ˮƽ������״̬
	}
	//����seek.txt�ļ�ͷβ��Ϣ��0~6��¼���һ���ļ�λ�ã�6~12��¼��һ���ļ�λ��
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
//�õ��ӹ������ļ�������Ϣ��E****�е�****
int getFILE(FILE* fpCondition,int&s)
{
	int i;//��ʱ����
	char temp[10];//��ʱ����
	//�������ĸE��ֱ�Ӷ���
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
		//�õ�'\n'��'\r'��ʾ��һ�ж�ȡ���
		else if(temp[i]=='\n'||temp[i]=='\r')
		{
			if(temp[i]=='\r')fgetc(fpCondition);
			temp[i]='\0';
			s=3;
			break;
		}
		//�õ�'\n'��'\r'��ʾ��һ�ж�ȡ���
		else if(temp[i]==EOF)
		{
			temp[i]='\0';
			s=3;
			break;
		}
	}
	return atoi(temp);
}

//����������Ϊ��������Ϣ����temp.txt�ļ���
void writeF(FILE* fpTemp,int length)
{
	char temp[5];
	int i,j;
	for(i=0;i<5;i++)temp[i]=NULL;
	//��λָ�������ļ�����λ��
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
//����������Ϊ���ӹ�������Ϣд��temp.txt�ļ���
void otherLine(FILE* fpCondition,FILE* fpTemp,FILE* fpTempa,int&sign)
{
	int count;
	int i,j;//��ʱ����
	int temp;
	char charT[10];
	char a;
	count=1;
	while(count<countNumber+1)
	{
		rewind(fpTempa);
		fseek(fpTemp,long((((conditionLength+1)*length+2)*count)*sizeof(char)),SEEK_SET);//��λtemp.txtָ��
		sign=0;
		//д��ӹ������ļ���Ŀ����
		while((temp=fgetc(fpCondition))!=int(','))
		{
			a=char(temp);
			fputc(int(temp),fpTemp);
		}
		while(((char)fgetc(fpTempa))!=',');
		//д��ӹ�����
		while(sign==0)
		{
			temp=getFILE(fpTempa,sign);//�õ��ò�������Ӧ���ļ�����
			if(sign==2)sign=0;
			else if(sign==3)sign=1;
			for(i=0;i<5;i++)charT[i]=NULL;
			i=getC(fpCondition);//�õ�����
			itoa(i,charT,10);
			for(i=0;i<5;i++)
			{
				if(charT[i]=='\0') break;
			}
			fseek(fpTemp,long((((conditionLength+1)*length+2)*count+temp*length)*sizeof(char)),SEEK_SET);//��λtemp.txtָ��
			fputc(int(','),fpTemp);
			for(j=0;j<i;j++)fputc(int(charT[j]),fpTemp);
		}
		count++;
	}
}
//��ȡ����
int getC(FILE* fpCondition)
{
	int i;//��ʱ����
	char temp[10];//��ʱ����
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
	return atoi(temp);//atoi()���ַ���ת��Ϊ����
}
//2010-3-30