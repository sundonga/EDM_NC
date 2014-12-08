#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;								// ��ǰʵ��
extern GraphParam EditGraphParam;
//***************************
//Բ���ܻ�������һԲ�����Ƚ���calculateNumber�ȷ֣����ÿ�ȷֵ��ҳ�С��minLength����minLength�����ܻ����������maxLength����maxLength�����ܻ�
int calculateNumber;//Բ���ܻ��ܻ�����
double minLength;//Բ���ܻ���С�ҳ�
double maxLength;//Բ���ܻ�����ҳ�
double cutlerRadius;
//***************************
extern double h_thickness;//�������
extern double now_up_X;//��ǰ��ƽ��X����
extern double now_up_Y;//��ǰ��ƽ��Y����
extern double now_up_Z;//��ǰ��ƽ��Z����
extern double now_down_X;//��ǰ��ƽ��X����
extern double now_down_Y;//��ǰ��ƽ��Y����
extern double now_down_Z;//��ǰ��ƽ��Z����
extern double pre_up_X;//ǰһ����ƽ��X����
extern double pre_up_Y;//ǰһ����ƽ��Y����
extern double pre_up_Z;//ǰһ����ƽ��Z����
extern double pre_down_X;//ǰһ����ƽ��X����
extern double pre_down_Y;//ǰһ����ƽ��Y����
extern double pre_down_Z;//ǰһ����ƽ��Z����
extern void Draw();//��������24������ֵ����һ���ı���
extern void CalNormal(double*,double*,double*,double*);//���������������ƽ��ķ�����
//***************************
double ZERO = 0.0001;//�������ʱ0
//******************************************
double calculateLineArc(double PointX,double PointY);//����ֱ����X����������ʱ��н�
double calculateArc(double centerPointX,double centerPointY,double StartPointX,double StartPointY,double endPointX,
					double endPointY,int circularMode);//����Բ���Ƕȣ�����circularMode����������Բ����˳԰������Բ������ͬG03,G02
double calculateChordLength(double R,double angle);//�����ҳ�
void calculateEndPoint(double NowArc,double PreArc,double R,double &PointX,double &PointY);//����Բ����һ������
int calculateCircleMode(double R,double Arc);//����ܻ�ģʽ������0��ʾ�ܻ�ģʽΪ��calculateNumber�ܻ�������1��ʾ��minLength�ܻ�������2��ʾ��maxLength�ܻ�
//��Բ����ֱ�ߡ�WhichIsUp��������Բ������ֱ������ƽ�棬1��ʾֱ�ߣ�������ʾԲ��
void CircleLine(double dLineX,double dLineY,double ArcCircle,double ArcStart,int caculateNumber,double R,int circleMode,double WhichIsUp);
void CircleToLine(double LineStartX,double LineStartY,double LineEndX,double LineEndY,double CircleCenterX,
				  double CircleCenterY,double CircleStartX,double CircleStartY,double CircleEndX,double CircleEndY,
				  int circularMode,int UpMode);//UpMode==1��һ����������ƽ��
void CirclePoint(double ArcCircle,int caculateNumber,double ArcCircleStart,double R,int circleMode,int WhichIsUp);//���Բ���ܻ�
void CircleCircle(double ArcCircle1,double R1,double ArcCircle1Start,int circle1Mode,double ArcCircle2,double ArcCircle2Start,
				  double R2,int circle2Mode,int WhichIsUp,int caculateNumber);//Բ����Բ���ܻ�
void CircleToCircle(double Circle1CenterX,double Circle1CenterY,double Circle1StartX,double Circle1StartY,
					double Circle1EndX,double Circle1EndY,double Circle2CenterX,double Circle2CenterY,
					double Circle2StartX,double Circle2StartY,double Circle2EndX,double Circle2EndY,
					int circular1Mode,int circular2Mode,int UpMode);
void LineToLine(double Line1StartX,double Line1StartY,double Line1EndX,double Line1EndY,
				double Line2StartX,double Line2StartY,double Line2EndX,double Line2EndY);
void LineLine(double dLine1X,double dLine1Y,double dLine2X,double dLine2Y,int number);
void drawColumn(double UpperX,double UpperY,double UpperZ,double DownX,double DownY,double DownZ);//2010-5-26
void Draw(double UX1,double UY1,double UZ1,double UX2,double UY2,double UZ2,
		  double DX1,double DY1,double DZ1,double DX2,double DY2,double DZ2,
		  double UCX,double UCY,double UCZ,double DCX,double DCY,double DCZ);//2010-5-26
//*******************************************

double calculateLineArc(double PointX,double PointY)//����ֱ����X��������н�
{
	if(abs(PointX)<ZERO&&PointY>0)//Y������
		return PI/2.0;
	else if(abs(PointY)<ZERO&&PointX<0)//X������
		return PI;
	else if(abs(PointX)<ZERO&&PointY<0)//Y������
		return 3*PI/2;
	else if(abs(PointY)<ZERO&&PointX>0)//X������
		return 0.0;
	else if(PointX>0&&PointY>0)//��һ����
		return atan(PointY/PointX);
	else if(PointX<0&&PointY>0)//�ڶ�����
		return atan(PointY/PointX)+PI;
	else if(PointX<0&&PointY<0)//��������
		return atan(PointY/PointX)+PI;
	else if(PointX>0&&PointY<0)//��������
		return 2*PI+atan(PointY/PointX);
	else if(PointX<ZERO&&PointY<ZERO)//ԭ��
		return 0.0;
	return 0.0;
}
//******************************
double calculateArc(double centerPointX,double centerPointY,double StartPointX,double StartPointY,double endPointX,
					double endPointY,int circularMode)//����Բ���Ƕ�
{
	double ArcStart,ArcEnd;
	double PointX,PointY;
	double Arc;
	//������ʼ����Բ�����߶�X��������н�
	PointX=StartPointX-centerPointX;
	PointY=StartPointY-centerPointY;
	ArcStart=calculateLineArc(PointX,PointY);
	//�����յ���Բ�����߶�X��������н�
	PointX=endPointX-centerPointX;
	PointY=endPointY-centerPointY;
	ArcEnd=calculateLineArc(PointX,PointY);
	if(ArcEnd>ArcStart)
		Arc=ArcEnd-ArcStart;
	else
		Arc=2*PI+ArcEnd-ArcStart;
	//˳Բ
	if(circularMode==2)
		Arc=2*PI-Arc;
	return Arc;
}
//**********************************
double calculateChordLength(double R,double angle)//�����ҳ�
{
	return 2*R*sin(angle/2.0);
}
//*****************************************
void calculateEndPoint(double NowArc,double PreArc,double R,double &PointX,double &PointY)//����Բ����һ������
{
	PointX=R*cos(NowArc)-R*cos(PreArc);
	PointY=R*sin(NowArc)-R*sin(PreArc);
}
//********************************************
int calculateCircleMode(double R,double Arc)//����ܻ�ģʽ������0��ʾ�ܻ�ģʽΪ��calculateNumber�ܻ�������1��ʾ��minLength�ܻ�������2��ʾ��maxLength�ܻ�
{
	double Length;
	Length=calculateChordLength(R,Arc/calculateNumber);
	if(Length>maxLength)
		return 2;
	else if(Length<minLength)
		return 1;
	else return 0;
	return 0;
}
//*********************************************
void CircleLine(double dLineX,double dLineY,double ArcCircle,double ArcStart,int caculateNumber,double R,int circleMode,double WhichIsUp)
{
	double dArc;
	int k;
	double NextPointX,NextPointY;
	dArc=ArcCircle/caculateNumber;//Բ���ܻ���ÿ���Ҷ�ӦԲ�Ľ�
	for(k=1;k<caculateNumber+1;k++)
	{
		if(circleMode==2)
		{
			//˳Բ��ʼ�Ƕȼ�ȥ�ܻ��Ƕ�
			calculateEndPoint(ArcStart-dArc*k,ArcStart-dArc*(k-1),R,NextPointX,NextPointY);
		}
		else
		{
			//��Բ��ʼ�Ƕȼ����ܻ��Ƕ�
			calculateEndPoint(ArcStart+dArc*k,ArcStart+dArc*(k-1),R,NextPointX,NextPointY);
		}
		if(WhichIsUp==1)
		{
			now_up_X=now_up_X+dLineX;
			now_up_Y=now_up_Y+dLineY;
			now_down_X=now_down_X+NextPointX;
			now_down_Y=now_down_Y+NextPointY;
		}
		else
		{
			now_down_X=now_down_X+dLineX;
			now_down_Y=now_down_Y+dLineY;
			now_up_X=now_up_X+NextPointX;
			now_up_Y=now_up_Y+NextPointY;
		}
		Draw();
	}
}
//*********************************************
void CircleToLine(double LineStartX,double LineStartY,double LineEndX,double LineEndY,double CircleCenterX,
				  double CircleCenterY,double CircleStartX,double CircleStartY,double CircleEndX,double CircleEndY,
				  int circularMode,int UpMode)
{
	double R,ArcCircle,LineLength,dArc,ArcCircleStart,ArcLine,dLineLength;
	double dLineX,dLineY;
	int i,j;
	//���Բ�뾶��ֱ�߳�
	R=sqrt((CircleStartX-CircleCenterX)*(CircleStartX-CircleCenterX)+(CircleStartY-CircleCenterY)*(CircleStartY-CircleCenterY));
	LineLength=sqrt((LineEndX-LineStartX)*(LineEndX-LineStartX)+(LineEndY-LineStartY)*(LineEndY-LineStartY));
	//ԲΪ�㣬��Ϊ���ֱ�ߣ�ֱ�ӻ�ͼ
	if(R<ZERO)
	{
		if(UpMode==1)
			{
				now_up_X=now_up_X+LineEndX-LineStartX;
				now_up_Y=now_up_Y+LineEndY-LineStartY;
				now_down_X=now_down_X;
				now_down_Y=now_down_Y;
			}
			else
			{
				now_down_X=now_down_X+LineEndX-LineStartX;
				now_down_Y=now_down_Y+LineEndY-LineStartY;
				now_up_X=now_up_X;
				now_up_Y=now_up_Y;
			}
			Draw();
	}
	else
	{
		//Բ�����Ľ�
		ArcCircle=calculateArc(CircleCenterX,CircleCenterY,CircleStartX,CircleStartY,CircleEndX,CircleEndY,circularMode);
		//Բ����ʼ����Բ������ͬX��н�
		ArcCircleStart=calculateLineArc(CircleStartX-CircleCenterX,CircleStartY-CircleCenterY);
		//ֱ����X��н�
		ArcLine=calculateLineArc(LineEndX-LineStartX,LineEndY-LineStartX);
		i=calculateCircleMode(R,ArcCircle);
		if(i==0)
		{
			j=calculateNumber;
			dArc=ArcCircle/j;
			dLineLength=LineLength/j;//ֱ���ܻ�
			dLineX=dLineLength*cos(ArcLine);
			dLineY=dLineLength*sin(ArcLine);
			CircleLine(dLineX,dLineY,ArcCircle,ArcCircleStart,j,R,circularMode,UpMode);
		}
		else if(i==1)
		{
			//Բ����С����С�ҳ�����Բ�����񣬻�����С�ҳ���Ӧ��Բ�ĽǴ���Բ��Բ�Ľǣ�ֱ�ӻ�ͼ
			if(minLength>=2*R||asin(minLength/(2*R))>ArcCircle/2)
			{
				if(UpMode==1)
				{
					now_up_X=now_up_X+LineEndX;
					now_up_Y=now_up_Y+LineEndY;
					now_down_X=now_down_X+CircleEndX;
					now_down_Y=now_down_Y+CircleEndY;
				}
				else
				{
					now_down_X=now_down_X+LineEndX;
					now_down_Y=now_down_Y+LineEndY;
					now_up_X=now_up_X+CircleEndX;
					now_up_Y=now_up_Y+CircleEndY;
				}
				Draw();
			}
			else
			{
				//��minLength�ܻ�
				j=asin(minLength/(2*R))/(ArcCircle/2);
				dArc=ArcCircle/j;
				dLineLength=LineLength/j;
				dLineX=dLineLength*cos(ArcLine);
				dLineY=dLineLength*sin(ArcLine);
				CircleLine(dLineX,dLineY,ArcCircle,ArcCircleStart,j,R,circularMode,UpMode);				
			}
		}
		else if(i==2)
		{
			//��maxLength�ܻ�
			j=asin(maxLength/(2*R))/(ArcCircle/2);
			dArc=ArcCircle/j;
			dLineLength=LineLength/j;
			dLineX=dLineLength*cos(ArcLine);
			dLineY=dLineLength*sin(ArcLine);
			CircleLine(dLineX,dLineY,ArcCircle,ArcCircleStart,j,R,circularMode,UpMode);
		}
	}
}
//*********************************************
void CirclePoint(double ArcCircle,int caculateNumber,double ArcCircleStart,double R,int circleMode,int WhichIsUp)//���Բ���ܻ�
{
	int k;
	double dArc;
	double NextPointX,NextPointY;
	dArc=ArcCircle/caculateNumber;
	for(k=1;k<caculateNumber+1;k++)
	{
		if(circleMode==2)
		{
			calculateEndPoint(ArcCircleStart-dArc*k,ArcCircleStart-dArc*(k-1),R,NextPointX,NextPointY);
		}
		else
		{
			calculateEndPoint(ArcCircleStart+dArc*k,ArcCircleStart+dArc*(k-1),R,NextPointX,NextPointY);
		}
		if(WhichIsUp==1)
		{
			now_up_X=now_up_X;
			now_up_Y=now_up_Y;
			now_down_X=now_down_X+NextPointX;
			now_down_Y=now_down_Y+NextPointY;
		}
		else
		{
			now_down_X=now_down_X;
			now_down_Y=now_down_Y;
			now_up_X=now_up_X+NextPointX;
			now_up_Y=now_up_Y+NextPointY;
		}
		Draw();
	}
}
//*********************************************
void CircleCircle(double ArcCircle1,double R1,double ArcCircle1Start,int circle1Mode,double ArcCircle2,double ArcCircle2Start,
				  double R2,int circle2Mode,int WhichIsUp,int caculateNumber)//Բ����Բ���ܻ�
{
	int k;
	double dArc1,dArc2;
	double NextPoint1X,NextPoint1Y,NextPoint2X,NextPoint2Y;

	dArc1=ArcCircle1/caculateNumber;
	dArc2=ArcCircle2/caculateNumber;

	for(k=1;k<caculateNumber+1;k++)
	{
		if(circle1Mode==2)
		{
			calculateEndPoint(ArcCircle1Start-dArc1*k,ArcCircle1Start-dArc1*(k-1),R1,NextPoint1X,NextPoint1Y);
		}
		else
		{
			calculateEndPoint(ArcCircle1Start+dArc1*k,ArcCircle1Start+dArc1*(k-1),R1,NextPoint1X,NextPoint1Y);
		}
		if(circle2Mode==2)
		{
			calculateEndPoint(ArcCircle2Start-dArc2*k,ArcCircle2Start-dArc2*(k-1),R2,NextPoint2X,NextPoint2Y);
		}
		else
		{
			calculateEndPoint(ArcCircle2Start+dArc2*k,ArcCircle2Start+dArc2*(k-1),R2,NextPoint2X,NextPoint2Y);
		}
		if(WhichIsUp==1)
		{
			now_up_X=now_up_X+NextPoint1X;
			now_up_Y=now_up_Y+NextPoint1Y;
			now_down_X=now_down_X+NextPoint2X;
			now_down_Y=now_down_Y+NextPoint2Y;
		}
		else
		{
			now_down_X=now_down_X+NextPoint1X;
			now_down_Y=now_down_Y+NextPoint1Y;
			now_up_X=now_up_X+NextPoint2X;
			now_up_Y=now_up_Y+NextPoint2Y;
		}
		Draw();
	}
}
//*********************************************
void CircleToCircle(double Circle1CenterX,double Circle1CenterY,double Circle1StartX,double Circle1StartY,
					double Circle1EndX,double Circle1EndY,double Circle2CenterX,double Circle2CenterY,
					double Circle2StartX,double Circle2StartY,double Circle2EndX,double Circle2EndY,
					int circular1Mode,int circular2Mode,int UpMode)
{
	double R1,R2;
	double ArcCircle1,ArcCircle2;
	double ArcCircle1Start,ArcCircle2Start;	
	int Circle1_is_point,Circle2_is_point;
	int i1,i2,j;
	int j1,j2;
//**********************************
	R1=sqrt((Circle1StartX-Circle1CenterX)*(Circle1StartX-Circle1CenterX)+(Circle1StartY-Circle1CenterY)*(Circle1StartY-Circle1CenterY));
	R2=sqrt((Circle2StartX-Circle2CenterX)*(Circle2StartX-Circle2CenterX)+(Circle2StartY-Circle2CenterY)*(Circle2StartY-Circle2CenterY));
//**********************************
//�ж�Բ���Ƿ�Ϊ��
	if(R1<ZERO)Circle1_is_point=1;
	else Circle1_is_point=0;

	if(R2<ZERO)Circle2_is_point=1;
	else Circle2_is_point=0;
//**********************************
//���Բ��Ϊ�����Ӧ��ArcCircle��ArcCircleStartֱ��ȡ0
	if(Circle1_is_point)
	{
		ArcCircle1=0.0;
		ArcCircle1Start=0.0;
	}
	else
	{
		ArcCircle1=calculateArc(Circle1CenterX,Circle1CenterY,Circle1StartX,Circle1StartY,Circle1EndX,Circle1EndY,circular1Mode);
		ArcCircle1Start=calculateLineArc(Circle1StartX-Circle1CenterX,Circle1StartY-Circle1CenterY);
	}

	if(Circle2_is_point)
	{
		ArcCircle2=0.0;
		ArcCircle2Start=0.0;
	}
	else
	{
		ArcCircle2=calculateArc(Circle2CenterX,Circle2CenterY,Circle2StartX,Circle2StartY,Circle2EndX,Circle2EndY,circular2Mode);
		ArcCircle2Start=calculateLineArc(Circle2StartX-Circle2CenterX,Circle2StartY-Circle2CenterY);
	}
//**********************************
//���Բ��Ϊ���򲻽����ܻ�
	if(Circle1_is_point)i1=-1;
	else i1=calculateCircleMode(R1,ArcCircle1);
	if(Circle2_is_point)i2=-1;
	else i2=calculateCircleMode(R2,ArcCircle2);	
//**********************************
//����Բ��Ϊ������������Բ�������ܻ���ͼ
	if(Circle1_is_point||Circle2_is_point)
	{
		if(i1==0)
		{
			j=calculateNumber;
		}
		else if(i1==1)
		{
			if(minLength>=2*R1||asin(minLength/(2*R1))>ArcCircle1/2)
			{
				j=1;
			}
			else
			{
				j=asin(minLength/(2*R1))/(ArcCircle1/2);
			}
		}
		else if(i1==2)
		{
			j=asin(maxLength/(2*R1))/(ArcCircle1/2);
		}
		else if(i2==0)
		{
			j=calculateNumber;
		}
		else if(i2==1)
		{
			if(minLength>=2*R2||asin(minLength/(2*R2))>ArcCircle2/2)
			{
				j=1;
			}
			else
			{
				j=asin(minLength/(2*R2))/(ArcCircle2/2);
			}
		}
		else if(i2==2)
		{
			j=asin(maxLength/(2*R2))/(ArcCircle2/2);
		}

		if(Circle1_is_point)CirclePoint(ArcCircle2,j,ArcCircle2Start,R2,circular2Mode,UpMode);
		else if(Circle2_is_point)CirclePoint(ArcCircle1,j,ArcCircle1Start,R1,circular1Mode,UpMode);
	}
	else
	{
		if(i1==0)
		{
			j1=calculateNumber;
		}
		else if(i1==1)
		{
			if(minLength>=2*R1||asin(minLength/(2*R1))>ArcCircle1/2)
			{
				j1=1;
			}
			else
			{
				j1=asin(minLength/(2*R1))/(ArcCircle1/2);
			}
		}
		else if(i1==2)
		{
			j1=asin(maxLength/(2*R1))/(ArcCircle1/2);
		}
		if(i2==0)
		{
			j2=calculateNumber;
		}
		else if(i2==1)
		{
			if(minLength>=2*R2||asin(minLength/(2*R2))>ArcCircle2/2)
			{
				j2=1;
			}
			else
			{
				j2=asin(minLength/(2*R2))/(ArcCircle2/2);
			}
		}
		else if(i2==2)
		{
			j2=asin(maxLength/(2*R2))/(ArcCircle2/2);
		}
		if(j1<j2)j=j2;
		else j=j1;
		CircleCircle(ArcCircle1,R1,ArcCircle1Start,circular1Mode,ArcCircle2,ArcCircle2Start,R2,circular2Mode,UpMode,j);
	}
}
//******************************************
void drawColumn(double UpperX,double UpperY,double UpperZ,double DownX,double DownY,double DownZ)
{
	double PointUpperX[6],PointUpperY[6],PointDownX[6],PointDownY[6];
	int i;
	double Arc;
	Arc=0.0;
	glColor4f(1.0,0.0,0.0,1.0);//����ͼ����ɫ
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	for(i=0;i<6;i++)
	{
		PointUpperX[i]=PointUpperY[i]=PointDownX[i]=PointDownY[i]=0.0;
	}
	for(i=0;i<6;i++)
	{		
		PointUpperX[i]=UpperX+cutlerRadius*cos(Arc);
		PointUpperY[i]=UpperY+cutlerRadius*sin(Arc);
		PointDownX[i]=DownX+cutlerRadius*cos(Arc);
		PointDownY[i]=DownY+cutlerRadius*sin(Arc);
		Arc+=PI/3;
	}
	for(i=0;i<5;i++)
	{
		Draw(PointUpperX[i],PointUpperY[i],UpperZ,PointUpperX[i+1],PointUpperY[i+1],UpperZ,
			PointDownX[i],PointDownY[i],DownZ,PointDownX[i+1],PointDownY[i+1],DownZ,
			UpperX,UpperY,UpperZ,DownX,DownY,DownZ);
	}
	Draw(PointUpperX[5],PointUpperY[5],UpperZ,PointUpperX[0],PointUpperY[0],UpperZ,
			PointDownX[5],PointDownY[5],DownZ,PointDownX[0],PointDownY[0],DownZ,
			UpperX,UpperY,UpperZ,DownX,DownY,DownZ);
}
//******************************************
void Draw(double UX1,double UY1,double UZ1,double UX2,double UY2,double UZ2,
		  double DX1,double DY1,double DZ1,double DX2,double DY2,double DZ2,
		  double UCX,double UCY,double UCZ,double DCX,double DCY,double DCZ)
{
	double p11[3],p12[3],p21[3],p22[3],c1[3],c2[3];
	double n[3];
	p11[0]=UX1;
	p11[1]=UY1;
	p11[2]=UZ1;
	p12[0]=UX2;
	p12[1]=UY2;
	p12[2]=UZ2;
	p21[0]=DX1;
	p21[1]=DY1;
	p21[2]=DZ1;
	p22[0]=DX2;
	p22[1]=DY2;
	p22[2]=DZ2;
	c1[0]=UCX;
	c1[1]=UCY;
	c1[2]=UCZ;
	c2[0]=DCX;
	c2[1]=DCY;
	c2[2]=DCZ;
	if(p11[0]==p12[0]&&p11[1]==p12[1])CalNormal(p11,p21,p22,n);//����ƽ�淨����
	else CalNormal(p11,p12,p21,n);//����ƽ�淨����
	//�ı��λ��Ʒ������֣��ֱ���������εõ��ı���
	glBegin(GL_TRIANGLES);
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f(p11[0],p11[1],p11[2]);
		glVertex3f(p12[0],p12[1],p12[2]);
		glVertex3f(p21[0],p21[1],p21[2]);
	glEnd();
	glBegin(GL_TRIANGLES);
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f(p12[0],p12[1],p12[2]);
		glVertex3f(p21[0],p21[1],p21[2]);
		glVertex3f(p22[0],p22[1],p22[2]);
	glEnd();
	CalNormal(p11,p12,c1,n);
	glBegin(GL_TRIANGLES);
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f(p11[0],p11[1],p11[2]);
		glVertex3f(p12[0],p12[1],p12[2]);
		glVertex3f(c1[0],c1[1],c1[2]);
	glEnd();
	CalNormal(p21,p22,c2,n);
	glBegin(GL_TRIANGLES);
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f(p21[0],p21[1],p21[2]);
		glVertex3f(p22[0],p22[1],p22[2]);
		glVertex3f(c2[0],c2[1],c2[2]);
	glEnd();
}
//******************************************
void LineLine(double dLine1X,double dLine1Y,double dLine2X,double dLine2Y,int number)
{
	int i;
	for(i=0;i<number;i++)
	{
		now_down_X=now_down_X+dLine2X;
		now_down_Y=now_down_Y+dLine2Y;
		now_up_X=now_up_X+dLine1X;
		now_up_Y=now_up_Y+dLine1Y;
		Draw();
	}
}
//******************************************
void LineToLine(double Line1StartX,double Line1StartY,double Line1EndX,double Line1EndY,
				double Line2StartX,double Line2StartY,double Line2EndX,double Line2EndY)
{
	double dLine1X,dLine1Y;
	double dLine2X,dLine2Y;
	int Line1Number;
	int Line2Number;
	int Number;
	double ArcLine1,ArcLine2;
	double Line1Length,Line2Length;
	double dLine1Length,dLine2Length;
	Line1Length=sqrt((Line1EndX-Line1StartX)*(Line1EndX-Line1StartX)+(Line1EndY-Line1StartY)*(Line1EndY-Line1StartY));
	Line2Length=sqrt((Line2EndX-Line2StartX)*(Line2EndX-Line2StartX)+(Line2EndY-Line2StartY)*(Line2EndY-Line2StartY));
	////ֱ����X��н�
	ArcLine1=calculateLineArc(Line1EndX-Line1StartX,Line1EndY-Line1StartX);
	ArcLine2=calculateLineArc(Line2EndX-Line2StartX,Line2EndY-Line2StartX);
	Line1Number=(int)(Line1Length/(cutlerRadius*2));
	Line2Number=(int)(Line2Length/(cutlerRadius*2));
	if(Line1Number>Line2Number)Number=Line1Number;
	else Number=Line2Number;
	dLine1Length=Line1Length/Number;
	dLine2Length=Line2Length/Number;
	dLine1X=dLine1Length*cos(ArcLine1);
	dLine1Y=dLine1Length*sin(ArcLine1);
	dLine2X=dLine2Length*cos(ArcLine2);
	dLine2Y=dLine2Length*sin(ArcLine2);
	LineLine(dLine1X,dLine1Y,dLine2X,dLine2Y,Number);
}