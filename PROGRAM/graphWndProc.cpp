#include "stdafx.h"
#include "EDM_NC_PROGRAM.h"

extern HINSTANCE hInst;								// ��ǰʵ��
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
				  int circularMode,int UpMode);//UpMode==1��һ����������ƽ��
extern void CircleToCircle(double Circle1CenterX,double Circle1CenterY,double Circle1StartX,double Circle1StartY,
					double Circle1EndX,double Circle1EndY,double Circle2CenterX,double Circle2CenterY,
					double Circle2StartX,double Circle2StartY,double Circle2EndX,double Circle2EndY,
					int circular1Mode,int circular2Mode,int UpMode);

//���ô��ڴ�С��б�ʼ�ȫ����־����
HGLRC hRCGraph=NULL;            //������Ⱦ����
HDC   hDCGraph=NULL;            //˽�е�GDI�豸����
HWND hWndGraph=NULL;           //�õ����ڵľ��
void SceneInit(int,int);//��ʼ��OPENGL����
void SceneShow(GLvoid);//��ʾ��ͼ
void EnableGraph();//ʹ��OPENGL
void DisableGraph();//ȡ��OPENGL
bool ChangeResolution(int,int,int);//�ı���Ļ�ֱ���
//Draw
void DrawGraph();//��ͼ����
void Draw();//�������ṩ�ĵ���������ı���
void CalNormal(double*,double*,double*,double*);//���������������ƽ��ķ�����
void DrawReset();//�ػ���Ӧ��ִ�й���
void Draw3D();
void Draw3DReset();
//
const MessageProc graphMessages[]={
	    WM_CREATE, goCreateGraph,
		WM_DESTROY, goDestroyGraph
};
int MyRegisterClassGraph(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;//���崰����ṹ�����
    //���ṹ���и���Ա
	wcex.cbSize = sizeof(WNDCLASSEX); //?????
	wcex.style			=CS_OWNDC;//�ͻ����ƶ�������/�߶ȸı�ʱ�ػ��ͻ���
	wcex.lpfnWndProc	= (WNDPROC)graphWndProc;//ָ�򴰿ڹ��̵ĳ�ָ��
	wcex.cbClsExtra		= 0;//ָ��������ṹ��洢��֮����⿪�ٵ��ֽڣ�ϵͳ��ʼ����Щ�ֽڵ�ַΪ0
	wcex.cbWndExtra		= 0;//ָ������ʵ�����⿪�ٵ��ֽ���
	wcex.hInstance		= hInstance;//ָ��ǰ���ڹ������ڵĳ���ʵ�����
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDC_EDM_NC_PROGRAM);//����ͼ��
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);//���
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);//��ɫ������ˢ
	wcex.lpszMenuName	= NULL;//(LPCTSTR)ID_Menu;//�˵���windowsCE��֧��
	wcex.lpszClassName	= TEXT("GRAPH");//ָ��һ���ַ���������ָ���Զ��崰���������
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);//��������
//ע����������
	if(!RegisterClassEx(&wcex)) return 0;
	return 1;
}
LRESULT CALLBACK graphWndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
	int i;
	//���Ҷ�Ӧ����ϢID,��ִ�ж�Ӧ����Ϣ�������
	for(i=0; i<dim(graphMessages);i++)
	{
		if(msgCode == graphMessages[i].uCode) return(*graphMessages[i].functionName)(hWnd,msgCode,wParam,lParam);
	}	
	//�Բ����ڵ���Ϣ������ȱʡ���ڹ���
	return DefWindowProc(hWnd, msgCode, wParam, lParam);
}
LRESULT goCreateGraph(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{	
	hWndGraph=hWnd;
	EnableGraph();//ʹ��OPGNGL
	SceneInit(500, 500);//��ʼ��ȫ�ֹ��ߴ��ڵ�
	wglMakeCurrent(NULL,NULL);//�ͷŵ�ǰDC
	//��ʼ��������
	calculateNumber=1000;
	minLength=0.001;
	maxLength=1.0;
	edit_turn_up_down=0.0;
	edit_turn_right_left=0.0;
	edit_up_down=0.0;
	edit_right_left=0.0;
	edit_zoom=1.0;
	edit_move_step=10.0;//ÿ�ΰ�ƽ�ư�ť��Ӧƽ��10.0mm
	edit_turn_step=10.0;//ÿ�ΰ�ѡ��ť��Ӧѡ��10��
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
	//�رգ��˳��������Graph���ڣ��Ա��ͷ�������Դ��Ȼ���˳�����
	DisableGraph();
	ChangeDisplaySettings(NULL, 0);
	PostQuitMessage(0);
	return 0;
}
//Graph��ʼ����ʼ
void SceneInit(int w,int h)
{
	//���������Ϣ
	float position[]={1.0,3.0,0.0,0.0};
	float ambient[]={0.2,0.2,0.2,1.0};
	float diffuse[]={1.0,1.0,1.0,1.0};
	float specular[]={1.0,1.0,1.0,1.0};
	//���ڴ�С500
	glViewport(0,0,500,500);
	//�趨ͶӰģʽ
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-100,100,-100,100,0,200);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();			//�õ�λ�����û���ǰ�ľ���

	glShadeModel(GL_SMOOTH);       //����ƽ����ɫ
	//
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      // ��ɫ���� 
	glClearDepth(400.0f);         //������Ȼ�����
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

void SceneShow(GLvoid)         //����������еĻ�ͼ����
{
	//�趨����������
	float specular[]={0.4,0.6,0.1,1.0};
	float low_shininess[]={10.0};
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,specular);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,low_shininess);
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	glPushMatrix();
	//�趨�۲�Ƕ�
		glTranslatef(0.0,0.0,-100.0);
		glTranslatef(0.0,edit_up_down,0.0);
		glTranslatef(edit_right_left,0.0,0.0);
		glScalef(edit_zoom,edit_zoom,edit_zoom);
		glRotatef(edit_turn_up_down,1.0,0.0,0.0);
		glRotatef(edit_turn_right_left,0.0,1.0,0.0);
		glPushMatrix();
			glColor4f(0.4,0.6,0.1,0.5);//����ͼ����ɫ
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			DrawGraph();//��ͼ
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
//�����Graph����
void EnableGraph()
{
	PIXELFORMATDESCRIPTOR pfd;
	int iFormat;
	hDCGraph = GetDC( hWndGraph );
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );    //�ظ�ʽ�����ӽṹ�Ĵ�С
	pfd.nVersion = 1;      //PIXELFORMATDESCRIPTOR�ṹ�İ汾��һ����Ϊ1
	pfd.dwFlags =  PFD_DRAW_TO_WINDOW| //һ��������ػ������Եı�־λ���绺���Ƿ�֧��GDI��Graph��
	   PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;   //����������������RGBA������ɫ����;
	pfd.cColorBits = 16;     //ÿ����ɫ����������ɫλƽ�����Ŀ������ɫ������ʽ�ǻ�������С
	pfd.cDepthBits = 32;     //�����������; 
	pfd.iLayerType = PFD_MAIN_PLANE; //�����ԣ�Ϊ��һ���Զ�������
	iFormat = ChoosePixelFormat( hDCGraph, &pfd );//ѡ��һ�����ظ�ʽ
	SetPixelFormat( hDCGraph, iFormat, &pfd ); //���õ�DC��
	hRCGraph = wglCreateContext( hDCGraph );    //������ͼ������
	wglMakeCurrent( hDCGraph, hRCGraph );     //ʹ֮��Ϊ��ǰ��ͼ������
}

// ȡ�� Graph ���ڳ������ǰ���ã��ͷ���Ⱦ�������豸�����Լ����մ��ھ����
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
		//׶�ӹ��������������ͼ
		if(((EditGraphParam.pDataNcGraphMem+i)->G_Dcompensate==152)||((EditGraphParam.pDataNcGraphMem+i)->G_Dcompensate==153)||
			((EditGraphParam.pDataNcGraphMem+i)->G_Dcompensate==150)||((EditGraphParam.pDataNcGraphMem+i)->G_tapemode==50)||
			((EditGraphParam.pDataNcGraphMem+i)->G_tapemode==52)||((EditGraphParam.pDataNcGraphMem+i)->G_tapemode==51))
		{
			//����ƽ���Ϊֱ��ֱ�ӻ�ͼ
			if(((EditGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==1)&&((EditGraphParam.pDataNcGraphMem+i)->G_pathmode_lower==1))
			{
				now_up_X=now_up_X+(EditGraphParam.pDataNcGraphMem+i)->B;
				now_down_X=now_down_X+(EditGraphParam.pDataNcGraphMem+i)->X;
				now_up_Y=now_up_Y+(EditGraphParam.pDataNcGraphMem+i)->C;
				now_down_Y=now_down_Y+(EditGraphParam.pDataNcGraphMem+i)->Y;
				Draw();
			}
			//��ƽ��ΪԲ����ƽ��Ϊֱ��
			else if((((EditGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==2)||((EditGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==3))
				&&((EditGraphParam.pDataNcGraphMem+i)->G_pathmode_lower==1))
			{
				CircleToLine(0.0,0.0,(EditGraphParam.pDataNcGraphMem+i)->X,(EditGraphParam.pDataNcGraphMem+i)->Y,
					(EditGraphParam.pDataNcGraphMem+i)->I1,(EditGraphParam.pDataNcGraphMem+i)->J1,
					0.0,0.0,(EditGraphParam.pDataNcGraphMem+i)->B,(EditGraphParam.pDataNcGraphMem+i)->C,
					(EditGraphParam.pDataNcGraphMem+i)->G_pathmode_upper,2);
			}
			//��ƽ��Ϊֱ�ߣ���ƽ��ΪԲ��
			else if((((EditGraphParam.pDataNcGraphMem+i)->G_pathmode_lower==2)||((EditGraphParam.pDataNcGraphMem+i)->G_pathmode_lower==3))
				&&((EditGraphParam.pDataNcGraphMem+i)->G_pathmode_upper==1))
			{
				CircleToLine(0.0,0.0,(EditGraphParam.pDataNcGraphMem+i)->B,(EditGraphParam.pDataNcGraphMem+i)->C,
					(EditGraphParam.pDataNcGraphMem+i)->I,(EditGraphParam.pDataNcGraphMem+i)->J,
					0.0,0.0,(EditGraphParam.pDataNcGraphMem+i)->X,(EditGraphParam.pDataNcGraphMem+i)->Y,
					(EditGraphParam.pDataNcGraphMem+i)->G_pathmode_lower,1);
			}
			//����ƽ���ΪԲ��
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
		//����ӹ�
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
	CalNormal(p1,p2,p3,n);//����ƽ�淨����
	//�ı��λ��Ʒ������֣��ֱ���������εõ��ı���
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
	wglMakeCurrent(hDCGraph,hRCGraph);//�õ���ǰDC
	SceneShow();//��ͼ
	SwapBuffers(hDCGraph);//˫���壬����̨�����ƶ���ǰ̨
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