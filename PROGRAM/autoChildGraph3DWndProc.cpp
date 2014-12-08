#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;								// ��ǰʵ��
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

//2010-5-31 ׶�ӹ������޸�
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
				  int circularMode,int UpMode);//UpMode==1��һ����������ƽ��
extern void CircleToCircle(double Circle1CenterX,double Circle1CenterY,double Circle1StartX,double Circle1StartY,
					double Circle1EndX,double Circle1EndY,double Circle2CenterX,double Circle2CenterY,
					double Circle2StartX,double Circle2StartY,double Circle2EndX,double Circle2EndY,
					int circular1Mode,int circular2Mode,int UpMode);
extern void LineToLine(double Line1StartX,double Line1StartY,double Line1EndX,double Line1EndY,
				double Line2StartX,double Line2StartY,double Line2EndX,double Line2EndY);
extern void drawColumn(double UpperX,double UpperY,double UpperZ,double DownX,double DownY,double DownZ);
extern void Draw();//�������ṩ�ĵ���������ı���
extern void CalNormal(double*,double*,double*,double*);//���������������ƽ��ķ�����
extern void DrawReset();//�ػ���Ӧ��ִ�й���
//***************************
//���ô��ڴ�С��б�ʼ�ȫ����־����
HGLRC hAutoRCGraph=NULL;            //������Ⱦ����
HDC   hAutoDCGraph=NULL;            //˽�е�GDI�豸����
HWND hAutoWndGraph=NULL;           //�õ����ڵľ��
void AutoSceneShow(GLvoid);//��ʾ��ͼ
void AutoSceneInit(int,int);//��ʼ��OPENGL����
void DrawAutoGraph();//��ͼ����
void DrawMachineMove();
void DrawMachine(double X,double Y,double Z,double UPMode);//UpMode Ϊ1��ʾΪ�ϵ��ܣ�Ϊ2��ʾΪ�µ���
void AutoEnableGraph();//ʹ��OPENGL
void AutoDisableGraph();//ȡ��OPENGL
void Auto3DDraw();
const MessageProc AutoGraphMessages[]={
	    WM_CREATE, goCreateAutoGraph,
		WM_DESTROY, goDestroyAutoGraph
};
int MyRegisterClassAutoGraph(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;//���崰����ṹ�����
    //���ṹ���и���Ա
	wcex.cbSize = sizeof(WNDCLASSEX); //?????
	wcex.style			=CS_OWNDC;//�ͻ����ƶ�������/�߶ȸı�ʱ�ػ��ͻ���
	wcex.lpfnWndProc	= (WNDPROC)AutoGraphWndProc;//ָ�򴰿ڹ��̵ĳ�ָ��
	wcex.cbClsExtra		= 0;//ָ��������ṹ��洢��֮����⿪�ٵ��ֽڣ�ϵͳ��ʼ����Щ�ֽڵ�ַΪ0
	wcex.cbWndExtra		= 0;//ָ������ʵ�����⿪�ٵ��ֽ���
	wcex.hInstance		= hInstance;//ָ��ǰ���ڹ������ڵĳ���ʵ�����
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDC_EDM_NC_PROGRAM);//����ͼ��
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);//���
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);//��ɫ������ˢ
	wcex.lpszMenuName	= NULL;//(LPCTSTR)ID_Menu;//�˵���windowsCE��֧��
	wcex.lpszClassName	= TEXT("AUTOGRAPH3DWND");//ָ��һ���ַ���������ָ���Զ��崰���������
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);//��������
//ע����������
	if(!RegisterClassEx(&wcex)) return 0;
	return 1;
}
LRESULT CALLBACK AutoGraphWndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;
	//���Ҷ�Ӧ����ϢID,��ִ�ж�Ӧ����Ϣ�������
	for(i=0; i<dim(AutoGraphMessages);i++)
	{
		if(msgCode == AutoGraphMessages[i].uCode) return(*AutoGraphMessages[i].functionName)(hWnd,msgCode,wParam,lParam);
	}
	//�Բ����ڵ���Ϣ������ȱʡ���ڹ���
	return DefWindowProc(hWnd, msgCode, wParam, lParam);
}
LRESULT goCreateAutoGraph(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{	
	hAutoWndGraph=hWnd;
	AutoEnableGraph();//ʹ��OPGNGL
	AutoSceneInit(395, 309);//��ʼ��ȫ�ֹ��ߴ��ڵ�
	wglMakeCurrent(NULL,NULL);//�ͷŵ�ǰDC
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
	//�رգ��˳��������AutoGraph���ڣ��Ա��ͷ�������Դ��Ȼ���˳�����
	AutoDisableGraph();
	ChangeDisplaySettings(NULL, 0);
	PostQuitMessage(0);
	return 0;
}
//AutoGraph��ʼ����ʼ
void AutoSceneInit(int w,int h)
{
	//���������Ϣ
	float position[]={1.0,3.0,0.0,0.0};
	float ambient[]={0.2,0.2,0.2,1.0};
	float diffuse[]={1.0,1.0,1.0,1.0};
	float specular[]={1.0,1.0,1.0,1.0};
	glViewport(0,0,395,309);
	//�趨ͶӰģʽ
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-300,300,-200,200,0,600);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();			//�õ�λ�����û���ǰ�ľ���

	glShadeModel(GL_SMOOTH);       //����ƽ����ɫ
	//
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      // ��ɫ���� 
	glClearDepth(600.0f);         //������Ȼ�����
	glEnable(GL_DEPTH_TEST);                            //������Ȳ���
	glDepthFunc(GL_LEQUAL);        //��Ȳ��Ե�����

	glHint(GL_PERSPECTIVE_CORRECTION_HINT|GL_LINE_SMOOTH_HINT|GL_POINT_SMOOTH_HINT|GL_POLYGON_SMOOTH_HINT, GL_NICEST); //͸��У������ʹ͸��Ч���Եø���һЩ
//�趨����
	glLightfv(GL_LIGHT0,GL_POSITION,position);
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);	
}

void AutoSceneShow(GLvoid)         //����������еĻ�ͼ����
{
	//�趨����������
	float specular[]={1.0,1.0,1.0,1.0};
	float low_shininess[]={10.0};
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,specular);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,low_shininess);
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	glPushMatrix();
	//�趨�۲�Ƕ�
		glTranslatef(0.0,0.0,-300.0);
		glTranslatef(0.0,auto_up_down,0.0);
		glTranslatef(auto_right_left,0.0,0.0);
		glScalef(auto_zoom,auto_zoom,auto_zoom);
		glRotatef(auto_turn_up_down,1.0,0.0,0.0);
		glRotatef(auto_turn_right_left,0.0,1.0,0.0);
		glPushMatrix();
			glColor4f(0.4,0.6,0.1,0.5);//����ͼ����ɫ
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			DrawAutoGraph();//��ͼ
			DrawMachineMove();
		glPopMatrix();
	glPopMatrix();
}  
//�����AutoGraph����
void EnableAutoGraph()
{
	PIXELFORMATDESCRIPTOR pfd;
	int iFormat;
	hAutoDCGraph = GetDC( hAutoWndGraph );
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );    //�ظ�ʽ�����ӽṹ�Ĵ�С
	pfd.nVersion = 1;      //PIXELFORMATDESCRIPTOR�ṹ�İ汾��һ����Ϊ1
	pfd.dwFlags =  PFD_DRAW_TO_WINDOW| //һ��������ػ������Եı�־λ���绺���Ƿ�֧��GDI��AutoGraph��
	   PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;   //����������������RGBA������ɫ����;
	pfd.cColorBits = 16;     //ÿ����ɫ����������ɫλƽ�����Ŀ������ɫ������ʽ�ǻ�������С
	pfd.cDepthBits = 32;     //�����������; 
	pfd.iLayerType = PFD_MAIN_PLANE; //�����ԣ�Ϊ��һ���Զ�������
	iFormat = ChoosePixelFormat( hAutoDCGraph, &pfd );//ѡ��һ�����ظ�ʽ
	SetPixelFormat( hAutoDCGraph, iFormat, &pfd ); //���õ�DC��
	hAutoRCGraph = wglCreateContext( hAutoDCGraph );    //������ͼ������
	wglMakeCurrent( hAutoDCGraph, hAutoRCGraph );     //ʹ֮��Ϊ��ǰ��ͼ������
}
void DrawAutoGraph()
{
	int i;
	for(i=0;i<AutoGraphParam.create_num+1;i++)
	{
		//׶�ӹ��������������ͼ
		if(((AutoGraphParam.pDataNcGraphMem+i)->G_Dcompensate==152)||((AutoGraphParam.pDataNcGraphMem+i)->G_Dcompensate==153)||((AutoGraphParam.pDataNcGraphMem+i)->G_Dcompensate==150)||((AutoGraphParam.pDataNcGraphMem+i)->G_tapemode==50)||((AutoGraphParam.pDataNcGraphMem+i)->G_tapemode==52)||((AutoGraphParam.pDataNcGraphMem+i)->G_tapemode==51))
		{
			//����ƽ���Ϊֱ��ֱ�ӻ�ͼ
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
			//��ƽ��ΪԲ����ƽ��Ϊֱ��
			else if((((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==2)||((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==3))
				&&((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_lower==1))
			{
				CircleToLine(0.0,0.0,(AutoGraphParam.pDataNcGraphMem+i)->X,(AutoGraphParam.pDataNcGraphMem+i)->Y,
					(AutoGraphParam.pDataNcGraphMem+i)->I1,(AutoGraphParam.pDataNcGraphMem+i)->J1,
					0.0,0.0,(AutoGraphParam.pDataNcGraphMem+i)->B,(AutoGraphParam.pDataNcGraphMem+i)->C,
					(AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_upper,2);
			}
			//��ƽ��Ϊֱ�ߣ���ƽ��ΪԲ��
			else if((((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_lower==2)||((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_lower==3))
				&&((AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==1))
			{
				CircleToLine(0.0,0.0,(AutoGraphParam.pDataNcGraphMem+i)->B,(AutoGraphParam.pDataNcGraphMem+i)->C,
					(AutoGraphParam.pDataNcGraphMem+i)->I,(AutoGraphParam.pDataNcGraphMem+i)->J,
					0.0,0.0,(AutoGraphParam.pDataNcGraphMem+i)->X,(AutoGraphParam.pDataNcGraphMem+i)->Y,
					(AutoGraphParam.pDataNcGraphMem+i)->G_pathmode_lower,1);
			}
			//����ƽ���ΪԲ��
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
		//����ӹ�
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
	CalNormal(PointDown1,PointDown2,PointDown3,n);//����ƽ�淨����
	glColor4f(1.0,1.0,1.0,0.5);//����ͼ����ɫ
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
	CalNormal(PointDown1,PointDown2,PointUp1,n);//����ƽ�淨����
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
	CalNormal(PointDown1,PointDown2,PointUp1,n);//����ƽ�淨����
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
//�����Graph����
void AutoEnableGraph()
{
	PIXELFORMATDESCRIPTOR pfd;
	int iFormat;
	hAutoDCGraph = GetDC(hAutoWndGraph);
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );    //�ظ�ʽ�����ӽṹ�Ĵ�С
	pfd.nVersion = 1;      //PIXELFORMATDESCRIPTOR�ṹ�İ汾��һ����Ϊ1
	pfd.dwFlags =  PFD_DRAW_TO_WINDOW| //һ��������ػ������Եı�־λ���绺���Ƿ�֧��GDI��Graph��
	   PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;   //����������������RGBA������ɫ����;
	pfd.cColorBits = 16;     //ÿ����ɫ����������ɫλƽ�����Ŀ������ɫ������ʽ�ǻ�������С
	pfd.cDepthBits = 32;     //�����������; 
	pfd.iLayerType = PFD_MAIN_PLANE; //�����ԣ�Ϊ��һ���Զ�������
	iFormat = ChoosePixelFormat( hAutoDCGraph, &pfd );//ѡ��һ�����ظ�ʽ
	SetPixelFormat( hAutoDCGraph, iFormat, &pfd ); //���õ�DC��
	hAutoRCGraph = wglCreateContext( hAutoDCGraph );    //������ͼ������
	wglMakeCurrent( hAutoDCGraph, hAutoRCGraph);     //ʹ֮��Ϊ��ǰ��ͼ������
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
	wglMakeCurrent(hAutoDCGraph,hAutoRCGraph);//�õ���ǰDC
	AutoSceneShow();//��ͼ
	SwapBuffers(hAutoDCGraph);//˫���壬����̨�����ƶ���ǰ̨
	wglMakeCurrent(NULL,NULL);
}