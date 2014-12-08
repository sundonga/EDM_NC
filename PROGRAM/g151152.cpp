#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;	

int stringnum(char *p, const char* s); /*返回字符串p中含有字符串s的个数, s必须是一个字符*/

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
	
	if(*c != 0)//建立刀补后      
	{			   
                if(dataChild->G_pathmode_upper == 1 && csChild->G_pathmode_upper ==1)//直线接直线
                {
                        X0 = csChild->B;
                        Y0 = csChild->C;
                        X1 = dataChild->B;
                        Y1 = dataChild->C;
								
                 //   if(csChild->G_coormode_9x == 91)        //G90的情况需要补充......................;
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
                   typeJudge = (r0y*r1y + r0x*r1x)/(sqrt(r0y*r0y + r0x*r0x)*sqrt(r1x*r1x + r1y*r1y));//判断伸长型和插入型
                   zeroError = ((r1x-r0x)*X0 +(r1y-r0y)*Y0);//判断伸长（插入）和缩短型
                //判断交接类型:缩短,插入,伸长
                   if( (zeroError >= 0 && (typeJudge> ANGLECONST)) || zeroError <= 0)
				   {                   //伸长型和缩短型程序一样,可以合并为一段程序***          
                        if((Y1*X0) == (Y0*X1))
                        {
							S1x= r0x;    S1y=r0y; 
							S2x= X0+X1+r1x;    S2y=Y0+Y1+r1y;
                                                       
							Sx = X0+r0x;
							Sy = Y0+r0y;
                        }   
                        else
                        {
							S1x = r0x;   S1y = r0y; //第一段线段起点处的半径矢量顶点坐标;
							S2x = X0+X1+r1x; S2y = Y0+Y1+r1y; //第二段线终点处的半径矢量顶点坐标;
                        
							Sx = ((S2y-S1y)*X0*X1+Y0*X1*S1x-Y1*X0*S2x)/(Y0*X1-X0*Y1);
							Sy =-((S2x-S1x)*Y0*Y1+X0*Y1*S1y-X1*Y0*S2y)/(Y0*X1-X0*Y1);
                        }//伸长线的交点坐标;
                    
                        asChild[*num] = *csChild;     //赋值,修正

                        asChild[*num].B = Sx - (*Start_B);
                        asChild[*num].C = Sy - (*Start_C);
               
					   *Start_B = Sx - X0;  
                       *Start_C = Sy - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
				   }//伸长性刀补
                else if( (((r1x-r0x)*X0 +(r1y-r0y)*Y0) >= 0) && typeJudge < ANGLECONST )//插入型
				{
						S1x = X0 + r0x;  S1y =Y0 + r0y; //第一段线段终点处的半径矢量顶点坐标;
						S2x = X0 + r1x;  S2y =Y0 + r1y;//第二段线起点处的半径矢量顶点坐标;
									//S1,S2两点的坐标;
						asChild[*num]= *csChild;     //赋值,

						asChild[*num].B = S1x - (*Start_B);
						asChild[*num].C = S1y - (*Start_C);
						
						asChild[*num].row_id=2;

						 *num=*num+1;
						asChild[*num]= *csChild;     //插入段修正

						asChild[*num].B = S2x - S1x;
						asChild[*num].C = S2y - S1y;
						asChild[*num].I1 = -r0x;
						asChild[*num].J1 = -r0y;
						if (csChild->G_Dcompensate == 151)  asChild[*num].G_pathmode_upper = 2;		  
						if (csChild->G_Dcompensate == 152)  asChild[*num].G_pathmode_upper = 3;								    

						*Start_B = r1x; 
						*Start_C = r1y;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
				   }//插入
				}//直线接直线;
                else if(dataChild->G_pathmode_upper == 1 && (csChild->G_pathmode_upper ==3 || csChild->G_pathmode_upper ==2))//圆弧接直线
                {
//********************************************************************************
                        X0 = csChild->B;
                        Y0 = csChild->C;
			            I_one = csChild->I1;
                        J_one = csChild->J1;

                        X1 = dataChild->B;
                        Y1 = dataChild->C;

					//	if(csChild->G_coormode_9x == 91)        //G90的情况需要补充......................;
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
							 }//确定r0y,r0x,r1y,r1x参量;

							if(csChild->G_pathmode_upper == 2)
							 {
								KBy0 = -(X0-I_one);
								KBx0 = Y0-J_one;
							 }
							else if(csChild->G_pathmode_upper == 3)
							 {
								KBy0 = X0-I_one;
								KBx0 = -(Y0-J_one);
							 }// 确定KB参量;
					//	}//G91
					   typeJudge = (r0y*r1y + r0x*r1x)/(sqrt(r0y*r0y + r0x*r0x)*sqrt(r1x*r1x + r1y*r1y));
					   zeroError = ((r1x-r0x)*KBx0 +(r1y-r0y)*KBy0);  //判断交接类型:缩短,插入,伸长
              
                      if( (typeJudge >= ANGLECONST && (zeroError > 0))||(zeroError < 0))
						 //||(zeroError==0&&csChild->G_Dcompensate == 151 && csChild->G_pathmode_upper == 2)||(zeroError==0&&csChild->G_Dcompensate == 152 && csChild->G_pathmode_upper== 3))) //缩短型
					  {
                       //缩短性刀补if( typeJudge >= ANGLECONST) && (((r1y-r0y)*KBy0 + (r1x-r0x)*KBx0) >= 0) )//伸长型
                            S1x = X0+r1x; S1y = Y0+r1y;
                            S2x = X0+X1+r1x;  S2y = Y0+Y1+r1y;//与圆弧相交的直线的两个点;

                            Ox = I_one;   Oy = J_one;//圆心坐标;
                            r_arc = sqrt( (X0+r0x-I_one)*(X0+r0x-I_one) + (Y0+r0y-J_one)*(Y0+r0y-J_one) );//圆弧半径

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
                            }//伸长线的交点坐标;
                            
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
                            asChild[*num] = *csChild;     //赋值,修正
  
                            asChild[*num].B = Sx - (*Start_B);
                            asChild[*num].C = Sy - (*Start_C);
			                asChild[*num].I1 = I_one-(*Start_B);
                            asChild[*num].J1 = J_one-(*Start_C);

                            *Start_B = Sx - X0;  
                            *Start_C = Sy - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;                                                                                                                                                                                                                                                                                                                                                                                                                                                    
						}//缩短型
                        
					  else if  ( zeroError==0 )// 相切型
						{
                            if( (r0x==r1x)&&(r0y==r1y) )
							{
								Sx = X0+r0x;
								Sy = Y0+r0y;
									
								asChild[*num]= *csChild;  
								
								asChild[*num].B = Sx - (*Start_B);
								asChild[*num].C = Sy - (*Start_C);//第一段圆弧的终点
								asChild[*num].I1 = I_one - (*Start_B);
								asChild[*num].J1 = J_one - (*Start_C);//圆弧的圆心坐标
								
								(*Start_B) = Sx - X0;	
								(*Start_C) = Sy - Y0;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
							}
							else
							{
								Sx = X0+r0x;
								Sy = Y0+r0y;
									
								asChild[*num]= *csChild;  
								
								asChild[*num].B = Sx - (*Start_B);
								asChild[*num].C = Sy - (*Start_C);//第一段圆弧的终点
								asChild[*num].I1 = I_one - (*Start_B);
								asChild[*num].J1 = J_one - (*Start_C);//圆弧的圆心坐标

                                asChild[*num].row_id=2;
								
								*num=*num+1;
								asChild[*num]=*csChild;

								asChild[*num].B = r1x - r0x;
								asChild[*num].C = r1y - r0y;//第一段圆弧的终点
								asChild[*num].I1 = -r0x;
								asChild[*num].J1 = -r0y;//圆弧的圆心坐标

								if(csChild->G_pathmode_upper == 3) 
									asChild[*num].G_pathmode_upper = 2;
								if (csChild->G_pathmode_upper == 2) 
									asChild[*num].G_pathmode_upper = 3; 
								
								*Start_B = r1x;	
								*Start_C = r1y;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
							}
						}

                    else if( ((zeroError >0)&&typeJudge < ANGLECONST)||(zeroError==0&&csChild->G_Dcompensate == 152 && csChild->G_pathmode_upper ==2)||(zeroError==0&&csChild->G_Dcompensate == 151 && csChild->G_pathmode_upper ==3) )//插入型
                        {    
                            S1x = X0 + r0x;  S1y = Y0 + r0y; //第一段线段终点处的半径矢量顶点坐标;
                            S2x = X0 + r1x;  S2y = Y0 + r1y;//第二段线起点处的半径矢量顶点坐标;
                                        //S1,S2两点的坐标;

                            asChild[*num] = *csChild;     //赋值,修正,圆弧部分
                            
                            asChild[*num].B = S1x - (*Start_B);
                            asChild[*num].C = S1y - (*Start_C);
                            asChild[*num].I1 = I_one-(*Start_B);
                            asChild[*num].J1 = J_one-(*Start_C);
                            
							asChild[*num].row_id=2;
                           
							*num = *num+1;
                            asChild[*num] = *csChild;     //赋值,修正,伸长的直线S1S3部分;      

			                asChild[*num].B = S2x - S1x;
                            asChild[*num].C = S2y - S1y;
                            asChild[*num].I1 = -r0x;
                            asChild[*num].J1 = -r0y; 
                           
							if(csChild->G_pathmode_upper ==3)  asChild[*num].G_pathmode_upper = 2;				                
                            if (csChild->G_pathmode_upper ==2) asChild[*num].G_pathmode_upper = 3;
                           
							*Start_B = r1x; 
                            *Start_C = r1y;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
                        }//插入         
				}//圆弧接直线
                else if((dataChild->G_pathmode_upper == 2 || dataChild->G_pathmode_upper == 3) && csChild->G_pathmode_upper ==1)//直线接圆弧
                {
                        X0 = csChild->B;
                        Y0 = csChild->C;

                        X1 = dataChild->B;
                        Y1 = dataChild->C;
                        I_two = dataChild->I1;
                        J_two = dataChild->J1;

				//		if(csChild->G_coormode_9x == 91)        //G90的情况需要补充......................;
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
							} //确定r0y,r0x,r1y,r1x参量;
                       
                        if(dataChild->G_pathmode_upper == 2)
                        {
                            KBy1 = I_two;
                            KBx1 =-J_two;
                        }
                        else if(dataChild->G_pathmode_upper == 3)
                        {
                            KBy1 =-I_two;
                            KBx1 = J_two;
                        }// 确定KB参量,圆弧起点处的切线;
				//	}//G91
                   typeJudge = (r0y*r1y + r0x*r1x)/(sqrt(r0y*r0y + r0x*r0x)*sqrt(r1x*r1x + r1y*r1y));
                   zeroError = ((r1x-r0x)*X0 +(r1y-r0y)*Y0);
                //判断交接类型:缩短,插入,伸长
  // if( (typeJudge >= ANGLECONST) && (((r1y-r0y)*Y0 + (r1x-r0x)*X0) >= 0) )//伸长型 
				   if(((typeJudge >= ANGLECONST) && (zeroError >= 0))||(zeroError < 0))
							 //||((zeroError== 0)&&dataChild->G_compensate == 151 && dataChild->G_pathmode_upper == 2)||((zeroError== 0)&&dataChild->G_compensate == 152 && dataChild->G_pathmode_upper == 3)) //缩短型
				   {
							S2x = r0x;    S2y = r0y;
                            S1x = X0+r0x; S1y = Y0+r0y;//与圆弧相交的直线的两个点;

                            Ox = X0+I_two;    Oy = Y0+J_two;//圆心坐标;
                            r_arc = sqrt( (X0+r1x-Ox)*(X0+r1x-Ox) + (Y0+r1y-Oy)*(Y0+r1y-Oy) );//圆弧半径

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

                            }//伸长线的交点坐标;
		   
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
						
                            asChild[*num]= *csChild;     //赋值,修正

                            asChild[*num].B = Sx - (*Start_B);
                            asChild[*num].C = Sy - (*Start_C);

                            *Start_B = Sx - X0;  
                            *Start_C = Sy - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
				   }//缩短型
                   else if  ( zeroError==0 )// 相切型
				   {
					   if((r0x==r1x)&&(r0y==r1y))
					   {
							Sx = X0+r0x;
							Sy = Y0+r0y;
							asChild[*num]= *csChild;  
							
							asChild[*num].B = Sx - (*Start_B);
							asChild[*num].C = Sy - (*Start_C);//第一段圆弧的终点

							*Start_B = Sx - X0;	
							*Start_C = Sy - Y0;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
					   }
					   else
					   {	
							Sx = X0+r0x;
							Sy = Y0+r0y;
								
							asChild[*num]= *csChild;  
							
							asChild[*num].B = Sx - (*Start_B);
							asChild[*num].C = Sy - (*Start_C);//第一段圆弧的终点
						
							asChild[*num].row_id=2;
							*num=*num+1;
							asChild[*num]=*csChild;

							asChild[*num].B = r1x - r0x;
							asChild[*num].C = r1y - r0y;//第一段圆弧的终点

							asChild[*num].I1 = -r0x;
							asChild[*num].J1 = -r0y;//圆弧的圆心坐标

							if(dataChild->G_pathmode_upper == 3) 
								asChild[*num].G_pathmode_upper = 2;
							if (dataChild->G_pathmode_upper == 2) 
								asChild[*num].G_pathmode_upper = 3; 
							
							*Start_B = r1x;	
							*Start_C = r1y;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
					   }
				   }
				else if((zeroError > 0 && typeJudge < ANGLECONST)||(zeroError== 0&&dataChild->G_compensate == 151 && dataChild->G_pathmode_upper == 3)||(zeroError== 0&&dataChild->G_compensate == 152 && dataChild->G_pathmode_upper == 2))//插入型
				{                           
						S1x = X0 + r0x;  S1y =Y0 + r0y; //第一段线段终点处的半径矢量顶点坐标;
                        S2x = X0 + r1x;  S2y =Y0 + r1y;//第二段线起点处的半径矢量顶点坐标;
                                    //S1,S2两点的坐标;                    
						asChild[*num]= *csChild;     //赋值,修正,第一段直线

						asChild[*num].B = S1x - (*Start_B);
						asChild[*num].C = S1y - (*Start_C);

						asChild[*num].row_id=2;

						*num=*num+1;
						asChild[*num] = *csChild;     //赋值,修正,插入圆弧;                 

						asChild[*num].B = S2x - S1x;
						asChild[*num].C = S2y - S1y;
						asChild[*num].I1 = -r0x;
						asChild[*num].J1 = -r0y;
						if(dataChild->G_pathmode_upper == 3) asChild[*num].G_pathmode_upper = 2;
						if (dataChild->G_pathmode_upper == 2) asChild[*num].G_pathmode_upper = 3;

						*Start_B = r1x; 
						*Start_C = r1y;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
				   }//插入         				
            }//直线接圆弧
            else if((dataChild->G_pathmode_upper==2 || dataChild->G_pathmode_upper==3)&& (csChild->G_pathmode_upper==2 || csChild->G_pathmode_upper==3))//圆弧接圆弧
                {
						X0 = csChild->B;
						Y0 = csChild->C;
						I_one = csChild->I1;
						J_one = csChild->J1;

						X1 = dataChild->B;
						Y1 = dataChild->C;
						I_two = dataChild->I1;
						J_two = dataChild->J1;
                
            //    if(csChild->G_coormode_9x == 91)        //G90的情况需要补充......................;
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
									}//确定r0y,r0x,r1y,r1x参量;
                    
                     if(csChild->G_pathmode_upper == 2)
                    {
                        KBy0 = -(X0-I_one);
                        KBx0 = Y0-J_one;
                    }
                    else if(csChild->G_pathmode_upper == 3)
                    {
                        KBy0 = X0-I_one;
                        KBx0 = -(Y0-J_one);
                    }// 确定KB参量;
                    if(dataChild->G_pathmode_upper == 2)
                    {
                        KBy1 = I_two;
                        KBx1 = -J_two;
                    }
                    else if(dataChild->G_pathmode_upper == 3)
                    {
                        KBy1 = -I_two;
                        KBx1 = J_two;
                    }// 确定KB参量,圆弧起点处的切线;
              //  }//G91           
                  typeJudge = (r0y*r1y + r0x*r1x)/(sqrt(r0y*r0y + r0x*r0x)*sqrt(r1x*r1x + r1y*r1y));
                   zeroError = ((r1x-r0x)*KBx0 +(r1y-r0y)*KBy0);

            //判断交接类型:缩短,插入,伸长
                    //if( ((r0y*r1y + r0x*r1x) >= ZERO) && (((r1y-r0y)*KBy0 + (r1x-r0x)*KBx0) >= 0) )//伸长型
					if( ( (typeJudge >= ANGLECONST) && (zeroError > 0)) || (zeroError< 0)) //||((zeroError== 0)&&csChild->G_compensate == 151 && csChild->G_pathmode_upper == 2)||((zeroError== 0)&&csChild->G_compensate == 152 && csChild->G_pathmode_upper == 3))) //缩短型
                    { //缩短性刀补
	                    S1x = X0; S1y = Y0;
                    
                        Ox=I_one;   Oy=J_one;//圆心坐标;
                        r_arc=sqrt( (X0+r0x-I_one)*(X0+r0x-I_one) + (Y0+r0y-J_one)*(Y0+r0y-J_one) );//圆弧半径
                        
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
							Kline = (Y0+J_two - Oy)/(X0+I_two - Ox);//过两圆心的直线斜率;

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
                            asChild[*num]= *csChild;     //赋值,修正

                            asChild[*num].B = Sx - (*Start_B);
                            asChild[*num].C = Sy - (*Start_C);
                            asChild[*num].I1 = I_one-(*Start_B);
                            asChild[*num].J1 = J_one-(*Start_C);

                            *Start_B = Sx - X0;  
                            *Start_C = Sy - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;

                        }//缩短型
						 else if  ( zeroError==0 )// 相切型
						{
                           if((r0x==r1x)&&(r0y==r1y))
						   {
								Sx = X0+r0x;
								Sy = Y0+r0y;
									
								asChild[*num]= *csChild;  
								
								asChild[*num].B = Sx - (*Start_B);
								asChild[*num].C = Sy - (*Start_C);//第一段圆弧的终点
								asChild[*num].I1 = I_one - (*Start_B);
								asChild[*num].J1 = J_one - (*Start_C);//圆弧的圆心坐标
								
								*Start_B = Sx - X0;	
								*Start_C = Sy - Y0;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
						   }
						   else 
						   {
								Sx = X0+r0x;
								Sy = Y0+r0y;
									
								asChild[*num]= *csChild;  
								
								asChild[*num].B = Sx - (*Start_B);
								asChild[*num].C = Sy - (*Start_C);//第一段圆弧的终点
								asChild[*num].I1 = I_one - (*Start_B);
								asChild[*num].J1 = J_one - (*Start_C);//圆弧的圆心坐标
						        asChild[*num].row_id=2;
								*num=*num+1;
								asChild[*num]= *csChild;
								
								asChild[*num].B = r1x - r0x;
								asChild[*num].C = r1y - r0y;//第一段圆弧的终点
								asChild[*num].I1 = -r0x;
								asChild[*num].J1 = -r0y;//圆弧的圆心坐标
								
								if(csChild->G_pathmode_lower == 3) 
									asChild[*num].G_pathmode_lower = 2;
								if (csChild->G_pathmode_lower == 2) 
									asChild[*num].G_pathmode_lower = 3; 
								
								*Start_B = Sx - X0;	
								*Start_C = Sy - Y0;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;

						   }
						}//相切型
                    else if(( (zeroError > 0) && (typeJudge < ANGLECONST) )||((zeroError == 0)&&csChild->G_compensate == 152 && csChild->G_pathmode_upper == 2)||((zeroError== 0)&&csChild->G_compensate == 151 && csChild->G_pathmode_upper == 3))//插入型
                        {
						  //插入性刀补
                            S1x = X0 + r0x;  S1y = Y0 + r0y; //第一段线段终点处的半径矢量顶点坐标;
                            S2x = X0 + r1x;  S2y = Y0 + r1y;//第二段线起点处的半径矢量顶点坐标;
                                        //S1,S2两点的坐标;

                            asChild[*num]= *csChild;     //赋值,修正,圆弧部分
                       
                            asChild[*num].B = S1x - (*Start_B);
                            asChild[*num].C = S1y - (*Start_C);
                            asChild[*num].I1 = I_one-(*Start_B);
                            asChild[*num].J1 = J_one-(*Start_C);
                      
							asChild[*num].row_id=2;
                             
							*num=*num+1;
                            asChild[*num] = *csChild;     //赋值,修正,圆弧部分
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
                            *Start_C = r1y;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
                        }//插入
                     }

			}//建立刀补后 if(c != 0)
			if (dataChild->G_Dcompensate != 150)
			{	
				*c = 1;
			}
			else if(dataChild->G_Dcompensate = 150)
			{
				if( dataChild->G_pathmode_upper==1 )
				{                              
		            *num=*num+1;
		            asChild[*num]= *dataChild;     //赋值,修正                            

                    asChild[*num].B = dataChild->X - (*Start_B);
                    asChild[*num].C = dataChild->Y - (*Start_C);
					
                    *c = 0;//为下一次刀补作初始化.
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

          if(*c != 0)//建立刀补后      
            {   
                if(dataChild->G_pathmode_lower == 1 && csChild->G_pathmode_lower ==1)//直线接直线
                {
                        X0 = csChild->X;
                        Y0 = csChild->Y;
                        X1 = dataChild->X;
                        Y1 = dataChild->Y;							
                
                 //   if(csChild->G_coormode_9x == 91)        //G90的情况需要补充......................;
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

                   typeJudge = (r0y*r1y + r0x*r1x)/(sqrt(r0y*r0y + r0x*r0x)*sqrt(r1x*r1x + r1y*r1y));//判断伸长型和插入型
                   zeroError = ((r1x-r0x)*X0 +(r1y-r0y)*Y0);//判断伸长（插入）和缩短型					
					    //MessageBox(hWnd,_fcvt( r0y, 7, &decimal, &tsign ),NULL,NULL); 				
                //判断交接类型:缩短,插入,伸长
                        if( (zeroError >= 0 && (typeJudge> ANGLECONST)) || zeroError <= 0)
                        {                   //伸长型和缩短型程序一样,可以合并为一段程序***                    							
                            if((Y1*X0) == (Y0*X1))
                            {
								S1x= r0x;    S1y=r0y; 
								S2x= X0+X1+r1x;    S2y=Y0+Y1+r1y;
	                                                       
								Sx = X0+r0x;
								Sy = Y0+r0y;
                            }   
                            else
                            {
								S1x = r0x;   S1y = r0y; //第一段线段起点处的半径矢量顶点坐标;
								S2x = X0+X1+r1x; S2y = Y0+Y1+r1y; //第二段线终点处的半径矢量顶点坐标;
	                        
								Sx = ((S2y-S1y)*X0*X1+Y0*X1*S1x-Y1*X0*S2x)/(Y0*X1-X0*Y1);
								Sy =-((S2x-S1x)*Y0*Y1+X0*Y1*S1y-X1*Y0*S2y)/(Y0*X1-X0*Y1);
                            }//伸长线的交点坐标;                        
                                
							asChild[*num].X = Sx - (*Start_X);
                            asChild[*num].Y = Sy - (*Start_Y);

                            *Start_X = Sx - X0;  
                            *Start_Y = Sy - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
						}//伸长性刀补         
                    else if( (((r1x-r0x)*X0 +(r1y-r0y)*Y0) >= 0) && typeJudge < ANGLECONST )//插入型
                        {
                            S1x = X0 + r0x;  S1y =Y0 + r0y; //第一段线段终点处的半径矢量顶点坐标;
                            S2x = X0 + r1x;  S2y =Y0 + r1y;//第二段线起点处的半径矢量顶点坐标;
                                        //S1,S2两点的坐标;
                            asChild[*num]= *csChild;     //赋值,
                            asChild[*num].X = S1x - (*Start_X);
                            asChild[*num].Y = S1y - (*Start_Y);

							asChild[*num].row_id=2;
                           
							*num=*num+1;                   
                            asChild[*num]= *csChild; //插入段修正
							
                            asChild[*num].X = S2x - S1x;
                            asChild[*num].Y = S2y - S1y;
                            asChild[*num].I = -r0x;
                            asChild[*num].J = -r0y;
							if (csChild->G_compensate == 41) 
								   asChild[*num].G_pathmode_lower = 2;
							if (csChild->G_compensate == 42) 
								    asChild[*num].G_pathmode_lower = 3;
  
                            *Start_X = r1x; 
                            *Start_Y = r1y;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
						}//插入						 					            
				}//直线接直线;
                else if(dataChild->G_pathmode_lower == 1 && (csChild->G_pathmode_lower ==3 || csChild->G_pathmode_lower ==2))//圆弧接直线
                {
//********************************************************************************
                        X0 = csChild->X;
                        Y0 = csChild->Y;
						I_one = csChild->I;
                        J_one = csChild->J;

                        X1 = dataChild->X;
                        Y1 = dataChild->Y;

              //      if(csChild->G_coormode_9x == 91)        //G90的情况需要补充......................;
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
                         }//确定r0y,r0x,r1y,r1x参量;

                        if(csChild->G_pathmode_lower == 2)
                         {
                            KBy0 = -(X0-I_one);
                            KBx0 = Y0-J_one;
                         }
                        else if(csChild->G_pathmode_lower == 3)
                         {
                            KBy0 = X0-I_one;
                            KBx0 = -(Y0-J_one);
                         }// 确定KB参量;
				//	}//G91
                   typeJudge = (r0y*r1y + r0x*r1x)/(sqrt(r0y*r0y + r0x*r0x)*sqrt(r1x*r1x + r1y*r1y));
                   zeroError = ((r1x-r0x)*KBx0 +(r1y-r0y)*KBy0);
                //判断交接类型:缩短,插入,伸长

                      if( (typeJudge >= ANGLECONST && (zeroError > 0)) ||(zeroError < 0))
						 // ||(zeroError==0&&csChild->G_Dcompensate == 151 && csChild->G_pathmode_lower == 2)||(zeroError==0&&csChild->G_Dcompensate == 152 && csChild->G_pathmode_lower== 3))) //缩短型
                     {
                       //缩短性刀补if( typeJudge >= ANGLECONST) && (((r1y-r0y)*KBy0 + (r1x-r0x)*KBx0) >= 0) )//伸长型
                            S1x = X0+r1x; S1y = Y0+r1y;
                            S2x = X0+X1+r1x;  S2y = Y0+Y1+r1y;//与圆弧相交的直线的两个点;

                            Ox = I_one;   Oy = J_one;//圆心坐标;
                            r_arc = sqrt( (X0+r0x-I_one)*(X0+r0x-I_one) + (Y0+r0y-J_one)*(Y0+r0y-J_one) );//圆弧半径

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
                            }//伸长线的交点坐标;
                            
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
                            asChild[*num] = *csChild;     //赋值,修正

                            asChild[*num].X = Sx - (*Start_X);
                            asChild[*num].Y = Sy - (*Start_Y);
							asChild[*num].I = I_one-(*Start_X);
                            asChild[*num].J = J_one-(*Start_Y);

                            *Start_X = Sx - X0;  
                            *Start_Y = Sy - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
						}//缩短型
                		else if  ( zeroError==0 )// 相切型
						{
                            if( (r0x==r1x)&&(r0y==r1y) )
							{
								Sx = X0+r0x;
								Sy = Y0+r0y;
									
								asChild[*num]= *csChild;  
								
								asChild[*num].X = Sx - (*Start_X);
								asChild[*num].Y = Sy - (*Start_Y);//第一段圆弧的终点
								asChild[*num].I = I_one - *Start_X;
								asChild[*num].J = J_one - *Start_Y;//圆弧的圆心坐标
								
								*Start_X = Sx - X0;	
								*Start_Y = Sy - Y0;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
							}
							else
							{
								Sx = X0+r0x;
								Sy = Y0+r0y;
									
								asChild[*num]= *csChild;  
								
								asChild[*num].X = Sx - (*Start_X);
								asChild[*num].Y = Sy - (*Start_Y);//第一段圆弧的终点
								asChild[*num].I = I_one - *Start_X;
								asChild[*num].J = J_one - *Start_Y;//圆弧的圆心坐标

								asChild[*num].row_id=2;

								*num=*num+1;
								asChild[*num]=*csChild;

								asChild[*num].X = r1x - r0x;
								asChild[*num].Y = r1y - r0y;//第一段圆弧的终点
								asChild[*num].I = -r0x;
								asChild[*num].J = -r0y;//圆弧的圆心坐标

								if(csChild->G_pathmode_lower == 3) 
									asChild[*num].G_pathmode_lower = 2;
								if (csChild->G_pathmode_lower == 2) 
									asChild[*num].G_pathmode_lower = 3; 
								
								*Start_X = r1x;	
								*Start_Y = r1y;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
							}
						}

                    else if( ((zeroError >0)&&typeJudge < ANGLECONST)||(zeroError==0&&csChild->G_Dcompensate == 152 && csChild->G_pathmode ==2)||(zeroError==0&&csChild->G_Dcompensate == 151 && csChild->G_pathmode ==3) )//插入型
                        {    
                            S1x = X0 + r0x;  S1y = Y0 + r0y; //第一段线段终点处的半径矢量顶点坐标;
                            S2x = X0 + r1x;  S2y = Y0 + r1y;//第二段线起点处的半径矢量顶点坐标;
                                        //S1,S2两点的坐标;
                           asChild[*num] = *csChild;     //赋值,修正,圆弧部分
                   
                            asChild[*num].X = S1x - (*Start_X);
                            asChild[*num].Y = S1y - (*Start_Y);
                            asChild[*num].I = I_one-(*Start_X);
                            asChild[*num].J = J_one-(*Start_Y);
                            
                           asChild[*num].row_id=2;

							*num = *num+1;
                            asChild[*num] = *csChild;     //赋值,修正,伸长的直线S1S3部分;      
						   
							asChild[*num].X = S2x - S1x;
                            asChild[*num].Y = S2y - S1y;
                            asChild[*num].I = -r0x;
                            asChild[*num].J = -r0y; 
                           if(csChild->G_pathmode_lower ==3) 
								   asChild[*num].G_pathmode_lower = 2;
                           if (csChild->G_pathmode_lower ==2) 
								   asChild[*num].G_pathmode_lower = 3;

                            *Start_X = r1x; 
                            *Start_Y = r1y;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
                        }//插入         
 
				}//圆弧接直线

			
                else if((dataChild->G_pathmode_lower == 2 || dataChild->G_pathmode_lower == 3) && csChild->G_pathmode_lower ==1)//直线接圆弧
                {
                        X0 = csChild->X;
                        Y0 = csChild->Y;

                        X1 = dataChild->X;
                        Y1 = dataChild->Y;
                        I_two = dataChild->I;
                        J_two = dataChild->J;

              //      if(csChild->G_coormode_9x == 91)        //G90的情况需要补充......................;
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
                        //确定r0y,r0x,r1y,r1x参量;
                        if(dataChild->G_pathmode_lower == 2)
                        {
                            KBy1 = I_two;
                            KBx1 =-J_two;
                        }
                        else if(dataChild->G_pathmode_lower == 3)
                        {
                            KBy1 =-I_two;
                            KBx1 = J_two;
                        }// 确定KB参量,圆弧起点处的切线;
				//	}//G91

                   typeJudge = (r0y*r1y + r0x*r1x)/(sqrt(r0y*r0y + r0x*r0x)*sqrt(r1x*r1x + r1y*r1y));
                   zeroError = ((r1x-r0x)*X0 +(r1y-r0y)*Y0);
                //判断交接类型:缩短,插入,伸长
  // if( (typeJudge >= ANGLECONST) && (((r1y-r0y)*Y0 + (r1x-r0x)*X0) >= 0) )//伸长型 
				   if(((typeJudge >= ANGLECONST) && (zeroError >= 0))||(zeroError < 0))
							 //||((zeroError== 0)&&dataChild->G_Dcompensate == 151 && dataChild->G_pathmode == 2)||((zeroError== 0)&&dataChild->G_Dcompensate == 152 && dataChild->G_pathmode == 3)) //缩短型
		                 {
							S2x = r0x;    S2y = r0y;
                            S1x = X0+r0x; S1y = Y0+r0y;//与圆弧相交的直线的两个点;

                            Ox = X0+I_two;    Oy = Y0+J_two;//圆心坐标;
                            r_arc = sqrt( (X0+r1x-Ox)*(X0+r1x-Ox) + (Y0+r1y-Oy)*(Y0+r1y-Oy) );//圆弧半径

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

                            }//伸长线的交点坐标;
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
						
                            asChild[*num]= *csChild;     //赋值,修正

                            asChild[*num].X = Sx - (*Start_X);
                            asChild[*num].Y = Sy - (*Start_Y);

                            *Start_X = Sx - X0;  
                            *Start_Y = Sy - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
                        }//缩短型
                        else if  ( zeroError==0 )// 相切型
						{
						   if((r0x==r1x)&&(r0y==r1y))
						   {
								Sx = X0+r0x;
								Sy = Y0+r0y;
								asChild[*num]= *csChild;  
								
								asChild[*num].X = Sx - (*Start_X);
								asChild[*num].Y = Sy - (*Start_Y);//第一段圆弧的终点
			
								*Start_X = Sx - X0;	
								*Start_Y = Sy - Y0;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
						   }
						   else
						   {	
									Sx = X0+r0x;
									Sy = Y0+r0y;
										
									asChild[*num]= *csChild;  
									
									asChild[*num].X = Sx - (*Start_X);
									asChild[*num].Y = Sy - (*Start_Y);//第一段圆弧的终点
                                    
									asChild[*num].row_id=2;

									*num=*num+1;
									asChild[*num]=*csChild;

									asChild[*num].X = r1x - r0x;
									asChild[*num].Y = r1y - r0y;//第一段圆弧的终点
									asChild[*num].I = -r0x;
									asChild[*num].J = -r0y;//圆弧的圆心坐标

									if(csChild->G_pathmode_lower == 3) 
										asChild[*num].G_pathmode_lower = 2;
									if (csChild->G_pathmode_lower == 2) 
										asChild[*num].G_pathmode_lower = 3; 
									
									*Start_X = r1x;	
									*Start_Y = r1y;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
								}
						}
					else if(zeroError > 0 && typeJudge < ANGLECONST)//||(zeroError== 0&&dataChild->G_Dcompensate == 151 && dataChild->G_pathmode == 3)||(zeroError== 0&&dataChild->G_Dcompensate == 152 && dataChild->G_pathmode == 2))//插入型
                        {                           
							S1x = X0 + r0x;  S1y =Y0 + r0y; //第一段线段终点处的半径矢量顶点坐标;
                            S2x = X0 + r1x;  S2y =Y0 + r1y;//第二段线起点处的半径矢量顶点坐标;
                                        //S1,S2两点的坐标;
							asChild[*num]= *csChild;     //赋值,修正,第一段直线
	                        
							asChild[*num].X = S1x - (*Start_X);
							asChild[*num].Y = S1y - (*Start_Y);
	
							asChild[*num].row_id=2;

							*num=*num+1;
							asChild[*num] = *csChild;     //赋值,修正,插入圆弧;
	                        
							asChild[*num].X = S2x - S1x;
							asChild[*num].Y = S2y - S1y;
							asChild[*num].I = -r0x;
							asChild[*num].J = -r0y;
							if(dataChild->G_pathmode_lower == 3) 
								asChild[*num].G_pathmode_lower = 2;
							if (dataChild->G_pathmode_lower == 2) 
								asChild[*num].G_pathmode_lower = 3;

							*Start_X = r1x; 
							*Start_Y = r1y;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
						}//插入         						
            }//直线接圆弧
            else if((dataChild->G_pathmode_lower == 2 || dataChild->G_pathmode_lower == 3)&& (csChild->G_pathmode_lower == 2 || csChild->G_pathmode_lower == 3))//圆弧接圆弧
                {
						X0 = csChild->X;
						Y0 = csChild->Y;
						I_one = csChild->I;
						J_one = csChild->J;

						X1 = dataChild->X;
						Y1 = dataChild->Y;
						I_two = dataChild->I;
						J_two = dataChild->J;

				//		if(csChild->G_coormode_9x == 91)        //G90的情况需要补充......................;
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
							//确定r0y,r0x,r1y,r1x参量;

							 if(csChild->G_pathmode_lower == 2)
							{
								KBy0 = -(X0-I_one);
								KBx0 = Y0-J_one;
							}
							else if(csChild->G_pathmode_lower == 3)
							{
								KBy0 = X0-I_one;
								KBx0 = -(Y0-J_one);
							}// 确定KB参量;
							if(dataChild->G_pathmode_lower == 2)
							{
								KBy1 = I_two;
								KBx1 = -J_two;
							}
							else if(dataChild->G_pathmode_lower == 3)
							{
								KBy1 = -I_two;
								KBx1 = J_two;
							}// 确定KB参量,圆弧起点处的切线;
				//		}//G91
		            
						   typeJudge = (r0y*r1y + r0x*r1x)/(sqrt(r0y*r0y + r0x*r0x)*sqrt(r1x*r1x + r1y*r1y));
						   zeroError = ((r1x-r0x)*X0 +(r1y-r0y)*KBy0);
            //判断交接类型:缩短,插入,伸长
                    //if( ((r0y*r1y + r0x*r1x) >= ZERO) && (((r1y-r0y)*KBy0 + (r1x-r0x)*KBx0) >= 0) )//伸长型
					if( ( (typeJudge >= ANGLECONST) && (zeroError >= 0) )|| (zeroError< 0)) //||((zeroError== 0)&&csChild->G_Dcompensate == 151 && csChild->G_pathmode == 2)||((zeroError== 0)&&csChild->G_Dcompensate == 152 && csChild->G_pathmode == 3))) //缩短型
                    {
	                    S1x = X0; S1y = Y0;                   
                        Ox=I_one;   Oy=J_one;//圆心坐标;
                        r_arc=sqrt( (X0+r0x-I_one)*(X0+r0x-I_one) + (Y0+r0y-J_one)*(Y0+r0y-J_one) );//圆弧半径
                        
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
							Kline = (Y0+J_two - Oy)/(X0+I_two - Ox);//过两圆心的直线斜率;
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
                        asChild[*num]= *csChild;     //赋值,修正

                        asChild[*num].X = Sx - (*Start_X);
                        asChild[*num].Y = Sy - (*Start_Y);
                        asChild[*num].I = I_one-(*Start_X);
                        asChild[*num].J = J_one-(*Start_Y);

						*num=*num+1; 
                        *Start_X = Sx - X0;  
                        *Start_Y = Sy - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
					}//缩短型
					 else if  ( zeroError==0 )// 相切型
					{
					   if((r0x==r1x)&&(r0y==r1y))
					   {
							Sx = X0+r0x;
							Sy = Y0+r0y;
								
							asChild[*num]= *csChild;  
							
							asChild[*num].X = Sx - (*Start_X);
							asChild[*num].Y = Sy - (*Start_Y);//第一段圆弧的终点
							asChild[*num].I = I_one - (*Start_X);
							asChild[*num].J = J_one - (*Start_Y);//圆弧的圆心坐标
							
							*num=*num+1; 
							*Start_X = Sx - X0;	
							*Start_Y = Sy - Y0;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;
					   }
					   else 
					   {
							Sx = X0+r0x;
							Sy = Y0+r0y;
								
							asChild[*num]= *csChild;  
							
							asChild[*num].X = Sx - (*Start_X);
							asChild[*num].Y = Sy - (*Start_Y);//第一段圆弧的终点

							asChild[*num].I = I_one - (*Start_X);
							asChild[*num].J = J_one - (*Start_Y);//圆弧的圆心坐标

                            asChild[*num].row_id=2;

							*num=*num+1;
							asChild[*num]= *csChild;
							
							asChild[*num].X = r1x - r0x;
							asChild[*num].Y = r1y - r0y;//第一段圆弧的终点
							asChild[*num].I = -r0x;
							asChild[*num].J = -r0y;//圆弧的圆心坐标
							
							if(csChild->G_pathmode_lower == 3) 
								asChild[*num].G_pathmode_lower = 2;
							if (csChild->G_pathmode_lower == 2) 
								asChild[*num].G_pathmode_lower = 3; 
							
							*num=*num+1; 
							*Start_X = Sx - X0;	
							*Start_Y = Sy - Y0;//转接点相对于第二段G代码起点的相对坐标,为求下一个拐点的相对坐标做准备;

					   }
					}
                    else if( (zeroError > 0) && (typeJudge < ANGLECONST) )//||((zeroError == 0)&&csChild->G_Dcompensate == 152 && csChild->G_pathmode == 2)||((zeroError== 0)&&csChild->G_Dcompensate == 151 && csChild->G_pathmode == 3))//插入型
                        {
                            S1x = X0 + r0x;  S1y = Y0 + r0y; //第一段线段终点处的半径矢量顶点坐标;
                            S2x = X0 + r1x;  S2y = Y0 + r1y;//第二段线起点处的半径矢量顶点坐标;
                                        //S1,S2两点的坐标;
                            asChild[*num]= *csChild;     //赋值,修正,圆弧部分
                    
                            asChild[*num].X = S1x - (*Start_X);
                            asChild[*num].Y = S1y - (*Start_Y);
                            asChild[*num].I = I_one-(*Start_X);
                            asChild[*num].J = J_one-(*Start_Y);
                            
							asChild[*num].row_id=2;
                             *num=*num+1;
                             asChild[*num] = *csChild;     //赋值,修正,圆弧部分

							 asChild[*num].X = S2x - S1x;
							 asChild[*num].Y = S2y - S1y;

							 asChild[*num].I = -r0x;
							 asChild[*num].J = -r0y;
							 if(csChild->G_pathmode_lower == 3) 
								asChild[*num].G_pathmode_lower = 2;
							 if (csChild->G_pathmode_lower == 2) 
								asChild[*num].G_pathmode_lower = 3; 

                            *Start_X = r1x;  
                            *Start_Y = r1y;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
                        }//插入
                }//圆弧接圆弧
			}//建立刀补后 if(c != 0)
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
		           asChild[*num]= *dataChild;     //赋值,修正
					
					//asChild[*num]= *csChild;     //赋值,修正                             

                    asChild[*num].X = dataChild->X - (*Start_X);
                    asChild[*num].Y = dataChild->Y - (*Start_Y);
					
                    *c = 0;//为下一次刀补作初始化.
                    *Start_X = 0;   
                    *Start_Y = 0;
				}
				else if(dataChild->G_pathmode_lower==2||dataChild->G_pathmode_lower==3)
				{
					MessageBox(hWnd,"ERROR:NOT ALLOW ESCAPE TOOL COMPASETE IN G02 OR G03 in g4142Fuction",NULL,NULL);
				}
			}


}	   



