#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;								// 当前实例
int AutographFirstEnterFlag;  //自动图形显示函数第一次进入标志

extern GraphParam AutoGraphParam;
extern  int auto_regraph_flag;

extern Coordata data_m;  //Machine coor
extern Coordata data_w;   //work coor
extern Coordata data_r;   //ralitive coor
extern Coorpuls pulse_m; //Machine pulse
extern Coorpuls pulse_w; //work pulse
//2010-5-26锥加工用参数
extern double z_position;
extern double xy_plane;
extern double h_thickness;
extern double HLOWDISTANCE;
extern int NowAutoGraph2D_3D;//2010-5-27
//end 2010-5-26
extern void Auto3DDraw();
extern int NowGraphPlane;
int AutoEndPaint;

void autodraw(HWND hWnd,double width,double length,double  mw, double ml);
void caculateTapPoint(double&,double&,double&,double&);
void PaintMark(double x,double y,double Arc,HDC PCD);
extern double calculateLineArc(double PointX,double PointY);//计算直线与X轴正方向逆时针夹角

const MessageProc autographMessages[]={
	WM_CREATE,               goCreateAutograph,
	WM_TIMER,                goTimerAutograph,
	WM_PAINT,                goPaintAutograph,
	WM_DRAWITEM,              goDrawitemMain,
	WM_DESTROY,              goDestroyAutograph
};
int MyRegisterClassAutograph(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	INITCOMMONCONTROLSEX icex;
	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_DBLCLKS;//指示可以发送双击消息到窗口
	wcex.lpfnWndProc	= (WNDPROC)autographWndProc;//指向窗口过程的长指针
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;//指向当前窗口过程所在的实例句柄
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);背景黑色画刷？？？？
	wcex.lpszMenuName	= NULL;//(LPCTSTR)ID_Menu;//(LPCTSTR)IDC_EX1;（windows CE不支持菜单）？？？？
	wcex.lpszClassName	= TEXT("AUTOGRAPHWND");//指定自定义窗口类的名称
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);//？？？？
//注册编辑窗口类
	if(!RegisterClassEx(&wcex)) return 0;
	icex.dwICC=ICC_LISTVIEW_CLASSES;//???????
	icex.dwSize=sizeof(INITCOMMONCONTROLSEX);//?????
	InitCommonControlsEx(&icex);//???
	return 1;
}
//编辑窗口过程体
LRESULT CALLBACK autographWndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;

	//查找对应的消息ID,并执行对应的消息处理过程
	for(i=0; i<dim(autographMessages);i++)
	{
		if(msgCode == autographMessages[i].uCode) return(*autographMessages[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//对不存在的消息，调用缺省窗口过程
	return DefWindowProc(hWnd, msgCode, wParam, lParam);
}
LRESULT goCreateAutograph(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	AutoEndPaint=0;
	SetTimer(hWnd,TIMER_AUTOGRAPH,200,NULL);
	return 0;
}//定时器？？？？

LRESULT goTimerAutograph(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	
	if(wParam==TIMER_AUTOGRAPH)
	{
		if(auto_regraph_flag == 1)
		{
			if(AutoEndPaint==0)
			{
				if(NowAutoGraph2D_3D==2)autodraw(AutoGraphParam.hWnd,AutoGraphParam.draw_width,AutoGraphParam.draw_length,
					AutoGraphParam.mw, AutoGraphParam.ml);
				else Auto3DDraw();
			}
		}
	}
	return 0;
}//?????
LRESULT goPaintAutograph(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;//定义画板结构体对象
	HDC hdc;//定义设备环境句柄，hdc显示设备环境
	hdc = BeginPaint(hWnd, &ps);//获得设备环境，与EndPaint配对使用	
	EndPaint(hWnd, &ps);
	return 0;
}//WM-PAINT消息处理过程

LRESULT goDestroyAutograph(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	PostQuitMessage(0);//向主窗口发送WM_QUIT消息，并把1传递给msg.wParam
	KillTimer(hWnd,TIMER_AUTOGRAPH);//不需要计时器时？？？？
	return 0;
}//收到WM_DESTROY后的处理过程体
void autodraw(HWND hWnd,double width,double length,double  mw, double ml)
{
	int i;
    static float  WC, LC;
    static double  poix,poiy;
    static RECT r1;
    static double poib,poic;
	static HDC PCD1;
	static HPEN hPen;
	int  sx,sy,sr;
	double angle1, angle2;
	double sm, R1;
	double smu, Ru1;
	m_point point,point_s;     //每行产生一个"绝对"坐标点
	m_point point1,point1_s;     //每行产生一个"绝对"坐标点
	m_point pointu,pointu_s;     //每行产生一个"绝对"坐标点
	m_point pointu1,pointu1_s;
	double pointX, pointY,pointI, pointJ;
	double pointX1, pointY1,pointI1, pointJ1;
	double temp;
	double prePoix,prePoiy,prePoib,prePoic;
	static double lineArc;
	double preLineArc;
	AutoEndPaint=1;
	if(AutographFirstEnterFlag == 0)
	{
		ReleaseDC(hWnd,PCD1);
		PCD1= GetDC(hWnd);
		GetClientRect(hWnd,&r1);
		SetMapMode(PCD1,MM_LOENGLISH);
		SetViewportOrgEx(PCD1,0,r1.bottom-r1.top,NULL);           //改变逻辑坐标原点到左下角y轴向上，x轴向右
		WC=(float)((r1.right-r1.left)/(width*1.2));
		LC=(float)((r1.bottom-r1.top)/(length*1.2));
		poix=poiy=poib=poic=0.0;
		lineArc=0.0;
	}
	
	prePoix=poix;
	prePoiy=poiy;
	prePoib=poib;
	prePoic=poic;
	preLineArc=lineArc;
	if(NowGraphPlane==19)
	{
		poix = data_w.y*1000;
		poiy = data_w.z*1000;
	}
	else if(NowGraphPlane==18)
	{
		poix = data_w.x*1000;
		poiy = data_w.z*1000;
	}
	else if(NowGraphPlane==17)
	{
		poix = data_w.x*1000;
		poiy = data_w.y*1000;
	}
	else if(NowGraphPlane==15)
	{
		poix = data_w.x*1000;
		poiy = data_w.y*1000;
		poib = data_w.b*1000;
		poic = data_w.c*1000;
		caculateTapPoint(poib,poic,poix,poiy);		
	}
	if(AutoGraphParam.draw_width>=AutoGraphParam.draw_length)
    {
		if(NowGraphPlane!=15)
		{        
			poix = poix*WC+(r1.right*0.5-mw);
			poiy = poiy*WC+(r1.bottom*0.5-ml);      //因为窗口坐标Y轴向下,此行代码是为了把显示图像移至窗口中央
		}
		else
		{
			poix = poix*WC+(r1.right*0.5-mw);
			poiy = poiy*WC+(r1.bottom*0.5-ml); 
			poib=poib*WC+(r1.right*0.5-mw);
			poic=poic*WC+(r1.bottom*0.5-ml);
		}
    }
    else
    {        
        poix = poix*LC+(r1.right*0.5-mw);
        poiy = poiy*LC+(r1.bottom*0.5-ml);
		if(NowGraphPlane==15)
		{
			poib=poib*LC+(r1.right*0.5-mw);
			poic=poic*LC+(r1.bottom*0.5-ml);
		}
    }
	lineArc=calculateLineArc(poix-prePoix,poiy-prePoiy);	
	if(NowGraphPlane==15)
	{
		hPen = CreatePen(PS_SOLID,4,RGB(0,0,0));
		SelectObject(PCD1,hPen);
		MoveToEx(PCD1,(int)prePoib,(int)prePoic,NULL);
		LineTo(PCD1,(int)prePoix,(int)prePoiy);
	}
	else
	{
		hPen = CreatePen(PS_SOLID,3,RGB(0,0,0));
		SelectObject(PCD1,hPen);
		PaintMark(prePoix,prePoiy,preLineArc,PCD1);
	}
	DeleteObject(hPen);
	
	
	if(NowGraphPlane==15)
	{
		hPen = CreatePen(PS_SOLID,3,RGB(0,255,255));
		SelectObject(PCD1,hPen);
		MoveToEx(PCD1,(int)poib,(int)poic,NULL);
		LineTo(PCD1,(int)poix,(int)poiy);
	}
	else
	{
		hPen = CreatePen(PS_SOLID,1,RGB(0,255,255));
		SelectObject(PCD1,hPen);
		PaintMark(poix,poiy,lineArc,PCD1);
	}
	DeleteObject(hPen);
	hPen = CreatePen(PS_SOLID,1,RGB(255,255,255));
	SelectObject(PCD1,hPen);
	point.X = 0;
	point.Y = 0;
    for(i=0;i<AutoGraphParam.create_num+1;i++)
    {
		point_s.X = point.X;
		point_s.Y = point.Y;
		if((AutoGraphParam.pDataNcGraphMem+i)->G_plane == 19)
		{
			point.X = point_s.X + (AutoGraphParam.pDataNcGraphMem+i)->Y*1000;
			point.Y = point_s.Y + (AutoGraphParam.pDataNcGraphMem+i)->Z*1000;
		}
		if((AutoGraphParam.pDataNcGraphMem+i)->G_plane == 18)
		{
			point.X = point_s.X + (AutoGraphParam.pDataNcGraphMem+i)->X*1000;
			point.Y = point_s.Y + (AutoGraphParam.pDataNcGraphMem+i)->Z*1000;
		}
		if((AutoGraphParam.pDataNcGraphMem+i)->G_plane == 17)
		{
			point.X = point_s.X + (AutoGraphParam.pDataNcGraphMem+i)->X*1000;
			point.Y = point_s.Y + (AutoGraphParam.pDataNcGraphMem+i)->Y*1000;
		}			
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
        if ((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode==1)
        {
            MoveToEx(PCD1,(int) point1_s.X, (int) point1_s.Y,NULL);
			LineTo(PCD1,(int) point1.X,(int)point1.Y);
		 }
        else if((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode==2 ||(AutoGraphParam.pDataNcGraphMem+i)->G_pathmode==3)
        {
			if((AutoGraphParam.pDataNcGraphMem+i)->G_plane == 19)
			{
				pointI = (AutoGraphParam.pDataNcGraphMem+i)->J;
				pointJ = (AutoGraphParam.pDataNcGraphMem+i)->K;
				pointX = (AutoGraphParam.pDataNcGraphMem+i)->Y;
				pointY = (AutoGraphParam.pDataNcGraphMem+i)->Z;
			}
			if((AutoGraphParam.pDataNcGraphMem+i)->G_plane == 18)
			{
				pointI = (AutoGraphParam.pDataNcGraphMem+i)->I;
				pointJ = (AutoGraphParam.pDataNcGraphMem+i)->K;
				pointX = (AutoGraphParam.pDataNcGraphMem+i)->X;
				pointY = (AutoGraphParam.pDataNcGraphMem+i)->Z;
			}
			if((AutoGraphParam.pDataNcGraphMem+i)->G_plane == 17)
			{
				pointI = (AutoGraphParam.pDataNcGraphMem+i)->I;
				pointJ = (AutoGraphParam.pDataNcGraphMem+i)->J;
				pointX = (AutoGraphParam.pDataNcGraphMem+i)->X;
				pointY = (AutoGraphParam.pDataNcGraphMem+i)->Y;
			}
            R1 = sqrt(pointI*pointI + pointJ*pointJ);
            sm = pointX*pointX + pointY*pointY;
			if(R1!=0)
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
			else if(R1==0)
			{
              angle1=0;
			  angle2=0;
			}     
            if(width>=length)
            {
				sx = (int)(point1_s.X+pointI*1000*WC);
				sy = (int)(point1_s.Y+pointJ*1000*WC);
				sr = (int)(R1*1000*WC);
            }else
            {
				sx = (int)(point1_s.X+pointI*1000*LC);
                sy = (int)(point1_s.Y+pointJ*1000*LC);
                sr = (int)(R1*1000*LC);
            }
			MoveToEx(PCD1,(int)point1_s.X, (int)point1_s.Y,NULL);
			if(angle1-angle2>=0)
			{
				if((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode==2)
				{
					AngleArc(PCD1,sx,sy,sr,360-angle1,(angle1-angle2) );
				}
				if((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode==3)
				{
					AngleArc(PCD1,sx,sy,sr,360-angle1,-(360-(angle1-angle2)));
				}
			}
			else
			{
				if((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode==2) 
				{
					AngleArc(PCD1,sx,sy,sr,360-angle1,(360-(angle2-angle1)));
				}
				if((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode==3)
				{
					AngleArc(PCD1,sx,sy,sr,360-angle1,(angle1-angle2) );
				}
			}
		}
	}
	if(NowGraphPlane==15)
	{
		DeleteObject(hPen);
		hPen = CreatePen(PS_SOLID,1,RGB(255,0,0));
		SelectObject(PCD1,hPen);
		pointu.X=0;
		pointu.Y=0;
		for(i=0;i<AutoGraphParam.create_num+1;i++)
		{
			pointu_s.X=pointu.X;
			pointu_s.Y=pointu.Y;
			pointu.X = pointu_s.X + (AutoGraphParam.pDataNcGraphMem+i)->B*1000;
			pointu.Y = pointu_s.Y + (AutoGraphParam.pDataNcGraphMem+i)->C*1000;
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
			if ((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==1)
			{
				MoveToEx(PCD1,(int) pointu1_s.X, (int) pointu1_s.Y,NULL);
				LineTo(PCD1,(int) pointu1.X,(int)pointu1.Y);			
			 }
			else if((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==2 ||(AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==3)
			{
				pointI1 = (AutoGraphParam.pDataNcGraphMem+i)->I1;
				pointJ1 = (AutoGraphParam.pDataNcGraphMem+i)->J1;
				pointX1 = (AutoGraphParam.pDataNcGraphMem+i)->B;
				pointY1 = (AutoGraphParam.pDataNcGraphMem+i)->C;
				Ru1=sqrt(pointI1*pointI1 + pointJ1*pointJ1);
				smu=pointX1*pointX1 + pointY1*pointY1;
				if(Ru1!=0)
				{  
					temp = -(pointI1/Ru1);
					if(temp >= 1) temp = 1;
					if(temp <= -1) temp =-1;
					if(pointJ1<=0)
						angle1 = 180*acos(temp)/3.14;
					else 
						angle1 = 360-180*acos(temp)/3.14;
					temp = (pointX1-pointI1)/Ru1;
					if(temp >= 1) temp = 1;
					if(temp <= -1) temp =-1;
					if(pointY1>pointJ1)
						angle2 = 180*acos(temp)/3.14;
					else
						angle2 = 360- 180*acos(temp)/3.14;			        
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
						if((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==2)
						{
							AngleArc(PCD1,sx,sy,sr,360-angle1,(angle1-angle2) );
						}
						if((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==3)
						{
							AngleArc(PCD1,sx,sy,sr,360-angle1,-(360-(angle1-angle2)));								
						}
					}
					else
					{
						if((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==2) 
						{
							AngleArc(PCD1,sx,sy,sr,360-angle1,(360-(angle2-angle1)));
						}
						if((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==3)
						{
							AngleArc(PCD1,sx,sy,sr,360-angle1,(angle1-angle2) );
						}
					}
				}												
				if(Ru1==0)
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
					if((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==2)
					{
						AngleArc(PCD1,sx,sy,sr,360-angle1,(angle1-angle2) );							
					}
					if((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==3)
					{
						AngleArc(PCD1,sx,sy,sr,360-angle1,-(360-(angle1-angle2)));								
					}
				}
			}
		}
	}
	DeleteObject(hPen);
	if(AutographFirstEnterFlag==0)	AutographFirstEnterFlag = 1;
	AutoEndPaint=0;
}
void caculateTapPoint(double &UpperX,double &UpperY,double &LowerX,double &LowerY)
{
	double cutlerUpperX,cutlerUpperY,cutlerLowerX,cutlerLowerY;
	Tape_data TapePara;
	TapePara.Tape_k1 = (HLOWDISTANCE + h_thickness)/ h_thickness;
	TapePara.Tape_k2 = HLOWDISTANCE/ h_thickness;
	TapePara.Tape_k3 = (HLOWDISTANCE - z_position)/ h_thickness;
	TapePara.Tape_k4 =	z_position/ h_thickness ;
	cutlerUpperX=UpperX;
	cutlerUpperY=UpperY;
	cutlerLowerX=LowerX;
	cutlerLowerY=LowerY;
	UpperX=(cutlerUpperX*TapePara.Tape_k1-TapePara.Tape_k3*cutlerLowerX)/(TapePara.Tape_k1*TapePara.Tape_k4+TapePara.Tape_k2*TapePara.Tape_k3);
	UpperY=(cutlerUpperY*TapePara.Tape_k1-TapePara.Tape_k3*cutlerLowerY)/(TapePara.Tape_k1*TapePara.Tape_k4+TapePara.Tape_k2*TapePara.Tape_k3);
	LowerX=(cutlerUpperX*TapePara.Tape_k2+TapePara.Tape_k4*cutlerLowerX)/(TapePara.Tape_k1*TapePara.Tape_k4+TapePara.Tape_k2*TapePara.Tape_k3);
	LowerY=(cutlerUpperY*TapePara.Tape_k2+TapePara.Tape_k4*cutlerLowerY)/(TapePara.Tape_k1*TapePara.Tape_k4+TapePara.Tape_k2*TapePara.Tape_k3);
}
void PaintMark(double x,double y,double Arc,HDC PCD)
{
	double Point1X,Point1Y,Point2X,Point2Y,Point3X,Point3Y,Point4X,Point4Y;
	double Length;
	Length=10;
	Point1X=x+Length*cos(Arc+3.14/3);
	Point1Y=y+Length*sin(Arc+3.14/3);
	Point2X=x+Length*cos(Arc+2*3.14/3);
	Point2Y=y+Length*sin(Arc+2*3.14/3);
	Point3X=x+Length*cos(Arc+4*3.14/3);
	Point3Y=y+Length*sin(Arc+4*3.14/3);
	Point4X=x+Length*cos(Arc+5*3.14/3);
	Point4Y=y+Length*sin(Arc+5*3.14/3);
	MoveToEx(PCD,(int) x, (int) y,NULL);
	LineTo(PCD,(int)Point1X,(int)Point1Y);
	LineTo(PCD,(int)Point2X,(int)Point2Y);
	LineTo(PCD,(int)x,(int)y);
	LineTo(PCD,(int)Point3X,(int)Point3Y);
	LineTo(PCD,(int)Point4X,(int)Point4Y);
	LineTo(PCD,(int)x,(int)y);
}