#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;	

int stringnum(char *p, const char* s); /*�����ַ���p�к����ַ���s�ĸ���, s������һ���ַ�*/

void g151152upFuction(HWND hWnd,int *sign, nc_data *dataChild,nc_data asChild[],int *num,double *Start_B, double *Start_C,int *c, nc_data *csChild);//11
void g151152lowFuction(HWND hWnd,int *sign, nc_data *dataChild,nc_data asChild[],int *num,double *Start_X, double *Start_Y,int *c, nc_data *csChild);//11

void g151152upFuction(HWND hWnd,int *sign, nc_data *dataChild,nc_data asChild[],int *num,double *Start_B, double *Start_C,int *c, nc_data *csChild)
{
    double Y0,X0,Y1,X1,R,d0,d1,r0y,r0x,r1y,r1x;
    double S1x,S1y,S2x,S2y,Sx,Sy,Sx1,Sy1,Sx2,Sy2;
    double J_one,I_one,J_two,I_two,KBy0,KBx0,KBy1,KBx1,Oy,Ox,K_arc,a_arc,b_arc,c_arc;
    double Kline,r_arc;
    double typeJudge;
    double zeroError;

	R=dataChild->D;
	
	if(*c != 0)//����������      
	{			   
                if(dataChild->G_pathmode_upper == 1 && csChild->G_pathmode_upper ==1)//ֱ�߽�ֱ��
                {
                        X0 = csChild->B;
                        Y0 = csChild->C;
                        X1 = dataChild->B;
                        Y1 = dataChild->C;
								
                 //   if(csChild->G_coormode_9x == 91)        //G90�������Ҫ����......................;
					//{
                        d0 = sqrt(X0*X0 +Y0*Y0);    
                        d1 = sqrt(X1*X1 +Y1*Y1);  
						
			         if(csChild->G_Dcompensate == 151)
                        {
                            r0y = R*X0/d0;
                            r0x = (-R)*Y0/d0;

                            r1y = R*X1/d1;
                            r1x = (-R)*Y1/d1;                           
                        }
                       else if(csChild->G_Dcompensate == 152)
                        {
                            r0y = (-R)*X0/d0;
                            r0x = R*Y0/d0;

                            r1y = (-R)*X1/d1;
                            r1x = R*Y1/d1;
					    }
				//	}//csChild->G_coormode_9x == 91
                   typeJudge = (r0y*r1y + r0x*r1x)/(sqrt(r0y*r0y + r0x*r0x)*sqrt(r1x*r1x + r1y*r1y));//�ж��쳤�ͺͲ�����
                   zeroError = ((r1x-r0x)*X0 +(r1y-r0y)*Y0);//�ж��쳤�����룩��������
                //�жϽ�������:����,����,�쳤
                   if( (zeroError >= 0 && (typeJudge> ANGLECONST)) || zeroError <= 0)
				   {                   //�쳤�ͺ������ͳ���һ��,���Ժϲ�Ϊһ�γ���***          
                        if((Y1*X0) == (Y0*X1))
                        {
							S1x= r0x;    S1y=r0y; 
							S2x= X0+X1+r1x;    S2y=Y0+Y1+r1y;
                                                       
							Sx = X0+r0x;
							Sy = Y0+r0y;
                        }   
                        else
                        {
							S1x = r0x;   S1y = r0y; //��һ���߶���㴦�İ뾶ʸ����������;
							S2x = X0+X1+r1x; S2y = Y0+Y1+r1y; //�ڶ������յ㴦�İ뾶ʸ����������;
                        
							Sx = ((S2y-S1y)*X0*X1+Y0*X1*S1x-Y1*X0*S2x)/(Y0*X1-X0*Y1);
							Sy =-((S2x-S1x)*Y0*Y1+X0*Y1*S1y-X1*Y0*S2y)/(Y0*X1-X0*Y1);
                        }//�쳤�ߵĽ�������;
                    
                        asChild[*num] = *csChild;     //��ֵ,����

                        asChild[*num].B = Sx - (*Start_B);
                        asChild[*num].C = Sy - (*Start_C);
               
					   *Start_B = Sx - X0;  
                       *Start_C = Sy - Y0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
				   }//�쳤�Ե���
                else if( (((r1x-r0x)*X0 +(r1y-r0y)*Y0) >= 0) && typeJudge < ANGLECONST )//������
				{
						S1x = X0 + r0x;  S1y =Y0 + r0y; //��һ���߶��յ㴦�İ뾶ʸ����������;
						S2x = X0 + r1x;  S2y =Y0 + r1y;//�ڶ�������㴦�İ뾶ʸ����������;
									//S1,S2���������;
						asChild[*num]= *csChild;     //��ֵ,

						asChild[*num].B = S1x - (*Start_B);
						asChild[*num].C = S1y - (*Start_C);
						
						asChild[*num].row_id=2;

						 *num=*num+1;
						asChild[*num]= *csChild;     //���������

						asChild[*num].B = S2x - S1x;
						asChild[*num].C = S2y - S1y;
						asChild[*num].I1 = -r0x;
						asChild[*num].J1 = -r0y;
						if (csChild->G_Dcompensate == 151)  asChild[*num].G_pathmode_upper = 2;		  
						if (csChild->G_Dcompensate == 152)  asChild[*num].G_pathmode_upper = 3;								    

						*Start_B = r1x; 
						*Start_C = r1y;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
				   }//����
				}//ֱ�߽�ֱ��;
                else if(dataChild->G_pathmode_upper == 1 && (csChild->G_pathmode_upper ==3 || csChild->G_pathmode_upper ==2))//Բ����ֱ��
                {
//********************************************************************************
                        X0 = csChild->B;
                        Y0 = csChild->C;
			            I_one = csChild->I1;
                        J_one = csChild->J1;

                        X1 = dataChild->B;
                        Y1 = dataChild->C;

					//	if(csChild->G_coormode_9x == 91)        //G90�������Ҫ����......................;
					//	{
							d0 = sqrt((Y0-J_one)*(Y0-J_one)+(X0-I_one)*(X0-I_one)); 
							d1 = sqrt(X1*X1 +Y1*Y1);    
	                        
						   if((csChild->G_Dcompensate == 151 && csChild->G_pathmode_upper == 2)||(csChild->G_Dcompensate == 152 && csChild->G_pathmode_upper == 3))
							 {
									r0y = R*(Y0-J_one)/d0;
									r0x = R*(X0-I_one)/d0;
							  }
						   else if((csChild->G_Dcompensate == 152 && csChild->G_pathmode_upper == 2)||(csChild->G_Dcompensate == 151 && csChild->G_pathmode_upper == 3))
							  {
									r0y = (-R)*(Y0-J_one)/d0;
									r0x = (-R)*(X0-I_one)/d0;
							  }

						   if(csChild->G_Dcompensate == 151)
							 {
									r1y = R*X1/d1;
									r1x = (-R)*Y1/d1;                               
							 }       
							else if(csChild->G_Dcompensate == 152)
							 {
									r1y = (-R)*X1/d1;
									r1x = R*Y1/d1;
							 }//ȷ��r0y,r0x,r1y,r1x����;

							if(csChild->G_pathmode_upper == 2)
							 {
								KBy0 = -(X0-I_one);
								KBx0 = Y0-J_one;
							 }
							else if(csChild->G_pathmode_upper == 3)
							 {
								KBy0 = X0-I_one;
								KBx0 = -(Y0-J_one);
							 }// ȷ��KB����;
					//	}//G91
					   typeJudge = (r0y*r1y + r0x*r1x)/(sqrt(r0y*r0y + r0x*r0x)*sqrt(r1x*r1x + r1y*r1y));
					   zeroError = ((r1x-r0x)*KBx0 +(r1y-r0y)*KBy0);  //�жϽ�������:����,����,�쳤
              
                      if( (typeJudge >= ANGLECONST && (zeroError > 0))||(zeroError < 0))
						 //||(zeroError==0&&csChild->G_Dcompensate == 151 && csChild->G_pathmode_upper == 2)||(zeroError==0&&csChild->G_Dcompensate == 152 && csChild->G_pathmode_upper== 3))) //������
					  {
                       //�����Ե���if( typeJudge >= ANGLECONST) && (((r1y-r0y)*KBy0 + (r1x-r0x)*KBx0) >= 0) )//�쳤��
                            S1x = X0+r1x; S1y = Y0+r1y;
                            S2x = X0+X1+r1x;  S2y = Y0+Y1+r1y;//��Բ���ཻ��ֱ�ߵ�������;

                            Ox = I_one;   Oy = J_one;//Բ������;
                            r_arc = sqrt( (X0+r0x-I_one)*(X0+r0x-I_one) + (Y0+r0y-J_one)*(Y0+r0y-J_one) );//Բ���뾶

                            if(S1x == S2x)
                            {
								Sx1 = S1x ;   Sy1 = Oy - sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));
								Sx2 = S1x ;   Sy2 = Oy + sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));
                            }
                            else
                            {
								K_arc = (S2y-S1y)/(S2x-S1x);
								a_arc = 1+K_arc*K_arc;
								b_arc = 2*K_arc*(S1y-Oy-K_arc*S1x)-2*Ox;
								c_arc = (S1y-Oy-K_arc*S1x)*(S1y-Oy-K_arc*S1x)-r_arc*r_arc+Ox*Ox;
	                            
								Sx1 = ( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sy1=S1y + K_arc*(Sx1-S1x);
								Sx2 = ( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sy2=S1y + K_arc*(Sx2-S1x);
                            }//�쳤�ߵĽ�������;
                            
							if( (( (X0-I_one) *X1+(Y0-J_one)*Y1)>=0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)<=0) || (((X0-I_one)*X1+(Y0-J_one)*Y1)<0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)>0) )
							{
								Sy = Sy1;
								Sx = Sx1;
							}
							else if( (((X0-I_one)*X1+(Y0-J_one)*Y1)>=0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)>0) || (((X0-I_one)*X1+(Y0-J_one)*Y1)<0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)<=0) )
							{
								Sy = Sy2;
								Sx = Sx2;
							}
                            asChild[*num] = *csChild;     //��ֵ,����
  
                            asChild[*num].B = Sx - (*Start_B);
                            asChild[*num].C = Sy - (*Start_C);
			                asChild[*num].I1 = I_one-(*Start_B);
                            asChild[*num].J1 = J_one-(*Start_C);

                            *Start_B = Sx - X0;  
                            *Start_C = Sy - Y0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;                                                                                                                                                                                                                                                                                                                                                                                                                                                    
						}//������
                        
					  else if  ( zeroError==0 )// ������
						{
                            if( (r0x==r1x)&&(r0y==r1y) )
							{
								Sx = X0+r0x;
								Sy = Y0+r0y;
									
								asChild[*num]= *csChild;  
								
								asChild[*num].B = Sx - (*Start_B);
								asChild[*num].C = Sy - (*Start_C);//��һ��Բ�����յ�
								asChild[*num].I1 = I_one - (*Start_B);
								asChild[*num].J1 = J_one - (*Start_C);//Բ����Բ������
								
								(*Start_B) = Sx - X0;	
								(*Start_C) = Sy - Y0;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
							}
							else
							{
								Sx = X0+r0x;
								Sy = Y0+r0y;
									
								asChild[*num]= *csChild;  
								
								asChild[*num].B = Sx - (*Start_B);
								asChild[*num].C = Sy - (*Start_C);//��һ��Բ�����յ�
								asChild[*num].I1 = I_one - (*Start_B);
								asChild[*num].J1 = J_one - (*Start_C);//Բ����Բ������

                                asChild[*num].row_id=2;
								
								*num=*num+1;
								asChild[*num]=*csChild;

								asChild[*num].B = r1x - r0x;
								asChild[*num].C = r1y - r0y;//��һ��Բ�����յ�
								asChild[*num].I1 = -r0x;
								asChild[*num].J1 = -r0y;//Բ����Բ������

								if(csChild->G_pathmode_upper == 3) 
									asChild[*num].G_pathmode_upper = 2;
								if (csChild->G_pathmode_upper == 2) 
									asChild[*num].G_pathmode_upper = 3; 
								
								*Start_B = r1x;	
								*Start_C = r1y;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
							}
						}

                    else if( ((zeroError >0)&&typeJudge < ANGLECONST)||(zeroError==0&&csChild->G_Dcompensate == 152 && csChild->G_pathmode_upper ==2)||(zeroError==0&&csChild->G_Dcompensate == 151 && csChild->G_pathmode_upper ==3) )//������
                        {    
                            S1x = X0 + r0x;  S1y = Y0 + r0y; //��һ���߶��յ㴦�İ뾶ʸ����������;
                            S2x = X0 + r1x;  S2y = Y0 + r1y;//�ڶ�������㴦�İ뾶ʸ����������;
                                        //S1,S2���������;

                            asChild[*num] = *csChild;     //��ֵ,����,Բ������
                            
                            asChild[*num].B = S1x - (*Start_B);
                            asChild[*num].C = S1y - (*Start_C);
                            asChild[*num].I1 = I_one-(*Start_B);
                            asChild[*num].J1 = J_one-(*Start_C);
                            
							asChild[*num].row_id=2;
                           
							*num = *num+1;
                            asChild[*num] = *csChild;     //��ֵ,����,�쳤��ֱ��S1S3����;      

			                asChild[*num].B = S2x - S1x;
                            asChild[*num].C = S2y - S1y;
                            asChild[*num].I1 = -r0x;
                            asChild[*num].J1 = -r0y; 
                           
							if(csChild->G_pathmode_upper ==3)  asChild[*num].G_pathmode_upper = 2;				                
                            if (csChild->G_pathmode_upper ==2) asChild[*num].G_pathmode_upper = 3;
                           
							*Start_B = r1x; 
                            *Start_C = r1y;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                        }//����         
				}//Բ����ֱ��
                else if((dataChild->G_pathmode_upper == 2 || dataChild->G_pathmode_upper == 3) && csChild->G_pathmode_upper ==1)//ֱ�߽�Բ��
                {
                        X0 = csChild->B;
                        Y0 = csChild->C;

                        X1 = dataChild->B;
                        Y1 = dataChild->C;
                        I_two = dataChild->I1;
                        J_two = dataChild->J1;

				//		if(csChild->G_coormode_9x == 91)        //G90�������Ҫ����......................;
				//		{
							d0 = sqrt(X0*X0 +Y0*Y0);    
							d1 = sqrt((Y1-J_two)*(Y1-J_two)+(X1-I_two)*(X1-I_two)); 
	                        
							if(csChild->G_Dcompensate == 151)
							{
								r0y = R*X0/d0;
								r0x = (-R)*Y0/d0;                               
							}       
							else if(csChild->G_Dcompensate == 152)
							{
								r0y = (-R)*X0/d0;
								r0x = R*Y0/d0;
							}
							if(dataChild->G_Dcompensate == 151 && dataChild->G_pathmode_upper == 2||dataChild->G_Dcompensate == 152 && dataChild->G_pathmode_upper == 3)
							{
								r1y = R*(-J_two)/d1;
								r1x = R*(-I_two)/d1;
							}
							else if(dataChild->G_Dcompensate == 152 && dataChild->G_pathmode_upper == 2||dataChild->G_Dcompensate == 151 && dataChild->G_pathmode_upper == 3)
							{
								r1y = R*J_two/d1;
								r1x = R*I_two/d1;
							} //ȷ��r0y,r0x,r1y,r1x����;
                       
                        if(dataChild->G_pathmode_upper == 2)
                        {
                            KBy1 = I_two;
                            KBx1 =-J_two;
                        }
                        else if(dataChild->G_pathmode_upper == 3)
                        {
                            KBy1 =-I_two;
                            KBx1 = J_two;
                        }// ȷ��KB����,Բ����㴦������;
				//	}//G91
                   typeJudge = (r0y*r1y + r0x*r1x)/(sqrt(r0y*r0y + r0x*r0x)*sqrt(r1x*r1x + r1y*r1y));
                   zeroError = ((r1x-r0x)*X0 +(r1y-r0y)*Y0);
                //�жϽ�������:����,����,�쳤
  // if( (typeJudge >= ANGLECONST) && (((r1y-r0y)*Y0 + (r1x-r0x)*X0) >= 0) )//�쳤�� 
				   if(((typeJudge >= ANGLECONST) && (zeroError >= 0))||(zeroError < 0))
							 //||((zeroError== 0)&&dataChild->G_compensate == 151 && dataChild->G_pathmode_upper == 2)||((zeroError== 0)&&dataChild->G_compensate == 152 && dataChild->G_pathmode_upper == 3)) //������
				   {
							S2x = r0x;    S2y = r0y;
                            S1x = X0+r0x; S1y = Y0+r0y;//��Բ���ཻ��ֱ�ߵ�������;

                            Ox = X0+I_two;    Oy = Y0+J_two;//Բ������;
                            r_arc = sqrt( (X0+r1x-Ox)*(X0+r1x-Ox) + (Y0+r1y-Oy)*(Y0+r1y-Oy) );//Բ���뾶

                            if(S1x == S2x)
								{
									Sx1=S1x ;	Sy1=Oy - sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));
									Sx2=S1x ;	Sy2=Oy + sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));
								}
							else
                            {
								K_arc = (S2y-S1y)/(S2x-S1x);
								a_arc = 1+K_arc*K_arc;
								b_arc = 2*K_arc*(S1y-Oy-K_arc*S1x)-2*Ox;
								c_arc =( S1y-Oy-K_arc*S1x)*(S1y-Oy-K_arc*S1x)-r_arc*r_arc+Ox*Ox;
		                            
			                    Sx1=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sy1=S1y + K_arc*(Sx1-S1x);								
								Sx2=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sy2=S1y + K_arc*(Sx2-S1x);

                            }//�쳤�ߵĽ�������;
		   
                             if( (((-I_two)*X0+(-J_two)*Y0)>=0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)<=0) || (((-I_two)*X0+(-J_two)*Y0)<=0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)>0) )
							 {
									Sx=Sx1;
									Sy=Sy1;
							 }
                            else if( (((-I_two)*X0+(-J_two)*Y0)>0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)>0) || (((-I_two)*X0+(-J_two)*Y0)<0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)<=0) )
							{
									Sx=Sx2;
									Sy=Sy2;
							 }
						
                            asChild[*num]= *csChild;     //��ֵ,����

                            asChild[*num].B = Sx - (*Start_B);
                            asChild[*num].C = Sy - (*Start_C);

                            *Start_B = Sx - X0;  
                            *Start_C = Sy - Y0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
				   }//������
                   else if  ( zeroError==0 )// ������
				   {
					   if((r0x==r1x)&&(r0y==r1y))
					   {
							Sx = X0+r0x;
							Sy = Y0+r0y;
							asChild[*num]= *csChild;  
							
							asChild[*num].B = Sx - (*Start_B);
							asChild[*num].C = Sy - (*Start_C);//��һ��Բ�����յ�

							*Start_B = Sx - X0;	
							*Start_C = Sy - Y0;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
					   }
					   else
					   {	
							Sx = X0+r0x;
							Sy = Y0+r0y;
								
							asChild[*num]= *csChild;  
							
							asChild[*num].B = Sx - (*Start_B);
							asChild[*num].C = Sy - (*Start_C);//��һ��Բ�����յ�
						
							asChild[*num].row_id=2;
							*num=*num+1;
							asChild[*num]=*csChild;

							asChild[*num].B = r1x - r0x;
							asChild[*num].C = r1y - r0y;//��һ��Բ�����յ�

							asChild[*num].I1 = -r0x;
							asChild[*num].J1 = -r0y;//Բ����Բ������

							if(dataChild->G_pathmode_upper == 3) 
								asChild[*num].G_pathmode_upper = 2;
							if (dataChild->G_pathmode_upper == 2) 
								asChild[*num].G_pathmode_upper = 3; 
							
							*Start_B = r1x;	
							*Start_C = r1y;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
					   }
				   }
				else if((zeroError > 0 && typeJudge < ANGLECONST)||(zeroError== 0&&dataChild->G_compensate == 151 && dataChild->G_pathmode_upper == 3)||(zeroError== 0&&dataChild->G_compensate == 152 && dataChild->G_pathmode_upper == 2))//������
				{                           
						S1x = X0 + r0x;  S1y =Y0 + r0y; //��һ���߶��յ㴦�İ뾶ʸ����������;
                        S2x = X0 + r1x;  S2y =Y0 + r1y;//�ڶ�������㴦�İ뾶ʸ����������;
                                    //S1,S2���������;                    
						asChild[*num]= *csChild;     //��ֵ,����,��һ��ֱ��

						asChild[*num].B = S1x - (*Start_B);
						asChild[*num].C = S1y - (*Start_C);

						asChild[*num].row_id=2;

						*num=*num+1;
						asChild[*num] = *csChild;     //��ֵ,����,����Բ��;                 

						asChild[*num].B = S2x - S1x;
						asChild[*num].C = S2y - S1y;
						asChild[*num].I1 = -r0x;
						asChild[*num].J1 = -r0y;
						if(dataChild->G_pathmode_upper == 3) asChild[*num].G_pathmode_upper = 2;
						if (dataChild->G_pathmode_upper == 2) asChild[*num].G_pathmode_upper = 3;

						*Start_B = r1x; 
						*Start_C = r1y;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
				   }//����         				
            }//ֱ�߽�Բ��
            else if((dataChild->G_pathmode_upper==2 || dataChild->G_pathmode_upper==3)&& (csChild->G_pathmode_upper==2 || csChild->G_pathmode_upper==3))//Բ����Բ��
                {
						X0 = csChild->B;
						Y0 = csChild->C;
						I_one = csChild->I1;
						J_one = csChild->J1;

						X1 = dataChild->B;
						Y1 = dataChild->C;
						I_two = dataChild->I1;
						J_two = dataChild->J1;
                
            //    if(csChild->G_coormode_9x == 91)        //G90�������Ҫ����......................;
			//	  {
                    d0 = sqrt((Y0-J_one)*(Y0-J_one)+(X0-I_one)*(X0-I_one)); 
                    d1 = sqrt((Y1-J_two)*(Y1-J_two)+(X1-I_two)*(X1-I_two)); 
                    
					if((csChild->G_Dcompensate == 151 && csChild->G_pathmode_upper == 2)||(csChild->G_Dcompensate == 152 && csChild->G_pathmode_upper == 3))
									{
										r0y = R*(Y0-J_one)/d0;
										r0x = R*(X0-I_one)/d0;
									}
					else if((csChild->G_Dcompensate == 152 && csChild->G_pathmode_upper == 2)||(csChild->G_Dcompensate == 151 && csChild->G_pathmode_upper == 3))
									{
										r0y = (-R)*(Y0-J_one)/d0;
										r0x = (-R)*(X0-I_one)/d0;
									}
					if((dataChild->G_Dcompensate == 151 && dataChild->G_pathmode_upper == 2)||(dataChild->G_Dcompensate == 152 && dataChild->G_pathmode_upper == 3))
									{
										r1y = R*(-J_two)/d1;
										r1x = R*(-I_two)/d1;
									}
					else if((dataChild->G_Dcompensate == 152 && dataChild->G_pathmode_upper == 2)||(dataChild->G_Dcompensate == 151 && dataChild->G_pathmode_upper == 3))
									{
										r1y = R*J_two/d1;
										r1x = R*I_two/d1;
									}//ȷ��r0y,r0x,r1y,r1x����;
                    
                     if(csChild->G_pathmode_upper == 2)
                    {
                        KBy0 = -(X0-I_one);
                        KBx0 = Y0-J_one;
                    }
                    else if(csChild->G_pathmode_upper == 3)
                    {
                        KBy0 = X0-I_one;
                        KBx0 = -(Y0-J_one);
                    }// ȷ��KB����;
                    if(dataChild->G_pathmode_upper == 2)
                    {
                        KBy1 = I_two;
                        KBx1 = -J_two;
                    }
                    else if(dataChild->G_pathmode_upper == 3)
                    {
                        KBy1 = -I_two;
                        KBx1 = J_two;
                    }// ȷ��KB����,Բ����㴦������;
              //  }//G91           
                  typeJudge = (r0y*r1y + r0x*r1x)/(sqrt(r0y*r0y + r0x*r0x)*sqrt(r1x*r1x + r1y*r1y));
                   zeroError = ((r1x-r0x)*KBx0 +(r1y-r0y)*KBy0);

            //�жϽ�������:����,����,�쳤
                    //if( ((r0y*r1y + r0x*r1x) >= ZERO) && (((r1y-r0y)*KBy0 + (r1x-r0x)*KBx0) >= 0) )//�쳤��
					if( ( (typeJudge >= ANGLECONST) && (zeroError > 0)) || (zeroError< 0)) //||((zeroError== 0)&&csChild->G_compensate == 151 && csChild->G_pathmode_upper == 2)||((zeroError== 0)&&csChild->G_compensate == 152 && csChild->G_pathmode_upper == 3))) //������
                    { //�����Ե���
	                    S1x = X0; S1y = Y0;
                    
                        Ox=I_one;   Oy=J_one;//Բ������;
                        r_arc=sqrt( (X0+r0x-I_one)*(X0+r0x-I_one) + (Y0+r0y-J_one)*(Y0+r0y-J_one) );//Բ���뾶
                        
                        if(X0+I_two-Ox==0)
                        {
							S2x = I_one;
							S2y = S1y;

							Sy1=S1y ;	Sx1=Ox + sqrt(r_arc*r_arc-(S2y-Oy)*(S2y-Oy));
							Sy2=S1y ;	Sx2=Ox - sqrt(r_arc*r_arc-(S2y-Oy)*(S2y-Oy));

                        }
                        else if(Y0+J_two-Oy==0)
                        {
							S2x = S1x; 
							S2y = J_one;
							
							Sx1=S1x ;	Sy1=Oy + sqrt(r_arc*r_arc-(S2x-Ox)*(S2x-Ox));
							Sx2=S1x ;	Sy2=Oy - sqrt(r_arc*r_arc-(S2x-Ox)*(S2x-Ox));
						
						}
                        else
                        {
							Kline = (Y0+J_two - Oy)/(X0+I_two - Ox);//����Բ�ĵ�ֱ��б��;

							S2y=Kline*(X0-Ox+Oy/Kline+Kline*Y0)/(1+Kline*Kline);
							S2x=(S2y+Oy+Kline*Ox)/Kline;
						}
                     
						if(S1x == S2x)
						{
							Sx1=S1x ;	Sy1=Oy + sqrt(r_arc*r_arc-(S2x-Ox)*(S2x-Ox));
							Sx2=S1x ;	Sy2=Oy - sqrt(r_arc*r_arc-(S2x-Ox)*(S2x-Ox));
						}
						else
						{
							K_arc = (S2y-S1y)/(S2x-S1x);
							a_arc = 1+K_arc*K_arc;
							b_arc = 2*K_arc*(S1y-Oy-K_arc*S1x) -2*Ox;
							c_arc = (S1y-Oy-K_arc*S1x)*(S1y-Oy-K_arc*S1x)-r_arc*r_arc+Ox*Ox;
							
							Sx2 = ( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sy2=S1y + K_arc*(Sx2-S1x);	                        
							Sx1 = ( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sy1=S1y + K_arc*(Sx1-S1x);

						}
								//if( (((Sx2-Sx1)*(X0-I_one)+(Sy2-Sy1)*(Y0-J_one))>0 && ((Sx2-Sx1)*KBx0+(Sy2-Sy1)*KBy0)<0) || (((Sx2-Sx1)*(X0-I_one)+(Sy2-Sy1)*(Y0-J_one))<=0 && ((Sx2-Sx1)*KBx0+(Sy2-Sy1)*KBy0)>=0 ))
					     if( ((KBx0*(X0-I_one)+KBy0*(Y0-J_one))>=0 && ((Sx2-Sx1)*KBx0+(Sy2-Sy1)*KBy0)<=0) || ((KBx0*(X0-I_one)+KBy0*(Y0-J_one))<=0 && ((Sx2-Sx1)*KBx0+(Sy2-Sy1)*KBy0)>0 ))
						{
								Sx=Sx1;
								Sy=Sy1;
						}
						//else if( (((Sx2-Sx1)*(X0-I_one)+(Sy2-Sy1)*(Y0-J_one))>0 && ((Sx2-Sx1)*KBx0+(Sy2-Sy1)*KBy0)>=0) || (((Sx2-Sx1)*(X0-I_one)+(Sy2-Sy1)*(Y0-J_one))<=0 && ((Sx2-Sx1)*KBx0+(Sy2-Sy1)*KBy0)<0 ))
						else if( ((KBx0*(X0-I_one)+KBy0*(Y0-J_one))>=0 && ((Sx2-Sx1)*KBx0+(Sy2-Sy1)*KBy0)>0) || ((KBx0*(X0-I_one)+KBy0*(Y0-J_one))<0 && ((Sx2-Sx1)*KBx0+(Sy2-Sy1)*KBy0)<=0 ))
						 {
								Sx=Sx2;
								Sy=Sy2;
						}
                            asChild[*num]= *csChild;     //��ֵ,����

                            asChild[*num].B = Sx - (*Start_B);
                            asChild[*num].C = Sy - (*Start_C);
                            asChild[*num].I1 = I_one-(*Start_B);
                            asChild[*num].J1 = J_one-(*Start_C);

                            *Start_B = Sx - X0;  
                            *Start_C = Sy - Y0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;

                        }//������
						 else if  ( zeroError==0 )// ������
						{
                           if((r0x==r1x)&&(r0y==r1y))
						   {
								Sx = X0+r0x;
								Sy = Y0+r0y;
									
								asChild[*num]= *csChild;  
								
								asChild[*num].B = Sx - (*Start_B);
								asChild[*num].C = Sy - (*Start_C);//��һ��Բ�����յ�
								asChild[*num].I1 = I_one - (*Start_B);
								asChild[*num].J1 = J_one - (*Start_C);//Բ����Բ������
								
								*Start_B = Sx - X0;	
								*Start_C = Sy - Y0;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
						   }
						   else 
						   {
								Sx = X0+r0x;
								Sy = Y0+r0y;
									
								asChild[*num]= *csChild;  
								
								asChild[*num].B = Sx - (*Start_B);
								asChild[*num].C = Sy - (*Start_C);//��һ��Բ�����յ�
								asChild[*num].I1 = I_one - (*Start_B);
								asChild[*num].J1 = J_one - (*Start_C);//Բ����Բ������
						        asChild[*num].row_id=2;
								*num=*num+1;
								asChild[*num]= *csChild;
								
								asChild[*num].B = r1x - r0x;
								asChild[*num].C = r1y - r0y;//��һ��Բ�����յ�
								asChild[*num].I1 = -r0x;
								asChild[*num].J1 = -r0y;//Բ����Բ������
								
								if(csChild->G_pathmode_lower == 3) 
									asChild[*num].G_pathmode_lower = 2;
								if (csChild->G_pathmode_lower == 2) 
									asChild[*num].G_pathmode_lower = 3; 
								
								*Start_B = Sx - X0;	
								*Start_C = Sy - Y0;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;

						   }
						}//������
                    else if(( (zeroError > 0) && (typeJudge < ANGLECONST) )||((zeroError == 0)&&csChild->G_compensate == 152 && csChild->G_pathmode_upper == 2)||((zeroError== 0)&&csChild->G_compensate == 151 && csChild->G_pathmode_upper == 3))//������
                        {
						  //�����Ե���
                            S1x = X0 + r0x;  S1y = Y0 + r0y; //��һ���߶��յ㴦�İ뾶ʸ����������;
                            S2x = X0 + r1x;  S2y = Y0 + r1y;//�ڶ�������㴦�İ뾶ʸ����������;
                                        //S1,S2���������;

                            asChild[*num]= *csChild;     //��ֵ,����,Բ������
                       
                            asChild[*num].B = S1x - (*Start_B);
                            asChild[*num].C = S1y - (*Start_C);
                            asChild[*num].I1 = I_one-(*Start_B);
                            asChild[*num].J1 = J_one-(*Start_C);
                      
							asChild[*num].row_id=2;
                             
							*num=*num+1;
                            asChild[*num] = *csChild;     //��ֵ,����,Բ������
                            asChild[*num].G_coormode_9x=91;

                            asChild[*num].B = S2x - S1x;
                            asChild[*num].C = S2y - S1y;
                            asChild[*num].I1 = -r0x;
                            asChild[*num].J1 = -r0y;
							if(csChild->G_pathmode_upper == 3) 
								asChild[*num].G_pathmode_upper = 2;
							if (csChild->G_pathmode_upper == 2) 
								asChild[*num].G_pathmode_upper = 3; 

                            *Start_B = r1x;  
                            *Start_C = r1y;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                        }//����
                     }

			}//���������� if(c != 0)
			if (dataChild->G_Dcompensate != 150)
			{	
				*c = 1;
			}
			else if(dataChild->G_Dcompensate = 150)
			{
				if( dataChild->G_pathmode_upper==1 )
				{                              
		            *num=*num+1;
		            asChild[*num]= *dataChild;     //��ֵ,����                            

                    asChild[*num].B = dataChild->X - (*Start_B);
                    asChild[*num].C = dataChild->Y - (*Start_C);
					
                    *c = 0;//Ϊ��һ�ε�������ʼ��.
                    *Start_B = 0.;   
                    *Start_C = 0.;
				}
				else if(dataChild->G_pathmode_upper==2||dataChild->G_pathmode_upper==3)
				{
					MessageBox(hWnd,"ERROR:NOT ALLOW ESCAPE TOOL COMPASETE IN G02 OR G03 in g4142Fuction",NULL,NULL);
				}
			}
}	   

void g151152lowFuction(HWND hWnd,int *sign, nc_data *dataChild,nc_data asChild[],int *num,double *Start_X, double *Start_Y,int *c, nc_data *csChild)
{
    double Y0,X0,Y1,X1,R,d0,d1,r0y,r0x,r1y,r1x;
    double S1x,S1y,S2x,S2y,Sx,Sy,Sx1,Sy1,Sx2,Sy2;
    double J_one,I_one,J_two,I_two,KBy0,KBx0,KBy1,KBx1,Oy,Ox,K_arc,a_arc,b_arc,c_arc;
    double Kline,r_arc;
    double typeJudge;
	double zeroError;

    R=dataChild->D;

          if(*c != 0)//����������      
            {   
                if(dataChild->G_pathmode_lower == 1 && csChild->G_pathmode_lower ==1)//ֱ�߽�ֱ��
                {
                        X0 = csChild->X;
                        Y0 = csChild->Y;
                        X1 = dataChild->X;
                        Y1 = dataChild->Y;							
                
                 //   if(csChild->G_coormode_9x == 91)        //G90�������Ҫ����......................;
				//	{
                        d0 = sqrt(X0*X0 +Y0*Y0);    
                        d1 = sqrt(X1*X1 +Y1*Y1);  
						
						if(csChild->G_Dcompensate == 151)
                        {
                            r0y = R*X0/d0;
                            r0x = (-R)*Y0/d0;

                            r1y = R*X1/d1;
                            r1x = (-R)*Y1/d1;                           
                        }
                       else if(csChild->G_Dcompensate == 152)
                        {
                            r0y = (-R)*X0/d0;
                            r0x = R*Y0/d0;

                            r1y = (-R)*X1/d1;
                            r1x = R*Y1/d1;
					    }
						
				//	}//csChild->G_coormode_9x == 91

                   typeJudge = (r0y*r1y + r0x*r1x)/(sqrt(r0y*r0y + r0x*r0x)*sqrt(r1x*r1x + r1y*r1y));//�ж��쳤�ͺͲ�����
                   zeroError = ((r1x-r0x)*X0 +(r1y-r0y)*Y0);//�ж��쳤�����룩��������					
					    //MessageBox(hWnd,_fcvt( r0y, 7, &decimal, &tsign ),NULL,NULL); 				
                //�жϽ�������:����,����,�쳤
                        if( (zeroError >= 0 && (typeJudge> ANGLECONST)) || zeroError <= 0)
                        {                   //�쳤�ͺ������ͳ���һ��,���Ժϲ�Ϊһ�γ���***                    							
                            if((Y1*X0) == (Y0*X1))
                            {
								S1x= r0x;    S1y=r0y; 
								S2x= X0+X1+r1x;    S2y=Y0+Y1+r1y;
	                                                       
								Sx = X0+r0x;
								Sy = Y0+r0y;
                            }   
                            else
                            {
								S1x = r0x;   S1y = r0y; //��һ���߶���㴦�İ뾶ʸ����������;
								S2x = X0+X1+r1x; S2y = Y0+Y1+r1y; //�ڶ������յ㴦�İ뾶ʸ����������;
	                        
								Sx = ((S2y-S1y)*X0*X1+Y0*X1*S1x-Y1*X0*S2x)/(Y0*X1-X0*Y1);
								Sy =-((S2x-S1x)*Y0*Y1+X0*Y1*S1y-X1*Y0*S2y)/(Y0*X1-X0*Y1);
                            }//�쳤�ߵĽ�������;                        
                                
							asChild[*num].X = Sx - (*Start_X);
                            asChild[*num].Y = Sy - (*Start_Y);

                            *Start_X = Sx - X0;  
                            *Start_Y = Sy - Y0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
						}//�쳤�Ե���         
                    else if( (((r1x-r0x)*X0 +(r1y-r0y)*Y0) >= 0) && typeJudge < ANGLECONST )//������
                        {
                            S1x = X0 + r0x;  S1y =Y0 + r0y; //��һ���߶��յ㴦�İ뾶ʸ����������;
                            S2x = X0 + r1x;  S2y =Y0 + r1y;//�ڶ�������㴦�İ뾶ʸ����������;
                                        //S1,S2���������;
                            asChild[*num]= *csChild;     //��ֵ,
                            asChild[*num].X = S1x - (*Start_X);
                            asChild[*num].Y = S1y - (*Start_Y);

							asChild[*num].row_id=2;
                           
							*num=*num+1;                   
                            asChild[*num]= *csChild; //���������
							
                            asChild[*num].X = S2x - S1x;
                            asChild[*num].Y = S2y - S1y;
                            asChild[*num].I = -r0x;
                            asChild[*num].J = -r0y;
							if (csChild->G_compensate == 41) 
								   asChild[*num].G_pathmode_lower = 2;
							if (csChild->G_compensate == 42) 
								    asChild[*num].G_pathmode_lower = 3;
  
                            *Start_X = r1x; 
                            *Start_Y = r1y;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
						}//����						 					            
				}//ֱ�߽�ֱ��;
                else if(dataChild->G_pathmode_lower == 1 && (csChild->G_pathmode_lower ==3 || csChild->G_pathmode_lower ==2))//Բ����ֱ��
                {
//********************************************************************************
                        X0 = csChild->X;
                        Y0 = csChild->Y;
						I_one = csChild->I;
                        J_one = csChild->J;

                        X1 = dataChild->X;
                        Y1 = dataChild->Y;

              //      if(csChild->G_coormode_9x == 91)        //G90�������Ҫ����......................;
              //      {
                        d0 = sqrt((Y0-J_one)*(Y0-J_one)+(X0-I_one)*(X0-I_one)); 
                        d1 = sqrt(X1*X1 +Y1*Y1);    
                        
                       if((csChild->G_Dcompensate == 151 && csChild->G_pathmode_lower == 2)||(csChild->G_Dcompensate == 151 && csChild->G_pathmode_lower == 3))
                         {
								r0y = R*(Y0-J_one)/d0;
								r0x = R*(X0-I_one)/d0;
                          }
                        else if((csChild->G_Dcompensate == 152 && csChild->G_pathmode_lower == 2)||(csChild->G_Dcompensate == 152 && csChild->G_pathmode_lower == 3))
                          {
								r0y = (-R)*(Y0-J_one)/d0;
								r0x = (-R)*(X0-I_one)/d0;
                          }

                        if(csChild->G_Dcompensate == 151)
                         {
								r1y = R*X1/d1;
								r1x = (-R)*Y1/d1;                               
                         }       
                        else if(csChild->G_Dcompensate == 152)
                         {
								r1y = (-R)*X1/d1;
								r1x = R*Y1/d1;
                         }//ȷ��r0y,r0x,r1y,r1x����;

                        if(csChild->G_pathmode_lower == 2)
                         {
                            KBy0 = -(X0-I_one);
                            KBx0 = Y0-J_one;
                         }
                        else if(csChild->G_pathmode_lower == 3)
                         {
                            KBy0 = X0-I_one;
                            KBx0 = -(Y0-J_one);
                         }// ȷ��KB����;
				//	}//G91
                   typeJudge = (r0y*r1y + r0x*r1x)/(sqrt(r0y*r0y + r0x*r0x)*sqrt(r1x*r1x + r1y*r1y));
                   zeroError = ((r1x-r0x)*KBx0 +(r1y-r0y)*KBy0);
                //�жϽ�������:����,����,�쳤

                      if( (typeJudge >= ANGLECONST && (zeroError > 0)) ||(zeroError < 0))
						 // ||(zeroError==0&&csChild->G_Dcompensate == 151 && csChild->G_pathmode_lower == 2)||(zeroError==0&&csChild->G_Dcompensate == 152 && csChild->G_pathmode_lower== 3))) //������
                     {
                       //�����Ե���if( typeJudge >= ANGLECONST) && (((r1y-r0y)*KBy0 + (r1x-r0x)*KBx0) >= 0) )//�쳤��
                            S1x = X0+r1x; S1y = Y0+r1y;
                            S2x = X0+X1+r1x;  S2y = Y0+Y1+r1y;//��Բ���ཻ��ֱ�ߵ�������;

                            Ox = I_one;   Oy = J_one;//Բ������;
                            r_arc = sqrt( (X0+r0x-I_one)*(X0+r0x-I_one) + (Y0+r0y-J_one)*(Y0+r0y-J_one) );//Բ���뾶

                            if(S1x == S2x)
                            {
								Sx1 = S1x ;   Sy1 = Oy - sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));
								Sx2 = S1x ;   Sy2 = Oy + sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));
                            }
                            else
                            {
								K_arc = (S2y-S1y)/(S2x-S1x);
								a_arc = 1+K_arc*K_arc;
								b_arc = 2*K_arc*(S1y-Oy-K_arc*S1x)-2*Ox;
								c_arc = (S1y-Oy-K_arc*S1x)*(S1y-Oy-K_arc*S1x)-r_arc*r_arc+Ox*Ox;
	                            
								Sx1 = ( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sy1=S1y + K_arc*(Sx1-S1x);
								Sx2 = ( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sy2=S1y + K_arc*(Sx2-S1x);
                            }//�쳤�ߵĽ�������;
                            
                        if( (( (X0-I_one) *X1+(Y0-J_one)*Y1)>=0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)<=0) || (((X0-I_one)*X1+(Y0-J_one)*Y1)<0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)>0) )
						{
								Sy = Sy1;
								Sx = Sx1;
						}
						else if( (((X0-I_one)*X1+(Y0-J_one)*Y1)>=0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)>0) || (((X0-I_one)*X1+(Y0-J_one)*Y1)<0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)<=0) )
						{
								Sy = Sy2;
								Sx = Sx2;
						}
                            asChild[*num] = *csChild;     //��ֵ,����

                            asChild[*num].X = Sx - (*Start_X);
                            asChild[*num].Y = Sy - (*Start_Y);
							asChild[*num].I = I_one-(*Start_X);
                            asChild[*num].J = J_one-(*Start_Y);

                            *Start_X = Sx - X0;  
                            *Start_Y = Sy - Y0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
						}//������
                		else if  ( zeroError==0 )// ������
						{
                            if( (r0x==r1x)&&(r0y==r1y) )
							{
								Sx = X0+r0x;
								Sy = Y0+r0y;
									
								asChild[*num]= *csChild;  
								
								asChild[*num].X = Sx - (*Start_X);
								asChild[*num].Y = Sy - (*Start_Y);//��һ��Բ�����յ�
								asChild[*num].I = I_one - *Start_X;
								asChild[*num].J = J_one - *Start_Y;//Բ����Բ������
								
								*Start_X = Sx - X0;	
								*Start_Y = Sy - Y0;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
							}
							else
							{
								Sx = X0+r0x;
								Sy = Y0+r0y;
									
								asChild[*num]= *csChild;  
								
								asChild[*num].X = Sx - (*Start_X);
								asChild[*num].Y = Sy - (*Start_Y);//��һ��Բ�����յ�
								asChild[*num].I = I_one - *Start_X;
								asChild[*num].J = J_one - *Start_Y;//Բ����Բ������

								asChild[*num].row_id=2;

								*num=*num+1;
								asChild[*num]=*csChild;

								asChild[*num].X = r1x - r0x;
								asChild[*num].Y = r1y - r0y;//��һ��Բ�����յ�
								asChild[*num].I = -r0x;
								asChild[*num].J = -r0y;//Բ����Բ������

								if(csChild->G_pathmode_lower == 3) 
									asChild[*num].G_pathmode_lower = 2;
								if (csChild->G_pathmode_lower == 2) 
									asChild[*num].G_pathmode_lower = 3; 
								
								*Start_X = r1x;	
								*Start_Y = r1y;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
							}
						}

                    else if( ((zeroError >0)&&typeJudge < ANGLECONST)||(zeroError==0&&csChild->G_Dcompensate == 152 && csChild->G_pathmode ==2)||(zeroError==0&&csChild->G_Dcompensate == 151 && csChild->G_pathmode ==3) )//������
                        {    
                            S1x = X0 + r0x;  S1y = Y0 + r0y; //��һ���߶��յ㴦�İ뾶ʸ����������;
                            S2x = X0 + r1x;  S2y = Y0 + r1y;//�ڶ�������㴦�İ뾶ʸ����������;
                                        //S1,S2���������;
                           asChild[*num] = *csChild;     //��ֵ,����,Բ������
                   
                            asChild[*num].X = S1x - (*Start_X);
                            asChild[*num].Y = S1y - (*Start_Y);
                            asChild[*num].I = I_one-(*Start_X);
                            asChild[*num].J = J_one-(*Start_Y);
                            
                           asChild[*num].row_id=2;

							*num = *num+1;
                            asChild[*num] = *csChild;     //��ֵ,����,�쳤��ֱ��S1S3����;      
						   
							asChild[*num].X = S2x - S1x;
                            asChild[*num].Y = S2y - S1y;
                            asChild[*num].I = -r0x;
                            asChild[*num].J = -r0y; 
                           if(csChild->G_pathmode_lower ==3) 
								   asChild[*num].G_pathmode_lower = 2;
                           if (csChild->G_pathmode_lower ==2) 
								   asChild[*num].G_pathmode_lower = 3;

                            *Start_X = r1x; 
                            *Start_Y = r1y;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                        }//����         
 
				}//Բ����ֱ��

			
                else if((dataChild->G_pathmode_lower == 2 || dataChild->G_pathmode_lower == 3) && csChild->G_pathmode_lower ==1)//ֱ�߽�Բ��
                {
                        X0 = csChild->X;
                        Y0 = csChild->Y;

                        X1 = dataChild->X;
                        Y1 = dataChild->Y;
                        I_two = dataChild->I;
                        J_two = dataChild->J;

              //      if(csChild->G_coormode_9x == 91)        //G90�������Ҫ����......................;
                //    {
                        d0 = sqrt(X0*X0 +Y0*Y0);    
                        d1 = sqrt((Y1-J_two)*(Y1-J_two)+(X1-I_two)*(X1-I_two)); 
                        
                        if(csChild->G_Dcompensate == 151)
                        {
                            r0y = R*X0/d0;
                            r0x = (-R)*Y0/d0;                               
                        }       
                        else if(csChild->G_Dcompensate == 152)
                        {
                            r0y = (-R)*X0/d0;
                            r0x = R*Y0/d0;
                        }
						if(dataChild->G_Dcompensate == 151 && dataChild->G_pathmode_lower == 2||dataChild->G_Dcompensate == 152 && dataChild->G_pathmode_lower == 3)
								{
									r1y = R*(-J_two)/d1;
									r1x = R*(-I_two)/d1;
								}
						else if(dataChild->G_Dcompensate == 152 && dataChild->G_pathmode_lower == 2||dataChild->G_Dcompensate == 151 && dataChild->G_pathmode_lower == 3)
								{
									r1y = R*J_two/d1;
									r1x = R*I_two/d1;

								}
                        //ȷ��r0y,r0x,r1y,r1x����;
                        if(dataChild->G_pathmode_lower == 2)
                        {
                            KBy1 = I_two;
                            KBx1 =-J_two;
                        }
                        else if(dataChild->G_pathmode_lower == 3)
                        {
                            KBy1 =-I_two;
                            KBx1 = J_two;
                        }// ȷ��KB����,Բ����㴦������;
				//	}//G91

                   typeJudge = (r0y*r1y + r0x*r1x)/(sqrt(r0y*r0y + r0x*r0x)*sqrt(r1x*r1x + r1y*r1y));
                   zeroError = ((r1x-r0x)*X0 +(r1y-r0y)*Y0);
                //�жϽ�������:����,����,�쳤
  // if( (typeJudge >= ANGLECONST) && (((r1y-r0y)*Y0 + (r1x-r0x)*X0) >= 0) )//�쳤�� 
				   if(((typeJudge >= ANGLECONST) && (zeroError >= 0))||(zeroError < 0))
							 //||((zeroError== 0)&&dataChild->G_Dcompensate == 151 && dataChild->G_pathmode == 2)||((zeroError== 0)&&dataChild->G_Dcompensate == 152 && dataChild->G_pathmode == 3)) //������
		                 {
							S2x = r0x;    S2y = r0y;
                            S1x = X0+r0x; S1y = Y0+r0y;//��Բ���ཻ��ֱ�ߵ�������;

                            Ox = X0+I_two;    Oy = Y0+J_two;//Բ������;
                            r_arc = sqrt( (X0+r1x-Ox)*(X0+r1x-Ox) + (Y0+r1y-Oy)*(Y0+r1y-Oy) );//Բ���뾶

                            if(S1x == S2x)
								{
									Sx1=S1x ;	Sy1=Oy - sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));
									Sx2=S1x ;	Sy2=Oy + sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));
								}            
							else
                            {
								K_arc = (S2y-S1y)/(S2x-S1x);
								a_arc = 1+K_arc*K_arc;
								b_arc = 2*K_arc*(S1y-Oy-K_arc*S1x)-2*Ox;
								c_arc =( S1y-Oy-K_arc*S1x)*(S1y-Oy-K_arc*S1x)-r_arc*r_arc+Ox*Ox;
		                            
			                    Sx1=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sy1=S1y + K_arc*(Sx1-S1x);								
								Sx2=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sy2=S1y + K_arc*(Sx2-S1x);

                            }//�쳤�ߵĽ�������;
							if( (((-I_two)*X0+(-J_two)*Y0)>=0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)<=0) || (((-I_two)*X0+(-J_two)*Y0)<=0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)>0) )
							{
									Sx=Sx1;
									Sy=Sy1;
							 }	 
							 else if( (((-I_two)*X0+(-J_two)*Y0)>0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)>0) || (((-I_two)*X0+(-J_two)*Y0)<0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)<=0) )
							 {
									Sx=Sx2;
									Sy=Sy2;
							 }
						
                            asChild[*num]= *csChild;     //��ֵ,����

                            asChild[*num].X = Sx - (*Start_X);
                            asChild[*num].Y = Sy - (*Start_Y);

                            *Start_X = Sx - X0;  
                            *Start_Y = Sy - Y0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                        }//������
                        else if  ( zeroError==0 )// ������
						{
						   if((r0x==r1x)&&(r0y==r1y))
						   {
								Sx = X0+r0x;
								Sy = Y0+r0y;
								asChild[*num]= *csChild;  
								
								asChild[*num].X = Sx - (*Start_X);
								asChild[*num].Y = Sy - (*Start_Y);//��һ��Բ�����յ�
			
								*Start_X = Sx - X0;	
								*Start_Y = Sy - Y0;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
						   }
						   else
						   {	
									Sx = X0+r0x;
									Sy = Y0+r0y;
										
									asChild[*num]= *csChild;  
									
									asChild[*num].X = Sx - (*Start_X);
									asChild[*num].Y = Sy - (*Start_Y);//��һ��Բ�����յ�
                                    
									asChild[*num].row_id=2;

									*num=*num+1;
									asChild[*num]=*csChild;

									asChild[*num].X = r1x - r0x;
									asChild[*num].Y = r1y - r0y;//��һ��Բ�����յ�
									asChild[*num].I = -r0x;
									asChild[*num].J = -r0y;//Բ����Բ������

									if(csChild->G_pathmode_lower == 3) 
										asChild[*num].G_pathmode_lower = 2;
									if (csChild->G_pathmode_lower == 2) 
										asChild[*num].G_pathmode_lower = 3; 
									
									*Start_X = r1x;	
									*Start_Y = r1y;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
								}
						}
					else if(zeroError > 0 && typeJudge < ANGLECONST)//||(zeroError== 0&&dataChild->G_Dcompensate == 151 && dataChild->G_pathmode == 3)||(zeroError== 0&&dataChild->G_Dcompensate == 152 && dataChild->G_pathmode == 2))//������
                        {                           
							S1x = X0 + r0x;  S1y =Y0 + r0y; //��һ���߶��յ㴦�İ뾶ʸ����������;
                            S2x = X0 + r1x;  S2y =Y0 + r1y;//�ڶ�������㴦�İ뾶ʸ����������;
                                        //S1,S2���������;
							asChild[*num]= *csChild;     //��ֵ,����,��һ��ֱ��
	                        
							asChild[*num].X = S1x - (*Start_X);
							asChild[*num].Y = S1y - (*Start_Y);
	
							asChild[*num].row_id=2;

							*num=*num+1;
							asChild[*num] = *csChild;     //��ֵ,����,����Բ��;
	                        
							asChild[*num].X = S2x - S1x;
							asChild[*num].Y = S2y - S1y;
							asChild[*num].I = -r0x;
							asChild[*num].J = -r0y;
							if(dataChild->G_pathmode_lower == 3) 
								asChild[*num].G_pathmode_lower = 2;
							if (dataChild->G_pathmode_lower == 2) 
								asChild[*num].G_pathmode_lower = 3;

							*Start_X = r1x; 
							*Start_Y = r1y;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
						}//����         						
            }//ֱ�߽�Բ��
            else if((dataChild->G_pathmode_lower == 2 || dataChild->G_pathmode_lower == 3)&& (csChild->G_pathmode_lower == 2 || csChild->G_pathmode_lower == 3))//Բ����Բ��
                {
						X0 = csChild->X;
						Y0 = csChild->Y;
						I_one = csChild->I;
						J_one = csChild->J;

						X1 = dataChild->X;
						Y1 = dataChild->Y;
						I_two = dataChild->I;
						J_two = dataChild->J;

				//		if(csChild->G_coormode_9x == 91)        //G90�������Ҫ����......................;
				//		{
							d0 = sqrt((Y0-J_one)*(Y0-J_one)+(X0-I_one)*(X0-I_one)); 
							d1 = sqrt((Y1-J_two)*(Y1-J_two)+(X1-I_two)*(X1-I_two)); 
		                    
							if((csChild->G_Dcompensate == 151 && csChild->G_pathmode_lower == 2)||(csChild->G_Dcompensate == 152 && csChild->G_pathmode_lower == 3))
											{
												r0y = R*(Y0-J_one)/d0;
												r0x = R*(X0-I_one)/d0;
											}
							else if((csChild->G_Dcompensate == 152 && csChild->G_pathmode_lower == 2)||(csChild->G_Dcompensate == 151 && csChild->G_pathmode_lower == 3))
											{
												r0y = (-R)*(Y0-J_one)/d0;
												r0x = (-R)*(X0-I_one)/d0;
											}

							if((dataChild->G_Dcompensate == 151 && dataChild->G_pathmode_lower == 2)||(dataChild->G_Dcompensate == 152 && dataChild->G_pathmode_lower == 3))
											{
												r1y = R*(-J_two)/d1;
												r1x = R*(-I_two)/d1;
											}
							else if((dataChild->G_Dcompensate == 152 && dataChild->G_pathmode_lower == 2)||(dataChild->G_Dcompensate == 151 && dataChild->G_pathmode_lower == 3))
											{
												r1y = R*J_two/d1;
												r1x = R*I_two/d1;
											}
							//ȷ��r0y,r0x,r1y,r1x����;

							 if(csChild->G_pathmode_lower == 2)
							{
								KBy0 = -(X0-I_one);
								KBx0 = Y0-J_one;
							}
							else if(csChild->G_pathmode_lower == 3)
							{
								KBy0 = X0-I_one;
								KBx0 = -(Y0-J_one);
							}// ȷ��KB����;
							if(dataChild->G_pathmode_lower == 2)
							{
								KBy1 = I_two;
								KBx1 = -J_two;
							}
							else if(dataChild->G_pathmode_lower == 3)
							{
								KBy1 = -I_two;
								KBx1 = J_two;
							}// ȷ��KB����,Բ����㴦������;
				//		}//G91
		            
						   typeJudge = (r0y*r1y + r0x*r1x)/(sqrt(r0y*r0y + r0x*r0x)*sqrt(r1x*r1x + r1y*r1y));
						   zeroError = ((r1x-r0x)*X0 +(r1y-r0y)*KBy0);
            //�жϽ�������:����,����,�쳤
                    //if( ((r0y*r1y + r0x*r1x) >= ZERO) && (((r1y-r0y)*KBy0 + (r1x-r0x)*KBx0) >= 0) )//�쳤��
					if( ( (typeJudge >= ANGLECONST) && (zeroError >= 0) )|| (zeroError< 0)) //||((zeroError== 0)&&csChild->G_Dcompensate == 151 && csChild->G_pathmode == 2)||((zeroError== 0)&&csChild->G_Dcompensate == 152 && csChild->G_pathmode == 3))) //������
                    {
	                    S1x = X0; S1y = Y0;                   
                        Ox=I_one;   Oy=J_one;//Բ������;
                        r_arc=sqrt( (X0+r0x-I_one)*(X0+r0x-I_one) + (Y0+r0y-J_one)*(Y0+r0y-J_one) );//Բ���뾶
                        
                        if(X0+I_two-Ox==0)
                        {
							S2x = I_one;
							S2y = S1y;

							Sy1=S1y ;	Sx1=Ox + sqrt(r_arc*r_arc-(S2y-Oy)*(S2y-Oy));
							Sy2=S1y ;	Sx2=Ox - sqrt(r_arc*r_arc-(S2y-Oy)*(S2y-Oy));
                        }
                        else if(Y0+J_two-Oy==0)
                        {
							S2x = S1x; 
							S2y = J_one;
							
							Sx1=S1x ;	Sy1=Oy + sqrt(r_arc*r_arc-(S2x-Ox)*(S2x-Ox));
							Sx2=S1x ;	Sy2=Oy - sqrt(r_arc*r_arc-(S2x-Ox)*(S2x-Ox));						
						}
                        else
                        {
							Kline = (Y0+J_two - Oy)/(X0+I_two - Ox);//����Բ�ĵ�ֱ��б��;
							S2y=Kline*(X0-Ox+Oy/Kline+Kline*Y0)/(1+Kline*Kline);
							S2x=(S2y+Oy+Kline*Ox)/Kline;							
						}    
						if(S1x == S2x)
						{
							Sx1=S1x ;	Sy1=Oy + sqrt(r_arc*r_arc-(S2x-Ox)*(S2x-Ox));
							Sx2=S1x ;	Sy2=Oy - sqrt(r_arc*r_arc-(S2x-Ox)*(S2x-Ox));
						}

						else
						{
							K_arc = (S2y-S1y)/(S2x-S1x);
							a_arc = 1+K_arc*K_arc;
							b_arc = 2*K_arc*(S1y-Oy-K_arc*S1x) -2*Ox;
							c_arc = (S1y-Oy-K_arc*S1x)*(S1y-Oy-K_arc*S1x)-r_arc*r_arc+Ox*Ox;
							
							Sx1 = ( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sy1=S1y + K_arc*(Sx1-S1x);	                        
							Sx2 = ( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sy2=S1y + K_arc*(Sx2-S1x);

						}					
						if( (((Sx2-Sx1)*(X0-I_one)+(Sy2-Sy1)*(Y0-J_one))>0 && ((Sx2-Sx1)*KBx0+(Sy2-Sy1)*KBy0)<=0) || (((Sx2-Sx1)*(X0-I_one)+(Sy2-Sy1)*(Y0-J_one))<=0 && ((Sx2-Sx1)*KBx0+(Sy2-Sy1)*KBy0)>=0 ))
						{
								Sx=Sx1;
								Sy=Sy1;
						}
						else if( (((Sx2-Sx1)*(X0-I_one)+(Sy2-Sy1)*(Y0-J_one))>0 && ((Sx2-Sx1)*KBx0+(Sy2-Sy1)*KBy0)>0) || (((Sx2-Sx1)*(X0-I_one)+(Sy2-Sy1)*(Y0-J_one))<=0 && ((Sx2-Sx1)*KBx0+(Sy2-Sy1)*KBy0)<0 ))
						{
								Sx=Sx2;
								Sy=Sy2;
						}
                        asChild[*num]= *csChild;     //��ֵ,����

                        asChild[*num].X = Sx - (*Start_X);
                        asChild[*num].Y = Sy - (*Start_Y);
                        asChild[*num].I = I_one-(*Start_X);
                        asChild[*num].J = J_one-(*Start_Y);

						*num=*num+1; 
                        *Start_X = Sx - X0;  
                        *Start_Y = Sy - Y0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
					}//������
					 else if  ( zeroError==0 )// ������
					{
					   if((r0x==r1x)&&(r0y==r1y))
					   {
							Sx = X0+r0x;
							Sy = Y0+r0y;
								
							asChild[*num]= *csChild;  
							
							asChild[*num].X = Sx - (*Start_X);
							asChild[*num].Y = Sy - (*Start_Y);//��һ��Բ�����յ�
							asChild[*num].I = I_one - (*Start_X);
							asChild[*num].J = J_one - (*Start_Y);//Բ����Բ������
							
							*num=*num+1; 
							*Start_X = Sx - X0;	
							*Start_Y = Sy - Y0;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
					   }
					   else 
					   {
							Sx = X0+r0x;
							Sy = Y0+r0y;
								
							asChild[*num]= *csChild;  
							
							asChild[*num].X = Sx - (*Start_X);
							asChild[*num].Y = Sy - (*Start_Y);//��һ��Բ�����յ�

							asChild[*num].I = I_one - (*Start_X);
							asChild[*num].J = J_one - (*Start_Y);//Բ����Բ������

                            asChild[*num].row_id=2;

							*num=*num+1;
							asChild[*num]= *csChild;
							
							asChild[*num].X = r1x - r0x;
							asChild[*num].Y = r1y - r0y;//��һ��Բ�����յ�
							asChild[*num].I = -r0x;
							asChild[*num].J = -r0y;//Բ����Բ������
							
							if(csChild->G_pathmode_lower == 3) 
								asChild[*num].G_pathmode_lower = 2;
							if (csChild->G_pathmode_lower == 2) 
								asChild[*num].G_pathmode_lower = 3; 
							
							*num=*num+1; 
							*Start_X = Sx - X0;	
							*Start_Y = Sy - Y0;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;

					   }
					}
                    else if( (zeroError > 0) && (typeJudge < ANGLECONST) )//||((zeroError == 0)&&csChild->G_Dcompensate == 152 && csChild->G_pathmode == 2)||((zeroError== 0)&&csChild->G_Dcompensate == 151 && csChild->G_pathmode == 3))//������
                        {
                            S1x = X0 + r0x;  S1y = Y0 + r0y; //��һ���߶��յ㴦�İ뾶ʸ����������;
                            S2x = X0 + r1x;  S2y = Y0 + r1y;//�ڶ�������㴦�İ뾶ʸ����������;
                                        //S1,S2���������;
                            asChild[*num]= *csChild;     //��ֵ,����,Բ������
                    
                            asChild[*num].X = S1x - (*Start_X);
                            asChild[*num].Y = S1y - (*Start_Y);
                            asChild[*num].I = I_one-(*Start_X);
                            asChild[*num].J = J_one-(*Start_Y);
                            
							asChild[*num].row_id=2;
                             *num=*num+1;
                             asChild[*num] = *csChild;     //��ֵ,����,Բ������

							 asChild[*num].X = S2x - S1x;
							 asChild[*num].Y = S2y - S1y;

							 asChild[*num].I = -r0x;
							 asChild[*num].J = -r0y;
							 if(csChild->G_pathmode_lower == 3) 
								asChild[*num].G_pathmode_lower = 2;
							 if (csChild->G_pathmode_lower == 2) 
								asChild[*num].G_pathmode_lower = 3; 

                            *Start_X = r1x;  
                            *Start_Y = r1y;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
                        }//����
                }//Բ����Բ��
			}//���������� if(c != 0)
			if (dataChild->G_Dcompensate != 150)
			{				
				*csChild = *dataChild;
				*c = 1;			
			}
			else if( dataChild->G_Dcompensate==150)
			{
				if(dataChild->G_pathmode_lower==1)
				{
		           *num=*num+1; 
		           asChild[*num]= *dataChild;     //��ֵ,����
					
					//asChild[*num]= *csChild;     //��ֵ,����                             

                    asChild[*num].X = dataChild->X - (*Start_X);
                    asChild[*num].Y = dataChild->Y - (*Start_Y);
					
                    *c = 0;//Ϊ��һ�ε�������ʼ��.
                    *Start_X = 0;   
                    *Start_Y = 0;
				}
				else if(dataChild->G_pathmode_lower==2||dataChild->G_pathmode_lower==3)
				{
					MessageBox(hWnd,"ERROR:NOT ALLOW ESCAPE TOOL COMPASETE IN G02 OR G03 in g4142Fuction",NULL,NULL);
				}
			}


}	   



