
#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;	

//extern TapeParam tapepm;  //锥度加工参数2010-5-28

//2010-5-26 锥加工参数修改
extern double z_position;
extern double xy_plane;
extern double h_thickness;
extern double HLOWDISTANCE;
//end 2010-5-26

//2010-5-18
extern double ZERO;//浮点计算时0
extern double calculateLineArc(double PointX,double PointY);//计算直线与X轴正方向逆时针夹角
extern double calculateArc(double centerPointX,double centerPointY,double StartPointX,double StartPointY,double endPointX,
					double endPointY,int circularMode);//计算圆弧角度，其中circularMode参数定义了圆弧是顺园还是逆圆，定义同G03,G02
extern double calculateChordLength(double R,double angle);//计算弦长
extern void calculateEndPoint(double NowArc,double PreArc,double R,double &PointX,double &PointY);//计算圆周上一点坐标
extern int NowGraphPlane;//2010-5-21
//*******************************************
//2010-5-18
extern int MILL_ESPARK;//2010-5-25
extern Coordata data_m;  //Machine coor
extern Coordata data_w;   //work coor
extern Coordata data_r;   //ralitive coor
extern Coordata data_coorswitch;
extern void fc_upday(HWND hWnd);
extern void readbuffer_to_fc(HWND hWnd);
extern HWND hWndCoor;
Coordata G54_coordinate,G55_coordinate,G56_coordinate,G57_coordinate,G58_coordinate,G59_coordinate;
Coordata Absolute_coordinate;

int G5xFlag = 54;

inline double Gcode2d(char* s, const char* gcode); /*将字符串s中字符串gcode后面的实型数字返回, gcode必须是一个字符!*/
inline int Gcode2i(char* s, const char* gcode); /*将字符串s中字符串gcode后面的整形数字返回, gcode必须是一个字符!*/
int stringnum(char *p, const char* s); /*返回字符串p中含有字符串s的个数, s必须是一个字符*/
void  gcodeDecode(HWND hWnd, char *pheadChild, nc_data *dataChild,int *G_numChild,int All_numChild,M_data *M_Child);
void  gcodeDecodeadd(HWND hWnd, char *pheadChild, nc_data *dataChild,int *old_numChild );
void g5152Fuction(HWND hWnd,int *sign, nc_data *dataChild,nc_data asChild[],int *num,double *Start_X, double *Start_Y,int *c,int *first5152flag,nc_data *csChild,double *Start_B,double  *Start_C);
void DmainFuction(HWND hWnd,int *sign, nc_data *dataChild,nc_data asChild[],int *num,double *Start_X, double *Start_Y,int *c, nc_data *csChild);

void g151152upFuction(HWND hWnd,int *sign, nc_data *dataChild,nc_data asChild[],int *num,double *Start_B, double *Start_C,int *c, nc_data *csChild);
void g151152lowFuction(HWND hWnd,int *sign, nc_data *dataChild,nc_data asChild[],int *num,double *Start_X, double *Start_Y,int *c, nc_data *csChild);

void g24Fuction(HWND hWnd,nc_data *dataChild,nc_data asChild[],int *num,nc_data *csChild,nc_data *tempdata,nc_data *ProData);
void CopeWith_G5xCode(HWND hWnd,Coordata Absolute_coor,Coordata G5x_coordinate,nc_data Indata[],nc_data Outdata[],int num);
void CopeWith_G92Code(HWND hWnd,Coordata Absolute_coor,Coordata G5x_coordinate,Coordata coorswitch_to_zero, Coordata G9xcode_temp);

int NowGraphPlane;

LRESULT DSP_Compute(HWND hWnd, nc_data indata[],int indatanum, nc_data outdata[],int *outdatanum)
{
	int ComputeFlag;
	int i;
	double pi = 3.1415926536;
	double X_Start,Y_Start,B_Start,C_Start,X_End,Y_End,B_End,C_End;
	double Arc_R1,Arc_R2;
	double Arc_a1_temp,Arc_a2_temp;
	double Arc_a1,Arc_a2;
	double Compute_X1,Compute_Y1,Compute_X2,Compute_Y2,Compute_B1,Compute_C1,Compute_B2,Compute_C2;
	int Arc_Num1,Arc_Num2;
	double Deltl1,Deltl2;
	double Deltl_a1,Deltl_a2; //delt对应的角度
	ComputeFlag = 0;
	*outdatanum = indatanum;
	for(i=0;i<indatanum;i++)
	{
		outdata[i] = indata[i];
		if( indata[i].G_tapemode==51 || indata[i].G_tapemode ==52 || indata[i].G_tapemode ==50)
		{
			if( (indata[i].G_pathmode_lower == 2 || indata[i].G_pathmode_lower == 3)&&(indata[i].G_pathmode_upper == 2 || indata[i].G_pathmode_upper == 3)) // circle to circle (includeing point to circle)
			{ 
				ComputeFlag = 1;
			}
		}
		if( indata[i].G_Dcompensate==151 || indata[i].G_Dcompensate ==152 || indata[i].G_Dcompensate ==150)
		{   
			if(indata[i].G_pathmode_lower == 1 && (indata[i].G_pathmode_upper == 2 || indata[i].G_pathmode_upper == 3))//直线对圆
				ComputeFlag = 2;
			else if(indata[i].G_pathmode_upper == 1 && (indata[i].G_pathmode_lower == 2 || indata[i].G_pathmode_lower == 3))//圆对直线
				ComputeFlag = 3;
			else if( (indata[i].G_pathmode_lower == 2 || indata[i].G_pathmode_lower == 3)&&(indata[i].G_pathmode_upper == 2 || indata[i].G_pathmode_upper == 3)) // circle to circle (includeing point to circle)
				ComputeFlag = 1;
		}
///////////////////////////////////////////////////////圆对圆/////////////////////////////////////////////////////////
		if(ComputeFlag == 1) //圆对圆
		{
			X_Start =  - indata[i].I;
			Y_Start =  - indata[i].J;
			B_Start =  - indata[i].I1;    //以圆心为坐标原点的坐标系下，圆弧起点坐标
			C_Start =  - indata[i].J1;
			
			X_End = indata[i].X - indata[i].I;
			Y_End = indata[i].Y - indata[i].J;
			B_End = indata[i].B - indata[i].I1;
			C_End = indata[i].C - indata[i].J1;       //以圆心为坐标原点的坐标系下,圆弧终点坐标
			
			Arc_R1 = sqrt(indata[i].I  * indata[i].I  + indata[i].J  * indata[i].J);
			Arc_R2 = sqrt(indata[i].I1 * indata[i].I1 + indata[i].J1 * indata[i].J1);
			Arc_a1=calculateArc(0.0,0.0,X_Start,Y_Start,X_End,Y_End,indata[i].G_pathmode_lower);//2010-5-18
			Arc_a2=calculateArc(0.0,0.0,B_Start,C_Start,B_End,C_End,indata[i].G_pathmode_upper);//2010-5-18
			Deltl1 = sqrt(8 * Arc_R1 * 0.001);//1微米径向误差规定下的步距;2*sqrt(R*R-(R-0.001)*(R-0.001))以mm为单位
			Deltl2 = sqrt(8 * Arc_R2 * 0.001);//1微米径向误差规定下的步距
			if(Arc_R1 != 0)
			{
				Deltl_a1 = 2*asin(Deltl1/(2*Arc_R1));
				Arc_Num1 = Arc_a1/Deltl_a1;//2010-5-18
			}
			else
			{
				Deltl_a1 = 0;	
				Arc_Num1 = 0;//2010-5-18
			}
			if(Arc_R2 != 0)
			{
				Deltl_a2 = 2*asin(Deltl2/(2*Arc_R2));
				Arc_Num2 = Arc_a2/Deltl_a2;//2010-5-18
			}
			else
			{
				Deltl_a2 = 0;	
				Arc_Num2 =0.0;//2010-5-18
			}
			if(Arc_Num1 > Arc_Num2)
			{
				Deltl1 = sqrt(8 * Arc_R1 * 0.001);//1微米径向误差规定下的步距
				Deltl2 = 2*Arc_R2*sin(Arc_a2/(2*Arc_Num1));
			}
			else
			{
				Deltl2 = sqrt(8 * Arc_R2 * 0.001);//1微米径向误差规定下的步距
				Deltl1 = 2*Arc_R1*sin(Arc_a1/(2*Arc_Num2));
			}
			outdata[i].Deltl1 = Deltl1;
			outdata[i].Deltl2 = Deltl2;
		}

//////////////////////////////////////////////////////直线对圆////////////////////////////////////////////////////////////					
		if(ComputeFlag ==  2) //直线对圆
		{

			B_Start =  - indata[i].I1;    //以圆心为坐标原点的坐标系下，圆弧起点坐标
			C_Start =  - indata[i].J1; 

			B_End = indata[i].B - indata[i].I1;
			C_End = indata[i].C - indata[i].J1;       //以圆心为坐标原点的坐标系下,圆弧终点坐标

			Arc_R2 = sqrt(indata[i].I1 * indata[i].I1 + indata[i].J1 * indata[i].J1);
			Arc_a2=calculateArc(0.0,0.0,B_Start,C_Start,B_End,C_End,indata[i].G_pathmode_upper);//2010-5-18
			Deltl2 = sqrt(8 * Arc_R2 * 0.001);//1微米径向误差规定下的步距

			if(Arc_R2 != 0)
			{
				Deltl_a2 = 2*asin(Deltl2/(2*Arc_R2));
				Arc_Num2 = Arc_a2/Deltl_a2;//2010-5-18
			}
			else
			{
				Deltl_a2 = 0;	
				Arc_Num2 = 0;//2010-5-18
			}
			outdata[i].numbc = Arc_Num2;
		}

///////////////////////////////////////////////////////圆对直线////////////////////////////////////////////////////////////					
		if(ComputeFlag ==  3 ) //圆对直线
		{
			X_Start =  - indata[i].I;
			Y_Start =  - indata[i].J;

			X_End = indata[i].X - indata[i].I;
			Y_End = indata[i].Y - indata[i].J;

			Arc_R1 = sqrt(indata[i].I  * indata[i].I  + indata[i].J  * indata[i].J);
			Arc_a1=calculateArc(0.0,0.0,X_Start,Y_Start,X_End,Y_End,indata[i].G_pathmode_lower);//2010-5-18
			Deltl1 = sqrt(8 * Arc_R1 * 0.001);//1微米径向误差规定下的步距

			if(Arc_R1 != 0)
			{
				Deltl_a1 = 2*asin(Deltl1/(2*Arc_R1));
				Arc_Num1 = Arc_a1/Deltl_a1;//2010-5-18
			}
			else
			{
				Deltl_a1 = 0;	
				Arc_Num1 = 0;//2010-5-18
			}			
			outdata[i].numxy= Arc_Num1;	
		}
	}
	return 0;
}
LRESULT Dcompensate(HWND hWnd, nc_data indata[],int indatanum, nc_data outdata[],int *outdatanumup,int *outdatanumlow,
				   double *Child_Start_point_X,double *Child_Start_point_Y, double *Child_Start_point_B,double *Child_Start_point_C, 
				   int *Child_build_c_up, int *Child_build_c_low, nc_data *Child_cs)//上下异性面11
{
	int compensate152153Flag;
	int i;

	compensate152153Flag = 0;
	*outdatanumup = 0;
	*outdatanumlow = 0;

	for(i=0;i<indatanum;i++)
	{
		if(indata[i].G_Dcompensate ==152 || indata[i].G_Dcompensate ==153||indata[i].G_Dcompensate ==150)			
		{
			compensate152153Flag = 1;
		}		
		if(compensate152153Flag == 1 )
		{		              
				//进行刀补 刀补 

			g151152upFuction(hWnd,&compensate152153Flag,&indata[i],outdata,outdatanumup,Child_Start_point_B,Child_Start_point_C,Child_build_c_up,Child_cs);

			g151152lowFuction(hWnd,&compensate152153Flag,&indata[i],outdata,outdatanumlow,Child_Start_point_X,Child_Start_point_Y,Child_build_c_low,Child_cs);
                                                        
            if(*outdatanumup<*outdatanumlow)
			{
				*outdatanumup = *outdatanumup+1;
				outdata[*outdatanumup].B=0;
				outdata[*outdatanumup].C=0;
				outdata[*outdatanumup].I1=0;
				outdata[*outdatanumup].J1=0;
			}

			else if(*outdatanumup>*outdatanumlow)
			 {
                  *outdatanumlow =*outdatanumlow + 1;
			      outdata[*outdatanumup].X=0;
			      outdata[*outdatanumup].Y=0;
			      outdata[*outdatanumup].I=0;
			      outdata[*outdatanumup].J=0;
                              
			 }
              if(i!=0&&*outdatanumup==*outdatanumlow)
			  {
                 *outdatanumup=*outdatanumup+1;
				 *outdatanumlow=*outdatanumlow+1;
				 }
                        				
				   
		 }
		//不需要刀补功能时;	
	    else if(indata[i].M != 30)   
		{
			outdata[*outdatanumup] = indata[i];			
			*outdatanumup=*outdatanumlow++;	
            compensate152153Flag = 0;	
		}  
	}
    return 0;
}
LRESULT tape(HWND hWnd, nc_data indata[],int indatanum, nc_data outdata[],int *outdatanum,
				   double *Child_Start_point_X,double *Child_Start_point_Y, int *Child_build_c,int *first5152flag, nc_data *Child_cs,double *Child_Start_point_B,double *Child_Start_point_C)
{
	int tape5152Flag;
	int i;
	int RowId;
	tape5152Flag = 0;
	*outdatanum = 0;
	RowId=0;
	for(i=0;i<indatanum;i++)
	{
		if(indata[i].G_tapemode ==51 || indata[i].G_tapemode ==52) tape5152Flag = 1;
		
		if(tape5152Flag == 1 )
		{ //进行锥加工 
			g5152Fuction(hWnd,&tape5152Flag,&indata[i],outdata,outdatanum,Child_Start_point_X,Child_Start_point_Y,Child_build_c,first5152flag,Child_cs,Child_Start_point_B,Child_Start_point_C);
		 }
		//不需要刀补功能时;	
	    else if(indata[i].M != 30)   
		{
			outdata[*outdatanum] = indata[i];							 
			*outdatanum = *outdatanum + 1;	
		}  
	}	
	for(i=0;i<*outdatanum;i++)
	{
		if(outdata[i].G_taperCorner==24)
		{
			RowId=outdata[i].row_id-1;
			g24Fuction(hWnd,&outdata[i+1],outdata,&i,&outdata[i-1],&outdata[i],&indata[RowId]);
		}
	}
			*outdatanum=*outdatanum;
    return 0;
}


LRESULT compensate(HWND hWnd, nc_data indata[],int indatanum, nc_data outdata[],int *outdatanum,
				   double *Child_Start_point_X,double *Child_Start_point_Y, int *Child_build_c, nc_data *Child_cs)
{
	int compens4142Flag;
	int i;

	compens4142Flag = 0;
	*outdatanum = 0;

	for(i=0;i<indatanum;i++)
	{
		if(indata[i].G_compensate ==41 || indata[i].G_compensate ==42) compens4142Flag = 1;
		
		
		if(compens4142Flag == 1 )
		{		              
			DmainFuction(hWnd,&compens4142Flag,&indata[i],outdata,outdatanum,Child_Start_point_X,Child_Start_point_Y,Child_build_c,Child_cs);				   
		 }
		//不需要刀补功能时;	
	    else if(indata[i].M != 30)   
		{
			outdata[*outdatanum] = indata[i];
			*outdatanum = *outdatanum + 1;
		}  
	}
    return 0;
}
/*进行DECODE_NUM_ONCE行译码*/
void File_Convert(HWND hWnd,int fd)
{
	int reallength;
    char ptext[100];
	int i=0;
	int j = 0;
	do
	{
		reallength = _read(fd, ptext+i, 1 );
		i++;

	}while( reallength!=0 );
	for(i=0;i<5;i++)ptext[i]='1';

	j=_lseek(fd,0,SEEK_SET);

	j = _write(fd,&ptext,sizeof(ptext));
	j=j;
}
LRESULT decode(HWND hWnd,LPNCCODE pData,nc_data outdata[],int *outdatanum,int *Child_decode_num,int *file_End_flag ,M_data MChild[] )
{  
    int reallength;
	char pText1[100];
	char pText2[100];
    nc_data *data;
	char *phead;        //程序每一行字符串的头指针
	char *delim =";";      //用回车符分割程序	
	int deviderflag=0;//上下异型面的：
	int devi_position=0;
    
    int i, j;//j是行号，i是每一行里的号
	int k=0;
	int n=0;
    int m=0;
	int g=0;
	i=0;

	M_data *Mdata;	
	Absolute_coordinate.x=data_m.x, Absolute_coordinate.y=data_m.y, Absolute_coordinate.z=data_m.z, Absolute_coordinate.b=data_m.b, Absolute_coordinate.c=data_m.c;

	data = (nc_data *)malloc(sizeof(nc_data));
	Mdata = (M_data *)malloc(sizeof(M_data));
	
	memset(data,0,sizeof(nc_data));
	memset(Mdata,0,sizeof(M_data));
		
    for(j=0;j<DECODE_NUM_ONCE;j++)
	{
		i=0;
		//读一行
		do
		{
			if(((pData+j)->pText[i-1] == 0)&&((pData+j)->pText[i] == 0)&&((pData+j)->pText[i+1] == 0))
			{
				*file_End_flag = 1;
				*outdatanum =j-n;
				return 0;
			}
			i++;
		}while(((pData+j)->pText[i-1] != 'M')&&((pData+j)->pText[i-1] != 'G'));
		if((pData+j)->pText[i-1] == 'M')
		{
			do
			{
				i++;
			}while((pData+j)->pText[i-1]!= ';');
			n++;
		}
		else if((pData+j)->pText[i-1] == 'G')
		{
			do{
				if((pData+j)->pText[i] ==':')
				{
					deviderflag = 1;
					devi_position = i;
				}
				i++;
			}while((pData+j)->pText[i-1] != ';');
		}		
		if(deviderflag==1)
		{
			for(i=devi_position+1,k=0;pText1[k-1]!=';';i++,k++)
			{  
				pText1[k]=(pData+j)->pText[i];
				(pData+j)->pText[i]=0;
			}
		}
		k=1;
	    if(deviderflag==0)
		{
		   if(*file_End_flag != 1)
		   {      
			  phead=(pData+j)->pText;			 		  			 
			  gcodeDecode(hWnd,phead, data, Child_decode_num, j, Mdata);   //译G码到结构体data
			  if((pData+j)->pText[0] == 'M') 
			  {
				  MChild[j] = *Mdata;
			  }
			  else
			  {
				  outdata[j-n] =  *data;
			  }	
		   }
		}
	   else if(deviderflag==1)
	   {
			if(*file_End_flag != 1)
			{
				phead=(pData+j)->pText;
				gcodeDecode(hWnd,phead, data, Child_decode_num, j, Mdata);  //译G码到结构体data
				for(i=devi_position+1,k=0;pText1[k-1]!=';';i++,k++)
				{  
					(pData+j)->pText[i] = pText1[k];					
				}
				phead=pText1;
				gcodeDecodeadd(hWnd,phead, data, Child_decode_num);  //译G码到结构体data

				deviderflag = 0;
				outdata[j-n] =  *data;
			}	
		}
		*outdatanum =j-n;
	}
	free(data);		
	return 0;
}

 //译一行G码到结构体data
void  gcodeDecode(HWND hWnd, char *pheadChild, nc_data *dataChild,int *G_numChild,int All_numChild,M_data *M_Child)
{	
	int i=0;
	//对一行进行译码
	//路径插补模式		
	dataChild->row_id_All = All_numChild;
	if(strstr(pheadChild,"M"))
	{
		M_Child->row_id_All = All_numChild;	
		M_Child->M = Gcode2i(pheadChild,"M");			
		i++;
	}
	else
	{
		dataChild->row_id = 1;

		if(strstr(pheadChild,"G00")) dataChild->G_pathmode=10;
		else if(strstr(pheadChild,"G01")) dataChild->G_pathmode=1;
		else if(strstr(pheadChild,"G02")) dataChild->G_pathmode=2;
		else if(strstr(pheadChild,"G03")) dataChild->G_pathmode=3;

		dataChild->G_pathmode_upper=dataChild->G_pathmode;
		dataChild->G_pathmode_lower=dataChild->G_pathmode;

		//坐标变换及坐标模式
		if(strstr(pheadChild,"G05")) dataChild->G_Coordinate=5;
		if(strstr(pheadChild,"G06")) dataChild->G_Coordinate=6;
		if(strstr(pheadChild,"G07")) dataChild->G_Coordinate=7;
		if(strstr(pheadChild,"G08")) dataChild->G_Coordinate=8;
		if(strstr(pheadChild,"G09")) dataChild->G_Coordinate=9;
		 
		//坐标平面  
		if(strstr(pheadChild,"G17")) 
		{
			dataChild->G_plane=17;
			NowGraphPlane=17;//2010-5-23
		}

		if(strstr(pheadChild,"G18")) 
		{
			dataChild->G_plane=18;
			NowGraphPlane=18;//2010-5-23
		}

		if(strstr(pheadChild,"G19")) 
		{
			dataChild->G_plane=19;
			NowGraphPlane=19;//2010-5-23
		}

		//刀具补偿
		if(strstr(pheadChild,"G40")) dataChild->G_compensate=40;
		if(strstr(pheadChild,"G41")) dataChild->G_compensate=41; 
		if(strstr(pheadChild,"G42")) dataChild->G_compensate=42; 
		
		//锥面处理
		if(strstr(pheadChild,"G50")) 
		{
			dataChild->G_tapemode=50;
			NowGraphPlane=15;//2010-5-51
		}

		if(strstr(pheadChild,"G51")) dataChild->G_tapemode=51;
		if(strstr(pheadChild,"G52")) dataChild->G_tapemode=52; 

		if(strstr(pheadChild,"G150")) 
		{
			dataChild->G_Dcompensate=150;
			NowGraphPlane=15;//2010-5-51
		}
		if(strstr(pheadChild,"G151")) dataChild->G_Dcompensate=151;
		if(strstr(pheadChild,"G152")) dataChild->G_Dcompensate=152; 
		
		//是否是设定坐标原点
		if(strstr(pheadChild,"G90")) dataChild->G_coormode_9x=90;
		if(strstr(pheadChild,"G91")) dataChild->G_coormode_9x=91;
		if(strstr(pheadChild,"G92")) dataChild->G_coormode_92=92;
		else dataChild->G_coormode_92=0;
		if(strstr(pheadChild,"G55")) dataChild->G_coormode_5x=55; 
		else if(strstr(pheadChild,"G56")) dataChild->G_coormode_5x=56; 
		else if(strstr(pheadChild,"G57")) dataChild->G_coormode_5x=57; 
		else if(strstr(pheadChild,"G58")) dataChild->G_coormode_5x=58; 
		else if(strstr(pheadChild,"G59")) dataChild->G_coormode_5x=59;
		//坐标值

		if(strstr(pheadChild,"X")) 
		{
			dataChild->X=Gcode2d(pheadChild,"X");
			if(MILL_ESPARK==0)dataChild->B=dataChild->X;//2010-5-25
		}
		else 
		{
			dataChild->X = 0;
			if(MILL_ESPARK==0)dataChild->B=dataChild->X;//2010-5-25
		}
		if(strstr(pheadChild,"Y")) 
		{
			dataChild->Y=Gcode2d(pheadChild,"Y");
			if(MILL_ESPARK==0)dataChild->C=dataChild->Y;
		}
		else 
		{
			dataChild->Y = 0;
			if(MILL_ESPARK==0)dataChild->C=dataChild->Y;//2010-5-25
		}
		if(strstr(pheadChild,"Z")) 
		{
			dataChild->Z=Gcode2d(pheadChild,"Z");
		}
		else dataChild->Z = 0;
		if(strstr(pheadChild,"I")) 
		{
			dataChild->I=Gcode2d(pheadChild,"I");
			if(MILL_ESPARK==0)dataChild->I1=dataChild->I;//2010-5-25
		}
		else 
		{
			dataChild->I = 0;
			if(MILL_ESPARK==0)dataChild->I1=dataChild->I;//2010-5-25
		}
		if(strstr(pheadChild,"J")) 
		{
			dataChild->J=Gcode2d(pheadChild,"J");
			if(MILL_ESPARK==0)dataChild->J1=dataChild->J;//2010-5-25
		}
		else 
		{
			dataChild->J = 0;
			if(MILL_ESPARK==0)dataChild->J1=dataChild->J;//2010-5-25
		}
		if(strstr(pheadChild,"K")) dataChild->K=Gcode2d(pheadChild,"K");
			else dataChild->K = 0;
		if(strstr(pheadChild,"R")) 
		{
			dataChild->R=Gcode2d(pheadChild,"R");
			if(MILL_ESPARK==0)dataChild->R1=dataChild->R;//2010-5-25
		}
		else 
		{
			dataChild->R = 0;
			if(MILL_ESPARK==0)dataChild->R=dataChild->R;//2010-5-25
		}
		//2010-5-24
		if(MILL_ESPARK==1)
		{
			dataChild->B = 0;
			dataChild->C = 0;
			dataChild->I1 = 0;
			dataChild->J1 = 0;
			dataChild->K1 = 0;
			dataChild->R1 = 0;
		}
		//2010-5-25

		//offset
		if(strstr(pheadChild,"D")) dataChild->D=Gcode2d(pheadChild,"D");
		//锥面度数
		if(strstr(pheadChild,"T")) dataChild->T=Gcode2d(pheadChild,"T");

		//M代码
		if(strstr(pheadChild,"M")) dataChild->M=Gcode2i(pheadChild,"M");
			else dataChild->M=0;
		//S代码
		if(strstr(pheadChild,"S")) dataChild->S=Gcode2i(pheadChild,"S");
		else dataChild->S = 0;

		if(strstr(pheadChild,"L")) dataChild->K=Gcode2d(pheadChild,"L");
			else dataChild->K = 0;
		if(strstr(pheadChild,"T1")) dataChild->R=Gcode2d(pheadChild,"T1");
			else dataChild->R = 0;
	} 
}
void  gcodeDecodeadd(HWND hWnd, char *pheadChild, nc_data *dataChild,int *old_numChild )
{			
    //路径插补模式
    if(strstr(pheadChild,"G00")) dataChild->G_pathmode_upper=0;
    if(strstr(pheadChild,"G01")) dataChild->G_pathmode_upper=1;
    if(strstr(pheadChild,"G02")) dataChild->G_pathmode_upper=2;
    if(strstr(pheadChild,"G03")) dataChild->G_pathmode_upper=3;
	if(strstr(pheadChild,"X")) dataChild->B=Gcode2d(pheadChild,"X");
	else dataChild->B = 0;
	if(strstr(pheadChild,"Y")) dataChild->C=Gcode2d(pheadChild,"Y");
	else dataChild->C = 0;
	if(strstr(pheadChild,"Z")) dataChild->Z=Gcode2d(pheadChild,"Z");
	else dataChild->Z = 0;
	if(strstr(pheadChild,"I")) dataChild->I1=Gcode2d(pheadChild,"I");
	else dataChild->I1 = 0;
	if(strstr(pheadChild,"J")) dataChild->J1=Gcode2d(pheadChild,"J");
	else dataChild->J1 = 0;
	if(strstr(pheadChild,"K")) dataChild->K1=Gcode2d(pheadChild,"K");
	else dataChild->K1 = 0;
	if(strstr(pheadChild,"R")) dataChild->R1=Gcode2d(pheadChild,"R");
	else dataChild->R1 = 0;
}
LRESULT CoordinateSwitch(HWND hWnd, nc_data indata[],int indatanum, nc_data outdata[],int *outdatanum)
{
	int i=0;
	int G54flag,G55flag,G56flag,G57flag,G58flag,G59flag;
	G54flag = 0,G55flag = 0,G56flag = 0,G57flag = 0,G58flag = 0,G59flag = 0;

	int G54G92flag,G55G92flag,G56G92flag,G57G92flag,G58G92flag,G59G92flag;
	G54G92flag = 0,G55G92flag = 0,G56G92flag = 0,G57G92flag = 0,G58G92flag = 0,G59G92flag = 0;

	Coordata coorswitch_to_zero;
	Coordata G92code_temp;
	coorswitch_to_zero.x = 0;coorswitch_to_zero.y = 0;coorswitch_to_zero.z = 0;coorswitch_to_zero.b = 0;coorswitch_to_zero.c = 0;
	G92code_temp.x = 0;G92code_temp.y = 0;G92code_temp.z = 0;G92code_temp.b = 0;G92code_temp.c = 0;


	*outdatanum = indatanum;
	for(i=0;i<indatanum;i++)
	{

		outdata[i] = indata[i];

		if(indata[i].G_coormode_92 == 92 )
		{

			G92code_temp.x = indata[i].X;
			G92code_temp.y = indata[i].Y;
			G92code_temp.z = indata[i].Z;
			G92code_temp.b = indata[i].B;
			G92code_temp.c = indata[i].C;

			Absolute_coordinate.x = Absolute_coordinate.x + G92code_temp.x;
			Absolute_coordinate.y = Absolute_coordinate.y + G92code_temp.y;
			Absolute_coordinate.z = Absolute_coordinate.z + G92code_temp.z;
			Absolute_coordinate.b = Absolute_coordinate.y + G92code_temp.b;
			Absolute_coordinate.c = Absolute_coordinate.z + G92code_temp.c;

			data_w.x = G92code_temp.x;//此处设定为工件坐标系的X原点;
			data_r.x = data_m.x - G92code_temp.x;//机器坐标 - 工件坐标 即为相对坐标;
				
			data_w.y = G92code_temp.y;//此处设定为工件坐标系的X原点;
			data_r.y = data_m.y - G92code_temp.y;//机器坐标 - 工件坐标 即为相对坐标;
				
			data_w.z = G92code_temp.y;//此处设定为工件坐标系的X原点;
			data_r.z = data_m.z - G92code_temp.y;//机器坐标 - 工件坐标 即为相对坐标;
				
			data_w.b = G92code_temp.b;//此处设定为工件坐标系的X原点;
			data_r.b = data_m.b - G92code_temp.b;//机器坐标 - 工件坐标 即为相对坐标;
				
			data_w.c = G92code_temp.c;//此处设定为工件坐标系的X原点;
			data_r.c = data_m.c - G92code_temp.c;//机器坐标 - 工件坐标 即为相对坐标;

			readbuffer_to_fc(hWnd);
			fc_upday(hWndCoor);

		}
////////G55 -59处理/////////////

		if(indata[i].G_coormode_5x == 55 ) 
		{	
			if(G55G92flag == 0)
			{
				CopeWith_G92Code(hWnd,Absolute_coordinate,G55_coordinate,coorswitch_to_zero,G92code_temp);
				G55G92flag = 1;
			}

			if(G55flag == 0)
			{
				Absolute_coordinate.x = Absolute_coordinate.x - G55_coordinate.x + coorswitch_to_zero.x ;
				Absolute_coordinate.y = Absolute_coordinate.y - G55_coordinate.y + coorswitch_to_zero.y;
				Absolute_coordinate.z = Absolute_coordinate.z - G55_coordinate.z + coorswitch_to_zero.z;
				Absolute_coordinate.b = Absolute_coordinate.b - G55_coordinate.b + coorswitch_to_zero.b;
				Absolute_coordinate.c = Absolute_coordinate.c - G55_coordinate.c + coorswitch_to_zero.c;

				coorswitch_to_zero.x = G55_coordinate.x;
				coorswitch_to_zero.y = G55_coordinate.y;
				coorswitch_to_zero.z = G55_coordinate.z;	
				coorswitch_to_zero.b = G55_coordinate.b;
				coorswitch_to_zero.c = G55_coordinate.c;

				G54flag = 0,G55flag = 1,G56flag = 0,G57flag = 0,G58flag = 0,G59flag = 0;

			}

			CopeWith_G5xCode(hWnd,Absolute_coordinate, G55_coordinate,indata, outdata, i);
			G5xFlag = 55;
		}
		else if(indata[i].G_coormode_5x == 56 ) 
		{
			if(G56G92flag == 0)
			{
				CopeWith_G92Code(hWnd,Absolute_coordinate,G56_coordinate,coorswitch_to_zero,G92code_temp);
				G56G92flag = 1;
			}
			if(G56flag == 0)
			{
				Absolute_coordinate.x = Absolute_coordinate.x - G56_coordinate.x + coorswitch_to_zero.x ;
				Absolute_coordinate.y = Absolute_coordinate.y - G56_coordinate.y + coorswitch_to_zero.y;
				Absolute_coordinate.z = Absolute_coordinate.z - G56_coordinate.z + coorswitch_to_zero.z;
				Absolute_coordinate.b = Absolute_coordinate.b - G56_coordinate.b + coorswitch_to_zero.b;
				Absolute_coordinate.c = Absolute_coordinate.c - G56_coordinate.c + coorswitch_to_zero.c;

				coorswitch_to_zero.x = G56_coordinate.x;
				coorswitch_to_zero.y = G56_coordinate.y;
				coorswitch_to_zero.z = G56_coordinate.z;	
				coorswitch_to_zero.b = G56_coordinate.b;
				coorswitch_to_zero.c = G56_coordinate.c;

				G54flag = 0,G55flag = 0,G56flag = 1,G57flag = 0,G58flag = 0,G59flag = 0;

			}
			CopeWith_G5xCode(hWnd,Absolute_coordinate, G56_coordinate,indata, outdata, i);
			G5xFlag = 56;
		
		}
		else if(indata[i].G_coormode_5x == 57 ) 
		{
			if(G57G92flag == 0)
			{
				CopeWith_G92Code(hWnd,Absolute_coordinate,G57_coordinate,coorswitch_to_zero,G92code_temp);
				G57G92flag = 1;
			}
			if(G57flag == 0)
			{
				Absolute_coordinate.x = Absolute_coordinate.x - G57_coordinate.x + coorswitch_to_zero.x ;
				Absolute_coordinate.y = Absolute_coordinate.y - G57_coordinate.y + coorswitch_to_zero.y;
				Absolute_coordinate.z = Absolute_coordinate.z - G57_coordinate.z + coorswitch_to_zero.z;
				Absolute_coordinate.b = Absolute_coordinate.b - G57_coordinate.b + coorswitch_to_zero.b;
				Absolute_coordinate.c = Absolute_coordinate.c - G57_coordinate.c + coorswitch_to_zero.c;

				coorswitch_to_zero.x = G57_coordinate.x;
				coorswitch_to_zero.y = G57_coordinate.y;
				coorswitch_to_zero.z = G57_coordinate.z;	
				coorswitch_to_zero.b = G57_coordinate.b;
				coorswitch_to_zero.c = G57_coordinate.c;

				G54flag = 0,G55flag = 0,G56flag = 0,G57flag = 1,G58flag = 0,G59flag = 0;

			}
			CopeWith_G5xCode(hWnd,Absolute_coordinate, G57_coordinate,indata, outdata, i);
			G5xFlag = 57;
	
		}
		else if(indata[i].G_coormode_5x == 58 ) 
		{
			if(G58G92flag == 0)
			{
				CopeWith_G92Code(hWnd,Absolute_coordinate,G58_coordinate,coorswitch_to_zero,G92code_temp);
				G58G92flag = 1;
			}
			if(G58flag == 0)
			{
				Absolute_coordinate.x = Absolute_coordinate.x - G58_coordinate.x + coorswitch_to_zero.x ;
				Absolute_coordinate.y = Absolute_coordinate.y - G58_coordinate.y + coorswitch_to_zero.y;
				Absolute_coordinate.z = Absolute_coordinate.z - G58_coordinate.z + coorswitch_to_zero.z;
				Absolute_coordinate.b = Absolute_coordinate.b - G58_coordinate.b + coorswitch_to_zero.b;
				Absolute_coordinate.c = Absolute_coordinate.c - G58_coordinate.c + coorswitch_to_zero.c;

				coorswitch_to_zero.x = G58_coordinate.x;
				coorswitch_to_zero.y = G58_coordinate.y;
				coorswitch_to_zero.z = G58_coordinate.z;	
				coorswitch_to_zero.b = G58_coordinate.b;
				coorswitch_to_zero.c = G58_coordinate.c;

				G54flag = 0,G55flag = 0,G56flag = 0,G57flag = 0,G58flag = 1,G59flag = 0;

			}

			CopeWith_G5xCode(hWnd,Absolute_coordinate, G58_coordinate,indata, outdata, i);
			G5xFlag = 58;
	
		}
		else if(indata[i].G_coormode_5x == 59 ) 
		{
			if(G59G92flag == 0)
			{
				CopeWith_G92Code(hWnd,Absolute_coordinate,G59_coordinate,coorswitch_to_zero,G92code_temp);
				G59G92flag = 1;
			}
			if(G59flag == 0)
			{
				Absolute_coordinate.x = Absolute_coordinate.x - G59_coordinate.x + coorswitch_to_zero.x ;
				Absolute_coordinate.y = Absolute_coordinate.y - G59_coordinate.y + coorswitch_to_zero.y;
				Absolute_coordinate.z = Absolute_coordinate.z - G59_coordinate.z + coorswitch_to_zero.z;
				Absolute_coordinate.b = Absolute_coordinate.b - G59_coordinate.b + coorswitch_to_zero.b;
				Absolute_coordinate.c = Absolute_coordinate.c - G59_coordinate.c + coorswitch_to_zero.c;

				coorswitch_to_zero.x = G59_coordinate.x;
				coorswitch_to_zero.y = G59_coordinate.y;
				coorswitch_to_zero.z = G59_coordinate.z;	
				coorswitch_to_zero.b = G59_coordinate.b;
				coorswitch_to_zero.c = G59_coordinate.c;

				G54flag = 0,G55flag = 0,G56flag = 0,G57flag = 0,G58flag = 0,G59flag = 1;

			}
			CopeWith_G5xCode(hWnd,Absolute_coordinate, G59_coordinate,indata, outdata, i);
			G5xFlag = 59;
				
		}
		else////////G54处理/////////////
		{
			if(G54G92flag == 0)
			{
				CopeWith_G92Code(hWnd,Absolute_coordinate,G54_coordinate,coorswitch_to_zero,G92code_temp);
				G54G92flag = 1;
			}
			if(G54flag == 0)
			{

				Absolute_coordinate.x = Absolute_coordinate.x - G54_coordinate.x + coorswitch_to_zero.x ;
				Absolute_coordinate.y = Absolute_coordinate.y - G54_coordinate.y + coorswitch_to_zero.y;
				Absolute_coordinate.z = Absolute_coordinate.z - G54_coordinate.z + coorswitch_to_zero.z;
				Absolute_coordinate.b = Absolute_coordinate.b - G54_coordinate.b + coorswitch_to_zero.b;
				Absolute_coordinate.c = Absolute_coordinate.c - G54_coordinate.c + coorswitch_to_zero.c;

				coorswitch_to_zero.x = G54_coordinate.x;
				coorswitch_to_zero.y = G54_coordinate.y;
				coorswitch_to_zero.z = G54_coordinate.z;
				coorswitch_to_zero.b = G54_coordinate.b;
				coorswitch_to_zero.c = G54_coordinate.c;
				
				G54flag = 1,G55flag = 0,G56flag = 0,G57flag = 0,G58flag = 0,G59flag = 0;

			}
			CopeWith_G5xCode(hWnd,Absolute_coordinate, G54_coordinate,indata, outdata, i);
			G5xFlag = 54;
		}


		if(indata[i].G_Coordinate != 9 )
		{

			if(indata[i].G_Coordinate == 5 ) 
			{
				outdata[i].X = -1 * indata[i].X;
				outdata[i].I = -1 * indata[i].I;
				if(indata[i].G_pathmode ==2) outdata[i].G_pathmode = 3;
				if(indata[i].G_pathmode ==3) outdata[i].G_pathmode = 2;
				if(indata[i].G_compensate==41) outdata[i].G_compensate = 42;
				if(indata[i].G_compensate==42) outdata[i].G_compensate = 41;
				if(indata[i].G_tapemode ==51) outdata[i].G_tapemode = 52;
				if(indata[i].G_tapemode ==52) outdata[i].G_tapemode = 51;

			}

			if(indata[i].G_Coordinate == 6 ) 
			{
				outdata[i].Y = -1 * indata[i].Y;
				outdata[i].J = -1 * indata[i].J;
				if(indata[i].G_pathmode ==2) outdata[i].G_pathmode = 3;
				if(indata[i].G_pathmode ==3) outdata[i].G_pathmode = 2;
				if(indata[i].G_compensate==41) outdata[i].G_compensate = 42;
				if(indata[i].G_compensate==42) outdata[i].G_compensate = 41;
				if(indata[i].G_tapemode ==51) outdata[i].G_tapemode = 52;
				if(indata[i].G_tapemode ==52) outdata[i].G_tapemode = 51;
			}
			
			if(indata[i].G_Coordinate == 7 ) 
			{
				outdata[i].Z = -1 * indata[i].Z;
				outdata[i].K = -1 * indata[i].K;
				if(indata[i].G_pathmode ==2) outdata[i].G_pathmode = 3;
				if(indata[i].G_pathmode ==3) outdata[i].G_pathmode = 2;
				if(indata[i].G_compensate==41) outdata[i].G_compensate = 42;
				if(indata[i].G_compensate==42) outdata[i].G_compensate = 41;
				if(indata[i].G_tapemode ==51) outdata[i].G_tapemode = 52;
				if(indata[i].G_tapemode ==52) outdata[i].G_tapemode = 51;
			}

			if(indata[i].G_Coordinate == 8 ) 
			{
				outdata[i].X = -1 * indata[i].X;
				outdata[i].Y = -1 * indata[i].Y;
				outdata[i].I = -1 * indata[i].I;
				outdata[i].I = -1 * indata[i].I;
				if(indata[i].G_pathmode ==2) outdata[i].G_pathmode = 3;
				if(indata[i].G_pathmode ==3) outdata[i].G_pathmode = 2;
				if(indata[i].G_compensate==41) outdata[i].G_compensate = 42;
				if(indata[i].G_compensate==42) outdata[i].G_compensate = 41;
				if(indata[i].G_tapemode ==51) outdata[i].G_tapemode = 52;
				if(indata[i].G_tapemode ==52) outdata[i].G_tapemode = 51;
			}

		}
		
		if(indata[i].G_Coordinate != 27 ) 
		{
			if(indata[i].G_Coordinate == 26 ) 
			{
				outdata[i].X = cos(indata[i].A) * indata[i].X + sin(indata[i].A) * indata[i].Y;
				outdata[i].Y = sin(indata[i].A) * indata[i].X - cos(indata[i].A) * indata[i].Y;
			}
		}
	}
	return 0;
}
void CopeWith_G92Code(HWND hWnd,Coordata Absolute_coor,Coordata G5x_coordinate,Coordata coorswitch_to_zero, Coordata G9xcode_temp)
{
////////G92处理/////////////
	G5x_coordinate.x = G5x_coordinate.x - G9xcode_temp.x;
	G5x_coordinate.y = G5x_coordinate.y - G9xcode_temp.y;
	G5x_coordinate.z = G5x_coordinate.z - G9xcode_temp.z;
	G5x_coordinate.b = G5x_coordinate.b - G9xcode_temp.b;
	G5x_coordinate.c = G5x_coordinate.c - G9xcode_temp.c;

	coorswitch_to_zero.x = coorswitch_to_zero.x - G9xcode_temp.x;
	coorswitch_to_zero.y = coorswitch_to_zero.y - G9xcode_temp.y;
	coorswitch_to_zero.z = coorswitch_to_zero.z - G9xcode_temp.z;
	coorswitch_to_zero.b = coorswitch_to_zero.b - G9xcode_temp.b;
	coorswitch_to_zero.c = coorswitch_to_zero.c - G9xcode_temp.c;


	readbuffer_to_fc(hWnd);
	fc_upday(hWndCoor);

}
void CopeWith_G5xCode(HWND hWnd,Coordata Absolute_coor,Coordata G5x_coordinate,nc_data Indata[],nc_data Outdata[],int num)
{
	int i = num;
	if(Indata[i].G_coormode_9x == 91 ) 
	{
		if(Indata[i].G_coormode_92 == 92 ) 
		{
			Outdata[i].X = 0;
			Outdata[i].Y = 0;
			Outdata[i].Z = 0;
			Outdata[i].B = 0;
			Outdata[i].C = 0;
		}
		else
		{
			Outdata[i].X = Indata[i].X;
			Outdata[i].Y = Indata[i].Y;
			Outdata[i].Z = Indata[i].Z;
			Outdata[i].B = Indata[i].B;
			Outdata[i].C = Indata[i].C;

			Absolute_coor.x = Absolute_coor.x + Outdata[i].X;
			Absolute_coor.y = Absolute_coor.y + Outdata[i].Y;
			Absolute_coor.z = Absolute_coor.z + Outdata[i].Z;
			Absolute_coor.b = Absolute_coor.b + Outdata[i].B;
			Absolute_coor.c = Absolute_coor.c + Outdata[i].C;
		}
	}
	////////G90处理/////////////
	if(Indata[i].G_coormode_9x == 90 ) 
	{
		if(Indata[i].G_coormode_92 == 92 ) 
		{
			Outdata[i].X = 0;
			Outdata[i].Y = 0;
			Outdata[i].Z = 0;
			Outdata[i].B = 0;
			Outdata[i].C = 0;
		}
		else
		{
			Outdata[i].X = Indata[i].X - Absolute_coor.x;
			Outdata[i].Y = Indata[i].Y - Absolute_coor.y;
			Outdata[i].Z = Indata[i].Z - Absolute_coor.z;
			Outdata[i].B = Indata[i].B - Absolute_coor.b;
			Outdata[i].C = Indata[i].C - Absolute_coor.c;

			Absolute_coor.x = Absolute_coor.x + Outdata[i].X;
			Absolute_coor.y = Absolute_coor.y + Outdata[i].Y;
			Absolute_coor.z = Absolute_coor.z + Outdata[i].Z;
			Absolute_coor.b = Absolute_coor.b + Outdata[i].B;
			Absolute_coor.c = Absolute_coor.c + Outdata[i].C;
		}
	}
	Absolute_coordinate = Absolute_coor;
}
LRESULT  find_draw_param (HWND hWnd,LPNCDATA pData,double *draw_width,double *draw_length,double  *draw_mw, double *draw_ml,int new_num)
{	
	 double width,length,mw,ml;
	 double width1,length1,mw1,ml1;
	 int i;
     double m , n, a, b;
     double m1 , n1, a1, b1;
  
     float WC ,LC;
     float WC1 ,LC1;
	 RECT r1,r2;
	 m_point point,pointu;

     m=n=0;
     a=b=0;

	 m1=n1=0;
     a1=b1=0;    
      
	 GetClientRect(hWnd,&r1);
	 GetClientRect(hWnd,&r2);
    /*坐标处理-"绝对坐标"*/
    point.X=0;
    point.Y=0;	
    pointu.X=0;
    pointu.Y=0;
    for(i=0;i<new_num;i++)
    {
		if((pData+i)->G_plane == 19)
		{
			point.X = point.X + (pData+i)->Y*1000;
			point.Y = point.Y + (pData+i)->Z*1000;
		}		
		if((pData+i)->G_plane == 18)
		{
			point.X = point.X + (pData+i)->X*1000;
			point.Y = point.Y + (pData+i)->Z*1000;
		}
		if((pData+i)->G_plane == 17)
		{
			point.X = point.X + (pData+i)->X*1000;
			point.Y = point.Y + (pData+i)->Y*1000;
			pointu.X = pointu.X + (pData+i)->B*1000;
			pointu.Y = pointu.Y + (pData+i)->C*1000;
		}        
		 //Y轴
        if(point.X >= 0 )
        {
			if(m < point.X)
				m = point.X;
            if(m1 < point.X)
				m1 = pointu.X;
        }
        else
        {
            if(n>point.X)
				n=point.X;  
            if(n1>point.X)
				n1=pointu.X;
        }
        //Z轴
        if(point.Y >=0)
        {
            if(a<point.Y)
				a = point.Y;
            if(a1<point.Y)
				a1 = pointu.Y;
        }
        else
        {
            if(b>point.Y)
				b = point.Y;
            if(b1>point.Y)
				b1 = pointu.Y;
        }
    }	
    /*求显示图形的长宽*/   
    width=fabs(m - n);
    length=fabs(a - b);
	 
    width1=fabs(m1 - n1);
    length1=fabs(a1 - b1);	
	/*画图准备*/      
    WC=(float)((r1.right-r1.left)/(width*1.2));
    LC=(float)((r1.bottom-r1.top)/(length*1.2));

	WC1=(float)((r2.right-r2.left)/(width1*1.2));
    LC1=(float)((r2.bottom-r2.top)/(length1*1.2));
	if(width>=length)
    {
        mw=((m+n)/2)*WC;
        ml=((a+b)/2)*WC;
	}
	else
	{
		mw=((m+n)/2)*LC;
        ml=((a+b)/2)*LC;
	}	
	if(width1>=length1)
    {
        mw1=((m1+n1)/2)*WC;
        ml1=((a1+b1)/2)*WC;
	}
	else
	{
		mw1=((m1+n1)/2)*LC;
        ml1=((a1+b1)/2)*LC;
	}	
	*draw_width =width;
	*draw_length = length;
	*draw_mw= mw;
	*draw_ml = ml;	
	return 0;
}
LRESULT draw_all(HWND hWnd,LPNCDATA pData,double width,double length,double mw, double ml,int new_num)
{ 
	int i; 
	int  sx,sy,sr;
	double angle1, angle2;
	double sm, R1;

	float WC ,LC;
	RECT r1;
	HDC PCD1;
	HPEN hPen;

	m_point point,point_s;     //每行产生一个"绝对"坐标点
	m_point point1,point1_s;     //每行产生一个"绝对"坐标点
	double pointX, pointY,pointI, pointJ;
	double temp;

	PCD1= GetDC(hWnd);

	hPen = CreatePen(PS_SOLID,1,RGB(255,255,255));
	SelectObject(PCD1,hPen);

	GetClientRect(hWnd,&r1);

	SetMapMode(PCD1,MM_LOENGLISH);
	SetViewportOrgEx(PCD1,0,r1.bottom-r1.top,NULL);           //改变逻辑坐标原点到左下角y轴向上，x轴向右
	//坐标处理-"绝对坐标"
	SetArcDirection(PCD1,AD_COUNTERCLOCKWISE);

	point.X = 0;
	point.Y = 0;
	/*画图准备*/
	WC = (float)((r1.right-r1.left)/(width*1.2));
	LC = (float)((r1.bottom-r1.top)/(length*1.2));	
    for(i=0;i<new_num+1;i++)
    {
		point_s.X = point.X;
		point_s.Y = point.Y;
		if((pData+i)->G_plane == 19)
		{
			point.X = point_s.X + (pData+i)->Y*1000;
			point.Y = point_s.Y + (pData+i)->Z*1000;
		}
		if((pData+i)->G_plane == 18)
		{
			point.X = point_s.X + (pData+i)->X*1000;
			point.Y = point_s.Y + (pData+i)->Z*1000;
		}
		if((pData+i)->G_plane == 17)
		{
			point.X = point_s.X + (pData+i)->X*1000;
			point.Y = point_s.Y + (pData+i)->Y*1000;
		}//将相对坐标pData转换为绝对坐标point_s中
			
		if(width>=length)
		{
			point1.X = point.X*WC + (r1.right*0.5-mw);
			point1.Y = point.Y*WC + (r1.bottom*0.5-ml);        
			point1_s.X = point_s.X*WC + (r1.right*0.5-mw);
			point1_s.Y = point_s.Y*WC + (r1.bottom*0.5-ml); 
		}
		else
		{
			point1.X = point.X*LC + (r1.right*0.5-mw);
			point1.Y = point.Y*LC + (r1.bottom*0.5-ml);        
			point1_s.X = point_s.X*LC + (r1.right*0.5-mw);
			point1_s.Y = point_s.Y*LC + (r1.bottom*0.5-ml); 
		}	
		if ((pData+i)->G_pathmode==1)
        {
            MoveToEx(PCD1,(int) point1_s.X, (int) point1_s.Y,NULL);
			LineTo(PCD1,(int) point1.X,(int)point1.Y);
		}
        else if((pData+i)->G_pathmode==2 ||(pData+i)->G_pathmode==3)
        {
			if((pData+i)->G_plane == 19)
			{
				pointI = (pData+i)->J;
				pointJ = (pData+i)->K;
				pointX = (pData+i)->Y;
				pointY = (pData+i)->Z;
			}
			if((pData+i)->G_plane == 18)
			{
				pointI = (pData+i)->I;
				pointJ = (pData+i)->K;
				pointX = (pData+i)->X;
				pointY = (pData+i)->Z;
			}
			if((pData+i)->G_plane == 17)
			{
				pointI = (pData+i)->I;
				pointJ = (pData+i)->J;
				pointX = (pData+i)->X;
				pointY = (pData+i)->Y;
			}
            R1 = sqrt(pointI*pointI + pointJ*pointJ);
            sm = pointX*pointX + pointY*pointY;
			if(R1>ZERO)
			{
				temp = -(pointI/R1);
				if(temp >= 1) temp = 1;
				if(temp <= -1) temp = -1;
			
				if(pointJ<=0)
					angle1 = 180*acos(temp)/3.14;
				else 
					angle1 = 360-180*acos(temp)/3.14;

				temp = (pointX-pointI)/R1;
				if(temp >= 1) temp = 1;
				if(temp <= -1) temp =-1;
				if(pointY>=pointJ)					
					angle2 = 180*acos(temp)/3.14;
				else
					angle2 = 360- 180*acos(temp)/3.14;			
			}
			else if(R1<ZERO)
			{
				angle1=0;
				angle2=0;
			}     
            if(width>=length)
            {
				sx = (int)(point1_s.X+pointI*1000*WC);
				sy = (int)(point1_s.Y+pointJ*1000*WC);
				sr = (int)(R1*1000*WC);
            }
			else
            {
				sx = (int)(point1_s.X+pointI*1000*LC);
                sy = (int)(point1_s.Y+pointJ*1000*LC);
                sr = (int)(R1*1000*LC);
            }

			MoveToEx(PCD1,(int)point1_s.X, (int)point1_s.Y,NULL);
			if(angle1-angle2>=0)
			{
				if((pData+i)->G_pathmode==2)
				{
					AngleArc(PCD1,sx,sy,sr,360-angle1,(angle1-angle2) );
				}
				if((pData+i)->G_pathmode==3)
				{
					AngleArc(PCD1,sx,sy,sr,360-angle1,-(360-(angle1-angle2)));
				}
			}
			else
			{
				if((pData+i)->G_pathmode==2) 
				{
					AngleArc(PCD1,sx,sy,sr,360-angle1,(360-(angle2-angle1)));
				}
				if((pData+i)->G_pathmode==3)
				{
					AngleArc(PCD1,sx,sy,sr,360-angle1,(angle1-angle2));
				}
			}
		}
	}
	ReleaseDC(hWnd,PCD1);
	DeleteObject(hPen);
    return 0; 
}
LRESULT draw_all_tape(HWND hWnd,LPNCDATA pData,double width,double length,double mw, double ml,int new_num)
{
	int i; 
	int  sx, sy,sr;
	double angle1, angle2;
	double smu, Ru1; 
	double WC ,LC;
	RECT r1;
	HDC PCD1;
	HPEN hPen;	 

	m_point pointu,pointu_s;     //每行产生一个"绝对"坐标点
	m_point pointu1,pointu1_s;

	double pointX1, pointY1,pointI1, pointJ1;
	double temp;

	PCD1= GetDC(hWnd);

	hPen = CreatePen(PS_SOLID,1,RGB(0,255,255));
	SelectObject(PCD1,hPen);

	GetClientRect(hWnd,&r1);

	SetMapMode(PCD1,MM_LOENGLISH);
	SetViewportOrgEx(PCD1,0,r1.bottom-r1.top,NULL);           //改变逻辑坐标原点到左下角y轴向上，x轴向右
	/*坐标处理-"绝对坐标"*/
	/*画图准备*/
	WC=(double)((r1.right-r1.left)/(width*1.2));
	LC=(double)((r1.bottom-r1.top)/(length*1.2));

	pointu.X=0;
	pointu.Y=0;

	for(i=0;i<new_num+1;i++)
	{
		pointu_s.X=pointu.X;
		pointu_s.Y=pointu.Y;

		pointu.X = pointu_s.X + (pData+i)->B*1000;
		pointu.Y = pointu_s.Y + (pData+i)->C*1000;

		if(width>=length)
		{
			pointu1.X=pointu.X*WC+(r1.right*0.5-mw);
			pointu1.Y=pointu.Y*WC+(r1.bottom*0.5-ml);        
			pointu1_s.X=pointu_s.X*WC+(r1.right*0.5-mw);
			pointu1_s.Y=pointu_s.Y*WC+(r1.bottom*0.5-ml); 
		}
		else
		{
			pointu1.X=pointu.X*LC+(r1.right*0.5-mw);
			pointu1.Y=pointu.Y*LC+(r1.bottom*0.5-ml);        
			pointu1_s.X=pointu_s.X*LC+(r1.right*0.5-mw);
			pointu1_s.Y=pointu_s.Y*LC+(r1.bottom*0.5-ml); 
		}				
		if ((pData+i)->G_pathmode_upper==1)
		{
			MoveToEx(PCD1,(int) pointu1_s.X, (int) pointu1_s.Y,NULL);
			LineTo(PCD1,(int) pointu1.X,(int)pointu1.Y);			
		 }
		else if((pData+i)->G_pathmode_upper==2 ||(pData+i)->G_pathmode_upper==3)
		{
			pointI1 = (pData+i)->I1;
			pointJ1 = (pData+i)->J1;
			pointX1 = (pData+i)->B;
			pointY1 = (pData+i)->C;

			Ru1=sqrt(pointI1*pointI1 + pointJ1*pointJ1);
			smu=pointX1*pointX1 + pointY1*pointY1;

			if(Ru1!=0)
			{    
				temp = -(pointI1/Ru1);
				if(temp >= 1) temp = 1;
				if(temp <= -1) temp =-1;
	          
				if(pointJ1<=0)angle1 = 180*acos(temp)/3.14;
				else angle1 = 360-180*acos(temp)/3.14;

				temp = (pointX1-pointI1)/Ru1;
				if(temp >= 1) temp = 1;
				if(temp <= -1) temp =-1;

				if(pointY1>pointJ1)angle2 = 180*acos(temp)/3.14;
				else angle2 = 360- 180*acos(temp)/3.14;
	        
				if(width>=length)
				{
					sx=(int)(pointu1_s.X+pointI1*1000*WC);
					sy=(int)(pointu1_s.Y+pointJ1*1000*WC);
					sr=(int)(Ru1*1000*WC);
				}
				else
				{
					sr=(int)(Ru1*1000*LC);
					sx=(int)(pointu1_s.X+pointI1*1000*LC);
					sy=(int)(pointu1_s.Y+pointJ1*1000*LC);
				}

				MoveToEx(PCD1,(int)pointu1_s.X, (int)pointu1_s.Y,NULL);

				if(angle1-angle2>=0)
				{
					if((pData+i)->G_pathmode_upper==2)
					{
						AngleArc(PCD1,sx,sy,sr,360-angle1,(angle1-angle2) );
					}
					if((pData+i)->G_pathmode_upper==3)
					{
						AngleArc(PCD1,sx,sy,sr,360-angle1,-(360-(angle1-angle2)));						
					}
				}
				else
				{
					if((pData+i)->G_pathmode_upper==2) 
					{
						AngleArc(PCD1,sx,sy,sr,360-angle1,(360-(angle2-angle1)));
					}
					if((pData+i)->G_pathmode_upper==3)
					{
						AngleArc(PCD1,sx,sy,sr,360-angle1,(angle1-angle2) );
					}
				}
			}
											
			if(Ru1<ZERO) 
			{      
				angle1 = 0;
				angle2 = 0;
				if(width>=length)
				{
					sx=(int)(pointu1_s.X+pointI1*1000*WC);
					sy=(int)(pointu1_s.Y+pointJ1*1000*WC);
					sr=(int)(Ru1*1000*WC);
				}
				else
				{
					sx=(int)(pointu1_s.X+pointI1*1000*LC);
					sy=(int)(pointu1_s.Y+pointJ1*1000*LC);
					sr=(int)(Ru1*1000*LC);
				}
				MoveToEx(PCD1,(int)pointu1_s.X, (int)pointu1_s.Y,NULL);

				if((pData+i)->G_pathmode_upper==2)
				{
					AngleArc(PCD1,sx,sy,sr,360-angle1,(angle1-angle2) );				
				}
				if((pData+i)->G_pathmode_upper==3)
				{
					AngleArc(PCD1,sx,sy,sr,360-angle1,-(360-(angle1-angle2)));				
				}
			}
		}
	}     
    ReleaseDC(hWnd,PCD1);
	DeleteObject(hPen);
    return 0;
}
//******************************************************************************************************************
/*将字符串s中的字符gcode后面的实型数返回, gcode必须是一个字符!*/
double Gcode2d(char* s, const char* gcode)
{
    double dvalue;
    int i;
    char *pos=strstr(s, gcode);
    char temp[25]="";
    for(i=1; isdigit(*(pos+i)) || (*(pos+i))==46 || (*(pos+i))==45; i++) //数字或小数点或负号
      temp[i-1]=*(pos+i);
    dvalue=atof(temp);
    return dvalue;
}
/*将字符串s中的字符gcode后面的整型数返回, gcode必须是一个字符!*/
int Gcode2i(char* s, const char* gcode)
{
    int ivalue;
    int i;
    char *pos=strstr(s, gcode);
    char temp[10]="";
    for(i=1; isdigit(*(pos+i)); i++ ) //只有正整数
      temp[i-1]=*(pos+i);
    ivalue=atoi(temp);
    return ivalue;
}
/*返回字符串p中含有字符串s的个数, s必须只是一个字符*/
int stringnum(char *p, const char* s)
{
    int num=0;
    char* pos;
    if(pos=strstr(p, s)) num++;
    else return 0;
    while (pos=strstr(pos+1, s)) {
        num++;
    }
    return num;
}
//*****************************************************************************************************************