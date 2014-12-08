/* NC-ISA Address */
#ifndef NC_ADDRESS_H	// 防止头文件重复引用
#define NC_ADDRESS_H


//速度寄存器

//DSP检测的速度的低八位和高八位读入地址;
const int ADR_XVEL_L = 0x10;
const int ADR_YVEL_L = 0x14;
const int ADR_ZVEL_L = 0x18;
const int ADR_BVEL_L = 0x1C;
const int ADR_CVEL_L = 0x20;



//插补过程所走脉冲数
const int ADR_XPUL_L = 0x30;
const int ADR_YPUL_L = 0x34;
const int ADR_ZPUL_L = 0x38;
const int ADR_BPUL_L = 0x3C;
const int ADR_CPUL_L = 0x40;



//限位与插补终点判断
const int ADR_LIMIT = 0x50;	
const int ADR_PULSENDSTART = 0x54; 
const int ADR_SpeedEnd_Flag = 0x58;//
const int ADR_SpeedStart_Flag = 0x5C;//此两地址决定是否进行速度检测分配,值都为0时速度线程才可以进行速度分配;

/*NC板RAS地址列表*/
//读入各轴实际位置的 底16位
const int ADR_XPOS_L = 0X100;
const int ADR_YPOS_L = 0X104;
const int ADR_ZPOS_L = 0X108;
const int ADR_BPOS_L = 0X10C;
const int ADR_CPOS_L = 0X110;
//**************************************************************

const int ADR_XPOS_OUT_L = 0X00;
const int ADR_YPOS_OUT_L = 0X04;
const int ADR_ZPOS_OUT_L = 0X08;  //读NC板中脉冲总数寄存器中的变量值



//读入高16位
const int ADR_XPOS_H = 0X120;
const int ADR_YPOS_H = 0X124;
const int ADR_ZPOS_H = 0X128;
const int ADR_BPOS_H = 0X12C;
const int ADR_CPOS_H = 0X130;



//归零处理
const int ADR_RETURN_ZERO = 0X1A0;  //写入归零指令
const int ADR_RETURN_ZERO_DIR = 0X1A4;  //写入归零方向
const int ADR_ZERO_IRQ = 0X1F0;   //读取寄存器信息判断是何轴到达零点  
const int ADR_RETURN_ZERO_SPEED = 0X1F4; //选择归零速度

//清零时位置计算所用计数器寄存器,X/Y/Z使用三个计数器,B/C使用两个
const int ADR_XPOS_CLR_L_FIR = 0X1B0;  
const int ADR_YPOS_CLR_L_FIR = 0X1B4;  
const int ADR_ZPOS_CLR_L_FIR = 0X1B8;  
const int ADR_BPOS_CLR_L_FIR = 0X1E0;  
const int ADR_CPOS_CLR_L_FIR = 0X1E8; 



const int ADR_XPOS_CLR_L_SEC = 0X1C0;  
const int ADR_YPOS_CLR_L_SEC = 0X1C4;  
const int ADR_ZPOS_CLR_L_SEC = 0X1C8;  
const int ADR_BPOS_CLR_L_SEC = 0X1E4;  
const int ADR_CPOS_CLR_L_SEC = 0X1EC;  

const int ADR_XPOS_CLR_L_THI = 0X1D0;  
const int ADR_YPOS_CLR_L_THI = 0X1D4; 
const int ADR_ZPOS_CLR_L_THI = 0X1D8;  
  

//const int ADR_DSP_OPENVELL	0X250
//const int ADR_DSP_OPENVELH	0X251
//const int ADR_DSP_NORMALVELL	0X252
//const int ADR_DSP_NORMALVELH	0X253
//const int ADR_DSP_RETROGRADEVELL	0X254
//const int ADR_DSP_RETROGRADEVELH	0X255
//const int ADR_DSP_RETADFLAG	0X256


//写入归零后机床的初始位置
const int ADR_XPOS_INIT_L = 0X140;
const int ADR_YPOS_INIT_L = 0X144;
const int ADR_ZPOS_INIT_L = 0X148;
const int ADR_BPOS_INIT_L = 0X14C;
const int ADR_CPOS_INIT_L = 0X150;


const int ADR_XPOS_INIT_H = 0X160;
const int ADR_YPOS_INIT_H = 0X164;
const int ADR_ZPOS_INIT_H = 0X168;
const int ADR_BPOS_INIT_H = 0X16C;
const int ADR_CPOS_INIT_H = 0X170;

const int ADR_C_IN_RAS_A = 0X300;
const int ADR_C_IN_RAS_B = 0X304;
const int ADR_A_ENC_B = 0X308;
const int ADR_B_ENC_A = 0X30C;

const int ADR_C_IN_RAS_Z = 0X310;
const int ADR_B_ENC_Z = 0X314;
const int ADR_B_ENC_B = 0X318;
const int ADR_A_ENC_A = 0X31C;

//const int ADR_CPOS_INIT_OUT_H 0x28A

//DDA与手动调节转换地址
const int ADR_DDATOMANUAL = 0X64;  // 0为手动,1为DDA

const int ADR_DATA_STARTEND = 0X68;  //数据开始与结束传送地址,确保五轴同时开始插补

//IQR3使能地址
const int ADR_IQR4_ENABLE  = 0X60;  //1允许触发中断,0不允许触发

const int ADR_SOFTLIM = 0X78;   //软限位是否起作用,'1'为没有软限位,'0'为软限位起作用;

//main中Start和Stop信号;
const int ADR_CONT_Star_Stop = 0X074;
//电机使能信号
const int ADR_SRV_ON = 0X6C;     //SRV_ON
//const int ADR_SRV_ON = 0X6C;     //SRV_ON
//const int ADR_SRV_CWL 0x260    //正转限位
//const int ADR_SRV_CCWL 0x261   //反转限位
const int ADR_SRV_INH = 0X70;     //脉冲禁止/有效信号

//电路板重置信号,此信号原本打算在不重启计算机的前提下发出RSTn信号,不过目前看来不好用,暂时保留
//const int ADR_BOARD_RSTN 0x269

//读入验证地址,可读取速度寄存器和插补脉冲总数寄存器的底八位,以前调试电路板用,目前仍保留
//const int ADR_TEMP 0x22C


const int ADR_test =		0X01;  //检测延迟
const int ADR_ab_con =		0X02; //异常导通
const int ADR_ab_stop =		0X03; //异常消电离
const int ADR_nor_con =		0X04; //正常低压导通
const int ADR_nor_stop =	0X05; //正常低压消电离
const int ADR_gap =			0X06; //每个IGBT开启间隔
const int ADR_I =			0X07; //igbt开启数量
const int ADR_start =		0X08; //电源开启信号

const int ADR_TEMPD  = 0X0200;    //测试用信号地址
const int ADR_TEMPE  = 0X0204;

//IO板用信号地址
//手动操作板

const int ADR_MANUAL_BORD_SPEED = 0x09;
const int ADR_MANUAL_BORD_AXIS  = 0x0A;


const int ADR_INA 	= 0X0B;
const int ADR_INB	= 0X0C;
const int ADR_OUTA 	= 0X0D;
const int ADR_OUTB  = 0X0E;
const int ADR_OUTC	= 0X0F;
//const int ADR_OUTD 	= 0X03;
//const int ADR_OUTH 	= 0X07;


const int DSP_PARAMETER_DECODE_NUM_ADR = 0X00;
const int DSP_PARAMETER_VSR_V_ADR = 0X04;
const int DSP_PARAMETER_DDA_Vcontrol_ADR = 0X08;
const int DSP_PARAMETER_SpeedChk_Flag_ADR = 0X0C;
const int DSP_PARAMETER_Reset_Flag_ADR = 0X10;
const int DSP_PARAMETER_Short_Stop_Flag_ADR = 0X14;
const int DSP_PARAMETER_Short_Start_Flag_ADR = 0X18;
const int DSP_PARAMETER_Frst_Start_Decode_Flag_ADR = 0X1C;
const int DSP_PARAMETER_Write_Decode_End_Flag_ADR = 0X20;
const int DSP_PARAMETER_Machining_Row_ADR = 0X24;
const int DSP_PARAMETER_Machining_ParaEnd_Flag_ADR = 0X28;
const int DSP_PARAMETER_Machining_M17_Flag_ADR = 0X2C;
const int DSP_PARAMETER_Hint_num = 0X30;
const int DSP_PARAMETER_DownTime = 0X34;
const int DSP_PARAMETER_Jump_Height = 0X38;
const int DSP_PARAMETER_DownTime_Flag = 0X3C;
const int DSP_PARAMETER_Sway_R = 0X40;
const int DSP_PARAMETER_Sway_End = 0X44;
const int DSP_PARAMETER_DownTime_Z_or_XYZ = 0X48;
const int DSP_PARAMETER_Sway_Switch_Flag = 0X4C;
const int DSP_PARAMETER_Jump_Select_Center = 0X50;
const int DSP_PARAMETER_Jump_Vel_Max = 0X54;
const int DSP_PARAMETER_Sway_FreeOr3D = 0X58;
const int DSP_PARAMETER_Sway_Polygon_N = 0X5C;
const int DSP_PARAMETER_Sway_F_Mode = 0X60;
const int DSP_PARAMETER_Sway_A = 0X64;
const int DSP_PARAMETER_Sway_A_Type = 0X68;
#endif
                      
