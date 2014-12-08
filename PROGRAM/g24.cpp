#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;	

extern TapeParam tapepm;  //锥度加工参数
extern double ZERO;
extern double ZERO_ERROR;
extern double ZERO_NEG;

extern double z_position;
extern double xy_plane;
extern double h_thickness;
static double HLOWDISTANCE = 50.0;
extern double a_uv_plane;
extern double b_xy_plane;

extern double calculateLineArc(double PointX,double PointY);//计算直线与X轴正方向逆时针夹角

extern double calculateArc(double centerPointX,double centerPointY,double StartPointX,double StartPointY,double endPointX,
					double endPointY,int circularMode);//计算圆弧角度，其中circularMode参数定义了圆弧是顺园还是逆圆，定义同G03,G02
/***************************************************/
int CircleMode(double Circle1CenterX,double Circle1CenterY,double Circle1Radius,
			   double Circle2CenterX,double Circle2CenterY,double Circle2Radius,
			   double Circle3CenterX,double Circle3CenterY,double Circle3Radius);//圆相切模式，1表示外切，2表示内切，3表示圆1外切，圆2内切，4表示圆1内切，圆2外切

int CircleToLinePosition(double CircleCenterX,double CircleCenterY,double LineEndX,double LineEndY);//返回0表示圆心在直线右侧，返回1表示圆心在直线左侧

double CalculatePointLength(double Point1X,double Point1Y,double Point2X,double Point2Y);//计算两点之间距离

void CalculateSubCirlce(double CircleCenterX,double CircleCenterY,double CirclePointX,double CirclePointY,
						double excursion,double& excursionCirclePointX,double& excursionCirclePointY,
						int tapMode,int CirlceMode);//计算子平面参数

int IsTriangle(double Line1,double Line2,double Line3);//判断三条直线是否构成三角形，返回1表示能，返回0表示不能

void CalculatePointPosition(double Point1X,double Point1Y,double LineLength,double Arc,double& Point2X,double& Point2Y);
//计算相对某点距离为Linelength，角度为Arc的点坐标

double CalculateArc(double Line1,double Line2,double Line3);//计算三角形边1，2之间的夹角

 void G24CircleToCircle(double *X0,double *Y0,double *I0,double *J0,double *X01,double *Y01,double *I01,double *J01,double *X1,double *Y1,double *I1,double *J1,
       double *D0,double *D1,double *T0,double *T1,double *TempR,int CircleToCircleMode,int *TempPathMode,int *dataPathMode,
	   int *CompensateMode1,int *TapeMode1,int *num,int *upLowflag,nc_data *asChild);

void G24LineToLine(double *X0,double *Y0,double *X01,double *Y01,double *I,double *J,double *X1,double *Y1,double *D0,double *D1,
           double *T0,double *T1,double *TempR,int *TempPathMode,double *ProgR,int *CompensateMode1,int *TapeMode1,int *num,int *upLowflag,nc_data *asChild);

void g24Fuction(HWND hWnd,nc_data *dataChild,nc_data asChild[],int *num,nc_data *csChild,nc_data *tempChild,nc_data *ProData);

double ProjectionCalculate(double Xrefernce,double Xsubplane);

void CalculateIntersection(double StartX1,double StartY1,double EndX1, double EndY1,double StartX2,double StartY2,double EndX2,
						   double EndY2,double &IntersectionX,double &IntersectionY);

double CalculateAcuteArc(double RightLine,double Line);

double CalculatePointtoLineLength(double Pointx,double Pionty,double Coefficientx,double Coefficienty,double Intercept);

int CircleMode1(double Circle1CenterX,double Circle1CenterY,double Circle1Radius,
			   double Circle2CenterX,double Circle2CenterY,double Circle2Radius);

void G24CircleToLine(double *X0,double *Y0,double *X01,double *Y01,double *I01,double *J01,double *X1,double *Y1,double *I1,double *J1,double *D0,double *D1,
           double *T0,double *T1,double *TempR,int CircleToCircleMode,int *TempPathMode,int *dataPathMode,int *CompensateMode1,
		   int *TapeMode1,int *num,int *upLowflag,nc_data *asChild);

void G24LineToCircle(double *X0,double *Y0,double *X01,double *Y01,double *I01,double *J01,double *X1,double *Y1,double *I1,double *J1,double *D0,double *D1,
           double *T0,double *T1,double *TempR,int CircleToCircleMode,int *TempPathMode,int *dataPathMode,int *CompensateMode1,
		   int *TapeMode1,int *num,int *upLowflag,int verticalflag,nc_data *asChild);

void CalculateLineExcursion(double ProgramLineX,double ProgramLineY,double excursion,int Mode,
							int upLowflag,double &excursionX,double &excursionY);

void CalculateCircleStartExcursion(double CircleStartX,double CircleStartY,double CircleCenterX,
								   double CircleCenterY,double excursion,int tapeMode,int pathMode,
								   int *upLowFlag,double &excursionX,double &excursionY);

void CalculateCircleEndExcursion(double CircleStartX,double CircleStartY,double CircleCenterX,
								 double CircleCenterY,double excursion,int tapeMode,int pathMode,int *upLowFlag,
								 double &excursionX,double &excursionY);
/***************************************************/
void g24Fuction(HWND hWnd,nc_data *dataChild,nc_data asChild[],int *num,nc_data *csChild,nc_data *tempChild,nc_data *ProData)
{ 		
	double ProgR,ProgR1;
	int UpLowflag,verticalflag; 
	double CircleRCenterX,CircleRCenterY,CircleR1CenterX,CircleR1CenterY;
	int CircleToCircleMode;
	double ArcLine,ArcCenter;
	double ProgramR1,ProgramR2,ProgramR3;
	double X0,Y0,X1,Y1,ProgramCircleR1CenterX,ProgramCircleR1CenterY,ProgramCircleR2CenterX,ProgramCircleR2CenterY,ProgramCircleR3CenterX,ProgramCircleR3CenterY;
	 if(dataChild->G_pathmode == 1 && csChild->G_pathmode ==1)//直线接直线               
	 {
		 ProgR=sqrt((ProData+1)->I*(ProData+1)->I+(ProData+1)->J*(ProData+1)->J);
		 UpLowflag=0;
		 G24LineToLine(&(csChild->X),&(csChild->Y),&(tempChild->X),&(tempChild->Y),&(tempChild->I),&(tempChild->J),
			 &(dataChild->X),&(dataChild->Y),&(csChild->D),&(dataChild->D),&(csChild->T),&(dataChild->T),&(tempChild->R),
			 &(tempChild->G_pathmode),&ProgR,&(dataChild->G_compensate),&(dataChild->G_tapemode),num,&UpLowflag,asChild);

		 UpLowflag=1;

		 G24LineToLine(&(csChild->B),&(csChild->C),&(tempChild->B),&(tempChild->C),&(tempChild->I1),&(tempChild->J1),
			 &(dataChild->B),&(dataChild->C),&(csChild->D),&(dataChild->D),&(csChild->T),&(dataChild->T),&(tempChild->R),
			 &(tempChild->G_pathmode),&ProgR,&(dataChild->G_compensate),&(dataChild->G_tapemode),num,&UpLowflag,asChild);
	 } 
	 if((dataChild->G_pathmode == 2||dataChild->G_pathmode == 3) && (csChild->G_pathmode ==2||csChild->G_pathmode ==3))//圆弧接圆弧
	 {
		 X0=(ProData)->X;
		 Y0=(ProData)->Y;
		 ProgramCircleR1CenterX=(ProData)->I;
		 ProgramCircleR1CenterY=(ProData)->J;
		 X1=(ProData+1)->X;
		 Y1=(ProData+1)->Y;
		 ProgramCircleR2CenterX=(ProData+1)->I;
		 ProgramCircleR2CenterY=(ProData+1)->J;
		 ProgramCircleR3CenterX=(ProData+2)->I;
		 ProgramCircleR3CenterY=(ProData+2)->J;
		 ProgramR1=CalculatePointLength(0,0,ProgramCircleR1CenterX,ProgramCircleR1CenterY);
		 ProgramR2=CalculatePointLength(0,0,ProgramCircleR2CenterX,ProgramCircleR2CenterY);
		 ProgramR3=CalculatePointLength(0,0,ProgramCircleR3CenterX,ProgramCircleR3CenterY);
		 CircleToCircleMode=CircleMode(ProgramCircleR1CenterX,ProgramCircleR1CenterY,ProgramR1,ProgramCircleR2CenterX+X0,ProgramCircleR2CenterY+Y0,ProgramR2,
							   ProgramCircleR3CenterX+X0+X1,ProgramCircleR3CenterY+Y0+Y1,ProgramR3);
		 UpLowflag=0;
		 G24CircleToCircle(&(csChild->X),&(csChild->Y),&(csChild->I),&(csChild->J),&(tempChild->X),&(tempChild->Y),&(tempChild->I),&(tempChild->J),
				  &(dataChild->X),&(dataChild->Y),&(dataChild->I),&(dataChild->J),&(csChild->D),&(dataChild->D),&(csChild->T),
				  &(dataChild->T),&(tempChild->R),CircleToCircleMode,&(tempChild->G_pathmode),&(dataChild->G_pathmode),
				  &(dataChild->G_compensate),&(dataChild->G_tapemode),num,&UpLowflag,asChild);

		UpLowflag=1;

		G24CircleToCircle(&(csChild->B),&(csChild->C),&(csChild->I1),&(csChild->J1),&(tempChild->B),&(tempChild->C),&(tempChild->I1),&(tempChild->J1),
						 &(dataChild->B),&(dataChild->C),&(dataChild->I1),&(dataChild->J1),&(csChild->D),&(dataChild->D),
						 &(csChild->T),&(dataChild->T),&(tempChild->R),CircleToCircleMode,&(tempChild->G_pathmode),
						 &(dataChild->G_pathmode),&(dataChild->G_compensate),&(dataChild->G_tapemode),num,&UpLowflag,asChild);
		}
	    if((dataChild->G_pathmode==1) && (csChild->G_pathmode ==2||csChild->G_pathmode ==3))//圆弧接直线
		{
			ProgR=sqrt((ProData)->I*(ProData)->I+(ProData)->J*(ProData)->J);
			ProgR1=sqrt((ProData+1)->I*(ProData+1)->I+(ProData+1)->J*(ProData+1)->J);
		
			CircleRCenterX=(ProData)->I;
			CircleRCenterY=(ProData)->J;
			CircleR1CenterX=(ProData)->X+(ProData+1)->I;
			CircleR1CenterY=(ProData)->Y+(ProData+1)->J;
			CircleToCircleMode=CircleMode1(CircleRCenterX,CircleRCenterY,ProgR,CircleR1CenterX,CircleR1CenterY,ProgR1);//1外切2内切

			ArcLine=calculateLineArc((ProData+2)->X,(ProData+2)->Y);//计算直线与X轴正方向逆时针夹角
			ArcCenter=calculateLineArc(CircleR1CenterX-CircleRCenterX,CircleR1CenterY-CircleRCenterY);//计算直线与X轴正方向逆时针夹角
			if(abs(abs(ArcLine-ArcCenter-PI/2)<0.1))
                verticalflag=0;
			else if(abs(abs(ArcLine-ArcCenter)-3*PI/2)<0.1)
                verticalflag=0;
			else 
                verticalflag=1;
		
			UpLowflag=0;
		    G24LineToCircle(&(dataChild->X),&(dataChild->Y),&(tempChild->X),&(tempChild->Y),&(tempChild->I),&(tempChild->J),&(csChild->X),
				&(csChild->Y),&(csChild->I),&(csChild->J),&(csChild->D),&(dataChild->D),&(csChild->T),&(dataChild->T),
				&(tempChild->R),CircleToCircleMode,&(tempChild->G_pathmode),&(csChild->G_pathmode),&(dataChild->G_compensate),
				&(dataChild->G_tapemode),num,&UpLowflag,verticalflag,asChild);
		    UpLowflag=1;
		    G24LineToCircle(&(dataChild->B),&(dataChild->C),&(tempChild->B),&(tempChild->C),&(tempChild->I1),&(tempChild->J1),&(csChild->B),
				&(csChild->C),&(csChild->I1),&(csChild->J1),&(csChild->D),&(dataChild->D),&(csChild->T),&(dataChild->T),&(tempChild->R),
				CircleToCircleMode,&(tempChild->G_pathmode),&(csChild->G_pathmode),&(dataChild->G_compensate),&(dataChild->G_tapemode),num,
				&UpLowflag,verticalflag,asChild);	
		}
		if((dataChild->G_pathmode == 2||dataChild->G_pathmode == 3)&&(csChild->G_pathmode==1))
		{
			ProgR=sqrt((ProData+1)->I*(ProData+1)->I+(ProData+1)->J*(ProData+1)->J);
			ProgR1=sqrt((ProData+2)->I*(ProData+2)->I+(ProData+2)->J*(ProData+2)->J);
		
			CircleRCenterX=(ProData+1)->I;
			CircleRCenterY=(ProData+1)->J;
			CircleR1CenterX=(ProData+1)->X+(ProData+2)->I;
			CircleR1CenterY=(ProData+1)->Y+(ProData+2)->J;
			CircleToCircleMode=CircleMode1(CircleRCenterX,CircleRCenterY,ProgR,CircleR1CenterX,CircleR1CenterY,ProgR1);//1外切2内切
		
			UpLowflag=0;
		    G24CircleToLine(&(csChild->X),&(csChild->Y),&(tempChild->X),&(tempChild->Y),&(tempChild->I),&(tempChild->J),&(dataChild->X),
				&(dataChild->Y),&(dataChild->I),&(dataChild->J),&(csChild->D),&(dataChild->D),&(csChild->T),&(dataChild->T),&(tempChild->R),
				CircleToCircleMode,&(tempChild->G_pathmode),&(dataChild->G_pathmode),&(dataChild->G_compensate),&(dataChild->G_tapemode),num,&UpLowflag,asChild);
		    UpLowflag=1;
		    G24CircleToLine(&(csChild->B),&(csChild->C),&(tempChild->B),&(tempChild->C),&(tempChild->I1),&(tempChild->J1),&(dataChild->B),&(dataChild->C),
				&(dataChild->I1),&(dataChild->J1),&(csChild->D),&(dataChild->D),&(csChild->T),&(dataChild->T),&(tempChild->R),
				CircleToCircleMode,&(tempChild->G_pathmode),&(dataChild->G_pathmode),&(dataChild->G_compensate),&(dataChild->G_tapemode),num,&UpLowflag,asChild);
		}
}
double ProjectionCalculate(double Xrefernce,double Xsubplane)
{
	double k1,k2;
	k1=  -(a_uv_plane)/ b_xy_plane;
	k2=	(a_uv_plane+b_xy_plane)/b_xy_plane ;
	return (k2*Xrefernce+k1*Xsubplane);
}
void CalculateIntersection(double StartX1,double StartY1,double EndX1, double EndY1,double StartX2,double StartY2,double EndX2,double EndY2,double &IntersectionX,double &IntersectionY)
{ //计算两条直线交点
	double K1,K2; 
	IntersectionX=IntersectionY=0;
	if((EndX1-StartX1)==0)
	{
		K2=(EndY2-StartY2)/(EndX2-StartX2);
		IntersectionX=StartX1;
		IntersectionY=StartY2+K2*(IntersectionX-StartX2);
	}
	else if((EndX2-StartX2)==0)
	{
		K1=(EndY1-StartY1)/(EndX1-StartX1);
		IntersectionX=StartX2;
		IntersectionY=StartY1+K1*(IntersectionX-StartX1);
	}
	else
	{
		K1=(EndY1-StartY1)/(EndX1-StartX1);
		K2=(EndY2-StartY2)/(EndX2-StartX2);
		IntersectionX=(StartY2-StartY1-K2*StartX2+K1*StartX1)/(K1-K2);
		IntersectionY=StartY1+K1*(IntersectionX-StartX1);
	}
}
void CalculateIntersection1(double SubX1,double SubY1,double SubX2, double SubY2,double StartX1,double StartY1,double EndX1,
							double EndY1,double &IntersectionX,double &IntersectionY)	
//求交点，已知一条直线的斜率（(StartX1,StartY1)(EndX1,EndY1)决定），和两条直线分别过的2个点（SubX1,SubY1）,（SubX2, SubY2）
{
	double K1,K2; 
	IntersectionX=IntersectionY=0;
	if((EndX1-StartX1)==0)
	{
		IntersectionX=SubX1;
		IntersectionY=SubY2;
	}
	else if((EndY1-StartY1)==0)
	{
		IntersectionX=SubX2;
		IntersectionY=SubY1;
	}
	else
	{
		K1=(EndY1-StartY1)/(EndX1-StartX1);
		K2=-(EndX1-StartX1)/(EndY1-StartY1);
		IntersectionX=(SubY2-SubY1-K2*SubX2+K1*SubX1)/(K1-K2);
		IntersectionY=SubY1+K1*(IntersectionX-SubX1);
	}
}
double CalculateAcuteArc(double RightLine,double Line)//in right triangle 求直角边对的角
{     
	double AcuteArc;
    AcuteArc=asin(RightLine/Line);
	return AcuteArc;
}
double CalculatePointtoLineLength(double Pointx,double Pionty,double Coefficientx,double Coefficienty,double Intercept)//点到直线ax+by+c=0的距离
{
	double PointToLineLength;
	if(Coefficientx==0&&Coefficienty!=0)
	{
		PointToLineLength=abs(Pionty+Intercept/Coefficienty);
	}
	else if(Coefficienty==0&&Coefficientx!=0)
	{
		PointToLineLength=abs(Pointx+Intercept/Coefficientx);
	}
	else 
	{
		PointToLineLength=abs(Coefficientx*Pointx+Coefficienty*Pionty+Intercept)/sqrt(Coefficientx*Coefficientx+Coefficienty*Coefficienty);
	}
	return PointToLineLength;
}
int CircleMode1(double Circle1CenterX,double Circle1CenterY,double Circle1Radius,
			   double Circle2CenterX,double Circle2CenterY,double Circle2Radius)//圆相切模式，1表示外切，2表示内切
{
	double LengthCircle1ToCircle2;
	int Circle1Circle2Mode;//1表示两圆外切，2表示两圆内切
	LengthCircle1ToCircle2=CalculatePointLength(Circle1CenterX,Circle1CenterY,Circle2CenterX,Circle2CenterY);
	if(abs(LengthCircle1ToCircle2-abs(Circle1Radius-Circle2Radius))>ZERO)Circle1Circle2Mode=1;
	else Circle1Circle2Mode=2;
	return Circle1Circle2Mode;
}
void CalculateLineExcursion(double ProgramLineX,double ProgramLineY,double excursion,int Mode,int upLowflag,double &excursionX,double &excursionY)
{
 	double R;
	R = sqrt((ProgramLineX*ProgramLineX)+(ProgramLineY*ProgramLineY));						
	if((Mode == 51&&upLowflag==0)||(Mode==52&&upLowflag==1))
	{	
		excursionY = (excursion)*(ProgramLineX)/R;
		excursionX = (-excursion)*(ProgramLineY)/R; 
	}
	else if((Mode==51&&upLowflag==1)||(Mode == 52&&upLowflag==0))
	{
		excursionY = (-excursion)*(ProgramLineX)/R;
		excursionX = (excursion)*(ProgramLineY)/R;							
	}
	if(Mode==41)
	{
      	excursionY = (excursion)*(ProgramLineX)/R;
		excursionX = (-excursion)*(ProgramLineY)/R;                      
	}
	else if(Mode==42)
	{
		excursionY = (-excursion)*(ProgramLineX)/R;
		excursionX = (excursion)*(ProgramLineY)/R;	
	}
}
void CalculateCircleEndExcursion(double CircleEndX,double CircleEndY,double CircleCenterX,double CircleCenterY,double excursion,int Mode,int pathMode,int *upLowFlag,double &excursionX,double &excursionY)
{
	double R;
	R=sqrt((CircleCenterX-CircleEndX)*(CircleCenterX-CircleEndX)+(CircleCenterY-CircleEndY)*(CircleCenterY-CircleEndY));
	if((Mode==51&&pathMode==2&&*upLowFlag==0)||(Mode==52&&pathMode==3&&*upLowFlag==0)||(Mode==52&&pathMode==2&&*upLowFlag==1)||
		(Mode==51&&pathMode==3&&*upLowFlag==1))
	{
		excursionY=(-excursion)*(CircleEndY-CircleCenterY)/R;
		excursionX=(-excursion)*(CircleEndX-CircleCenterX)/R;
	}
	else if((Mode==51&&pathMode==2&&*upLowFlag==1)||(Mode==52&&pathMode==3&&*upLowFlag==1)||(Mode==52&&pathMode==2&&*upLowFlag==0)||
		(Mode==51&&pathMode==3&&*upLowFlag==0))
	{
		excursionY=(excursion)*(CircleEndY-CircleCenterY)/R;
		excursionX=(excursion)*(CircleEndX-CircleCenterX)/R;
	}
	if((Mode==41&&pathMode==2)||(Mode==42&&pathMode==3))
	{	
		excursionY=(excursion)*(CircleEndY-CircleCenterY)/R;
        excursionX=(excursion)*(CircleEndX-CircleCenterX)/R;
	}
	else if((Mode==42&&pathMode==2)||(Mode==41&&pathMode==3))
	{
		excursionY=-(excursion)*(CircleEndY-CircleCenterY)/R;
        excursionX=-(excursion)*(CircleEndX-CircleCenterX)/R;
	}
}
void CalculateCircleStartExcursion(double CircleStartX,double CircleStartY,double CircleCenterX,double CircleCenterY,double excursion,int Mode,int pathMode,int *upLowFlag,double &excursionX,double &excursionY)
{
	double R;
	R=sqrt((CircleCenterX-CircleStartX)*(CircleCenterX-CircleStartX)+(CircleCenterY-CircleStartY)*(CircleCenterY-CircleStartY));
    if((Mode==51&&pathMode==2&&*upLowFlag==0)||(Mode==52&&pathMode==3&&*upLowFlag==0)||(Mode==52&&pathMode==2&&*upLowFlag==1)||(Mode==51&&pathMode==3&&*upLowFlag==1))
	{
		excursionY=(-excursion)*(CircleStartY-CircleCenterY)/R;
		excursionX=(-excursion)*(CircleStartX-CircleCenterX)/R;
	}
	else if((Mode==51&&pathMode==2&&*upLowFlag==1)||(Mode==52&&pathMode==3&&*upLowFlag==1)||(Mode==52&&pathMode==2&&*upLowFlag==0)||(Mode==51&&pathMode==3&&*upLowFlag==0))
	{
		excursionY=(excursion)*(CircleStartY-CircleCenterY)/R;
		excursionX=(excursion)*(CircleStartX-CircleCenterX)/R;
	}
	if((Mode==41&&pathMode==2)||(Mode==42&&pathMode==3))
	{	
		excursionY=(excursion)*(CircleStartY-CircleCenterY)/R;
        excursionX=(excursion)*(CircleStartX-CircleCenterX)/R;
	}
	else if((Mode==42&&pathMode==2)||(Mode==41&&pathMode==3))
	{
		excursionY=-(excursion)*(CircleStartY-CircleCenterY)/R;
        excursionX=-(excursion)*(CircleStartX-CircleCenterX)/R;
	}
}
int CircleToLinePosition(double CircleCenterX,double CircleCenterY,double LineEndX,double LineEndY)//返回0表示圆心在直线右侧，返回1表示圆心在直线左侧
{
	double ArcR1,ArcR2,Arc;
	ArcR1=calculateLineArc(LineEndX,LineEndY);
	ArcR2=calculateLineArc(CircleCenterX,CircleCenterY);

	Arc=ArcR2-ArcR1;

	if(Arc>0&&Arc<PI)
	{
		return 1;
	}      
	else if(Arc>PI||Arc<0)
	{
		return 0;
	}
	return 0;
}
/***************************************************/
double CalculatePointLength(double Point1X,double Point1Y,double Point2X,double Point2Y)
{
	double X,Y,L;
	X=Point1X-Point2X;
	Y=Point1Y-Point2Y;
	L=sqrt(X*X+Y*Y);
	return L;
}
/***************************************************/
void CalculateSubCirlce(double CircleCenterX,double CircleCenterY,double CirclePointX,double CirclePointY,
						double excursion,double& excursionCirclePointX,double& excursionCirclePointY,
						int tapMode,int CirlceMode)//计算子平面参数
{
	double R;
	double excursionX,excursionY;
	R=excursionX=excursionY=0.0;
	R = sqrt((CirclePointY-CircleCenterY)*(CirclePointY-CircleCenterY)+(CirclePointX-CircleCenterX)*(CirclePointX-CircleCenterX));
	if((tapMode == 51 && CirlceMode == 2)||(tapMode == 52 && CirlceMode == 3))
	{
		excursionY = (-excursion)*(CirclePointY-CircleCenterY)/R;
		excursionX = (-excursion)*(CirclePointX-CircleCenterX)/R;   
	}
	else if((tapMode == 52 && CirlceMode == 2)||(tapMode == 51 && CirlceMode == 3))
	{
		excursionY = excursion*(CirclePointY-CircleCenterY)/R;
		excursionX = excursion*(CirclePointX-CircleCenterX)/R;
	}
	excursionCirclePointX=CirclePointX+excursionX;
	excursionCirclePointY=CirclePointY+excursionY;
}
/***************************************************/
int IsTriangle(double Line1,double Line2,double Line3)//判断三条直线是否构成三角形，返回1表示能，返回0表示不能
{
	if(Line1<0)return 0;
	else if(Line2<0)return 0;
	else if(Line3<0)return 0;
	else if(Line1+Line2<Line3)return 0;
	else if(Line2+Line3<Line1)return 0;
	else if(Line3+Line1<Line2)return 0;
	else if(abs(Line1-Line2)>Line3)return 0;
	else if(abs(Line2-Line3)>Line1)return 0;
	else if(abs(Line3-Line1)>Line2)return 0;
	return 1;
}
/***************************************************/
void CalculatePointPosition(double Point1X,double Point1Y,double LineLength,double Arc,double& Point2X,double& Point2Y)//计算相对某点距离为Linelength，角度为Arc的点坐标
{
	Point2X=Point1X+LineLength*cos(Arc);
	Point2Y=Point1Y+LineLength*sin(Arc);
}
/***************************************************/
double CalculateArc(double Line1,double Line2,double Line3)//计算三角形边1，2之间的夹角
{
	double cosA;
	double Arc;
	cosA=(Line1*Line1+Line2*Line2-Line3*Line3)/(2*Line1*Line2);
	Arc=acos(cosA);
	return Arc;
}
/***************************************************/
int CircleMode(double Circle1CenterX,double Circle1CenterY,double Circle1Radius,
			   double Circle2CenterX,double Circle2CenterY,double Circle2Radius,
			   double Circle3CenterX,double Circle3CenterY,double Circle3Radius)//圆相切模式，1表示外切，2表示内切，3表示圆1外切，圆2内切，4表示圆1内切，圆2外切
{
	double LengthCircle1ToCircle2;
	double LengthCircle2ToCircle3;
	int Circle1Circle2Mode,Circle2Circle3Mode;//1表示两圆外切，2表示两圆内切
	LengthCircle1ToCircle2=CalculatePointLength(Circle1CenterX,Circle1CenterY,Circle2CenterX,Circle2CenterY);
	LengthCircle2ToCircle3=CalculatePointLength(Circle3CenterX,Circle3CenterY,Circle2CenterX,Circle2CenterY);
	if(abs(LengthCircle1ToCircle2-abs(Circle1Radius-Circle2Radius))>0.0001)Circle1Circle2Mode=1;
	else Circle1Circle2Mode=2;
	if(abs(LengthCircle2ToCircle3-abs(Circle2Radius-Circle3Radius))>0.0001)Circle2Circle3Mode=1;
	else Circle2Circle3Mode=2;
	if(Circle1Circle2Mode==1&&Circle2Circle3Mode==1)return 1;
	else if(Circle1Circle2Mode==2&&Circle2Circle3Mode==2)return 2;
	else if(Circle1Circle2Mode==1&&Circle2Circle3Mode==2)return 3;
	else if(Circle1Circle2Mode==2&&Circle2Circle3Mode==1)return 4;
	return 1;
}
 void G24CircleToCircle(double *X0,double *Y0,double *I0,double *J0,double *X01,double *Y01,double *I01,double *J01,double *X1,double *Y1,double *I1,double *J1,
       double *D0,double *D1,double *T0,double *T1,double *TempR,int CircleToCircleMode,int *TempPathMode,int *dataPathMode,int *CompensateMode1,int *TapeMode1,int *num,int *upLowflag, nc_data *asChild)
{
	int CirclePosition;//记录所求圆弧的位置，1表示在上方，2表示在下方
	double R1,R2,R3,SubR2;//编程平面三个圆弧半径
	double Circle1excursionD,Circle3excursionD,Circle1excursionT,Circle3excursionT;//下平面第一段线段的投影半径
	double ArcCircle1ToCircle3,ArcCircle1ToCircle2,ArcCircle2ToCircle3;//圆弧圆心连线与X正半轴夹角
	double Circle1Circle2Length,Circle2Circle3Length,Circle1Circle3Length;//圆弧圆心连线距离
	double ArcLine1Line2,ArcLine1Line3,ArcLine2Line3;//两圆弧圆心连线之间的夹角
	double excursionY1S,excursionX1S,excursionX2S,excursionY2S;
	//编程平面和子平面各点坐标，相对于编程平面第一个圆弧起始点
	double CircleR1StartX,CircleR1StartY,CircleR1EndX,CircleR1EndY,CircleR1CenterX,CircleR1CenterY;
	double CircleR2StartX,CircleR2StartY,CircleR2EndX,CircleR2EndY,CircleR2CenterX,CircleR2CenterY;
	double CircleR3StartX,CircleR3StartY,CircleR3EndX,CircleR3EndY,CircleR3CenterX,CircleR3CenterY;
	//
	CircleR1StartX=0;
	CircleR1StartY=0;
	CircleR1EndX=CircleR1StartX+(*X0);
	CircleR1EndY=CircleR1StartY+(*Y0);
	CircleR1CenterX=(*I0);
	CircleR1CenterY=(*J0);
	CircleR2StartX=CircleR1EndX;
	CircleR2StartY=CircleR1EndY;
	CircleR2EndX=CircleR2StartX+(*X01);
	CircleR2EndY=CircleR2StartY+(*Y01);
	CircleR2CenterX=CircleR2StartX+(*I01);
	CircleR2CenterY=CircleR2StartY+(*J01);
	CircleR3StartX=CircleR2EndX;
	CircleR3StartY=CircleR2EndY;
	CircleR3EndX=CircleR3StartX+(*X1);
	CircleR3EndY=CircleR3StartY+(*Y1);
	CircleR3CenterX=CircleR3StartX+(*I1);
	CircleR3CenterY=CircleR3StartY+(*J1);
	// 
   	Circle1excursionD=(*D0)/cos((*T0/1000)*PI/180.0);
	Circle3excursionD=(*D1)/cos((*T1/1000)*PI/180.0);
	if(*upLowflag==0)
	{
	    Circle1excursionT=xy_plane*tan((*T0)*PI/1000/180);//下平面第一段线段的投影半径
	    Circle3excursionT=xy_plane*tan((*T1)*PI/1000/180);//下平面第一段的投影半径		
	}
	else if(*upLowflag==1)
	{
	    Circle1excursionT=(h_thickness- xy_plane)*tan((*T0)*PI/1000/180);//上平面第一段线段的投影半径
	    Circle3excursionT=(h_thickness- xy_plane)*tan((*T1)*PI/1000/180);//上平面第一段的投影半径		
	}
        CalculateCircleStartExcursion(CircleR3StartX,CircleR3StartY,CircleR3CenterX,CircleR3CenterY,(Circle3excursionD-Circle1excursionD),*CompensateMode1,*dataPathMode,
		upLowflag,excursionX1S,excursionY1S);
        CalculateCircleStartExcursion(CircleR3StartX,CircleR3StartY,CircleR3CenterX,CircleR3CenterY,(Circle3excursionT-Circle1excursionT),*TapeMode1,*dataPathMode,
		upLowflag,excursionX2S,excursionY2S);
	//
	CircleR3StartX=CircleR2EndX+(excursionX1S+excursionX2S);
	CircleR3StartY=CircleR2EndY+(excursionY1S+excursionY2S);
	CircleR3EndX=CircleR3StartX+(*X1);
	CircleR3EndY=CircleR3StartY+(*Y1);
	CircleR3CenterX=CircleR3StartX+(*I1);
	CircleR3CenterY=CircleR3StartY+(*J1);

	R1=CalculatePointLength(CircleR1CenterX,CircleR1CenterY,CircleR1StartX,CircleR1StartY);
	R2=CalculatePointLength(CircleR2CenterX,CircleR2CenterY,CircleR2StartX,CircleR2StartY);
	R3=CalculatePointLength(CircleR3CenterX,CircleR3CenterY,CircleR3StartX,CircleR3StartY);
	SubR2=*TempR;
	if(SubR2<0.0001)SubR2=R2+Circle1excursionT;

	Circle1Circle3Length=CalculatePointLength(CircleR1CenterX,CircleR1CenterY,CircleR3CenterX,CircleR3CenterY);
	switch(CircleToCircleMode)
	{
	case 1:
		Circle1Circle2Length=R1+SubR2;
		Circle2Circle3Length=R3+SubR2;
		break;
	case 2:
		Circle1Circle2Length=abs(SubR2-R1);
		Circle2Circle3Length=abs(SubR2-R3);
		break;
	case 3:
		Circle1Circle2Length=SubR2+R1;
		Circle2Circle3Length=abs(SubR2-R3);
		break;
	case 4:
		Circle1Circle2Length=abs(SubR2-R1);
		Circle2Circle3Length=SubR2+R3;
		break;
	}
	if(IsTriangle(Circle1Circle2Length,Circle2Circle3Length,Circle1Circle3Length)==1)
	{
		ArcCircle1ToCircle3=calculateLineArc(CircleR3CenterX-CircleR1CenterX,CircleR3CenterY-CircleR1CenterY);
		//
		ArcLine1Line2=CalculateArc(Circle1Circle2Length,Circle1Circle3Length,Circle2Circle3Length);
		ArcLine1Line3=CalculateArc(Circle2Circle3Length,Circle1Circle3Length,Circle1Circle2Length);
		ArcLine2Line3=CalculateArc(Circle1Circle2Length,Circle2Circle3Length,Circle1Circle3Length);
		//
		CirclePosition=CircleToLinePosition(CircleR3CenterX-CircleR1CenterX,CircleR3CenterY-CircleR1CenterY,
			CircleR2CenterX-CircleR1CenterX,CircleR2CenterY-CircleR1CenterY);//1表示点在直线左侧，0右侧
		if(CirclePosition==1)
		{
             ArcCircle1ToCircle2=ArcCircle1ToCircle3-ArcLine1Line2;
             ArcCircle2ToCircle3=ArcCircle1ToCircle3+ArcLine1Line3;
		}
		else 
		{
             ArcCircle1ToCircle2=ArcCircle1ToCircle3+ArcLine1Line2; 
             ArcCircle2ToCircle3=ArcCircle1ToCircle3-ArcLine1Line3;
		}
		CalculatePointPosition(CircleR1CenterX,CircleR1CenterY,Circle1Circle2Length,ArcCircle1ToCircle2,CircleR2CenterX,CircleR2CenterY);
        if(CircleToCircleMode==1)
		{
			CalculatePointPosition(CircleR1CenterX,CircleR1CenterY,R1,ArcCircle1ToCircle2,CircleR1EndX,CircleR1EndY);
			CalculatePointPosition(CircleR3CenterX,CircleR3CenterY,R3,PI+ArcCircle2ToCircle3,CircleR3StartX,CircleR3StartY);
		}
		else if(CircleToCircleMode==2)
		{
			CalculatePointPosition(CircleR1CenterX,CircleR1CenterY,R1,ArcCircle1ToCircle2+PI,CircleR1EndX,CircleR1EndY);
			CalculatePointPosition(CircleR3CenterX,CircleR3CenterY,R3,PI+ArcCircle2ToCircle3,CircleR3StartX,CircleR3StartY);
		}
		else if(CircleToCircleMode==3)
		{
			CalculatePointPosition(CircleR1CenterX,CircleR1CenterY,R1,ArcCircle1ToCircle2,CircleR1EndX,CircleR1EndY);
			CalculatePointPosition(CircleR3CenterX,CircleR3CenterY,R3,PI+ArcCircle2ToCircle3,CircleR3StartX,CircleR3StartY);
		}
		else if(CircleToCircleMode==4)
		{
			CalculatePointPosition(CircleR1CenterX,CircleR1CenterY,R1,ArcCircle1ToCircle2,CircleR1EndX,CircleR1EndY);
			CalculatePointPosition(CircleR3CenterX,CircleR3CenterY,R3,PI+ArcCircle2ToCircle3,CircleR3StartX,CircleR3StartY);
		}

		CircleR2StartX=CircleR1EndX;
		CircleR2StartY=CircleR1EndY;		
		CircleR2EndX=CircleR3StartX;
		CircleR2EndY=CircleR3StartY;
		
		if(*upLowflag==0)			
		{
			asChild[*num-1].X = CircleR1EndX-CircleR1StartX; 
			asChild[*num-1].Y = CircleR1EndY-CircleR1StartY;
			asChild[*num-1].I = CircleR1CenterX-CircleR1StartX;  
			asChild[*num-1].J = CircleR1CenterY-CircleR1StartY;

			asChild[*num].X = CircleR2EndX-CircleR2StartX; 
			asChild[*num].Y = CircleR2EndY-CircleR2StartY;
			asChild[*num].I = CircleR2CenterX-CircleR2StartX;  
			asChild[*num].J = CircleR2CenterY-CircleR2StartY;

			asChild[*num+1].X = CircleR3EndX-CircleR3StartX; 
			asChild[*num+1].Y = CircleR3EndY-CircleR3StartY;
			asChild[*num+1].I = CircleR3CenterX-CircleR3StartX;  
			asChild[*num+1].J = CircleR3CenterY-CircleR3StartY;	
		}
		if(*upLowflag==1)			
		{
			asChild[*num-1].B = CircleR1EndX-CircleR1StartX; 
			asChild[*num-1].C = CircleR1EndY-CircleR1StartY;
			asChild[*num-1].I1 = CircleR1CenterX-CircleR1StartX;  
			asChild[*num-1].J1 = CircleR1CenterY-CircleR1StartY;

			asChild[*num].B = CircleR2EndX-CircleR2StartX; 
			asChild[*num].C = CircleR2EndY-CircleR2StartY;
			asChild[*num].I1 = CircleR2CenterX-CircleR2StartX;  
			asChild[*num].J1 = CircleR2CenterY-CircleR2StartY;

			asChild[*num+1].B = CircleR3EndX-CircleR3StartX; 
			asChild[*num+1].C = CircleR3EndY-CircleR3StartY;
			asChild[*num+1].I1 = CircleR3CenterX-CircleR3StartX;  
			asChild[*num+1].J1 = CircleR3CenterY-CircleR3StartY;	
		}			
	}
}
 void G24CircleToLine(double *X0,double *Y0,double *X01,double *Y01,double *I01,double *J01,double *X1,double *Y1,double *I1,double *J1,double *D0,double *D1,
           double *T0,double *T1,double *TempR,int CircleToCircleMode,int *TempPathMode,int *dataPathMode,int *CompensateMode1,int *TapeMode1,int *num,int *upLowflag,nc_data *asChild)
{//直线接圆弧
	double R1,R2;//下平面两个圆弧半径
	double Circle1excursionD,LineexcursionD,Circle1excursionT,LineexcursionT;//投影半径
	double ArcCircle1ToCircle2;//圆弧圆心连线与X正半轴夹角
	double ArcLine;//直线的倾斜角
	double Circle1Circle2Length;//圆弧圆心连线距离
	double LineCircleLength,LineCircle1Length;//圆心到直线的距离
	double ArcLineCenter;//直线与圆弧圆心连线的夹角
	double Coefficientx,Coefficienty,Intercept;//计算点到直线距离
	double RightLine1,RightLine2;
	double PiontAX,PointAY,PointBX,PointBY,PointCX,PointCY,PointDX,PointDY;
	double excursionX1S,excursionY1S,excursionX2S,excursionY2S;

	//编程平面和子平面各点坐标，相对于编程平面第一个圆弧起始点
	double CircleR1StartX,CircleR1StartY,CircleR1EndX,CircleR1EndY,CircleR1CenterX,CircleR1CenterY;
	double CircleRStartX,CircleRStartY,CircleREndX,CircleREndY,CircleRCenterX,CircleRCenterY;
	double LineStartX,LineStartY,LineEndX,LineEndY;//
	//
	LineexcursionD=(*D0)/cos((*T0/1000)*PI/180.0);
	Circle1excursionD=(*D1)/cos((*T1/1000)*PI/180.0);
	if(*upLowflag==0)
	{
		LineexcursionT=xy_plane*tan((*T0)*PI/1000/180);//下平面第一段线段的投影半径
	    Circle1excursionT=xy_plane*tan((*T1)*PI/1000/180);//下平面第一段的投影半径		
	}
	else if(*upLowflag==1)
	{
		LineexcursionT=(h_thickness- xy_plane)*tan((*T0)*PI/1000/180);//上平面第一段线段的投影半径
	    Circle1excursionT=(h_thickness- xy_plane)*tan((*T1)*PI/1000/180);//上平面第一段的投影半径		
	}
	LineStartX=0;
	LineStartY=0;
	LineEndX=LineStartX+(*X0);
	LineEndY=LineStartY+(*Y0);
	CircleRStartX=LineEndX;
	CircleRStartY=LineEndY;
	CircleREndX=CircleRStartX+(*X01);
	CircleREndY=CircleRStartY+(*Y01);
	CircleRCenterX=CircleRStartX+(*I01);
	CircleRCenterY=CircleRStartY+(*J01);

	R1=*TempR;	
	if(R1<0.0001)R1=sqrt((*I01)*(*I01)+(*J01)*(*J01))+LineexcursionT;

	CircleR1StartX=CircleREndX;
	CircleR1StartY=CircleREndY;
	CircleR1EndX=CircleR1StartX+(*X1);
	CircleR1EndY=CircleR1StartY+(*Y1);
	CircleR1CenterX=CircleR1StartX+(*I1);
	CircleR1CenterY=CircleR1StartY+(*J1);

    CalculateCircleStartExcursion(CircleR1StartX,CircleR1StartY,CircleR1CenterX,CircleR1CenterY,(Circle1excursionD-LineexcursionD),*CompensateMode1,*dataPathMode,
		upLowflag,excursionX1S,excursionY1S);
    CalculateCircleStartExcursion(CircleR1StartX,CircleR1StartY,CircleR1CenterX,CircleR1CenterY,(Circle1excursionT-LineexcursionT),*TapeMode1,*dataPathMode,
		upLowflag,excursionX2S,excursionY2S);

	CircleR1StartX=CircleREndX+(excursionX1S+excursionX2S);
	CircleR1StartY=CircleREndY+(excursionY1S+excursionY2S);
	CircleR1EndX=CircleR1StartX+(*X1);
	CircleR1EndY=CircleR1StartY+(*Y1);
	CircleR1CenterX=CircleR1StartX+(*I1);
	CircleR1CenterY=CircleR1StartY+(*J1);

	R2=CalculatePointLength(CircleR1CenterX,CircleR1CenterY,CircleR1StartX,CircleR1StartY);
	ArcLine=calculateLineArc(LineEndX-LineStartX,LineEndY-LineStartY);//计算直线与X轴正方向逆时针夹角
	LineCircleLength=R1;
	{
		if(LineEndX-LineStartX==0)
		{
		   Coefficientx=0;
		   Coefficienty=1;
		   Intercept=-(LineEndY-LineStartY);
		}
		else 
		{
		   Coefficientx=(LineEndY-LineStartY)/(LineEndX-LineStartX);
		   Coefficienty=-1;
		   Intercept=(LineStartY)-Coefficientx*(LineStartX);
		}
	   LineCircle1Length=CalculatePointtoLineLength(CircleR1CenterX,CircleR1CenterY, Coefficientx,Coefficienty,Intercept);
	}//计算圆心1到sub面上的直线的距离
	if(CircleToCircleMode==1)
	{
		Circle1Circle2Length=R1+R2;
	}
	else if(CircleToCircleMode==2)
	{
		Circle1Circle2Length=abs(R1-R2);
	}
    RightLine1=abs(LineCircle1Length-R1);
	RightLine2=sqrt(Circle1Circle2Length*Circle1Circle2Length-RightLine1*RightLine1);
    ArcLineCenter=CalculateAcuteArc(RightLine1,Circle1Circle2Length);

	if(CircleToCircleMode==1)
	{
		if(abs(LineCircle1Length-LineCircleLength)<ZERO)
		{
			if(CircleR1CenterX>CircleRCenterX)
				ArcCircle1ToCircle2=0+ArcLine;
			else if(CircleR1CenterX<CircleRCenterX)
				ArcCircle1ToCircle2=PI+ArcLine;
			else if(CircleR1CenterY>CircleRCenterY)
				ArcCircle1ToCircle2=PI/2+ArcLine;	
			else if(CircleR1CenterY<CircleRCenterY)
				ArcCircle1ToCircle2=-PI/2+ArcLine;
		}
		else if(LineCircle1Length>LineCircleLength)
		{
			ArcCircle1ToCircle2=ArcLine-ArcLineCenter;
		}
		else if(LineCircle1Length<LineCircleLength)
		{
			ArcCircle1ToCircle2=ArcLine+ArcLineCenter;
		}
	}
	else if(CircleToCircleMode==2&&R1<R2)
	{
		if(abs(LineCircle1Length-LineCircleLength)<ZERO)
		{
			if(CircleR1CenterX>CircleRCenterX)
				ArcCircle1ToCircle2=0+ArcLine;
			else if(CircleR1CenterX<CircleRCenterX)
				ArcCircle1ToCircle2=PI+ArcLine;
			else if(CircleR1CenterY>CircleRCenterY)
				ArcCircle1ToCircle2=PI/2+ArcLine;
			else if(CircleR1CenterY<CircleRCenterY)
				ArcCircle1ToCircle2=-PI/2+ArcLine;
		}
		else if(LineCircle1Length>LineCircleLength)
		{
			ArcCircle1ToCircle2=ArcLine+ArcLineCenter+PI;
		}
		else if(LineCircle1Length<LineCircleLength)
		{
			ArcCircle1ToCircle2=PI+ArcLine-ArcLineCenter;
		}
	}
	else if(CircleToCircleMode==2&&R1>R2)
	{
		if(abs(LineCircle1Length-LineCircleLength)<ZERO)
		{
			if(CircleR1CenterX>CircleRCenterX)
				ArcCircle1ToCircle2=0+ArcLine;
			else if(CircleR1CenterX<CircleRCenterX)
				ArcCircle1ToCircle2=PI+ArcLine;
			else if(CircleR1CenterY>CircleRCenterY)
				ArcCircle1ToCircle2=PI/2+ArcLine;	
			else if(CircleR1CenterY<CircleRCenterY)
				ArcCircle1ToCircle2=-PI/2+ArcLine;
		}
		else if(LineCircle1Length>LineCircleLength)
		{
			ArcCircle1ToCircle2=ArcLine-ArcLineCenter;
		}
		else if(LineCircle1Length<LineCircleLength)
		{
			ArcCircle1ToCircle2=ArcLine+ArcLineCenter;
		}
	} 
    CalculateIntersection1(LineStartX,LineStartY,CircleR1CenterX, CircleR1CenterY,LineStartX,LineStartY,LineEndX,LineEndY,PiontAX,PointAY);	
 //计算两条直线交点
	if(CircleToCircleMode==2&&R1<R2)
		CalculatePointPosition(PiontAX,PointAY,RightLine2,ArcLine,PointBX,PointBY);//计算相对某点距离为Linelength，角度为Arc的点坐标
	else 
		CalculatePointPosition(PiontAX,PointAY,RightLine2,ArcLine+PI,PointBX,PointBY);//计算相对某点距离为Linelength，角度为Arc的点坐标
	LineEndX=PointBX;
	LineEndY=PointBY;
	CircleRStartX=PointBX;
	CircleRStartY=PointBY;

	CalculatePointPosition(CircleR1CenterX,CircleR1CenterY,Circle1Circle2Length,PI+ArcCircle1ToCircle2,PointCX,PointCY);
	CircleRCenterX=PointCX;
	CircleRCenterY=PointCY;
	if(CircleToCircleMode==2&&R1>R2)
		CalculatePointPosition(CircleR1CenterX,CircleR1CenterY,R2,ArcCircle1ToCircle2,PointDX,PointDY);
	else
		CalculatePointPosition(CircleR1CenterX,CircleR1CenterY,R2,PI+ArcCircle1ToCircle2,PointDX,PointDY);
	CircleREndX=PointDX;
	CircleREndY=PointDY;
	CircleR1StartX=PointDX;
	CircleR1StartY=PointDY;
	
	if(*upLowflag==0)
	{
		asChild[*num-1].X = LineEndX-LineStartX; 
		asChild[*num-1].Y = LineEndY-LineStartY;				

		asChild[*num].X = CircleREndX-CircleRStartX; 
		asChild[*num].Y = CircleREndY-CircleRStartY;
		asChild[*num].I = CircleRCenterX-CircleRStartX;  
		asChild[*num].J = CircleRCenterY-CircleRStartY;						

		asChild[*num+1].X = CircleR1EndX-CircleR1StartX; 
		asChild[*num+1].Y = CircleR1EndY-CircleR1StartY;
		asChild[*num+1].I = CircleR1CenterX-CircleR1StartX;  
		asChild[*num+1].J = CircleR1CenterY-CircleR1StartY;
	}
	else if(*upLowflag==1)
	{
		asChild[*num-1].B = LineEndX-LineStartX; 
		asChild[*num-1].C = LineEndY-LineStartY;				

		asChild[*num].B = CircleREndX-CircleRStartX; 
		asChild[*num].C = CircleREndY-CircleRStartY;
		asChild[*num].I1 = CircleRCenterX-CircleRStartX;  
		asChild[*num].J1 = CircleRCenterY-CircleRStartY;						

		asChild[*num+1].B = CircleR1EndX-CircleR1StartX; 
		asChild[*num+1].C = CircleR1EndY-CircleR1StartY;
		asChild[*num+1].I1 = CircleR1CenterX-CircleR1StartX;  
		asChild[*num+1].J1 = CircleR1CenterY-CircleR1StartY;
	}
}
 void G24LineToLine(double *X0,double *Y0,double *X01,double *Y01,double *I,double *J,double *X1,double *Y1,double *D0,double *D1,
           double *T0,double *T1,double *TempR,int *TempPathMode,double *ProgR,int *CompensateMode1,int *TapeMode1,int *num,int *upLowflag,nc_data *asChild)
{
	double ProgramR;//编程平面圆弧半径
	double R;//指定圆弧半径
	double Line1excursionD,Line2excursionD,Line1excursionT,Line2excursionT;//投影半径
	double Sx,Sy;//直线交点
	double L;
	double dx0,dy0,dx1,dy1;
	double Arc1,Arc2,Arc3;
	double excursionX1,excursionY1,excursionX2,excursionY2;//
	int    Model1;
	//编程平面和子平面各点坐标，相对于编程平面第一个圆弧起始点
	double Line1StartX,Line1StartY,Line1EndX,Line1EndY;
	double CircleRStartX,CircleRStartY,CircleREndX,CircleREndY,CircleRCenterX,CircleRCenterY;
	double Line2StartX,Line2StartY,Line2EndX,Line2EndY;

	Line1StartX=0.0;
	Line1StartY=0.0;
	Line1EndX=Line1StartX+(*X0);
	Line1EndY=Line1StartY+(*Y0);
	CircleRStartX=Line1EndX;
	CircleRStartY=Line1EndY;
	CircleREndX=CircleRStartX+(*X01);
	CircleREndY=CircleRStartY+(*Y01);
	CircleRCenterX=CircleRStartX+(*I);
	CircleRCenterY=CircleRStartY+(*J);
	ProgramR=*ProgR;

	Line1excursionD=(*D0)/sin((90-((*T0)/1000))*3.14/180.0);
	Line2excursionD=(*D1)/sin((90-((*T1)/1000))*3.14/180.0);
	Line1excursionT=xy_plane*tan((*T0)*3.14/1000/180);//下平面第一段线段的投影半径
	Line2excursionT=xy_plane*tan((*T1)*3.14/1000/180);//下平面第一段线段的投影半径
	
	if(*upLowflag==0)
	{
		R=*TempR;	
		if(R<0.0001)R=*ProgR+Line1excursionT;
	}
	else if(*upLowflag==1)
	{
		R=sqrt((*I)*(*I)+(*J)*(*J));
	}
    Model1=*CompensateMode1;
	CalculateLineExcursion((*X1),(*Y1),(Line2excursionD-Line1excursionD),Model1,*upLowflag,excursionX1,excursionY1);
    Model1= *TapeMode1;
	CalculateLineExcursion((*X1),(*Y1),(Line2excursionT-Line1excursionT),Model1,*upLowflag,excursionX2,excursionY2);

	Line2StartX=CircleREndX-(excursionX1+excursionX2);
	Line2StartY=CircleREndY-(excursionY1+excursionY2);
	Line2EndX=Line2StartX+(*X1);
	Line2EndY=Line2StartY+(*Y1);

	CalculateIntersection(Line1StartX,Line1StartY,Line1EndX, Line1EndY,Line2StartX,Line2StartY,Line2EndX,Line2EndY,Sx,Sy);

	Arc1=calculateLineArc(Line1EndX-Line1StartX,Line1EndY-Line1StartY);
	Arc2=calculateLineArc(Line2EndX-Line2StartX,Line2EndY-Line2StartY);
	if(Arc1<Arc2)
	{
		Arc3=3.14+(Arc1-Arc2);
	}
	else 
	{
		Arc3=3.14-(Arc1-Arc2);
	}
	if(abs(Arc3/2-PI/2)>ZERO)
	{  
	   L=R/tan(Arc3/2);
	}
	L=abs(L);

	dx0=L*cos(Arc1);
	dy0=L*sin(Arc1);
	dx1=L*cos(Arc2);
	dy1=L*sin(Arc2); 
	{
		double Arc;
		Arc=Arc2-Arc1;
		if(Arc<0)
		Arc=2*PI+Arc;
		if(*TempPathMode==2)
		{
			Arc=2*PI-Arc;
		}
        if(Arc>PI)
		{
			Line1EndX=Sx+dx0;
			Line1EndY=Sy+dy0;
			Line2StartX=Sx-dx1;
			Line2StartY=Sy-dy1;
		}
		else if(Arc<PI)
		{
			Line1EndX=Sx-dx0;
			Line1EndY=Sy-dy0;
			Line2StartX=Sx+dx1;
			Line2StartY=Sy+dy1;
		}
	}
    CircleRStartX=Line1EndX;
    CircleRStartY=Line1EndY;
    CircleREndX=Line2StartX;
    CircleREndY=Line2StartY;
	{
		double ArcR1,ArcR2,Arc;
		ArcR1=calculateLineArc(Line1EndX,Line1EndY);
		ArcR2=calculateLineArc(CircleREndX,CircleREndY);
		if(ArcR2>ArcR1)
			Arc=ArcR2-ArcR1;
		else
			Arc=2*PI+ArcR2-ArcR1;
		if(Arc>0)
		{
			CircleRCenterX=R*cos(Arc1-PI/2)+CircleRStartX;
			CircleRCenterY=R*sin(Arc1-PI/2)+CircleRStartY;
		}      
		else
		{
			CircleRCenterX=R*cos(Arc1+PI/2)+CircleRStartX;
			CircleRCenterY=R*sin(Arc1+PI/2)+CircleRStartY;
		}
	}
    if(*upLowflag==0)
	{
		asChild[*num-1].X = Line1EndX-Line1StartX; 
		asChild[*num-1].Y = Line1EndY-Line1StartY;
		
		asChild[*num].X = CircleREndX-CircleRStartX; 
		asChild[*num].Y = CircleREndY-CircleRStartY;
		asChild[*num].I = CircleRCenterX-CircleRStartX;  
		asChild[*num].J = CircleRCenterY-CircleRStartY;

		asChild[*num+1].X = Line2EndX-Line2StartX; 
		asChild[*num+1].Y = Line2EndY-Line2StartY;
    }
	else if(*upLowflag==1)
	{
		asChild[*num-1].B = Line1EndX-Line1StartX; 
		asChild[*num-1].C = Line1EndY-Line1StartY;
		
		asChild[*num].B = CircleREndX-CircleRStartX; 
		asChild[*num].C = CircleREndY-CircleRStartY;
		asChild[*num].I1 = CircleRCenterX-CircleRStartX;  
		asChild[*num].J1 = CircleRCenterY-CircleRStartY;
		
		asChild[*num+1].B = Line2EndX-Line2StartX; 
		asChild[*num+1].C = Line2EndY-Line2StartY;
	} 
}
void G24LineToCircle(double *X0,double *Y0,double *X01,double *Y01,double *I01,double *J01,double *X1,double *Y1,double *I1,double *J1,double *D0,double *D1,
           double *T0,double *T1,double *TempR,int CircleToCircleMode,int *TempPathMode,int *csPathMode,int *CompensateMode1,int *TapeMode1,int *num,int *upLowflag,int verticalflag,nc_data *asChild)
{//圆弧接直线
	double R1,R2;//下平面两个圆弧半径
	double Circle1excursionD,LineexcursionD,Circle1excursionT,LineexcursionT;//投影半径
	double ArcCircle1ToCircle2;//圆弧圆心连线与X正半轴夹角
	double ArcLine;//直线的倾斜角
	double Circle1Circle2Length;//圆弧圆心连线距离
	double LineCircleLength,LineCircle1Length;//圆心到直线的距离
	double ArcLineCenter;//直线与圆弧圆心连线的夹角
	double Coefficientx,Coefficienty,Intercept;//计算点到直线距离
	double RightLine1,RightLine2;
	double PiontAX,PointAY,PointBX,PointBY,PointCX,PointCY,PointDX,PointDY;
	double excursionX1,excursionY1,excursionX2,excursionY2;

	int CenterToLinePosition;

	//编程平面和子平面各点坐标，相对于编程平面第一个圆弧起始点
	double CircleR1StartX,CircleR1StartY,CircleR1EndX,CircleR1EndY,CircleR1CenterX,CircleR1CenterY;
	double CircleRStartX,CircleRStartY,CircleREndX,CircleREndY,CircleRCenterX,CircleRCenterY;
	double LineStartX,LineStartY,LineEndX,LineEndY;//
	//
	LineexcursionD=(*D0)/cos((*T0/1000)*PI/180.0);
	Circle1excursionD=(*D1)/cos((*T1/1000)*PI/180.0);
	if(*upLowflag==0)
	{
		LineexcursionT=xy_plane*tan((*T0)*PI/1000/180);//下平面第一段线段的投影半径
	    Circle1excursionT=xy_plane*tan((*T1)*PI/1000/180);//下平面第一段的投影半径		
	}
	else if(*upLowflag==1)
	{
		LineexcursionT=(h_thickness- xy_plane)*tan((*T0)*PI/1000/180);//上平面第一段线段的投影半径
	    Circle1excursionT=(h_thickness- xy_plane)*tan((*T1)*PI/1000/180);//上平面第一段的投影半径		
	}
	CircleR1StartX=0;
	CircleR1StartY=0;
	CircleR1EndX=CircleR1StartX+(*X1);
	CircleR1EndY=CircleR1StartY+(*Y1);
	CircleR1CenterX=CircleR1StartX+(*I1);
	CircleR1CenterY=CircleR1StartY+(*J1);
	
    CircleRStartX=CircleR1EndX;
	CircleRStartY=CircleR1EndY;
	CircleREndX=CircleRStartX+(*X01);
	CircleREndY=CircleRStartY+(*Y01);
	CircleRCenterX=CircleRStartX+(*I01);
	CircleRCenterY=CircleRStartY+(*J01);

	LineStartX=CircleREndX;
	LineStartY=CircleREndY;
	LineEndX=LineStartX+(*X0);
	LineEndY=LineStartY+(*Y0);

	CalculateLineExcursion(LineEndX-LineStartX,LineEndY-LineStartY,(LineexcursionD-Circle1excursionD),*CompensateMode1,*upLowflag,excursionX1,excursionY1);
    CalculateLineExcursion(LineEndX-LineStartX,LineEndY-LineStartY,(LineexcursionT-Circle1excursionT),*TapeMode1,*upLowflag,excursionX2,excursionY2);
	
	LineStartX=CircleREndX+(excursionX1+excursionX2);
	LineStartY=CircleREndY+(excursionY1+excursionY2);
	LineEndX=LineStartX+(*X0);
	LineEndY=LineStartY+(*Y0);

	R1=CalculatePointLength(CircleR1CenterX,CircleR1CenterY,CircleR1StartX,CircleR1StartY);
	R2=*TempR;	
	if(R2<0.0001)
	{
        R2=sqrt((*I01)*(*I01)+(*J01)*(*J01))+Circle1excursionT;
	}
	ArcLine=calculateLineArc(LineEndX-LineStartX,LineEndY-LineStartY);//计算直线与X轴正方向逆时针夹角
	LineCircleLength=R2;
	{
		if(LineEndX-LineStartX==0)
		{
		   Coefficienty=0;
		   Coefficientx=1;
		   Intercept=-LineStartX;
		}
		else 
		{
		   Coefficientx=(LineEndY-LineStartY)/(LineEndX-LineStartX);
		   Coefficienty=-1;
		   Intercept=(LineStartY)-Coefficientx*(LineStartX);
		}

       LineCircle1Length=CalculatePointtoLineLength(CircleR1CenterX,CircleR1CenterY, Coefficientx,Coefficienty,Intercept);
	}//计算圆心到sub面上的直线的距离

	if(CircleToCircleMode==1)
	{
        Circle1Circle2Length=R1+R2;
	}
	else if(CircleToCircleMode==2)
	{
        Circle1Circle2Length=abs(R1-R2);
	}

    RightLine1=abs(LineCircle1Length-LineCircleLength);
	RightLine2=sqrt(Circle1Circle2Length*Circle1Circle2Length-RightLine1*RightLine1);
    ArcLineCenter=CalculateAcuteArc(RightLine1,Circle1Circle2Length);
	if(verticalflag==1)
	{
		if(CircleToCircleMode==1)
		{
			if(abs(LineCircle1Length-LineCircleLength)<ZERO)
			{
				 if(CircleR1CenterX>CircleRCenterX)
					  ArcCircle1ToCircle2=ArcLine-0;
				 else if(CircleR1CenterX<CircleRCenterX)
					  ArcCircle1ToCircle2=ArcLine-PI;
				 else if(CircleR1CenterY>CircleRCenterY)
					  ArcCircle1ToCircle2=ArcLine-PI/2;			 
				 else if(CircleR1CenterY<CircleRCenterY)
					  ArcCircle1ToCircle2=ArcLine+PI/2;
			}
			else if(LineCircle1Length>LineCircleLength)
			{
				ArcCircle1ToCircle2=ArcLine-ArcLineCenter;
			}
			else if(LineCircle1Length<LineCircleLength)
			{
				ArcCircle1ToCircle2=ArcLine+ArcLineCenter;
			}//圆心1指向插入圆的直线角度
		}
		else if(CircleToCircleMode==2&&R1>R2)
		{
			if(abs(LineCircle1Length-LineCircleLength)<ZERO)
			{
				 if(CircleR1CenterX>CircleRCenterX)
					  ArcCircle1ToCircle2=ArcLine-0;
				 else if(CircleR1CenterX<CircleRCenterX)
					  ArcCircle1ToCircle2=ArcLine-PI;
				 else if(CircleR1CenterY>CircleRCenterY)
					  ArcCircle1ToCircle2=ArcLine-PI/2;			 
				 else if(CircleR1CenterY<CircleRCenterY)
					  ArcCircle1ToCircle2=ArcLine+PI/2;
			}
			else if(LineCircle1Length>LineCircleLength)
			{
				if(CircleR1CenterX>CircleRCenterX)
				   ArcCircle1ToCircle2=ArcLine+ArcLineCenter+PI;
			 }
			 else if(LineCircle1Length<LineCircleLength)
			 {
					ArcCircle1ToCircle2=ArcLine-ArcLineCenter+PI;
			 }
		}
		else if(CircleToCircleMode==2&&R1<R2)
		{
			if(abs(LineCircle1Length-LineCircleLength)<ZERO)
			{
				 if(CircleR1CenterX>CircleRCenterX)
					  ArcCircle1ToCircle2=ArcLine-0;
				 else if(CircleR1CenterX<CircleRCenterX)
					  ArcCircle1ToCircle2=ArcLine-PI;
				 else if(CircleR1CenterY>CircleRCenterY)
					  ArcCircle1ToCircle2=ArcLine-PI/2;			 
				 else if(CircleR1CenterY<CircleRCenterY)
					  ArcCircle1ToCircle2=ArcLine+PI/2;
			}
			else if(LineCircle1Length>LineCircleLength)
			{
				   ArcCircle1ToCircle2=ArcLine-ArcLineCenter;
			 }
			 else if(LineCircle1Length<LineCircleLength)
			 {
					ArcCircle1ToCircle2=ArcLine+ArcLineCenter;
			 }//圆心1指向插入圆的直线角度
		}
		CalculateIntersection1(LineStartX,LineStartY,CircleR1CenterX, CircleR1CenterY,LineStartX,LineStartY,LineEndX,LineEndY,PiontAX,PointAY);	
	 //计算两条直线交点
		if(CircleToCircleMode==2&&R1>R2)
			CalculatePointPosition(PiontAX,PointAY,RightLine2,ArcLine+PI,PointBX,PointBY);//计算相对某点距离为Linelength，角度为Arc的点坐标
		else
			CalculatePointPosition(PiontAX,PointAY,RightLine2,ArcLine,PointBX,PointBY);//计算相对某点距离为Linelength，角度为Arc的点坐标
		LineStartX=PointBX;
		LineStartY=PointBY;
		CircleREndX=PointBX;
		CircleREndY=PointBY;

		CalculatePointPosition(CircleR1CenterX,CircleR1CenterY,Circle1Circle2Length,ArcCircle1ToCircle2,PointCX,PointCY);
		CircleRCenterX=PointCX;
		CircleRCenterY=PointCY;

		if(CircleToCircleMode==2&&R1<R2)
			CalculatePointPosition(CircleR1CenterX,CircleR1CenterY,R1,PI+ArcCircle1ToCircle2,PointDX,PointDY);			
		else 
			CalculatePointPosition(CircleR1CenterX,CircleR1CenterY,R1,ArcCircle1ToCircle2,PointDX,PointDY);
		CircleRStartX=PointDX;
		CircleRStartY=PointDY;
		CircleR1EndX=PointDX;
		CircleR1EndY=PointDY;
	}
	if(*upLowflag==0)
	{
		asChild[*num-1].X = CircleR1EndX-CircleR1StartX; 
		asChild[*num-1].Y = CircleR1EndY-CircleR1StartY;
		asChild[*num-1].I = CircleR1CenterX-CircleR1StartX;  
		asChild[*num-1].J = CircleR1CenterY-CircleR1StartY;				

		asChild[*num].X = CircleREndX-CircleRStartX; 
		asChild[*num].Y = CircleREndY-CircleRStartY;
		asChild[*num].I = CircleRCenterX-CircleRStartX;  
		asChild[*num].J = CircleRCenterY-CircleRStartY;						

		asChild[*num+1].X = LineEndX-LineStartX; 
		asChild[*num+1].Y = LineEndY-LineStartY;	
	}
	else if(*upLowflag==1)
	{
		asChild[*num-1].B = CircleR1EndX-CircleR1StartX; 
		asChild[*num-1].C = CircleR1EndY-CircleR1StartY;
		asChild[*num-1].I1 = CircleR1CenterX-CircleR1StartX;  
		asChild[*num-1].J1 = CircleR1CenterY-CircleR1StartY;				

		asChild[*num].B = CircleREndX-CircleRStartX; 
		asChild[*num].C = CircleREndY-CircleRStartY;
		asChild[*num].I1 = CircleRCenterX-CircleRStartX;  
		asChild[*num].J1 = CircleRCenterY-CircleRStartY;						

		asChild[*num+1].B = LineEndX-LineStartX; 
		asChild[*num+1].C = LineEndY-LineStartY;	
	}
}