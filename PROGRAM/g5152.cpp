#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;	

extern TapeParam tapepm;  //锥度加工参数
extern double ZERO;
extern double ZERO_ERROR=0.00001;
extern double ZERO_NEG=-0.00001;
//2010-5-26锥加工用参数
extern double z_position;
extern double xy_plane;
extern double h_thickness;
extern double HLOWDISTANCE;
//end 2010-5-26

extern void g5152Fuction(HWND hWnd,int *sign, nc_data *dataChild,nc_data asChild[],int *num,double *Start_X, double *Start_Y,int *c,int *first5152flag,nc_data *csChild,double *Start_B,double  *Start_C);
extern double calculateLineArc(double PointX,double PointY);//计算直线与X轴正方向逆时针夹角
extern int Arcdirection(double Arc1,double Arc2,double Arc3);

void g5152Fuction(HWND hWnd,int *sign, nc_data *dataChild,nc_data asChild[],int *num,double *Start_X, double *Start_Y,int *c,int *first5152flag, nc_data *csChild,double *Start_B,double  *Start_C)
{ 
	double Ru0,Ru1,Rl0,Rl1;
    double Y0,X0,Y1,X1,d0,d1,ru0y,ru0x,ru1y,ru1x,rl0x,rl0y,rl1y,rl1x;
    double Su1x,Su1y,Su2x,Su2y,Sl1x,Sl1y,Sl2y,Sl2x,Sux,Suy,Sux1,Suy1,Sux2,Suy2,Slx,Sly,Sly1,Slx1,Sly2,Slx2;
    double J_one,I_one,J_two,I_two,KBy0,KBx0,KBy1,KBx1,Oy,Ox,K_arc,a_arc,b_arc,c_arc;
    double Kline,r_arc;
       int tape_class;
    double typeJudge_up,typeJudge_low;
	double zeroErrorUp,zeroErrorLow;
	double J_one1,I_one1,J_two1,I_two1;
    double r0y1,r0x1,r1y1,r1x1,d11,d01;//恒锥度时重新计算的偏移矢量
	double Arc1,Arc2,Arc3;
	double tempx,tempy;//计算插入小圆弧的方向时用到的
	double Sx,Sy,K,d,r;

	Ru0 =(h_thickness- xy_plane)*tan(csChild->T*3.14/1000/180);    //上平面第一段线段的投影半径
	Ru1 =(h_thickness- xy_plane)*tan(dataChild->T*3.14/1000/180);  //上平面第二段线段的投影半径
    Rl0 = xy_plane*tan(csChild->T*3.14/1000/180);    //下平面第一段线段的投影半径
	Rl1 = xy_plane*tan(dataChild->T*3.14/1000/180);  //下平面第二段线段的投影半径
	
	tape_class=1;

	 if(*c != 0)//建立刀补后      
	 {			
       if(dataChild->G_pathmode == 1 && csChild->G_pathmode ==1)//直线接直线
//********************************************************************************                
	   {
              X0 = csChild->X;
              Y0 = csChild->Y;
              X1 = dataChild->X;
              Y1 = dataChild->Y;
               

					d0 = sqrt(X0*X0 +Y0*Y0);    
					d1 = sqrt(X1*X1 +Y1*Y1);  
					
					if(csChild->G_tapemode == 51)
					{
						ru0y = Ru0*X0/d0;             //上平面偏移矢量
						ru0x = (-Ru0)*Y0/d0;
						
						ru1y = Ru1*X1/d1;
						ru1x = (-Ru1)*Y1/d1; 

						rl0y = (-Rl0)*X0/d0;           //下平面偏移矢量
						rl0x = Rl0*Y0/d0;
						
						rl1y = (-Rl1)*X1/d1;
						rl1x = Rl1*Y1/d1; 
					}
					else if(csChild->G_tapemode == 52)
					{
						ru0y = (-Ru0)*X0/d0;               //上平面偏移矢量
						ru0x = Ru0*Y0/d0;

						ru1y = (-Ru1)*X1/d1;
						ru1x = Ru1*Y1/d1;

						rl0y = Rl0*X0/d0;                  //下平面偏移矢量
						rl0x = (-Rl0)*Y0/d0;

						rl1y = Rl1*X1/d1;
						rl1x = (-Rl1)*Y1/d1;
					}
	
				  if((sqrt(ru0y*ru0y + ru0x*ru0x)*sqrt(ru1x*ru1x + ru1y*ru1y))==0) 
				  {   
					  typeJudge_up=0;
				  }
				  else 
				  {
					  typeJudge_up=(ru0y*ru1y + ru0x*ru1x)/(sqrt(ru0y*ru0y + ru0x*ru0x)*sqrt(ru1x*ru1x + ru1y*ru1y));
				  }
				  if(sqrt(rl0y*rl0y + rl0x*rl0x)*sqrt(rl1x*rl1x + rl1y*rl1y)==0)
				  {
					  typeJudge_low=0;
				  }
				  else
				  {
					  typeJudge_low=(rl0y*rl1y + rl0x*rl1x)/(sqrt(rl0y*rl0y + rl0x*rl0x)*sqrt(rl1x*rl1x + rl1y*rl1y));
				  }

				 zeroErrorUp = (ru1y-ru0y)*Y0 + (ru1x-ru0x)*X0;
				 zeroErrorLow = (rl1y-rl0y)*Y0 + (rl1x-rl0x)*X0;
				if(*first5152flag == 1)
				{       
					    ru0y = 0;               //上平面偏移矢量
                        ru0x = 0;

						rl0y = 0;               //下平面偏移矢量
                        rl0x = 0;
				}
              //判断交接类型:缩短,插入,伸长
              //下平面伸长型，上平面缩短型
                if( zeroErrorLow >= ZERO && (typeJudge_low< ANGLECONST) && zeroErrorUp <= 0)//上平面缩短，下平面插入型
				{
					 if((Y1*X0) == (Y0*X1))
                        {
							Su1x= ru0x;    Su1y=ru0y;              //上平面拐点计算
							Su2x= X0+X1+ru1x;    Su2y=Y0+Y1+ru1y;
							
							Sux = X0+ru0x;
							Suy = Y0+ru0y;
                        }   
                        else
                        {
							Su1x= ru0x;   Su1y=ru0y;//上平面第一段线段起点处的半径矢量顶点坐标;  //上平面拐点计算
							Su2x= X0+X1+ru1x; Su2y=Y0+Y1+ru1y;//上平面第二段线终点处的半径矢量顶点坐标;
                        
							Sux = ((Su2y-Su1y)*X0*X1+Y0*X1*Su1x-Y1*X0*Su2x)/(Y0*X1-X0*Y1);
							Suy =-((Su2x-Su1x)*Y0*Y1+X0*Y1*Su1y-X1*Y0*Su2y)/(Y0*X1-X0*Y1);
                       
						}//伸长线的交点坐标;

						Sl1x= X0 + rl0x;  Sl1y=Y0 + rl0y; //第一段线段终点处的半径矢量顶点坐标;
                        Sl2x= X0 + rl1x;  Sl2y=Y0 + rl1y;//第二段线起点处的半径矢量顶点坐标;

						if (tape_class == 0)
						{
							asChild[*num] = *csChild;     //赋值,修正

							asChild[*num].B = Sux - (*Start_B);  //上平面缩短
							asChild[*num].C = Suy - (*Start_C);
							asChild[*num].X = Sl1x - (*Start_X);  //下平面插入
							asChild[*num].Y = Sl1y - (*Start_Y);
							asChild[*num].row_id=2;
									 
							*num=*num+1;
							asChild[*num] = *csChild;                //上平面一点下平面过渡圆

							 if(*first5152flag == 1)
							 {       
								asChild[*num].B = 0 ;  //上平面
								asChild[*num].C = 0;
								asChild[*num].X = 0;  //下平面
								asChild[*num].Y = 0;

								*first5152flag = 0;
								}
							 else
							 {
								if(X0>0)
									{ 
									  tempx=X0+100;
									  tempy=Y0/X0*tempx;
									}
									else if(X0<0)
									{
									  tempx=X0-100;
									  tempy=Y0/X0*tempx;
									}
									else 
									{
										if(Y0>0)
										{
											tempx=0;
											tempy=Y0+100;
										}
										if(Y0<0)
										{
											tempx=0;
											tempy=Y0-100;
										}
									}
								Arc1=calculateLineArc(rl0x,rl0y);
								Arc2=calculateLineArc(tempx-X0,tempy-Y0);
								Arc3=calculateLineArc(rl1x,rl1y);
									
								asChild[*num].G_pathmode = Arcdirection(Arc1,Arc2,Arc3);
								asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
								asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;	
								
								asChild[*num].X = rl1x - rl0x ;  //下平面
								asChild[*num].Y = rl1y - rl0y;
								asChild[*num].I = -rl0x ;  
								asChild[*num].J = -rl0y ;

								asChild[*num].B = 0;  //上平面
								asChild[*num].C = 0;
								asChild[*num].I1 = 0;  
								asChild[*num].J1 = 0;				
							 }	
								*num=*num+1;										
								*Start_B =  Sux - X0 ;  //上平面
								*Start_C =  Suy - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
								*Start_X = rl1x;  //下平面
								*Start_Y = rl1y;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
						}

						if (tape_class == 1)
						{
						asChild[*num] = *csChild;     //赋值,修正

						asChild[*num].B = Sux - (*Start_B);  //上平面缩短
						asChild[*num].C = Suy - (*Start_C);
						asChild[*num].X = Sux - ru0x + rl0x - (*Start_X);  //下平面插入
						asChild[*num].Y = Suy - ru0y + rl0y - (*Start_Y);
						asChild[*num].row_id=2;

						*num=*num+1;
						asChild[*num] = *csChild;                //上平面一点下平面过渡圆

						if(*first5152flag == 1)
						{      
							asChild[*num].B = 0 ;  //上平面
							asChild[*num].C = 0;
							asChild[*num].X = 0;  //下平面
							asChild[*num].Y = 0;

							*first5152flag = 0;
						}
						else
						{
						 if(X0>0)
						 { 
						  tempx=X0+100;
						  tempy=Y0/X0*tempx;
						 }
						else if(X0<0)
						 {
						  tempx=X0-100;
						  tempy=Y0/X0*tempx;
						 }
					   else 
					   {
						if(Y0>0)
						{
							tempx=0;
							tempy=Y0+100;
						}
						if(Y0<0)
						{
							tempx=0;
							tempy=Y0-100;
						}
					  }
					Arc1=calculateLineArc(rl0x - ru0x,rl0y - ru0y);
					Arc2=calculateLineArc(tempx-(Sux),tempy-(Suy));
					Arc3=calculateLineArc(rl1x - ru1x,rl1y - ru1y);
						
					asChild[*num].G_pathmode = Arcdirection(Arc1,Arc2,Arc3);
					asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
					asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;

					asChild[*num].X = rl1x - rl0x - (ru1x - ru0x) ;  //下平面
					asChild[*num].Y = rl1y - rl0y - (ru1y - ru0y);
					asChild[*num].I = ru0x - rl0x ;  
					asChild[*num].J = ru0y - rl0y ;

					asChild[*num].B = 0;  //上平面
					asChild[*num].C = 0;
					asChild[*num].I1 = 0 ;  
					asChild[*num].J1 = 0 ;
						}
						*num=*num+1;
								
						*Start_B =  Sux - X0 ;  //上平面
						*Start_C =  Suy - Y0 ;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
						*Start_X = *Start_B + rl1x - ru1x;  //下平面
						*Start_Y = *Start_C + rl1y - ru1y;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;	
					}
						*csChild = *dataChild;
						 
			   }///上平面缩短，下平面插入型
                   
			 else if( zeroErrorUp >= 0 && (typeJudge_up< ANGLECONST)  &&  zeroErrorLow <= 0 )//下平面缩短，上平面插入型
				{
					Su1x= X0 + ru0x;  Su1y=Y0 + ru0y; //第一段线段终点处的半径矢量顶点坐标;
                    Su2x= X0 + ru1x;  Su2y=Y0 + ru1y;//第二段线起点处的半径矢量顶点坐标; //S1,S2两点的坐标;
                               
					if((Y1*X0) == (Y0*X1))
					{
						Sl1x= rl0x;    Sl1y=rl0y;              //下平面拐点计算
						Sl2x= X0+X1+rl1x;    Sl2y=Y0+Y1+rl1y;
						Slx = X0+rl0x;
						Sly = Y0+rl0y;
                    }   
                    else
                    {   Sl1x= rl0x;   Sl1y=rl0y;//下平面第一段线段起点处的半径矢量顶点坐标;   //下平面拐点计算
						Sl2x= X0+X1+rl1x; Sl2y=Y0+Y1+rl1y;//下平面第二段线终点处的半径矢量顶点坐标;
                    
						Slx = ((Sl2y-Sl1y)*X0*X1+Y0*X1*Sl1x-Y1*X0*Sl2x)/(Y0*X1-X0*Y1);
						Sly =-((Sl2x-Sl1x)*Y0*Y1+X0*Y1*Sl1y-X1*Y0*Sl2y)/(Y0*X1-X0*Y1);						
					}

				 if (tape_class == 0)
				 {
					asChild[*num] = *csChild;     //赋值,修正
                          
					asChild[*num].B = Su1x - (*Start_B);  //上平面插入
					asChild[*num].C = Su1y - (*Start_C);
					asChild[*num].X = Slx - (*Start_X);  //下平面缩短
					asChild[*num].Y = Sly - (*Start_Y);
					asChild[*num].row_id=2;
							 
					*num=*num+1;
					asChild[*num] = *csChild;                //下平面一点上平面过渡圆
					 
					if(*first5152flag == 1)
					{       
						asChild[*num].B = 0 ;  //上平面
						asChild[*num].C = 0;
						asChild[*num].X = 0;  //下平面
						asChild[*num].Y = 0;

						*first5152flag = 0;
					}
					else
					{							
					  if(X0>0)
					  { 
					   tempx=X0+100;
					   tempy=Y0/X0*tempx;
					  }
					  else if(X0<0)
					  {
					   tempx=X0-100;
					   tempy=Y0/X0*tempx;
					  }
					 else 
					 {
					   if(Y0>0)
					   {
						tempx=0;
						tempy=Y0+100;
					   }
					  if(Y0<0)
					  {
						tempx=0;
						tempy=Y0-100;
					  }
					 }

					Arc1=calculateLineArc(ru0x,ru0y);
					Arc2=calculateLineArc(tempx-X0,tempy-Y0);
					Arc3=calculateLineArc(ru1x,ru1y);
						
					asChild[*num].G_pathmode = Arcdirection(Arc1,Arc2,Arc3);
					asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
					asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;	 
							
					asChild[*num].B = ru1x - ru0x ;  //上平面
					asChild[*num].C = ru1y - ru0y;
					asChild[*num].I1 = -ru0x ;  //上平面
					asChild[*num].J1 = -ru0y ;
					
					asChild[*num].X = 0;  //下平面
					asChild[*num].Y = 0;
					asChild[*num].I = 0;  //下平面
					asChild[*num].J = 0;
						}
								
					 *num=*num+1;
							
					*Start_B = ru1x;  //上平面
					*Start_C = ru1y;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
					*Start_X = Slx - X0 ;  //下平面
					*Start_Y = Sly - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
				}

				if (tape_class == 1)
				{
					asChild[*num] = *csChild;     //赋值,修正
                          
					asChild[*num].B = Slx - rl0x + ru0x - (*Start_B);  //上平面插入
					asChild[*num].C = Sly - rl0y + ru0y - (*Start_C);
					asChild[*num].X = Slx - (*Start_X);  //下平面缩短
					asChild[*num].Y = Sly - (*Start_Y);
					asChild[*num].row_id=2;
							 
					*num=*num+1;
                    asChild[*num] = *csChild;                //下平面一点上平面过渡圆
					 
					if(*first5152flag == 1)
					{       
						asChild[*num].B = 0 ;  //上平面
						asChild[*num].C = 0;
						asChild[*num].X = 0;  //下平面
						asChild[*num].Y = 0;

						 *first5152flag = 0;
					}
					else
					{
					 if(X0>0)
						{ 
						  tempx=X0+100;
						  tempy=Y0/X0*tempx;
						}
					 else if(X0<0)
						{
						  tempx=X0-100;
						  tempy=Y0/X0*tempx;
						}
					 else 
						{
							if(Y0>0)
							{
								tempx=0;
								tempy=Y0+100;
							}
							if(Y0<0)
							{
								tempx=0;
								tempy=Y0-100;
							}
						}

						Arc1=calculateLineArc(ru0x - rl0x,ru0y - rl0y);
						Arc2=calculateLineArc(tempx-(Slx),tempy-(Sly));
						Arc3=calculateLineArc(ru1x - rl1x,ru1y - rl1y);
							
						asChild[*num].G_pathmode = Arcdirection(Arc1,Arc2,Arc3);
						asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
						asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;
								
						asChild[*num].B = ru1x - ru0x - (rl1x - rl0x) ;  //上平面
						asChild[*num].C = ru1y - ru0y - (rl1y - rl0y);
						asChild[*num].I1 = rl0x - ru0x ;  //上平面
						asChild[*num].J1 = rl0y - ru0y ;
						
						asChild[*num].X = 0;  //下平面
						asChild[*num].Y = 0;
						asChild[*num].I = 0;  //下平面
						asChild[*num].J = 0;
					}
									
					 *num=*num+1;
							
					*Start_X = Slx - X0 ;  //下平面
					*Start_Y = Sly - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;							
					*Start_B = *Start_X - rl1x + ru1x;  //上平面
					*Start_C = *Start_Y - rl1y + ru1y;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
				}
					*csChild = *dataChild;
				}
			 else 
			 {    //伸长缩短                 
			    if((Y1*X0) == (Y0*X1))
                {
					Su1x = ru0x;    Su1y = ru0y;              //上平面拐点计算
					Su2x = X0+X1+ru1x;    Su2y = Y0+Y1+ru1y;
					Sux = X0+ru0x;
					Suy = Y0+ru0y;

					Sl1x = rl0x;    Sl1y = rl0y;              //下平面拐点计算
					Sl2x = X0+X1+rl1x;    Sl2y = Y0+Y1+rl1y;
					Slx = X0+rl0x;
					Sly = Y0+rl0y;
                }   
                else
                {
					Su1x= ru0x;   Su1y = ru0y;//上平面第一段线段起点处的半径矢量顶点坐标;  //上平面拐点计算
					Su2x= X0+X1+ru1x; Su2y = Y0+Y1+ru1y;//上平面第二段线终点处的半径矢量顶点坐标;
				    Sux = ((Su2y-Su1y)*X0*X1+Y0*X1*Su1x-Y1*X0*Su2x)/(Y0*X1-X0*Y1);
					Suy = -((Su2x-Su1x)*Y0*Y1+X0*Y1*Su1y-X1*Y0*Su2y)/(Y0*X1-X0*Y1);

					Sl1x = rl0x;   Sl1y = rl0y;//下平面第一段线段起点处的半径矢量顶点坐标;   //下平面拐点计算
					Sl2x = X0+X1+rl1x; Sl2y = Y0+Y1+rl1y;//下平面第二段线终点处的半径矢量顶点坐标;
					Slx = ((Sl2y-Sl1y)*X0*X1+Y0*X1*Sl1x-Y1*X0*Sl2x)/(Y0*X1-X0*Y1);
					Sly =-((Sl2x-Sl1x)*Y0*Y1+X0*Y1*Sl1y-X1*Y0*Sl2y)/(Y0*X1-X0*Y1);
					}//伸长线的交点坐标;

				if(tape_class == 0)
				{                                           //尖锥度
					asChild[*num] = *csChild;     //赋值,修正
                  
					asChild[*num].B = Sux - (*Start_B);  //上平面伸长
					asChild[*num].C = Suy - (*Start_C);
					asChild[*num].X = Slx - (*Start_X);  //下平面缩短
					asChild[*num].Y = Sly - (*Start_Y);
					
				    *num=*num+1;
					asChild[*num] = *csChild; 
					*Start_B = Sux - X0;  //上平面
					*Start_C = Suy - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
					*Start_X = Slx - X0;  //下平面
					*Start_Y = Sly - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;

                    *first5152flag = 0;

					*csChild = *dataChild;
					}

				else if (tape_class == 1)
				{      
				  if (zeroErrorLow >= 0 && (typeJudge_low> ANGLECONST) && zeroErrorUp <= 0)//下伸上缩
				  {                                      //恒锥度
					asChild[*num] = *csChild;     //赋值,修正

					if(*first5152flag == 1)
					{
						asChild[*num].B = Sux - (*Start_B);  //上平面
						asChild[*num].C = Suy - (*Start_C); 
						asChild[*num].X = Slx - (*Start_X);  //下平面
						asChild[*num].Y = Sly - (*Start_Y);
							 
						*num=*num+1;

						*Start_B = Sux - X0;  //上平面
						*Start_C = Suy - Y0;
						*Start_X = Slx - X0;  //下平面
						*Start_Y = Sly - Y0;

						*first5152flag = 0;
						*csChild = *dataChild;
					}
					else
					{
						asChild[*num].B = Sux - (*Start_B);  //上平面
						asChild[*num].C = Suy - (*Start_C); 
						asChild[*num].X = Sux - ru0x + rl0x - (*Start_X);  //下平面
						asChild[*num].Y = Suy - ru0y + rl0y - (*Start_Y);
						asChild[*num].row_id=2;	 
						*num=*num+1;

						asChild[*num] = *csChild; 

						asChild[*num].B = 0;  //上平面缩短
						asChild[*num].C = 0; 
						asChild[*num].I1 = 0 ;  
						asChild[*num].J1 = 0 ;

						asChild[*num].X = rl1x - rl0x - (ru1x - ru0x) ;  //下平面伸长
						asChild[*num].Y = rl1y - rl0y - (ru1y - ru0y) ;
						asChild[*num].I = ru0x - rl0x ;  
						asChild[*num].J = ru0y - rl0y ;

						if(X0>0)
						{ 
						  tempx=X0+100;
						  tempy=Y0/X0*tempx;
						}
					   else if(X0<0)
						{
						  tempx=X0-100;
						  tempy=Y0/X0*tempx;
						}
						else 
						{
							if(Y0>0)
							{
								tempx=0;
								tempy=Y0+100;
							}
							if(Y0<0)
							{
								tempx=0;
								tempy=Y0-100;
							}
						 }
						Arc1=calculateLineArc(rl0x - ru0x,rl0y - ru0y);
						Arc2=calculateLineArc(Slx-Sux,Sly-Suy);
						Arc3=calculateLineArc(rl1x - ru1x,rl1y - ru1y);
							
						asChild[*num].G_pathmode = Arcdirection(Arc1,Arc2,Arc3);
						asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
						asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;
				  		*num=*num+1;

						*Start_B = Sux - X0;  //上平面
						*Start_C = Suy - Y0;
						*Start_X = *Start_B + rl1x - ru1x;  //下平面
						*Start_Y = *Start_C + rl1y - ru1y;

						*csChild = *dataChild;
					}
				  }

                  if (zeroErrorUp >= 0 && (typeJudge_up> ANGLECONST) && zeroErrorLow <= 0)//上伸下缩
				  {
                    asChild[*num] = *csChild;     //赋值,修正

					if(*first5152flag == 1)
					{
						asChild[*num].B = Sux - (*Start_B);  //上平面
						asChild[*num].C = Suy - (*Start_C); 
						asChild[*num].X = Slx - (*Start_X);  //下平面
						asChild[*num].Y = Sly - (*Start_Y);
							 
						*num=*num+1;

						*Start_B = Sux - X0;  //上平面
						*Start_C = Suy - Y0;
						*Start_X = Slx - X0;  //下平面
						*Start_Y = Sly - Y0;

						*first5152flag = 0;
						*csChild = *dataChild;
					}
					else 
					{
						asChild[*num].B = Slx - rl0x + ru0x - (*Start_B);  //上平面伸长
						asChild[*num].C = Sly - rl0y + ru0y - (*Start_C); 
						asChild[*num].X =  Slx - (*Start_X);  //下平面缩短
						asChild[*num].Y =  Sly - (*Start_Y);
						asChild[*num].row_id=2;		 
						
						*num=*num+1;
                        asChild[*num] = *csChild; 

						asChild[*num].B = ru1x - ru0x - (rl1x - rl0x) ;  //上平面
						asChild[*num].C = ru1y - ru0y - (rl1y - rl0y);
						asChild[*num].I1 = rl0x - ru0x ;  //上平面
						asChild[*num].J1 = rl0y - ru0y ;

						asChild[*num].X = 0;  //下平面
						asChild[*num].Y = 0;
						asChild[*num].I = 0;  //下平面
						asChild[*num].J = 0;

						if(X1>0)
						{ 
						  tempx=X0+100;
						  tempy=Y0/X0*tempx;
						}
					   else if(X1<0)
						{
						  tempx=X0-100;
						  tempy=Y0/X0*tempx;
						}
					   else 
						{
							if(Y1>0)
							{
								tempx=0;
								tempy=Y0+100;
							}
							if(Y1<0)
							{
								tempx=0;
								tempy=Y0-100;
							}
						}
						Arc1=calculateLineArc(ru0x - rl0x,ru0y - rl0y);
						Arc2=calculateLineArc(Sux-Slx,Suy-Sly);
						Arc3=calculateLineArc(ru1x - rl1x,ru1y - rl1y);

						asChild[*num].G_pathmode = Arcdirection(Arc1,Arc2,Arc3);
						asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
						asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;

						*num=*num+1;
						*Start_X = Slx - X0 ;  //下平面
						*Start_Y = Sly - Y0;								
						*Start_B = *Start_X - rl1x + ru1x;  //上平面
						*Start_C = *Start_Y - rl1y + ru1y;
							 				
						*csChild = *dataChild;
					}
				  }
			}	
		}//

                    if(dataChild->G_tapemode == 50)       //取消刀补;
                        {	
                            asChild[*num]= *dataChild;     //赋值,修正
					
			                asChild[*num].B = X1 - (*Start_B);
                            asChild[*num].C = Y1 - (*Start_C);
			                asChild[*num].X = X1 - (*Start_X);
                            asChild[*num].Y = Y1 - (*Start_Y);
							
							*num=*num+1;
                            *sign = 0;//跳出补偿
                            *c = 0;//为下一次刀补作初始化.
                            *Start_X = 0.;   
                            *Start_Y = 0.;
                            *Start_B = 0.;   
                            *Start_C = 0.;

					}//取消刀补;            
			}//直线接直线;
                else if(dataChild->G_pathmode == 1 && (csChild->G_pathmode ==3 || csChild->G_pathmode ==2))//圆弧接直线
				{
//******************************************************************************** 
                        X0 = csChild->X;
                        Y0 = csChild->Y;
						I_one=csChild->I;
                        J_one=csChild->J;

                        X1 = dataChild->X;
                        Y1 = dataChild->Y;
                   

                        d0 = sqrt((Y0-J_one)*(Y0-J_one)+(X0-I_one)*(X0-I_one)); 
                        d1 = sqrt(X1*X1 +Y1*Y1);    
                        
						if((csChild->G_tapemode == 51 && csChild->G_pathmode == 2)||(csChild->G_tapemode == 52 && csChild->G_pathmode == 3))
						{
								ru0y = Ru0*(Y0-J_one)/d0;
								ru0x = Ru0*(X0-I_one)/d0;

								rl0y = (-Rl0)*(Y0-J_one)/d0;
								rl0x = (-Rl0)*(X0-I_one)/d0;
						} 
						else if((csChild->G_tapemode == 52 && csChild->G_pathmode == 2)||(csChild->G_tapemode == 51 && csChild->G_pathmode == 3))
						{
								 ru0y = (-Ru0)*(Y0-J_one)/d0;
								 ru0x = (-Ru0)*(X0-I_one)/d0;

								 rl0y = Rl0*(Y0-J_one)/d0;
								 rl0x = Rl0*(X0-I_one)/d0;
						}

                        if(csChild->G_tapemode == 51)
                        {
                            ru1y = Ru1*X1/d1;
                            ru1x = (-Ru1)*Y1/d1;   

							rl1y = (-Rl1)*X1/d1;
                            rl1x =  Rl1*Y1/d1;  
                        }       
                        else if(csChild->G_tapemode == 52)
                        {
                            ru1y = (-Ru1)*X1/d1;
                            ru1x = Ru1*Y1/d1;

							rl1y = Rl1*X1/d1;
                            rl1x = (-Rl1)*Y1/d1;
                        }//确定r0y,r0x,r1y,r1x参量;

                        if(csChild->G_pathmode == 2)
                        {
                            KBy0= -(X0-I_one);
                            KBx0=Y0-J_one;
                        }
                        else if(csChild->G_pathmode == 3)
                        {
                            KBy0=X0-I_one;
                            KBx0= -(Y0-J_one);
                        }// 确定KB参量;

                //判断交接类型:缩短,插入,伸长

					  if((sqrt(ru0y*ru0y + ru0x*ru0x)*sqrt(ru1x*ru1x + ru1y*ru1y))==0) 
					  {   
						  typeJudge_up=0;
					  }
					  else 
					  {
						  typeJudge_up=(ru0y*ru1y+ru0x*ru1x)/(sqrt(ru0y*ru0y+ru0x*ru0x)*sqrt(ru1x*ru1x+ru1y*ru1y));
					  }
					  if(sqrt(rl0y*rl0y+rl0x*rl0x)*sqrt(rl1x*rl1x+rl1y*rl1y)==0)
					  {
						  typeJudge_low=0;
					  }
					  else
					  {
						  typeJudge_low=(rl0y*rl1y+rl0x*rl1x)/(sqrt(rl0y*rl0y+rl0x*rl0x)*sqrt(rl1x*rl1x+rl1y*rl1y));
					  }

					 zeroErrorUp = (ru1y-ru0y)*KBy0+(ru1x-ru0x)*KBx0;
					 zeroErrorLow = (rl1y-rl0y)*KBy0+(rl1x-rl0x)*KBx0;
					 
					 if(zeroErrorLow>ZERO&&typeJudge_low> ANGLECONST&&(abs(ru1x*ru0y-ru0x*ru1y)>ZERO))//判断直线与圆弧会不会不相交
					 {
					   Sx=X0+X1+rl1x;  Sy=Y0+Y1+rl1y; 
					   r=sqrt((X0+rl0x-I_one)*(X0+rl0x-I_one) + (Y0+rl0y-J_one)*(Y0+rl0y-J_one));
					   if(X1==0)
					   {
						   d=abs(I_one-X0-rl0x);
					   }
					   else
					   {
                         K=Y1/X1;
						 d=abs(K*(I_one-Sx)-(J_one-Sy))/sqrt(1+K*K);
					   }
					   if(d-r>ZERO)
					   {
                           typeJudge_low=-1;
					   }
					 }
					 else if(zeroErrorUp>ZERO&&typeJudge_up> ANGLECONST&&(abs(ru1x*ru0y-ru0x*ru1y)>ZERO))
					 {                                                
					   Sx=X0+X1+ru1x;  Sy=Y0+Y1+ru1y; 
					   r=sqrt((X0+ru0x-I_one)*(X0+ru0x-I_one) + (Y0+ru0y-J_one)*(Y0+ru0y-J_one));
					   if(X1==0)
					   {
						   d=abs(I_one-X0-ru0x);
					   }
					   else
					   {
                         K=Y1/X1;
						 d=abs(K*(I_one-Sx)-(J_one-Sy))/sqrt(1+K*K);
					   }
					   if(d-r>ZERO)
					   {
                           typeJudge_up=-1;
					   }
					 }

			if(  (zeroErrorLow>ZERO&&typeJudge_low<ANGLECONST&&zeroErrorUp<ZERO_NEG)
				||(abs(zeroErrorLow)<=ZERO&&abs(typeJudge_low+1)<ZERO&&((csChild->G_tapemode == 51 && csChild->G_pathmode ==2)||(csChild->G_tapemode == 52 && csChild->G_pathmode ==3))))
//上表面缩短下表面插入型
			{  
					   Su1x=X0+ru1x; Su1y=Y0+ru1y;                                                //上平面缩短型
					   Su2x=X0+X1+ru1x;  Su2y=Y0+Y1+ru1y;//与圆弧相交的直线的两个点;

						Ox=I_one;   Oy=J_one;//圆心坐标;
						r_arc=sqrt( (X0+ru0x-I_one)*(X0+ru0x-I_one) + (Y0+ru0y-J_one)*(Y0+ru0y-J_one) );//圆弧半径

						if(abs(X1) <= ZERO)//csChild->D+
						{
						   Sux1=Su1x ;   Suy1=Oy - sqrt(r_arc*r_arc-(Su1x-Ox)*(Su1x-Ox));
						   Sux2=Su1x ;   Suy2=Oy + sqrt(r_arc*r_arc-(Su1x-Ox)*(Su1x-Ox));
						}		                           
						else
						{
							  K_arc=Y1/X1;
							  a_arc=1+K_arc*K_arc;
							  b_arc=2*K_arc*(Su2y-Oy-K_arc*Su2x)-2*Ox;
							  c_arc=(Su2y-Oy-K_arc*Su2x)*(Su2y-Oy-K_arc*Su2x)-r_arc*r_arc+Ox*Ox;
				                            
							  Sux2=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Suy2=Su1y + K_arc*(Sux2-Su1x);
							  Sux1=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Suy1=Su1y + K_arc*(Sux1-Su1x);
						}//伸长线的交点坐标;

						 //确定唯一解 				
						if( (( (X0-I_one) *X1+(Y0-J_one)*Y1)>=0 && ((Sux2-Sux1)*X1+(Suy2-Suy1)*Y1)<=0) || (((X0-I_one)*X1+(Y0-J_one)*Y1)<0 && ((Sux2-Sux1)*X1+(Suy2-Suy1)*Y1)>0) )
						{
							Suy=Suy1;
							Sux=Sux1;
						}
						else if( (((X0-I_one)*X1+(Y0-J_one)*Y1)>=0 && ((Sux2-Sux1)*X1+(Suy2-Suy1)*Y1)>0) || (((X0-I_one)*X1+(Y0-J_one)*Y1)<0 && ((Sux2-Sux1)*X1+(Suy2-Suy1)*Y1)<=0) )			  
						{
							Suy=Suy2;
							Sux=Sux2;
						} 
                             //下平面插入型   //S1,S2两点的坐标;
						Sl1x= X0 + rl0x;  Sl1y=Y0 + rl0y; //第一段线段终点处的半径矢量顶点坐标;
						Sl2x= X0 + rl1x;  Sl2y=Y0 + rl1y;//第二段线起点处的半径矢量顶点坐标;

                        if(tape_class==0)
						{
			                asChild[*num] = *csChild;     //赋值,修正        //下平面伸长   
			                asChild[*num].X = Sl1x - (*Start_X);
                            asChild[*num].Y = Sl1y - (*Start_Y);
                            asChild[*num].I = I_one - (*Start_X);
                            asChild[*num].J = J_one - (*Start_Y);

							
			                asChild[*num].B = Sux - (*Start_B);         //上平面缩短    
			                asChild[*num].C = Suy - (*Start_C);
			                asChild[*num].I1 = I_one-(*Start_B);
			                asChild[*num].J1 = J_one-(*Start_C);

							asChild[*num].row_id=2;	
			                
							*num=*num+1;//第一段圆弧;
                            asChild[*num] = *csChild;     //赋值,修正        //下平面插入圆弧
						
			                asChild[*num].X = rl1x - rl0x ;
                            asChild[*num].Y = rl1y - rl0y ;
                            asChild[*num].I = -rl0x;
                            asChild[*num].J = -rl0y;

							if(X1>0)
							{ 
							  tempx=X1-100;
							  tempy=Y0+Y1/X1*(tempx-X0);
							}
							else if(X1<0)
							{
							  tempx=X1+100;
							  tempy=Y0+Y1/X1*(tempx-X0);
							}
							else 
							{
								if(Y1>0)
								{
									tempx=0;
									tempy=Y1-100;
								}
								if(Y1<0)
								{
                                    tempx=0;
									tempy=Y1+100;
								}
							}
							Arc1=calculateLineArc(rl0x,rl0y);
							Arc2=calculateLineArc(tempx-Sl1x,tempy-Sl1y);
							Arc3=calculateLineArc(rl1x,rl1y);
								
							asChild[*num].G_pathmode = Arcdirection(Arc1,Arc2,Arc3);
							asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
							asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;

			                asChild[*num].B = 0;         //上平面为一点    
			                asChild[*num].C = 0;
			                asChild[*num].I1 = 0;
			                asChild[*num].J1 = 0;
							
			                *num=*num+1;//第二段插入;

			                *Start_B = Sux-X0; //上面缩短型 
                            *Start_C = Suy-Y0;                                 
							
							*Start_X = rl1x;  //下平面
		                    *Start_Y = rl1y;

                            *first5152flag=0;

							*csChild = *dataChild;

						}
						else if(tape_class==1)
						{
                             I_one1 = I_one - Sux;
                             J_one1 = J_one - Suy;//圆心相对于(Sx,Sy)的坐标

							d01 = sqrt(I_one1*I_one1+J_one1*J_one1);

							if((csChild->G_tapemode == 51 && csChild->G_pathmode == 2)||(csChild->G_tapemode == 52 && csChild->G_pathmode == 3))
							{
									r0y1 = -(Ru0+Rl0)*(Suy-J_one)/d01;
									r0x1 = -(Ru0+Rl0)*(Sux-I_one)/d01;
							} 
							else if((csChild->G_tapemode == 52 && csChild->G_pathmode == 2)||(csChild->G_tapemode == 51 && csChild->G_pathmode == 3))
							{
									 r0y1 = (Ru0+Rl0)*(Suy-J_one)/d01;
									 r0x1 = (Ru0+Rl0)*(Sux-I_one)/d01;
							}
               
				            asChild[*num] = *csChild;     //赋值,修正        //下平面   
							
							asChild[*num].X = Sux + r0x1- (*Start_X);
							asChild[*num].Y = Suy + r0y1- (*Start_Y);
							asChild[*num].I = I_one -(*Start_X);
							asChild[*num].J = J_one -(*Start_Y);
											
							asChild[*num].B = Sux - (*Start_B);         //上平面    
							asChild[*num].C = Suy - (*Start_C);
							asChild[*num].I1 = I_one-(*Start_B);
							asChild[*num].J1 = J_one-(*Start_C);
							
							asChild[*num].row_id=2;	

                            asChild[*num].G_pathmode_upper = csChild->G_pathmode;  
							asChild[*num].G_pathmode_lower = csChild->G_pathmode;

							*num=*num+1;//第一段圆弧;

			                asChild[*num] = *csChild;     //赋值,修正        //下平面   
							asChild[*num].X = (rl1x - ru1x) - r0x1;
							asChild[*num].Y = (rl1y - ru1y) - r0y1;
							asChild[*num].I = -r0x1;
							asChild[*num].J = -r0y1;
											
							asChild[*num].B = 0;         //上平面    
							asChild[*num].C = 0;
							asChild[*num].I1 = 0;
							asChild[*num].J1 = 0;

							if(X1>0)
							{ 
							  tempx=X1-100;
							  tempy=Y0+Y1/X1*(tempx-X0);
							}
							else if(X1<0)
							{
							  tempx=X1+100;
							  tempy=Y0+Y1/X1*(tempx-X0);
							}
							else 
							{
								if(Y1>0)
								{
									tempx=0;
									tempy=Y1-100;
								}
								if(Y1<0)
								{
                                    tempx=0;
									tempy=Y1+100;
								}
							}
							Arc1=calculateLineArc(r0x1,r0y1);
							Arc2=calculateLineArc(tempx-Sux-r0x1,tempy-Suy-r0y1);
							Arc3=calculateLineArc(rl1x-ru1x,rl1y-ru1y);
								
							asChild[*num].G_pathmode = Arcdirection(Arc1,Arc2,Arc3);
							asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
							asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;

								*num=*num+1;
							
							*Start_B = Sux - X0;  
							*Start_C = Suy - Y0; 

							*Start_X = *Start_B - ru1x + rl1x;  
							*Start_Y = *Start_C - ru1y + rl1y;

                            *csChild = *dataChild;
						}	
					}//上表面缩短下表面插入型;

			else if( (zeroErrorUp >ZERO&&typeJudge_up< ANGLECONST &&zeroErrorLow < ZERO_NEG)
					||(abs(zeroErrorUp)<=ZERO&&abs(typeJudge_up+1)<ZERO&&((csChild->G_tapemode == 51 && csChild->G_pathmode == 3)||(csChild->G_tapemode == 52 && csChild->G_pathmode== 2))))
				//下表面缩短上表面插入型
				{              
			       Sl1x=X0+rl1x; Sl1y=Y0+rl1y;                                                //下平面缩短型
                   Sl2x=X0+X1+rl1x;  Sl2y=Y0+Y1+rl1y;//与圆弧相交的直线的两个点;

                    Ox=I_one;   Oy=J_one;//圆心坐标;
                    r_arc=sqrt( (X0+rl0x-I_one)*(X0+rl0x-I_one) + (Y0+rl0y-J_one)*(Y0+rl0y-J_one) );//圆弧半径

                   if(abs(X1)<=ZERO)
				   {
		               Slx1=Sl1x ;   Sly1=Oy - sqrt(r_arc*r_arc-(Sl1x-Ox)*(Sl1x-Ox));
		               Slx2=Sl1x ;   Sly2=Oy + sqrt(r_arc*r_arc-(Sl1x-Ox)*(Sl1x-Ox));
				   }          
                   else
				   {
		              K_arc=Y1/X1;
		              a_arc=1+K_arc*K_arc;
		              b_arc=2*K_arc*(Sl2y-Oy-K_arc*Sl2x)-2*Ox;
		              c_arc=(Sl2y-Oy-K_arc*Sl2x)*(Sl2y-Oy-K_arc*Sl2x)-r_arc*r_arc+Ox*Ox;
                            
	                  Slx1=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sly1=Sl2y + K_arc*(Slx1-Sl2x);
	                  Slx2=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sly2=Sl2y + K_arc*(Slx2-Sl2x);
				   }//伸长线的交点坐标;

				   //确定唯一解 
				   if( ((rl0x*X1+rl0y*Y1)>=0 && ((Slx2-Slx1)*X1+(Sly2-Sly1)*Y1)<=0) || ((rl0x*X1+rl0y*Y1)<0 && ((Slx2-Slx1)*X1+(Sly2-Sly1)*Y1)>0) )
				   {
			         Sly=Sly1;
			         Slx=Slx1;
				   }							
			       else if( ((rl0x*X1+rl0y*Y1)>=0 && ((Slx2-Slx1)*X1+(Sly2-Sly1)*Y1)>0) || ((rl0x*X1+rl0y*Y1)<0 && ((Slx2-Slx1)*X1+(Sly2-Sly1)*Y1)<=0) )
				   {
			         Sly=Sly2;
			         Slx=Slx2;
				   }
					 Su1x= X0 + ru0x;  Su1y=Y0 + ru0y; //第一段线段终点处的半径矢量顶点坐标;
					 Su2x= X0 + ru1x;  Su2y=Y0 + ru1y;//第二段线起点处的半径矢量顶点坐标;
                                        //S1,S2两点的坐标;
                     if(tape_class==0)
					 {				     
				        asChild[*num] = *csChild;     //赋值,修正        //下平面   
		                asChild[*num].X = Slx - (*Start_X);
                        asChild[*num].Y=  Sly - (*Start_Y);
                        asChild[*num].I = I_one-(*Start_X);
                        asChild[*num].J = J_one-(*Start_Y);
						
		                asChild[*num].B = X0 + ru0x - (*Start_B);         //上平面    
		                asChild[*num].C = Y0 + ru0y - (*Start_C);
		                asChild[*num].I1 = I_one - (*Start_B);
		                asChild[*num].J1 = J_one - (*Start_C);
                        
						asChild[*num].row_id=2;	
						asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;  
						asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;
		              
						*num=*num+1;//第一段圆弧;
						
		                asChild[*num] = *csChild;            //下平面
		                asChild[*num].X = 0;
                        asChild[*num].Y = 0;
                        asChild[*num].I = 0;
                        asChild[*num].J = 0;

		                asChild[*num].B = ru1x - ru0x;         //上平面    
		                asChild[*num].C = ru1y - ru0y;
		                asChild[*num].I1 = -ru0x;
		                asChild[*num].J1 = -ru0y;

						if(X1>0)
						{ 
						  tempx=X1-100;
						  tempy=Y0+Y1/X1*(tempx-X0);
						}
						else if(X1<0)
						{
						  tempx=X1+100;
						  tempy=Y0+Y1/X1*(tempx-X0);
						}
						else 
						{
							if(Y1>0)
							{
								tempx=0;
								tempy=Y1-100;
							}
							if(Y1<0)
							{
                                tempx=0;
								tempy=Y1+100;
							}
						}	
							
						Arc1=calculateLineArc(ru0x,ru0y);
						Arc2=calculateLineArc(tempx-Su1x,tempy-Su1y);
						Arc3=calculateLineArc(ru1x,ru1y);
							
						asChild[*num].G_pathmode = Arcdirection(Arc1,Arc2,Arc3);
						asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
						asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;
						
		                *Start_X =Slx-X0;  
                        *Start_Y =Sly-Y0; 
		                *Start_B = ru1x;  
                        *Start_C = ru1y;

		                *num=*num+1;//第二段插入圆弧;
						*csChild = *dataChild;
					 }
					
					 else if(tape_class==1)
					 {
                        I_one1 = I_one - Slx;
                        J_one1 = J_one - Sly;//圆心相对于(Sx,Sy)的坐标

						d01 = sqrt(I_one1*I_one1+J_one1*J_one1);

						if((csChild->G_tapemode == 51 && csChild->G_pathmode == 2)||(csChild->G_tapemode == 52 && csChild->G_pathmode == 3))
						{
							r0y1 = (Rl0+Ru0)*(Sly-J_one)/d01;
							r0x1 = (Rl0+Ru0)*(Slx-I_one)/d01;
						} 
						else if((csChild->G_tapemode == 52 && csChild->G_pathmode == 2)||(csChild->G_tapemode == 51 && csChild->G_pathmode == 3))
						{
							 r0y1 = -(Rl0+Ru0)*(Sly-J_one)/d01;
							 r0x1 = -(Rl0+Ru0)*(Slx-I_one)/d01;
						}
	                    asChild[*num] = *csChild;     //赋值,修正        //下平面   
						asChild[*num].X = Slx - (*Start_X);
						asChild[*num].Y = Sly - (*Start_Y);
						asChild[*num].I = I_one-(*Start_X);
						asChild[*num].J = J_one-(*Start_Y);
										
						asChild[*num].B = Slx + r0x1 - (*Start_B);         //上平面    
						asChild[*num].C = Sly + r0y1 - (*Start_C);
						asChild[*num].I1 = I_one -(*Start_B);
						asChild[*num].J1 = J_one -(*Start_C);

						asChild[*num].row_id=2;	
                        asChild[*num].G_pathmode_upper = csChild->G_pathmode;  
						asChild[*num].G_pathmode_lower = csChild->G_pathmode;

						*num=*num+1;//第一段圆弧;

                        asChild[*num] = *csChild;     //赋值,修正        //下平面   
						asChild[*num].X = 0;
						asChild[*num].Y = 0;
						asChild[*num].I = 0;
						asChild[*num].J = 0;
										
						asChild[*num].B = (ru1x - rl1x) - r0x1;         //上平面    
						asChild[*num].C = (ru1y - rl1y) - r0y1;
						asChild[*num].I1 = -r0x1;
						asChild[*num].J1 = -r0y1;
			           
						if(X1>0)
						{ 
						  tempx=X1-100;
						  tempy=Y0+Y1/X1*(tempx-X0);
						}
						else if(X1<0)
						{
						  tempx=X1+100;
						  tempy=Y0+Y1/X1*(tempx-X0);
						}
						else 
						{
							if(Y1>0)
							{
								tempx=0;
								tempy=Y1-100;
							}
							if(Y1<0)
							{
                                tempx=0;
								tempy=Y1+100;
							}
						}				
						Arc1=calculateLineArc(r0x1,r0y1);
						Arc2=calculateLineArc(tempx-Slx-rl0x,tempy-Sly-rl0y);
						Arc3=calculateLineArc(ru1x-rl1x,ru1y-rl1y);
							
						asChild[*num].G_pathmode = Arcdirection(Arc1,Arc2,Arc3);
						asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
						asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;

						*Start_X = Slx - X0;  
						*Start_Y = Sly - Y0; 
						*Start_B = *Start_X - rl1x + ru1x;  
						*Start_C = *Start_Y - rl1y + ru1y;
						
						*num=*num+1;
                        *csChild = *dataChild;
					 }
					}//下平面缩短上平面插入型;

				else if (abs(zeroErrorUp)<0.01&&abs(typeJudge_up-1)<0.01)
				{
						 Sux=X0+ru0x;Suy=Y0+ru0y;
					     Slx=X0+rl0x;Sly=Y0+rl0y;

                        asChild[*num]= *csChild;     //赋值,修正,第一段圆弧部分
                    
                        asChild[*num].B = Sux - (*Start_B);
                        asChild[*num].C = Suy - (*Start_C);
                        asChild[*num].I1 = I_one-(*Start_B);
                        asChild[*num].J1 = J_one-(*Start_C);
                    
                        asChild[*num].X = Slx - (*Start_X);
                        asChild[*num].Y = Sly - (*Start_Y);
                        asChild[*num].I = I_one -(*Start_X);
                        asChild[*num].J = J_one -(*Start_Y);
						asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
						asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;
						*Start_B = ru1x;  
						*Start_C = ru1y;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;    
						*Start_X = rl1x;  
						*Start_Y = rl1y;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备；

						*num=*num+1;
						*csChild = *dataChild;
					}
				else 
				{  //伸长型改为缩短型
						Su1x=X0+ru1x; Su1y=Y0+ru1y;                                                //上平面缩短型
						Su2x=X0+X1+ru1x;  Su2y=Y0+Y1+ru1y;//与圆弧相交的直线的两个点;

						Ox=I_one;   Oy=J_one;//圆心坐标;
						r_arc=sqrt( (X0+ru0x-I_one)*(X0+ru0x-I_one) + (Y0+ru0y-J_one)*(Y0+ru0y-J_one) );//圆弧半径

						if(abs(X1) <= 0.20000001)
						{
							if(abs(r_arc*r_arc-(Su1x-Ox)*(Su1x-Ox))<=ZERO)
							{
								Sux1=Su1x ;   Suy1=Oy;
								Sux2=Su1x ;   Suy2=Oy;
							}
							else
							{
								Sux1=Su1x ;   Suy1=Oy - sqrt(r_arc*r_arc-(Su1x-Ox)*(Su1x-Ox));
								Sux2=Su1x ;   Suy2=Oy + sqrt(r_arc*r_arc-(Su1x-Ox)*(Su1x-Ox));
							}
						}
	                   
						else
						{
							K_arc=Y1/X1;
							a_arc=1+K_arc*K_arc;
							b_arc=2*K_arc*(Su2y-Oy-K_arc*Su2x)-2*Ox;
							c_arc=(Su2y-Oy-K_arc*Su2x)*(Su2y-Oy-K_arc*Su2x)-r_arc*r_arc+Ox*Ox;						                            
					
							if((b_arc*b_arc-4*a_arc*c_arc<0)&&abs(b_arc*b_arc-4*a_arc*c_arc)<=ZERO)
							{
								Sux1=( -b_arc)/(2*a_arc);	Suy1=Su1y + K_arc*(Sux1-Su1x);
								Sux2=( -b_arc)/(2*a_arc);	Suy2=Su1y + K_arc*(Sux2-Su1x);
							}
							else
							{
								Sux1=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Suy1=Su1y + K_arc*(Sux1-Su1x);
								Sux2=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Suy2=Su1y + K_arc*(Sux2-Su1x);
							}
						}//伸长线的交点坐标;
			//确定唯一解 
						//if( ((ru0x*X1+ru0y*Y1)>=0 && ((Sux2-Sux1)*X1+(Suy2-Suy1)*Y1)<=0) || ((ru0x*X1+ru0y*Y1)<0 && ((Sux2-Sux1)*X1+(Suy2-Suy1)*Y1)>0) )
						if( (( (X0-I_one) *X1+(Y0-J_one)*Y1)>=0 && ((Sux2-Sux1)*X1+(Suy2-Suy1)*Y1)<=0) || (((X0-I_one)*X1+(Y0-J_one)*Y1)<0 && ((Sux2-Sux1)*X1+(Suy2-Suy1)*Y1)>0) )
							{
								Suy=Suy1;
								Sux=Sux1;
							}
						else if( (((X0-I_one)*X1+(Y0-J_one)*Y1)>=0 && ((Sux2-Sux1)*X1+(Suy2-Suy1)*Y1)>0) || (((X0-I_one)*X1+(Y0-J_one)*Y1)<0 && ((Sux2-Sux1)*X1+(Suy2-Suy1)*Y1)<=0) )			
						//else if( ((ru0x*X1+ru0y*Y1)>=0 && ((Sux2-Sux1)*X1+(Suy2-Suy1)*Y1)>0) || ((ru0x*X1+ru0y*Y1)<0 && ((Sux2-Sux1)*X1+(Suy2-Suy1)*Y1)<=0) )   
						{
								Suy=Suy2;
								Sux=Sux2;
							}                  
                            
		                Sl1x=X0+rl1x; Sl1y=Y0+rl1y;                                               
                        Sl2x=X0+X1+rl1x;  Sl2y=Y0+Y1+rl1y;//与圆弧相交的直线的两个点;

                        Ox=I_one;   Oy=J_one;//圆心坐标;
                        r_arc=sqrt( (X0+rl0x-I_one)*(X0+rl0x-I_one) + (Y0+rl0y-J_one)*(Y0+rl0y-J_one) );//圆弧半径

                        if(abs(X1) <= ZERO)
                        {
							if(abs(r_arc*r_arc-(Sl1x-Ox)*(Sl1x-Ox))<=ZERO)
							{
								Slx1=Sl1x ;   Sly1=Oy;
								Slx2=Sl1x ;   Sly2=Oy;
							}
							else
							{
								Slx1=Sl1x ;   Sly1=Oy + sqrt(r_arc*r_arc-(Sl1x-Ox)*(Sl1x-Ox));
								Slx2=Sl1x ;   Sly2=Oy - sqrt(r_arc*r_arc-(Sl1x-Ox)*(Sl1x-Ox));
							}
                        } 
                        else
                        {
							K_arc=Y1/X1;
							a_arc=1+K_arc*K_arc;
							b_arc=2*K_arc*(Sl2y-Oy-K_arc*Sl2x)-2*Ox;
							c_arc=(Sl2y-Oy-K_arc*Sl2x)*(Sl2y-Oy-K_arc*Sl2x)-r_arc*r_arc+Ox*Ox;

							if(abs(b_arc*b_arc-4*a_arc*c_arc)<=ZERO)
							{								
								Slx1=( -b_arc)/(2*a_arc);   Sly1=Sl2y + K_arc*(Slx1-Sl2x);						                            
                                Slx2=( -b_arc)/(2*a_arc);   Sly2=Sl2y + K_arc*(Slx2-Sl2x);
							}
							else
							{
								Slx1=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sly1=Sl2y + K_arc*(Slx1-Sl2x);						                            
								Slx2=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sly2=Sl2y + K_arc*(Slx2-Sl2x);
							}
                        }//伸长线的交点坐标;
						//确定唯一解 
						//if( ((rl0x*X1+rl0y*Y1)>=0 && ((Slx2-Slx1)*X1+(Sly2-Sly1)*Y1)<=0) || ((rl0x*X1+rl0y*Y1)<0 && ((Slx2-Slx1)*X1+(Sly2-Sly1)*Y1)>0) )
						if( (( (X0-I_one) *X1+(Y0-J_one)*Y1)>=0 && ((Slx2-Slx1)*X1+(Sly2-Sly1)*Y1)<=0) || (((X0-I_one)*X1+(Y0-J_one)*Y1)<0 && ((Slx2-Slx1)*X1+(Sly2-Sly1)*Y1)>0) )
							{
								Sly=Sly1;
								Slx=Slx1;
							}								
						//else if( ((rl0x*X1+rl0y*Y1)>=0 && ((Slx2-Slx1)*X1+(Sly2-Sly1)*Y1)>0) || ((rl0x*X1+rl0y*Y1)<0 && ((Slx2-Slx1)*X1+(Sly2-Sly1)*Y1)<=0) )
						else if( (((X0-I_one)*X1+(Y0-J_one)*Y1)>=0 && ((Slx2-Slx1)*X1+(Sly2-Sly1)*Y1)>0) || (((X0-I_one)*X1+(Y0-J_one)*Y1)<0 && ((Slx2-Slx1)*X1+(Sly2-Sly1)*Y1)<=0) )
						{
								Sly=Sly2;
								Slx=Slx2;
							}

						if(tape_class==0)
						{  
							asChild[*num] = *csChild;     //赋值,修正        //下平面   
							asChild[*num].X = Slx - (*Start_X);
							asChild[*num].Y=  Sly - (*Start_Y);
							asChild[*num].I = I_one-(*Start_X);
							asChild[*num].J = J_one-(*Start_Y);
											
							asChild[*num].B = Sux - (*Start_B);         //上平面    
							asChild[*num].C = Suy - (*Start_C);
							asChild[*num].I1 = I_one-(*Start_B);
							asChild[*num].J1 = J_one-(*Start_C);

							*num=*num+1;//第一段圆弧;

							*Start_X = Slx - X0;  //下平面
							*Start_Y = Sly - Y0;

							*Start_B = Sux - X0;  
							*Start_C = Suy - Y0;

							*csChild = *dataChild;
						}										
						else if(tape_class==1)
						{  
						 if ((zeroErrorUp>ZERO&&typeJudge_up> ANGLECONST&&zeroErrorLow<ZERO_NEG)
							 ||(abs(zeroErrorLow)<=ZERO&&abs(typeJudge_low+1)<ZERO&&((csChild->G_tapemode == 51 && csChild->G_pathmode == 3)||(csChild->G_tapemode == 52 && csChild->G_pathmode== 2))))//上表面伸长下表面缩短型		
						 {       
								I_one1 = I_one - Slx;
								J_one1 = J_one - Sly;//圆心相对于(Sx,Sy)的坐标
								d01 = sqrt(I_one1*I_one1+J_one1*J_one1);

							 if((csChild->G_tapemode == 51 && csChild->G_pathmode == 2)||(csChild->G_tapemode == 52 && csChild->G_pathmode == 3))
								{
									r0y1 = (Rl0+Ru0)*(Sly-J_one)/d01;
									r0x1 = (Rl0+Ru0)*(Slx-I_one)/d01;
								} 
							 else if((csChild->G_tapemode == 52 && csChild->G_pathmode == 2)||(csChild->G_tapemode == 51 && csChild->G_pathmode == 3))
								{
									 r0y1 = -(Rl0+Ru0)*(Sly-J_one)/d01;
									 r0x1 = -(Rl0+Ru0)*(Slx-I_one)/d01;
								}

							 asChild[*num] = *csChild;     //赋值,修正        //下平面   
							 asChild[*num].X = Slx - (*Start_X);
							 asChild[*num].Y = Sly - (*Start_Y);
							 asChild[*num].I = I_one-(*Start_X);
							 asChild[*num].J = J_one-(*Start_Y);
											
							 asChild[*num].B = Slx + r0x1 - (*Start_B);         //上平面    
							 asChild[*num].C = Sly + r0y1 - (*Start_C);
							 asChild[*num].I1 = I_one -(*Start_B);
							 asChild[*num].J1 = J_one -(*Start_C);
							
							 asChild[*num].row_id=2;	

							 *num=*num+1;//第一段圆弧;
							 asChild[*num] = *csChild;     //赋值,修正        //下平面   
							 
							 asChild[*num].X = 0;
							 asChild[*num].Y = 0;
							 asChild[*num].I = 0;
							 asChild[*num].J = 0;
											
							 asChild[*num].B = ru1x - rl1x - r0x1;         //上平面    
							 asChild[*num].C = ru1y - rl1y - r0y1;
							 asChild[*num].I1 = -r0x1;
							 asChild[*num].J1 = -r0y1;
				           
							 Arc1=calculateLineArc(r0x1,r0y1);
							 Arc2=calculateLineArc(Sux-Slx,Suy-Sly);
							 Arc3=calculateLineArc(ru1x-rl1x,ru1y-rl1y);
								
							 asChild[*num].G_pathmode = Arcdirection(Arc1,Arc2,Arc3);
							 asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
							 asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;

							*Start_X = Slx - X0;  
							*Start_Y = Sly - Y0; 
							*Start_B = *Start_X - rl1x + ru1x;  
							*Start_C = *Start_Y - rl1y + ru1y;
							
							*num=*num+1;
							*csChild = *dataChild;
						 }
						 if ((zeroErrorLow>ZERO&&typeJudge_low>ANGLECONST&&zeroErrorUp<-0.001)
							 ||(abs(zeroErrorUp)<=ZERO&&abs(typeJudge_up+1)<ZERO&&((csChild->G_tapemode == 51 && csChild->G_pathmode == 2)||(csChild->G_tapemode == 52 && csChild->G_pathmode== 3))))//上表面缩短下表面伸长型
						 {             
							I_one1 = I_one - Sux;
							J_one1 = J_one - Suy;//圆心相对于(Sx,Sy)的坐标

							d01 = sqrt(I_one1*I_one1+J_one1*J_one1);

							if((csChild->G_tapemode == 51 && csChild->G_pathmode == 2)||(csChild->G_tapemode == 52 && csChild->G_pathmode == 3))
							{
								r0y1 = -(Ru0+Rl0)*(Suy-J_one)/d01;
								r0x1 = -(Ru0+Rl0)*(Sux-I_one)/d01;
							} 
							else if((csChild->G_tapemode == 52 && csChild->G_pathmode == 2)||(csChild->G_tapemode == 51 && csChild->G_pathmode == 3))
							{
								 r0y1 = (Ru0+Rl0)*(Suy-J_one)/d01;
								 r0x1 = (Ru0+Rl0)*(Sux-I_one)/d01;
							}

							asChild[*num] = *csChild;     //赋值,修正        //下平面   
							asChild[*num].X = Sux + r0x1 - (*Start_X);
							asChild[*num].Y = Suy + r0y1 - (*Start_Y);
							asChild[*num].I = I_one -(*Start_X);
							asChild[*num].J = J_one -(*Start_Y);
											
							asChild[*num].B = Sux - (*Start_B);         //上平面    
							asChild[*num].C = Suy - (*Start_C);
							asChild[*num].I1 = I_one-(*Start_B);
							asChild[*num].J1 = J_one-(*Start_C);
							asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
							asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;

							asChild[*num].row_id=2;	
							*num=*num+1;//第一段圆弧;
							asChild[*num] = *csChild;     //赋值,修正        //下平面   
							
							asChild[*num].X = rl1x - ru1x - r0x1;
							asChild[*num].Y = rl1y - ru1y - r0y1;
							asChild[*num].I = -r0x1;
							asChild[*num].J = -r0y1;
											
							asChild[*num].B = 0;         //上平面    
							asChild[*num].C = 0;
							asChild[*num].I1 = 0;
							asChild[*num].J1 = 0;

							Arc1=calculateLineArc(r0x1,r0y1);
							Arc2=calculateLineArc(Slx-Sux,Sly-Suy);
							Arc3=calculateLineArc(rl1x-ru1x,rl1y-ru1y);
								
							asChild[*num].G_pathmode = Arcdirection(Arc1,Arc2,Arc3);
							asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
							asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;

							*num=*num+1;
							*Start_B = Sux - X0;  
							*Start_C = Suy - Y0;
							
							*Start_X = *Start_B + rl1x - ru1x;  
							*Start_Y = *Start_C + rl1y - ru1y; 

							*csChild = *dataChild;
						  }	
						 }
						  }//上平面缩短下平面伸长型;

                         if(dataChild->G_tapemode == 50)       //取消刀补;
                        {	
                            asChild[*num]= *dataChild;     //赋值,修正									
			                asChild[*num].B = X1 - (*Start_B);
                            asChild[*num].C = Y1 - (*Start_C); 
							asChild[*num].X = X1 - (*Start_X);
                            asChild[*num].Y = Y1 - (*Start_Y);
                             
							*num=*num+1;
                            *sign = 0;//跳出补偿
                            *c = 0;//为下一次刀补作初始化.                   
							*Start_X = 0;   
                            *Start_Y = 0;
                            *Start_B = 0;   
                            *Start_C = 0;
                      }//取消刀补;
		   }//圆弧接直线         
             else if((dataChild->G_pathmode == 2 || dataChild->G_pathmode == 3) && csChild->G_pathmode ==1)//直线接圆弧
//********************************************************************************                
			 {
                    X0 = csChild->X;
                    Y0 = csChild->Y;

                    X1 = dataChild->X;
                    Y1 = dataChild->Y;
                    I_two=dataChild->I;
                    J_two=dataChild->J;

                        d0 = sqrt(X0*X0 +Y0*Y0);    
                        d1 = sqrt((Y1-J_two)*(Y1-J_two)+(X1-I_two)*(X1-I_two)); 

                       if(csChild->G_tapemode == 51)
                        {
                            ru0y = Ru0*X0/d0;
                            ru0x = (-Ru0)*Y0/d0;  
                            
			                rl0y = (-Rl0)*X0/d0;
                            rl0x = Rl0*Y0/d0; 
                        }       
                        else if(csChild->G_tapemode == 52)
                        {
                            ru0y = (-Ru0)*X0/d0;
                            ru0x = Ru0*Y0/d0;  
                            
			                rl0y = Rl0*X0/d0;
                            rl0x = (-Rl0)*Y0/d0; 
                        }             
					   if((csChild->G_tapemode == 51 && dataChild->G_pathmode == 2)||(csChild->G_tapemode == 52 && dataChild->G_pathmode == 3))
					   {
						 ru1y = Ru1*(-J_two)/d1;
						 ru1x = Ru1*(-I_two)/d1;

						 rl1y = (-Rl1)*(-J_two)/d1;
						 rl1x = (-Rl1)*(-I_two)/d1;
					   }
					   else if((csChild->G_tapemode == 52 && dataChild->G_pathmode == 2)||(csChild->G_tapemode == 51 && dataChild->G_pathmode == 3))
					   {
						ru1y = (-Ru1)*(-J_two)/d1;
						ru1x = (-Ru1)*(-I_two)/d1;

						rl1y = Rl1*(-J_two)/d1;
						rl1x = Rl1*(-I_two)/d1; 
					   }
                     if(dataChild->G_pathmode == 2)
					 {
                        KBy1= I_two;
                        KBx1=-J_two;
					 }
                     else if(dataChild->G_pathmode == 3)
					 {
                        KBy1=-I_two;
                        KBx1= J_two;
					 }// 确定KB参量,圆弧起点处的切线;

                //判断交接类型:缩短,插入,伸长

				  if((sqrt(ru0y*ru0y + ru0x*ru0x)*sqrt(ru1x*ru1x + ru1y*ru1y))==0) 
				  {   
					  typeJudge_up=0;
				  }
				  else 
				  {
					  typeJudge_up=(ru0y*ru1y + ru0x*ru1x)/(sqrt(ru0y*ru0y + ru0x*ru0x)*sqrt(ru1x*ru1x + ru1y*ru1y));
				  }
				  if(sqrt(rl0y*rl0y + rl0x*rl0x)*sqrt(rl1x*rl1x + rl1y*rl1y)==0)
				  {
					  typeJudge_low=0;
				  }
				  else
				  {
					  typeJudge_low=(rl0y*rl1y + rl0x*rl1x)/(sqrt(rl0y*rl0y + rl0x*rl0x)*sqrt(rl1x*rl1x + rl1y*rl1y));
				  }
		   
				 zeroErrorUp = (ru1y-ru0y)*Y0 + (ru1x-ru0x)*X0;
				 zeroErrorLow = (rl1y-rl0y)*Y0 + (rl1x-rl0x)*X0;
                 
				 if(*first5152flag == 1)
				  {       
					    ru0y = 0;               //上平面偏移矢量
                        ru0x = 0;
						rl0y = 0;               //下平面偏移矢量
                        rl0x = 0;
				  }
				  if(zeroErrorUp<ZERO_NEG)
				  {
					if(sqrt(I_two*I_two+J_two*J_two)<=Ru0) return;
				  }
				  else if (zeroErrorLow<ZERO_NEG)
				  {
					if(sqrt(I_two*I_two+J_two*J_two)<=Rl0) return;
				  }//判断圆弧半径够不够大

				 if((zeroErrorLow>ZERO&&typeJudge_low> ANGLECONST)||abs(typeJudge_low-1)<ZERO)//判断直线与圆弧会不会不相交||(abs(ru1x-ru0x)<ZERO&&abs(ru1y-ru0y)<ZERO)
				 {
				   Sx=rl0x;  Sy=rl0y; 
				   r=sqrt( (rl1x-I_two)*(rl1x-I_two) + (rl1y-J_two)*(rl1y-J_two) );
				   if(X0==0)
				   {
					   d=abs(I_two-X0-rl1x);
				   }
				   else
				   {
                     K=Y0/X0;
					 d=abs(K*(I_two-Sx)-(J_two-Sy))/sqrt(1+K*K);
				   }
				   if((d-r)>ZERO)
				   {
                       typeJudge_low=-1;
				   }
				 }
				 else if((zeroErrorUp>ZERO&&typeJudge_up> ANGLECONST)||abs(typeJudge_up-1)<ZERO)
				 {                                                
				   Sx=ru0x;  Sy=ru0y; 
				   r=sqrt( (ru1x-I_two)*(ru1x-I_two) + (ru1y-J_two)*(ru1y-J_two) );
				   if(X0==0)
				   {
					   d=abs(I_two-X0-ru1x);
				   }
				   else
				   {
                     K=Y0/X0;
					 d=abs(K*(I_two-Sx)-(J_two-Sy))/sqrt(1+K*K);
				   }
				   if(d>r)
				   {
                       typeJudge_up=-1;
				   }
				 }

				  if(    (zeroErrorLow>ZERO&&typeJudge_low<ANGLECONST&&zeroErrorUp<ZERO_NEG) 
					  ||(abs(zeroErrorLow)<=ZERO&&abs(typeJudge_low+1)<ZERO&&((dataChild->G_tapemode == 51 && dataChild->G_pathmode == 2)|| (dataChild->G_tapemode == 52 && dataChild->G_pathmode == 3)) ) )
				  {	 //上缩下插
                       Su1x=ru0x;    Su1y=ru0y;
                       Su2x=X0+ru0x; Su2y=Y0+ru0y;//与圆弧相交的直线的两个点;

                       Ox=X0+I_two;    Oy=Y0+J_two;//圆心坐标;
                       r_arc=sqrt( (X0+ru1x-Ox)*(X0+ru1x-Ox) + (Y0+ru1y-Oy)*(Y0+ru1y-Oy) );//圆弧半径

                       if(Su1x == Su2x)
					   {
							Sux1=Su1x ;   Suy1=Oy - sqrt(r_arc*r_arc-(Su1x-Ox)*(Su1x-Ox));
							Sux2=Su1x ;   Suy2=Oy + sqrt(r_arc*r_arc-(Su1x-Ox)*(Su1x-Ox));
                        }
                       else
					   {
							K_arc=(Y0/X0);
							a_arc=1+K_arc*K_arc;
							b_arc=2*K_arc*(Su2y-Ox-K_arc*Su2x)-2*Ox;
							c_arc=(Su2y-Oy-K_arc*Su2x)*(Su2y-Oy-K_arc*Su2x)-r_arc*r_arc+Ox*Ox;
                            
							Sux1=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Suy1=Su1y + K_arc*(Sux1-Su1x);
		                    Sux2=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Suy2=Su1y + K_arc*(Sux2-Su1x);
					   }//伸长线的交点坐标;
                     if( (((-I_two)*X0+(-J_two)*Y0)>=0 && ((Sux2-Sux1)*X0+(Suy2-Suy1)*Y0)<=0) || (((-I_two)*X0+(-J_two)*Y0)<=0 && ((Sux2-Sux1)*X0+(Suy2-Suy1)*Y0)>0) )
                        {
							Suy=Suy1;
							Sux=Sux1;
                        }
                     else if( (((-I_two)*X0+(-J_two)*Y0)>0 && ((Sux2-Sux1)*X0+(Suy2-Suy1)*Y0)>0) || (((-I_two)*X0+(-J_two)*Y0)<0 && ((Sux2-Sux1)*X0+(Suy2-Suy1)*Y0)<=0) )
                        {
							Suy=Suy2;
							Sux=Sux2;
                        }

                     Sl1x= X0 + rl0x;  Sl1y=Y0 + rl0y; //第一段线段终点处的半径矢量顶点坐标;
					 Sl2x= X0 + rl1x;  Sl2y=Y0 + rl1y;//第二段线起点处的半径矢量顶点坐标; //S1,S2两点的坐标;
                                   
					 if (tape_class == 0)
					 {                               
                        asChild[*num]= *csChild;     //赋值,修正第一段直线

                        asChild[*num].B = Sux - (*Start_B);
                        asChild[*num].C = Suy - (*Start_C);
                        asChild[*num].X = Sl1x - (*Start_X);
                        asChild[*num].Y = Sl1y - (*Start_Y);

                        asChild[*num].row_id=2;	
						
						*num=*num+1;//第一段直线;
                    	asChild[*num] = *csChild;     //赋值,修正        //上平面点，下平面圆弧
		               
						if(*first5152flag==1)
						{	
							asChild[*num].X = rl1x - rl0x ;
							asChild[*num].Y = rl1y - rl0y ;
							asChild[*num].I = 0.5*(rl1x);
							asChild[*num].J = 0.5*(rl1y);
						}
						else
						{
							asChild[*num].X = Sl2x - Sl1x;           //下平面   
							asChild[*num].Y = Sl2y - Sl1y;
							asChild[*num].I = (-rl0x);            
							asChild[*num].J = (-rl0y);
						}
		                asChild[*num].B = 0;         //上平面    
		                asChild[*num].C = 0;
                        asChild[*num].I1 = 0;
                        asChild[*num].J1 = 0;

						if(X0>0)
						{ 
						  tempx=X0+100;
						  tempy=Y0/X0*tempx;
						}
						else if(X0<0)
						{
						  tempx=X0-100;
						  tempy=Y0/X0*tempx;
						}
						else 
						{
							if(Y0>0)
							{
								tempx=0;
								tempy=Y0+100;
							}
							if(Y0<0)
							{
                                tempx=0;
								tempy=Y0-100;
							}
						}
						Arc1=calculateLineArc(rl0x,rl0y);
						Arc2=calculateLineArc(tempx-X0,tempy-Y0);
						Arc3=calculateLineArc(rl1x,rl1y);
							
						asChild[*num].G_pathmode = Arcdirection(Arc1,Arc2,Arc3);
						asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
						asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;

						*Start_X =rl1x;  
						*Start_Y =rl1y; 
						*Start_B =Sux - X0;  
						*Start_C =Suy - Y0;

						*first5152flag = 0;
						*num=*num+1;
						*csChild = *dataChild;
					 }						
					 if (tape_class == 1)
					 {     						                                                    
					   asChild[*num]= *csChild;     //赋值,修正第一段直线

					   if(*first5152flag==1)
					   {
                        asChild[*num].B = Sux - (*Start_B);
                        asChild[*num].C = Suy - (*Start_C);                                                                  				
						asChild[*num].X = Sl1x - (*Start_X);  //下平面
						asChild[*num].Y = Sl1y - (*Start_Y); 

						*num=*num+1;//第一段直线;

                        *first5152flag = 0;
		               
						*Start_B = Sux - X0;  
                        *Start_C = Suy - Y0;  
						*Start_X = Sl1x - X0;  
                        *Start_Y = Sl1y - Y0; 

						*csChild = *dataChild;
					   }
					   else
					   {
                        asChild[*num].B = Sux - (*Start_B);
                        asChild[*num].C = Suy - (*Start_C);                                                             				
						asChild[*num].X = Sux - ru0x + rl0x - (*Start_X);  //下平面
						asChild[*num].Y = Suy - ru0y + rl0y - (*Start_Y); 
					     
						asChild[*num].row_id=2;	                      
                        
						*num=*num+1;//第一段直线;
                    	asChild[*num] = *csChild;     //赋值,修正        
		               
                        I_two1 = I_two - (Sux - X0);
                        J_two1 = J_two - (Suy - Y0);//圆心相对于(Sx,Sy)的坐标
					    d11 = sqrt(I_two1*I_two1+J_two1*J_two1);

						if((csChild->G_tapemode == 51 && dataChild->G_pathmode == 2)||(csChild->G_tapemode == 52 && dataChild->G_pathmode == 3))
						{
							 r1y1 = -(Ru1+Rl1)*(-J_two)/d11;
							 r1x1 = -(Ru1+Rl1)*(-I_two)/d11;
						 }
						 else if((csChild->G_tapemode == 52 && dataChild->G_pathmode == 2)||(csChild->G_tapemode == 51 && dataChild->G_pathmode == 3))
						 {
							r1y1 = (Ru1+Rl1)*(-J_two)/d11;
							r1x1 = (Ru1+Rl1)*(-I_two)/d11;
						}
						asChild[*num].X = r1x1 - (rl0x - ru0x) ;  //下平面伸长
						asChild[*num].Y = r1y1 - (rl0y - ru0y) ;
						asChild[*num].I = ru0x - rl0x ;  
						asChild[*num].J = ru0y - rl0y ;

						asChild[*num].B = 0 ;  //下平面伸长
						asChild[*num].C = 0 ;
						asChild[*num].I1 = 0 ;  
						asChild[*num].J1 = 0 ;

						if(X0>0)
						{ 
						  tempx=X0+100;
						  tempy=Y0/X0*tempx;
						}
						else if(X0<0)
						{
						  tempx=X0-100;
						  tempy=Y0/X0*tempx;
						}
						else 
						{
							if(Y0>0)
							{
								tempx=0;
								tempy=Y0+100;
							}
							if(Y0<0)
							{
                                tempx=0;
								tempy=Y0-100;
							}
						}
						Arc1=calculateLineArc(rl0x-ru0x,rl0y-ru0y);
						Arc2=calculateLineArc(tempx-(Sux - ru0x + rl0x),tempy-(Suy - ru0y + rl0y));
						Arc3=calculateLineArc(r1x1,r1y1);
							
						asChild[*num].G_pathmode = Arcdirection(Arc1,Arc2,Arc3);
						asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
						asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;

						*Start_B = Sux - X0;  
                        *Start_C = Suy - Y0; 
						*Start_X = *Start_B + r1x1;  
                        *Start_Y = *Start_C + r1y1; 
		
						*num=*num+1;
						*csChild = *dataChild;
					   }
					 }
					 }//上缩下插
                        
                     else if( (zeroErrorUp>ZERO && typeJudge_up<ANGLECONST&&zeroErrorLow<ZERO_NEG)
						 ||(abs(zeroErrorUp)<=ZERO&&abs(typeJudge_up+1)<ZERO&&((dataChild->G_tapemode == 51 && dataChild->G_pathmode == 3)|| (dataChild->G_tapemode == 52 && dataChild->G_pathmode == 2) ) )  ) 
                       //上插下缩
					 {
							Su1x = X0 + ru0x;  Su1y = Y0 + ru0y; //第一段线段终点处的半径矢量顶点坐标;
							Su2x = X0 + ru1x;  Su2y = Y0 + ru1y;//第二段线起点处的半径矢量顶点坐标;
										//S1,S2两点的坐标;                         

							Sl1x=rl0x;    Sl1y=rl0y;
							Sl2x=X0+rl0x; Sl2y=Y0+rl0y;//与圆弧相交的直线的两个点;

							Ox=X0+I_two;    Oy=Y0+J_two;//圆心坐标;
							r_arc=sqrt( (X0+rl1x-Ox)*(X0+rl1x-Ox) + (Y0+rl1y-Oy)*(Y0+rl1y-Oy) );//圆弧半径

							if(Sl1x == Sl2x)
							{
								Slx1=Sl1x ;   Sly1=Oy - sqrt(r_arc*r_arc-(Sl1x-Ox)*(Sl1x-Ox));
								Slx2=Sl1x ;   Sly2=Oy + sqrt(r_arc*r_arc-(Sl1x-Ox)*(Sl1x-Ox));
							}
							else
							{
								K_arc=(Y0/X0);
								a_arc=1+K_arc*K_arc;
								b_arc=2*K_arc*(Sl2y-Ox-K_arc*Sl2x)-2*Ox;
								c_arc=(Sl2y-Oy-K_arc*Sl2x)*(Sl2y-Oy-K_arc*Sl2x)-r_arc*r_arc+Ox*Ox;
	                            
								Slx1=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sly1=Sl2y + K_arc*(Slx1-Sl2x);
								Slx2=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);   Sly2=Sl2y + K_arc*(Slx2-Sl2x);
							}//伸长线的交点坐标;

							   //if( ((S2x*X0+S2y*Y0)>=0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)<=0) || ((S2x*X0+S2y*Y0)<0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)>0) )
						   if( (((-I_two)*X0+(-J_two)*Y0)>=0 && ((Slx2-Slx1)*X0+(Sly2-Sly1)*Y0)<=0) || (((-I_two)*X0+(-J_two)*Y0)<=0 && ((Slx2-Slx1)*X0+(Sly2-Sly1)*Y0)>0) )
						   {
								Sly=Sly1;
								Slx=Slx1;
						   }
                            //else if( ((S2x*X0+S2y*Y0)>=0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)>0) || ((S2x*X0+S2y*Y0)<0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)<=0) )
						   else if( (((-I_two)*X0+(-J_two)*Y0)>=0 && ((Slx2-Slx1)*X0+(Sly2-Sly1)*Y0)>0) || (((-I_two)*X0+(-J_two)*Y0)<0 && ((Slx2-Slx1)*X0+(Sly2-Sly1)*Y0)<=0) )
							{
								Sly=Sly2;
								Slx=Slx2;
							}                              
							if(tape_class == 0)
							{                           
								asChild[*num]= *csChild;     //赋值,修正第一段直线
								asChild[*num].B = Su1x - (*Start_B);
								asChild[*num].C = Su1y - (*Start_C);
								asChild[*num].X = Slx - (*Start_X);
								asChild[*num].Y = Sly - (*Start_Y);
                           
								asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;  
								asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;
								
								asChild[*num].row_id=2;	
								
								*num=*num+1;//第一段直线;
                        		asChild[*num] = *csChild;     //赋值,修正        //下平面点，上平面圆弧
								
								asChild[*num].X = 0;           //下平面   
								asChild[*num].Y = 0;
								asChild[*num].I = 0;
								asChild[*num].J = 0;

								if(*first5152flag==1)
								{
									asChild[*num].B = Su2x - Su1x;         //上平面    
									asChild[*num].C = Su2y - Su1y;
									asChild[*num].I1 = 0.5*(ru1x);            
									asChild[*num].J1 = 0.5*(ru1y);
								}
								else
								{
									asChild[*num].B = Su2x - Su1x;         //上平面    
									asChild[*num].C = Su2y - Su1y;
									asChild[*num].I1 = -(ru0x);            
									asChild[*num].J1 = -(ru0y);
								}

								if(X0>0)
								{ 
									  tempx=X0+100;
									  tempy=Y0/X0*tempx;
								}
								else if(X0<0)
								{
									  tempx=X0-100;
									  tempy=Y0/X0*tempx;
									}
								else 
								{
									if(Y0>0)
									{
										tempx=0;
										tempy=Y0+100;
									}
									if(Y0<0)
									{
										tempx=0;
										tempy=Y0-100;
									}
								}
								Arc1=calculateLineArc(ru0x,ru0y);
								Arc2=calculateLineArc(tempx-X0,tempy-Y0);
								Arc3=calculateLineArc(ru1x,ru1y);
									
								asChild[*num].G_pathmode = Arcdirection(Arc1,Arc2,Arc3);
								asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
								asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;
							
							   *num=*num+1;//第二段插入;
							   
								*Start_X = Slx - X0;  
								*Start_Y = Sly - Y0; 

								*Start_B = ru1x;  
								*Start_C = ru1y;

								*first5152flag=0;

							   *csChild = *dataChild;
							}

							if(tape_class == 1)
							{ 
								asChild[*num]= *csChild;     //赋值,修正第一段直线

								if(*first5152flag==1)
							   {
								asChild[*num].B = Su1x - (*Start_B);
								asChild[*num].C = Su1y - (*Start_C);                                                                  				
								asChild[*num].X = Slx - (*Start_X);  //下平面
								asChild[*num].Y = Sly - (*Start_Y); 
							                           
								*num=*num+1;//第一段直线;

								*first5152flag = 0;
   				               
								*Start_B = Su1x - X0;  
								*Start_C = Su1y - Y0;				      
								*Start_X = Slx - X0;  
								*Start_Y = Sly - Y0; 

								*csChild = *dataChild;
							   }
							   else
							   {
								asChild[*num].B = Slx - rl0x + ru0x - (*Start_B);
								asChild[*num].C = Sly - rl0y + ru0y - (*Start_C);                                                                 				
								asChild[*num].X = Slx - (*Start_X);  //下平面
								asChild[*num].Y = Sly - (*Start_Y); 

								asChild[*num].row_id=2;	
								*num=*num+1;//第一段直线;
                        		asChild[*num] = *csChild;     //赋值,修正  

								I_two1 = I_two - (Slx - X0);
								J_two1 = J_two - (Sly - Y0);//圆心相对于(Sx,Sy)的坐标
								d11 = sqrt(I_two1*I_two1+J_two1*J_two1);

								if((csChild->G_tapemode == 51 && dataChild->G_pathmode == 2)||(csChild->G_tapemode == 52 && dataChild->G_pathmode == 3))
								{
									 r1y1 = (Ru1+Rl1)*(-J_two)/d11;
									 r1x1 = (Ru1+Rl1)*(-I_two)/d11;
								}
								else if((csChild->G_tapemode == 52 && dataChild->G_pathmode == 2)||(csChild->G_tapemode == 51 && dataChild->G_pathmode == 3))
								{
									r1y1 = -(Ru1+Rl1)*(-J_two)/d11;
									r1x1 = -(Ru1+Rl1)*(-I_two)/d11; 
								}

								asChild[*num].B = r1x1 - (ru0x - rl0x) ;  //上平面
								asChild[*num].C = r1y1 - (ru0y - rl0y);
								asChild[*num].I1 = rl0x - ru0x ;  //上平面
								asChild[*num].J1 = rl0y - ru0y ;

								asChild[*num].X = 0;  //下平面
								asChild[*num].Y = 0;
								asChild[*num].I = 0;  //下平面
								asChild[*num].J = 0;

								if(X0>0)
								{ 
									  tempx=X0+100;
									  tempy=Y0/X0*tempx;
								}
								else if(X0<0)
								{
									  tempx=X0-100;
									  tempy=Y0/X0*tempx;
								}
								else 
								{
									if(Y0>0)
									{
										tempx=0;
										tempy=Y0+100;
									}
									if(Y0<0)
									{
										tempx=0;
										tempy=Y0-100;
									}
								}
								Arc1=calculateLineArc(ru0x-rl0x,ru0y-rl0y);
								Arc2=calculateLineArc(tempx-(Slx-rl0x+ru0x),tempy-(Sly-rl0y+ru0y));
								Arc3=calculateLineArc(r1x1,r1y1);
									
								asChild[*num].G_pathmode = Arcdirection(Arc1,Arc2,Arc3);
								asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
								asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;

								*num=*num+1;

								*Start_X = Slx - X0;  
								*Start_Y = Sly - Y0; 
								*Start_B = *Start_X + r1x1;  
								*Start_C = *Start_Y + r1y1;

								*csChild = *dataChild;
								}
							}						
						}
					else if (abs(zeroErrorUp)<=ZERO&&abs(typeJudge_up-1.0)<ZERO)
					{
					    Sux=X0+ru0x;Suy=Y0+ru0y;
					    Slx=X0+rl0x;Sly=Y0+rl0y;
						asChild[*num]= *csChild;     //赋值,修正,第一段圆弧部分
                    
						asChild[*num].B = Sux - (*Start_B);
						asChild[*num].C = Suy - (*Start_C);                   
						asChild[*num].X = Slx - (*Start_X);
						asChild[*num].Y = Sly - (*Start_Y);

						*Start_B = ru1x;  
						*Start_C = ru1y;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;    
						*Start_X = rl1x;  
						*Start_Y = rl1y;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备；

						*num=*num+1;
						*csChild = *dataChild;
					  }
                      else	//伸长缩短			  
					  {
						Su1x=ru0x;    Su1y=ru0y;
						Su2x=X0+ru0x; Su2y=Y0+ru0y;//与圆弧相交的直线的两个点;

						Ox=X0+I_two;    Oy=Y0+J_two;//圆心坐标;
						r_arc=sqrt( (X0+ru1x-Ox)*(X0+ru1x-Ox) + (Y0+ru1y-Oy)*(Y0+ru1y-Oy) );//圆弧半径

						if(abs(Su1x - Su2x)<ZERO)
							{
								Sux1=Su1x ;	Suy1=Oy + sqrt(r_arc*r_arc-(Su1x-Ox)*(Su1x-Ox));
								Sux2=Su1x ;	Suy2=Oy - sqrt(r_arc*r_arc-(Su1x-Ox)*(Su1x-Ox));
							}
						else
						{
							K_arc = (Su2y-Su1y)/(Su2x-Su1x);
							a_arc = 1+K_arc*K_arc;
							b_arc = 2*K_arc*(Su2y-Oy-K_arc*Su2x)-2*Ox;
							c_arc =( Su2y-Oy-K_arc*Su2x)*(Su2y-Oy-K_arc*Su2x)-r_arc*r_arc+Ox*Ox;
	                         
							if((b_arc*b_arc-4*a_arc*c_arc<0)&&abs(b_arc*b_arc-4*a_arc*c_arc)<=ZERO)
							{
								Sux1=( -b_arc)/(2*a_arc);	Suy1=Su1y + K_arc*(Sux1-Su1x);								
								Sux2=( -b_arc)/(2*a_arc);	Suy2=Su1y + K_arc*(Sux2-Su1x); 
							}
							else
							{
							Sux1=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Suy1=Su1y + K_arc*(Sux1-Su1x);								
							Sux2=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Suy2=Su1y + K_arc*(Sux2-Su1x);
							}
						}
					 if( (((-I_two)*X0+(-J_two)*Y0)>=0 && ((Sux2-Sux1)*X0+(Suy2-Suy1)*Y0)<=0) || (((-I_two)*X0+(-J_two)*Y0)<0 && ((Sux2-Sux1)*X0+(Suy2-Suy1)*Y0)>0) )
					 {
						Suy=Suy1;
						Sux=Sux1;
					 }
					 else if( (((-I_two)*X0+(-J_two)*Y0)>=0 && ((Sux2-Sux1)*X0+(Suy2-Suy1)*Y0)>0) || (((-I_two)*X0+(-J_two)*Y0)<0 && ((Sux2-Sux1)*X0+(Suy2-Suy1)*Y0)<=0) )
					 {
						Suy=Suy2;
						Sux=Sux2;
					 }

					Sl1x=rl0x;    Sl1y=rl0y;
					Sl2x=X0+rl0x; Sl2y=Y0+rl0y;//与圆弧相交的直线的两个点;

					Ox=X0+I_two;    Oy=Y0+J_two;//圆心坐标;
					r_arc=sqrt( (X0+rl1x-Ox)*(X0+rl1x-Ox) + (Y0+rl1y-Oy)*(Y0+rl1y-Oy) );//圆弧半径

					if(abs(X0)<=ZERO)
					{
						if(abs(r_arc*r_arc-(Sl1x-Ox)*(Sl1x-Ox))<=ZERO)
						{
						   Slx1=Sl1x ;	Sly1=Oy ;
						   Slx2=Sl1x ;	Sly2=Oy ;
						}
						else
						{
							Slx1=Sl1x ;	Sly1=Oy + sqrt(r_arc*r_arc-(Sl1x-Ox)*(Sl1x-Ox));
							Slx2=Sl1x ;	Sly2=Oy - sqrt(r_arc*r_arc-(Sl1x-Ox)*(Sl1x-Ox));
						}
					}
					else
					{
						K_arc = (Sl2y-Sl1y)/(Sl2x-Sl1x);
						a_arc = 1+K_arc*K_arc;
						b_arc = 2*K_arc*(Sl2y-Oy-K_arc*Sl2x)-2*Ox;
						c_arc =( Sl2y-Oy-K_arc*Sl2x)*(Sl2y-Oy-K_arc*Sl2x)-r_arc*r_arc+Ox*Ox;
                            
						if((b_arc*b_arc-4*a_arc*c_arc<=0)&&abs(b_arc*b_arc-4*a_arc*c_arc)<=0.0001)
						{
						  Slx1=( -b_arc)/(2*a_arc);	Sly1=Sl1y + K_arc*(Slx1-Sl1x);
						  Slx2=( -b_arc)/(2*a_arc);	Sly2=Sl1y + K_arc*(Slx2-Sl1x);
						}
						else
						{
						  Slx1=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sly1=Sl1y + K_arc*(Slx1-Sl1x);
						  Slx2=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sly2=Sl1y + K_arc*(Slx2-Sl1x);
						}
					}  
				  if( (((-I_two)*X0+(-J_two)*Y0)>=0 && ((Slx2-Slx1)*X0+(Sly2-Sly1)*Y0)<=0) || (((-I_two)*X0+(-J_two)*Y0)<0 && ((Slx2-Slx1)*X0+(Sly2-Sly1)*Y0)>0) )
				  {
					Sly=Sly1;
					Slx=Slx1;
				  }
				  else if( (((-I_two)*X0+(-J_two)*Y0)>=0 && ((Slx2-Slx1)*X0+(Sly2-Sly1)*Y0)>0) || (((-I_two)*X0+(-J_two)*Y0)<0 && ((Slx2-Slx1)*X0+(Sly2-Sly1)*Y0)<=0) )
				  {
					Sly=Sly2;
					Slx=Slx2;
				  }                            
				  if(tape_class == 0)
				  {      
					asChild[*num]= *csChild;     //赋值,修正第一段直线

					asChild[*num].B = Sux - (*Start_B);
					asChild[*num].C = Suy - (*Start_C);

					asChild[*num].X = Slx - (*Start_X);
					asChild[*num].Y = Sly - (*Start_Y);
               
					*num=*num+1;//第一段直线;

					*Start_X =Slx-X0;  
					*Start_Y =Sly-Y0; 

					*Start_B =Sux-X0;  
					*Start_C =Suy-Y0;

					*first5152flag=0;

					*csChild = *dataChild;
				  }
				  else if(tape_class == 1)
				  {     
				   if   ((zeroErrorUp>ZERO && typeJudge_up>ANGLECONST&&zeroErrorLow<ZERO_NEG)
					   ||(abs(zeroErrorLow)<=ZERO&&abs(typeJudge_low+1)<ZERO&&((dataChild->G_tapemode == 51 && dataChild->G_pathmode == 3)|| (dataChild->G_tapemode == 52 && dataChild->G_pathmode == 2))))//上伸下缩
					{   
					  if(*first5152flag==1)
					  {
						asChild[*num]= *csChild;     //赋值,修正第一段直线

						asChild[*num].B = Sux - (*Start_B);
						asChild[*num].C = Suy - (*Start_C);
                                                                                     				
						asChild[*num].X = Slx - (*Start_X);  //下平面
						asChild[*num].Y = Sly - (*Start_Y); 

						*first5152flag = 0;
						*num=*num+1;

						*Start_X = Slx - X0;  
						*Start_Y = Sly - Y0; 

						*Start_B = Sux - X0;  
						*Start_C = Suy - Y0;

						*csChild = *dataChild;
					  }
					  else
					  {
						asChild[*num]= *csChild;     //赋值,修正第一段直线

						asChild[*num].B = Slx - rl0x + ru0x - (*Start_B);
						asChild[*num].C = Sly - rl0y + ru0y - (*Start_C);
                                                                                     				
						asChild[*num].X = Slx - (*Start_X);  //下平面
						asChild[*num].Y = Sly - (*Start_Y); 

						asChild[*num].row_id=2;	
						
						*num=*num+1;//第一段直线;
            			asChild[*num] = *csChild;     //赋值,修正  

						I_two1 = I_two - (Slx - X0);
						J_two1 = J_two - (Sly - Y0);//圆心相对于(Sx,Sy)的坐标

						d11 = sqrt(I_two1*I_two1+J_two1*J_two1);

						 if((csChild->G_tapemode == 51 && dataChild->G_pathmode == 2)||(csChild->G_tapemode == 52 && dataChild->G_pathmode == 3))
						 {
							 r1y1 = (Rl1+Ru1)*(-J_two)/d11;
							 r1x1 = (Rl1+Ru1)*(-I_two)/d11;
						 }
						  else if((csChild->G_tapemode == 52 && dataChild->G_pathmode == 2)||(csChild->G_tapemode == 51 && dataChild->G_pathmode == 3))
						  {
							r1y1 = -(Rl1+Ru1)*(-J_two)/d11;
							r1x1 = -(Rl1+Ru1)*(-I_two)/d11; 
						   }
						asChild[*num].B = r1x1 - (ru0x - rl0x) ;  //上平面
						asChild[*num].C = r1y1 - (ru0y - rl0y);
						asChild[*num].I1 = rl0x - ru0x ;  //上平面
						asChild[*num].J1 = rl0y - ru0y ;

						asChild[*num].X = 0;  //下平面
						asChild[*num].Y = 0;
						asChild[*num].I = 0;  //下平面
						asChild[*num].J = 0;

						Arc1=calculateLineArc(ru0x-rl0x,ru0y-rl0y);
						Arc2=calculateLineArc(Sux-Slx,Suy-Sly);
						Arc3=calculateLineArc(r1x1,r1y1);
							
						asChild[*num].G_pathmode = Arcdirection(Arc1,Arc2,Arc3);
						asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
						asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;

						*Start_X = Slx - X0;  
						*Start_Y = Sly - Y0; 
						*Start_B = *Start_X + r1x1;  
						*Start_C = *Start_Y + r1y1;
						
						*num=*num+1;
						*csChild = *dataChild;
					  }
				   }
				  if( (zeroErrorLow>ZERO && typeJudge_low>ANGLECONST&&zeroErrorUp<ZERO_NEG)
					  ||(abs(zeroErrorUp)<=ZERO&&abs(typeJudge_up+1)<ZERO&&((dataChild->G_tapemode == 51 && dataChild->G_pathmode == 2)|| (dataChild->G_tapemode == 52 && dataChild->G_pathmode == 3))) )//上缩下伸
					 {
					   asChild[*num]= *csChild;     //赋值,修正第一段直线

					   if(*first5152flag==1)
					   {
						asChild[*num].B = Sux - (*Start_B);
						asChild[*num].C = Suy - (*Start_C);                                                                                             				
						asChild[*num].X = Slx - (*Start_X);  //下平面
						asChild[*num].Y = Sly - (*Start_Y); 						                         

						*num=*num+1;//第一段直线;
						*first5152flag = 0;
		               
						*Start_B = Sux - X0;  
						*Start_C = Suy - Y0;				               
						*Start_X = Slx - X0;  
						*Start_Y = Sly - Y0; 

						*csChild = *dataChild;
					   }
					   else
					   {
						asChild[*num].B = Sux - (*Start_B);
						asChild[*num].C = Suy - (*Start_C);                                                                                            				
						asChild[*num].X = Sux - ru0x + rl0x - (*Start_X);  //下平面
						asChild[*num].Y = Suy - ru0y + rl0y - (*Start_Y); 

						asChild[*num].row_id=2;	
						
						*num=*num+1;//第一段直线;
                		asChild[*num] = *csChild;     //赋值,修正        
		               
						I_two1 = I_two - (Sux - X0);
						J_two1 = J_two - (Suy - Y0);//圆心相对于(Sx,Sy)的坐标

						d11 = sqrt(I_two1*I_two1+J_two1*J_two1);

						if((csChild->G_tapemode == 51 && dataChild->G_pathmode == 2)||(csChild->G_tapemode == 52 && dataChild->G_pathmode == 3))
						{
							 r1y1 = -(Ru1+Rl1)*(-J_two)/d1;
							 r1x1 = -(Ru1+Rl1)*(-I_two)/d1;
						 }
						else if((csChild->G_tapemode == 52 && dataChild->G_pathmode == 2)||(csChild->G_tapemode == 51 && dataChild->G_pathmode == 3))
						{
							r1y1 = (Ru1+Rl1)*(-J_two)/d1;
							r1x1 = (Ru1+Rl1)*(-I_two)/d1;
						}
						asChild[*num].X = r1x1 - (rl0x - ru0x) ;  //下平面伸长
						asChild[*num].Y = r1y1 - (rl0y - ru0y) ;
						asChild[*num].I = ru0x - rl0x ;  
						asChild[*num].J = ru0y - rl0y ;

						asChild[*num].B = 0 ;  //下平面伸长
						asChild[*num].C = 0 ;
						asChild[*num].I1 = 0 ;  
						asChild[*num].J1 = 0 ;

						Arc1=calculateLineArc(rl0x-ru0x,rl0y-ru0y);
						Arc2=calculateLineArc(Slx-Sux,Sly-Suy);
						Arc3=calculateLineArc(r1x1,r1y1);
							
						asChild[*num].G_pathmode = Arcdirection(Arc1,Arc2,Arc3);
						asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
						asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;
					  		
						*Start_B = Sux - X0;  
						*Start_C = Suy - Y0;			               
						*Start_X = *Start_B + r1x1;  
						*Start_Y = *Start_C + r1y1; 

                        *num=*num+1;
						*csChild = *dataChild;
					   }
				     }
					}
					}//伸长缩短型
                  
					if(dataChild->G_compensate == 50)       //取消刀补;
					{								
						MessageBox(hWnd,"ERROR:NOT ALLOW ESCAPE TOOL TAPE IN G02 OR G03 in g5152Fuction",NULL,NULL);
					}
					}//直线接圆弧  

				 else if((dataChild->G_pathmode==2 || dataChild->G_pathmode==3)&& (csChild->G_pathmode==2 || csChild->G_pathmode==3))//圆弧接圆弧
				 {
						X0 = csChild->X;
						Y0 = csChild->Y;
						I_one=csChild->I;
						J_one=csChild->J;

						X1 = dataChild->X;
						Y1 = dataChild->Y;
						I_two=dataChild->I;
						J_two=dataChild->J;

							d0 = sqrt((Y0-J_one)*(Y0-J_one)+(X0-I_one)*(X0-I_one)); 
							d1 = sqrt((Y1-J_two)*(Y1-J_two)+(X1-I_two)*(X1-I_two)); 
		                    
						if((csChild->G_tapemode == 51 && csChild->G_pathmode == 2)||(csChild->G_tapemode == 52 && csChild->G_pathmode == 3))
						{
							ru0y = Ru0*(Y0-J_one)/d0;
							ru0x = Ru0*(X0-I_one)/d0;
	               
							rl0y = (-Rl0)*(Y0-J_one)/d0;
							rl0x = (-Rl0)*(X0-I_one)/d0;   
						}
					   else if((csChild->G_tapemode == 52 && csChild->G_pathmode == 2)||(csChild->G_tapemode == 51 && csChild->G_pathmode == 3))
					   {
							ru0y = (-Ru0)*(Y0-J_one)/d0;
							ru0x = (-Ru0)*(X0-I_one)/d0;
	               
							rl0y = Rl0*(Y0-J_one)/d0;
							rl0x = Rl0*(X0-I_one)/d0;
						}
						if((dataChild->G_tapemode == 51 && dataChild->G_pathmode == 2)||(dataChild->G_tapemode == 52 && dataChild->G_pathmode == 3))
						{
							ru1y = Ru1*(-J_two)/d1;
							ru1x = Ru1*(-I_two)/d1;

							rl1y = (-Rl1)*(-J_two)/d1;
							rl1x = (-Rl1)*(-I_two)/d1;
						}
						else if((dataChild->G_tapemode == 52 && dataChild->G_pathmode == 2)||(dataChild->G_tapemode == 51 && dataChild->G_pathmode == 3))
						{
							ru1y = Ru1*J_two/d1;
							ru1x = Ru1*I_two/d1;

							rl1y = (-Rl1)*J_two/d1;
							rl1x = (-Rl1)*I_two/d1;
						}//确定r0y,r0x,r1y,r1x参量;
						
						if(csChild->G_pathmode == 2)
						{
							KBy0= -(X0-I_one);
							KBx0=Y0-J_one;
						}
						else if(csChild->G_pathmode == 3)
						{
							KBy0=X0-I_one;
							KBx0= -(Y0-J_one);
						}// 确定KB参量;
						if(dataChild->G_pathmode == 2)
						{
							KBy1= I_two;
							KBx1= -J_two;
						}
						else if(dataChild->G_pathmode == 3)
						{
							KBy1= -I_two;
							KBx1= J_two;
						}// 确定KB参量,圆弧起点处的切线;


					  if((sqrt(ru0y*ru0y + ru0x*ru0x)*sqrt(ru1x*ru1x + ru1y*ru1y))==0) 
					  {   
						  typeJudge_up=0;
					  }
					  else 
					  {
						  typeJudge_up=(ru0y*ru1y + ru0x*ru1x)/(sqrt(ru0y*ru0y + ru0x*ru0x)*sqrt(ru1x*ru1x + ru1y*ru1y));
					  }
					  if(sqrt(rl0y*rl0y + rl0x*rl0x)*sqrt(rl1x*rl1x + rl1y*rl1y)==0)
					  {
						  typeJudge_low=0;
					  }
					  else
					  {
						  typeJudge_low=(rl0y*rl1y + rl0x*rl1x)/(sqrt(rl0y*rl0y + rl0x*rl0x)*sqrt(rl1x*rl1x + rl1y*rl1y));
					  }
					   
					 zeroErrorUp = (ru1y-ru0y)*KBy0 + (ru1x-ru0x)*KBx0;
					 zeroErrorLow = (rl1y-rl0y)*KBy0 + (rl1x-rl0x)*KBx0;    

			 		if(zeroErrorUp<=0)
					{
						if(sqrt(I_two*I_two+J_two*J_two)<=Ru0)return;
					}
					else if (zeroErrorLow<=0)
					{
						if((I_two*I_two+J_two*J_two)<=Rl0)return;
					}
					if(zeroErrorLow>ZERO&&typeJudge_low> ANGLECONST)//判断直线与圆弧会不会不相交
					 {
					   Sx=X0;  Sy=Y0; 
                        r=sqrt( (X0+rl0x-I_one)*(X0+rl0x-I_one) + (Y0+rl0y-J_one)*(Y0+rl0y-J_one) );
					   if(abs(Y0+J_two-J_one)<ZERO)
					   {
						   d=abs(I_one-X0-rl0x);
					   }
					   else
					   {
                         K=-(X0+I_two-I_one)/(Y0+J_two-J_one);
						 d=abs(K*(I_one-Sx)-(J_one-Sy))/sqrt(1+K*K);
					   }
					   if(d>r)
					   {
                           typeJudge_low=-1;
					   }
					 }
					 else if(zeroErrorUp>ZERO&&typeJudge_up> ANGLECONST)
					 {                                                
					   Sx=X0;  Sy=Y0;
                        r=sqrt( (X0+ru0x-I_one)*(X0+ru0x-I_one) + (Y0+ru0y-J_one)*(Y0+ru0y-J_one) );
					   if(abs(Y0+J_two-J_one)<ZERO)
					   {
						   d=abs(I_one-X0-ru0x);
					   }
					   else
					   {
                         K=-(X0+I_two-I_one)/(Y0+J_two-J_one);
						 d=abs(K*(I_one-Sx)-(J_one-Sy))/sqrt(1+K*K);
					   }
					   if(d>r)
					   {
                           typeJudge_up=-1;
					   }
					 }
                    //判断交接类型:缩短,插入,伸长		 	 
                   if(((zeroErrorUp>ZERO && typeJudge_up<ANGLECONST)&&zeroErrorLow<ZERO_NEG )
					   ||(abs(zeroErrorUp)<=ZERO&&abs(typeJudge_up+1)<ZERO &&((dataChild->G_tapemode == 51 && dataChild->G_pathmode == 3)||(dataChild->G_tapemode == 52 && dataChild->G_pathmode == 2) ) )) 
                    //上表面插入下表面缩短型
				   {
                        Su1x= X0 + ru0x;  Su1y=Y0 + ru0y; //第一段线段终点处的半径矢量顶点坐标;
                        Su2x= X0 + ru1x;  Su2y=Y0 + ru1y;//第二段线起点处的半径矢量顶点坐标;
                                        //S1,S2两点的坐标;
                        Sl1x=X0; Sl1y=Y0;
                    
                        Ox=I_one;   Oy=J_one;//圆心坐标;
                        r_arc=sqrt( (X0+rl0x-I_one)*(X0+rl0x-I_one) + (Y0+rl0y-J_one)*(Y0+rl0y-J_one) );//圆弧半径
                        
                        if(abs(X0+I_two-Ox)<=ZERO)
                        {
							Sl2x = I_one;
							Sl2y = Sl1y;							
							Slx1=Ox + sqrt(r_arc*r_arc-(Sl2y-Oy)*(Sl2y-Oy));	Sly1=Sl2y;
							Slx2=Ox - sqrt(r_arc*r_arc-(Sl2y-Oy)*(Sl2y-Oy));	Sly2=Sl2y;

                        }
                        else if(abs(Y0+J_two-Oy)<=ZERO)            
                        {
							Sl2x = Sl1x; 
							Sl2y = J_one;								
							Slx1=Sl1x ;	Sly1=Oy - sqrt(r_arc*r_arc-(Sl2x-Ox)*(Sl2x-Ox));
							Slx2=Sl1x ;	Sly2=Oy + sqrt(r_arc*r_arc-(Sl2x-Ox)*(Sl2x-Ox));

                        }                                                                                                                                                                                                                      
                        else
                        {
							Kline=(Y0+J_two - Oy)/(X0+I_two - Ox);//过两圆心的直线斜率;
							//Sl2x=(Kline/(Kline*Kline+1))*(Y0-Oy+X0/Kline+Kline*Ox);//过两圆心的直线,
                                        //以第一个圆的圆心坐标为点坐标,计算
                                        //出与两圆交点连线的 交点;
                           // Sl2y=Kline*(Sl2x-Ox)+Oy;
						    Sl2y=Kline*(X0-Ox+Oy/Kline+Kline*Y0)/(1+Kline*Kline);
						    Sl2x=(Sl2y+Oy+Kline*Ox)/Kline;

							K_arc=(Sl2y-Sl1y)/(Sl2x-Sl1x);
							a_arc=1+K_arc*K_arc;
							b_arc=2*K_arc*(Sl1y-Oy-K_arc*Sl1y)-2*Ox;
							c_arc=(Sl2y-Oy-K_arc*Sl2x)*(Sl2y-Oy-K_arc*Sl2x)-r_arc*r_arc+Ox*Ox;
	                        
							Slx1=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sly1=Sl1y + K_arc*(Slx1-Sl1x);
							Slx2=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sly2=Sl1y + K_arc*(Slx2-Sl1x);
                        }              
                       //if( ((rl1x*KBx0+rl1y*KBy0)>=0 && ((Slx2-Slx1)*KBx0+(Sly2-Sly1)*KBy0)<0) || ((rl1x*KBx0+rl1y*KBy0)<0 && ((Slx2-Slx1)*KBx0+(Sly2-Sly1)*KBy0)>0) )
                        if( (((Slx2-Slx1)*(X0-I_one)+(Sly2-Sly1)*(Y0-J_one))>0 && ((Slx2-Slx1)*KBx0+(Sly2-Sly1)*KBy0)<0) || (((Slx2-Slx1)*(X0-I_one)+(Sly2-Sly1)*(Y0-J_one))<=0 && ((Slx2-Slx1)*KBx0+(Sly2-Sly1)*KBy0)>=0 ))    
						{
							Sly=Sly1;
							Slx=Slx1;
						}
                     // else if(((rl1x*KBx0+rl1y*KBy0)>=0 && ((Slx2-Slx1)*KBx0+(Sly2-Sly1)*KBy0)>=0)||((rl1x*KBx0+rl1y*KBy0)<0 && ((Slx2-Slx1)*KBx0+(Sly2-Sly1)*KBy0)<=0) )
                        else if( (((Slx2-Slx1)*(X0-I_one)+(Sly2-Sly1)*(Y0-J_one))>0 && ((Slx2-Slx1)*KBx0+(Sly2-Sly1)*KBy0)>=0) || (((Slx2-Slx1)*(X0-I_one)+(Sly2-Sly1)*(Y0-J_one))<=0 && ((Slx2-Slx1)*KBx0+(Sly2-Sly1)*KBy0)<0 ))
						{
							Sly=Sly2;
							Slx=Slx2;
						}
                            
                        if(tape_class==0)
						{
                            asChild[*num]= *csChild;     //赋值,修正,第一段圆弧部分
                        
                            asChild[*num].B = Su1x - (*Start_B);
                            asChild[*num].C = Su1y - (*Start_C);
                            asChild[*num].I1 = I_one-(*Start_B);
                            asChild[*num].J1 = J_one-(*Start_C);
                        
                            asChild[*num].X = Slx - (*Start_X);
                            asChild[*num].Y = Sly - (*Start_Y);
                            asChild[*num].I = I_one -(*Start_X);
                            asChild[*num].J = J_one -(*Start_Y);
                            
							asChild[*num].row_id=2;	
							
							*num=*num+1;
                            asChild[*num]= *csChild; 

							if(KBx0>0&&(Y0+J_two - Oy)>ZERO)
							{ 
							  Kline=-(X0+I_two-Ox)/(Y0+J_two-Oy);
							  tempx=X0+100;
							  tempy=Kline*tempx;
							}
							else if(KBx0<0&&(Y0+J_two - Oy)>ZERO)
							{
							  Kline=-(X0+I_two - Ox)/(Y0+J_two - Oy);
							  tempx=X0-100;
							  tempy=Kline*tempx;
							}
							else 
							{
								if(KBy0>0)
								{
									tempx=0;
									tempy=Y0+30;
								}
								if(KBy0<0)
								{
                                    tempx=0;
									tempy=Y0-30;
								}
							}
							Arc1=calculateLineArc(ru0x,ru0y);
							Arc2=calculateLineArc(tempx-X0,tempy-Y0);
							Arc3=calculateLineArc(ru1x,ru1y);
								
							asChild[*num].G_pathmode = Arcdirection(Arc1,Arc2,Arc3);
							asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
							asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;
                                    
							asChild[*num].B = Su2x-Su1x;
							asChild[*num].C = Su2y-Su1y;
							asChild[*num].I1 = -(ru0x);
							asChild[*num].J1 = -(ru0y);
	                    
							asChild[*num].X = 0;
							asChild[*num].Y = 0;
							asChild[*num].I = 0;
							asChild[*num].J = 0;
	                    
							 *num=*num+1;//插入圆弧部分;

							*Start_B = ru1x;  
							*Start_C = ru1y;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;    

							*Start_X = Slx - X0;  
							*Start_Y = Sly - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备；

							*csChild = *dataChild;
						}
						if(tape_class==1)
						{
						   asChild[*num]= *csChild;     //赋值,修正
                      
                            I_one1 = I_one - Slx;
                            J_one1 = J_one - Sly;//圆心相对于(Sx,Sy)的坐标
							d01 = sqrt(I_one1*I_one1+J_one1*J_one1);

							if((csChild->G_tapemode == 51 && csChild->G_pathmode == 2)||(csChild->G_tapemode == 52 && csChild->G_pathmode == 3))
							{              
								r0y1 = (Rl0+Ru0)*(Sly-J_one)/d01;
								r0x1 = (Rl0+Ru0)*(Slx-I_one)/d01;  
							}
						   else if((csChild->G_tapemode == 52 && csChild->G_pathmode == 2)||(csChild->G_tapemode == 51 && csChild->G_pathmode == 3))
						   {              
								r0y1 = -(Rl0+Ru0)*(Sly-J_one)/d01;
								r0x1 = -(Rl0+Ru0)*(Slx-I_one)/d01;       
							}

							asChild[*num].B = Slx + r0x1 -(*Start_B);
							asChild[*num].C = Sly + r0y1 -(*Start_C);
							asChild[*num].I1 = I_one-(*Start_B);
							asChild[*num].J1 = J_one-(*Start_C);

							asChild[*num].X = Slx - (*Start_X);
							asChild[*num].Y = Sly - (*Start_Y);
							asChild[*num].I = I_one -(*Start_X);
							asChild[*num].J = J_one -(*Start_Y);

							asChild[*num].row_id=2;	
							 
							*num=*num+1;//第一段圆弧;
							asChild[*num]= *csChild;
							
							I_two1 = I_two - (Slx - X0);
							J_two1 = J_two - (Sly - Y0);//圆心相对于(Sx,Sy)的坐标
							d11 = sqrt(I_two1*I_two1+J_two1*J_two1);

							if((dataChild->G_tapemode == 51 && dataChild->G_pathmode == 2)||(dataChild->G_tapemode == 52 && dataChild->G_pathmode == 3))
							{
								r1y1 = -(Rl1+Ru1)*(-J_two1)/d11;
								r1x1 = -(Rl1+Ru1)*(-I_two1)/d11;
							}
							else if((dataChild->G_tapemode == 52 && dataChild->G_pathmode == 2)||(dataChild->G_tapemode == 51 && dataChild->G_pathmode == 3))
							{
								r1y1 = (Rl1+Ru1)*J_two1/d11;
								r1x1 = (Rl1+Ru1)*I_two1/d11;
							}
							asChild[*num].B = r1x1 - r0x1;
							asChild[*num].C = r1y1 - r0y1;
							asChild[*num].I1 = -r0x1;
							asChild[*num].J1 = -r0y1;

							asChild[*num].X = 0;
							asChild[*num].Y = 0;
							asChild[*num].I = 0;
							asChild[*num].J = 0;
										
							if(KBx0>0&&abs(Y0+J_two-Oy)>ZERO)
							{ 
							  Kline=-(X0+I_two - Ox)/(Y0+J_two - Oy);
							  tempx=X0+100;
							  tempy=Kline*(tempx-X0)+Y0;
							}
							else if(KBx0<0&&(Y0+J_two-Oy)>ZERO)
							{
							  Kline=-(X0+I_two-Ox)/(Y0+J_two-Oy);
							  tempx=X0-100;
							  tempy=Kline*(tempx-X0)+Y0;
							}
							else 
							{
								if(KBy0>0)
								{
									tempx=0;
									tempy=Y0+100;
								}
								if(KBy0<0)
								{
                                    tempx=0;
									tempy=Y0-100;
								}
							}
							Arc1=calculateLineArc(r0x1,r0y1);
							Arc2=calculateLineArc(tempx-(Slx),tempy-(Sly));
							Arc3=calculateLineArc(r1x1,r1y1);
								
							asChild[*num].G_pathmode = Arcdirection(Arc1,Arc2,Arc3);
							asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
							asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;

							*Start_X = Slx - X0;  
							*Start_Y = Sly - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备；										
							*Start_B = *Start_X + r1x1; // 
							*Start_C = *Start_Y + r1y1;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;    

							*num = *num + 1;							
							*csChild = *dataChild;
						}
					 }
                    else if( (zeroErrorLow>ZERO && typeJudge_low < ANGLECONST&&zeroErrorUp<ZERO_NEG )
						|| (abs(zeroErrorLow)<ZERO&&(abs(typeJudge_low+1)<ZERO) &&((dataChild->G_tapemode == 51 && dataChild->G_pathmode == 2)||(dataChild->G_tapemode == 52 && dataChild->G_pathmode == 3) )  ) )
					{       //上缩下插      
                        Su1x=X0; Su1y=Y0;
                    
                        Ox=I_one;   Oy=J_one;//圆心坐标;
                        r_arc=sqrt( (X0+ru0x-I_one)*(X0+ru0x-I_one) + (Y0+ru0y-J_one)*(Y0+ru0y-J_one) );//圆弧半径
                        
                        if(abs(X0+I_two-Ox)<=ZERO)
                        {
							Su2x = I_one;
							Su2y = Su1y;

							Sux1=Ox + sqrt(r_arc*r_arc-(Su2y-Oy)*(Su2y-Oy));	Suy1=Su2y;
			                Sux2=Ox - sqrt(r_arc*r_arc-(Su2y-Oy)*(Su2y-Oy));	Suy2=Su2y;
                        }
                        else if(abs(Y0+J_two-Oy)<=ZERO)            
                        {
							Su2x = Su1x; 
							Su2y = J_one;

							Sux1=Su1x ;	Suy1=Oy - sqrt(r_arc*r_arc-(Su2x-Ox)*(Su2x-Ox));
							Sux2=Su1x ;	Suy2=Oy + sqrt(r_arc*r_arc-(Su2x-Ox)*(Su2x-Ox));
                        }                                                                                                                                                                                                                      
                        else
                        {
							Kline=(Y0+J_two - Oy)/(X0+I_two - Ox);//过两圆心的直线斜率;
							Su2y=Kline*(X0-Ox+Oy/Kline+Kline*Y0)/(1+Kline*Kline);
							Su2x=(Su2y+Oy+Kline*Ox)/Kline;

							K_arc=(Su2y-Su1y)/(Su2x-Su1x);
							a_arc=1+K_arc*K_arc;
							b_arc=2*K_arc*(Su2y-Oy-K_arc*Su2x)-2*Ox;
							c_arc=(Su2y-Oy-K_arc*Su2x)*(Su2y-Oy-K_arc*Su2x)-r_arc*r_arc+Ox*Ox;
		                        
							Sux1=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Suy1=Su1y + K_arc*(Sux1-Su1x);
							Sux2=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Suy2=Su1y + K_arc*(Sux2-Su1x);
                        }
					      //if( ((ru1x*KBx0+ru1y*KBy0)>=0 && ((Sux2-Sux1)*KBx0+(Suy2-Suy1)*KBy0)<=0) || ((ru1x*KBx0+ru1y*KBy0)<0 && ((Sux2-Sux1)*KBx0+(Suy2-Suy1)*KBy0)>0) )
						if( (((Sux2-Sux1)*(X0-I_one)+(Suy2-Suy1)*(Y0-J_one))>0 && ((Sux2-Sux1)*KBx0+(Suy2-Suy1)*KBy0)<0) || (((Sux2-Sux1)*(X0-I_one)+(Suy2-Suy1)*(Y0-J_one))<=0 && ((Sux2-Sux1)*KBx0+(Suy2-Suy1)*KBy0)>=0 ))                     
						{
							Suy=Suy1;
							Sux=Sux1;
						}
				          // else if(((ru1x*KBx0+ru1y*KBy0)>=0 && ((Sux2-Sux1)*KBx0+(Suy2-Suy1)*KBy0)>0)||((ru1x*KBx0+ru1y*KBy0)<0 && ((Sux2-Sux1)*KBx0+(Suy2-Suy1)*KBy0)<=0) )
					   else if( (((Sux2-Sux1)*(X0-I_one)+(Suy2-Suy1)*(Y0-J_one))>0 && ((Sux2-Sux1)*KBx0+(Suy2-Suy1)*KBy0)>=0) || (((Sux2-Sux1)*(X0-I_one)+(Suy2-Suy1)*(Y0-J_one))<=0 && ((Sux2-Sux1)*KBx0+(Suy2-Suy1)*KBy0)<0 ))            
					   {
							Suy=Suy2;
							Sux=Sux2;
						}

						Sl1x= X0 + rl0x;  Sl1y=Y0 + rl0y; //第一段线段终点处的半径矢量顶点坐标;//S1,S2两点的坐标;
						Sl2x= X0 + rl1x;  Sl2y=Y0 + rl1y;//第二段线起点处的半径矢量顶点坐标;
                           
						if(tape_class==0)//下平面插入型刀补
						{
                            asChild[*num]= *csChild;     //赋值,修正,第一段圆弧部分
                        
                            asChild[*num].B = Sux - (*Start_B);
                            asChild[*num].C = Suy - (*Start_C);
                            asChild[*num].I1 = I_one-(*Start_B);
                            asChild[*num].J1 = J_one-(*Start_C);
                        
                            asChild[*num].X = Sl1x - (*Start_X);
                            asChild[*num].Y = Sl1y - (*Start_Y);
                            asChild[*num].I = I_one-(*Start_X);
                            asChild[*num].J = J_one-(*Start_Y);

							asChild[*num].row_id=2;	
                           
							*num=*num+1;
                            asChild[*num]= *csChild;

							if(KBx0>0&&(Y0+J_two - Oy)>ZERO)
							{ 
							  Kline=-(X0+I_two - Ox)/(Y0+J_two - Oy);
							  tempx=X0+1;
							  tempy=Kline*tempx;
							}
							else if(KBx0<0&&(Y0+J_two - Oy)>ZERO)
							{
							  Kline=-(X0+I_two - Ox)/(Y0+J_two - Oy);
							  tempx=X0-1;
							  tempy=Kline*tempx;
							}
							else 
							{
								if(KBy0>0)
								{
									tempx=0;
									tempy=Y0+1;
								}
								if(KBy0<0)
								{
                                    tempx=0;
									tempy=Y0-1;
								}
							}
							Arc1=calculateLineArc(rl0x,rl0y);
							Arc2=calculateLineArc(tempx-X0,tempy-Y0);
							Arc3=calculateLineArc(rl1x,rl1y);
								
							asChild[*num].G_pathmode = Arcdirection(Arc1,Arc2,Arc3);
							asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
							asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;
                        
							 asChild[*num].X = Sl2x-Sl1x;
							 asChild[*num].Y = Sl2y-Sl1y;
							 asChild[*num].I = -(rl0x);
							 asChild[*num].J = -(rl0y);
	                        
							asChild[*num].B = 0;
							asChild[*num].C = 0;
							asChild[*num].I1 = 0;
							asChild[*num].J1 = 0;
	                        
							 *num=*num+1;//插入圆弧部分;

							*Start_X = rl1x;  
							*Start_Y = rl1y;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;    

							*Start_B = Sux - X0;  
							*Start_C = Suy - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备；

								*csChild = *dataChild;
						}
						  
						else if(tape_class==1)
						{                  
                            asChild[*num]= *csChild;     //赋值,修正,第一段圆弧部分
                        
                            I_one1 = I_one - Sux;
                            J_one1 = J_one - Suy;//圆心相对于(Sx,Sy)的坐标

							d01 = sqrt(I_one1*I_one1+J_one1*J_one1);

							if((csChild->G_tapemode == 51 && csChild->G_pathmode == 2)||(csChild->G_tapemode == 52 && csChild->G_pathmode == 3))
							{
								r0y1 = -(Ru0+Rl0)*(Suy-J_one)/d01;
								r0x1 = -(Rl0+Ru0)*(Sux-I_one)/d01;
							}
						   else if((csChild->G_tapemode == 52 && csChild->G_pathmode == 2)||(csChild->G_tapemode == 51 && csChild->G_pathmode == 3))
							{
								r0y1 = (Ru0+Rl0)*(Suy-J_one)/d01;
								r0x1 = (Ru0+Rl0)*(Sux-I_one)/d01;		                       
							}

                            asChild[*num].B = Sux - (*Start_B);
                            asChild[*num].C = Suy - (*Start_C);
                            asChild[*num].I1 = I_one-(*Start_B);
                            asChild[*num].J1 = J_one-(*Start_C);
                        
                            asChild[*num].X = Sux + r0x1 - (*Start_X);
                            asChild[*num].Y = Suy + r0y1 - (*Start_Y);
                            asChild[*num].I = I_one-(*Start_X);
                            asChild[*num].J = J_one-(*Start_Y);

                            asChild[*num].row_id=2;	

							*num=*num+1;
                            asChild[*num]= *csChild; 
			             		
							I_two1 = I_two - (Sux - X0);
							J_two1 = J_two - (Suy - Y0);//圆心相对于(Sx,Sy)的坐标
							d11 = sqrt(I_two1*I_two1+J_two1*J_two1);

							if((dataChild->G_tapemode == 51 && dataChild->G_pathmode == 2)||(dataChild->G_tapemode == 52 && dataChild->G_pathmode == 3))
							{
								r1y1 = -(Ru1+Rl1)*(-J_two1)/d11;
								r1x1 = -(Ru1+Rl1)*(-I_two1)/d11;
							}
							else if((dataChild->G_tapemode == 52 && dataChild->G_pathmode == 2)||(dataChild->G_tapemode == 51 && dataChild->G_pathmode == 3))
							{
								r1y1 = -(Ru1+Rl1)*J_two1/d11;
								r1x1 = -(Ru1+Rl1)*I_two1/d11;
							}
							asChild[*num].X = r1x1 - r0x1;
							asChild[*num].Y = r1y1 - r0y1;
							asChild[*num].I = -r0x1;
							asChild[*num].J = -r0y1;

							asChild[*num].B = 0;
							asChild[*num].C = 0;
							asChild[*num].I1 = 0;
							asChild[*num].J1 = 0;

							if(KBx0>0&&(Y0+J_two-Oy)>ZERO)
							{ 
							  Kline=-(X0+I_two-Ox)/(Y0+J_two-Oy);
							  tempx=X0+100;
							  tempy=Kline*tempx;
							}
							else if(KBx0<0&&(Y0+J_two-Oy)>ZERO)
							{
							  Kline=-(X0+I_two-Ox)/(Y0+J_two-Oy);
							  tempx=X0-100;
							  tempy=Kline*tempx;
							}
							else 
							{
								if(KBy0>0)
								{
									tempx=0;
									tempy=Y0+100;
								}
								if(KBy0<0)
								{
                                    tempx=0;
									tempy=Y0-100;
								}
							}
							Arc1=calculateLineArc(r0x1,r0y1);
							Arc2=calculateLineArc(tempx-(Sux),tempy-(Suy));
							Arc3=calculateLineArc(r1x1,r1y1);
								
							asChild[*num].G_pathmode = Arcdirection(Arc1,Arc2,Arc3);
							asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
							asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;

							*Start_B = Sux - X0;  
							*Start_C = Suy - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备；																	
							*Start_X = *Start_B + r1x1; // 
							*Start_Y = *Start_C + r1y1;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;  

							*num = *num + 1;
							*csChild = *dataChild;
						  }
				}//上缩下插
                else if (abs(ru1x*ru0y-ru0x*ru1y)<ZERO&&abs(typeJudge_up-1)<ZERO)
				{
			           Sux=X0+ru0x;Suy=Y0+ru0y;
					   Slx=X0+rl0x;Sly=Y0+rl0y;

                        asChild[*num]= *csChild;     //赋值,修正,第一段圆弧部分
                    
                        asChild[*num].B = Sux - (*Start_B);
                        asChild[*num].C = Suy - (*Start_C);
                        asChild[*num].I1 = I_one-(*Start_B);
                        asChild[*num].J1 = J_one-(*Start_C);
                    
                        asChild[*num].X = Slx - (*Start_X);
                        asChild[*num].Y = Sly - (*Start_Y);
                        asChild[*num].I = I_one -(*Start_X);
                        asChild[*num].J = J_one -(*Start_Y);

						asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
						asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;
                            
						*Start_B = ru1x;  
						*Start_C = ru1y;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;    
						*Start_X = rl1x;  
						*Start_Y = rl1y;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备；
						*num=*num+1;
						*csChild = *dataChild;
					}
                else   
				{	 
				        Su1x=X0; Su1y=Y0;
                    
						Ox=I_one;   Oy=J_one;//圆心坐标;
						r_arc=sqrt( (X0+ru0x-I_one)*(X0+ru0x-I_one) + (Y0+ru0y-J_one)*(Y0+ru0y-J_one) );//圆弧半径   
						if(abs(X0+I_two-Ox) <=ZERO)
						{
							Su2x = I_one;
							Su2y = Su1y;
							Sux1=Ox + sqrt(r_arc*r_arc - (Su2y-Oy)*(Su2y-Oy));	Suy1=Su2y;
							Sux2=Ox - sqrt(r_arc*r_arc - (Su2y-Oy)*(Su2y-Oy));	Suy2=Su2y;
						}
						else if(abs(Y0+J_two-Oy)<=ZERO)            
						{
							Su2x = Su1x; 
							Su2y = J_one;
							Sux1=Su1x ;	Suy1=Oy - sqrt(r_arc*r_arc - (Su2x-Ox)*(Su2x-Ox));
							Sux2=Su1x ;	Suy2=Oy + sqrt(r_arc*r_arc - (Su2x-Ox)*(Su2x-Ox));
						}                                                                                                                                                                                                                      
						else
						{
							Kline=(Y0+J_two - Oy)/(X0+I_two - Ox);//过两圆心的直线斜率;
							Su2y=Kline*(X0-Ox+Oy/Kline+Kline*Y0)/(1+Kline*Kline);
							Su2x=(Su2y+Oy+Kline*Ox)/Kline;
				
							K_arc=(Su2y-Su1y)/(Su2x-Su1x);
							a_arc=1+K_arc*K_arc;
							b_arc=2*K_arc*(Su1y-Oy-K_arc*Su1x)-2*Ox;
							c_arc=(Su1y-Oy-K_arc*Su1x)*(Su1y-Oy-K_arc*Su1x)-r_arc*r_arc+Ox*Ox;
											
							Sux1=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Suy1=Su1y + K_arc*(Sux1-Su1x);
							Sux2=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Suy2=Su1y + K_arc*(Sux2-Su1x);
						}	                       
					   //if( ((ru1x*KBx0+ru1y*KBy0)>=0 && ((Sux2-Sux1)*KBx0+(Suy2-Suy1)*KBy0)<0) || ((ru1x*KBx0+ru1y*KBy0)<0 && ((Sux2-Sux1)*KBx0+(Suy2-Suy1)*KBy0)>0) )
						if( ((KBx0*(X0-I_one)+KBy0*(Y0-J_one))>=0 && ((Sux2-Sux1)*KBx0+(Suy2-Suy1)*KBy0)<=0) || ((KBx0*(X0-I_one)+KBy0*(Y0-J_one))<=0 && ((Sux2-Sux1)*KBx0+(Suy2-Suy1)*KBy0)>=0 ))
						{
							Suy=Suy1;
							Sux=Sux1;
						}
					 //else if(((ru1x*KBx0+ru1y*KBy0)>=0 && ((Sux2-Sux1)*KBx0+(Suy2-Suy1)*KBy0)>=0)||((ru1x*KBx0+ru1y*KBy0)<=0 && ((Sux2-Sux1)*KBx0+(Suy2-Suy1)*KBy0)<=0) )
						else if( ((KBx0*(X0-I_one)+KBy0*(Y0-J_one))>0 && ((Sux2-Sux1)*KBx0+(Suy2-Suy1)*KBy0)>0) || ((KBx0*(X0-I_one)+KBy0*(Y0-J_one))<=0 && ((Sux2-Sux1)*KBx0+(Suy2-Suy1)*KBy0)<0 ))
						{
							Suy=Suy2;
							Sux=Sux2;
						}	  
							
				        Sl1x=X0; Sl1y=Y0;         
						Ox=I_one;   Oy=J_one;//圆心坐标;
						r_arc=sqrt( (X0+rl0x-I_one)*(X0+rl0x-I_one) + (Y0+rl0y-J_one)*(Y0+rl0y-J_one) );//圆弧半径
                        
						if(abs(X0+I_two-Ox)<= ZERO)
						{
							Sl2x = I_one;
							Sl2y = Sl1y;

							Slx1=Ox + sqrt(r_arc*r_arc-(Sl2y-Oy)*(Sl2y-Oy));	Sly1=Sl2y;
		                    Slx2=Ox - sqrt(r_arc*r_arc-(Sl2y-Oy)*(Sl2y-Oy));	Sly2=Sl2y;
						}
						else if(abs(Y0+J_two-Oy)<=ZERO)            
						{
							Sl2x = Sl1x; 
							Sl2y = J_one;

							Slx1=Sl1x ;	Sly1=Oy - sqrt(r_arc*r_arc-(Sl2x-Ox)*(Sl2x-Ox));
							Slx2=Sl1x ;	Sly2=Oy + sqrt(r_arc*r_arc-(Sl2x-Ox)*(Sl2x-Ox));
						}                                                                                                                                                                                                                      
						else
						{
							Kline=(Y0+J_two - Oy)/(X0+I_two - Ox);//过两圆心的直线斜率;
							Sl2y=Kline*(X0-Ox+Oy/Kline+Kline*Y0)/(1+Kline*Kline);
							Sl2x=(Su2y+Oy+Kline*Ox)/Kline;

							K_arc=(Sl2y-Sl1y)/(Sl2x-Sl1x);
							a_arc=1+K_arc*K_arc;
							b_arc=2*K_arc*(Sl1y-Oy-K_arc*Sl1x)-2*Ox;
							c_arc=(Sl1y-Oy-K_arc*Sl1x)*(Sl1y-Oy-K_arc*Sl1x)-r_arc*r_arc+Ox*Ox;
	                        
							Slx1=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sly1=Sl1y + K_arc*(Slx1-Sl1x);
							Slx2=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sly2=Sl1y + K_arc*(Slx2-Sl1x);
						}
					   //if( ((rl1x*KBx0+rl1y*KBy0)>=0 && ((Slx2-Slx1)*KBx0+(Sly2-Sly1)*KBy0)<0) || ((rl1x*KBx0+rl1y*KBy0)<0 && ((Slx2-Slx1)*KBx0+(Sly2-Sly1)*KBy0)>0) )
						if( ((KBx0*(X0-I_one)+KBy0*(Y0-J_one))>=0 && ((Slx2-Slx1)*KBx0+(Sly2-Sly1)*KBy0)<=0) || ((KBx0*(X0-I_one)+KBy0*(Y0-J_one))<=0 && ((Slx2-Slx1)*KBx0+(Sly2-Sly1)*KBy0)>=0 ))
						{
							Sly=Sly1;
							Slx=Slx1;
						}
					   //else if(((rl1x*KBx0+rl1y*KBy0)>=0 && ((Slx2-Slx1)*KBx0+(Sly2-Sly1)*KBy0)>=0)||((rl1x*KBx0+rl1y*KBy0)<=0 && ((Slx2-Slx1)*KBx0+(Sly2-Sly1)*KBy0)<=0) )
					   else if( ((KBx0*(X0-I_one)+KBy0*(Y0-J_one))>0 && ((Slx2-Slx1)*KBx0+(Sly2-Sly1)*KBy0)>0) || ((KBx0*(X0-I_one)+KBy0*(Y0-J_one))<=0 && ((Slx2-Slx1)*KBx0+(Sly2-Sly1)*KBy0)<0 ))	
					    {
							Sly=Sly2;
							Slx=Slx2;
						}//  

					 if(tape_class==0)//尖锥度
					 {
						 asChild[*num]= *csChild;     //赋值,修正    上平面插入直线，下平面一点
                      
						asChild[*num].B  = Sux-(*Start_B);
						asChild[*num].C  = Suy-(*Start_C);
						asChild[*num].I1 = I_one-(*Start_B);
						asChild[*num].J1 = J_one-(*Start_C);

						asChild[*num].X = Slx - (*Start_X);
						asChild[*num].Y = Sly - (*Start_Y);
						asChild[*num].I = I_one-(*Start_X);
						asChild[*num].J = J_one-(*Start_Y);
                        
						 *num=*num+1;//第一段圆弧;
          
						*Start_B = Sux - X0;  
						*Start_C = Suy - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;    

						*Start_X = Slx - X0;  
						*Start_Y = Sly - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备；


						*csChild = *dataChild;
						}
                        
                     if(tape_class==1)//恒锥度
					 {  
                       if((zeroErrorUp>ZERO &&typeJudge_up>ANGLECONST&&zeroErrorLow<ZERO_NEG)
						   ||(abs(zeroErrorLow)<ZERO&&(abs(typeJudge_low+1)<ZERO) &&((dataChild->G_tapemode == 51 && dataChild->G_pathmode == 3)||(dataChild->G_tapemode == 52 && dataChild->G_pathmode == 2) )  ))  //上表面伸长下表面缩短型
					   {
						   asChild[*num]= *csChild;     //赋值,修正
                      
                            I_one1 = I_one - Slx;
                            J_one1 = J_one - Sly;//圆心相对于(Sx,Sy)的坐标
							d01 = sqrt(I_one1*I_one1+J_one1*J_one1);

							if((csChild->G_tapemode == 51 && csChild->G_pathmode == 2)||(csChild->G_tapemode == 52 && csChild->G_pathmode == 3))
							{
								r0y1 = (Rl0+Ru0)*(Sly-J_one)/d01;
								r0x1 = (Rl0+Ru0)*(Slx-I_one)/d01;
							}
							else if((csChild->G_tapemode == 52 && csChild->G_pathmode == 2)||(csChild->G_tapemode == 51 && csChild->G_pathmode == 3))
							{
								r0y1 = -(Ru0+Rl0)*(Sly-J_one)/d01;
								r0x1 = -(Ru0+Rl0)*(Slx-I_one)/d01;  
							}
							asChild[*num].B = Slx + r0x1 -(*Start_B);
							asChild[*num].C = Sly + r0y1 -(*Start_C);
							asChild[*num].I1 = I_one-(*Start_B);
							asChild[*num].J1 = J_one-(*Start_C);

							asChild[*num].X = Slx - (*Start_X);
							asChild[*num].Y = Sly - (*Start_Y);
							asChild[*num].I = I_one -(*Start_X);
							asChild[*num].J = J_one -(*Start_Y);

							asChild[*num].row_id=2;	
							
							*num=*num+1;//第一段圆弧;
							asChild[*num]= *csChild;
							
							I_two1 = I_two - (Slx - X0);
							J_two1 = J_two - (Sly - Y0);//圆心相对于(Sx,Sy)的坐标
							d11 = sqrt(I_two1*I_two1+J_two1*J_two1);

							if((dataChild->G_tapemode == 51 && dataChild->G_pathmode == 2)||(dataChild->G_tapemode == 52 && dataChild->G_pathmode == 3))
							{
									r1y1 = (Rl1+Ru1)*(-J_two1)/d11;
									r1x1 = (Rl1+Ru1)*(-I_two1)/d11;
							}
							else if((dataChild->G_tapemode == 52 && dataChild->G_pathmode == 2)||(dataChild->G_tapemode == 51 && dataChild->G_pathmode == 3))
							{
									r1y1 = (Ru1+Rl1)*J_two1/d11;
									r1x1 = (Ru1+Rl1)*I_two1/d11;
							}
							asChild[*num].B = r1x1 - r0x1;
							asChild[*num].C = r1y1 - r0y1;
							asChild[*num].I1 = -r0x1;
							asChild[*num].J1 = -r0y1;

							asChild[*num].X = 0;
							asChild[*num].Y = 0;
							asChild[*num].I = 0;
							asChild[*num].J = 0;

							Arc1=calculateLineArc(r0x1,r0y1);
							Arc2=calculateLineArc(Sux-Slx,Suy-Sly);
							Arc3=calculateLineArc(r1x1,r1y1);
									
							asChild[*num].G_pathmode = Arcdirection(Arc1,Arc2,Arc3);
							asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
							asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;

							*Start_X = Slx - X0;  
							*Start_Y = Sly - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备；									
							
							*Start_B = *Start_X + r1x1; // 
							*Start_C = *Start_Y + r1y1;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备; 

							*num = *num + 1;
							*csChild = *dataChild;
					   }
                 else if ( (zeroErrorLow>ZERO && typeJudge_low>ANGLECONST&&zeroErrorUp<ZERO_NEG)
					 ||(abs(zeroErrorUp)<ZERO&&(abs(typeJudge_up+1)<ZERO) &&((dataChild->G_tapemode == 51 && dataChild->G_pathmode == 2)||(dataChild->G_tapemode == 52 && dataChild->G_pathmode == 3)))) //下伸上缩
				 {	
							asChild[*num]= *csChild;

						    I_one1 = I_one - Sux;
                            J_one1 = J_one - Suy;//圆心相对于(Sx,Sy)的坐标
							d01 = sqrt(I_one1*I_one1+J_one1*J_one1);

							if((csChild->G_tapemode == 51 && csChild->G_pathmode == 2)||(csChild->G_tapemode == 52 && csChild->G_pathmode == 3))
							{
								r0y1 = -(Ru0+Rl0)*(Suy-J_one)/d01;
								r0x1 = -(Ru0+Rl0)*(Sux-I_one)/d01;				                      
							}
						   else if((csChild->G_tapemode == 52 && csChild->G_pathmode == 2)||(csChild->G_tapemode == 51 && csChild->G_pathmode == 3))
							{
								r0y1 = (Ru0+Rl0)*(Suy-J_one)/d01;
								r0x1 = (Ru0+Rl0)*(Sux-I_one)/d01;				                       
							}

							asChild[*num].B = Sux - (*Start_B);
							asChild[*num].C = Suy - (*Start_C);
							asChild[*num].I1 = I_one - (*Start_B);
							asChild[*num].J1 = J_one - (*Start_C);

							asChild[*num].X = Sux + r0x1 - (*Start_X);
							asChild[*num].Y = Suy + r0y1 - (*Start_Y);
							asChild[*num].I = I_one - (*Start_X);
							asChild[*num].J = J_one - (*Start_Y);
            
							asChild[*num].row_id=2;	

							*num=*num+1;//第一段圆弧;
							asChild[*num]= *csChild;

							I_two1 = I_two - (Sux - X0);
							J_two1 = J_two - (Suy - Y0);//圆心相对于(Sx,Sy)的坐标

							d11 = sqrt(I_two1*I_two1+J_two1*J_two1);

							if((dataChild->G_tapemode == 51 && dataChild->G_pathmode == 2)||(dataChild->G_tapemode == 52 && dataChild->G_pathmode == 3))
							{
								r1y1 = -(Ru1+Rl1)*(-J_two1)/d11;
								r1x1 = -(Ru1+Rl1)*(-I_two1)/d11;
							}
							else if((dataChild->G_tapemode == 52 && dataChild->G_pathmode == 2)||(dataChild->G_tapemode == 51 && dataChild->G_pathmode == 3))
							{
								r1y1 = -(Ru1+Rl1)*J_two1/d11;
								r1x1 = -(Ru1+Rl1)*I_two1/d11;
							}
							asChild[*num].B = 0;
							asChild[*num].C = 0;
							asChild[*num].I1 = 0;
							asChild[*num].J1 = 0;

							asChild[*num].X = r1x1 - r0x1;
							asChild[*num].Y = r1y1 - r0y1;
							asChild[*num].I = -r0x1;
							asChild[*num].J = -r0y1;

							Arc1=calculateLineArc(r0x1,r0y1);
							Arc2=calculateLineArc(Slx-Sux,Sly-Suy);
							Arc3=calculateLineArc(r1x1,r1y1);

							asChild[*num].G_pathmode = Arcdirection(Arc1,Arc2,Arc3);
							asChild[*num].G_pathmode_upper = asChild[*num].G_pathmode;
							asChild[*num].G_pathmode_lower = asChild[*num].G_pathmode;
					
							*Start_B = Sux - X0;  
							*Start_C = Suy - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备；													
							*Start_X = *Start_B + r1x1; // 
							*Start_Y = *Start_C + r1y1;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;    
							*num = *num + 1;
							*csChild = *dataChild;
						}
					 }
				}

                        if(dataChild->G_compensate == 50)       //取消刀补;
                        {
							MessageBox(hWnd,"ERROR:NOT ALLOW ESCAPE TOOL TAPE IN G02 OR G03 in g5152Fuction",NULL,NULL);
						}
						
		  }//圆弧接圆弧
    
		}//建立刀补后 if(c != 0)

			   else if(dataChild->G_tapemode != 50)
			   {
					*csChild = *dataChild;
					*c = 1;
					*first5152flag=1;
				}
}

int Arcdirection(double Arc1,double Arc2,double Arc3)
{
		int G_pathmode;
		double Arc12,Arc13;
		G_pathmode=1;
          if(Arc2>Arc1)
			{
				Arc2=Arc2-2*PI;
			}
			if(Arc3>Arc1)
			{
				Arc3=Arc3-2*PI;
			}
			Arc12=Arc1-Arc2;
			Arc13=Arc1-Arc3;
			if(Arc12>Arc13)
			{
				G_pathmode = 3;
			}
			else if(Arc12<Arc13)
			{
				G_pathmode = 2;
			}
			return  G_pathmode;
}

