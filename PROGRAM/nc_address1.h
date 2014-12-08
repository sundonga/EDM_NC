/* NC-ISA Address */
#ifndef NC_ADDRESS_H	// ��ֹͷ�ļ��ظ�����
#define NC_ADDRESS_H

//�ٶȼĴ���

//DSP�����ٶȵĵͰ�λ�͸߰�λ�����ַ;
#define ADR_XVEL_L 0x10
#define ADR_YVEL_L 0x14
#define ADR_ZVEL_L 0x18
#define ADR_BVEL_L 0x1C
#define ADR_CVEL_L 0x20



//�岹��������������
#define ADR_XPUL_L 0x30
#define ADR_YPUL_L 0x34
#define ADR_ZPUL_L 0x38
#define ADR_BPUL_L 0x3C
#define ADR_CPUL_L 0x40



//��λ��岹�յ��ж�
#define ADR_LIMIT 0x50	
#define ADR_PULSENDSTART 0x54 
#define ADR_SpeedEnd_Flag 0x58//
#define ADR_SpeedStart_Flag 0x5C//������ַ�����Ƿ�����ٶȼ�����,ֵ��Ϊ0ʱ�ٶ��̲߳ſ��Խ����ٶȷ���;

/*NC��RAS��ַ�б�*/
//�������ʵ��λ�õ� ��16λ
#define ADR_XPOS_L 0X100
#define ADR_YPOS_L 0X104
#define ADR_ZPOS_L 0X108
#define ADR_BPOS_L 0X10C
#define ADR_CPOS_L 0X110
//**************************************************************

#define ADR_XPOS_OUT_L 0X00
#define ADR_YPOS_OUT_L 0X04
#define ADR_ZPOS_OUT_L 0X08  //��NC�������������Ĵ����еı���ֵ



//�����16λ
#define ADR_XPOS_H 0X120
#define ADR_YPOS_H 0X124
#define ADR_ZPOS_H 0X128
#define ADR_BPOS_H 0X12C
#define ADR_CPOS_H 0X130



//���㴦��
#define ADR_RETURN_ZERO 0X1A0  //д�����ָ��
#define ADR_RETURN_ZERO_DIR 0X1A4  //д����㷽��
#define ADR_ZERO_IRQ 0X1F0   //��ȡ�Ĵ�����Ϣ�ж��Ǻ��ᵽ�����  
#define ADR_RETURN_ZERO_SPEED 0X1F4 //ѡ������ٶ�

//����ʱλ�ü������ü������Ĵ���,X/Y/Zʹ������������,B/Cʹ������
#define ADR_XPOS_CLR_L_FIR 0X1B0  
#define ADR_YPOS_CLR_L_FIR 0X1B4  
#define ADR_ZPOS_CLR_L_FIR 0X1B8  
#define ADR_BPOS_CLR_L_FIR 0X1E0  
#define ADR_CPOS_CLR_L_FIR 0X1E8  



#define ADR_XPOS_CLR_L_SEC 0X1C0  
#define ADR_YPOS_CLR_L_SEC 0X1C4  
#define ADR_ZPOS_CLR_L_SEC 0X1C8  
#define ADR_BPOS_CLR_L_SEC 0X1E4  
#define ADR_CPOS_CLR_L_SEC 0X1EC  

#define ADR_XPOS_CLR_L_THI 0X1D0  
#define ADR_YPOS_CLR_L_THI 0X1D4 
#define ADR_ZPOS_CLR_L_THI 0X1D8  
  

//#define ADR_DSP_OPENVELL	0X250
//#define ADR_DSP_OPENVELH	0X251
//#define ADR_DSP_NORMALVELL	0X252
//#define ADR_DSP_NORMALVELH	0X253
//#define ADR_DSP_RETROGRADEVELL	0X254
//#define ADR_DSP_RETROGRADEVELH	0X255
//#define ADR_DSP_RETADFLAG	0X256


//д����������ĳ�ʼλ��
#define ADR_XPOS_INIT_L 0X140
#define ADR_YPOS_INIT_L 0X144
#define ADR_ZPOS_INIT_L 0X148
#define ADR_BPOS_INIT_L 0X14C
#define ADR_CPOS_INIT_L 0X150


#define ADR_XPOS_INIT_H 0X160
#define ADR_YPOS_INIT_H 0X164
#define ADR_ZPOS_INIT_H 0X168
#define ADR_BPOS_INIT_H 0X16C
#define ADR_CPOS_INIT_H 0X170


//#define ADR_CPOS_INIT_OUT_H 0x28A

//DDA���ֶ�����ת����ַ
#define ADR_DDATOMANUAL 0X64   // 0Ϊ�ֶ�,1ΪDDA

#define ADR_DATA_STARTEND 0X68  //���ݿ�ʼ��������͵�ַ,ȷ������ͬʱ��ʼ�岹

//IQR3ʹ�ܵ�ַ
#define ADR_IRQ3_ENABLE  0x24F  //1�������ж�,0��������

#define ADR_SOFTLIM 0X78   //����λ�Ƿ�������,'1'Ϊû������λ,'0'Ϊ����λ������;

//main��Start��Stop�ź�;
#define ADR_CONT_Star_Stop 0X074
//���ʹ���ź�
#define ADR_SRV_ON 0X6C     //SRV_ON
//#define ADR_SRV_CWL 0x260    //��ת��λ
//#define ADR_SRV_CCWL 0x261   //��ת��λ
#define ADR_SRV_INH 0X70     //�����ֹ/��Ч�ź�

//��·�������ź�,���ź�ԭ�������ڲ������������ǰ���·���RSTn�ź�,����Ŀǰ����������,��ʱ����
//#define ADR_BOARD_RSTN 0x269

//������֤��ַ,�ɶ�ȡ�ٶȼĴ����Ͳ岹���������Ĵ����ĵװ�λ,��ǰ���Ե�·����,Ŀǰ�Ա���
//#define ADR_TEMP 0x22C

#define ADR_TEMPD  0X0200    //�������źŵ�ַ
#define ADR_TEMPE  0X0204

//IO�����źŵ�ַ
//�ֶ�������

#define ADR_MANUAL_BORD_SPEED 0x2B5
#define ADR_MANUAL_BORD_AXIS  0x2B4


#define ADR_INA 	0X0A
#define ADR_INB		0X0B
#define ADR_OUTA 	0X00
#define ADR_OUTB 	0X01
#define ADR_OUTC	0X02
#define ADR_OUTD 	0X03
#define ADR_OUTH 	0X07


#define DSP_PARAMETER_DECODE_NUM_ADR 0X00
#define DSP_PARAMETER_VSR_V_ADR 0X04
#define DSP_PARAMETER_DDA_Vcontrol_ADR 0X08
#define DSP_PARAMETER_SpeedChk_Flag_ADR 0X0C
#define DSP_PARAMETER_Reset_Flag_ADR 0X10
#define DSP_PARAMETER_Short_Stop_Flag_ADR 0X14
#define DSP_PARAMETER_Short_Start_Flag_ADR 0X18
#define DSP_PARAMETER_Frst_Start_Decode_Flag_ADR 0X1C
#define DSP_PARAMETER_Write_Decode_End_Flag_ADR 0X20

#endif
                      
