#include <stdio.h>
#include <math.h>
typedef struct 
{
	int G_pathmode;		//·���岹ģʽG00(G0)=0 G01(G1)=1 G02(G2)=2 G03(G3)=3
	double X , Y;	//����
	double I, J;		//Բ��������
}  nc_data;
nc_data cs[]={ {1,0,1,0,0},{1,-1,0,0,0},{1,0,1,0,0}, {2,2,0,1,-1},{2,0,-2,0,-1},{1,0,1,0,0},{1,-0.9,0,0,0},{1,0,-0.5,0,0} },cs1[100],cs2[100];
nc_data cs0[]= {{0,0,0,0,0},{0,0,0,0,0}};
//�������߲���֮��������Լ��켣������ϱ���Ĺ켣


struct mihua
{
	double X,Y;
} ; //�������
//ȫ�ֱ�������
double Xtop,Ytop,Xbtm,Ybtm;//ֱ�ߵ�һЩ����
double Xtopstart,Ytopstart,Xtopend, Ytopend,Xbtmstart,Ybtmstart,Xbtmend,Ybtmend,Rtoptop,Rbtmbtm,deltl;//Բ����һЩ����
int top,btm;//������¼�ܻ��˶��ٶ�
struct mihua m1[2000],m2[2000],tmp1[2000],tmp2[2000];//��¼ÿ���߶ε��ܻ���
int d=0;//��������ͼ�ε��ܻ�����


//һЩ����������
//����ֱ�ߵ��ܻ�����
void line(double X,double Y,double deltl,struct mihua tmp[],int *a )
{
	double len;
	int m;
	len=sqrt(X*X + Y*Y);
	for(m=0;len>=deltl;m++)
	{
		tmp[m].X=deltl*X/sqrt(X*X + Y*Y);
	    tmp[m].Y=deltl*Y/sqrt(X*X + Y*Y);
		len=len-deltl;
	}
	tmp[m].X=X-m*deltl*X/sqrt(X*X + Y*Y);
	tmp[m].Y=Y-m*deltl*Y/sqrt(X*X + Y*Y);
	*a = m;
}
// ���¶�Ӧֱ�ߵ��ܻ�����
void lineline(double Xtop,double Ytop,double Xbottom,double Ybottom,struct mihua tmp1[],struct mihua tmp2[],int *c1,int *c2)//ֱ�߶�ֱ�ߵĴ�����
{
	int c; 
	int m;// ��¼�ܻ�����
	double deltl2,deltl1;
	struct mihua tmp[2000];
	if( (Xtop*Xtop + Ytop*Ytop) >= (Xbottom*Xbottom + Ybottom*Ybottom) )
	{
		deltl2=0.04;
		line(Xbottom,Ybottom,deltl2,tmp,&c);//���±�����ܻ�����tmp[]�У�����¼��ĸ�����c�С�
		for(m=0;m<=c;m++)
		{
		tmp2[m]=tmp[m];
		}
		*c2 = c;
		deltl1=deltl2*sqrt( (Xtop*Xtop + Ytop*Ytop)/(Xbottom*Xbottom + Ybottom*Ybottom) );
		line(Xtop,Ytop,deltl1,tmp,&c);
		for(m=0;m<=c;m++)
		{
		tmp1[m]=tmp[m];
		}
		*c1 = c;
	}
	else
	{
		deltl1=0.04;
		line(Xtop,Ytop,deltl1,tmp,&c);
		for(m=0;m<=c;m++)
		{
		tmp1[m]=tmp[m];
		}
		*c1 = c;
		deltl2=deltl1*sqrt( (Xbottom*Xbottom + Ybottom*Ybottom)/(Xtop*Xtop + Ytop*Ytop) );
		line(Xbottom,Ybottom,deltl2,tmp,&c);
		for(m=0;m<=c;m++)
		{
		tmp2[m]=tmp[m];
		}
		*c2 = c;
	}
}
// ����Բ�����ܻ�����
void arc(double X1,double Y1,double X2,double Y2,double R, double deltl,struct mihua arc[],int *a ) //��һ��Բ���ܻ������÷����ްɣ�
{
	int i;//ѭ������
	double m,n,L,deltx,delty;
	m=deltl*sqrt(4*R*R-deltl*deltl)/(2*R*R);
	n=deltl*deltl/(2*R*R);
	L=sqrt( (X2-X1)*(X2-X1) + (Y2-Y1)*(Y2-Y1) );//ʣ���߶εĳ���
 	for(i=0;L>deltl;i++)
	{
		deltx=m*Y1-n*X1;
		delty=-(m*X1+n*Y1);
		arc[i].X=deltx;
		arc[i].Y=delty;
		X1+=deltx;
		Y1+=delty;
		L=sqrt((X2-X1)*(X2-X1) + (Y2-Y1)*(Y2-Y1) );
	}
	arc[i].X=X2-X1;
	arc[i].Y=Y2-Y1;
	*a = i;
}

void arc1(double X1,double Y1,double X2,double Y2,double R, double deltl,struct mihua arc[],int *a ) //��һ��Բ���ܻ������÷����ްɣ�
{
	int i;//ѭ������
	double m,n,L,deltx,delty;
	m=deltl*sqrt(4*R*R-deltl*deltl)/(2*R*R);
	n=deltl*deltl/(2*R*R);
	L=sqrt( (X2-X1)*(X2-X1) + (Y2-Y1)*(Y2-Y1) );//ʣ���߶εĳ���
 	for(i=0;L>deltl;i++)
	{
		deltx=-(m*Y1+n*X1);
		delty= (m*X1-n*Y1);
		arc[i].X=deltx;
		arc[i].Y=delty;
		X1+=deltx;
		Y1+=delty;
		L=sqrt((X2-X1)*(X2-X1) + (Y2-Y1)*(Y2-Y1) );
	}
	arc[i].X=X2-X1;
	arc[i].Y=Y2-Y1;
	*a = i;
}

// ���¶�ӦԲ�����ܻ�����
void arcarc(double Xtopstart,double Ytopstart,double Xtopend,double Ytopend,
			 double Xbtmstart,double Ybtmstart,double Xbtmend,double Ybtmend,
			 struct mihua tmp1[],struct mihua tmp2[],int *c1,int *c2
			 )
{
    struct mihua tmp[2000];
	int c;//��¼�ܻ��ĸ���
	int m;//ѭ������
	double  Rtop,Rbtm,deltl1,deltl2;  //����Բ���İ뾶���ڽ����߳�������
    Rtop = sqrt(Xtopstart*Xtopstart + Ytopstart*Ytopstart);
    Rbtm = sqrt(Xbtmstart*Xbtmstart + Ybtmstart*Ybtmstart);
	if(Rtop >= Rbtm)
	{
		deltl2 = sqrt(8*Rbtm*0.001);
		arc(Xbtmstart,Ybtmstart,Xbtmend,Ybtmend,Rbtm,deltl2,tmp,&c);
		for(m=0;m<=c;m++)
		{
		tmp2[m]=tmp[m];
		}
		*c2 = c;
        
		//������Բ���ĳ��ȣ���ȿ������Բ���Ĳ���
		deltl1 = deltl2 * Rtop*asin(sqrt((Xtopend-Xtopstart)*(Xtopend-Xtopstart)+(Ytopend-Ytopstart)*(Ytopend-Ytopstart))/(2*Rtop))/(Rbtm*asin(sqrt((Xbtmend-Xbtmstart)*(Xbtmend-Xbtmstart)+(Ybtmend-Ybtmstart)*(Ybtmend-Ybtmstart))/(2*Rbtm)));
        arc(Xtopstart,Ytopstart,Xtopend,Ytopend,Rtop,deltl1,tmp,&c);
		for(m=0;m<=c;m++)
		{
		tmp1[m]=tmp[m];
		}
		*c1 = c;
	}
	else
	{
		deltl1 = sqrt(8*Rtop*0.001);
		arc(Xtopstart,Ytopstart,Xtopend,Ytopend,Rtop,deltl1,tmp,&c);
		for(m=0;m<=c;m++)
		{
		tmp1[m]=tmp[m];
		}
		*c1 = c;
        
		//������Բ���ĳ��ȣ���ȿ������Բ���Ĳ���
		deltl2 = deltl1 / (Rtop*asin(sqrt((Xtopend-Xtopstart)*(Xtopend-Xtopstart)+(Ytopend-Ytopstart)*(Ytopend-Ytopstart))/(2*Rtop))/(Rbtm*asin(sqrt((Xbtmend-Xbtmstart)*(Xbtmend-Xbtmstart)+(Ybtmend-Ybtmstart)*(Ybtmend-Ybtmstart))/(2*Rbtm))));
		arc(Xbtmstart,Ybtmstart,Xbtmend,Ybtmend,Rbtm,deltl2,tmp,&c);
		for(m=0;m<=c;m++)
		{
		tmp2[m]=tmp[m];
		}
		*c2 = c;
	}
}
//������������



main()
{
    
	int N=7; //�����Ԫ�ظ���
	//double H;//�ο���๤���ϱ���ľ���
	//float a;//׶�Ƚ�
	double R=0.2;//�൱�ڵ��߲����ĵ��߰뾶
	double X0,Y0,X1,Y1,d0,d1,r0x,r0y,r1x,r1y;
	double S1x,S1y,S2x,S2y,S3x,S3y,K1,K2,Sx,Sy,Sx1,Sy1,Sx2,Sy2,Start_X=0,Start_Y=0,Start_X2=0,Start_Y2=0;
	double I_one,J_one,I_two,J_two,KBx0,KBy0,KBx1,KBy1,Ox,Oy,K_arc,a_arc,b_arc,c_arc,r_arc,Kline;
	int j=0,i=0,m;
	//R=H*tan(a);//���±���������xyƽ���ϵ�ͶӰ֮��ľ���

    

	if(cs[j].G_pathmode == 1 && cs[j+1].G_pathmode ==1)//ֱ�߽�ֱ��
	{
		printf("1\n");
		X0 = cs[j].X;
		Y0 = cs[j].Y;
		X1 = cs[j+1].X; 
		Y1 = cs[j+1].Y;
		//��ʼ��ƫ��ʸ��(��ƫ)
		d0 = sqrt(X0*X0 +Y0*Y0);	
		d1 = sqrt(X1*X1 +Y1*Y1);
		r0x = 0;
		r0y = 0;
		r1x = R*Y1/d1;
		r1y = (-R)*X1/d1;
		//�жϽ�������
		if( ( (r1x-r0x)*X0 + (r1y-r0y)*Y0 )<0 )//������
		{
			//���ϱ����Ӧ�ĵ�
			if(X0 == 0)
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;
				S2x=X0 + r1x;S2y=Y0 + r1y;K2=Y1/X1;
				Sx =S1x;
				Sy =K2*(Sx - S2x) + S2y;
			}
			else if(X1 ==0)
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;K1=Y0/X0;
				S2x=X0 + r1x;S2y=Y0 + r1y;
				Sx =S2x;
				Sy =K1*(Sx - S1x) + S1y;
			}
			else
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;K1=Y0/X0;
				S2x=X0 + r1x;S2y=Y0 + r1y;K2=Y1/X1;
				Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
				Sy = K1*(Sx - S1x) + S1y;
			}
			//ת������
			cs1[i]= cs[j];
			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;
			cs2[i]= cs[j];
			i++;
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;
		}
		else //�쳤��
		{
			//���ϱ����Ӧ�ĵ�
			if(X0 == 0)
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;
				S2x=X0 + r1x;S2y=Y0 + r1y;K2=Y1/X1;
				Sx =S1x;
				Sy =K2*(Sx - S2x) + S2y;
			}
			else if(X1 ==0)
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;K1=Y0/X0;
				S2x=X0 + r1x;S2y=Y0 + r1y;
				Sx =S2x;
				Sy =K1*(Sx - S1x) + S1y;
			}
			else
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;K1=Y0/X0;
				S2x=X0 + r1x;S2y=Y0 + r1y;K2=Y1/X1;
				Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
				Sy = K1*(Sx - S1x) + S1y;
			}
			//ת������
			cs1[i]= cs[j];
			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;
			cs2[i]= cs[j];
			i++;
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;
		}
	}//ֱ�߽�ֱ�߽���

	else if(cs[j].G_pathmode == 1 && (cs[j+1].G_pathmode ==3 || cs[j+1].G_pathmode ==2))//ֱ�߽�Բ��
	{
			printf("3\n");
		X0 = cs[j].X;
		Y0 = cs[j].Y;

		I_two=cs[j+1].I;
		J_two=cs[j+1].J;

		X1 = cs[j+1].X;
		Y1 = cs[j+1].Y;

		d0 = sqrt(X0*X0 + Y0*Y0);
		d1 = sqrt((X1-I_two)*(X1-I_two)+(Y1-J_two)*(Y1-J_two));	

		r0x = 0;
		r0y = 0;		
		if(cs[j+1].G_pathmode == 2)//Բ��˳ʱ��
		{
		    r1x = (-R)*(-I_two)/d1;
			r1y = (-R)*(-J_two)/d1;
		}
		else if (cs[j+1].G_pathmode == 3)//Բ����ʱ��
		{
			r1x = R*(-I_two)/d1;
			r1y = R*(-J_two)/d1;
		}

	
		//ȷ��KB�����ڶ���Բ����������б��
		if(cs[j+1].G_pathmode == 2)
		{
			KBx1 = -J_two;
			KBy1 =  I_two;
		}
		else if(cs[j+1].G_pathmode == 3)
		{
			KBx1 =  J_two;
			KBy1 = -I_two;
		}// ȷ��KB��������;

		//�жϽ������ͣ���յ��Ӧ�����꣬������
		if( (X0*(r1x-r0x)+Y0*(r1y-r0y))>0  )//�쳤��
		{
			printf("3.1\n");
			if(KBx1 == 0 && X0 != 0)
			{
			S1x= X0+r0x;	S1y=Y0+r0y;  K1=Y0/X0;
			S2x= X0+r1x;	S2y=Y0+r1y; 
							
			Sx = S2x;
			Sy = K1*(Sx - S1x) + S1y;//ֱ��б�����޴�ʱ;					
			}
			else if(KBx1 != 0 && X0 == 0)
			{
			S1x= X0+r0x;	S1y=Y0+r0y;   
			S2x= X0+r1x;	S2y=Y0+r1y;	 K2=KBy1/KBx1;
							
			Sx = S1x;
			Sy = K2*(Sx - S2x) + S2y;//ֱ��б�����޴�ʱ;					
			}
			
			else
			{
				S1x= X0+r0x;	S1y=Y0+r0y;  K1=Y0/X0;
				S2x= X0+r1x;	S2y=Y0+r1y;	 K2=KBy1/KBx1;
			
							
			Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
			Sy = K1*(Sx - S1x) + S1y;
			}//�쳤�ߵĽ�������;
			//ת������
			cs1[i]= cs[j];
			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;//��һ��ֱ�ߵ��յ�
			cs2[i]= cs[j];//�±����Ӧ��
			i++;

			cs1[i].X = S2x - Sx;
			cs1[i].Y = S2y - Sy;//Բ�����ߵ��쳤����
			cs1[i].G_pathmode = 1;
			cs2[i]= cs0[0];//�±����Ӧ��
			i++;

			Start_X = S2x - X0;	
			Start_Y = S2y - Y0;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
		}//�쳤�ͽ���
		else if ( (r0x == r1x) && (r0y == r1y) )//������
			{
			printf("3.2\n");
			Sx = X0+r0x;
			Sy = Y0+r0y;
			cs1[i]= cs[j];
            cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;//��һ��ֱ�ߵ��յ�
			cs2[i]= cs[j];//�±����Ӧ��
			i++;
			printf("1233\n");
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
			}//�����ͽ���;


		else if ( (X0*(r1x-r0x)+Y0*(r1y-r0y) )<0||(cs[j+1].G_pathmode==3 && r0x == -(r1x) && r0y == -(r1y) ) )// ������
		{
			printf("3.3\n");
			S1x=X0+r0x;	S1y=Y0+r0y;
			S3x=r0x;	S3y=r0y;//��Բ���ཻ��ֱ�ߵ�������;

			Ox=X0+I_two;	Oy=Y0+J_two;//Բ������;
			r_arc=sqrt( (X0+r1x-Ox)*(X0+r1x-Ox) + (Y0+r1y-Oy)*(Y0+r1y-Oy) );//Բ���뾶
			if(S1x == S3x)
			{
			Sx1=S1x ;	Sy1=Oy + sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));
			Sx2=S1x ;	Sy2=Oy - sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));
			}
			else if(S1y == S3y)
			{
			Sx1=Ox + sqrt(r_arc*r_arc-(S1y-Oy)*(S1y-Oy));	Sy1=S1y;
			Sx2=Ox - sqrt(r_arc*r_arc-(S1y-Oy)*(S1y-Oy));	Sy2=S1y;
			}
			else
			{
			K_arc=(S3y-S1y)/(S3x-S1x);
			a_arc=1+K_arc*K_arc;
			b_arc=2*K_arc*(S1y-Oy-K_arc*S1x-Ox/K_arc);
			c_arc=(S1y-Oy-K_arc*S1x)*(S1y-Oy-K_arc*S1x)-r_arc*r_arc+Ox*Ox;
							
			Sx1=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sy1=S1y + K_arc*(Sx1-S1x);
			Sx2=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sy2=S1y + K_arc*(Sx2-S1x);
			}//��������;
			//�ж������������е��ĸ�
			if( ((r1x*X0+r1y*Y0)>=0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)<=0) || ((r1x*X1+r1y*Y1)<0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)>0) )
							{
							Sx=Sx2;
							Sy=Sy2;
							}
			else if( ((r1x*X0+r1y*Y0)>=0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)>0) || ((r1x*X0+r1y*Y0)<0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)<=0) )
							{
							Sx=Sx1;
							Sy=Sy1;
							}



			//����ת��
			cs1[i]= cs[j];



			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;//��һ��ֱ�ߵ��յ�

			cs2[i]= cs[j];//�±����Ӧ��
			i++;
			
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
		}//�����ͽ���

		else if( r0x == -(r1x) && r0y == -(r1y) && cs[j+1].G_pathmode==2)// ���ཻ��
		{
			printf("3.4\n");
			S1x= X0+r0x;	S1y=Y0+r0y; 
			S2x= X0+r1x;	S2y=Y0+r1y;
			//��������Բ��+ת������
			cs1[i]= cs[j];
			cs1[i].X = S1x - Start_X;
			cs1[i].Y = S1y - Start_Y;//��һ��ֱ�ߵ��յ�
			cs2[i]= cs[j];//�±����Ӧ��
			i++;

			cs1[i].X = S2x - S1x;
			cs1[i].Y = S2y - S1y;//����Բ���ߵ��յ�����
			cs1[i].I = -(r0x);
			cs1[i].J = -(r0y);//����Բ����Բ������
			if(cs[j+1].G_pathmode == 2) 
				cs1[i].G_pathmode = 3;
			else if(cs[j+1].G_pathmode == 3)
				cs1[i].G_pathmode = 2;
			cs2[i]= cs0[0];//�±����Ӧ��
			i++;

			Start_X = r1x;	
			Start_Y = r1y;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
		}//���ཻ�ͽ���
	}//ֱ�߽�Բ������




//����֮������
for(j=1;j<N-1;j++)
{
	if(cs[j].G_pathmode == 1 && cs[j+1].G_pathmode ==1)//ֱ�߽�ֱ��
	{
		printf("1\n");
		X0 = cs[j].X;
		Y0 = cs[j].Y;
		X1 = cs[j+1].X; 
		Y1 = cs[j+1].Y;
		//��ʼ��ƫ��ʸ��(��ƫ)
		d0 = sqrt(X0*X0 +Y0*Y0);	
		d1 = sqrt(X1*X1 +Y1*Y1);
		r0x = R*Y0/d0;
		r0y = (-R)*X0/d0;
		r1x = R*Y1/d1;
		r1y = (-R)*X1/d1;
		//�жϽ�������
		if( ( (r1x-r0x)*X0 + (r1y-r0y)*Y0 )<0 )//������
		{
			//���ϱ����Ӧ�ĵ�
			if(X0 == 0)
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;
				S2x=X0 + r1x;S2y=Y0 + r1y;K2=Y1/X1;
				Sx =S1x;
				Sy =K2*(Sx - S2x) + S2y;
			}
			else if(X1 ==0)
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;K1=Y0/X0;
				S2x=X0 + r1x;S2y=Y0 + r1y;
				Sx =S2x;
				Sy =K1*(Sx - S1x) + S1y;
			}
			else
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;K1=Y0/X0;
				S2x=X0 + r1x;S2y=Y0 + r1y;K2=Y1/X1;
				Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
				Sy = K1*(Sx - S1x) + S1y;
			}
			//ת������
			/*cs1[i]= cs[j];
			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;
			cs2[i]= cs[j];
			i++;
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;*/
			cs1[i]= cs[j];
			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;
			
			
			cs2[i]= cs[j];
			cs2[i].X= -r0x + Sx - Start_X2;
			cs2[i].Y= -r0y + Sy - Start_Y2;
			
			i++;
			//Բ������
			cs1[i]=cs0[0];
			cs2[i].G_pathmode = 2;
			cs2[i].X= r0x - r1x;
			cs2[i].Y= r0y - r1y;
			cs2[i].I=  r0x;
			cs2[i].J=  r0y;
			i++;

			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;
			Start_X2 = -r1x + Start_X;	
			Start_Y2 = -r1y + Start_Y;




		}
		else //�쳤��
		{
			//���ϱ����Ӧ�ĵ�
			if(X0 == 0)
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;
				S2x=X0 + r1x;S2y=Y0 + r1y;K2=Y1/X1;
				Sx =S1x;
				Sy =K2*(Sx - S2x) + S2y;
			}
			else if(X1 ==0)
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;K1=Y0/X0;
				S2x=X0 + r1x;S2y=Y0 + r1y;
				Sx =S2x;
				Sy =K1*(Sx - S1x) + S1y;
			}
			else
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;K1=Y0/X0;
				S2x=X0 + r1x;S2y=Y0 + r1y;K2=Y1/X1;
				Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
				Sy = K1*(Sx - S1x) + S1y;
			}
			//ת������
			cs1[i]= cs[j];
			cs1[i].X = S1x - Start_X;
			cs1[i].Y = S1y - Start_Y;
			cs2[i]= cs[j];
			cs2[i].X = cs[j].X - Start_X2;
			cs2[i].Y = cs[j].Y - Start_Y2;
			i++;

			//Բ������
			
			cs1[i].G_pathmode = 3;
			cs1[i].X= r1x - r0x;
			cs1[i].Y= r1y - r0y;
			cs1[i].I=  -r0x;
			cs1[i].J=  -r0y;

			cs2[i]=cs0[0];
			i++;

			Start_X = S2x - X0;	
			Start_Y = S2y - Y0;
			Start_X2 = 0;	
			Start_Y2 = 0;

		}
	}//ֱ�߽�ֱ�߽���
else if(cs[j+1].G_pathmode == 1 && (cs[j].G_pathmode ==3 || cs[j].G_pathmode ==2))//Բ����ֱ��
{
	printf("2\n");

		X0 = cs[j].X;
		Y0 = cs[j].Y;
		I_one=cs[j].I;
		J_one=cs[j].J;

		X1 = cs[j+1].X;
		Y1 = cs[j+1].Y;

		d0 = sqrt((X0-I_one)*(X0-I_one)+(Y0-J_one)*(Y0-J_one));	
		d1 = sqrt(X1*X1 +Y1*Y1);
		printf("%f,%f\n",d0,d1);
		if(cs[j].G_pathmode == 2)//Բ��˳ʱ��
		{
		    r0x = (-R)*(X0-I_one)/d0;
			r0y = (-R)*(Y0-J_one)/d0;
			printf("%f,%f\n",r0x,r0y);
		}
		else if (cs[j].G_pathmode == 3)//Բ����ʱ��
		{
			r0x = (R)*(X0-I_one)/d0;
			r0y = (R)*(Y0-J_one)/d0;
		}

		r1x = R*Y1/d1;
		r1y = (-R)*X1/d1;
		printf("%f,%f\n",r1x,r1y);
		//ȷ��KB����Բ���յ������б��
		if(cs[j].G_pathmode == 2)
		{
			KBx0 = Y0-J_one;
			KBy0 = -(X0-I_one);
		}
		else if(cs[j].G_pathmode == 3)
		{
			KBx0 = -(Y0-J_one);
			KBy0 =  (X0-I_one);
		}// ȷ��KB��������;
		//�жϽ������ͣ���յ��Ӧ�����꣬������
		if( (KBx0*(r1x-r0x)+KBy0*(r1y-r0y))>0 )//�쳤��
		{

			printf("2.1\n");
			if(KBx0 == 0 && X1 != 0)
			{
			S1x= X0+r0x;	S1y=Y0+r0y; 
			S2x= X0+r1x;	S2y=Y0+r1y; K2=Y1/X1;
							
			Sx = S1x;
			Sy = K2*(Sx - S2x) + S2y;//ֱ��б�����޴�ʱ;					
			}
			else if(KBx0 != 0 && X1 == 0)
			{
			S1x= X0+r0x;	S1y=Y0+r0y;  K1=KBy0/KBx0; 
			S2x= X0+r1x;	S2y=Y0+r1y;
							
			Sx = S2x;
			Sy = K1*(Sx - S1x) + S1y;//ֱ��б�����޴�ʱ;					
			}
			
			else
			{
			S1x= X0+r0x;	S1y=Y0+r0y;  K1=KBy0/KBx0; 
			S2x= X0+r1x;	S2y=Y0+r1y; K2=Y1/X1;
							
			Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
			Sy = K1*(Sx - S1x) + S1y;
			}//�쳤�ߵĽ�������;
			//ת������
			/*cs1[i]= cs[j];
			cs1[i].X = S1x - Start_X;
			cs1[i].Y = S1y - Start_Y;//��һ��Բ�����յ�

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//Բ����Բ������

			cs2[i]= cs[j];//�±����Ӧ��
			i++;

			cs1[i].X = Sx - S1x;
			cs1[i].Y = Sy - S1y;//Բ�����ߵ��쳤����
			cs1[i].G_pathmode = 1;

			cs2[i]= cs0[0];//�±����Ӧ��,���һ����㣬ֻ��Ϊ�˺��ϱ���Ԫ�ظ������Զ�Ӧ��
			i++;

			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
			*/
			cs1[i]= cs[j];
			cs1[i].X = S1x - Start_X;
			cs1[i].Y = S1y - Start_Y;//��һ��Բ�����յ�

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//Բ����Բ������

			cs2[i]= cs[j];
			cs2[i].X = cs[j].X - Start_X2;
			cs2[i].Y = cs[j].Y - Start_Y2;
			cs2[i].I= cs[j].I - Start_X2;
			cs2[i].J= cs[j].J - Start_Y2;
			i++;

		
			cs1[i].G_pathmode = 3;
			cs1[i].X = r1x-r0x;
			cs1[i].Y = r1y-r0y;
			cs1[i].I = -r0x;
			cs1[i].J = -r0y;
			cs2[i]= cs0[0];//�±����Ӧ��
			i++;
			Start_X = S2x - X0;	
			Start_Y = S2y - Y0;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
			Start_X2 = 0;	
			Start_Y2 = 0;


		  }
			//�쳤�ͽ���
		else if  ( r0x == r1x && r0y == r1y )// ������
			{
			printf("2.2\n");
			Sx = X0+r0x;
			Sy = Y0+r0y;
			cs1[i]= cs[j];
            cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;//��һ��Բ�����յ�

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//Բ����Բ������

			cs2[i]= cs[j];//�±����Ӧ��
			i++;
			
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
			}//�����ͽ���;
		else if ( (KBx0*(r1x-r0x)+KBy0*(r1y-r0y))<0 || (r0x == -(r1x) && r0y == -(r1y)&&cs[j].G_pathmode == 3))// ������
		{
			printf("2.3\n");
			S2x=X0+r1x;	S2y=Y0+r1y;
			S3x=X0+X1+r1x;	S3y=Y0+Y1+r1y;//��Բ���ཻ��ֱ�ߵ�������;
			printf("%f,%f,%f,%f\n",S2x,S2y,S3x,S3y);

			Ox=I_one;	Oy=J_one;//Բ������;
			r_arc=sqrt( (X0+r0x-I_one)*(X0+r0x-I_one) + (Y0+r0y-J_one)*(Y0+r0y-J_one) );//Բ���뾶
			printf("%f\n",r_arc);
			if(S2x == S3x)
			{
			Sx1=S2x ;	Sy1=Oy + sqrt(r_arc*r_arc-(S2x-Ox)*(S2x-Ox));
			Sx2=S2x ;	Sy2=Oy - sqrt(r_arc*r_arc-(S2x-Ox)*(S2x-Ox));
			}
			else if(S2y == S3y)
			{
			Sx1=Ox + sqrt(r_arc*r_arc-(S2y-Oy)*(S2y-Oy));	Sy1=S2y;
			Sx2=Ox - sqrt(r_arc*r_arc-(S2y-Oy)*(S2y-Oy));	Sy2=S2y;
			}
			else
			{
			K_arc=(S3y-S2y)/(S3x-S2x);
			a_arc=1+K_arc*K_arc;
			b_arc=2*K_arc*(S2y-Oy-K_arc*S2x-Ox/K_arc);
			c_arc=(S2y-Oy-K_arc*S2x)*(S2y-Oy-K_arc*S2x)-r_arc*r_arc+Ox*Ox;
							
			Sx1=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sy1=S2y + K_arc*(Sx1-S2x);
			Sx2=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sy2=S2y + K_arc*(Sx2-S2x);
			}//��������;
			//�ж������������е��ĸ�
			if( ((r0x*X1+r0y*Y1)>=0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)<=0) || ((r0x*X1+r0y*Y1)<0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)>0) )
							{
							Sx=Sx2;
							Sy=Sy2;
							}
			else if( ((r0x*X1+r0y*Y1)>=0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)>0) || ((r0x*X1+r0y*Y1)<0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)<=0) )
							{
							Sx=Sx1;
							Sy=Sy1;
							}



			//����ת��
			/*cs1[i]= cs[j];



			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;//��һ��Բ�����յ�

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//Բ����Բ������

			cs2[i]= cs[j];//�±����Ӧ��
			i++;
			*/
			cs1[i]= cs[j];
			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;
			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//Բ����Բ������
			cs2[i]= cs[j];
			cs2[i].X= -r0x + Sx - Start_X2;
			cs2[i].Y= -r0y + Sy - Start_Y2; 
			cs2[i].I= cs[j].I - Start_X2;
			cs2[i].J= cs[j].J - Start_Y2;
			i++;
			//Բ������
			cs1[i]=cs0[0];
			cs2[i].G_pathmode=2;
			cs2[i].X= r0x - r1x;
			cs2[i].Y= r0y - r1y;
			cs2[i].I=  r0x;
			cs2[i].J=  r0y;
			i++;

			

			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;// ע�⣺ת�ӵ������Ȼ������ڲο�����ԭ���ĵ㡣��Ϊ��һ���ߵĶ˵���һ�����Ϊ����ԭ��ġ�ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
			Start_X2 = -r1x + Start_X;	
			Start_Y2 = -r1y + Start_Y;
		}//�����ͽ���

		else if( r0x == -(r1x) && r0y == -(r1y) && cs[j].G_pathmode == 2 )// ���ཻ��
		{
			printf("2.4\n");
			S1x= X0+r0x;	S1y=Y0+r0y; 
			S2x= X0+r1x;	S2y=Y0+r1y;
			//��������Բ��+ת������
			cs1[i]= cs[j];
			cs1[i].X = S1x - Start_X;
			cs1[i].Y = S1y - Start_Y;//��һ��Բ�����յ�

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//Բ����Բ������

			cs2[i]= cs[j];//�±����Ӧ��
			i++;
			cs1[i].X = S2x - S1x;
			cs1[i].Y = S2y - S1y;//����Բ���ߵ��յ�����
			cs1[i].I = -(r0x);
			cs1[i].J = -(r0y);//����Բ����Բ������
			if(cs[j].G_pathmode == 2) 
				cs1[i].G_pathmode = 3;
			else if(cs[j].G_pathmode == 3)
				cs1[i].G_pathmode = 2;

			cs2[i]= cs0[0];//�±����Ӧ��
			i++;

			Start_X = r1x;	
			Start_Y = r1y;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
		}//���ཻ�ͽ���
	}//Բ����ֱ�߽���
	
		else if(cs[j].G_pathmode == 1 && (cs[j+1].G_pathmode ==3 || cs[j+1].G_pathmode ==2))//ֱ�߽�Բ��
	{
			printf("3\n");
		X0 = cs[j].X;
		Y0 = cs[j].Y;

		I_two=cs[j+1].I;
		J_two=cs[j+1].J;

		X1 = cs[j+1].X;
		Y1 = cs[j+1].Y;

		d0 = sqrt(X0*X0 + Y0*Y0);
		d1 = sqrt((X1-I_two)*(X1-I_two)+(Y1-J_two)*(Y1-J_two));	

		r0x = R*Y0/d0;
		r0y = (-R)*X0/d0;		
		if(cs[j+1].G_pathmode == 2)//Բ��˳ʱ��
		{
		    r1x = (-R)*(-I_two)/d1;
			r1y = (-R)*(-J_two)/d1;
		}
		else if (cs[j+1].G_pathmode == 3)//Բ����ʱ��
		{
			r1x = R*(-I_two)/d1;
			r1y = R*(-J_two)/d1;
		}

	
		//ȷ��KB�����ڶ���Բ����������б��
		if(cs[j+1].G_pathmode == 2)
		{
			KBx1 = -J_two;
			KBy1 =  I_two;
		}
		else if(cs[j+1].G_pathmode == 3)
		{
			KBx1 =  J_two;
			KBy1 = -I_two;
		}// ȷ��KB��������;

		//�жϽ������ͣ���յ��Ӧ�����꣬������
		if( (X0*(r1x-r0x)+Y0*(r1y-r0y))>0  )//�쳤��
		{
			printf("3.1\n");
			if(KBx1 == 0 && X0 != 0)
			{
			S1x= X0+r0x;	S1y=Y0+r0y;  K1=Y0/X0;
			S2x= X0+r1x;	S2y=Y0+r1y; 
							
			Sx = S2x;
			Sy = K1*(Sx - S1x) + S1y;//ֱ��б�����޴�ʱ;					
			}
			else if(KBx1 != 0 && X0 == 0)
			{
			S1x= X0+r0x;	S1y=Y0+r0y;   
			S2x= X0+r1x;	S2y=Y0+r1y;	 K2=KBy1/KBx1;
							
			Sx = S1x;
			Sy = K2*(Sx - S2x) + S2y;//ֱ��б�����޴�ʱ;					
			}
			
			else
			{
				S1x= X0+r0x;	S1y=Y0+r0y;  K1=Y0/X0;
				S2x= X0+r1x;	S2y=Y0+r1y;	 K2=KBy1/KBx1;
			
							
			Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
			Sy = K1*(Sx - S1x) + S1y;
			}//�쳤�ߵĽ�������;
			//ת������
			/*cs1[i]= cs[j];
			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;//��һ��ֱ�ߵ��յ�
			cs2[i]= cs[j];//�±����Ӧ��
			i++;

			cs1[i].X = S2x - Sx;
			cs1[i].Y = S2y - Sy;//Բ�����ߵ��쳤����
			cs1[i].G_pathmode = 1;
			cs2[i]= cs0[0];//�±����Ӧ��
			i++;*/
            cs1[i]= cs[j];
			cs1[i].X = S1x - Start_X;
			cs1[i].Y = S1y - Start_Y;
			cs2[i]= cs[j];
			cs2[i].X = cs[j].X - Start_X2;
			cs2[i].Y = cs[j].Y - Start_Y2;
			i++;

			//Բ������
			
			cs1[i].G_pathmode = 3;
			cs1[i].X= r1x - r0x;
			cs1[i].Y= r1y - r0y;
			cs1[i].I=  -r0x;
			cs1[i].J=  -r0y;

			cs2[i]=cs0[0];
			i++;


			Start_X = S2x - X0;	
			Start_Y = S2y - Y0;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
			Start_X2 = 0;	
			Start_Y2 = 0;
		}//�쳤�ͽ���
		else if ( (r0x == r1x) && (r0y == r1y) )//������
			{
			printf("3.2\n");
			Sx = X0+r0x;
			Sy = Y0+r0y;
			cs1[i]= cs[j];
            cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;//��һ��ֱ�ߵ��յ�
			cs2[i]= cs[j];//�±����Ӧ��
			i++;
			printf("1233\n");
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
			}//�����ͽ���;


		else if ( (X0*(r1x-r0x)+Y0*(r1y-r0y) )<0||(cs[j+1].G_pathmode==3 && r0x == -(r1x) && r0y == -(r1y) ) )// ������
		{
			printf("3.3\n");
			S1x=X0+r0x;	S1y=Y0+r0y;
			S3x=r0x;	S3y=r0y;//��Բ���ཻ��ֱ�ߵ�������;

			Ox=X0+I_two;	Oy=Y0+J_two;//Բ������;
			r_arc=sqrt( (X0+r1x-Ox)*(X0+r1x-Ox) + (Y0+r1y-Oy)*(Y0+r1y-Oy) );//Բ���뾶
			if(S1x == S3x)
			{
			Sx1=S1x ;	Sy1=Oy + sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));
			Sx2=S1x ;	Sy2=Oy - sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));
			}
			else if(S1y == S3y)
			{
			Sx1=Ox + sqrt(r_arc*r_arc-(S1y-Oy)*(S1y-Oy));	Sy1=S1y;
			Sx2=Ox - sqrt(r_arc*r_arc-(S1y-Oy)*(S1y-Oy));	Sy2=S1y;
			}
			else
			{
			K_arc=(S3y-S1y)/(S3x-S1x);
			a_arc=1+K_arc*K_arc;
			b_arc=2*K_arc*(S1y-Oy-K_arc*S1x-Ox/K_arc);
			c_arc=(S1y-Oy-K_arc*S1x)*(S1y-Oy-K_arc*S1x)-r_arc*r_arc+Ox*Ox;
							
			Sx1=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sy1=S1y + K_arc*(Sx1-S1x);
			Sx2=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sy2=S1y + K_arc*(Sx2-S1x);
			}//��������;
			//�ж������������е��ĸ�
			if( ((r1x*X0+r1y*Y0)>=0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)<=0) || ((r1x*X1+r1y*Y1)<0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)>0) )
							{
							Sx=Sx2;
							Sy=Sy2;
							}
			else if( ((r1x*X0+r1y*Y0)>=0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)>0) || ((r1x*X0+r1y*Y0)<0 && ((Sx2-Sx1)*X0+(Sy2-Sy1)*Y0)<=0) )
							{
							Sx=Sx1;
							Sy=Sy1;
							}



			//����ת��
			/*cs1[i]= cs[j];



			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;//��һ��ֱ�ߵ��յ�

			cs2[i]= cs[j];//�±����Ӧ��
			i++;*/
			cs1[i]= cs[j];
			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;
			cs2[i]= cs[j];
			cs2[i].X= -r0x + Sx - Start_X2;
			cs2[i].Y= -r0y + Sy - Start_Y2;
			i++;
			//Բ������
			cs1[i]=cs0[0];
			cs2[i].G_pathmode = 2;
			cs2[i].X= r0x - r1x;
			cs2[i].Y= r0y - r1y;
			cs2[i].I=  r0x;
			cs2[i].J=  r0y;
			i++;
			
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
			Start_X2 = -r1x + Start_X;	
			Start_Y2 = -r1y + Start_Y;
		}//�����ͽ���

		else if( r0x == -(r1x) && r0y == -(r1y) && cs[j+1].G_pathmode==2)// ���ཻ��
		{
			printf("3.4\n");
			S1x= X0+r0x;	S1y=Y0+r0y; 
			S2x= X0+r1x;	S2y=Y0+r1y;
			//��������Բ��+ת������
			cs1[i]= cs[j];
			cs1[i].X = S1x - Start_X;
			cs1[i].Y = S1y - Start_Y;//��һ��ֱ�ߵ��յ�
			cs2[i]= cs[j];//�±����Ӧ��
			i++;

			cs1[i].X = S2x - S1x;
			cs1[i].Y = S2y - S1y;//����Բ���ߵ��յ�����
			cs1[i].I = -(r0x);
			cs1[i].J = -(r0y);//����Բ����Բ������
			if(cs[j+1].G_pathmode == 2) 
				cs1[i].G_pathmode = 3;
			else if(cs[j+1].G_pathmode == 3)
				cs1[i].G_pathmode = 2;
			cs2[i]= cs0[0];//�±����Ӧ��
			i++;

			Start_X = r1x;	
			Start_Y = r1y;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
		}//���ཻ�ͽ���
	}//ֱ�߽�Բ������



	else if((cs[j].G_pathmode==2 || cs[j].G_pathmode==3)&& (cs[j+1].G_pathmode==2 || cs[j+1].G_pathmode==3))//Բ����Բ��
	{
		printf("4\n");
		X0 = cs[j].X;
		Y0 = cs[j].Y;
		I_one=cs[j].I;
		J_one=cs[j].J;

		X1 = cs[j+1].X;
		Y1 = cs[j+1].Y;
		I_two=cs[j+1].I;
		J_two=cs[j+1].J;

		d0 = sqrt((X0-I_one)*(X0-I_one)+(Y0-J_one)*(Y0-J_one));	
		d1 = sqrt((X1-I_two)*(X1-I_two)+(Y1-J_two)*(Y1-J_two));

		if(cs[j].G_pathmode == 2)//Բ��˳ʱ��
		{
		    r0x = (-R)*(X0-I_one)/d0;
			r0y = (-R)*(Y0-J_one)/d0;
		}
		else if (cs[j].G_pathmode == 3)//Բ����ʱ��
		{
			r0x = (R)*(X0-I_one)/d0;
			r0y = (R)*(Y0-J_one)/d0;
		}


		if(cs[j+1].G_pathmode == 2)//Բ��˳ʱ��
		{
		    r1x = (-R)*(-I_two)/d1;
			r1y = (-R)*(-J_two)/d1;
		}
		else if (cs[j+1].G_pathmode == 3)//Բ����ʱ��
		{
			r1x = R*(-I_two)/d1;
			r1y = R*(-J_two)/d1;
		}
		//ȷ��KB1��KB2
		if(cs[j].G_pathmode == 2)
		{
			KBx0 = Y0-J_one;
			KBy0 = -(X0-I_one);
		}
		else if(cs[j].G_pathmode == 3)
		{
			KBx0 = -(Y0-J_one);
			KBy0 =  (X0-I_one);
		}

		if(cs[j+1].G_pathmode == 2)
		{
			KBx1 = -J_two;
			KBy1 =  I_two;
		}
		else if(cs[j+1].G_pathmode == 3)
		{
			KBx1 =  J_two;
			KBy1 = -I_two;
		}// ȷ��KB��������;
		if( (KBx0*(r1x-r0x)+KBy0*(r1y-r0y))>0  )//�쳤��
		{

			printf("4.1\n");
			if(KBx0 == 0 && KBx1 != 0)
			{
			S1x= X0+r0x;	S1y=Y0+r0y; 
			S2x= X0+r1x;	S2y=Y0+r1y; K2=KBy1/KBx1;
							
			Sx = S1x;
			Sy = K2*(Sx - S2x) + S2y;//ֱ��б�����޴�ʱ;					
			}
			else if(KBx0 != 0 && KBx1 == 0)
			{
			S1x= X0+r0x;	S1y=Y0+r0y;  K1=KBy0/KBx0; 
			S2x= X0+r1x;	S2y=Y0+r1y;
							
			Sx = S2x;
			Sy = K1*(Sx - S1x) + S1y;//ֱ��б�����޴�ʱ;					
			}
			else if((KBy0*KBx1) == (KBy1*KBx0))
			{
			S1x= X0+r0x;	S1y=Y0+r0y;  
			S2x= X0+r1x;	S2y=Y0+r1y;
							
			Sx = X0+r0x;
			Sy = Y0+r0y;
			
			}//ֱ��б�����ʱ;
			else
			{
			S1x= X0+r0x;	S1y=Y0+r0y;  K1=KBy0/KBx0; 
			S2x= X0+r1x;	S2y=Y0+r1y;  K2=KBy1/KBx1;
							
			Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
			Sy = K1*(Sx - S1x) + S1y;
			}//�쳤�ߵĽ�������;
			//ת������
			/*cs1[i]= cs[j];
			cs1[i].X = S1x - Start_X;
			cs1[i].Y = S1y - Start_Y;//��һ��Բ�����յ�

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//Բ����Բ������

			cs2[i]= cs[j];//�±����Ӧ��
			i++;
			cs1[i].X = Sx - S1x;
			cs1[i].Y = Sy - S1y;//Բ�����ߵ��쳤����
			cs1[i].G_pathmode = 1;

			cs2[i]= cs0[0];//�±����Ӧ��
			i++;
			
			cs1[i].X = S2x - Sx;
			cs1[i].Y = S2y - Sy;//�ڶ���Բ�����ߵ��쳤����
			cs1[i].G_pathmode = 1;

			cs2[i]= cs0[0];//�±����Ӧ��
			i++;*/

			cs1[i]= cs[j];
			cs1[i].X = S1x - Start_X;
			cs1[i].Y = S1y - Start_Y;//��һ��Բ�����յ�

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//Բ����Բ������

			cs2[i]= cs[j];//�±����Ӧ��
			cs2[i].X = cs[j].X - Start_X2;
			cs2[i].Y = cs[j].Y - Start_Y2;
			cs2[i].I= cs[j].I - Start_X2;
			cs2[i].J= cs[j].J - Start_Y2;
			i++;

		
			cs1[i].G_pathmode = 3;
			cs1[i].X = r1x-r0x;
			cs1[i].Y = r1y-r0y;
			cs1[i].I = -r0x;
			cs1[i].J = -r0y;
			cs2[i]= cs0[0];//�±����Ӧ��
			i++;


			Start_X = S2x - X0;	
			Start_Y = S2y - Y0;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
			Start_X2 = 0;	
			Start_Y2 = 0;
		}
			//�쳤�ͽ���
		else if  ( r0x == r1x && r0y == r1y )// ������
			{
			printf("4.4\n");
			Sx = X0+r0x;
			Sy = Y0+r0y;
			cs1[i]= cs[j];
            cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;//��һ��Բ�����յ�

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//Բ����Բ������

			cs2[i]= cs[j];//�±����Ӧ��
			i++;
			
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
			}//�����ͽ���;

		else if ( (KBx0*(r1x-r0x)+KBy0*(r1y-r0y))<0 || (r0x == -(r1x) && r0y == -(r1y)&&cs[j].G_pathmode == 3))// ������
		{
			printf("4.3\n");
			S1x=X0;S1y=Y0;//��Բ�ཻ���ϵĵ�
			

			Ox=I_one;	Oy=J_one;//Բ������;
			r_arc=sqrt( (X0+r0x-I_one)*(X0+r0x-I_one) + (Y0+r0y-J_one)*(Y0+r0y-J_one) );//Բ���뾶
			if(X0+I_two-Ox==0)
			{
				S2x=I_one;
				S2y=S1y;
			}
			else if(Y0+J_two-Oy==0)
			{
				S2x=S1x;
				S2y=J_one;
			}
			else
			{
				Kline=(Y0+J_two-Oy)/(X0+I_two-Ox);
				S2y=Kline*(X0-Ox+Oy/Kline+Kline*Y0)/(1+Kline*Kline);
				S2x=(S2y+Oy+Kline*Ox)/Kline;
			}

			if(S1x == S2x)
			{
			Sx1=S1x ;	Sy1=Oy + sqrt(r_arc*r_arc-(S2x-Ox)*(S2x-Ox));
			Sx2=S1x ;	Sy2=Oy - sqrt(r_arc*r_arc-(S2x-Ox)*(S2x-Ox));
			printf("%f,%f\n",Sx1,Sy1);
			}
			else if(S1y == S2y)
			{
			Sx1=Ox + sqrt(r_arc*r_arc-(S2y-Oy)*(S2y-Oy));	Sy1=S2y;
			Sx2=Ox - sqrt(r_arc*r_arc-(S2y-Oy)*(S2y-Oy));	Sy2=S2y;
			}
			else
			{
			K_arc=(S2y-S1y)/(S2x-S1x);
			a_arc=1+K_arc*K_arc;
			b_arc=2*K_arc*(S1y-Oy-K_arc*S1x-Ox/K_arc);
			c_arc=(S1y-Oy-K_arc*S1x)*(S1y-Oy-K_arc*S1x)-r_arc*r_arc+Ox*Ox;
							
			Sx1=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sy1=S1y + K_arc*(Sx1-S1x);
			Sx2=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sy2=S1y + K_arc*(Sx2-S1x);
			}//��������;



			//�ж������������е��ĸ�
			if( ((r0x*(S2x-S1x)+r1y*(S2y-S1y))>=0 && ((Sx2-Sx1)*(S2x-S1x)+(Sy2-Sy1)*(S2y-S1y))<=0) || ((r1x*(S2x-S1x)+r1y*(S2y-S1y))>0 && ((Sx2-Sx1)*(S2x-S1x)+(Sy2-Sy1)*(S2y-S1y))>0 ))
							{
							Sx=Sx1;
							Sy=Sy1;
							}
			else if( ((r0x*(S2x-S1x)+r1y*(S2y-S1y))>=0 && ((Sx2-Sx1)*(S2x-S1x)+(Sy2-Sy1)*(S2y-S1y))>0) || ((r1x*(S2x-S1x)+r1y*(S2y-S1y))>0 && ((Sx2-Sx1)*(S2x-S1x)+(Sy2-Sy1)*(S2y-S1y))<=0 ))
							{
							Sx=Sx2;
							Sy=Sy2;
							}



			//����ת��
			/*cs1[i]= cs[j];



			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;//��һ��Բ�����յ�

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//Բ����Բ������

			cs2[i]= cs[j];//�±����Ӧ��
			i++;*/

			cs1[i]= cs[j];
			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;
			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//Բ����Բ������
			cs2[i]= cs[j];
			cs2[i].X= -r0x + Sx  - Start_X2;
			cs2[i].Y= -r0y + Sy  - Start_Y2;
			cs2[i].I= cs[j].I - Start_X2;
			cs2[i].J= cs[j].J - Start_Y2;
			i++;
			//Բ������
			cs1[i]=cs0[0];
			cs2[i].G_pathmode=2;
			cs2[i].X= r0x - r1x;
			cs2[i].Y= r0y - r1y;
			cs2[i].I=  r0x;
			cs2[i].J=  r0y;
			i++;
			
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
			Start_X2 = -r1x + Start_X;	
			Start_Y2 = -r1y + Start_Y;
		}//�����ͽ���

		else if( r0x == -(r1x) && r0y == -(r1y)&& cs[j].G_pathmode == 2 )// ���ཻ��
		{
			printf("4.4\n");
			S1x= X0+r0x;	S1y=Y0+r0y; 
			S2x= X0+r1x;	S2y=Y0+r1y;
			//��������Բ��+ת������
			cs1[i]= cs[j];
			cs1[i].X = S1x - Start_X;
			cs1[i].Y = S1y - Start_Y;//��һ��Բ�����յ�

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//Բ����Բ������

			cs2[i]= cs[j];//�±����Ӧ��
			i++;
			cs1[i].X = S2x - S1x;
			cs1[i].Y = S2y - S1y;//����Բ���ߵ��յ�����
			cs1[i].I = -(r0x);
			cs1[i].J = -(r0y);//����Բ����Բ������
			if(cs[j].G_pathmode == 2) 
				cs1[i].G_pathmode = 3;
			else if(cs[j].G_pathmode == 3)
				cs1[i].G_pathmode = 2;

			cs2[i]= cs0[0];//�±����Ӧ��
			i++;

			Start_X = r1x;	
			Start_Y = r1y;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
		}//���ཻ�ͽ���
	}//Բ����Բ������			


}//��ʱj=N-1��


//��ʼ�˵����ǵ�����±�������


if(cs[j].G_pathmode == 1 && cs[j+1].G_pathmode ==1)//ֱ�߽�ֱ��
	{
		printf("1\n");
		X0 = cs[j].X;
		Y0 = cs[j].Y;
		X1 = cs[j+1].X; 
		Y1 = cs[j+1].Y;
		//��ʼ��ƫ��ʸ��(��ƫ)
		d0 = sqrt(X0*X0 +Y0*Y0);	
		d1 = sqrt(X1*X1 +Y1*Y1);
		r0x = R*Y0/d0;
		r0y = (-R)*X0/d0;
		r1x = 0;
		r1y = 0;
		//�жϽ�������
		if( ( (r1x-r0x)*X0 + (r1y-r0y)*Y0 )<0 )//������
		{
			//���ϱ����Ӧ�ĵ�
			if(X0 == 0)
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;
				S2x=X0 + r1x;S2y=Y0 + r1y;K2=Y1/X1;
				Sx =S1x;
				Sy =K2*(Sx - S2x) + S2y;
			}
			else if(X1 ==0)
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;K1=Y0/X0;
				S2x=X0 + r1x;S2y=Y0 + r1y;
				Sx =S2x;
				Sy =K1*(Sx - S1x) + S1y;
			}
			else
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;K1=Y0/X0;
				S2x=X0 + r1x;S2y=Y0 + r1y;K2=Y1/X1;
				Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
				Sy = K1*(Sx - S1x) + S1y;
			}
			//ת������
			cs1[i]= cs[j];
			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;
			cs2[i]= cs[j];
			cs2[i].X = cs[j].X - Start_X2;
			cs2[i].Y = cs[j].Y - Start_Y2;
			i++;
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;
		}
		else //�쳤��
		{
			//���ϱ����Ӧ�ĵ�
			if(X0 == 0)
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;
				S2x=X0 + r1x;S2y=Y0 + r1y;K2=Y1/X1;
				Sx =S1x;
				Sy =K2*(Sx - S2x) + S2y;
			}
			else if(X1 ==0)
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;K1=Y0/X0;
				S2x=X0 + r1x;S2y=Y0 + r1y;
				Sx =S2x;
				Sy =K1*(Sx - S1x) + S1y;
			}
			else
			{
				S1x=X0 + r0x;S1y=Y0 + r0y;K1=Y0/X0;
				S2x=X0 + r1x;S2y=Y0 + r1y;K2=Y1/X1;
				Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
				Sy = K1*(Sx - S1x) + S1y;
			}
			//ת������
			cs1[i]= cs[j];
			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;
			cs2[i]= cs[j];
			cs2[i].X = cs[j].X - Start_X2;
			cs2[i].Y = cs[j].Y - Start_Y2;
			i++;
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;
		}
	}//ֱ�߽�ֱ�߽���

else if(cs[j+1].G_pathmode == 1 && (cs[j].G_pathmode ==3 || cs[j].G_pathmode ==2))//Բ����ֱ��
{
	printf("2\n");

		X0 = cs[j].X;
		Y0 = cs[j].Y;
		I_one=cs[j].I;
		J_one=cs[j].J;

		X1 = cs[j+1].X;
		Y1 = cs[j+1].Y;

		d0 = sqrt((X0-I_one)*(X0-I_one)+(Y0-J_one)*(Y0-J_one));	
		d1 = sqrt(X1*X1 +Y1*Y1);
		printf("%f,%f\n",d0,d1);
		if(cs[j].G_pathmode == 2)//Բ��˳ʱ��
		{
		    r0x = (-R)*(X0-I_one)/d0;
			r0y = (-R)*(Y0-J_one)/d0;
			printf("%f,%f\n",r0x,r0y);
		}
		else if (cs[j].G_pathmode == 3)//Բ����ʱ��
		{
			r0x = (R)*(X0-I_one)/d0;
			r0y = (R)*(Y0-J_one)/d0;
		}

		r1x = 0;
		r1y = 0;
		
		//ȷ��KB����Բ���յ������б��
		if(cs[j].G_pathmode == 2)
		{
			KBx0 = Y0-J_one;
			KBy0 = -(X0-I_one);
		}
		else if(cs[j].G_pathmode == 3)
		{
			KBx0 = -(Y0-J_one);
			KBy0 =  (X0-I_one);
		}// ȷ��KB��������;
		//�жϽ������ͣ���յ��Ӧ�����꣬������
		if( (KBx0*(r1x-r0x)+KBy0*(r1y-r0y))>0 )//�쳤��
		{

			printf("2.1\n");
			if(KBx0 == 0 && X1 != 0)
			{
			S1x= X0+r0x;	S1y=Y0+r0y; 
			S2x= X0+r1x;	S2y=Y0+r1y; K2=Y1/X1;
							
			Sx = S1x;
			Sy = K2*(Sx - S2x) + S2y;//ֱ��б�����޴�ʱ;					
			}
			else if(KBx0 != 0 && X1 == 0)
			{
			S1x= X0+r0x;	S1y=Y0+r0y;  K1=KBy0/KBx0; 
			S2x= X0+r1x;	S2y=Y0+r1y;
							
			Sx = S2x;
			Sy = K1*(Sx - S1x) + S1y;//ֱ��б�����޴�ʱ;					
			}
			
			else
			{
			S1x= X0+r0x;	S1y=Y0+r0y;  K1=KBy0/KBx0; 
			S2x= X0+r1x;	S2y=Y0+r1y; K2=Y1/X1;
							
			Sx = (S2y - S1y - K2*S2x + K1*S1x)/(K1 - K2);
			Sy = K1*(Sx - S1x) + S1y;
			}//�쳤�ߵĽ�������;
			//ת������
			cs1[i]= cs[j];
			cs1[i].X = S1x - Start_X;
			cs1[i].Y = S1y - Start_Y;//��һ��Բ�����յ�

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//Բ����Բ������

			cs2[i]= cs[j];//�±����Ӧ��
			cs2[i].X = cs[j].X - Start_X2;
			cs2[i].Y = cs[j].Y - Start_Y2;
			
			i++;

			cs1[i].X = Sx - S1x;
			cs1[i].Y = Sy - S1y;//Բ�����ߵ��쳤����
			cs1[i].G_pathmode = 1;

			cs2[i]= cs0[0];//�±����Ӧ��,���һ����㣬ֻ��Ϊ�˺��ϱ���Ԫ�ظ������Զ�Ӧ��
			i++;

			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
		}
			//�쳤�ͽ���
		else if  ( r0x == r1x && r0y == r1y )// ������
			{
			printf("2.2\n");
			Sx = X0+r0x;
			Sy = Y0+r0y;
			cs1[i]= cs[j];
            cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;//��һ��Բ�����յ�

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//Բ����Բ������

			cs2[i]= cs[j];//�±����Ӧ��
			i++;
			
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
			}//�����ͽ���;
		else if ( (KBx0*(r1x-r0x)+KBy0*(r1y-r0y))<0 || (r0x == -(r1x) && r0y == -(r1y)&&cs[j].G_pathmode == 3))// ������
		{
			printf("2.3\n");
			S2x=X0+r1x;	S2y=Y0+r1y;
			S3x=X0+X1+r1x;	S3y=Y0+Y1+r1y;//��Բ���ཻ��ֱ�ߵ�������;
			printf("%f,%f,%f,%f\n",S2x,S2y,S3x,S3y);

			Ox=I_one;	Oy=J_one;//Բ������;
			r_arc=sqrt( (X0+r0x-I_one)*(X0+r0x-I_one) + (Y0+r0y-J_one)*(Y0+r0y-J_one) );//Բ���뾶
			printf("%f\n",r_arc);
			if(S2x == S3x)
			{
			Sx1=S2x ;	Sy1=Oy + sqrt(r_arc*r_arc-(S2x-Ox)*(S2x-Ox));
			Sx2=S2x ;	Sy2=Oy - sqrt(r_arc*r_arc-(S2x-Ox)*(S2x-Ox));
			}
			else if(S2y == S3y)
			{
			Sx1=Ox + sqrt(r_arc*r_arc-(S2y-Oy)*(S2y-Oy));	Sy1=S2y;
			Sx2=Ox - sqrt(r_arc*r_arc-(S2y-Oy)*(S2y-Oy));	Sy2=S2y;
			}
			else
			{
			K_arc=(S3y-S2y)/(S3x-S2x);
			a_arc=1+K_arc*K_arc;
			b_arc=2*K_arc*(S2y-Oy-K_arc*S2x-Ox/K_arc);
			c_arc=(S2y-Oy-K_arc*S2x)*(S2y-Oy-K_arc*S2x)-r_arc*r_arc+Ox*Ox;
							
			Sx1=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sy1=S2y + K_arc*(Sx1-S2x);
			Sx2=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sy2=S2y + K_arc*(Sx2-S2x);
			}//��������;
			//�ж������������е��ĸ�
			if( ((r0x*X1+r0y*Y1)>=0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)<=0) || ((r0x*X1+r0y*Y1)<0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)>0) )
							{
							Sx=Sx2;
							Sy=Sy2;
							}
			else if( ((r0x*X1+r0y*Y1)>=0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)>0) || ((r0x*X1+r0y*Y1)<0 && ((Sx2-Sx1)*X1+(Sy2-Sy1)*Y1)<=0) )
							{
							Sx=Sx1;
							Sy=Sy1;
							}



			//����ת��
			cs1[i]= cs[j];



			cs1[i].X = Sx - Start_X;
			cs1[i].Y = Sy - Start_Y;//��һ��Բ�����յ�

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//Բ����Բ������

			cs2[i]= cs[j];//�±����Ӧ��
			cs2[i].X= cs[j].X - Start_X2;
			cs2[i].Y= cs[j].Y - Start_Y2;
			cs2[i].I= cs[j].I - Start_X2;
			cs2[i].J= cs[j].J - Start_Y2;
			i++;
			
			Start_X = Sx - X0;	
			Start_Y = Sy - Y0;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
		}//�����ͽ���

		else if( r0x == -(r1x) && r0y == -(r1y) && cs[j].G_pathmode == 2 )// ���ཻ��
		{
			printf("2.4\n");
			S1x= X0+r0x;	S1y=Y0+r0y; 
			S2x= X0+r1x;	S2y=Y0+r1y;
			//��������Բ��+ת������
			cs1[i]= cs[j];
			cs1[i].X = S1x - Start_X;
			cs1[i].Y = S1y - Start_Y;//��һ��Բ�����յ�

			cs1[i].I = I_one - Start_X;
			cs1[i].J = J_one - Start_Y;//Բ����Բ������

			cs2[i]= cs[j];//�±����Ӧ��

			i++;
			cs1[i].X = S2x - S1x;
			cs1[i].Y = S2y - S1y;//����Բ���ߵ��յ�����
			cs1[i].I = -(r0x);
			cs1[i].J = -(r0y);//����Բ����Բ������
			if(cs[j].G_pathmode == 2) 
				cs1[i].G_pathmode = 3;
			else if(cs[j].G_pathmode == 3)
				cs1[i].G_pathmode = 2;

			cs2[i]= cs0[0];//�±����Ӧ��
			i++;

			Start_X = r1x;	
			Start_Y = r1y;//ת�ӵ�����ڵڶ���G���������������,Ϊ����һ���յ�����������׼��;
		}//���ཻ�ͽ���
	}//Բ����ֱ�߽���



//�����˵�����һ��

		cs1[i]= cs[j+1]; // ���һ�����˵�
		cs1[i].X = cs[j+1].X - Start_X;
		cs1[i].Y = cs[j+1].Y - Start_Y;

		cs2[i]= cs[j+1];//�±����Ӧ��  ���յ缫˿�䴹ֱ��


		
		
		
//��ʼ�켣�ܻ�

for(j=0;j<i+1;j++)
{
if (cs1[j].G_pathmode == 1 && cs2[j].G_pathmode == 1)//ֱ�߶�ֱ��
{
	printf("lineline\n");
	Xtop = 	cs1[j].X;
	Ytop = 	cs1[j].Y;
    Xbtm = 	cs2[j].X;
	Ybtm = 	cs2[j].Y;
	lineline(Xtop,Ytop,Xbtm,Ybtm,tmp1,tmp2,&top,&btm); //ֱ�߶�ֱ�ߵĺ����������ǽ�����������Ķ�Ӧ�߶��ܻ���
	// ��ʱ��tmp1��tmp2�д����ܻ������ݣ�top��btm Ϊ�ܻ��ĵ�����
	for(m=0;m<=(top>btm?top:btm);d++,m++)
	{
		m1[d].X=tmp1[m].X;
		m1[d].Y=tmp1[m].Y;
		m2[d].X=tmp2[m].X;
		m2[d].Y=tmp2[m].Y;
	}

}

else if((cs1[j].G_pathmode == 2 &&  cs2[j].G_pathmode == 2)|| (cs2[j].G_pathmode == 3&& cs2[j].G_pathmode == 3))//Բ����Բ��
{
	printf("arcarc\n");
	//��������Բ���������յ�����
	Xtopstart=-cs1[j].I;
	Ytopstart=-cs1[j].J;
	Xtopend=cs1[j].X-cs1[j].I;
	Ytopend=cs1[j].Y-cs1[j].J;
	Xbtmstart=-cs2[j].I;
	Ybtmstart=-cs2[j].J;
	Xbtmend=cs2[j].X-cs2[j].I;
	Ybtmend=cs2[j].Y-cs2[j].J;

	arcarc( Xtopstart,Ytopstart, Xtopend, Ytopend,
		    Xbtmstart,Ybtmstart, Xbtmend, Ybtmend,
			tmp1,tmp2,&top,&btm
		  );//���ú���
	
	for(m=0;m<=(top>btm?top:btm);d++,m++)//�ṹ�岻����ֵ�Ǿ�������
	{
		m1[d].X=tmp1[m].X;
		m1[d].Y=tmp1[m].Y;
		m2[d].X=tmp2[m].X;
		m2[d].Y=tmp2[m].Y;
	}
	
}
else if(cs1[j].G_pathmode == 1 && cs2[j].G_pathmode == 0)//ֱ�߶Ե�
{
	printf("linepoint\n");
	Xtop = 	cs1[j].X;
	Ytop = 	cs1[j].Y;
	line(Xtop,Ytop,0.02,tmp1,&top);
	for(m=0;m<=top;d++,m++)
	{
		m1[d].X=tmp1[m].X;
		m1[d].Y=tmp1[m].Y;
		m2[d].X=0;
		m2[d].Y=0;
	}
}
else if( (cs1[j].G_pathmode == 3)&& cs2[j].G_pathmode == 0)//Բ���Ե�
{
	printf("arcpoint\n"); 
	Xtopstart=-cs1[j].I;
	Ytopstart=-cs1[j].J;
	Xtopend=cs1[j].X-cs1[j].I;
	Ytopend=cs1[j].Y-cs1[j].J;
	Rtoptop = sqrt(Xtopstart*Xtopstart + Ytopstart*Ytopstart);
	deltl = sqrt(8*Rtoptop*0.001);
	arc1(Xtopstart,Ytopstart,Xtopend,Ytopend,Rtoptop,deltl,tmp1,&top);
	for(m=0;m<=top;d++,m++)
	{
		m1[d].X=tmp1[m].X;
		m1[d].Y=tmp1[m].Y;
		m2[d].X=0;
		m2[d].Y=0;
	}
}

else if(cs1[j].G_pathmode == 0 &&  cs2[j].G_pathmode == 2 )//���Բ��
{
	printf("pointarc\n");
	Xbtmstart=-cs2[j].I;
	Ybtmstart=-cs2[j].J;
	Xbtmend=cs2[j].X-cs2[j].I;
	Ybtmend=cs2[j].Y-cs2[j].J;
	Rbtmbtm = sqrt(Xbtmstart*Xbtmstart + Ybtmstart*Ybtmstart);
	deltl = sqrt(8*Rbtmbtm*0.0002);
	arc(Xbtmstart,Ybtmstart,Xbtmend,Ybtmend,Rbtmbtm,deltl,tmp2,&btm);
	for(m=0;m<=btm;d++,m++)
	{
		m2[d].X=tmp2[m].X;
		m2[d].Y=tmp2[m].Y;
		m1[d].X=0;
		m1[d].Y=0;
	}


}

}


//�ܻ���ϡ����ݴ��� m1[]��m2[]�С� 



struct touying
{
	double X,Y,U,V;
} ; 
struct touying ty[2000];
struct touying tydata[2000];
int k;
double h1=1,h2=1,l=3;//�ֱ�Ϊ�±������UV�ľ��롢�����ĸ߶ȡ�����˿����ľ��롣
double x1[2000],y1[2000],x2[2000],y2[2000];
x1[0]=0;
y1[0]=0;
x2[0]=0;
y2[0]=0;
tydata[0].X = (h1+h2)*m2[0].X / h2 - h1*m1[0].X / h2;
tydata[0].Y = (h1+h2)*m2[0].Y / h2 - h1*m1[0].Y / h2;
tydata[0].U = l*(m1[0].X - m2[0].X)/h2 + tydata[0].X;
tydata[0].V = l*(m1[0].Y - m2[0].Y)/h2 + tydata[0].Y;	

ty[0].X = (h1+h2)*m2[0].X / h2 - h1*m1[0].X / h2;
ty[0].Y = (h1+h2)*m2[0].Y / h2 - h1*m1[0].Y / h2;
ty[0].U = l*(m1[0].X - m2[0].X)/h2 + ty[0].X;
ty[0].V = l*(m1[0].Y - m2[0].Y)/h2 + ty[0].Y;	
	x1[1] = m1[0].X ;
	y1[1] = m1[0].Y ;
	x2[1] = m2[0].X ;
	y2[1] = m2[0].Y ;
for(k=1;k<d;k++)
{

	ty[k].X = (h1+h2)*(m2[k].X + x2[k])/ h2 - h1*(m1[k].X +	x1[k])/h2;
	ty[k].Y = (h1+h2)*(m2[k].Y + y2[k]) / h2 - h1*(m1[k].Y + y1[k])/h2;
	ty[k].U = l*((m1[k].X +	x1[k]) - (m2[k].X + x2[k]))/h2 + ty[k].X;
	ty[k].V = l*((m1[k].Y + y1[k]) - (m2[k].Y + y2[k]))/h2 + ty[k].Y;	

	tydata[k].X= ty[k].X - ty[k-1].X;
	tydata[k].Y= ty[k].Y - ty[k-1].Y;
	tydata[k].U= ty[k].U - ty[k-1].U;
	tydata[k].V= ty[k].V - ty[k-1].V;
	
	x1[k+1] = m1[k].X + x1[k];
	y1[k+1] = m1[k].Y +	y1[k];
	x2[k+1] = m2[k].X +	x2[k];
	y2[k+1] = m2[k].Y +	y2[k];
}

for(m=0;m<k;m++)
	{
		
		printf("%f,%f,%f,%f\n",tydata[m].X,tydata[m].Y,tydata[m].U,tydata[m].V);
	}
	printf("%d\n",k);
}



/*for(m=0;m<=i;m++)
{
	printf("%d,%f,%f,%f,%f\n",cs1[m].G_pathmode,cs1[m].X,cs1[m].Y,cs1[m].I,cs1[m].J);
}
	printf("\n");
for(m=0;m<=i;m++)
	printf("%d,%f,%f,%f,%f\n",cs2[m].G_pathmode,cs2[m].X,cs2[m].Y,cs2[m].I,cs2[m].J);

for(m=0;m<d;m++)
	{
		
		printf("%f,%f,%f,%f\n",m1[m].X,m1[m].Y,m2[m].X,m2[m].Y);
	}
	printf("%d\n",d);

}*/





