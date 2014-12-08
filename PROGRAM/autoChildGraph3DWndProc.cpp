#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;								// 当前实例
//***************************
extern double h_thickness;
extern double now_up_X;
extern double now_up_Y;
extern double now_up_Z;
extern double now_down_X;
extern double now_down_Y;
extern double now_down_Z;
extern double pre_up_X;
extern double pre_up_Y;
extern double pre_up_Z;
extern double pre_down_X;
extern double pre_down_Y;
extern double pre_down_Z;

extern int auto_up_down;
extern int auto_right_left;
extern double auto_zoom;
extern int auto_turn_up_down;
extern int auto_turn_right_left;
extern int auto_move_step;
extern int auto_turn_step;
extern double cutlerRadius;

//2010-5-31 锥加工参数修改
extern double z_position;
extern double xy_plane;
extern double h_thickness;
extern double HLOWDISTANCE;
//end 2010-5-31
//***************************
extern int auto_regraph_flag;
extern GraphParam AutoGraphParam;
//***************************
extern Coordata data_w;   //work coor
//***************************
extern void CircleToLine(double LineStartX,double LineStartY,double LineEndX,double LineEndY,double CircleCenterX,
				  double CircleCenterY,double CircleStartX,double CircleStartY,double CircleEndX,double CircleEndY,
				  int circularMode,int UpMode);//UpMode==1第一个变量在上平面
extern void CircleToCircle(double Circle1CenterX,double Circle1CenterY,double Circle1StartX,double Circle1StartY,
					double Circle1EndX,double Circle1EndY,double Circle2CenterX,double Circle2CenterY,
					double Circle2StartX,double Circle2StartY,double Circle2EndX,double Circle2EndY,
					int circular1Mode,int circular2Mode,int UpMode);
extern void LineToLine(double Line1StartX,double Line1StartY,double Line1EndX,double Line1EndY,
				double Line2StartX,double Line2StartY,double Line2EndX,double Line2EndY);
extern void drawColumn(double UpperX,double UpperY,double UpperZ,double DownX,double DownY,double DownZ);
extern void Draw();//根据所提供的点坐标绘制四边形
extern void CalNormal(double*,double*,double*,double*);//计算三个点做组成平面的法向量
extern void DrawReset();//重画对应的执行过程
//***************************
//设置窗口大小，斜率及全屏标志变量
HGLRC hAutoRCGraph=NULL;            //定义渲染环境
HDC   hAutoDCGraph=NULL;            //私有的GDI设备环境
HWND hAutoWndGraph=NULL;           //得到窗口的句柄
void AutoSceneShow(GLvoid);//显示绘图
void AutoSceneInit(int,int);//初始化OPENGL窗口
void DrawAutoGraph();//绘图过程
void DrawMachineMove();
void DrawMachine(double X,double Y,double Z,double UPMode);//UpMode 为1表示为上刀架，为2表示为下刀架
void AutoEnableGraph();//使能OPENGL
void AutoDisableGraph();//取消OPENGL
void Auto3DDraw();
const MessageProc AutoGraphMessages[]={
	    WM_CREATE, goCreateAutoGraph,
		WM_DESTROY, goDestroyAutoGraph
};
int MyRegisterClassAutoGraph(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;//定义窗口类结构体对象
    //填充结构体中各成员
	wcex.cbSize = sizeof(WNDCLASSEX); //?????
	wcex.style			=CS_OWNDC;//客户区移动或其宽度/高度改变时重画客户区
	wcex.lpfnWndProc	= (WNDPROC)AutoGraphWndProc;//指向窗口过程的长指针
	wcex.cbClsExtra		= 0;//指定窗口类结构体存储区之后额外开辟的字节，系统初始化这些字节地址为0
	wcex.cbWndExtra		= 0;//指定窗口实例额外开辟的字节数
	wcex.hInstance		= hInstance;//指向当前窗口过程所在的程序实例句柄
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDC_EDM_NC_PROGRAM);//下载图标
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);//鼠标
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);//白色背景画刷
	wcex.lpszMenuName	= NULL;//(LPCTSTR)ID_Menu;//菜单，windowsCE不支持
	wcex.lpszClassName	= TEXT("AUTOGRAPH3DWND");//指向一个字符串常量，指定自定义窗口类的名称
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);//？？？？
//注册主窗口类
	if(!RegisterClassEx(&wcex)) return 0;
	return 1;
}
LRESULT CALLBACK AutoGraphWndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;
	//查找对应的消息ID,并执行对应的消息处理过程
	for(i=0; i<dim(AutoGraphMessages);i++)
	{
		if(msgCode == AutoGraphMessages[i].uCode) return(*AutoGraphMessages[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//对不存在的消息，调用缺省窗口过程
	return DefWindowProc(hWnd, msgCode, wParam, lParam);
}
LRESULT goCreateAutoGraph(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{	
	hAutoWndGraph=hWnd;
	AutoEnableGraph();//使能OPGNGL
	AutoSceneInit(395, 309);//初始化全局光线窗口等
	wglMakeCurrent(NULL,NULL);//释放当前DC
	now_up_X=0.0;
	now_up_Y=0.0;
	now_up_Z=h_thickness; 
	now_down_X=0.0;
	now_down_Y=0.0;
	now_down_Z=0.0;
	pre_up_X=0.0;
	pre_up_Y=0.0;
	pre_up_Z=h_thickness;
	pre_down_X=0.0;
	pre_down_Y=0.0;
	pre_down_Z=0.0;
	cutlerRadius=0.2;
	return 0;
}
LRESULT goDestroyAutoGraph(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//关闭，退出程序。清除AutoGraph窗口，以便释放所有资源，然后退出程序。
	AutoDisableGraph();
	ChangeDisplaySettings(NULL, 0);
	PostQuitMessage(0);
	return 0;
}
//AutoGraph初始化开始
void AutoSceneInit(int w,int h)
{
	//定义光照信息
	float position[]={1.0,3.0,0.0,0.0};
	float ambient[]={0.2,0.2,0.2,1.0};
	float diffuse[]={1.0,1.0,1.0,1.0};
	float specular[]={1.0,1.0,1.0,1.0};
	glViewport(0,0,395,309);
	//设定投影模式
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-300,300,-200,200,0,600);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();			//用单位矩阵置换当前的矩阵

	glShadeModel(GL_SMOOTH);       //允许平滑着色
	//
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      // 黑色背景 
	glClearDepth(600.0f);         //设置深度缓冲区
	glEnable(GL_DEPTH_TEST);                            //允许深度测试
	glDepthFunc(GL_LEQUAL);        //深度测试的类型

	glHint(GL_PERSPECTIVE_CORRECTION_HINT|GL_LINE_SMOOTH_HINT|GL_POINT_SMOOTH_HINT|GL_POLYGON_SMOOTH_HINT, GL_NICEST); //透视校正，会使透视效果显得更好一些
//设定光照
	glLightfv(GL_LIGHT0,GL_POSITION,position);
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);	
}

void AutoSceneShow(GLvoid)         //这里进行所有的绘图工作
{
	//设定物体的纹理和
	float specular[]={1.0,1.0,1.0,1.0};
	float low_shininess[]={10.0};
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,specular);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,low_shininess);
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	glPushMatrix();
	//设定观察角度
		glTranslatef(0.0,0.0,-300.0);
		glTranslatef(0.0,auto_up_down,0.0);
		glTranslatef(auto_right_left,0.0,0.0);
		glScalef(auto_zoom,auto_zoom,auto_zoom);
		glRotatef(auto_turn_up_down,1.0,0.0,0.0);
		glRotatef(auto_turn_right_left,0.0,1.0,0.0);
		glPushMatrix();
			glColor4f(0.4,0.6,0.1,0.5);//定义图形颜色
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			DrawAutoGraph();//绘图
			DrawMachineMove();
		glPopMatrix();
	glPopMatrix();
}  
//激活创建AutoGraph窗口
void EnableAutoGraph()
{
	PIXELFORMATDESCRIPTOR pfd;
	int iFormat;
	hAutoDCGraph = GetDC( hAutoWndGraph );
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );    //素格式描述子结构的大小
	pfd.nVersion = 1;      //PIXELFORMATDESCRIPTOR结构的版本，一般设为1
	pfd.dwFlags =  PFD_DRAW_TO_WINDOW| //一组表明象素缓冲特性的标志位，如缓冲是否支持GDI或AutoGraph等
	   PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;   //明象素数据类型是RGBA还是颜色索引;
	pfd.cColorBits = 16;     //每个颜色缓冲区中颜色位平面的数目，对颜色索引方式是缓冲区大小
	pfd.cDepthBits = 32;     //缓冲区的深度; 
	pfd.iLayerType = PFD_MAIN_PLANE; //被忽略，为了一致性而包含的
	iFormat = ChoosePixelFormat( hAutoDCGraph, &pfd );//选择一个像素格式
	SetPixelFormat( hAutoDCGraph, iFormat, &pfd ); //设置到DC中
	hAutoRCGraph = wglCreateContext( hAutoDCGraph );    //创建绘图描述表
	wglMakeCurrent( hAutoDCGraph, hAutoRCGraph );     //使之成为当前绘图描述表
}
void DrawAutoGraph()
{
	int i;
	for(i=0;i<AutoGraphParam.create_num+1;i++)
	{
		//锥加工、上下异性面绘图
		if(((AutoGraphParam.pDataNcGraphMem+i)->G_Dcompensate==152)||((AutoGraphParam.pDataNcGraphMem+i)->G_Dcompensate==153)||((AutoGraphParam.pDataNcGraphMem+i)->G_Dcompensate==150)||((AutoGraphParam.pDataNcGraphMem+i)->G_tapemode==50)||((AutoGraphParam.pDataNcGraphMem+i)->G_tapemode==52)||((AutoGraphParam.pDataNcGraphMem+i)->G_tapemode==51))
		{
			//上下平面均为直线直接绘图
			if((((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==1)&&((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_lower==1))&&(
				((AutoGraphParam.pDataNcGraphMem+i)->G_tapemode==50)||((AutoGraphParam.pDataNcGraphMem+i)->G_tapemode==52)||((AutoGraphParam.pDataNcGraphMem+i)->G_tapemode==51)))
			{
				now_up_X=now_up_X+(AutoGraphParam.pDataNcGraphMem+i)->B;
				now_down_X=now_down_X+(AutoGraphParam.pDataNcGraphMem+i)->X;
				now_up_Y=now_up_Y+(AutoGraphParam.pDataNcGraphMem+i)->C;
				now_down_Y=now_down_Y+(AutoGraphParam.pDataNcGraphMem+i)->Y;
				Draw();
			}
			else if((((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==1)&&((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_lower==1))&&(
				((AutoGraphParam.pDataNcGraphMem+i)->G_Dcompensate==150)||((AutoGraphParam.pDataNcGraphMem+i)->G_Dcompensate==152)||((AutoGraphParam.pDataNcGraphMem+i)->G_Dcompensate==151)))
				LineToLine(0.0,0.0,(AutoGraphParam.pDataNcGraphMem+i)->B,(AutoGraphParam.pDataNcGraphMem+i)->C,
				0.0,0.0,(AutoGraphParam.pDataNcGraphMem+i)->X,(AutoGraphParam.pDataNcGraphMem+i)->Y);
			//上平面为圆，下平面为直线
			else if((((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==2)||((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==3))
				&&((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_lower==1))
			{
				CircleToLine(0.0,0.0,(AutoGraphParam.pDataNcGraphMem+i)->X,(AutoGraphParam.pDataNcGraphMem+i)->Y,
					(AutoGraphParam.pDataNcGraphMem+i)->I1,(AutoGraphParam.pDataNcGraphMem+i)->J1,
					0.0,0.0,(AutoGraphParam.pDataNcGraphMem+i)->B,(AutoGraphParam.pDataNcGraphMem+i)->C,
					(AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_upper,2);
			}
			//上平面为直线，下平面为圆弧
			else if((((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_lower==2)||((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_lower==3))
				&&((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==1))
			{
				CircleToLine(0.0,0.0,(AutoGraphParam.pDataNcGraphMem+i)->B,(AutoGraphParam.pDataNcGraphMem+i)->C,
					(AutoGraphParam.pDataNcGraphMem+i)->I,(AutoGraphParam.pDataNcGraphMem+i)->J,
					0.0,0.0,(AutoGraphParam.pDataNcGraphMem+i)->X,(AutoGraphParam.pDataNcGraphMem+i)->Y,
					(AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_lower,1);
			}
			//上下平面均为圆弧
			else if((((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_lower==2)||((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_lower==3))
				&&((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==2)||((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==3))
			{
				CircleToCircle((AutoGraphParam.pDataNcGraphMem+i)->I1,(AutoGraphParam.pDataNcGraphMem+i)->J1,0.0,0.0,
					(AutoGraphParam.pDataNcGraphMem+i)->B,(AutoGraphParam.pDataNcGraphMem+i)->C,
					(AutoGraphParam.pDataNcGraphMem+i)->I,(AutoGraphParam.pDataNcGraphMem+i)->J,0.0,0.0,
					(AutoGraphParam.pDataNcGraphMem+i)->X,(AutoGraphParam.pDataNcGraphMem+i)->Y,
					(AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_upper,(AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_lower,1);
			}
		}
		//常规加工
		else
		{
			if((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode==1)
			{
				now_up_X=now_up_X+(AutoGraphParam.pDataNcGraphMem+i)->X;
				now_down_X=now_down_X+(AutoGraphParam.pDataNcGraphMem+i)->X;
				now_up_Y=now_up_Y+(AutoGraphParam.pDataNcGraphMem+i)->Y;
				now_down_Y=now_down_Y+(AutoGraphParam.pDataNcGraphMem+i)->Y;
				Draw();
			}
			else if(((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode==2)||((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode==3))
			{
				CircleToCircle((AutoGraphParam.pDataNcGraphMem+i)->I,(AutoGraphParam.pDataNcGraphMem+i)->J,0.0,0.0,
					(AutoGraphParam.pDataNcGraphMem+i)->X,(AutoGraphParam.pDataNcGraphMem+i)->Y,
					(AutoGraphParam.pDataNcGraphMem+i)->I,(AutoGraphParam.pDataNcGraphMem+i)->J,0.0,0.0,
					(AutoGraphParam.pDataNcGraphMem+i)->X,(AutoGraphParam.pDataNcGraphMem+i)->Y,
					(AutoGraphParam.pDataNcGraphMem+i)->G_pathmode,(AutoGraphParam.pDataNcGraphMem+i)->G_pathmode,1);
			}
		}
	}
}
void DrawMachine(double X,double Y,double Z,double UpMode)
{
	double PointDown1[3],PointDown2[3],PointDown3[3],PointDown4[3];
	double PointUp1[3],PointUp2[3],PointUp3[3],PointUp4[3];
	double n[3];
	PointDown1[0]=PointUp1[0]=X+5;
	PointDown1[1]=PointUp1[1]=Y+5;
	PointDown2[0]=PointUp2[0]=X-5;
	PointDown2[1]=PointUp2[1]=Y+5;
	PointDown3[0]=PointUp3[0]=X-5;
	PointDown3[1]=PointUp3[1]=Y-5;
	PointDown4[0]=PointUp4[0]=X+5;
	PointDown4[1]=PointUp4[1]=Y-5;
	if(UpMode==1)
	{
		PointDown1[2]=PointDown2[2]=PointDown3[2]=PointDown4[2]=Z;
		PointUp1[2]=PointUp2[2]=PointUp3[2]=PointUp4[2]=Z+5;
	}
	else
	{
		PointDown1[2]=PointDown2[2]=PointDown3[2]=PointDown4[2]=Z-5;
		PointUp1[2]=PointUp2[2]=PointUp3[2]=PointUp4[2]=Z;
	}
	/*******************************************************/
	CalNormal(PointDown1,PointDown2,PointDown3,n);//计算平面法向量
	glColor4f(1.0,1.0,1.0,0.5);//定义图形颜色
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glBegin(GL_TRIANGLES);
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f(PointDown1[0],PointDown1[1],PointDown1[2]);
		glVertex3f(PointDown2[0],PointDown2[1],PointDown2[2]);
		glVertex3f(PointDown3[0],PointDown3[1],PointDown3[2]);
	glEnd();
	glBegin(GL_TRIANGLES);
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f(PointDown1[0],PointDown1[1],PointDown1[2]);
		glVertex3f(PointDown4[0],PointDown4[1],PointDown4[2]);
		glVertex3f(PointDown3[0],PointDown3[1],PointDown3[2]);
	glEnd();
	/**************************************/
	glBegin(GL_TRIANGLES);
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f(PointUp1[0],PointUp1[1],PointUp1[2]);
		glVertex3f(PointUp2[0],PointUp2[1],PointUp2[2]);
		glVertex3f(PointUp3[0],PointUp3[1],PointUp3[2]);
	glEnd();
	glBegin(GL_TRIANGLES);
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f(PointUp1[0],PointUp1[1],PointUp1[2]);
		glVertex3f(PointUp4[0],PointUp4[1],PointUp4[2]);
		glVertex3f(PointUp3[0],PointUp3[1],PointUp3[2]);
	glEnd();
	/*******************************************************/
	CalNormal(PointDown1,PointDown2,PointUp1,n);//计算平面法向量
	glBegin(GL_TRIANGLES);
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f(PointDown1[0],PointDown1[1],PointDown1[2]);
		glVertex3f(PointDown2[0],PointDown2[1],PointDown2[2]);
		glVertex3f(PointUp1[0],PointUp1[1],PointUp1[2]);
	glEnd();
	glBegin(GL_TRIANGLES);
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f(PointUp1[0],PointUp1[1],PointUp1[2]);
		glVertex3f(PointUp2[0],PointUp2[1],PointUp2[2]);
		glVertex3f(PointDown2[0],PointDown2[1],PointDown2[2]);
	glEnd();
	/**************************************/
	glBegin(GL_TRIANGLES);
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f(PointDown3[0],PointDown3[1],PointDown3[2]);
		glVertex3f(PointDown4[0],PointDown4[1],PointDown4[2]);
		glVertex3f(PointUp3[0],PointUp3[1],PointUp3[2]);
	glEnd();
	glBegin(GL_TRIANGLES);
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f(PointDown4[0],PointDown4[1],PointDown4[2]);
		glVertex3f(PointUp4[0],PointUp4[1],PointUp4[2]);
		glVertex3f(PointUp3[0],PointUp3[1],PointUp3[2]);
	glEnd();
	/*******************************************************/
	CalNormal(PointDown1,PointDown2,PointUp1,n);//计算平面法向量
	glBegin(GL_TRIANGLES);
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f(PointDown3[0],PointDown3[1],PointDown3[2]);
		glVertex3f(PointDown2[0],PointDown2[1],PointDown2[2]);
		glVertex3f(PointUp3[0],PointUp3[1],PointUp3[2]);
	glEnd();
	glBegin(GL_TRIANGLES);
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f(PointUp3[0],PointUp3[1],PointUp3[2]);
		glVertex3f(PointUp2[0],PointUp2[1],PointUp2[2]);
		glVertex3f(PointDown2[0],PointDown2[1],PointDown2[2]);
	glEnd();
	/**************************************/
	glBegin(GL_TRIANGLES);
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f(PointDown1[0],PointDown1[1],PointDown1[2]);
		glVertex3f(PointDown4[0],PointDown4[1],PointDown4[2]);
		glVertex3f(PointUp1[0],PointUp1[1],PointUp1[2]);
	glEnd();
	glBegin(GL_TRIANGLES);
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f(PointDown4[0],PointDown4[1],PointDown4[2]);
		glVertex3f(PointUp4[0],PointUp4[1],PointUp4[2]);
		glVertex3f(PointUp1[0],PointUp1[1],PointUp1[2]);
	glEnd();
}
void DrawMachineMove()
{
	drawColumn(data_w.x,data_w.y,-HLOWDISTANCE,data_w.b,data_w.c,z_position);
	DrawMachine(data_w.x,data_w.y,-HLOWDISTANCE,0);
	DrawMachine(data_w.b,data_w.c,z_position,1);
}
//激活创建Graph窗口
void AutoEnableGraph()
{
	PIXELFORMATDESCRIPTOR pfd;
	int iFormat;
	hAutoDCGraph = GetDC(hAutoWndGraph);
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );    //素格式描述子结构的大小
	pfd.nVersion = 1;      //PIXELFORMATDESCRIPTOR结构的版本，一般设为1
	pfd.dwFlags =  PFD_DRAW_TO_WINDOW| //一组表明象素缓冲特性的标志位，如缓冲是否支持GDI或Graph等
	   PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;   //明象素数据类型是RGBA还是颜色索引;
	pfd.cColorBits = 16;     //每个颜色缓冲区中颜色位平面的数目，对颜色索引方式是缓冲区大小
	pfd.cDepthBits = 32;     //缓冲区的深度; 
	pfd.iLayerType = PFD_MAIN_PLANE; //被忽略，为了一致性而包含的
	iFormat = ChoosePixelFormat( hAutoDCGraph, &pfd );//选择一个像素格式
	SetPixelFormat( hAutoDCGraph, iFormat, &pfd ); //设置到DC中
	hAutoRCGraph = wglCreateContext( hAutoDCGraph );    //创建绘图描述表
	wglMakeCurrent( hAutoDCGraph, hAutoRCGraph);     //使之成为当前绘图描述表
}
void AutoDisableGraph()
{
	wglDeleteContext( hAutoRCGraph );
	ReleaseDC( hAutoWndGraph,hAutoDCGraph);
}

void Auto3DDraw()
{
	now_up_X=0.0;
	now_up_Y=0.0;
	now_up_Z=h_thickness;
	now_down_X=0.0;
	now_down_Y=0.0;
	now_down_Z=0.0;
	pre_up_X=0.0;
	pre_up_Y=0.0;
	pre_up_Z=h_thickness;
	pre_down_X=0.0;
	pre_down_Y=0.0;
	pre_down_Z=0.0;
	wglMakeCurrent(hAutoDCGraph,hAutoRCGraph);//得到当前DC
	AutoSceneShow();//绘图
	SwapBuffers(hAutoDCGraph);//双缓冲，将后台缓存移动到前台
	wglMakeCurrent(NULL,NULL);
}