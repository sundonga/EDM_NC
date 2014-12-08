#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;								// 当前实例
extern GraphParam EditGraphParam;
extern int edit_regraph_flag;
extern double h_thickness;

extern int calculateNumber;
extern double minLength;
extern double maxLength;

float edit_turn_up_down;
float edit_turn_right_left;
float edit_up_down;
float edit_right_left;

float edit_zoom;

float edit_move_step;
float edit_turn_step;

double now_up_X;
double now_up_Y;
double now_up_Z;
double now_down_X;
double now_down_Y;
double now_down_Z;
double pre_up_X;
double pre_up_Y;
double pre_up_Z;
double pre_down_X;
double pre_down_Y;
double pre_down_Z;
//***************************
extern double ZERO;
extern void CircleToLine(double LineStartX,double LineStartY,double LineEndX,double LineEndY,double CircleCenterX,
				  double CircleCenterY,double CircleStartX,double CircleStartY,double CircleEndX,double CircleEndY,
				  int circularMode,int UpMode);//UpMode==1第一个变量在上平面
extern void CircleToCircle(double Circle1CenterX,double Circle1CenterY,double Circle1StartX,double Circle1StartY,
					double Circle1EndX,double Circle1EndY,double Circle2CenterX,double Circle2CenterY,
					double Circle2StartX,double Circle2StartY,double Circle2EndX,double Circle2EndY,
					int circular1Mode,int circular2Mode,int UpMode);

//设置窗口大小，斜率及全屏标志变量
HGLRC hRCGraph=NULL;            //定义渲染环境
HDC   hDCGraph=NULL;            //私有的GDI设备环境
HWND hWndGraph=NULL;           //得到窗口的句柄
void SceneInit(int,int);//初始化OPENGL窗口
void SceneShow(GLvoid);//显示绘图
void EnableGraph();//使能OPENGL
void DisableGraph();//取消OPENGL
bool ChangeResolution(int,int,int);//改变屏幕分辨率
//Draw
void DrawGraph();//绘图过程
void Draw();//根据所提供的点坐标绘制四边形
void CalNormal(double*,double*,double*,double*);//计算三个点做组成平面的法向量
void DrawReset();//重画对应的执行过程
void Draw3D();
void Draw3DReset();
//
const MessageProc graphMessages[]={
	    WM_CREATE, goCreateGraph,
		WM_DESTROY, goDestroyGraph
};
int MyRegisterClassGraph(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;//定义窗口类结构体对象
    //填充结构体中各成员
	wcex.cbSize = sizeof(WNDCLASSEX); //?????
	wcex.style			=CS_OWNDC;//客户区移动或其宽度/高度改变时重画客户区
	wcex.lpfnWndProc	= (WNDPROC)graphWndProc;//指向窗口过程的长指针
	wcex.cbClsExtra		= 0;//指定窗口类结构体存储区之后额外开辟的字节，系统初始化这些字节地址为0
	wcex.cbWndExtra		= 0;//指定窗口实例额外开辟的字节数
	wcex.hInstance		= hInstance;//指向当前窗口过程所在的程序实例句柄
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDC_EDM_NC_PROGRAM);//下载图标
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);//鼠标
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);//白色背景画刷
	wcex.lpszMenuName	= NULL;//(LPCTSTR)ID_Menu;//菜单，windowsCE不支持
	wcex.lpszClassName	= TEXT("GRAPH");//指向一个字符串常量，指定自定义窗口类的名称
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);//？？？？
//注册主窗口类
	if(!RegisterClassEx(&wcex)) return 0;
	return 1;
}
LRESULT CALLBACK graphWndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;
	//查找对应的消息ID,并执行对应的消息处理过程
	for(i=0; i<dim(graphMessages);i++)
	{
		if(msgCode == graphMessages[i].uCode) return(*graphMessages[i].functionName)(hWnd,msgCode,wParam,lParam);
	}	
	//对不存在的消息，调用缺省窗口过程
	return DefWindowProc(hWnd, msgCode, wParam, lParam);
}
LRESULT goCreateGraph(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{	
	hWndGraph=hWnd;
	EnableGraph();//使能OPGNGL
	SceneInit(500, 500);//初始化全局光线窗口等
	wglMakeCurrent(NULL,NULL);//释放当前DC
	//初始化各变量
	calculateNumber=1000;
	minLength=0.001;
	maxLength=1.0;
	edit_turn_up_down=0.0;
	edit_turn_right_left=0.0;
	edit_up_down=0.0;
	edit_right_left=0.0;
	edit_zoom=1.0;
	edit_move_step=10.0;//每次按平移按钮对应平移10.0mm
	edit_turn_step=10.0;//每次按选择按钮对应选择10度
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
	return 0;
}
LRESULT goDestroyGraph(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	//关闭，退出程序。清除Graph窗口，以便释放所有资源，然后退出程序。
	DisableGraph();
	ChangeDisplaySettings(NULL, 0);
	PostQuitMessage(0);
	return 0;
}
//Graph初始化开始
void SceneInit(int w,int h)
{
	//定义光照信息
	float position[]={1.0,3.0,0.0,0.0};
	float ambient[]={0.2,0.2,0.2,1.0};
	float diffuse[]={1.0,1.0,1.0,1.0};
	float specular[]={1.0,1.0,1.0,1.0};
	//窗口大小500
	glViewport(0,0,500,500);
	//设定投影模式
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-100,100,-100,100,0,200);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();			//用单位矩阵置换当前的矩阵

	glShadeModel(GL_SMOOTH);       //允许平滑着色
	//
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      // 黑色背景 
	glClearDepth(400.0f);         //设置深度缓冲区
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

void SceneShow(GLvoid)         //这里进行所有的绘图工作
{
	//设定物体的纹理和
	float specular[]={0.4,0.6,0.1,1.0};
	float low_shininess[]={10.0};
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,specular);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,low_shininess);
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	glPushMatrix();
	//设定观察角度
		glTranslatef(0.0,0.0,-100.0);
		glTranslatef(0.0,edit_up_down,0.0);
		glTranslatef(edit_right_left,0.0,0.0);
		glScalef(edit_zoom,edit_zoom,edit_zoom);
		glRotatef(edit_turn_up_down,1.0,0.0,0.0);
		glRotatef(edit_turn_right_left,0.0,1.0,0.0);
		glPushMatrix();
			glColor4f(0.4,0.6,0.1,0.5);//定义图形颜色
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			DrawGraph();//绘图
		glPopMatrix();
	glPopMatrix();
}  
void DrawReset()
{
	float specular[]={0.4,0.6,0.1,1.0};
	float low_shininess[]={10.0};
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,specular);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,low_shininess);
	glPushMatrix();
	glPopMatrix();
}
//激活创建Graph窗口
void EnableGraph()
{
	PIXELFORMATDESCRIPTOR pfd;
	int iFormat;
	hDCGraph = GetDC( hWndGraph );
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );    //素格式描述子结构的大小
	pfd.nVersion = 1;      //PIXELFORMATDESCRIPTOR结构的版本，一般设为1
	pfd.dwFlags =  PFD_DRAW_TO_WINDOW| //一组表明象素缓冲特性的标志位，如缓冲是否支持GDI或Graph等
	   PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;   //明象素数据类型是RGBA还是颜色索引;
	pfd.cColorBits = 16;     //每个颜色缓冲区中颜色位平面的数目，对颜色索引方式是缓冲区大小
	pfd.cDepthBits = 32;     //缓冲区的深度; 
	pfd.iLayerType = PFD_MAIN_PLANE; //被忽略，为了一致性而包含的
	iFormat = ChoosePixelFormat( hDCGraph, &pfd );//选择一个像素格式
	SetPixelFormat( hDCGraph, iFormat, &pfd ); //设置到DC中
	hRCGraph = wglCreateContext( hDCGraph );    //创建绘图描述表
	wglMakeCurrent( hDCGraph, hRCGraph );     //使之成为当前绘图描述表
}

// 取消 Graph ，在程序结束前调用，释放渲染环境，设备环境以及最终窗口句柄。
void DisableGraph()
{
	wglDeleteContext( hRCGraph );
	ReleaseDC( hWndGraph, hDCGraph );
}
void DrawGraph()
{
	int i;
	for(i=0;i<EditGraphParam.create_num+1;i++)
	{
		//锥加工、上下异性面绘图
		if(((EditGraphParam.pDataNcGraphMem+i)->G_Dcompensate==152)||((EditGraphParam.pDataNcGraphMem+i)->G_Dcompensate==153)||
			((EditGraphParam.pDataNcGraphMem+i)->G_Dcompensate==150)||((EditGraphParam.pDataNcGraphMem+i)->G_tapemode==50)||
			((EditGraphParam.pDataNcGraphMem+i)->G_tapemode==52)||((EditGraphParam.pDataNcGraphMem+i)->G_tapemode==51))
		{
			//上下平面均为直线直接绘图
			if(((EditGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==1)&&((EditGraphParam.pDataNcGraphMem+i)->G_pathmode_lower==1))
			{
				now_up_X=now_up_X+(EditGraphParam.pDataNcGraphMem+i)->B;
				now_down_X=now_down_X+(EditGraphParam.pDataNcGraphMem+i)->X;
				now_up_Y=now_up_Y+(EditGraphParam.pDataNcGraphMem+i)->C;
				now_down_Y=now_down_Y+(EditGraphParam.pDataNcGraphMem+i)->Y;
				Draw();
			}
			//上平面为圆，下平面为直线
			else if((((EditGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==2)||((EditGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==3))
				&&((EditGraphParam.pDataNcGraphMem+i)->G_pathmode_lower==1))
			{
				CircleToLine(0.0,0.0,(EditGraphParam.pDataNcGraphMem+i)->X,(EditGraphParam.pDataNcGraphMem+i)->Y,
					(EditGraphParam.pDataNcGraphMem+i)->I1,(EditGraphParam.pDataNcGraphMem+i)->J1,
					0.0,0.0,(EditGraphParam.pDataNcGraphMem+i)->B,(EditGraphParam.pDataNcGraphMem+i)->C,
					(EditGraphParam.pDataNcGraphMem+i)->G_pathmode_upper,2);
			}
			//上平面为直线，下平面为圆弧
			else if((((EditGraphParam.pDataNcGraphMem+i)->G_pathmode_lower==2)||((EditGraphParam.pDataNcGraphMem+i)->G_pathmode_lower==3))
				&&((EditGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==1))
			{
				CircleToLine(0.0,0.0,(EditGraphParam.pDataNcGraphMem+i)->B,(EditGraphParam.pDataNcGraphMem+i)->C,
					(EditGraphParam.pDataNcGraphMem+i)->I,(EditGraphParam.pDataNcGraphMem+i)->J,
					0.0,0.0,(EditGraphParam.pDataNcGraphMem+i)->X,(EditGraphParam.pDataNcGraphMem+i)->Y,
					(EditGraphParam.pDataNcGraphMem+i)->G_pathmode_lower,1);
			}
			//上下平面均为圆弧
			else if((((EditGraphParam.pDataNcGraphMem+i)->G_pathmode_lower==2)||((EditGraphParam.pDataNcGraphMem+i)->G_pathmode_lower==3))
				&&((EditGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==2)||((EditGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==3))
			{
				CircleToCircle((EditGraphParam.pDataNcGraphMem+i)->I1,(EditGraphParam.pDataNcGraphMem+i)->J1,0.0,0.0,
					(EditGraphParam.pDataNcGraphMem+i)->B,(EditGraphParam.pDataNcGraphMem+i)->C,
					(EditGraphParam.pDataNcGraphMem+i)->I,(EditGraphParam.pDataNcGraphMem+i)->J,0.0,0.0,
					(EditGraphParam.pDataNcGraphMem+i)->X,(EditGraphParam.pDataNcGraphMem+i)->Y,
					(EditGraphParam.pDataNcGraphMem+i)->G_pathmode_upper,(EditGraphParam.pDataNcGraphMem+i)->G_pathmode_lower,1);
			}
		}
		//常规加工
		else
		{
			if((EditGraphParam.pDataNcGraphMem+i)->G_pathmode==1)
			{
				now_up_X=now_up_X+(EditGraphParam.pDataNcGraphMem+i)->X;
				now_down_X=now_down_X+(EditGraphParam.pDataNcGraphMem+i)->X;
				now_up_Y=now_up_Y+(EditGraphParam.pDataNcGraphMem+i)->Y;
				now_down_Y=now_down_Y+(EditGraphParam.pDataNcGraphMem+i)->Y;
				Draw();
			}
			else if(((EditGraphParam.pDataNcGraphMem+i)->G_pathmode==2)||((EditGraphParam.pDataNcGraphMem+i)->G_pathmode==3))
			{
				CircleToCircle((EditGraphParam.pDataNcGraphMem+i)->I,(EditGraphParam.pDataNcGraphMem+i)->J,0.0,0.0,
					(EditGraphParam.pDataNcGraphMem+i)->X,(EditGraphParam.pDataNcGraphMem+i)->Y,
					(EditGraphParam.pDataNcGraphMem+i)->I,(EditGraphParam.pDataNcGraphMem+i)->J,0.0,0.0,
					(EditGraphParam.pDataNcGraphMem+i)->X,(EditGraphParam.pDataNcGraphMem+i)->Y,
					(EditGraphParam.pDataNcGraphMem+i)->G_pathmode,(EditGraphParam.pDataNcGraphMem+i)->G_pathmode,1);
			}
		}
	}
}
void Draw()
{
	double p1[3],p2[3],p3[3],p4[3];
	double n[3];
	p1[0]=pre_up_X;
	p1[1]=pre_up_Y;
	p1[2]=pre_up_Z;
	p2[0]=pre_down_X;
	p2[1]=pre_down_Y;
	p2[2]=pre_down_Z;
	p3[0]=now_up_X;
	p3[1]=now_up_Y;
	p3[2]=now_up_Z;
	p4[0]=now_down_X;
	p4[1]=now_down_Y;
	p4[2]=now_down_Z;
	CalNormal(p1,p2,p3,n);//计算平面法向量
	//四边形绘制分两部分，分别绘制三角形得到四边形
	glBegin(GL_TRIANGLES);
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f(p1[0],p1[1],p1[2]);
		glVertex3f(p2[0],p2[1],p2[2]);
		glVertex3f(p3[0],p3[1],p3[2]);
	glEnd();
	glBegin(GL_TRIANGLES);
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f(p2[0],p2[1],p2[2]);
		glVertex3f(p3[0],p3[1],p3[2]);
		glVertex3f(p4[0],p4[1],p4[2]);
	glEnd();
	pre_up_X=now_up_X;
	pre_up_Y=now_up_Y;
	pre_up_Z=now_up_Z=h_thickness;
	pre_down_X=now_down_X;
	pre_down_Y=now_down_Y;
	pre_down_Z=now_down_Z=0.0;
}
void CalNormal(double *p1,double *p2,double *p3,double *n)
{
	double a[3],b[3];
	double length;
	a[0]=p2[0]-p1[0];
	a[1]=p2[1]-p1[1];
	a[2]=p2[2]-p1[2];
	b[0]=p3[0]-p1[0];
	b[1]=p3[1]-p1[1];
	b[2]=p3[2]-p1[2];
	n[0]=a[1]*b[2]-a[2]*b[1];
	n[1]=a[2]*b[0]-a[0]*b[2];
	n[2]=a[0]*b[1]-a[1]*b[0];
	length=sqrt(n[0]*n[0]+n[1]*n[1]+n[2]*n[2]);
	if(length!=0)
	{
		n[0]=n[0]/length;
		n[1]=n[1]/length;
		n[2]=n[2]/length;
	}
}
void Draw3D()
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
	wglMakeCurrent(hDCGraph,hRCGraph);//得到当前DC
	SceneShow();//绘图
	SwapBuffers(hDCGraph);//双缓冲，将后台缓存移动到前台
	wglMakeCurrent(NULL,NULL);
}
void Draw3DReset()
{
	edit_turn_up_down=0.0;
	edit_turn_right_left=0.0;
	edit_up_down=0.0;
	edit_right_left=0.0;
	edit_zoom=1.0;
	edit_move_step=10.0;
	edit_turn_step=10.0;
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
	wglMakeCurrent(hDCGraph,hRCGraph);
	DrawReset();
	SwapBuffers(hDCGraph);
	wglMakeCurrent(NULL,NULL);
}