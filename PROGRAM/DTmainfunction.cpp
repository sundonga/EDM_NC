#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"
extern HINSTANCE hInst;	

extern int Arcdirection(double Arc1,double Arc2,double Arc3);
extern double calculateLineArc(double PointX,double PointY);//����ֱ����X����������ʱ��н�
extern TapeParam tapepm;  //׶�ȼӹ�����
extern double ZERO;
extern double ZERO_ERROR;
extern double ZERO_NEG;

/******************************************************************************************/
extern void LineshortenFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double X1,
						double Y1,double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y);

extern void LineextendFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double X1,
					   double Y1,double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y);

extern int calculateLineinsertdirection(double Sx,double Sy,double S1x,double S1y,double S2x,double S2y,double X0,double Y0);

extern void CircleToLineshortenFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_one,
								double J_one,double X1,double Y1,double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y);

extern void CircleToLinetangentialFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_one,double J_one,
								   double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y);

extern void CircleToLineextendFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_one,double J_one,
							   double X1,double Y1,double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y);

extern int calculateCircleToLineinsertdirection(double Sx,double Sy,double S1x,double S1y,double S2x,double S2y,double X0,double Y0);

extern void LineToCircleshortenFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_two,double J_two,
								double X1,double Y1,double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y);

extern void LineToCircletangentialFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_two,double J_two,
								   double X1,double Y1,double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y);

extern void LineToCircleextendFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_two,double J_two,
							   double X1,double Y1,double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y);

extern int calculateLineToCircleinsertdirection(double S1x,double S1y,double S2x,double S2y,double X0,double Y0);

extern void CircleToCircleshortenFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_one,double J_one,
								  double X1,double Y1,double I_two,double J_two,double r0x,double r0y,double r1x,double r1y,double KBx0,double KBy0,double *Start_X, double *Start_Y);

extern void CircleToCircletangentialFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_one,double J_one,
									 double X1,double Y1,double I_two,double J_two,double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y);

extern void CircleToCircleextendFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_one,double J_one,
								 double X1,double Y1,double I_two,double J_two,double r0x,double r0y,double r1x,double r1y,double KBx0,double KBy0,double *Start_X, double *Start_Y);

extern int calculateCircleToCircleinsertdirection(double S1x,double S1y,double S2x,double S2y,double KBx0,
										   double KBy0,double X0,double Y0,double I_two,double J_two,double Ox,double Oy);
/******************************************************************************************/

void DmainFuction(HWND hWnd,int *sign, nc_data *dataChild,nc_data asChild[],int *num,double *Start_X, double *Start_Y,int *c, nc_data *csChild)
{                         //���߲���״̬��־
    double Y0,X0,Y1,X1,d0,d1,r0y,r0x,r1y,r1x;
    double J_one,I_one,J_two,I_two,KBy0,KBx0,KBy1,KBx1;
    double typeJudge;
	double zeroError;
	double pi = 3.14159;
    double R0,R1;
	if(dataChild->G_taperCorner==24)
	{
       dataChild->T=csChild->T;
	}
	R0=csChild->D/cos(((csChild->T/1000))*pi/180.0);
	R1=dataChild->D/cos(((dataChild->T/1000))*pi/180.0);
	if(*c != 0)//����������      
	{				   
        if(dataChild->G_pathmode == 1 && csChild->G_pathmode ==1)//ֱ�߽�ֱ��
		{
			if(csChild->G_plane == 18)
            {
                X0 = csChild->Z;
                Y0 = csChild->X;
                X1 = dataChild->Z;
                Y1 = dataChild->X;
            }
            
            else if(csChild->G_plane == 19)
            {
                X0 = csChild->Y;
                Y0 = csChild->Z;
                X1 = dataChild->Y;
                Y1 = dataChild->Z;											
            }
			else
            {
                X0 = csChild->X;
                Y0 = csChild->Y;
                X1 = dataChild->X;
                Y1 = dataChild->Y;
            }
			d0 = sqrt(X0*X0 +Y0*Y0);
			d1 = sqrt(X1*X1 +Y1*Y1);
			if(csChild->G_compensate == 41)
			{
				if(d0>ZERO)
				{
					r0y = R0*X0/d0;
					r0x = (-R0)*Y0/d0;
				}
				else	r0y=r0x=0.0;
				if(d1>ZERO)
				{
					r1y = R1*X1/d1;
					r1x = (-R1)*Y1/d1;
				}
				else	r1y=r1x=0.0;
			}
			else if(csChild->G_compensate == 42)
			{
				if(d0>ZERO)
				{
					r0y = (-R0)*X0/d0;
					r0x = R0*Y0/d0;
				}
				else	r0y=r0x=0.0;
				if(d1>ZERO)
				{
					r1y = (-R1)*X1/d1;
					r1x = R1*Y1/d1;
				}
				else	r1y=r1x=0.0;
			}						
			if((sqrt(r0y*r0y + r0x*r0x)*sqrt(r1x*r1x + r1y*r1y))<ZERO) 
			{ 
				typeJudge=0;
			}
			else 
			{
				typeJudge = (r0y*r1y + r0x*r1x)/(sqrt(r0y*r0y + r0x*r0x)*sqrt(r1x*r1x + r1y*r1y));//�ж��쳤�ͺͲ�����
			}
			zeroError = ((r1x-r0x)*X0 +(r1y-r0y)*Y0);//�ж��쳤�����룩��������
			if( (zeroError>= 0 && (typeJudge>ANGLECONST)) || zeroError <= 0)
			{                 //�쳤�ͺ������ͳ���һ��,���Ժϲ�Ϊһ�γ���*** 
				LineshortenFuction(csChild,asChild,num,X0,Y0,X1,Y1,r0x,r0y,r1x,r1y,Start_X,Start_Y);
			}
			else if( (((r1x-r0x)*X0 +(r1y-r0y)*Y0)>=0) && typeJudge<ANGLECONST )//������
			{
				LineextendFuction(csChild,asChild,num,X0,Y0,X1,Y1,r0x,r0y,r1x,r1y,Start_X,Start_Y);
			}
			if(dataChild->G_compensate == 40)       //ȡ������;
			{
				asChild[*num]= *dataChild;     //��ֵ,����
	            
				if(csChild->G_plane == 18)
				{
					asChild[*num].Z = X1 - (*Start_X);
					asChild[*num].X = Y1 - (*Start_Y);
				}
				else if(csChild->G_plane == 17)
				{
					asChild[*num].X = X1 - (*Start_X);
					asChild[*num].Y = Y1 - (*Start_Y);
				}
				else if(csChild->G_plane == 19)
				{	
					asChild[*num].Y = X1 - (*Start_X);
					asChild[*num].Z = Y1 - (*Start_Y);
				}
				*num=*num+1;
				*c = 0;//Ϊ��һ�ε�������ʼ��.
				*sign=0;
				*Start_X = 0.0;   
				*Start_Y = 0.0;
			}//ȡ������;			
		}//ֱ�߽�ֱ��;

		else if(dataChild->G_pathmode == 1 && (csChild->G_pathmode ==3 || csChild->G_pathmode ==2))//Բ����ֱ��
		{
	//********************************************************************************
			if(csChild->G_plane == 18)
			{
				X0 = csChild->Z;
				Y0 = csChild->X;
				I_one = csChild->K;
				J_one = csChild->I;
				X1 = dataChild->Z;
				Y1 = dataChild->X;
			}
			else if(csChild->G_plane == 17)
			{
				X0 = csChild->X;
				Y0 = csChild->Y;
				I_one = csChild->I;
				J_one = csChild->J;
				X1 = dataChild->X;
				Y1 = dataChild->Y;
			}
			else if(csChild->G_plane == 19)
			{
				X0 = csChild->Y;
				Y0 = csChild->Z;
				I_one = csChild->J;
				J_one = csChild->K;
				X1 = dataChild->Y;
				Y1 = dataChild->Z;
			}
			d0 = sqrt((Y0-J_one)*(Y0-J_one)+(X0-I_one)*(X0-I_one)); 
			d1 = sqrt(X1*X1 +Y1*Y1);           
			if((csChild->G_compensate == 41 && csChild->G_pathmode == 2)||(csChild->G_compensate == 42 && csChild->G_pathmode == 3))
			{
				if(d0>ZERO)
				{
					r0y = R0*(Y0-J_one)/d0;
					r0x = R0*(X0-I_one)/d0;
				}
				else	r0y=r0x=0.0;
			}
			else if((csChild->G_compensate == 42 && csChild->G_pathmode == 2)||(csChild->G_compensate == 41 && csChild->G_pathmode == 3))
			{
				if(d0>ZERO)
				{
					r0y = (-R0)*(Y0-J_one)/d0;
					r0x = (-R0)*(X0-I_one)/d0;
				}
				else	r0y=r0x=0.0;
			}
			if(csChild->G_compensate == 41)
			{
				if(d1>ZERO)
				{
					r1y = R1*X1/d1;
					r1x = (-R1)*Y1/d1;
				}
				else	r1x=r1y=0.0;
			}       
			else if(csChild->G_compensate == 42)
			{
				if(d1>ZERO)
				{
					r1y = (-R1)*X1/d1;
					r1x = R1*Y1/d1;
				}
				else	r1x=r1y=0.0;
			}//ȷ��r0y,r0x,r1y,r1x����;
			if(csChild->G_pathmode == 2)
			{
				KBy0 = -(X0-I_one);
				KBx0 = Y0-J_one;
			}
			else if(csChild->G_pathmode == 3)
			{
				KBy0 = X0-I_one;
				KBx0 = -(Y0-J_one);
			}// ȷ��KB����;
  		   if((sqrt(r0y*r0y + r0x*r0x)*sqrt(r1x*r1x + r1y*r1y))<ZERO) 
		   {   
			   typeJudge=0;
		   }
		   else 
		   {
			   typeJudge = (r0y*r1y + r0x*r1x)/(sqrt(r0y*r0y + r0x*r0x)*sqrt(r1x*r1x + r1y*r1y));
		   }
		   zeroError = ((r1x-r0x)*KBx0 +(r1y-r0y)*KBy0);//�жϽ�������:����,����,�쳤
	                
		   if( (typeJudge>= ANGLECONST && zeroError> ZERO)||(zeroError<-0.0001&&(abs(zeroError)>ZERO))||
			   (abs(zeroError)<ZERO&&abs(typeJudge+1)<ZERO&&((csChild->G_compensate == 42 && csChild->G_pathmode ==3)||
			   (csChild->G_compensate == 41 && csChild->G_pathmode ==2) ))) //������
		   {
			   CircleToLineshortenFuction(csChild,asChild,num,X0,Y0,I_one,J_one,X1,Y1,r0x,r0y,r1x,r1y,Start_X,Start_Y);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
		   }//������
		   else if( abs(zeroError)<ZERO&&abs(r0x*r1y-r1x*r0y)<ZERO&&abs(typeJudge-1)<ZERO) //����
		   {
			   CircleToLinetangentialFuction(csChild,asChild,num,X0,Y0,I_one,J_one,r0x,r0y,r1x,r1y,Start_X,Start_Y);                     
			}//������
		   else if( ((zeroError>ZERO)&&typeJudge<ANGLECONST)||(abs(zeroError)<ZERO&&abs(typeJudge+1)<ZERO&&((csChild->G_compensate == 42 && csChild->G_pathmode ==2)||
			   (csChild->G_compensate == 41 && csChild->G_pathmode ==3) )))//������
		   {
			   CircleToLineextendFuction(csChild,asChild,num,X0,Y0,I_one,J_one,X1,Y1,r0x,r0y,r1x,r1y,Start_X,Start_Y);                     
		   }//����         
		   if(dataChild->G_compensate == 40)       //ȡ������;
		   {
			   asChild[*num] = *dataChild;     //��ֵ,����

			   if(csChild->G_plane == 18)
			   {
				   asChild[*num].I = X1 - (*Start_X);
				   asChild[*num].Y = Y1 - (*Start_Y);
			   }
			   else if(csChild->G_plane == 17)
			   {
				   asChild[*num].X = X1 - (*Start_X);
				   asChild[*num].Y = Y1 - (*Start_Y);
			   }
			   else if(csChild->G_plane == 19)
			   {
				   asChild[*num].Y = X1 - (*Start_X);
				   asChild[*num].Z = Y1 - (*Start_Y);
			   }
			   *num = *num+1;
			   *c = 0;//Ϊ��һ�ε�������ʼ��.
			   *sign=0;
			   *Start_X = 0;   
			   *Start_Y = 0;
		   } //ȡ������
		}//Բ����ֱ��

		else if((dataChild->G_pathmode == 2 || dataChild->G_pathmode == 3) && csChild->G_pathmode ==1)//ֱ�߽�Բ��
		{
			if(csChild->G_plane == 18)
			{
				X0 = csChild->Z;
				Y0 = csChild->X;
				X1 = dataChild->Z;
				Y1 = dataChild->X;
				I_two = dataChild->K;
				J_two = dataChild->I;
			}
			else if(csChild->G_plane == 17)
			{
				X0 = csChild->X;
				Y0 = csChild->Y;
				X1 = dataChild->X;
				Y1 = dataChild->Y;
				I_two = dataChild->I;
				J_two = dataChild->J;
			}
			else if(csChild->G_plane == 19)
			{
				X0 = csChild->Y;
				Y0 = csChild->Z;
				X1 = dataChild->Y;
				Y1 = dataChild->Z;
				I_two = dataChild->J;
				J_two = dataChild->K;
			}
			d0 = sqrt(X0*X0 +Y0*Y0);    
			d1 = sqrt((Y1-J_two)*(Y1-J_two)+(X1-I_two)*(X1-I_two)); 
	        
			if(csChild->G_compensate == 41)
			{
				if(d0>ZERO)
				{
					r0y = R0*X0/d0;
					r0x = (-R0)*Y0/d0; 
				}
				else	r0x=r0y=0.0;
			}       
			else if(csChild->G_compensate == 42)
			{
				if(d0>ZERO)
				{
					r0y = (-R0)*X0/d0;
					r0x = R0*Y0/d0;
				}
				else	r0x=r0y=0.0;
			}
			if(dataChild->G_compensate == 41 && dataChild->G_pathmode == 2||dataChild->G_compensate == 42 && dataChild->G_pathmode == 3)
			{
				if(d1>ZERO)
				{
					r1y = R1*(-J_two)/d1;
					r1x = R1*(-I_two)/d1;
				}
				else	r1x=r1y=0.0;
			}
			else if(dataChild->G_compensate == 42 && dataChild->G_pathmode == 2||dataChild->G_compensate == 41 && dataChild->G_pathmode == 3)
			{
				if(d1>ZERO)
				{
					r1y = R1*J_two/d1;
					r1x = R1*I_two/d1;
				}
				else r1x=r1y=0.0;
			}
			//ȷ��r0y,r0x,r1y,r1x����;
			if(dataChild->G_pathmode == 2)
			{
			   KBy1 = I_two;
			   KBx1 =-J_two;
			}
			else if(dataChild->G_pathmode == 3)
			{
			   KBy1 =-I_two;
			   KBx1 = J_two;
			}// ȷ��KB����,Բ����㴦������;
		   if((sqrt(r0y*r0y + r0x*r0x)*sqrt(r1x*r1x + r1y*r1y))<ZERO) 
		   { 
			   typeJudge=0;
		   }
		   else 
		   {
			   typeJudge = (r0y*r1y + r0x*r1x)/(sqrt(r0y*r0y + r0x*r0x)*sqrt(r1x*r1x + r1y*r1y));
		   }
		   zeroError = ((r1x-r0x)*X0 +(r1y-r0y)*Y0);    //�жϽ�������:����,����,�쳤
		   
		   if((typeJudge >= ANGLECONST && zeroError>ZERO)||(zeroError<-0.0001)||
			   (zeroError == 0&&abs(typeJudge+1)<ZERO&&dataChild->G_compensate == 41 && dataChild->G_pathmode == 2)||
			   (zeroError== 0&&dataChild->G_compensate == 42 && dataChild->G_pathmode == 3)) //������
		   {
			   LineToCircleshortenFuction(csChild,asChild,num,X0,Y0,I_two,J_two,X1,Y1,r0x,r0y,r1x,r1y,Start_X,Start_Y);
		   }//������     
		   else if( (abs(zeroError)<ZERO)&&abs(r0x*r1y-r1x*r0y)<ZERO&& abs(typeJudge-1)<ZERO) //������
		   {
			   LineToCircletangentialFuction(csChild,asChild,num,X0,Y0,I_two,J_two,X1,Y1,r0x,r0y,r1x,r1y,Start_X,Start_Y);
		   }//������
		   else if((zeroError>ZERO && typeJudge<ANGLECONST)||(zeroError<ZERO&&(typeJudge+1)<ZERO&&dataChild->G_compensate == 41 && dataChild->G_pathmode == 3)||
			   (zeroError== 0&&dataChild->G_compensate == 42 && dataChild->G_pathmode == 2))//������
		   {
			   LineToCircleextendFuction(csChild,asChild,num,X0,Y0,I_two,J_two,X1,Y1,r0x,r0y,r1x,r1y,Start_X,Start_Y);
		   }//���� 
		   if(dataChild->G_compensate == 40)       //ȡ������;
		   {
			   MessageBox(hWnd,"ERROR:NOT ALLOW ESCAPE TOOL COMPASETE IN G02 OR G03 in g4142Fuction",NULL,NULL);
		   }//ȡ������
		}//ֱ�߽�Բ��
		else if((dataChild->G_pathmode==2 || dataChild->G_pathmode==3)&& (csChild->G_pathmode==2 || csChild->G_pathmode==3))//Բ����Բ��
		{
			if(csChild->G_plane == 18)
			{
				X0 = csChild->Z;
				Y0 = csChild->X;
				I_one = csChild->K;
				J_one = csChild->I;

				X1 = dataChild->Z;
				Y1 = dataChild->X;
				I_two = dataChild->K;
				J_two = dataChild->I;
			}
			else if(csChild->G_plane == 17)
			{
				X0 = csChild->X;
				Y0 = csChild->Y;
				I_one = csChild->I;
				J_one = csChild->J;

				X1 = dataChild->X;
				Y1 = dataChild->Y;
				I_two = dataChild->I;
				J_two = dataChild->J;
			}
			else if(csChild->G_plane == 19)
			{
				X0 = csChild->Y;
				Y0 = csChild->Z;
				I_one = csChild->J;
				J_one = csChild->K;

				X1 = dataChild->Y;
				Y1 = dataChild->Z;
				I_two = dataChild->J;
				J_two = dataChild->K;
			}
			d0 = sqrt((Y0-J_one)*(Y0-J_one)+(X0-I_one)*(X0-I_one)); 
			d1 = sqrt((Y1-J_two)*(Y1-J_two)+(X1-I_two)*(X1-I_two)); 
		    
			if((csChild->G_compensate == 41 && csChild->G_pathmode == 2)||(csChild->G_compensate == 42 && csChild->G_pathmode == 3))
			{
				if(d0>ZERO)
				{
				r0y = R0*(Y0-J_one)/d0;
				r0x = R0*(X0-I_one)/d0;
				}
				else	r0x=r0y=0.0;
			}
			else if((csChild->G_compensate == 42 && csChild->G_pathmode == 2)||(csChild->G_compensate == 41 && csChild->G_pathmode == 3))
			{
				if(d0>ZERO)
				{
					r0y = (-R0)*(Y0-J_one)/d0;
					r0x = (-R0)*(X0-I_one)/d0;
				}
				else	r0x=r0y=0.0;
			}
			if((dataChild->G_compensate == 41 && dataChild->G_pathmode == 2)||(dataChild->G_compensate == 42 && dataChild->G_pathmode == 3))
			{
				if(d1>ZERO)
				{
					r1y = R1*(-J_two)/d1;
					r1x = R1*(-I_two)/d1;
				}
				else	r1x=r1y=0.0;
			}
			else if((dataChild->G_compensate == 42 && dataChild->G_pathmode == 2)||(dataChild->G_compensate == 41 && dataChild->G_pathmode == 3))
			{
				if(d1>ZERO)
				{
					r1y = R1*J_two/d1;
					r1x = R1*I_two/d1;
				}
				else	r1x=r1y=0.0;
			}
			//ȷ��r0y,r0x,r1y,r1x����;
			 if(csChild->G_pathmode == 2)
			{
				KBy0 = -(X0-I_one);
				KBx0 = Y0-J_one;
			}
			else if(csChild->G_pathmode == 3)
			{
				KBy0 = X0-I_one;
				KBx0 = -(Y0-J_one);
			}// ȷ��KB����;
			if(dataChild->G_pathmode == 2)
			{
				KBy1 = I_two;
				KBx1 = -J_two;
			}
			else if(dataChild->G_pathmode == 3)
			{
				KBy1 = -I_two;
				KBx1 = J_two;
			}// ȷ��KB����,Բ����㴦������;
			if((sqrt(r0y*r0y + r0x*r0x)*sqrt(r1x*r1x + r1y*r1y))<ZERO) 
			{   
				typeJudge=0;
			}
			else 
			{
				typeJudge = (r0y*r1y + r0x*r1x)/(sqrt(r0y*r0y + r0x*r0x)*sqrt(r1x*r1x + r1y*r1y));
			}
			zeroError = ((r1x-r0x)*KBx0 +(r1y-r0y)*KBy0);

			if((typeJudge >= ANGLECONST && zeroError>ZERO)||(zeroError< -0.0001)||(abs(zeroError) <ZERO&&abs(typeJudge+1)<ZERO
				&&((csChild->G_compensate == 42 && csChild->G_pathmode == 3)||(csChild->G_compensate == 41 && csChild->G_pathmode == 2)) )) //������
			{
				CircleToCircleshortenFuction(csChild,asChild,num,X0,Y0,I_one,J_one,X1,Y1,I_two,J_two,r0x,r0y,r1x,r1y,KBx0,KBy0,Start_X,Start_Y);
			}//������
			else   if( (abs(zeroError)<ZERO)&&abs(typeJudge-1)<ZERO) //������
			{
			   CircleToCircletangentialFuction(csChild,asChild,num,X0,Y0,I_one,J_one,X1,Y1,I_two,J_two,r0x,r0y,r1x,r1y,Start_X,Start_Y);
			}//������
			else if(( (zeroError>ZERO) && (typeJudge < ANGLECONST) )||(abs(zeroError)<ZERO&&(typeJudge+1)<ZERO&&((csChild->G_compensate == 42 && csChild->G_pathmode == 2)||
				(csChild->G_compensate == 41 && csChild->G_pathmode == 3))))//������
			{
				CircleToCircleextendFuction(csChild,asChild,num,X0,Y0,I_one,J_one,X1,Y1,I_two,J_two,r0x,r0y,r1x,r1y,KBx0,KBy0,Start_X,Start_Y);
			}//����

			if(dataChild->G_compensate == 40)       //ȡ������;
			{
				MessageBox(hWnd,"ERROR:NOT ALLOW ESCAPE TOOL COMPASETE IN G02 OR G03 in g4142Fuction",NULL,NULL);
			}//ȡ������
		}//Բ����Բ��
	}//���������� if(c != 0)	
	if (dataChild->G_compensate != 40)
	{	
		*csChild = *dataChild;
		*c = 1;			
	}
}	