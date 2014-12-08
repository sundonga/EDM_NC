#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"
extern HINSTANCE hInst;	
extern int Arcdirection(double Arc1,double Arc2,double Arc3);
extern double calculateLineArc(double PointX,double PointY);//计算直线与X轴正方向逆时针夹角
extern TapeParam tapepm;  //锥度加工参数
extern double ZERO;
extern double ZERO_ERROR;
extern double ZERO_NEG;
/******************************************************************************************/
void LineshortenFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double X1,
						double Y1,double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y);

void LineextendFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double X1,
					   double Y1,double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y);

int calculateLineinsertdirection(double Sx,double Sy,double S1x,double S1y,double S2x,double S2y,double X0,double Y0);

void CircleToLineshortenFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_one,
								double J_one,double X1,double Y1,double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y);

void CircleToLinetangentialFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_one,double J_one,
								   double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y);

void CircleToLineextendFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_one,double J_one,
							   double X1,double Y1,double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y);

int calculateCircleToLineinsertdirection(double Sx,double Sy,double S1x,double S1y,double S2x,double S2y,double X0,double Y0);

void LineToCircleshortenFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_two,double J_two,
								double X1,double Y1,double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y);

void LineToCircletangentialFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_two,double J_two,
								   double X1,double Y1,double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y);

void LineToCircleextendFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_two,double J_two,
							   double X1,double Y1,double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y);

int calculateLineToCircleinsertdirection(double S1x,double S1y,double S2x,double S2y,double X0,double Y0);

void CircleToCircleshortenFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_one,double J_one,
								  double X1,double Y1,double I_two,double J_two,double r0x,double r0y,double r1x,double r1y,double KBx0,double KBy0,double *Start_X, double *Start_Y);

void CircleToCircletangentialFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_one,double J_one,
									 double X1,double Y1,double I_two,double J_two,double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y);

void CircleToCircleextendFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_one,double J_one,
								 double X1,double Y1,double I_two,double J_two,double r0x,double r0y,double r1x,double r1y,double KBx0,double KBy0,double *Start_X, double *Start_Y);

int calculateCircleToCircleinsertdirection(double S1x,double S1y,double S2x,double S2y,double KBx0,
										   double KBy0,double X0,double Y0,double I_two,double J_two,double Ox,double Oy);
/******************************************************************************************/
void LineshortenFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,
						double X1,double Y1,double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y)
{//伸长型和缩短型程序一样,可以合并为一段程序*** 
	double S1x,S1y,S2x,S2y,Sx,Sy;

    if(abs((Y1*X0)-(Y0*X1))<ZERO)
    {
		S1x= r0x;
		S1y=r0y; 

		S2x= X0+X1+r1x;
		S2y=Y0+Y1+r1y;
                                   
		Sx = X0+r0x;
		Sy = Y0+r0y;
    }   
    else
    {
		S1x = r0x;
		S1y = r0y; //第一段线段起点处的半径矢量顶点坐标;

		S2x = X0+X1+r1x;
		S2y = Y0+Y1+r1y; //第二段线终点处的半径矢量顶点坐标;
    
		Sx = ((S2y-S1y)*X0*X1+Y0*X1*S1x-Y1*X0*S2x)/(Y0*X1-X0*Y1);
		Sy =-((S2x-S1x)*Y0*Y1+X0*Y1*S1y-X1*Y0*S2y)/(Y0*X1-X0*Y1);
    }//伸长线的交点坐标;

    asChild[*num] = *csChild;     //赋值,修正
	if(csChild->G_plane == 18)
    {
        asChild[*num].Z = Sx - (*Start_X);
        asChild[*num].X = Sy - (*Start_Y);
    }   
    else if(csChild->G_plane == 19)
    {
        asChild[*num].Y = Sx - (*Start_X);
        asChild[*num].Z = Sy - (*Start_Y);
    }
	else
    {
        asChild[*num].X = Sx - (*Start_X);
        asChild[*num].Y = Sy - (*Start_Y);
    }
	*num=*num+1;
    *Start_X = Sx - X0;  
    *Start_Y = Sy - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
}
/******************************************************************************************/
void LineextendFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double X1,double Y1,
					   double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y)
{	
    double S1x,S1y,S2x,S2y;
	double S1x1,S1y1,S2x1,S2y1,Sx,Sy;

	S1x = X0 + r0x;
	S1y =Y0 + r0y; //第一段线段终点处的半径矢量顶点坐标;	//S1,S2两点的坐标;

	S2x = X0 + r1x;
	S2y =Y0 + r1y;//第二段线起点处的半径矢量顶点坐标;
	
    if(abs((Y1*X0)-(Y0*X1))<ZERO)
    {                              
		Sx = X0+r0x;
		Sy = Y0+r0y;
    }   
    else
    {
		S1x1 = r0x;   
		S1y1 = r0y; //第一段线段起点处的半径矢量顶点坐标;

		S2x1 = X0+X1+r1x; 
		S2y1 = Y0+Y1+r1y; //第二段线终点处的半径矢量顶点坐标;
    
		Sx = ((S2y-S1y)*X0*X1+Y0*X1*S1x-Y1*X0*S2x)/(Y0*X1-X0*Y1);
		Sy =-((S2x-S1x)*Y0*Y1+X0*Y1*S1y-X1*Y0*S2y)/(Y0*X1-X0*Y1);
    }//伸长线的交点坐标;

	asChild[*num]= *csChild;     //赋值,

	if(csChild->G_plane == 18)
	{
		asChild[*num].Z = S1x - (*Start_X);
		asChild[*num].X = S1y - (*Start_Y);
		asChild[*num].row_id=2;
	}	
	else if(csChild->G_plane == 19)
	{
		asChild[*num].Y = S1x - (*Start_X);
		asChild[*num].Z = S1y - (*Start_Y);
		asChild[*num].row_id=2;
	}
	else
	{
		asChild[*num].X = S1x - (*Start_X);
		asChild[*num].Y = S1y - (*Start_Y);
		asChild[*num].row_id=2;
	}
	*num=*num+1;
	asChild[*num]= *csChild;     //插入段修正
	if(csChild->G_plane == 18)
	{
		asChild[*num].Z = S2x - S1x;
		asChild[*num].X = S2y - S1y;
		asChild[*num].K = -r0x;
		asChild[*num].I = -r0y;
	}
	
	else if(csChild->G_plane == 19)
	{
		asChild[*num].Y = S2x - S1x;
		asChild[*num].Z = S2y - S1y;
		asChild[*num].J = -r0x;
		asChild[*num].K = -r0y;
	}
	else
	{
		asChild[*num].X = S2x - S1x;
		asChild[*num].Y = S2y - S1y;
		asChild[*num].I = -r0x;
		asChild[*num].J = -r0y;
	}
   asChild[*num].G_pathmode=calculateLineinsertdirection(Sx,Sy,S1x1,S1y1,S2x1,S2y1,X0,Y0);
   asChild[*num].G_pathmode_upper=asChild[*num].G_pathmode;
   asChild[*num].G_pathmode_lower=asChild[*num].G_pathmode;
    *num=*num+1;
	*Start_X = r1x; 
	*Start_Y = r1y;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
}//插入 
/******************************************************************************************/
int calculateLineinsertdirection(double Sx,double Sy,double S1x,double S1y,double S2x,double S2y,double X0,double Y0) 
{
	double Arc1,Arc2,Arc3;
	int direction;
	Arc1=calculateLineArc(S1x-X0,S1y-Y0);
	Arc2=calculateLineArc(Sx-X0,Sy-Y0);
	Arc3=calculateLineArc(S2x-X0,S2y-Y0);
	direction = Arcdirection(Arc1,Arc2,Arc3);
    return direction;
}
/******************************************************************************************/
void CircleToLineshortenFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_one,
								double J_one,double X1,double Y1,double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y)
{    // 圆弧接直线，缩短性刀补if( typeJudge >= ANGLECONST) && (((r1y-r0y)*KBy0 + (r1x-r0x)*KBx0) >= 0) )//伸长型
	double Ox,Oy,r_arc,K_arc,a_arc,b_arc,c_arc;
    double S1x,S1y,S2x,S2y;
    double Sx1,Sy1,Sx2,Sy2;
	double Sx,Sy;

    S1x = X0+r1x;
	S1y = Y0+r1y;

    S2x = X0+X1+r1x;
	S2y = Y0+Y1+r1y;//与圆弧相交的直线的两个点;

    Ox = I_one;
	Oy = J_one;//圆心坐标;

    r_arc = sqrt( (X0+r0x-I_one)*(X0+r0x-I_one) + (Y0+r0y-J_one)*(Y0+r0y-J_one) );//圆弧半径

    if(abs(S1x-S2x)<ZERO)
    {
		Sx1 = S1x ;
		Sy1 = Oy - sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));

		Sx2 = S1x ;
		Sy2 = Oy + sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));
    }
    else
    {
		K_arc = (S2y-S1y)/(S2x-S1x);
		a_arc = 1+K_arc*K_arc;
		b_arc = 2*K_arc*(S1y-Oy-K_arc*S1x)-2*Ox;
		c_arc = (S1y-Oy-K_arc*S1x)*(S1y-Oy-K_arc*S1x)-r_arc*r_arc+Ox*Ox;

		Sx1 = ( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);
		Sy1=S1y + K_arc*(Sx1-S1x);

		Sx2 = ( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);
		Sy2=S1y + K_arc*(Sx2-S1x);
	}//伸长线的交点坐标;

	if( (( (X0-I_one) *X1+(Y0-J_one)*Y1)>=0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)<=0) ||
		(((X0-I_one)*X1+(Y0-J_one)*Y1)<0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)>0) )
	{
		Sy = Sy1;
		Sx = Sx1;
	}
	/*else if( (((X0-I_one)*X1+(Y0-J_one)*Y1)>=0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)>0) || (((X0-I_one)*X1+(Y0-J_one)*Y1)<0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)<=0) )*/
	else
	{
		Sy = Sy2;
		Sx = Sx2;
	}
    asChild[*num] = *csChild;     //赋值,修正
    
    if(csChild->G_plane == 18)
    {
        asChild[*num].Z = Sx - (*Start_X);
        asChild[*num].X = Sy - (*Start_Y);
        asChild[*num].K = I_one-(*Start_X);
        asChild[*num].I = J_one-(*Start_Y);
    }
    
    else if(csChild->G_plane == 19)
    {
        asChild[*num].Y = Sx - (*Start_X);
        asChild[*num].Z = Sy - (*Start_Y);
        asChild[*num].J = I_one-(*Start_X);
        asChild[*num].K = J_one-(*Start_Y);
    }
	else
    {
        asChild[*num].X = Sx - (*Start_X);
        asChild[*num].Y = Sy - (*Start_Y);
		asChild[*num].I = I_one-(*Start_X);
        asChild[*num].J = J_one-(*Start_Y);
    }
	*num = *num+1;
    *Start_X = Sx - X0;  
    *Start_Y = Sy - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
}//缩短型
/******************************************************************************************/
void CircleToLinetangentialFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_one,double J_one,
								   double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y)			    
{ 
	double Sx,Sy;
    Sx = X0+r0x; 
	Sy = Y0+r0y;

    asChild[*num] = *csChild;     //赋值,修正
    if(csChild->G_plane == 18)
    {
        asChild[*num].Z = Sx - (*Start_X);
        asChild[*num].X = Sy - (*Start_Y);
        asChild[*num].K = I_one-(*Start_X);
        asChild[*num].I = J_one-(*Start_Y);
    }
    
    else if(csChild->G_plane == 19)
    {
        asChild[*num].Y = Sx - (*Start_X);
        asChild[*num].Z = Sy - (*Start_Y);
        asChild[*num].J = I_one-(*Start_X);
        asChild[*num].K = J_one-(*Start_Y);
    }
	else
    {
        asChild[*num].X = Sx - (*Start_X);
        asChild[*num].Y = Sy - (*Start_Y);
		asChild[*num].I = I_one-(*Start_X);
        asChild[*num].J = J_one-(*Start_Y);
    }
	*num = *num+1;
    *Start_X = Sx - X0;  
    *Start_Y = Sy - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
	if(csChild->G_taperCorner==24)
	{
	   *Start_X = r1x;  
	   *Start_Y = r1y;
	}
}
/******************************************************************************************/
void CircleToLineextendFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_one,double J_one,
							   double X1,double Y1,double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y)			    
{ 
    double S1x,S1y,S2x,S2y;  

    S1x = X0 + r0x;  
	S1y = Y0 + r0y; //第一段线段终点处的半径矢量顶点坐标;

    S2x = X0 + r1x;  
	S2y = Y0 + r1y;//第二段线起点处的半径矢量顶点坐标;
    asChild[*num] = *csChild;     //赋值,修正,圆弧部分 

    if(csChild->G_plane == 18)
    {
        asChild[*num].Z = S1x - (*Start_X);
        asChild[*num].X = S1y - (*Start_Y);
        asChild[*num].K = I_one-(*Start_X);
        asChild[*num].I = J_one-(*Start_Y);
		asChild[*num].row_id=2;
    }
    
    else if(csChild->G_plane == 19)
    {
        asChild[*num].Y = S1x - (*Start_X);
        asChild[*num].Z = S1y - (*Start_Y);
        asChild[*num].J = I_one-(*Start_X);
        asChild[*num].K = J_one-(*Start_Y);
		asChild[*num].row_id=2;
    }
	else
    {
        asChild[*num].X = S1x - (*Start_X);
        asChild[*num].Y = S1y - (*Start_Y);
        asChild[*num].I = I_one-(*Start_X);
        asChild[*num].J = J_one-(*Start_Y);
       asChild[*num].row_id=2;
    }
	*num = *num+1;
    asChild[*num] = *csChild;     //赋值,修正,伸长的直线S1S3部分;
	
	if(csChild->G_plane == 18)
    {
        asChild[*num].Z = S2x - S1x;
        asChild[*num].X = S2y - S1y;
        asChild[*num].K = -r0x;
        asChild[*num].I = -r0y; 
    } 
    else if(csChild->G_plane == 19)
    {
        asChild[*num].Y = S2x - S1x;
        asChild[*num].Z = S2y - S1y;
        asChild[*num].J = -r0x;
        asChild[*num].K = -r0y;
	}
	else
    {
	    asChild[*num].X = S2x - S1x;
        asChild[*num].Y = S2y - S1y;
        asChild[*num].I = -r0x;
        asChild[*num].J = -r0y; 
    }
	asChild[*num].G_pathmode =calculateCircleToLineinsertdirection(S1x,S1y,S2x,S2y,X0,Y0,X1,Y1);       
    *num=*num+1;
    *Start_X = r1x; 
    *Start_Y = r1y;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
}//插入   
/******************************************************************************************/
int calculateCircleToLineinsertdirection(double S1x,double S1y,double S2x,double S2y,double X0,double Y0,double X1,double Y1) 
{
	double tempx,tempy;
	double Arc1,Arc2,Arc3;
	int direction;
	if(X1>ZERO)
	{ 
	  tempx=X1-100;
	  tempy=Y0+Y1/X1*(tempx-X0);
	}
	else if(X1<-ZERO)
	{
	  tempx=X1+100;
	  tempy=Y0+Y1/X1*(tempx-X0);
	}
	else 
	{
		if(Y1>ZERO)
		{
			tempx=0;
			tempy=Y1-100;
		}
		else
		{
            tempx=0;
			tempy=Y1+100;
		}
	}
	Arc1=calculateLineArc(S1x-X0,S1y-Y0);
	Arc2=calculateLineArc(tempx-X0,tempy-Y0);
	Arc3=calculateLineArc(S2x-X0,S2y-Y0);
	direction = Arcdirection(Arc1,Arc2,Arc3);
    return direction;
}
/******************************************************************************************/
void LineToCircleshortenFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_two,
								double J_two,double X1,double Y1,double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y)
{
	double Ox,Oy,S1x,S1y,S2x,S2y;
	double K_arc,a_arc,b_arc,c_arc,r_arc;
	double Sx1,Sy1,Sx2,Sy2;
	double Sx,Sy;

    Ox = X0+I_two;
	Oy = Y0+J_two;//圆心坐标;

    r_arc = sqrt( (X0+r1x-Ox)*(X0+r1x-Ox) + (Y0+r1y-Oy)*(Y0+r1y-Oy) );//圆弧半径

	S2x = r0x;
	S2y = r0y;

    S1x = X0+r0x;
	S1y = Y0+r0y;//与圆弧相交的直线的两个点;

    if(abs(S1x - S2x)<ZERO)
	{
		Sx1=S1x;
		Sy1=Oy - sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));

		Sx2=S1x ;
		Sy2=Oy + sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));
	}
	else 
    {
		K_arc = (S2y-S1y)/(S2x-S1x);
		a_arc = 1+K_arc*K_arc;
		b_arc = 2*K_arc*(S1y-Oy-K_arc*S1x)-2*Ox;
		c_arc =( S1y-Oy-K_arc*S1x)*(S1y-Oy-K_arc*S1x)-r_arc*r_arc+Ox*Ox;

		Sx1=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);
		Sy1=S1y + K_arc*(Sx1-S1x);

		Sx2=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);
		Sy2=S1y + K_arc*(Sx2-S1x);
    }//伸长线的交点坐标;
	if( ((-I_two)*X0+(-J_two)*Y0>0 && (Sx2-Sx1)*X0+(Sy2-Sy1)*Y0<=0) || ((-I_two)*X0+(-J_two)*Y0<=0 && (Sx2-Sx1)*X0+(Sy2-Sy1)*Y0>0) )
	{
		Sx=Sx1;
		Sy=Sy1;
	}
	//else if( ((-I_two)*X0+(-J_two)*Y0>0 && (Sx2-Sx1)*X0+(Sy2-Sy1)*Y0>0) || ((-I_two)*X0+(-J_two)*Y0<=0 && (Sx2-Sx1)*X0+(Sy2-Sy1)*Y0<=0) )
	else
	{
		Sx=Sx2;
		Sy=Sy2;
	}						
    asChild[*num]= *csChild;     //赋值,修正
    if(csChild->G_plane == 18)
    {
        asChild[*num].Z = Sx - (*Start_X);
        asChild[*num].X = Sy - (*Start_Y);
    }   
    else if(csChild->G_plane == 19)
    {
        asChild[*num].Y = Sx - (*Start_X);
        asChild[*num].Z = Sy - (*Start_Y);
    }
	else
    {
        asChild[*num].X = Sx - (*Start_X);
        asChild[*num].Y = Sy - (*Start_Y);
    }
    *num = *num+1;
    *Start_X = Sx - X0;  
    *Start_Y = Sy - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
}//缩短型     
/******************************************************************************************/
void LineToCircletangentialFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_two,
								   double J_two,double X1,double Y1,double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y)
{
	double Sx,Sy;
    Sx = X0+r0x; Sy = Y0+r0y;//与圆弧相交的直线的两个点;

    asChild[*num]= *csChild;     //赋值,修正
    if(csChild->G_plane == 18)
    {
        asChild[*num].Z = Sx - (*Start_X);
        asChild[*num].X = Sy - (*Start_Y);
    }
   
    else if(csChild->G_plane == 19)
    {
        asChild[*num].Y = Sx - (*Start_X);
        asChild[*num].Z = Sy - (*Start_Y);
    }
	else
    {
        asChild[*num].X = Sx - (*Start_X);
        asChild[*num].Y = Sy - (*Start_Y);
    }
    *num = *num+1;
    *Start_X = Sx - X0;  
    *Start_Y = Sy - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
	if(csChild->G_taperCorner==24)
	{
		*Start_X =r1x;  
		*Start_Y =r1y;
	}
}//相切型
/******************************************************************************************/
void LineToCircleextendFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_two,
							   double J_two,double X1,double Y1,double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y)
 {
	double S1x,S1y,S2x,S2y;
	S1x = X0 + r0x;
	S1y =Y0 + r0y; //第一段线段终点处的半径矢量顶点坐标;
	S2x = X0 + r1x;
	S2y =Y0 + r1y;//第二段线起点处的半径矢量顶点坐标;
                     
	asChild[*num]= *csChild;     //赋值,修正,第一段直线    
	if(csChild->G_plane == 18)
	{
		asChild[*num].Z = S1x - (*Start_X);
		asChild[*num].X = S1y - (*Start_Y);
		asChild[*num].row_id=2;
	}
	
	else if(csChild->G_plane == 19)
	{
		asChild[*num].Y = S1x - (*Start_X);
		asChild[*num].Z = S1y - (*Start_Y);
		asChild[*num].row_id=2;
	}
	else
	{
		asChild[*num].X = S1x - (*Start_X);
		asChild[*num].Y = S1y - (*Start_Y);
		asChild[*num].row_id=2;
	}
	*num=*num+1;
	asChild[*num] = *csChild;     //赋值,修正,插入圆弧;
	if(csChild->G_plane == 18)
	{
		asChild[*num].Z = S2x - S1x;
		asChild[*num].X = S2y - S1y;
		asChild[*num].I = -r0x;
		asChild[*num].J = -r0y;
	}
	
	else if(csChild->G_plane == 19)
	{
		asChild[*num].Y = S2x - S1x;
		asChild[*num].Z = S2y - S1y;
		asChild[*num].I = -r0x;
		asChild[*num].J = -r0y;
	}
	else
	{
		asChild[*num].X = S2x - S1x;
		asChild[*num].Y = S2y - S1y;
		asChild[*num].I = -r0x;
		asChild[*num].J = -r0y;
	}
	asChild[*num].G_pathmode =calculateLineToCircleinsertdirection(S1x,S1y,S2x,S2y,X0,Y0);;
    *num=*num+1;
	*Start_X = r1x; 
	*Start_Y = r1y;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
}//插入    
/******************************************************************************************/
int calculateLineToCircleinsertdirection(double S1x,double S1y,double S2x,double S2y,double X0,double Y0)
{
	double tempx,tempy;
	double Arc1,Arc2,Arc3;
	int direction;
	if(X0>ZERO)
	{ 
	  tempx=X0+100;
	  tempy=Y0/X0*tempx;
	}
	else if(X0<-ZERO)
	{
	  tempx=X0-100;
	  tempy=Y0/X0*tempx;
	}
	else 
	{
		if(Y0>ZERO)
		{
			tempx=0;
			tempy=Y0+100;
		}
		else
		{
            tempx=0;
			tempy=Y0-100;
		}
	}
	Arc1=calculateLineArc(S1x-X0,S1y-Y0);
	Arc2=calculateLineArc(tempx-X0,tempy-Y0);
	Arc3=calculateLineArc(S2x-X0,S2y-Y0);
	direction = Arcdirection(Arc1,Arc2,Arc3);
    return direction;
}
/******************************************************************************************/
void CircleToCircleshortenFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_one,double J_one,double X1,double Y1,double I_two,double J_two,
								  double r0x,double r0y,double r1x,double r1y,double KBx0,double KBy0,double *Start_X, double *Start_Y)
{	
	double S1x,S1y,S2x,S2y;
	double Ox,Oy,r_arc,K_arc,a_arc,b_arc,c_arc,Kline;
    double Sx1,Sy1,Sx2,Sy2;
    double Sx,Sy;
    S1x = X0; 
	S1y = Y0;     
    Ox=I_one;  
	Oy=J_one;//圆心坐标;
    r_arc=sqrt( (X0+r0x-I_one)*(X0+r0x-I_one) + (Y0+r0y-J_one)*(Y0+r0y-J_one) );//圆弧半径  
    if(abs(X0+I_two-Ox)<ZERO)
    {
		S2x = I_one;
		S2y = S1y;

		Sy1=S1y ;	
		Sx1=Ox + sqrt(r_arc*r_arc-(S2y-Oy)*(S2y-Oy));
		Sy2=S1y ;	
		Sx2=Ox - sqrt(r_arc*r_arc-(S2y-Oy)*(S2y-Oy));
    }
    else if(abs(Y0+J_two-Oy)<ZERO)
    {
		S2x = S1x; 
		S2y = J_one;
		
		Sx1=S1x;	
		Sy1=Oy - sqrt(r_arc*r_arc-(S2x-Ox)*(S2x-Ox));

		Sx2=S1x;	
		Sy2=Oy + sqrt(r_arc*r_arc-(S2x-Ox)*(S2x-Ox));
	}
    else
    {
		Kline = (Y0+J_two - Oy)/(X0+I_two - Ox);//过两圆心的直线斜率;
		S2y=Kline*(X0-Ox+Oy/Kline+Kline*Y0)/(1+Kline*Kline);
		S2x=(S2y+Oy+Kline*Ox)/Kline;
		if(abs(S1x - S2x)<ZERO)
		{
			Sx1=S1x ;	
			Sy1=Oy - sqrt(r_arc*r_arc-(S2x-Ox)*(S2x-Ox));
			Sx2=S1x ;	
			Sy2=Oy + sqrt(r_arc*r_arc-(S2x-Ox)*(S2x-Ox));
		}
        else
		{
			K_arc = (S2y-S1y)/(S2x-S1x);
			a_arc = 1+K_arc*K_arc;
			b_arc = 2*K_arc*(S1y-Oy-K_arc*S1x) -2*Ox;
			c_arc = (S1y-Oy-K_arc*S1x)*(S1y-Oy-K_arc*S1x)-r_arc*r_arc+Ox*Ox;			
			Sx1 = ( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);  
			Sy1=S1y + K_arc*(Sx1-S1x);	                        
			Sx2 = ( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);  
			Sy2=S1y + K_arc*(Sx2-S1x);
		}
	}
	if( ((KBx0*(X0-I_one)+KBy0*(Y0-J_one))>=0 && ((Sx2-Sx1)*KBx0+(Sy2-Sy1)*KBy0)<=0) || 
		((KBx0*(X0-I_one)+KBy0*(Y0-J_one))<=0 && ((Sx2-Sx1)*KBx0+(Sy2-Sy1)*KBy0)>=0 ))
	{
	   Sx=Sx1;
	   Sy=Sy1;
	}
	/*else if( ((KBx0*(X0-I_one)+KBy0*(Y0-J_one))>0 && ((Sx2-Sx1)*KBx0+(Sy2-Sy1)*KBy0)>0) || ((KBx0*(X0-I_one)+KBy0*(Y0-J_one))<=0 && ((Sx2-Sx1)*KBx0+(Sy2-Sy1)*KBy0)<0 ))*/
	else
	{
		Sx=Sx2;
		Sy=Sy2;
	}
    asChild[*num]= *csChild;     //赋值,修正
    if(csChild->G_plane == 18)
    {
        asChild[*num].Z = Sx - (*Start_X);
        asChild[*num].X = Sy - (*Start_Y);
        asChild[*num].K = I_one-(*Start_X);
        asChild[*num].I = J_one-(*Start_Y);
    }
    
    else if(csChild->G_plane == 19)
    {
        asChild[*num].Y = Sx - (*Start_X);
        asChild[*num].Z = Sy - (*Start_Y);
        asChild[*num].J = I_one-(*Start_X);
        asChild[*num].K = J_one-(*Start_Y);
    } 
	else
    {
        asChild[*num].X = Sx - (*Start_X);
        asChild[*num].Y = Sy - (*Start_Y);
        asChild[*num].I = I_one-(*Start_X);
        asChild[*num].J = J_one-(*Start_Y);
    }      
    *num = *num+1;;
    *Start_X = Sx - X0;  
    *Start_Y = Sy - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
}//缩短型
/******************************************************************************************/
void CircleToCircletangentialFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_one,double J_one,
									 double X1,double Y1,double I_two,double J_two,double r0x,double r0y,double r1x,double r1y,double *Start_X, double *Start_Y)
{
	double Sx,Sy;
    Sx = X0+r0x; Sy = Y0+r0y;     
    asChild[*num]= *csChild;     //赋值,修正
    if(csChild->G_plane == 18)
    {
        asChild[*num].Z = Sx - (*Start_X);
        asChild[*num].X = Sy - (*Start_Y);
        asChild[*num].K = I_one-(*Start_X);
        asChild[*num].I = J_one-(*Start_Y);
    }
    
    else if(csChild->G_plane == 19)
    {
        asChild[*num].Y = Sx - (*Start_X);
        asChild[*num].Z = Sy - (*Start_Y);
        asChild[*num].J = I_one-(*Start_X);
        asChild[*num].K = J_one-(*Start_Y);
    }
	else
    {
        asChild[*num].X = Sx - (*Start_X);
        asChild[*num].Y = Sy - (*Start_Y);
        asChild[*num].I = I_one-(*Start_X);
        asChild[*num].J = J_one-(*Start_Y);
    }   
    *num = *num+1;;
    *Start_X = Sx - X0;  
    *Start_Y = Sy - Y0;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
   
	if(csChild->G_taperCorner==24)
    {
		*Start_X = r1x;  
		*Start_Y = r1y;
    }
}//相切型
void CircleToCircleextendFuction(nc_data *csChild,nc_data asChild[],int *num,double X0,double Y0,double I_one,double J_one,
								 double X1,double Y1,double I_two,double J_two,double r0x,double r0y,double r1x,double r1y,double KBx0,double KBy0,double *Start_X, double *Start_Y)
{
   double S1x,S1y,S2x,S2y;

    S1x = X0 + r0x; 
	S1y = Y0 + r0y; //第一段线段终点处的半径矢量顶点坐标;
    S2x = X0 + r1x;  
	S2y = Y0 + r1y;//第二段线起点处的半径矢量顶点坐标;

    asChild[*num]= *csChild;     //赋值,修正,圆弧部分   
    if(csChild->G_plane == 18)
    {
        asChild[*num].Z = S1x - (*Start_X);
        asChild[*num].X = S1y - (*Start_Y);
        asChild[*num].K = I_one-(*Start_X);
        asChild[*num].I = J_one-(*Start_Y);
		asChild[*num].row_id=2;
    }
    
    else if(csChild->G_plane == 19)
    {
        asChild[*num].Y = S1x - (*Start_X);
        asChild[*num].Z = S1y - (*Start_Y);
        asChild[*num].J = I_one-(*Start_X);
        asChild[*num].K = J_one-(*Start_Y);
		asChild[*num].row_id=2;
    }
	else
    {
        asChild[*num].X = S1x - (*Start_X);
        asChild[*num].Y = S1y - (*Start_Y);
        asChild[*num].I = I_one-(*Start_X);
        asChild[*num].J = J_one-(*Start_Y);
		asChild[*num].row_id=2;
    }
	*num=*num+1;
    asChild[*num] = *csChild;     //赋值,修正,圆弧部分
    if(csChild->G_plane == 18)
    {
        asChild[*num].Z = S2x - S1x;
        asChild[*num].X = S2y - S1y;
        asChild[*num].I = -r0x;
        asChild[*num].J = -r0y;
    }
    
    else if(csChild->G_plane == 19)
    {
        asChild[*num].Y = S2x - S1x;
        asChild[*num].Z = S2y - S1y;
        asChild[*num].I = -r0x;
        asChild[*num].J = -r0y;
    }
	else
    {
        asChild[*num].X = S2x - S1x;
        asChild[*num].Y = S2y - S1y;
        asChild[*num].I = -r0x;
        asChild[*num].J = -r0y;
    }
    asChild[*num].G_pathmode=calculateCircleToCircleinsertdirection(S1x,S1y,S2x,S2y,KBx0,KBy0,X0,Y0,I_two,J_two,I_one,J_one);
	*num = *num+1;
    *Start_X = r1x;  
    *Start_Y = r1y;//转接点相对与第二段G代码起点的相对坐标,为下一次刀补作准备;
}//插入
/******************************************************************************************/
int calculateCircleToCircleinsertdirection(double S1x,double S1y,double S2x,double S2y,double KBx0,double KBy0,double X0,double Y0,double I_two,double J_two,double I_one,double J_one)
{
	int direction;
	double tempx,tempy;
	double Kline;
	double Arc1,Arc2,Arc3;

	if(KBx0>0&&(Y0+J_two-J_one)>ZERO)
	{
		Kline=-(X0+I_two-I_one)/(Y0+J_two-J_one);
		tempx=X0+100;
		tempy=Kline*tempx;
	}
	else if(KBx0<0&&(Y0+J_two-J_one)>ZERO)
	{
		Kline=-(X0+I_two-I_one)/(Y0+J_two - J_one);
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
		else
		{
            tempx=0;
			tempy=Y0-30;
		}
	}
	Arc1=calculateLineArc(S1x-X0,S1y-Y0);
	Arc2=calculateLineArc(tempx-X0,tempy-Y0);
	Arc3=calculateLineArc(S2x-X0,S2y-Y0);		
	direction = Arcdirection(Arc1,Arc2,Arc3);
    return direction;
}
/******************************************************************************************/